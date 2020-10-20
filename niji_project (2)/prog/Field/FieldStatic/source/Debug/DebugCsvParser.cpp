//======================================================================
/**
* @file DebugCsvParser.cpp
* @date 2016/10/04 16:18:19
* @author ikuta_junya
* @brief csvファイルのパーサー、CTR/Win32共通。デバッグ用。
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if PM_DEBUG

#include <string.h>

// gfl2
#include <util/include/gfl2_std_string.h>
#include <fs/include/gfl2_FsPcUtil.h>
#include <debug/include/gfl2_DebugPrint.h>

// field
#include "Field/FieldStatic/include/Debug/DebugCsvParser.h"


#if defined(GF_PLATFORM_WIN32)
#include <windows.h>    // for MultiByteToWideChar,WideCharToMultiByte
#endif
 
#if defined(GF_PLATFORM_CTR)
#include <locale.h>
#include <stdarg.h>
#endif


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)


#if defined(GF_PLATFORM_WIN32)
// utf文字列をsjis文字列に変換する
// @note unicode文字列に一度変換するため、使用メモリに注意が必要
void Utf8toSjis( gfl2::heap::HeapBase* pHeap, const char *pUtf8, char** ppOut, size_t* pSize )
{
  // utf8 -> unicode
  // 変換に必要なバッファサイズを取得
  int lenUnicode = MultiByteToWideChar( CP_UTF8, 0, pUtf8, -1, NULL, 0);
  wchar_t* pUnicodeBuf = GFL_NEW_ARRAY( pHeap ) wchar_t[ lenUnicode ];
  MultiByteToWideChar( CP_UTF8, 0, pUtf8, -1, pUnicodeBuf, lenUnicode );

  // unicode -> sjis
  int lenSjis = WideCharToMultiByte( CP_ACP, 0, pUnicodeBuf, lenUnicode, NULL, 0, NULL, NULL );
  c8* pSjisBuf = GFL_NEW_ARRAY( pHeap ) c8[ lenSjis ];
  WideCharToMultiByte( CP_ACP, 0, pUnicodeBuf, lenUnicode, pSjisBuf, lenSjis, NULL, NULL );

  *ppOut = pSjisBuf;
  *pSize = lenSjis;

  GFL_SAFE_DELETE_ARRAY( pUnicodeBuf );
}
#endif



/**
* @breaf 多バイト文字列 (multibyte string) をワイド文字列 (wide string) に変換します
* @param wcstr      ド文字列を格納する場所
* @param mbstr      変換する多バイト文字列
* @param count      変換する文字列の文字数
* @return 変換に成功したとき: 変換した文字数 (終端のナルワイド文字は除く)/失敗or不正な多バイト文字が含まれているとき: (size_t)(-1)
*/
size_t CsvParser::MbstoWcs( wchar_t *wcstr, const char *mbstr, size_t count )
{
  size_t ret;

#if defined(GF_PLATFORM_WIN32)
  char * current_locale; 
  char prev_locale[512];

  // 現在のロケールを保存
  strcpy_s( prev_locale, sizeof( prev_locale ), setlocale( LC_CTYPE, NULL ) );

  current_locale = setlocale( LC_CTYPE, "JPN" );
  if( current_locale != NULL )
  { 
    size_t ret;
    if( mbstowcs_s( &ret, wcstr, count, mbstr, _TRUNCATE ) != 0 )
    {
      ret = -1;
    }
    setlocale( LC_CTYPE, prev_locale );      // 復元
    return ret;
  }
  else
  {
    return static_cast< size_t >( -1 );
  }

#elif defined(GF_PLATFORM_CTR)
  ret = mbstowcs( wcstr, mbstr, count );
#endif

  return ret;
}


// 改行コード(CRLF)判定
// @note ポインタが0x0dの場合、１つ先まで参照します
b32 IsCRLF( const c8* ptr )
{
  if( *ptr == 0x0d && *(ptr + 1) == 0x0a )
  {
    return true;
  }
  else
  {
    return false;
  }
}

b32 IsLF( const c8* ptr )
{
  if( *ptr == 0x0a )
  {
    return true;
  }
  else
  {
    return false;
  }
}

b32 IsEnterCode( c8* ptr, c8** pOutNext = NULL )
{
  if( IsCRLF( ptr ) )
  {
    if( pOutNext ) *pOutNext = ( ptr + 2 );
    return true;
  }


  if( IsLF( ptr ) )
  {
    if( pOutNext ) *pOutNext = ( ptr + 1 );
    return true;
  }

  return false;
}

