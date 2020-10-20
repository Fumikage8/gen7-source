﻿#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_Base64Utility.h
 * @brief  Base64変換ユーティリティークラス
 * @author h.suzuki
 * @date   2011.10.21
 */
//=============================================================================

#ifndef __GFLNET_BASE64_UTIL_H__
#define __GFLNET_BASE64_UTIL_H__
#pragma once

#include <nn.h>

#include <gflnet2/include/gflnet2_NetworkDefine.h>


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)  

/**
 * @namespace gflnet2::util
 * @brief     ユーティリティ機能関連
 */
GFL_NAMESPACE_BEGIN(util)


/**
 * @brief Base64エンコードおよびデコード用ユーティリティクラス
 */
class Base64Utility
{
private:
  Base64Utility();
  ~Base64Utility();

private:
  static void EncodeChar(u32 srcCode, u32 srcSize, u8 *destBuf, u32 destIdx);
  static s8   CodeToValue(u8 code);
  static void DecodeStr( u32 encodeIdx, const u8 *src, u8 *dest, u32 &dstIdx );

public:

  //------------------------------------------------------------------
  /**
   * @brief   Base64エンコード時に必要となるバッファサイズを取得する
   *
   * @param[in]   srcSize エンコード対象となるデータのサイズ
   *
   * @return  エンコードで必要となるバッファサイズ
   */
  //------------------------------------------------------------------
  static u32 GetNeedEncodeBufferSize( u32 srcSize );

  //------------------------------------------------------------------
  /**
   * @brief   Base64デコード時に必要となるバッファサイズを取得する
   *
   * @param[in]   srcSize デコード対象となるデータのサイズ
   *
   * @return  デコードで必要となるバッファサイズ
   */
  //------------------------------------------------------------------
  static u32 GetNeedDecodeBufferSize( u32 srcSize );

  //------------------------------------------------------------------
  /**
   * @brief   Base64エンコード処理
   *          一般で使用される「+」「/」「=」の代わりに「.」「-」「*」が使用される。
   *          (NEXライブラリでは、上記の変換ルールが適応される）
   *
   * @param[out]   dstBuf  エンコード文字列格納先バッファへのポインタ。
   *                       GetNeedEncodeBufferSize()で取得したサイズを確保したバッファを渡すこと。
   *
   * @param[in]   srcBuf  変換対象となるバッファへのポインタ
   * @param[in]   srcSize 変換対象データのバイトサイズ
   *
   * @return 変換に成功したならtrueを返却
   */
  //------------------------------------------------------------------
   static bool Encode( void *dstBuf, const void* srcBuf, const u32 srcSize );

  //------------------------------------------------------------------
  /**
   * @brief   Base64デコード処理
   *
   * @param[out]   dstBuf  デコード後データを格納するためのバッファへのポインタ。
   *                       GetNeedDecodeBufferSize()で取得したサイズを確保したバッファを渡すこと。
   * @param[out]   dstSize デコード後データのバイトサイズが格納される
   * @param[in]    srcBuf  デコード対象となるバッファへのポインタ
   * @param[in]    srcSize デコード対象データのバイトサイズ
   *
   * @return 変換に成功したならtrueを返却
   */
  //------------------------------------------------------------------
  static bool Decode( void *dstBuf, u32 &dstSize, const void *srcBuf, const u32 srcSize );
};


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gflnet2)



#endif // __GFLNET_BASE64_UTIL_H__
#endif
