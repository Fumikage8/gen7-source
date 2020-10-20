//===========================================================================
/**
 * @file    gfl2_StrSys.cpp
 * @brief   GAMEFREAK Library  String Print System ( 文字列処理システムコア )
 * @author  GAMEFREAK inc.
 * @date    2010.11.12
 */
//===========================================================================
#include <str/include/gfl2_StrSys.h>
#include <macro/include/gfl2_Macros.h>

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <wchar.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)

//----------------------------------------------------------------------------------
/**
 * タグ開始コード以降、スキップする要素数を取得
 *
 * @param[in]   sp    タグ開始コードを指しているポインタ
 *
 * @retval  u32       スキップする要素数
 */
//----------------------------------------------------------------------------------
static u32 tag_CheckSkipLength( const STRCODE* sp )
{
  GFL_ASSERT((*sp) == TAG_START_CODE);

  u32 num = *( sp+1 );

  return ( 2 + num );
}

//----------------------------------------------------------------------------------
/**
 * タグ要素をスキップ
 *
 * @param[in]   sp    タグ開始コードを指している文字列ポインタ
 *
 * @retval      const STRCODE*    タグ要素を全てスキップした後のポインタ
 */
//----------------------------------------------------------------------------------
static const STRCODE* tag_Skip( const STRCODE* sp )
{
  GFL_ASSERT((*sp) == TAG_START_CODE);

  u32 len = tag_CheckSkipLength( sp );
  sp += len;

  return sp;
}

//----------------------------------------------------------------------------------
/**
 * 指定行の先頭までポインタ移動
 *
 * @param[in]   str
 * @param[in]   lineIdx
 *
 * @retval  const STRCODE*
 */
//----------------------------------------------------------------------------------
static const STRCODE* line_Skip( const STRCODE* str, u32 lineIdx )
{
  if( lineIdx )
  {
    while( *str != EOM_CODE )
    {
      if( *str == CR_CODE )
      {
        ++str;
        if( --lineIdx == 0 ){
          break;
        }
        continue;
      }

      if( *str != TAG_START_CODE )
      {
        ++str;
      }
      else
      {
        str = tag_Skip( str );
      }
    }
  }

  if( lineIdx == 0 )
  {
    return str;
  }

  return NULL;
}

//--------------------------------------------------------------
/**
 * 文字列の長さ（終端文字を含まない）を返す
 *
 * @param[in]    str    文字列ポインタ
 *
 * @retval  u32   文字列の長さ
 */
//--------------------------------------------------------------
u32 StrLen( const STRCODE* str )
{
  u32 len = 0;
  while( *str != EOM_CODE )
  {
    if( *str == TAG_START_CODE )
    {
      const STRCODE* prev = str;

      str = tag_Skip( str );
      len += static_cast<uptr>(str - prev);
    }
    else
    {
      ++str;
      ++len;
    }

    if( len >= WARNING_LENGTH )
    {
      GFL_ASSERT(0);    // 文字列が想定以上の長さです
      break;
    }
  }
  return len;
}

//--------------------------------------------------------------
/**
 * 文字列コピー
 *
 * @param[out]   dst    コピー先の文字配列
 * @param[in]    src    コピー元の文字配列
 *
 * @retval  u32   コピーした文字数（終端文字を含まない）
 */
//--------------------------------------------------------------
u32 StrCopy( STRCODE* dst, const STRCODE* src )
{
  u32 len = 0;

  for ever
  {
    *dst = *src;

    // タグはスキップ後までの文字列をそのままコピーする
    if( *dst == TAG_START_CODE )
    {
      const STRCODE* prev = src;

      src = tag_Skip( src );
      len += static_cast<uptr>(src - prev);

      while( prev != src )
      {
        *(++dst) = *(++prev);
      }
    }

    if( *dst == EOM_CODE )
    {
      break;
    }

    ++dst;
    ++src;
    ++len;
    if( len >= WARNING_LENGTH )
    {
      GFL_ASSERT(0);    // 文字列が想定以上の長さです
      --len;
      *dst = EOM_CODE;
      break;
    }
  }
  return len;
}
//--------------------------------------------------------------
/**
 * 文字列コピー（最大文字数の指定あり）
 *
 * @param[out]   dst          コピー先の文字配列
 * @param[in]    src          コピー元の文字配列
 * @param[in]    numCharMax   コピーする最大文字数（終端文字を含む）
 *
 * @retval  u32   コピーした文字数（終端文字を含まない）
 */
