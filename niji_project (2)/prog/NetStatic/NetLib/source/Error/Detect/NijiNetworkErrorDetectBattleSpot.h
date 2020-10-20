//==============================================================================
/**
 * @file	NijiNetworkErrorDetectBattleSpot.h
 * @brief	Niji通信エラーリスナーの検知クラス
 * @author	ichiraku_katsuhiko
 * @date	2015/05/11 16:46:54
 */
// =============================================================================
#if defined(GF_PLATFORM_CTR)
#ifndef __NIJI_NETWORK_ERROR_DETECT_BATTLESPOT_H__
#define __NIJI_NETWORK_ERROR_DETECT_BATTLESPOT_H__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include "NijiNetworkErrorDetectDefault.h" // 非公開ヘッダ

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Error)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//------------------------------------------------------------------
/**
 * @brief   Niji通信エラー検知クラス
 *
 * @note    通信エラーのリスナーがある際はここに継承していく。
 *          但し、エラー以外の通知があるリスナーはその限りではない。
 */
//------------------------------------------------------------------
class NijiNetworkErrorDetectBattleSpot : public NijiNetworkErrorDetectDefault
{
  GFL_FORBID_COPY_AND_ASSIGN(NijiNetworkErrorDetectBattleSpot);
public:
  // コンストラクタ
  NijiNetworkErrorDetectBattleSpot( NetLib::Error::NijiNetworkErrorManager* errorManager );
  // デストラクタ
  virtual ~NijiNetworkErrorDetectBattleSpot();

  //=============================================================================
  /**
   *  NetGameErrorListenerのリスナー
   */
  //=============================================================================
  //------------------------------------------------------------------
  /**
   * @brief P2P切断通知
   */
  //------------------------------------------------------------------
  virtual void OnNetGameSessionLeft();

  //=============================================================================
  /**
   *  NexErrorListenerのリスナー
   */
  //=============================================================================
  //------------------------------------------------------------------
  /**
  * @brief NEX内部のエラー通知
  *
  * @param[in] result     エラー値
  */
  //------------------------------------------------------------------
  virtual void OnNexErrorHandling( const nn::nex::qResult& result );

  //=============================================================================
  /**
   *  P2pConnectionErrorListenerのリスナー
   */
  //=============================================================================
  //------------------------------------------------------------------
  /**
   * @brief P2P切断通知
   */
  //------------------------------------------------------------------
  virtual void OnP2pConnectionErrorSessionLeft();
};

GFL_NAMESPACE_END(Error)
GFL_NAMESPACE_END(NetLib)

#endif // __NIJI_NETWORK_ERROR_DETECT_BATTLESPOT_H__
#endif // GF_PLATFORM_CTR
