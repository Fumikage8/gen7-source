//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_CommandListManager.cpp
 *	@brief  コマンドリストマネージャ
 *	@author	tomoya takahashi
 *	@date		2010.10.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include <grp/gfl_CommandListManager.h>

#include <debug/gfl_Assert.h>
#include <debug/gfl_DebugPrint.h>

namespace gfl {
namespace grp {



//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	コマンドリストクラス
//=====================================
class CommandList{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(CommandList);


public:
  // コンストラクタ・デストラクタ
  CommandList(void);
  ~CommandList();
  
  // 初期化
  void Initialize( const CommandListSetUp& setup );
  
  // 破棄
  void Finalize(void);

  // 初期化されているかチェック
  bool IsInitialized( void ) const;

  // バインド
  void Bind(void);

  // クリア
  void Clear(void);

  // 実行
  void RunAsync(void);

  // 実行完了待ち
  void WaitDone(void);

  // 実行をとめる
  void StopRun(void);

  // コマンドセーブ　開始
  void StartCommandSave( void );

  // コマンドセーブ　終了
  void EndCommandSave( u32* p_buffoffset = NULL, s32* p_buffsize = NULL, u32* p_reqid = NULL, s32* p_reqsize = NULL );

  // 保存情報を再利用
  void ReuseCommandSave( bool is_copy );
  void ReuseCommandSaveEx( bool is_copy, u32 buffoffset, s32 buffsize, u32 reqid, s32 reqsize );

  // バッファのアドレスを取得
  GLvoid* GetBufferAddr( void ) const;

  // 蓄積サイズを取得
  GLsizei GetBufferSize( void ) const;

  // バッファの使用量をダンプ
  void Dump( void  ) const;

  // コマンドリストIDの取得
  inline GLuint GetBufferID(void) const { return m_bufferID; }
  


#if GFL_DEBUG
  static const u32 TIMEOUT_TIME = 2;      //タイムアウト秒数
  static void TimeoutCallback(void);  //タイムアウトコールバッグ
#endif

private:
  
  GLuint    m_bufferID;     // バッファID
  bool      m_initialized;  // 初期化されているかフラグ
  bool      m_saveCommand;  // 保存コマンド保持フラグ
  GLint     m_bufferAddr;   // バッファの先頭アドレス

