//==============================================================================
/**
 * @file	gflnet2_IrAdapterErrorListener.h
 * @brief	IrAdapterのエラーリスナー
 * @author	ichiraku_katsuhiko
 * @date	2015/05/07 12:47:55
 */
// =============================================================================

#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_IRADAPTERERRORLISTENER_H__
#define __GFLNET_IRADAPTERERRORLISTENER_H__
#pragma once

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//------------------------------------------------------------------
/**
 * @brief   NetGameエラー通知ハンドラ
 * @return  NetGameBase及び、NetGameの通知を受け取る場合は、本クラスを継承し仮想関数を実装して下さい。
 */
//------------------------------------------------------------------
namespace gflnet2{
namespace dev{

class IrAdapterErrorListener
{
public:
  //------------------------------------------------------------------
  /**
   * @brief 赤外線壊れている
   */
  //------------------------------------------------------------------
  virtual void OnIrAdapterErrorIrBroken() = 0;

  //------------------------------------------------------------------
  /**
   * @brief 繋いだタイプが違う
   */
  //------------------------------------------------------------------
  virtual void OnIrAdapterErrorMissmatch() = 0;

  //------------------------------------------------------------------
  /**
   * @brief IrAdapterの内部エラー
   */
  //------------------------------------------------------------------
  virtual void OnIrAdapterErrorHandling() = 0;

};

} // namespace dev
} // namespace gflnet2

#endif // __GFLNET_IRADAPTERERRORLISTENER_H__
#endif // GF_PLATFORM_CTR