//--------------------------------------------------------------
u32 StrNCopy( STRCODE* dst, const STRCODE* src, u32 numCharMax )
{
  u32 len = 0;

  while( numCharMax )
  {
    *dst = *src;

    if( *dst == TAG_START_CODE )
    {
      u32 skip_len = tag_CheckSkipLength( src );
      if( numCharMax > skip_len )
      {
        numCharMax -= skip_len;
        len += skip_len;
        while( skip_len-- )
        {
          *(++dst) = *(++src);
        }
        continue;
      }
      else
      {
        *dst = EOM_CODE;
      }
    }

    if( *dst == EOM_CODE )
    {
      break;
    }

    ++dst;
    ++src;
    ++len;
    --numCharMax;
  }

  if( (numCharMax == 0) && (len>0) )
  {
    --len;
    *(dst-1) = EOM_CODE;
  }

  return len;
}
//--------------------------------------------------------------
/**
 * 文字列比較
 *
 * @param[in]    s1       比較用文字列１
 * @param[in]    s2       比較用文字列２
 *
 * @retval  bool    s1, s2 の内容が一致していたらTRUEを返す
 */
//--------------------------------------------------------------
bool StrComp( const STRCODE* s1, const STRCODE* s2 )
{
  while( *s1 == *s2 )
  {
    if( *s1 == TAG_START_CODE )
    {
      const STRCODE* s1_next = tag_Skip( s1 );
      while( s1 != s1_next )
      {
        ++s1;
        ++s2;
        if( *s1 != *s2 )
        {
          return false;
        }
      }
    }

    if( *s1 == EOM_CODE )
    {
      return true;
    }
    ++s1;
    ++s2;
  }
  return false;
}
//--------------------------------------------------------------
/**
 * 行数カウント
 *
 * @param[in]    str      文字列ポインタ
 *
 * @retval  u32  行数（1 origin）
 */
//--------------------------------------------------------------
u32 CountLines( const STRCODE* str )
{
  u32 count = 1;

  while( *str != EOM_CODE )
  {
    if( *str == CR_CODE )
    {
      ++count;
    }

    if( *str != TAG_START_CODE )
    {
      ++str;
    }
    else
    {
      str = tag_Skip( str );
    }
  }

  return count;
}

//--------------------------------------------------------------
/**
 * 指定行の文字列長（終端文字・改行文字を含まない）を取得する
 *
 * @param[in]   str       文字列ポインタ
 * @param[in]   lineIdx   行番号（0〜）
 *
 * @retval  u32   lineIdxで指定された行の文字列長（改行／終端コードを含まない）
 */
//--------------------------------------------------------------
u32 LineLen( const STRCODE* str, u32 lineIdx )
{
  str = line_Skip( str, lineIdx );

  if( str != NULL )
  {
    const STRCODE* start = str;

    while( (*str!=EOM_CODE) && (*str!=CR_CODE) )
    {
      if( *str != TAG_START_CODE )
      {
        ++str;
      }
      else
      {
        str = tag_Skip( str );
      }
    }

    u32 len = static_cast<uptr>(str - start);
    return len;
  }

  return 0;
}

//--------------------------------------------------------------
/**
 * 指定行の先頭ポインタを取得
 *
 * @param[in]   str       文字列ポインタ
 * @param[in]   lineIdx   行番号（0〜）
 *
 * @retval  u32   lineIdxで指定された行の先頭文字ポインタ
 */
//--------------------------------------------------------------
const STRCODE* GetLinePtr( const STRCODE* str, u32 lineIdx )
{
  return line_Skip( str, lineIdx );
}

//--------------------------------------------------------------
/**
 * 指定行の文字列をコピー
 *
 * @param[out]    dst         コピー先の文字配列
 * @param[in]     dstElems    dst の要素数
 * @param[in]     src         コピー元の文字配列
 * @param[in]     lineIdx     行番号（0〜）
 *
 * @retval  u32   コピーした文字数（終端文字を含まない）
 */
