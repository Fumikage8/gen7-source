//===========================================================================
/**
 * @file    gfl_StrSys.cpp
 * @brief   GAMEFREAK Library  String Print System ( 文字列処理システムコア )
 * @author  GAMEFREAK inc.
 * @date    2010.11.12
 */
//===========================================================================
#include <gfl_Common.h>

#include <str/gfl_StrSys.h>


namespace gfl {
namespace str {

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

  return 2 + num;
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

  if( lineIdx == 0 ){
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
u32
StrLen( const STRCODE* str )
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
u32
StrCopy( STRCODE* dst, const STRCODE* src )
{
  u32 len = 0;
  while( 1 )
  {
    *dst = *src;

    // タグはスキップ後までの文字列をそのままコピーする
    if( *dst == TAG_START_CODE )
    {
      const STRCODE* prev = src;

      src = tag_Skip( src );
      len += static_cast<uptr>(src - prev);

      while( prev != src ){
        *(++dst) = *(++prev);
      }
    }

    if( *dst == EOM_CODE ){
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
u32
StrNCopy( STRCODE* dst, const STRCODE* src, u32 numCharMax )
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
      else{
        *dst = EOM_CODE;
      }
    }

    if( *dst == EOM_CODE ){
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
bool
StrComp( const STRCODE* s1, const STRCODE* s2 )
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
        if( *s1 != *s2 ){
          return false;
        }
      }
    }

    if( *s1 == EOM_CODE ){
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
 * @param[in]   lineIdx   行番号（0～）
 *
 * @retval  u32   lineIdxで指定された行の文字列長（改行／終端コードを含まない）
 */
//--------------------------------------------------------------
u32
LineLen( const STRCODE* str, u32 lineIdx )
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
 * @param[in]   lineIdx   行番号（0～）
 *
 * @retval  u32   lineIdxで指定された行の先頭文字ポインタ
 */
//--------------------------------------------------------------
const STRCODE*
GetLinePtr( const STRCODE* str, u32 lineIdx )
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
 * @param[in]     lineIdx     行番号（0～）
 *
 * @retval  u32   コピーした文字数（終端文字を含まない）
 */
//--------------------------------------------------------------
u32
LineCopy( STRCODE* dst, u32 dstElems, const STRCODE* src, u32 lineIdx )
{
  const STRCODE* line_top = line_Skip( src, lineIdx );

  if( line_top != NULL )
  {
    u32 len = LineLen( src, lineIdx ) + 1;
    if( len >= dstElems ){
      len = dstElems - 1;
    }
    return StrNCopy( dst, line_top, len );
  }
  return 0;
}

//--------------------------------------------------------------
/**
 * タグ開始コード～引数部分をスキップした文字列ポインタを返す
 *
 * @param[in]   str   文字列ポインタ（タグ開始コードを指していること）
 *
 * @retval  STRCODE*  タグ開始コード～タグ引数部をスキップした文字列ポインタ
 */
//--------------------------------------------------------------
const STRCODE*
SkipTagCode( const STRCODE* str )
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
u16
GetTagCode( const STRCODE* str )
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
u16
GetTagParamCount( const STRCODE* str )
{
  if( *str == TAG_START_CODE )
  {
    u16 numParams = static_cast<u16>( str[1] );
    if( (numParams > 0) && (numParams<NUM_TAG_PARAM_MAX) ){
      return numParams - 1;
    }else{
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
 * @param[in]   paramIdx  パラメータIndex（0～）
 *
 * @retval  u16   タグ引数
 */
//--------------------------------------------------------------
u16
GetTagParam( const STRCODE* str, u32 paramIdx )
{
  if( *str == TAG_START_CODE )
  {
    u32 numParams = static_cast<u32>( str[1] );

    if( numParams > NUM_TAG_PARAM_MAX ){
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

#if _GFL_STRCODE == _GFL_STRCODE_UTF16
//--------------------------------------------------------------------
/**
 *  utf16->sjis 変換
 *
 *  @param[in]    code   変換元のUTF-16コード
 *  @param[out]   dst    変換後のsjisコードを格納するバッファ
 *
 *  @retval  bool  変換できたら true
 */
//--------------------------------------------------------------------
bool utf16_to_sjis( wchar_t code, char* dst )
{
  int numPutCode = 0;

  do {
    // 上位8bit == 0 -> ascii
    if( ((code & 0xff00) == 0)
    &&  ((code & 0x00ff) < 0x007f)
    ){
      dst[numPutCode++] = code & 0x00ff;
      break;
    }

    // ある程度連続した文字コード群処理
    {
      static const struct {
        wchar_t   uc_min;
        wchar_t   uc_max;
        u16       sjis_min;
      }convert_cont[] = {
        { 0x3041,  0x3093,  0x829f },   // 3041 ~ 3093：ひらがな（ぁ～ん）
        { 0x30a1,  0x30df,  0x8340 },   // 30a1 ~ 30df：カタカナ（ァ～ミ）
        { 0x30e0,  0x30f6,  0x8380 },   // 30e0 ~ 30f6：カタカナ（ム～ヶ）
        { 0xff10,  0xff19,  0x824f },   // ff10 ~ ff19：全角数字（０～９）
        { 0xff21,  0xff3a,  0x8260 },   // ff21 ~ ff3a：全角数字（Ａ～Ｚ）
        { 0xff41,  0xff5a,  0x8281 },   // ff41 ~ ff5a：全角数字（ａ～ｚ）
      };

      for(int i=0; i<GFL_NELEMS(convert_cont); ++i)
      {
        if( (code >= convert_cont[i].uc_min) && (code <= convert_cont[i].uc_max) )
        {
          u16 sjis = convert_cont[i].sjis_min + ( code - convert_cont[i].uc_min );
          dst[numPutCode++] = (sjis >> 8) & 0xff;
          dst[numPutCode++] = sjis & 0xff;
          break;
        }
      }
      if( numPutCode ){
        break;
      }
    }

    // 使いそうな記号など個別に
    {
      static const struct {
        u16  unicode;
        u16  sjis;
      }convert_indv[] = {
        { 0x3000,  0x8140 },  //
        { 0x3002,  0x8142 },  // 。
        { 0xff1f,  0x8148 },  // ？
        { 0xff01,  0x8149 },  // ！
        { 0x30fc,  0x815b },  // ー
        { 0xff0d,  0x817c },  // －
        { 0x25cb,  0x819b },  // ○
        { 0x25cf,  0x819c },  // ●
        { 0x25ce,  0x819d },  // ◎
        { 0x25c7,  0x819e },  // ◇
        { 0x25c6,  0x819f },  // ◆
        { 0x25a1,  0x81a0 },  // □
        { 0x25a0,  0x81a1 },  // ■
        { 0x25b3,  0x81a2 },  // △
        { 0x25b2,  0x81a3 },  // ▲
        { 0x25bd,  0x81a4 },  // ▽
        { 0x25bc,  0x81a5 },  // ▼
        { 0x2192,  0x81a8 },  // →
        { 0x2190,  0x81a9 },  // ←
        { 0x2191,  0x81aa },  // ↑
        { 0x2193,  0x81ab },  // →
      };

      for(int i=0; i<GFL_NELEMS(convert_indv); ++i)
      {
        if( convert_indv[i].unicode == code )
        {
          dst[numPutCode++] = (convert_indv[i].sjis >> 8) & 0xff;
          dst[numPutCode++] = convert_indv[i].sjis & 0xff;
          break;
        }
      }
      if( numPutCode ){
        break;
      }
    }

  }while( 0 );


  if( numPutCode ){
    dst[numPutCode] = '\0';
    return true;
  }

  return false;
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
void
PrintCode( const STRCODE* str, const char* caption )
{
  if( caption ){
    GFL_PRINT( caption );
  }

  #if _GFL_STRCODE == _GFL_STRCODE_UTF16
  {
    char sjis_buf[ 4 ];

    while( *str != EOM_CODE )
    {
      if( *str != TAG_START_CODE )
      {
        if( utf16_to_sjis(*str, sjis_buf) ){
          GFL_PRINT( sjis_buf );
        }else{
          GFL_PRINT("[%04x]", *str);
        }
        ++str;
      }
      else
      {
        const STRCODE* next = tag_Skip( str );
        GFL_PRINT("[TAG>]");
        while( ++str != next )
        {
          GFL_PRINT("%04x,", *str);
        }
        GFL_PRINT("[<TAG]");
      }
    }
  }
  #else
  GFL_PRINT( str );
  #endif

  GFL_PRINT("\n");
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
bool    StrComp_J( const char* s1, const char* s2 )         // [SJIS]
{
  while( *s1 == *s2 )
  {
    if( *s1 == '\0' ){
      return true;
    }
    ++s1;
    ++s2;
  }
  return false;
}
bool    StrComp_U( const wchar_t* s1, const wchar_t* s2 )   // [UTF16]
{
  while( *s1 == *s2 )
  {
    if( *s1 == L'\0' ){
      return true;
    }
    ++s1;
    ++s2;
  }
  return false;
}

} /* end of namespace str */
} /* end of namespace gfl */
