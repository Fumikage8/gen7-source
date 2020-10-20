//===========================================================================
/**
 * @file    gfl_Assert.cpp
 * @brief   アサート処理
 * @author  GAMEFREAK inc.
 * @date    2011.10.06
 */
//===========================================================================

//#include <gflib.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#if defined(GF_PLATFORM_WIN32)
#define NOMINMAX
#include <windows.h>
#include <string>
#include <ostream>
#include <sstream>
#include <fstream>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#include <time.h>
#endif // if defined(WIN32)

//#pragma warning(disable:4996)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(debug)

#if defined(GF_PLATFORM_WIN32)


/**
* @brief   スタックトレース情報取得クラス
* @note    Initialize()後にCapture()を呼ぶ、Capture()は何度呼んでも良い
*/
class StackTrace
{
public:

  static size_t const DEFAULT_STACK_SIZE = 32;  ///< 保持スタック情報最大数

  /**
  * @brief   コンストラクタ
  */
  StackTrace( void ) : m_StackSize( 0 ), m_IsReady( false ), m_hProcess( NULL )
  {
    for( u32 i = 0; i < DEFAULT_STACK_SIZE; i++ )
    {
      m_StackAdressTable[ i ] = NULL;
    }
  }

  /**
  * @brief   デストラクタ 
  */
  ~StackTrace( void )
  {
    Terminate();
  }

  /**
  * @brief   初期化処理
  *
  * @return  初期化に成功したらなtrue、失敗したならfalse
  */
  b32 Initialize( void )
  {
    // 初期化済み
    if( m_IsReady )
    {
      return true;
    }

    // プロセス用のシンボルハンドラを初期化し、シンボル情報を取得できるようにする
    HANDLE hProcess( NULL );
    hProcess = GetCurrentProcess();
    SymSetOptions( SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME );
    if( SymInitialize( hProcess, NULL, TRUE ) )
    {
      m_IsReady = true;
      m_hProcess = hProcess;
      return true;
    }
    else
    {
      return false;
    }
  }

  /**
  * @brief   終了処理
  *
  * @return  無し
  */
  void Terminate( void )
  {
    // @note 他でSymを使用していた場合、問答無用でCleanupしてしまう
    if( m_IsReady )
    {
      SymCleanup( m_hProcess );
    }

    for( u32 i = 0; i < DEFAULT_STACK_SIZE; i++ )
    {
      m_StackAdressTable[ i ] = NULL;
    }

    m_StackSize = 0;
    m_IsReady = false;
    m_hProcess = NULL;
  }

  /**
  * @brief   スタックトレース情報を取得し、保持する
  *
  * @return  無し
  */
  void Capture( void )
  {
    if( m_IsReady == false )
    {
      return;
    }

    // この関数の情報を削除したいが、最適化によりスタック情報が欠落することがあり
    // 本来欲しい情報を消してしまう可能性があるため無加工で保存する
    m_StackSize = CaptureImpl( &m_StackAdressTable[0], DEFAULT_STACK_SIZE );
  }

  /**
  * @brief   スタックトレース情報をログ出力する
  *
  * @return  無し
  */
  void Dump( void ) const
  {
    GFL_PRINT("---- Stack Trace Dump ---- \n");
    GFL_PRINT("adress: module: function: file: line\n");
    u32 stackSize( GetSize() );
    for( u32 i = 0; i < stackSize; i++ )
    {
      GFL_PRINT("%s \n", GetStackInfo( i ).c_str() );
    }
    GFL_PRINT("-------------------------- \n");
  }

  /**
  * @brief   スタックのデータ数を取得する
  *
  * @return  スタックのデータ数
  */
  u32 GetSize( void ) const
  {
    return m_StackSize;
  }

  /*
  * @brief   スタックの情報を文字列で取得する
  * @note    フォーマットは[adress: module: function: file: line]、シンボルから情報を取得できなかった項目は???になる
  *
  * @param   no データ番号( 0 〜 GetSize() - 1 )
  *
  * @return  スタックの情報文字列
  */
  std::string GetStackInfo( u32 no ) const
  {
    if( no >= GetSize() )
    {
      return std::string();
    }

    return GetSymbol( m_StackAdressTable[no] ).c_str();
  }

private:

  /**
  * @brief   スタックトレース情報を取得し、アドレスを配列に格納する
  *
  * @param   ppAdressTable  アドレスを格納する配列
  * @param   size           最大格納数           
  *
  * @return  実際に格納した数
  */
  u32 CaptureImpl( void **ppAdressTable, s32 size ) const
  {
    if( size >= 63 )
    {
      size = 62;
    }
    return RtlCaptureStackBackTrace( 0, size, ppAdressTable, 0 );
  }