//--------------------------------------------------------------
u32 LineCopy( STRCODE* dst, u32 dstElems, const STRCODE* src, u32 lineIdx )
{
  const STRCODE* line_top = line_Skip( src, lineIdx );

  if( line_top != NULL )
  {
    u32 len = LineLen( src, lineIdx ) + 1;
    if( len >= dstElems )
    {
      len = dstElems - 1;
    }
    return StrNCopy( dst, line_top, len );
  }
  return 0;
}

//--------------------------------------------------------------
/**
 * タグ開始コード〜引数部分をスキップした文字列ポインタを返す
 *
 * @param[in]   str   文字列ポインタ（タグ開始コードを指していること）
 *
 * @retval  STRCODE*  タグ開始コード〜タグ引数部をスキップした文字列ポインタ
 */
//--------------------------------------------------------------
const STRCODE* SkipTagCode( const STRCODE* str )
{
  if( *str == TAG_START_CODE )
  {
    str = tag_Skip( str );
  }
  return str;
}

//--------------------------------------------------------------
/**
 * ユーザータグコードを取得
 *
 * @param[in]   str       文字列ポインタ（タグ開始コードを指していること）
 *
 * @retval  u16   タグコード
 */
//--------------------------------------------------------------
u16 GetTagCode( const STRCODE* str )
{
  if( *str == TAG_START_CODE )
  {
    str += 2;
    return static_cast<u16>(*str);
  }
  else
  {
    GFL_ASSERT(0);
  }

  return 0;
}

//--------------------------------------------------------------
/**
 * タグ引数の個数を取得
 *
 * @param[in]   str   文字列ポインタ（タグ開始コードを指していること）
 *
 * @retval  u16   タグ引数の個数
 */
//--------------------------------------------------------------
u16 GetTagParamCount( const STRCODE* str )
{
  if( *str == TAG_START_CODE )
  {
    u16 numParams = static_cast<u16>( str[1] );
    if( (numParams > 0) && (numParams<NUM_TAG_PARAM_MAX) )
    {
      return numParams - 1;
    }
    else
    {
      GFL_ASSERT_MSG(0, "numParams=%d", numParams);
    }
  }
  else
  {
    GFL_ASSERT(0);
  }
  return 0;
}

//--------------------------------------------------------------
/**
 * タグ引数を取得
 *
 * @param[in]   str       文字列ポインタ（タグ開始コードを指していること）
 * @param[in]   paramIdx  パラメータIndex（0〜）
 *
 * @retval  u16   タグ引数
 */
//--------------------------------------------------------------
u16 GetTagParam( const STRCODE* str, u32 paramIdx )
{
  if( *str == TAG_START_CODE )
  {
    u32 numParams = static_cast<u32>( str[1] );

    if( numParams > NUM_TAG_PARAM_MAX )
    {
      GFL_ASSERT_MSG( 0, "numParams=%d", numParams );
      return 0;
    }

    if( numParams && (paramIdx <(numParams-1)) )
    {
      return static_cast<u16>( str[ 3 + paramIdx ] );
    }
    else
    {
      GFL_ASSERT_MSG(0, "paramIdx=%d, numParams=%d", paramIdx, numParams);
    }
  }
  else
  {
    GFL_ASSERT(0);
  }

  return 0;
}

#if ( _GFL_STRCODE == _GFL_STRCODE_UTF16 )
//--------------------------------------------------------------------
/**
 *  @brief  utf16->utf8 変換
 *  @detail 4byteのUTF-8文字列を返却します
 *          変換元が2byte長以下の場合、空いたバッファにはnull文字を格納します
 *
 *  @param[in]    src   変換元のUTF-16コード
 *  @param[out]   dst    変換後のUTF-8コードを格納するバッファ
 *
 *  @retval  bool  変換できたら true
 */
