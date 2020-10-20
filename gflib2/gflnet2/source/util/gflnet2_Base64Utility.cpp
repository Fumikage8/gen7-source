#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_Base64Utility.cpp
 * @brief  Base64変換ユーティリティークラス
 * @author h.suzuki
 * @date   2011.10.21
 */
//=============================================================================

#include <nn.h>

#include <gflnet2/include/gflnet2_NetworkDefine.h>

#include <gflnet2/include/util/gflnet2_Base64Utility.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(util)


// エンコード用文字列
static const u8 *CVT_CODE = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.-";
static const u8 REST_CODE = '*';



//------------------------------------------------------------------
/**
 * @brief   コンストラクタ（隠蔽）
 */
//------------------------------------------------------------------
Base64Utility::Base64Utility()
{
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ（隠蔽）
 */
//------------------------------------------------------------------
Base64Utility::~Base64Utility()
{
}


//------------------------------------------------------------------
/**
 * @brief   Base64エンコード時に必要となるバッファサイズを取得する
 *
 * @param   srcSize エンコード対象となるデータのサイズ
 *
 * @return  エンコードで必要となるバッファサイズ
 */
//------------------------------------------------------------------
u32 Base64Utility::GetNeedEncodeBufferSize( u32 srcSize )
{
  u32 retSize   = srcSize;
  u32 remainder = (srcSize%3);

  if( remainder != 0 ){
    retSize += (3-remainder);
  }
  return ((retSize<<2) / 3) + 1;  // '\0'の１バイトを追加
}


//------------------------------------------------------------------
/**
 * @brief   Base64デコード時に必要となるバッファサイズを取得する
 *
 * @param   srcSize デコード対象となるデータのサイズ
 *
 * @return  デコードで必要となるバッファサイズ
 */
//------------------------------------------------------------------
u32 Base64Utility::GetNeedDecodeBufferSize( u32 srcSize )
{
  return (srcSize>>2) * 3;
}


//------------------------------------------------------------------
/**
 * @brief   Base64エンコード処理
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
bool Base64Utility::Encode( void *dstBuf, const void* srcBuf, const u32 srcSize )
{
  u8        *src  = (u8*)(srcBuf);
  u8        *dst  = (u8*)(dstBuf);
  u32       code  = 0;
  u32       i     = 0;
  u32       k     = 0;

  if( (dstBuf ==NULL) || (srcBuf == NULL) || (srcSize == 0) ){
    return false;
  }

  for( u32 n = 0; n < srcSize; n++ )
  {
    code <<= 8;
    code |= (*src);

    // 24bit単位に編集
    if(i == 2)
    {
      EncodeChar(code, i, dst, k);

      k   += 4;
      i    = 0;
      code = 0;
    }
    else{
      i++;
    }
    src++;
  }

  /* 24bitに満たない場合 */
  if( i != 0 ){
    EncodeChar(code, i-1, dst, k);
  }

  return true;
}


//------------------------------------------------------------------
/**
 * @brief   Base64デコード処理
 *
 * @param   dstBuf  デコード後データを格納するためのバッファへのポインタ
 * @param   dstSize デコード後データのバイトサイズが格納される
 * @param   srcBuf  デコード対象となるバッファへのポインタ
 * @param   srcSize デコード対象データのバイトサイズ
 *
 * @return  デコードに成功したならtrueを返却
 */
//------------------------------------------------------------------
bool Base64Utility::Decode( void *dstBuf, u32 &dstSize, const void *srcBuf, const u32 srcSize )
{
  dstSize = 0;

  if( (dstBuf ==NULL) || (srcBuf == NULL) || (srcSize == 0) ){
    return false;
  }
  // 元データは4の倍数のサイズでなければならない！
  if( (srcSize&0x03) != 0 ){
    return false;
  }

  for(u32 srcIndex = 0; (srcIndex < srcSize); srcIndex += 4 )
  {
    DecodeStr(srcIndex, (u8*)srcBuf, (u8*)dstBuf, dstSize);
  }

  return true;
}


//------------------------------------------------------------------
/**
 * @brief   Base64エンコード処理
 *          一般で使用される「+」「/」「=」の代わりに「.」「-」「*」が使用される。
 *
 * @param   srcCode 変換元データコード
 *          srcSize 変換元データサイズ
 * @param   destBuf エンコードデータ格納先バッファへのポインタ
 * @param   destIdx エンコードデータ格納インデックス
 */
//------------------------------------------------------------------
void Base64Utility::EncodeChar(u32 srcCode, u32 srcSize, u8 *destBuf, u32 destIdx)
{
  u32   n = 0;

  /* 最終位置の計算 */
  for ( u32 i = srcSize; i < 2; i++ ){
    srcCode <<= 8;
  }

  /* BASE64変換 */
  for ( u32 base = 18; n < (srcSize + 2); n++, base -= 6) {
    destBuf[destIdx++] = CVT_CODE[ (u32)((srcCode>>base) & 0x3F) ];
  }

  // 端数の判断
  for (  u32 i = n; i < 4; i++ ) {
    destBuf[destIdx++] = REST_CODE;   // 端数
  }
}


//------------------------------------------------------------------
/**
 * @brief   Base64コードを元データコードに変換
 *
 * @param   code 変換元コード
 *
 * @return  デコード後のデータ
 */
//------------------------------------------------------------------
s8 Base64Utility::CodeToValue(u8 code)
{
  if( (code >= 'A') && (code <= 'Z') ) {
    return (code - 'A');
  }
  else if( (code >= 'a') && (code <= 'z') ) {
    return ((code - 'a') +26);
  }
  else if( (code >= '0') && (code <= '9') ) {
    return ((code - '0') +52);
  }
  else if( '.' == code ) {
    return 62;
  }
  else if( '-' == code ) {
    return 63;
  }
  else {
    return -1;
  }
}


//------------------------------------------------------------------
/**
 * @brief   Base64文字列を元データに変換
 *
 * @param   encodeIdx 変換対象データの参照インデックス
 * @param   src       Base64文字列データへのポインタ
 * @param   dest      デコードデータ格納バッファへのポインタ
 *
 * @return  デコードに成功したならtrueを返却
 */
//------------------------------------------------------------------
void Base64Utility::DecodeStr( u32 encodeIdx, const u8 *src, u8 *dest, u32& dstIdx )
{
  unsigned  long  base64    = 0;
  u32             decodeNum = 0;
  u32             termIndex = encodeIdx + 4;

  for (u32 i = encodeIdx; i < termIndex; i++)
  {
    int value = CodeToValue(src[i]);

    if( value >= 0 )
    {
      base64 |= (u8)value;
      decodeNum++;
    }
    if ((i - encodeIdx) != 3){
      base64 <<= 6;
    }
  }
  base64 <<= 8;

  u32 validNum = decodeNum*6/8;
  u32 endIdx   = 3-validNum;

  u8 *base64Ptr = (u8*)(&base64);
  for(u32 i = 3; i > endIdx; i--) {
    dest[dstIdx++] = base64Ptr[i];
  }
}





GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gflnet2)

#endif