  GLuint    m_saveBufferOffset; // 保存開始オフセット
  GLsizei   m_saveBufferSize;   // 保存バイトサイズ
  GLuint    m_saveRequestId;    // コマンドリクエスト開始ID
  GLsizei   m_saveRequestSize;  // コマンドリクエスト保存数

} ;


#if GFL_COMMAND_LIST_DEBUG_USE
bool CommandListManager::m_debug_flag = false;          // コマンドリスト　デバッグ機能　ON・OFFフラグ
s64 CommandListManager::m_debug_commandwait_ms = 0;  // コマンド実行完了待ち時間
s64 CommandListManager::m_debug_commandDoneMs = 0;  // コマンド実行にかかった時間
s64 CommandListManager::m_debug_commandStartDoneMs = 0;// コマンド実行を開始した時間
void (*CommandListManager::m_debug_commandCallbackFunc)(void*) = NULL;  // コマンド完了コールバック関数ポインタ
void* CommandListManager::m_debug_commandCallbackWork = NULL;  // コマンド完了コールバックに渡すユーザーワーク
bool  CommandListManager::m_debug_commandCallbackFlag = false;  // コールバックが呼ばれることを待つ為のフラグ
s32  CommandListManager::m_debug_commandUseReqCount = 0; // 実行中のコマンド蓄積数
s32  CommandListManager::m_debug_commandUseBuffSize = 0;  // 実行中のコマンドバッファ蓄積サイズ
const char* CommandListManager::m_debug_gpuPrintBuf[2][DEBUG_GPU_PRINT_BUF_MAX] = {NULL};  // GPUデバッグ出力　文字列バッファ
s32  CommandListManager::m_debug_gpuPrintBuffIndex = 0;  //GPUデバッグ出力　ダブルバッファ用インデックス
s32  CommandListManager::m_debug_gpuPrintBuffCount = 0;  //GPUデバッグ出力　バッファ格納カウンタ
s32  CommandListManager::m_debug_gpuPrintBuffPrintCount = 0;  //GPUデバッグ出力　バッファ出力カウンタ
s32  CommandListManager::m_debug_gpuPrintBuffPrintCountMax = 0;  //GPUデバッグ出力　バッファ出力カウンタ最大値

static GLint  m_debug_LocalUseBufSize;
static GLint  m_debug_LocalUseReqCount;

#endif  // GFL_COMMAND_LIST_DEBUG_USE

//=============================================================================
/**
 *					コマンドリストマネージャ実装
*/
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ
 *
 *	@param  allocator   アロケータ
 *	@param  arrayMax    配列最大数
 */
//-----------------------------------------------------------------------------
CommandListManager::CommandListManager( heap::NwAllocator* allocator, u32 arrayMax )
{
  Initialize( allocator, arrayMax );
}

//----------------------------------------------------------------------------
/**
 *	@brief  デストラクタ
 */
//-----------------------------------------------------------------------------
CommandListManager::~CommandListManager()
{
  Finalize();
}


//----------------------------------------------------------------------------
/**
 *	@brief  初期化
 *
 *	@param	allocator   アロケータ
 *	@param	arrayMax    登録コマンドリスト最大数
 */
//-----------------------------------------------------------------------------
void CommandListManager::Initialize( heap::NwAllocator* allocator, u32 arrayMax )
{
  heap::HeapBase* heap; 
  
  GFL_ASSERT( m_array == NULL );
  

  heap = allocator->GetHeapBase();

  m_array     = GFL_NEW_ARRAY(heap) CommandList[arrayMax];
  m_arrayMax  = arrayMax; 

  m_saveCommandID   = COMMANDLIST_ID_ERROR;
  m_runCommandID    = COMMANDLIST_ID_ERROR;
  m_bindCommandID   = COMMANDLIST_ID_ERROR;

  m_runing = false;
  m_saving = false;
  
}

//----------------------------------------------------------------------------
/**
 *	@brief  破棄
 *
 *	@param	none
 */
//-----------------------------------------------------------------------------
void CommandListManager::Finalize(void)
{
  if( m_array != NULL ){
    // 実行中のコマンドを停止
    if( m_runing ){

      //実行の完了を待つ
      m_array[ m_runCommandID ].WaitDone();
      m_runing = false;
    }

    GFL_DELETE_ARRAY m_array;
    m_array = NULL;
    m_arrayMax = 0;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    コマンドリスト生成
 *
 *	@param	セットアップデータ
 *
 *	@retval コマンドリストID
 *	@retval COMMANDLIST_ID_ERROR  コマンドリストを生成できなかった。
 */
//-----------------------------------------------------------------------------
CommandListID CommandListManager::CreateBuffer( const CommandListSetUp& setup )
{
  CommandListID id;

  id = GetClearBuffer();
  if( id == COMMANDLIST_ID_ERROR ){
    return id;
  }

  m_array[id].Initialize( setup );

  return id;
}

//----------------------------------------------------------------------------
/**
 *	@brief    コマンドリスト破棄
 *
 *	@param	id  コマンドリストID
 */
//-----------------------------------------------------------------------------
void CommandListManager::DeleteBuffer( CommandListID id )
{
  if( !( id < m_arrayMax ) ){
    GFL_ASSERT(0);
    return ;
  }

  if( m_array[ id ].IsInitialized() ){
    
    // 実行を停止
    if( m_runing && (m_runCommandID == id) ){
      //GFL_PRINT( "runStop\n" );
      m_array[ id ].WaitDone();
      m_runing = false;
    }

    m_array[id].Finalize();
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  コマンドを格納していくCommandListを指定
 *
 *	@param	id  コマンドリストID
 */
//-----------------------------------------------------------------------------
void CommandListManager::Bind( CommandListID id )
{
  if( !( id < m_arrayMax ) ){
    GFL_ASSERT(0);
    return ;
  }

  m_array[id].Bind();
  // バインドIDを保存
  m_bindCommandID = id;
}

//----------------------------------------------------------------------------
/**
 *	@brief  コマンドの内容をクリアする
 *
 *	@param	id  コマンドリストID
 */
//-----------------------------------------------------------------------------
void CommandListManager::Clear( CommandListID id )
{
  if( !( id < m_arrayMax ) ){
    GFL_ASSERT(0);
    return ;
  }

  m_array[id].Clear();

  // bindCommandIDを再バインド
  BindCurrentCommandList();
}

//----------------------------------------------------------------------------
/**
 *	@brief  実行するCommandListを指定
 *
 *	@param	id  コマンドリストID
 */
//-----------------------------------------------------------------------------
void CommandListManager::RunAsync( CommandListID id )
{
  if( !( id < m_arrayMax ) ){
    GFL_ASSERT(0);
    return ;
  }

  // 完了しているかチェック
  if( m_runing ) {
    WaitDone();
  }

#if GFL_COMMAND_LIST_DEBUG_USE
  if( m_debug_flag )
  {
    m_array[id].Bind();
    // デバッグコールバックON状態ならば、GPU処理時間を計測
    m_debug_commandStartDoneMs = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMicroSeconds();
    
    GLint size;
    GLint num;
    nngxGetCmdlistParameteri( NN_GX_CMDLIST_USED_REQCOUNT, &num );
    nngxGetCmdlistParameteri( NN_GX_CMDLIST_USED_BUFSIZE, &size );
    m_debug_commandUseReqCount  = num;
    m_debug_commandUseBuffSize  = size;
    
    //GFL_PRINT( "run command id %d size 0x%x num %d\n", id, size, num );
    //GFL_PRINT( "tick %d\n", nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMicroSeconds() );


    // コールバック待ち状態にする。
    m_debug_commandCallbackFlag = true;

    // GPUデバッグ出力　ダブルバッファ制御
    m_debug_gpuPrintBuffIndex         = (m_debug_gpuPrintBuffIndex + 1) % 2;
    m_debug_gpuPrintBuffPrintCount    = 0;
    m_debug_gpuPrintBuffPrintCountMax = m_debug_gpuPrintBuffCount;
    m_debug_gpuPrintBuffCount         = 0;

    nngxSetCmdlistCallback( DebugCommandEndCallback );
    nngxEnableCmdlistCallback( -1 );
  }else{
    m_array[id].Bind();

    nngxSetCmdlistCallback( NULL );
    nngxDisableCmdlistCallback( -1 );

  }

#if GFL_DEBUG

  // WaitDone のタイムアウト設定
  nngxSetTimeout( nn::os::Tick(nn::fnd::TimeSpan::FromSeconds(CommandList::TIMEOUT_TIME)) , CommandList::TimeoutCallback );
#endif // GFL_DEBUG

#endif // GFL_COMMAND_LIST_DEBUG_USE
  
  m_array[id].RunAsync();
  m_runCommandID = id;
  m_runing = true;



  // bindCommandIDを再バインド
  BindCurrentCommandList();
}

//----------------------------------------------------------------------------
/**
 *	@brief  実行が完了するまで待つ
 */
//-----------------------------------------------------------------------------
void CommandListManager::WaitDone( void )
{
  if( m_runing == false ){
    return ;
  }

#if GFL_COMMAND_LIST_DEBUG_USE
  if( m_debug_flag ){
    nn::os::Tick starttick;
    nn::os::Tick endtick;

    starttick = nn::os::Tick::GetSystemCurrent();
    
    m_array[m_runCommandID].WaitDone(); // 
    
    endtick = nn::os::Tick::GetSystemCurrent();
    m_debug_commandwait_ms = endtick.ToTimeSpan().GetMicroSeconds() - starttick.ToTimeSpan().GetMicroSeconds();


    // まだコールバックが呼ばれていなければ、自分で呼んでしまう。
    if( m_debug_commandCallbackFlag == true ){
      DebugCommandEndCallback(-1);
    }


  }
  else
  {
    m_array[m_runCommandID].WaitDone(); // 
  }
  
#else
  
  m_array[m_runCommandID].WaitDone(); // 
#endif

  m_runing = false;

  // bindCommandIDを再バインド
  BindCurrentCommandList();
}

//----------------------------------------------------------------------------
/**
 *	@brief  コマンド実行中かチェック
 *	@retval true    コマンド実行中
 *	@retval false   コマンド実行していない
 */
//-----------------------------------------------------------------------------
bool CommandListManager::IsRuning( void ) const
{
  return m_runing;
}

//----------------------------------------------------------------------------
/**
 *	@brief    実行しているコマンドIDを取得
 *
 *	@return 実行しているコマンドID
 */
//-----------------------------------------------------------------------------
CommandListID CommandListManager::GetRuningCommandID(void) const
{
  if( m_runing ){
    return m_runCommandID;
  }
  return COMMANDLIST_ID_ERROR;
}


//----------------------------------------------------------------------------
/**
 *	@brief    実行したコマンドIDを取得
 *
 *	@return 実行したコマンドID
 */
//-----------------------------------------------------------------------------
CommandListID CommandListManager::GetRanCommandID(void) const
{
  if( !m_runing ){
    return m_runCommandID;
  }
  return COMMANDLIST_ID_ERROR;
}

//----------------------------------------------------------------------------
/**
 *	@brief  コマンドリストへのコマンドセーブ　開始
 *
 *	@param	コマンドをセーブするコマンドリストID
 */
//-----------------------------------------------------------------------------
void CommandListManager::StartCommandSave( CommandListID id )
{
  if( !( id < m_arrayMax ) ){
    GFL_ASSERT(0);
    return ;
  }
  GFL_ASSERT( m_saving == false );
  if( m_saving ){
    // 今のを終わらせる。
    // ここにきた場合の動作は保証できない。
    EndCommandSave();
  }

  m_array[id].StartCommandSave();
  m_saveCommandID = id;
  m_saving = true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  コマンドリストへのコマンドセーブ　終了
 */
//-----------------------------------------------------------------------------
void CommandListManager::EndCommandSave( void )
{
  if( m_saving == false ){
    return ;
  }

  m_array[m_saveCommandID].EndCommandSave();
  m_saving = false;

  // bindCommandIDを再バインド
  BindCurrentCommandList();
}



//----------------------------------------------------------------------------
/**
 *	@brief  コマンドリストへのコマンドセーブ　終了  セーブ情報取得バージョン
 *	*この関数内で、Save前にバインドされていたコマンドリストを再バインドします。
 *
 *	@param  p_buffoffset  セーブ開始バッファオフセット
 *	@param  p_buffsize    セーブバッファサイズ
 *	@param  p_reqid       セーブ開始リクエストID
 *	@param  p_reqsize     セーブリクエストサイズ
 */
//-----------------------------------------------------------------------------
void CommandListManager::EndCommandSaveEx( u32* p_buffoffset, s32* p_buffsize, u32* p_reqid, s32* p_reqsize )
{
  if( m_saving == false ){
    return ;
  }

  m_array[m_saveCommandID].EndCommandSave( p_buffoffset, p_buffsize, p_reqid, p_reqsize );
  m_saving = false;

  // bindCommandIDを再バインド
  BindCurrentCommandList();
}

//----------------------------------------------------------------------------
/**
 *	@brief  保存したコマンドを今バインドされているコマンドリストに追加する。
 *
 *	@param	save_id    保存コマンドリストID
 *	@param  is_copy    コマンドリストをコピーするのか、それともリンクをはるのか
 */
//-----------------------------------------------------------------------------
void CommandListManager::ReuseCommandSave( CommandListID save_id, bool is_copy )
{
  if( !( save_id < m_arrayMax ) ){
    GFL_ASSERT(0);
    return ;
  }
  m_array[ save_id ].ReuseCommandSave(is_copy);
}

//----------------------------------------------------------------------------
/**
 *	@brief  保存したコマンドを今バインドされているコマンドリストに追加する。
 *
 *	@param	save_id    保存コマンドリストID
 *	@param  is_copy    コマンドリストをコピーするのか、それともリンクをはるのか
 *	*コピーすると、save_id側のコマンドをクリアすることが出来ますが、
 *	　コピーにCPUの負荷がかかります。
 *
 *	@param  buffoffset  バッファオフセット
 *	@param  buffsize    バッファサイズ
 *	@param  reqid       リクエストID
 *	@param  reqsize     リクエストサイズ
 */
//-----------------------------------------------------------------------------
void CommandListManager::ReuseCommandSaveEx( CommandListID save_id, bool is_copy, u32 buffoffset, s32 buffsize, u32 reqid, s32 reqsize )
{
  if( !( save_id < m_arrayMax ) ){
    GFL_ASSERT(0);
    return ;
  }
  m_array[ save_id ].ReuseCommandSaveEx(is_copy, buffoffset, buffsize, reqid, reqsize);
}

//----------------------------------------------------------------------------
/**
 *	@brief  セーブ中かチェック
 *
 *	@retval true    セーブ中
 *	@retval false   していない
 */
//-----------------------------------------------------------------------------
bool CommandListManager::IsSaving( void ) const
{
  return m_saving;
}

//----------------------------------------------------------------------------
/**
 *	@brief    セーブしたコマンドではなく、コマンドリストの内容を全て、カレントのコマンドの後ろに蓄積させる。
 *
 *	@param	add_id    蓄積するコマンドリストID
 *	@param  is_copy    コマンドリストをコピーするのか、それともリンクをはるのか
 *	*コピーすると、save_id側のコマンドをクリアすることが出来ますが、
 *	　コピーにCPUの負荷がかかります。
 */
//-----------------------------------------------------------------------------
void CommandListManager::AddCommand( CommandListID add_id, bool is_copy )
{
  GLint use_bufsize;
  GLint use_reqcount;
  
  if( !( add_id < m_arrayMax ) ){
    GFL_ASSERT(0);
    return ;
  }

  // パラメータを取得するために、add_idのコマンドリストをバインド
  m_array[ add_id ].Bind();

  nngxGetCmdlistParameteri( NN_GX_CMDLIST_USED_BUFSIZE, &use_bufsize );
  nngxGetCmdlistParameteri( NN_GX_CMDLIST_USED_REQCOUNT, &use_reqcount );

  // カレントのコマンドリストをバインド
  BindCurrentCommandList();

#if GFL_COMMAND_LIST_DEBUG_USE
  {
    // デバッグ用に、カレントのバッファがどのくらい余っているか取っとく。
    nngxGetCmdlistParameteri( NN_GX_CMDLIST_USED_BUFSIZE, &m_debug_LocalUseBufSize );
    nngxGetCmdlistParameteri( NN_GX_CMDLIST_USED_REQCOUNT, &m_debug_LocalUseReqCount );
  }
#endif
 
  // リンク情報を追加
  nngxUseSavedCmdlist( m_array[ add_id ].GetBufferID(), 
      0, 
      use_bufsize, 
      0, 
      use_reqcount, 
      0, 
      is_copy ? GL_TRUE : GL_FALSE );
  
  GFL_GRPGL_ERROR_CHECK();
}

//----------------------------------------------------------------------------
/**
 *	@briefバッファの状態を取得
 *
 *	@param	p_usebuff   バッファの使用量
 *	@param	p_usereq    リクエストカウントの使用量
 *	@param	p_restbuff  バッファ残りサイズ
 *	@param	p_restreq   リクエストカウントバッファ残り数
 */
//-----------------------------------------------------------------------------
void CommandListManager::GetBufferStatus( CommandListID id, u32* p_usebuff, u32* p_usereq, u32* p_restbuff, u32* p_restreq )
{
  GLint use_bufsize;
  GLint use_reqcount;
  GLint max_bufsize;
  GLint max_reqcount;
  
  if( !( id < m_arrayMax ) ){
    GFL_ASSERT(0);
    return ;
  }


  // パラメータを取得するために、add_idのコマンドリストをバインド
  m_array[ id ].Bind();

  nngxGetCmdlistParameteri( NN_GX_CMDLIST_USED_BUFSIZE, &use_bufsize );
  nngxGetCmdlistParameteri( NN_GX_CMDLIST_USED_REQCOUNT, &use_reqcount );
  nngxGetCmdlistParameteri( NN_GX_CMDLIST_MAX_BUFSIZE, &max_bufsize );
  nngxGetCmdlistParameteri( NN_GX_CMDLIST_MAX_REQCOUNT, &max_reqcount );

  if( p_usebuff ){
    *p_usebuff = static_cast<u32>(use_bufsize);
  }
  if( p_usereq ){
    *p_usereq = static_cast<u32>(use_reqcount);
  }
  if( p_restbuff ){
    *p_restbuff = static_cast<u32>(max_bufsize - use_bufsize);
  }
  if( p_restreq ){
    *p_restreq = static_cast<u32>(max_reqcount - use_reqcount);
  }

  // カレントのコマンドリストをバインド
  BindCurrentCommandList();
}


//----------------------------------------------------------------------------
/**
 *	@brief    バインドされているバッファの状態をログに出力
 */
//-----------------------------------------------------------------------------
void CommandListManager::Dump( void ) const
{
  m_array[ m_bindCommandID ].Dump();
}

#if GFL_COMMAND_LIST_DEBUG_USE
//----------------------------------------------------------------------------
/**
 *	@brief  デバッグ用　デバッグ機能のON・OFF
 *
 *	@param	flag  true:ON   false:OFF
 */
//-----------------------------------------------------------------------------
void CommandListManager::DebugSetFlag( bool flag )
{
  m_debug_flag = flag;
  if( m_debug_flag ){
  }else{
    m_debug_commandCallbackFlag = false;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  デバッグ用　GPU側の処理完了コールバック　ON・OFF
 *
 *	@param	func  関数ポインタ
 *	@param  work  ユーザーワーク
 */
//-----------------------------------------------------------------------------
void CommandListManager::DebugSetCommandEndCallback(void(*func)( void* ), void* work )
{
  m_debug_commandCallbackFunc = func;
  m_debug_commandCallbackWork = work;
}

//----------------------------------------------------------------------------
/**
 *	@brief  デバッグ用　１つ前のコマンド実行にかかった処理時間
 *
 *  @retval マイクロ秒
 */
//-----------------------------------------------------------------------------
s32 CommandListManager::DebugGetDoneMs(void)
{
  return static_cast<s32>(m_debug_commandDoneMs);
}

//----------------------------------------------------------------------------
/**
 *	@brief  デバッグ用　１つ前のコマンド実行でGPUの処理待ちをした時間
 *
 *  @retval WaitDone処理をまったマイクロ秒
 */
//-----------------------------------------------------------------------------
s32 CommandListManager::DebugGetWaitDoneMs(void)
{
  return static_cast<s32>(m_debug_commandwait_ms);
}

//----------------------------------------------------------------------------
/**
 *	@brief  デバッグ用　実行コマンドリストに蓄積されていコマンド数
 *
 *  @retval コマンド数
 */
//-----------------------------------------------------------------------------
s32 CommandListManager::DebugGetUseReqCount(void)
{
  return m_debug_commandUseReqCount;
}

//----------------------------------------------------------------------------
/**
 *	@brief  デバッグ用　実行コマンドリストに蓄積されているコマンド総バイトサイズ
 *
 *  @retval WaitDone処理をまったマイクロ秒
 */
//-----------------------------------------------------------------------------
s32 CommandListManager::DebugGetUseBuffSize(void)
{
  return m_debug_commandUseBuffSize;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ここまでに格納されているコマンドが実行されたら表示されるデバッグプリント
 *	　　　　を登録
 *
 *	@param	str   文字列
 */
//-----------------------------------------------------------------------------
void CommandListManager::DebugPutGpuDebugPrint( const char* str )
{

  if( m_debug_flag )
  {
    GLint requestCount = 0;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_USED_REQCOUNT, &requestCount);
    if (requestCount == 0)
    {
      // 0にコールバックは設定しない。
      //nngxFlush3DCommand();
      nngxGetCmdlistParameteri(NN_GX_CMDLIST_USED_REQCOUNT, &requestCount);
    }
    nngxEnableCmdlistCallback( requestCount );
    
    m_debug_gpuPrintBuf[ m_debug_gpuPrintBuffIndex ][ m_debug_gpuPrintBuffCount ] = str;
    m_debug_gpuPrintBuffCount ++;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  コールバック関数
 */
//-----------------------------------------------------------------------------
void CommandListManager::DebugCommandEndCallback(GLint id)
{
  GFL_UNUSED(id);

  // デバッグ出力表示
  if( m_debug_gpuPrintBuffPrintCountMax > m_debug_gpuPrintBuffPrintCount ){
    GFL_PRINT( m_debug_gpuPrintBuf[ (m_debug_gpuPrintBuffIndex + 1) % 2 ][ m_debug_gpuPrintBuffPrintCount ] );
    m_debug_gpuPrintBuffPrintCount++;
    nngxDisableCmdlistCallback( id );
    return ; 
  }
  

  // プリントが終了した後の1回目が本物
  if(m_debug_commandCallbackFlag){
    
    // ユーザー定義コールバック
    if( m_debug_commandCallbackFunc ){
      m_debug_commandCallbackFunc( m_debug_commandCallbackWork );
    }

    //GPU実行時間計測
    nn::os::Tick endtick = nn::os::Tick::GetSystemCurrent();

    //GFL_PRINT( "end command endid %d", id );
    //GFL_PRINT( " tick %d\n", endtick.ToTimeSpan().GetMicroSeconds() );

    m_debug_commandDoneMs = (endtick.ToTimeSpan().GetMicroSeconds() - m_debug_commandStartDoneMs);


    // コールバックがコールされた。
    m_debug_commandCallbackFlag = false;
  }
}
#endif  // GFL_COMMAND_LIST_DEBUG_USE


//----------------------------------------------------------------------------
/**
 *	@brief  空いているワークを取得
 *
   *	@retval コマンドリストID
   *	@retval COMMANDLIST_ID_ERROR  コマンドリストを生成できなかった。
 */
//-----------------------------------------------------------------------------
CommandListID CommandListManager::GetClearBuffer(void) const
{
  CommandListID ret = COMMANDLIST_ID_ERROR;
  
  for( int i=0; i<m_arrayMax; i++ ){
    if( m_array[i].IsInitialized() == false ){
      ret = i;
      break;
    }
  }

  GFL_ASSERT( ret != COMMANDLIST_ID_ERROR ); //@check
  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  カレントのコマンドリストをバインドする
 */
//-----------------------------------------------------------------------------
void CommandListManager::BindCurrentCommandList( void )
{
  if(m_bindCommandID != COMMANDLIST_ID_ERROR){
    // bindCommandIDを再バインド
    m_array[m_bindCommandID].Bind();
  }
}



//=============================================================================
/**
 *					コマンドリスト実装
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ
 */
//-----------------------------------------------------------------------------
CommandList::CommandList(void)
{
}

//----------------------------------------------------------------------------
/**
 *	@brief  デストラクタ
 */
//-----------------------------------------------------------------------------
CommandList::~CommandList()
{
  Finalize();
}

//----------------------------------------------------------------------------
/**
 *	@brief  初期化
 *
 *	@param	setup   セットアップデータ
 */
//-----------------------------------------------------------------------------
void CommandList::Initialize( const CommandListSetUp& setup )
{
  GFL_ASSERT( m_initialized == false );
  
  // コマンドリストバッファを確保
  nngxGenCmdlists(1, &m_bufferID);
  nngxBindCmdlist(m_bufferID);
  nngxCmdlistStorage( setup.bufferSize, setup.queueingCount );
  nngxSetCmdlistParameteri(NN_GX_CMDLIST_RUN_MODE, NN_GX_CMDLIST_SERIAL_RUN);
  GFL_GRPGL_ERROR_CHECK();
  nngxClearCmdlist();

  nngxGetCmdlistParameteri( NN_GX_CMDLIST_TOP_BUFADDR, &m_bufferAddr );

  m_saveCommand     = false;
  m_initialized     = true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  破棄
 */
//-----------------------------------------------------------------------------
void CommandList::Finalize(void)
{
  if( m_initialized ){

#ifdef GFL_COMMAND_LIST_DEBUG_USE
    // コールバックをはずす
    nngxBindCmdlist(m_bufferID);
    nngxSetCmdlistCallback( NULL );
    nngxDisableCmdlistCallback( -1 );
#endif
    nngxBindCmdlist(0);
    nngxDeleteCmdlists( 1, &m_bufferID );
    GFL_GRPGL_ERROR_CHECK();
    m_initialized = false;
  }
  
}

//----------------------------------------------------------------------------
/**
 *	@brief  初期化されているかチェック
 *	
 *	@retval true    初期化されている
 *	@retval false   初期化されていない
 */
//-----------------------------------------------------------------------------
bool CommandList::IsInitialized( void ) const
{
  return m_initialized;
}

//----------------------------------------------------------------------------
/**
 *	@brief  自分をカレントにバインドする
 */
//-----------------------------------------------------------------------------
void CommandList::Bind(void)
{
  GFL_ASSERT( m_initialized );
  nngxBindCmdlist(m_bufferID);
  GFL_GRPGL_ERROR_CHECK();
}

//----------------------------------------------------------------------------
/**
 *	@brief  コマンドリストのクリア
 */
//-----------------------------------------------------------------------------
void CommandList::Clear(void)
{
  GFL_ASSERT( m_initialized );
  nngxBindCmdlist(m_bufferID);
  nngxClearCmdlist();
  
  {//@fix 友也さん承認：未初期化レジスタの初期化
    static const u32 s_TagCommands[] ={ 
      0x00000000, 0x000F0065,//シザーテスト無効　H3DModelがちらつきます
      0x00000000, 0x000F0084,//Texture Mip Map 無効　ハングにつながります
      0x00000000, 0x000F0094,//Texture Mip Map 無効　ハングにつながります
      0x00000000, 0x000F009C//Texture Mip Map 無効　ハングにつながります
    };
    nngxAdd3DCommand( s_TagCommands, sizeof(s_TagCommands), GL_TRUE);
  }
  
  GFL_GRPGL_ERROR_CHECK();
}

//----------------------------------------------------------------------------
/**
 *	@brief  コマンドを実行させる。
 */
//-----------------------------------------------------------------------------
void CommandList::RunAsync(void)
{
  GFL_ASSERT( m_initialized );

  
  nngxBindCmdlist(m_bufferID);


  // サイズチェック
#if GFL_DEBUG
  {
    GLint param;

    nngxGetCmdlistParameteri( NN_GX_CMDLIST_USED_BUFSIZE, &param );
    // 16の倍数である必要がある。
    GFL_ASSERT_MSG( (param % 16) == 0, "cmdlist size err %d\n", param );
    
  }
#endif

  
  nngxRunCmdlist();

  GFL_GRPGL_ERROR_CHECK();
}

//----------------------------------------------------------------------------
/**
 *	@brief  実行の完了待ちをおこなう。
 */
//-----------------------------------------------------------------------------
void CommandList::WaitDone(void)
{

  GFL_ASSERT( m_initialized );
  nngxBindCmdlist(m_bufferID);
  //nngxFlush3DCommand();
  nngxWaitCmdlistDone();
  nngxStopCmdlist();
  GFL_GRPGL_ERROR_CHECK();

}

//----------------------------------------------------------------------------
/**
 *	@brief  実行をとめる
 */
//-----------------------------------------------------------------------------
void CommandList::StopRun(void)
{
  GFL_ASSERT( m_initialized );
  nngxBindCmdlist(m_bufferID);
  nngxStopCmdlist();
  nngxWaitCmdlistDone();
  GFL_GRPGL_ERROR_CHECK();
}

//----------------------------------------------------------------------------
/**
 *	@brief  再利用コマンドの保存を開始する。
 */
//-----------------------------------------------------------------------------
void CommandList::StartCommandSave( void )
{
  GFL_ASSERT( m_initialized );
  nngxBindCmdlist(m_bufferID);
  nngxStartCmdlistSave();
  GFL_GRPGL_ERROR_CHECK();
}

//----------------------------------------------------------------------------
/**
 *	@brief  再利用コマンドの保存を終了する。
 */
//-----------------------------------------------------------------------------
void CommandList::EndCommandSave( u32* p_buffoffset, s32* p_buffsize, u32* p_reqid, s32* p_reqsize )
{
  GFL_ASSERT( m_initialized );

  nngxSplitDrawCmdlist();
  
  nngxStopCmdlistSave(
      &m_saveBufferOffset,
      &m_saveBufferSize,
      &m_saveRequestId,
      &m_saveRequestSize);

  GFL_GRPGL_ERROR_CHECK();

  if( p_buffoffset ){
    *p_buffoffset = m_saveBufferOffset;
  }
  if( p_buffsize ){
    *p_buffsize   = m_saveBufferSize;
  }
  if( p_reqid ){
    *p_reqid      = m_saveRequestId;
  }
  if( p_reqsize ){
    *p_reqsize    = m_saveRequestSize;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  再利用コマンドをカレントのコマンドリストに追加
 */
//-----------------------------------------------------------------------------
void CommandList::ReuseCommandSave( bool is_copy )
{
  this->ReuseCommandSaveEx( is_copy, m_saveBufferOffset, m_saveBufferSize, m_saveRequestId, m_saveRequestSize );
}

//----------------------------------------------------------------------------
/**
 *	@brief  再利用コマンドをカレントのコマンドリストに追加
 */
//-----------------------------------------------------------------------------
void CommandList::ReuseCommandSaveEx( bool is_copy, u32 buffoffset, s32 buffsize, u32 reqid, s32 reqsize )
{
  GFL_ASSERT( m_initialized );

#if GFL_COMMAND_LIST_DEBUG_USE
  {
    // デバッグ用に、カレントのバッファがどのくらい余っているか取っとく。
    nngxGetCmdlistParameteri( NN_GX_CMDLIST_USED_BUFSIZE, &m_debug_LocalUseBufSize );
    nngxGetCmdlistParameteri( NN_GX_CMDLIST_USED_REQCOUNT, &m_debug_LocalUseReqCount );
  }
#endif

  // セーブリクエスト　が　１つの場合。
  // セーブ最後の区切りコマンドのみということなので、
  // ジャンプを使用する。
  //
  // ２つ以上の場合は、複数のリクエストが絡んでいるので、
  // UseSavedCmdlistを使用する。
  if( m_saveRequestSize > 1 ){

    nngxUseSavedCmdlist(
        m_bufferID,
        m_saveBufferOffset,
        m_saveBufferSize,
        m_saveRequestId,
        m_saveRequestSize,
        0,
        is_copy ? GL_TRUE : GL_FALSE);
  }
  else if( m_saveBufferSize > 0 ){
    nngxAddJumpCommand( reinterpret_cast<GLvoid*>(m_bufferAddr + m_saveBufferOffset), m_saveBufferSize );
  }
  GFL_GRPGL_ERROR_CHECK();
}

// バッファのアドレスを取得
//----------------------------------------------------------------------------
/**
 *	@brief    バッファのアドレスを取得
 *	@return バッファのアドレス
 */
//-----------------------------------------------------------------------------
GLvoid* CommandList::GetBufferAddr( void ) const
{
  GFL_ASSERT( m_initialized );
  
  return reinterpret_cast<GLvoid*>(m_bufferAddr);
}

//----------------------------------------------------------------------------
/**
 *	@brief  バッファの蓄積サイズ
 */
//-----------------------------------------------------------------------------
GLsizei CommandList::GetBufferSize( void ) const
{
  GLint size;
  GFL_ASSERT( m_initialized );
  
  nngxBindCmdlist(m_bufferID);

  // サイズチェック
  nngxGetCmdlistParameteri( NN_GX_CMDLIST_USED_BUFSIZE, &size );

  GFL_GRPGL_ERROR_CHECK();

  return static_cast<GLsizei>(size);
}

//----------------------------------------------------------------------------
/**
 *	@brief  コマンドリスト　バッファ情報ダンプ
 */
//-----------------------------------------------------------------------------
void CommandList::Dump( void  ) const
{
  GLint size;
  GFL_ASSERT( m_initialized );
  
  // サイズチェック
  nngxGetCmdlistParameteri( NN_GX_CMDLIST_USED_BUFSIZE, &size );
  GFL_PRINT( "used bufffer size 0x%x\n", size );

  // REQUESTコマンド数
  nngxGetCmdlistParameteri( NN_GX_CMDLIST_USED_REQCOUNT, &size );
  GFL_PRINT( "used req count 0x%x\n", size );


  // MaxBufferサイズ
  nngxGetCmdlistParameteri( NN_GX_CMDLIST_MAX_BUFSIZE, &size );
  GFL_PRINT( "max buffer size 0x%x\n", size );

  // MaxReqサイズ
  nngxGetCmdlistParameteri( NN_GX_CMDLIST_MAX_REQCOUNT, &size );
  GFL_PRINT( "max req count 0x%x\n", size );
}


#if GFL_DEBUG
//----------------------------------------------------------------------------
/**
 *	@brief  タイムアウトコールバック関数
 */
//-----------------------------------------------------------------------------
void CommandList::TimeoutCallback(void)
{
  GLint param;
  nngxGetCmdlistParameteri( NN_GX_CMDLIST_HW_STATE, &param );
#if GFL_DEBUG_PRINT
  GFL_PRINT( "GpuWaitDone Timeout GPU HW STATE 0x%x\n", param );
  GFL_ASSERT( 0 );

#else
  //ここのアサートをエラーアプレットで出そうとすると、無限ループになる。
  //なのでFatalエラーにしてしまう。
  //ThrowFatalErrは使用禁止関数なので、製品に乗らないように。
  //ここは製品版で呼ばれないからOK。
  nn::Result result(nn::Result::LEVEL_SUCCESS,
                    nn::Result::SUMMARY_SUCCESS,
                    nn::Result::MODULE_APPLICATION,
                    param );
  nn::err::CTR::ThrowFatalErr( result , __current_pc() );
  NN_ERR_THROW_FATAL(result);
#endif
}
#endif



} /* namespace gfl */
} /* namespace grp */