  /**
  * @brief   アドレスからシンボル情報を取得する
  *
  * @param   pAddress 対象アドレス
  *
  * @return  シンボル情報を成型した文字列
  */
  std::string GetSymbol( void *pAddress ) const
  {
    if( pAddress == NULL || m_IsReady == false )
    {
      return std::string();
    }

#if defined(_WIN64)
    DWORD64  dwDisplacement = 0;
#else
    DWORD    dwDisplacement = 0;
#endif
    DWORD64  dwAddress = (DWORD64)pAddress;
    std::ostringstream ss;
    ss << "0x" << pAddress;

    // モジュール名
    IMAGEHLP_MODULE imageModule = { sizeof(IMAGEHLP_MODULE) };
    if( SymGetModuleInfo( m_hProcess , (DWORD)pAddress, &imageModule ) )
    {
      ss << ": " << imageModule.ModuleName;
    }
    else
    {
      ss << ": ???";
    }

    IMAGEHLP_SYMBOL* pImageSymbol;
    char buffer[ MAX_PATH + sizeof(IMAGEHLP_SYMBOL) ] = { 0 };
    pImageSymbol = (IMAGEHLP_SYMBOL*)buffer;
    pImageSymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
    pImageSymbol->MaxNameLength = MAX_PATH;

    if( SymGetSymFromAddr( m_hProcess, dwAddress, &dwDisplacement, pImageSymbol ) )
    {
      ss <<": " << pImageSymbol->Name;
    }
    else
    {
      ss << ": ???";
    }

    // 行番号
    DWORD  dwLineDisplacement = 0; // こちらはなぜか32
#if defined(_WIN64)
    IMAGEHLP_LINE line ={sizeof(IMAGEHLP_LINE)};
#else
    IMAGEHLP_LINE64 line ={sizeof(IMAGEHLP_LINE64)};
#endif
    if( SymGetLineFromAddr64( m_hProcess, dwAddress, &dwLineDisplacement, &line ) )
    {
      ss <<": " << line.FileName << ": " << line.LineNumber;
    }
    else
    {
      ss << ": ???";
    }

    return ss.str();
  }

  void*   m_StackAdressTable[ DEFAULT_STACK_SIZE ];     // スタックトレースで取得したアドレス保持するテーブル
  u32     m_StackSize;                                  // 上記テーブルの有効要素数
  b8      m_IsReady;                                    // trueなら初期化済み
  
  // windows関連
  HANDLE  m_hProcess;                                   // このアプリケーションのハンドル、シンボル情報の取得に使用する

};

#endif // #if defined(GF_PLATFORM_WIN32)

static GFL_ASSERT_LoopFunc				LoopFunc = NULL;

#if GFL_ENABLE_ASSERT
#if defined( WIN32 )
const char* const FILE_PATH_OUT_ASSERT_LOG("assert_log.txt");   ///< ログファイルの出力パス
static char debugmessagebuff[1024];                             ///< windowsの場合、ログファイルへの誘導文を追加するので拡張
#else
static char debugmessagebuff[512];
#endif // #if defined( WIN32 ) 
#endif
//------------------------------------------------------------------
/**
 * アサートループ時処理関数をユーザ設定する
 *
 * @param   loopFunc		ループ時に呼ばれるコールバック関数
 *
 */
//------------------------------------------------------------------
void GFL_ASSERT_SetLoopFunc( GFL_ASSERT_LoopFunc loopFunc )
{
	LoopFunc = loopFunc;
}

#if defined(GF_PLATFORM_WIN32)

/**
* @brief   アサート内容を外部出力する
*
* @param   filename    アサートが発生したファイル
* @param   line_n      アサートが発生した行
* @param   exp         アサート内容説明文
* @param   rStackTrace スタック情報
*
* @return  無し
*/
void GFL_ASSERT_OutPutLog( const char * filename, unsigned int line_n, const char *exp, const StackTrace& rStackTrace )
{
  std::ofstream ofs( FILE_PATH_OUT_ASSERT_LOG );

  if( filename )
  {
    ofs << "ASSERTION FAILED \nfilename: " << filename;
  }
  else
  {
    ofs << "ASSERTION FAILED \nfilename: ???";
  }

  ofs << "\nline: " << line_n;

  if( exp )
  {
    ofs << "\nexp: " << exp << "\n" << std::endl;
  }
  else
  {
    ofs << "\nexp: ???" << "\n" << std::endl;
  }

  ofs << "---- Stack Trace Dump ----" << std::endl;
  ofs << "adress: module: function: file: line" << std::endl;
  u32 stackSize( rStackTrace.GetSize() );
  for( u32 i = 0; i < stackSize; i++ )
  {
    ofs << rStackTrace.GetStackInfo( i ) << std::endl;
  }
  ofs << "--------------------------" << std::endl;
}
#endif // #if defined(GF_PLATFORM_WIN32)

