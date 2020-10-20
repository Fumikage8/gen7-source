//==============================================================================
/**
 * @file	P2pConnectionMigrationListener.h
 * @brief	P2PConnectionManagerのエラーイベントリスナー
 * @author	kousaka_shin
 * @date	2015/05/20 14:51:26
 */
// =============================================================================

#ifndef __P2P_CONNECTION_MIGRATION_LISTENER_HEADER__
#define __P2P_CONNECTION_MIGRATION_LISTENER_HEADER__
#pragma once

namespace NetLib{
namespace P2P{

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//------------------------------------------------------------------
/**
 * @brief   P2PConnectionManagerのエラー通知イベントハンドラ
 *          P2Pエラーリスナープロジェクト版です。ライブリラリ側のものプラスαがあります。
 */
//------------------------------------------------------------------
class P2pConnectionMigrationListener
{
public:


  //------------------------------------------------------------------
  /**
   * @brief マイグレーション発生通知
   */
  //------------------------------------------------------------------
  virtual void OnP2PConnectionMigrationSuccess() = 0;

};

} // namespace P2P
} // namespace NetLib

#endif // __P2P_CONNECTION_MIGRATION_LISTENER_HEADER__

