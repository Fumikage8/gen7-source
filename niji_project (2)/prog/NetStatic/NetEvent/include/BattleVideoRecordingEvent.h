// ============================================================================
/*
 * @file BattleVideoRecordingEvent.h
 * @brief バトルビデオ録画アプリ起動イベント
 * @date 2015.09.15
 * @author yuto_uchida
 */
// ============================================================================
#if !defined( BATTLE_VIDEO_RECORDING_EVENT_H_INCLUDE )
#define BATTLE_VIDEO_RECORDING_EVENT_H_INCLUDE
#pragma once

//  niji
#include <macro/include/gfl2_Macros.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"

#include "NetApp/BattleVideoRecording/include/BattleVideoRecordingEventAppParam.h"
#include "NetApp/BattleVideoRecording/include/BattleVideoRecordingProc.h"
#include "NetStatic/NetEvent/include/FatalErrorEvent.h"

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(BattleVideoRecording)

  //----------------------------------------------------------------------
  /**
   *  @brief  バトルビデオ録画アプリ起動クラス
   */
  //----------------------------------------------------------------------
  class Event : public GameSys::GameEvent
  {
    GFL_FORBID_COPY_AND_ASSIGN( Event );
  public:
    //  イベント開始
    static void StartEvent(GameSys::GameManager* gmgr, EVENT_APP_PARAM* pAppParam );

    // コンストラクタ
    Event( gfl2::heap::HeapBase* pHeap );

    // デストラクタ
    virtual~Event();

    virtual bool BootChk( GameSys::GameManager* pGameManager ) { return true; }
    virtual void InitFunc( GameSys::GameManager* pGameManager );
    virtual void EndFunc( GameSys::GameManager* pGameManager );
    virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  private:
    //  アプリ用のパラメータを設定
    void SetAppParam( EVENT_APP_PARAM* appParam )
    {
      GFL_ASSERT( appParam );
      m_pEventAppParam = appParam;
    }

    void  _Clear()
    {
      m_pProc           = NULL;
      m_pEventAppParam  = NULL;
    }

    NetApp::BattleVideoRecording::Proc*             m_pProc;
    NetApp::BattleVideoRecording::APP_PARAM         m_appParam;
    EVENT_APP_PARAM*                                m_pEventAppParam;

    NetEvent::FatalError::EventParam                m_fatalEventParam;
  };

GFL_NAMESPACE_END(BattleVideoRecording)
GFL_NAMESPACE_END(NetEvent)

#endif // BATTLE_VIDEO_RECORDING_EVENT_H_INCLUDE