struct CsvParser::RowObject::ColumnData
{
  c8* pTextData;
  u32 textLen;
};

CsvParser::RowObject::RowObject( gfl2::heap::HeapBase* pHeap, c8* pData ) :
  m_pColumnDataTable( NULL )
  ,m_Size( 0 )
{
  u32 dataSize = GetDataSize( pData );
  if( dataSize == 0 )
  {
    return;
  }

  m_pColumnDataTable = GFL_NEW_ARRAY( pHeap ) ColumnData[ dataSize ];
  m_Size = dataSize;

  s32 dataCount = 0;
  s32 textCount = 0;
  c8* ptr = pData;
  c8* dataStartPtr = ptr;
  b32 escapeFlag = false;
  b32 isDoubleQuotation = false;

  while( 1 )
  {
    if( *ptr == '"' )
    {
      escapeFlag = !escapeFlag;
      isDoubleQuotation = true;
    }

    if( ( !escapeFlag && *ptr == ',' ) || ( !escapeFlag && IsEnterCode( ptr ) ) || *ptr == '\0' )
    {
      if( isDoubleQuotation )
      {
        dataStartPtr++;
        textCount = textCount - 2;
      }

      m_pColumnDataTable[dataCount].pTextData = GFL_NEW_ARRAY( pHeap ) c8[ textCount + 1 ];
      m_pColumnDataTable[dataCount].textLen = textCount;
      if( textCount > 0 )
      {
        gfl2::std::MemCopy( dataStartPtr, m_pColumnDataTable[dataCount].pTextData, textCount );
      }
      m_pColumnDataTable[dataCount].pTextData[textCount] = '\0';

      // @note csvはutf8。win32のcharはsjis。ctrのcharはutf8。の状態。
      // @note win32版ではutf8をsjis変換し保持しておく
      // @note ctrでsjisを扱い、変換しようとするとプログラムサイズが25kbほど増えるため、見送った
#if defined(GF_PLATFORM_WIN32)
      {
        c8* pSjis;
        size_t size;
        Utf8toSjis( pHeap, m_pColumnDataTable[dataCount].pTextData, &pSjis, &size );
        GFL_SAFE_DELETE_ARRAY(  m_pColumnDataTable[dataCount].pTextData );
        m_pColumnDataTable[dataCount].pTextData = pSjis;
        m_pColumnDataTable[dataCount].textLen = size - 1;   // @note \0つきなので-1
      }
#endif

      // GFL_PRINT( "m_ppTable[%d][%s]\n", dataCount, m_ppTable[dataCount] );
      if( ( !escapeFlag && IsEnterCode( ptr ) ) || *ptr == '\0' )
      {
        break;
      }
      dataCount++;
      textCount = 0;
      ptr++;
      dataStartPtr = ptr;
      isDoubleQuotation = false;
    }
    else
    {
      textCount++;
      ptr++;
    }
  }
}

CsvParser::RowObject::~RowObject()
{
  if( m_pColumnDataTable )
  {
    for( u32 i = 0; i < m_Size; i++ )
    {
      GFL_SAFE_DELETE_ARRAY( m_pColumnDataTable[i].pTextData );
    }
  }

  GFL_SAFE_DELETE_ARRAY( m_pColumnDataTable );
}

const c8* CsvParser::RowObject::GetColumnData( s32 columnNo ) const
{
  return m_pColumnDataTable[columnNo].pTextData;
}

u32 CsvParser::RowObject::GetColumnTextLen( s32 columnNo ) const
{
  return m_pColumnDataTable[columnNo].textLen;
}

b32 CsvParser::RowObject::Comparisons( s32 columnNo, gfl2::heap::HeapBase* pHeap, const wchar_t* pText ) const
{
  if( columnNo >= m_Size )
  {
    return false;
  }

  if( m_pColumnDataTable[columnNo].textLen == 0 )
  {
    // 相手も0なら一緒
    return ( ::std::wcslen( pText ) == 0 );
  }
  else
  {
    b32 result = false;

    // @note 多バイト文字状態では、csv=ShiftJIS、win32=ShiftJIS、ctr=utf-8状態なので、utf-16に変換してから比較する
    wchar_t* pWork = GFL_NEW_ARRAY( pHeap ) wchar_t [ m_pColumnDataTable[columnNo].textLen + 1 ];
    MbstoWcs( pWork, m_pColumnDataTable[columnNo].pTextData, m_pColumnDataTable[columnNo].textLen + 1 );

    //if( gfl2::core::Utility::Strcmp( pRowObject->GetColumnData( 2 ), "○" ) == 0 )
    if( ::std::wcscmp( pWork, pText ) == 0 )
    {
      result = true;
    }

    GFL_SAFE_DELETE_ARRAY( pWork );
    return result;
  }
}

