﻿#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_UrlCoderUtility.h
 * @brief  UrlCoderユーティリティークラス
 * @author h.suzuki
 * @date   2011.10.21
 */
//=============================================================================

#ifndef __GFLNET_URL_CODER_UTIL_H__
#define __GFLNET_URL_CODER_UTIL_H__
#pragma once

#include <nn.h>

#include <gflnet2/include/gflnet2_NetworkDefine.h>


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)  
GFL_NAMESPACE_BEGIN(util)


/**
 * @brief URLエンコードおよびデコード用ユーティリティクラス
 */
class UrlCoderUtility
{
private:
  UrlCoderUtility();
  ~UrlCoderUtility();

private:
  static u8   EncodeHexValue( u8 value );
  static u8   CodeToValue(u8 code);
  //
  static inline bool IsNeedEncode( u8 code );

public:
  //------------------------------------------------------------------
  /**
   * @brief   UrlCoderエンコード時に必要となるバッファサイズを取得する
   *
   * @param   srcBuf[in]  エンコード対象バッファ
   * @param   srcSize[in] エンコード対象バッファのバイトサイズ
   *
   * @return  エンコードで必要となるバッファサイズ
   */
  //------------------------------------------------------------------
  static u32 GetNeedEncodeBufferSize( const void* srcBuf, u32 srcSize );

  //------------------------------------------------------------------
  /**
   * @brief   UrlCoderデコード時に必要となるバッファサイズを取得する
   *
   * @param   srcBuf[in]  デコード対象となるデータバッファ
   *          srcSize[in] デコード対象となるデータのバイトサイズ
   *
   * @return  デコードで必要となるバッファサイズ
   */
  //------------------------------------------------------------------
  static u32 GetNeedDecodeBufferSize( const void* srcBuf, u32 srcSize );

  //------------------------------------------------------------------
  /**
   * @brief   URLエンコード処理
   *
   * @param   dstBuf[out] エンコード文字列格納先バッファへのポインタ。
   *                      GetNeedEncodeBufferSize()で取得したサイズを確保したバッファを渡すこと。
   *
   * @param   srcBuf[in]  変換対象となるバッファへのポインタ
   * @param   srcSize[in] 変換対象データのバイトサイズ
   *
   * @return 変換に成功したならtrueを返却
   */
  //------------------------------------------------------------------
   static bool Encode( void *dstBuf, const void* srcBuf, const u32 srcSize );

  //------------------------------------------------------------------
  /**
   * @brief   URLデコード処理
   *
   * @param   dstBuf[out]  デコード後データを格納するためのバッファへのポインタ
   *                       GetNeedDecodeBufferSize()で取得したサイズを確保したバッファを渡すこと。
   * @param   dstSize[out] デコード後データのバイトサイズが格納される
   * @param   srcBuf[in]   デコード対象となるバッファへのポインタ
   * @param   srcSize[in]  デコード対象データのバイトサイズ
   *
   * @return 変換に成功したならtrueを返却
   */
  //------------------------------------------------------------------
  static bool Decode( void *dstBuf, u32 &dstSize, const void *srcBuf, const u32 srcSize );
};


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gflnet2)



#endif // __GFLNET_URL_CODER_UTIL_H__
#endif