#if GFL_ENABLE_ASSERT

//==============================================================================================
// ユーザ定義の表示関数群を管理するためのポインタ
//==============================================================================================
static GFL_ASSERT_DispInitFunc		DispInitFunc = NULL;
static GFL_ASSERT_DispMainFunc		DispMainFunc = NULL;
//static GFL_ASSERT_DispFinishFunc	DispFinishFunc = NULL;

static bool s_isAssertEnable = true;

static void InitAssertDisp( void );
static void FinishAssertDisp( const char* buff, b32 isStop );

//アサートを一時的に無効にする/再開する
//@attention 製品版で使っていない、大丈夫？→製品版でもASSERT_STOP自体は有効。逆に無効にされるとまずいかも。
void SetAssertEnable( bool isEnable )
{
  s_isAssertEnable = isEnable;
}
bool GetAssertEnable( void )
{
  return s_isAssertEnable;
}

//------------------------------------------------------------------
/**
 * アサート情報出力処理関数をユーザ設定する
 *
 * @param   initFunc		表示初期化時に呼ばれるコールバック関数
 * @param   mainFunc		文字出力ごとに呼ばれるコールバック関数
 * @param   finishFunc		表示終了時に呼ばれるコールバック関数
 *
 */
//------------------------------------------------------------------
void GFL_ASSERT_SetDisplayFunc( GFL_ASSERT_DispInitFunc initFunc, GFL_ASSERT_DispMainFunc mainFunc, GFL_ASSERT_DispFinishFunc finishFunc )
{
	DispInitFunc = initFunc;
	DispMainFunc = mainFunc;
//	DispFinishFunc = finishFunc;
}

//------------------------------------------------------------------
/**
 * アサート情報出力前処理
 *
 * @param   none		
 *
 */
//------------------------------------------------------------------
static void InitAssertDisp( void )
{
	if( DispInitFunc != NULL )
	{
		DispInitFunc();
	}
}

//------------------------------------------------------------------
/**
 * アサート情報出力終了処理
 */
//------------------------------------------------------------------
static void FinishAssertDisp( const char* buff, b32 isStop )
{
  if( LoopFunc )
  {
    LoopFunc( buff, isStop );
  }
}


void GFLassert( const char * filename, unsigned int line_no, b32 isStop, const char * exp )
{
  if( GetAssertEnable() == false )
  {
    return;
  }

	InitAssertDisp();
	GFL_PRINT("  **** ASSERTION FAILED ! **** \n");
	GFL_PRINT("%s(%d)\n", filename, line_no);
  GFL_PRINT("%s\n",exp);
  
  if(DispMainFunc){
    DispMainFunc( exp );
  }

  // windows環境ならスタック情報を出力する
  // @note 可能ならターゲットハードでも行う
#if defined( WIN32 )

  // スタック情報を出力
  StackTrace backTrace;
  if( backTrace.Initialize() )
  {
    backTrace.Capture();
    backTrace.Dump();
    GFL_ASSERT_OutPutLog( filename, line_no, exp, backTrace );
    backTrace.Terminate();
  }

  char filePathBuffer[ _MAX_PATH + 1 ];
  DWORD dwRet = GetFullPathNameA(
    (LPCSTR)FILE_PATH_OUT_ASSERT_LOG,
    sizeof(filePathBuffer)/sizeof(filePathBuffer[0]),
    (LPSTR)filePathBuffer,
    NULL
    );

  // 出力したスタック情報ファイルへの誘導文を追加する
  if( dwRet != 0 )
  {
    sprintf_s(debugmessagebuff,"ASSERTION FAILED !\n%s(%d)%s\n\nスタックトレース情報を出力しました\n%s",
      filename, (int)line_no, exp, filePathBuffer
      );
  }
  else
  {
    sprintf_s(debugmessagebuff,"ASSERTION FAILED !\n%s(%d)%s\n\nスタックトレース情報を出力しました\n%s",
      filename, (int)line_no, exp, FILE_PATH_OUT_ASSERT_LOG
      );
  }

#else
  sprintf(debugmessagebuff,"ASSERTION FAILED !\n%s(%d)%s",filename, (int)line_no, exp);
#endif // if defined( WIN32 )
  
  FinishAssertDisp( debugmessagebuff, isStop );
}

