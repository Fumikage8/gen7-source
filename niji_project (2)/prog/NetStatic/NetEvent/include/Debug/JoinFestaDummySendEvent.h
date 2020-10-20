// ============================================================================
/*
 * @file JoinFestaDummySendEvent.h
 * @brief アトラクションでデバッグ用に他人になりすましてビーコン送信するイベント
 * @date 2016.05.09
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTA_DUMMY_SEND_EVENT_H_INCLUDE )
#define JOINFESTA_DUMMY_SEND_EVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"

#if PM_DEBUG

GFL_NAMESPACE_BEGIN(NetEvent)

class JoinFestaDummySendEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaDummySendEvent );
public:

  //--------------------------------------------------------------
  /**
   * @brief アトラクションでデバッグ用に他人になりすましてビーコン送信するイベント呼び出し
   * 
   * @param   pGameManager  ゲームマネージャ
   */
  //--------------------------------------------------------------
  static void StartEvent( GameSys::GameManager* pGameManager );

public:
  JoinFestaDummySendEvent( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaDummySendEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

public:
  static u32 m_patternMax;  // なりすましパターン

private:
  static const u32 BEACON_ROTATE_TIME   = 3000; //!< 拡張情報のローテション間隔（ms）

  u32 m_pattern;  // なりすましパターン
  u64 m_rotateTime;   //! ローテションタイム
  bool m_host;

};


GFL_NAMESPACE_END(NetEvent)

#endif // PM_DEBUG

#endif // JOINFESTA_DUMMY_SEND_EVENT_H_INCLUDE
