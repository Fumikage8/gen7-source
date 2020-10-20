#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet_NexP2pStationManager.h
 * @brief	  nn::nex::Station監視クラス
 * @author	ichiraku_katsuhiko
 * @date	2013/05/23, 17:45
 * 
 * @note  reliableパケットの値を監視し、一定時間変更がないと、切断を呼びます。
 *         ランダムマッチの切断対策用です。
 *        ※他の箇所では使用禁止です。
 */
// =============================================================================

#if !defined( __GFLNET_NEXP2PSTATIONMANAGER_H__ )
#define	__GFLNET_NEXP2PSTATIONMANAGER_H__
#pragma once

#include <nex.h>

namespace gflnet2{
namespace nex{

//----------------------------------------------------------------------
/**
 *	@brief  Station監視用クラス
 */
//----------------------------------------------------------------------
class NexP2pStationManager
{
  GFL_FORBID_COPY_AND_ASSIGN(NexP2pStationManager); //コピーコンストラクタ＋代入禁止
public:
  NexP2pStationManager( void );
  virtual ~NexP2pStationManager( void );

public:

  /**
   * @brief リセット
   */
  void Reset( void );

  /**
   * @brief ポーリングによる更新処理
   */
  bool Update( void );

private:

  nn::nex::qUnsignedInt32   m_beforeValue;    ///< 判定用値
  nn::nex::Time             m_intervalTime;   ///< 判定要時刻
  nn::nex::Time             m_disconnectTime; ///< 判定要時刻

private:

  /**
   * @brief 監視秒間隔
   */
  static const u64 TIME_INTERVAL  = 1000; // 1秒

  /**
   * @brief 切断と判断する時間
   */
  static const u64 TIME_OUT       = 7000; // 7秒

};


} // nex
} // gflnet

#endif	/* __GFLNET_NEXP2PSTATIONMANAGER_H__ */

#endif
