// ============================================================================
/*
 * @file gfl2_ISHA256CalcEventListener.h
 * @brief ハッシュの計算の完了を受け取るイベントリスナーです。
 * @date 2015.03.11
 */
// ============================================================================
#if !defined( GFL2_ISHA256CALCEVENTLISTENER_H_INCLUDE )
#define GFL2_ISHA256CALCEVENTLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)


class ISHA256CalcEventListener
{
public:
  // --------------------------------------------------------------------------
  /**
   * @brief  ハッシュの計算処理が成功した時に呼び出されるイベントハンドラです。
   * @param  pSHA256Hash SHA256ハッシュ
   * @param  hashSize ハッシュサイズ
   */
  // --------------------------------------------------------------------------
  virtual void OnCalcHashSuccess( const void* pSHA256Hash, u32 hashSize ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  ハッシュの計算処理が失敗した時に呼び出されるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnCalcHashFailure() = 0;
};


GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_ISHA256CALCEVENTLISTENER_H_INCLUDE
