#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexSessionEventListener.h
 * @brief  セッションイベント関連通知イベントハンドラ
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_NEX_SESSION_EVENT_LISTENER_H__
#define __GFLNET_NEX_SESSION_EVENT_LISTENER_H__
#pragma once

#include <nn.h>
#include <nex.h>



namespace gflnet2{
namespace nex{

//------------------------------------------------------------------
/*!
 * @brief セッション状態通知イベントハンドラ
 *
 * @note セッション状態変化イベント関連の通知を受け取る場合は、
 *       本クラスを継承し仮想関数を実装してください。
 */
//------------------------------------------------------------------
class NexSessionEventListener
{
//  GFL_FORBID_COPY_AND_ASSIGN(NexSessionEventListener); //コピーコンストラクタ＋代入禁止
public:

  //------------------------------------------------------------------
  /**
  * @brief セッションへの参加通知
  *
  * @param[in] pid       参加者のプリンシバルID
  * @param[in] sessionId 通知対象セッションID
  */
  //------------------------------------------------------------------
  virtual void OnSessionParticipate( nn::nex::PrincipalID pid, u32 sessionId ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief セッションからの退出通知
  *
  * @param[in] pid       退出者のプリンシバルID
  * @param[in] sessionId 通知対象セッションID
  */
  //------------------------------------------------------------------
  virtual void OnSessionEndParticipation( nn::nex::PrincipalID pid, u32 sessionId ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief セッションからの切断通知
  *
  * @param[in] pid       退出者のプリンシバルID
  * @param[in] sessionId 通知対象セッションID
  */
  //------------------------------------------------------------------
  virtual void OnSessionDisconnect( nn::nex::PrincipalID pid, u32 sessionId ) = 0;
};


} // nex
} // gflnet



#endif // __GFLNET_NEX_SESSION_EVENT_LISTENER_H__
#endif
