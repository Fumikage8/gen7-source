#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_UrlCoderUtility.cpp
 * @brief  UrlCoder変換ユーティリティークラス
 * @author h.suzuki
 * @date   2011.10.21
 */
//=============================================================================

#include <nn.h>

#include <gflnet2/include/gflnet2_NetworkDefine.h>

#include <gflnet2/include/util/gflnet2_UrlCoderUtility.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(util)

#define ASCII_ALPHA1_START  'A'
#define ASCII_ALPHA1_END    'Z'
#define ASCII_ALPHA2_START  'a'
#define ASCII_ALPHA2_END    'z'
#define ASCII_DIGIT_START   '0'
#define ASCII_DIGIT_END     '9'
#define ASCII_HYPHEN        '-'
#define ASCII_PERIOD        '.'
#define ASCII_UNDERSCORE    '_'
#define ASCII_TILDA         '~'
#define ASCII_PLUS          '+'
#define ASCII_PERCENT       '%'

// エンコード用文字列
static const u8 *CVT_URL_CODE = "0123456789ABCDEF";



//------------------------------------------------------------------
/**
 * @brief   コンストラクタ（隠蔽）
 */
//------------------------------------------------------------------
UrlCoderUtility::UrlCoderUtility()
{
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ（隠蔽）
 */
//------------------------------------------------------------------
UrlCoderUtility::~UrlCoderUtility()
{
}

//------------------------------------------------------------------
/**
 * @brief   文字のエンコードが必要かを判断
 *
 * @param   変換判定文字コード
 *
 * @return  エンコードが必要な文字ならtrueを返却
 */
//------------------------------------------------------------------
inline bool UrlCoderUtility::IsNeedEncode( u8 srcCode )
{
  return !(((ASCII_ALPHA1_START <= srcCode) &&  (srcCode<= ASCII_ALPHA1_END)) ||
        ((ASCII_ALPHA2_START <= srcCode) &&  (srcCode<= ASCII_ALPHA2_END)) ||
        ((ASCII_DIGIT_START  <= srcCode) &&  (srcCode<= ASCII_DIGIT_END))  ||
        (srcCode == ASCII_HYPHEN)     ||
        (srcCode == ASCII_PERIOD)     ||
        (srcCode == ASCII_UNDERSCORE) ||
        (srcCode == ASCII_TILDA));
}

//------------------------------------------------------------------
/**
 * @brief   UrlCoderエンコード時に必要となるバッファサイズを取得する
 *
 * @param   srcBuf  エンコード対象バッファ
 * @param   srcSize エンコード対象バッファのバイトサイズ
 *
 * @return  エンコードで必要となるバッファサイズ
 */
//------------------------------------------------------------------
u32 UrlCoderUtility::GetNeedEncodeBufferSize( const void* srcBuf, u32 srcSize )
{
  if( (srcBuf == NULL) || (srcSize == 0) ){
    return 0;
  }
  u32 needSize = 0;
  u8  *src     = (u8*)(srcBuf);

  for( u32 n = 0; n < srcSize; n++ )
  {
    // 無変換文字
    if( IsNeedEncode(src[n]) )
    {
      needSize++;
    }
    else
    {
      needSize+=3;
    }
  }
  return needSize;
}

//------------------------------------------------------------------
/**
 * @brief   UrlCoderデコード時に必要となるバッファサイズを取得する
 *
 * @param   srcBuf  デコード対象となるデータバッファ
 *          srcSize デコード対象となるデータのバイトサイズ
 *
 * @return  デコードで必要となるバッファサイズ
 */
//------------------------------------------------------------------
u32 UrlCoderUtility::GetNeedDecodeBufferSize( const void* srcBuf, u32 srcSize )
{
  if( (srcBuf == NULL) || (srcSize == 0) ){
    return 0;
  }
  u32 needSize = 0;
  u8  *src     = (u8*)(srcBuf);

  for( u32 n = 0; n < srcSize; n++ )
  {
    if( src[n] == ASCII_PERCENT ){
      n += 2;
    }
    needSize++;
  }
  return needSize;
}

//------------------------------------------------------------------
/**
 * @brief   UrlCoderエンコード処理
 *          一般で使用される「+」「/」「=」の代わりに「.」「-」「*」が使用される。
 *
 * @param   dstBuf  エンコード文字列格納先バッファへのポインタ。
 *                  GetNeedEncodeBufferSize()で取得したサイズを確保したバッファを渡すこと。
 *
 * @param   srcBuf  変換対象となるバッファへのポインタ
 * @param   srcSize 変換対象データのバイトサイズ
 *
 * @return  エンコードに成功したならtrueを返却
 */
//------------------------------------------------------------------
bool UrlCoderUtility::Encode( void *dstBuf, const void* srcBuf, const u32 srcSize )
{
  if( (dstBuf ==NULL) || (srcBuf == NULL) || (srcSize == 0) ){
    return false;
  }
  u8 *src = (u8*)(srcBuf);
  u8 *dst = (u8*)(dstBuf);

  for( u32 n = 0; n < srcSize; n++ )
  {
    u8 srcCode = src[n];

    // 無変換文字
    if( IsNeedEncode(srcCode) )
    {
      (*dst) = srcCode;
      dst++;
    }
    else
    {
      (*dst) = ASCII_PERCENT;
      dst++;
      (*dst) = EncodeHexValue( (srcCode>>4)&0x0F );
      dst++;
      (*dst) = EncodeHexValue( (srcCode&0x0F)    );
      dst++;
    }
  }

  return true;
}

//------------------------------------------------------------------
/**
 * @brief   UrlCoderデコード処理
 *
 * @param   dstBuf  デコード後データを格納するためのバッファへのポインタ
 * @param   dstSize デコード後データのバイトサイズが格納される
 * @param   srcBuf  デコード対象となるバッファへのポインタ
 * @param   srcSize デコード対象データのバイトサイズ
 *
 * @return  デコードに成功したならtrueを返却
 */
//------------------------------------------------------------------
bool UrlCoderUtility::Decode( void *dstBuf, u32 &dstSize, const void *srcBuf, const u32 srcSize )
{
  dstSize = 0;

  if( (dstBuf ==NULL) || (srcBuf == NULL) || (srcSize == 0) ){
    return false;
  }
  u8 *src = (u8*)(srcBuf);
  u8 *dst = (u8*)(dstBuf);

  for( u32 n = 0; n < srcSize; n++ )
  {
    u8 srcCode = src[n];

    if( srcCode == ASCII_PERCENT )
    {
      *dst = (CodeToValue(src[n+1])<<4) + (CodeToValue(src[n+2]));
      n += 2;
    }
    else if( srcCode ==ASCII_PLUS ){
      *dst = ' ';
    }
    else
    {
      *dst = srcCode;
    }
    dst++;
    dstSize++;
  }

  return true;
}

//------------------------------------------------------------------
/**
 * @brief   16進数数値を文字に変換
 *
 * @param   value 変換元データ
 *
 * @return  変換された文字コード
 */
//------------------------------------------------------------------
u8 UrlCoderUtility::EncodeHexValue( u8 value )
{
  return CVT_URL_CODE[value];
}

//------------------------------------------------------------------
/**
 * @brief   UrlCoderコードを元データコードに変換
 *
 * @param   code 変換元コード
 *
 * @return  デコード後のデータ
 */
//------------------------------------------------------------------
u8 UrlCoderUtility::CodeToValue(u8 code)
{
  if( (code >= 'A') && (code <= 'Z') ) {
    return (code - 'A') + 10;
  }
  else if( (code >= 'a') && (code <= 'z') ) {
    return (code - 'a') + 10;
  }
  else if( (code >= '0') && (code <= '9') ) {
    return (code - '0');
  }
  return 0;
}


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gflnet2)

#endif