//--------------------------------------------------------------------
bool utf16_to_utf8( const wchar_t src, char* dst )
{
  // read single character from src
  wchar_t wcWork = src;
  size_t chSize;

  const char null = 0x00;
  char chWork1, chWork2, chWork3;

  if (wcWork <= ((wchar_t)0x007f))
  {
    // 0x0000 -> 0x007f
    chSize = 1;
  }
  else if (wcWork <= ((wchar_t)0x07ff))
  {
    // 0x0080 -> 0x07ff
    chSize = 2;
  }
  else
  {
    // 0x0800 -> 0xffff
    chSize = 3;
  }

  switch (chSize)
  {
  case 1:
    {
      // (00000000 0aaabbbb)UTF-16 -> (0aaabbbb)UTF-8
      *dst = (char)wcWork;              /* 0aaabbbb */
      *(dst + 1) = null;
      *(dst + 2) = null;
      *(dst + 3) = null;
      break;
    }

  case 2:
    {
      // (00000aaa bbbbcccc)UTF-16 -> (110aaabb 10bbcccc)UTF-8
      chWork1 = (char)(wcWork >> 6);     /* 000aaabb */
      chWork1 |= (char)0xc0;             /* 110aaabb */
      chWork2 = (char)wcWork;            /* bbbbcccc */
      chWork2 &= (char)0x3f;             /* 00bbcccc */
      chWork2 |= (char)0x80;             /* 10bbcccc */

      *dst = chWork1;
      *(dst + 1) = chWork2;
      *(dst + 2) = null;
      *(dst + 3) = null;
      break;
    }
  case 3:
    {
      // (aaaabbbb ccccdddd)UTF-16 -> (1110aaaa 10bbbbcc 10ccdddd)UTF-8
      chWork1 = (char)(wcWork >> 12);    /* ????aaaa */
      chWork1 &= (char)0x0f;             /* 0000aaaa */
      chWork1 |= (char)0xe0;             /* 1110aaaa */
      chWork2 = (char)(wcWork >> 6);     /* aabbbbcc */
      chWork2 &= (char)0x3f;             /* 00bbbbcc */
      chWork2 |= (char)0x80;             /* 10bbbbcc */
      chWork3 = (char)wcWork;            /* ccccdddd */
      chWork3 &= (char)0x3f;             /* 00ccdddd */
      chWork3 |= (char)0x80;             /* 10ccdddd */

      *dst = chWork1;
      *(dst + 1) = chWork2;
      *(dst + 2) = chWork3;
      *(dst + 3) = null;
      break;
    }
  }
  return true;
}
#endif  // #if _GFL_STRCODE == _GFL_STRCODE_UTF16


//--------------------------------------------------------------
/**
 * 文字コード Print （デバッグ用）
 *
 * @param[in]     str         文字列ポインタ
 * @param[in]     caption     プリントするキャプション
 */
//--------------------------------------------------------------
#if GFL_DEBUG
void PrintCode( const STRCODE* str, const char* caption )
{
  if( caption )
  {
    GFL_RELEASE_PRINT( caption );
  }

  #if ( _GFL_STRCODE == _GFL_STRCODE_UTF16 )
  {
    char utf8_buf[ 4 ];

    while( *str != EOM_CODE )
    {
      if( *str != TAG_START_CODE )
      {
        if( utf16_to_utf8(*str, utf8_buf) )
        {
          GFL_RELEASE_PRINT( utf8_buf );
        }
        else
        {
          GFL_RELEASE_PRINT("[%04x]", *str);
        }
        ++str;
      }
      else
      {
        const STRCODE* next = tag_Skip( str );
        GFL_RELEASE_PRINT("[TAG>]");
        while( ++str != next )
        {
          GFL_RELEASE_PRINT("%04x,", *str);
        }
        GFL_RELEASE_PRINT("[<TAG]");
      }
    }
  }
  #else
  GFL_RELEASE_PRINT( str );
  #endif

  GFL_RELEASE_PRINT("\n");
}
#else
// リリース版では何もしない
void
PrintCode( const STRCODE* , const char*  )
{
}
#endif

/**
 * 文字列比較（タグを考慮しない）
 *
 * @param[in]    s1       比較用文字列１
 * @param[in]    s2       比較用文字列２
 *
 * @retval  bool    s1, s2 の内容が一致していたらTRUEを返す
 */
bool StrComp_J( const char* s1, const char* s2 )         // [SJIS]
{
  while( *s1 == *s2 )
  {
    if( *s1 == '\0' )
    {
      return true;
    }
    ++s1;
    ++s2;
  }
  return false;
}
bool StrComp_U( const wchar_t* s1, const wchar_t* s2 )   // [UTF16]
{
  while( *s1 == *s2 )
  {
    if( *s1 == L'\0' )
    {
      return true;
    }
    ++s1;
    ++s2;
  }
  return false;
}

GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)