void GFLassertMsg( const char * filename, unsigned int line_no, b32 isStop, const char * fmt, ... )
{
  if( GetAssertEnable() == false )
  {
    return;
  }

	InitAssertDisp();
	GFL_PRINT("  **** ASSERTION FAILED ! **** \n");
	GFL_PRINT("%s(%d)\n", filename, line_no);
  {
    char buff[256];
    va_list vlist;
    va_start(vlist, fmt);
#if defined(GF_PLATFORM_WIN32)
    vsprintf_s(buff, fmt, vlist);
#elif defined(GF_PLATFORM_CTR)
    vsprintf(buff, fmt, vlist);
#endif
    va_end(vlist);
    GFL_PRINT("%s\n", buff);

    if(DispMainFunc){
      DispMainFunc( buff );
    }

    // windows環境ならスタック情報を出力する
    // @note 可能ならターゲットハードでも行う
#if defined( WIN32 )

    // スタック情報を出力
    StackTrace backTrace;
    if( backTrace.Initialize() )
    {
      backTrace.Capture();
      backTrace.Dump();
      GFL_ASSERT_OutPutLog( filename, line_no, buff, backTrace );
      backTrace.Terminate();
    }

    // 出力したスタック情報ファイルへの誘導文を追加する
    char filePathBuffer[ _MAX_PATH + 1 ];
    DWORD dwRet = GetFullPathNameA(
      (LPCSTR)FILE_PATH_OUT_ASSERT_LOG,
      sizeof( filePathBuffer ) / sizeof( filePathBuffer[0] ),
      (LPSTR)filePathBuffer,
      NULL
      );

    if( dwRet != 0 )
    {
      sprintf_s(debugmessagebuff,"ASSERTION FAILED !\n%s(%d)\n%s\n\nスタックトレース情報を出力しました\n%s",
        filename, (int)line_no, buff, filePathBuffer
        );
    }
    else
    {
      sprintf_s(debugmessagebuff,"ASSERTION FAILED !\n%s(%d)\n%s\n\nスタックトレース情報を出力しました\n%s",
        filename, (int)line_no, buff, FILE_PATH_OUT_ASSERT_LOG
        );
    }

#else
    sprintf(debugmessagebuff,"ASSERTION FAILED !\n%s(%d)%s",filename, (int)line_no, buff);
#endif // if defined( WIN32 )

    FinishAssertDisp( debugmessagebuff, isStop );
  }
}

// coverity[+kill]
void GFLassertStop( const char * filename, unsigned int line_no, const char * exp )
{
  GFLassert( filename, line_no, true, exp );
}

// coverity[+kill]
void GFLassertMsgStop( const char * filename, unsigned int line_no, const char * fmt, ... )
{
  //可変長引数をそのまま渡すことが出来ないようなので、ここで展開する。
  char buff[256];
  va_list vlist;
  va_start(vlist, fmt);
#if defined(GF_PLATFORM_WIN32)
    vsprintf_s(buff, fmt, vlist);
#elif defined(GF_PLATFORM_CTR)
    vsprintf(buff, fmt, vlist);
#endif
  va_end(vlist);

  // @attention 現状の挙動ならdefine部分で下記関数を呼ぶようにすれば良いが、kujira世代の方式を継承し分けている。
  GFLassertMsg( filename, line_no, true, buff );
}



#else //GFL_ENABLE_ASSERT

void GFLassert( const char * , unsigned int , b32, const char *  )
{
  //製品版では処理をしない
}

void GFLassertMsg( const char * , unsigned int , b32, const char * , ... )
{
  //製品版では処理をしない
}

// coverity[+kill]
void GFLassertStop( const char * , unsigned int , const char *  )
{
  //製品版は無限ループ
  
  while( 1 ) // @note 念のため、コールバック関数が設定されていない場合もループ
  {
    if( LoopFunc )
    {
      LoopFunc( "", true );
    }
	}
}

// coverity[+kill]
void GFLassertMsgStop( const char * , unsigned int , const char * , ... )
{
  //製品版は無限ループ

  while( 1 ) // @note 念のため、コールバック関数が設定されていない場合もループ
  {
    if( LoopFunc )
    {
      LoopFunc( "", true );
    }
	}
}

#endif //GFL_ENABLE_ASSERT

GFL_NAMESPACE_END(debug)
GFL_NAMESPACE_END(gfl2)