s32 CsvParser::RowObject::GetDataSize( c8* pData ) const
{
  s32 dataCount = 0;
  b32 escapeFlag = false;

  c8* ptr = pData;
  while( !( !escapeFlag && IsEnterCode( ptr ) ) && *ptr != '\0' )
  {
    if( *ptr == '"' )
    {
      escapeFlag = !escapeFlag;
    }

    if( !escapeFlag && *ptr == ',' )
    {
      dataCount++;
    }
    ptr++;
  }

  dataCount++;    // 最後に,は付かないので
  return dataCount;
}

CsvParser::CsvParser( void ) : 
  m_pData( NULL )
  ,m_LineNum( 0 )
{

}

CsvParser::CsvParser( gfl2::heap::HeapBase* pHeap, const c8* pFilePath ) :
  m_pData( NULL )
  ,m_LineNum( 0 )
{
  // csvファイルがあればロードし、セットアップする
  size_t fileSize;
  if( gfl2::fs::PcUtil::GetFileSize( &fileSize, pFilePath, true ).IsSuccess() )
  {
    c8* pBuffer = reinterpret_cast<c8*>( GflHeapAllocMemory( pHeap, fileSize ) );

    size_t size;
    if ( gfl2::fs::PcUtil::ReadFile( &size, pBuffer, fileSize, pFilePath, true ).IsSuccess() )
    {
      // セットアップ
      SetData( pBuffer, fileSize );
    }
    else
    {
      GflHeapSafeFreeMemory( pBuffer );
    }
  }
}

CsvParser::~CsvParser()
{
  DeleteData();
}

/**
* @breaf データを設定
* @note  既に設定されている場合は失敗します
*
* @param  pData コンマ形式で記述されて文字列(メモリ管理も渡す)  
* @param  size  データのサイズ
*
* @return なし
*/
void CsvParser::SetData( c8* pData, s32 size )
{
  if( pData == NULL )
  {
    return;
  }

  c8* pEnd = ( pData + size );

  m_pData = pData;

  s32 lineCount = 0;
  c8* ptr = pData;
  b32 escapeFlag = false;
  while( ptr != pEnd )
  {
    if (*ptr == '"')
    {
      escapeFlag = !escapeFlag;
    }
    
    if( IsEnterCode( ptr, &ptr ) )
    {
      if (!escapeFlag)
      {
        lineCount++;
      }
    }
    else
    {
      ptr++;
    }
  }

  m_LineNum = lineCount;
}

/**
* @breaf データを削除
*/
void CsvParser::DeleteData( void )
{
  GflHeapSafeFreeMemory( m_pData );
  m_LineNum = 0;
}

/**
* @breaf 1行分のデータを取得
* @param 1行分のデータ管理オブジェクト作成に使用するヒープ
* @param index 行番号
* @return 1行分のデータ管理オブジェクト( index値が不正な場合はNULL )
*/
CsvParser::RowObject* CsvParser::GetRowData( gfl2::heap::HeapBase* pHeap, s32 index )
{
  if( index >= m_LineNum )
  {
    return NULL;
  }

  return GFL_NEW( pHeap ) RowObject( pHeap, GetLineTop( index ) );
}


/**
* @breaf 行の頭を取得
* @param index 行番号
* @return 行の頭のポインタ
*/
c8* CsvParser::GetLineTop( s32 index )
{
  // @todo 高速化
  if( index >= m_LineNum )
  {
    return NULL;
  }

  s32 lineCount = 0;
  c8* ptr = m_pData;
  b32 escapeFlag = false;

  while( *ptr != -1 )
  {
    if( lineCount == index )
    {
      return ptr;
    }

    if (*ptr == '"')
    {
      escapeFlag = !escapeFlag;
    }


    // \r\nをチェック
    if( IsEnterCode( ptr, &ptr ) )
    {
      if(!escapeFlag)
      {
        lineCount++;
      }
    }
    else
    {
      ptr++;
    }
  }

  return NULL;
}


GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif