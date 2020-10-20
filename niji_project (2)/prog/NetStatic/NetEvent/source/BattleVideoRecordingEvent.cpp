// ============================================================================
/*
 * @file BattleVideoRecordingEvent.cpp
 * @brief バトルビデオ録画アプリ起動イベント
 * @date 2015.09.15
 * @author yuto_uchida
 */
// ============================================================================
#include "NetStatic/NetEvent/include/BattleVideoRecordingEvent.h"

//  nijiのインクルード
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "System/include/HeapDefine.h"
#include "GameSys/include/DllProc.h"
#include "GameSys/include/GameData.h"

#include "NetApp/BattleVideoRecording/include/BattleVideoRecordingEventAppParam.h"

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(BattleVideoRecording)

  // コンストラクタ
  Event::Event( gfl2::heap::HeapBase* pHeap ) : GameSys::GameEvent( pHeap )
  {
    _Clear();
  }

  // デストラクタ
  Event::~Event()
  {
  }

  //  イベント開始
  void Event::StartEvent(GameSys::GameManager* gmgr, EVENT_APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );

    GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
    Event*  pEvent  = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Event>(pGameEventManager);
    pEvent->SetAppParam( pAppParam );
  }

  void Event::InitFunc( GameSys::GameManager* pGameManager )
  {
    UCHIDA_PRINT( "Video Rec Event Start!\n" );

    //  Proc作成
    {
      //  イベントに設定しているアプリ入力パラメータをアプリパラメータに転送
      gfl2::std::MemCopy( &m_pEventAppParam->in.appInParam, &m_appParam.in, sizeof( m_appParam.in ) );
    }

    GameSys::GameManager* gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    NetApp::BattleVideoRecording::Proc* pProc = 
      NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::BattleVideoRecording::Proc, NetApp::BattleVideoRecording::APP_PARAM>( gameManager->GetProcManager(), &m_appParam, gameManager->GetAsyncFileManager() , "BattleVideoRecording.cro" );

    GFL_ASSERT( pProc );

    m_pProc = pProc;
  }

  void Event::EndFunc( GameSys::GameManager* pGameManager )
  {
    UCHIDA_PRINT( "Video Rec Event End!\n" );
  }

  GameSys::GMEVENT_RESULT Event::MainFunc( GameSys::GameManager* pGameManager )
  {
    enum
    {
      SEQ_PROCT_WAIT  = 0,
      SEQ_ERROR_NET_WAIT,
      SEQ_END
    };

    GFL_ASSERT( m_pEventAppParam );

    s32 seq = GameSys::GameEvent::GetSeqNo();
    if( seq == SEQ_PROCT_WAIT )
    {
      //  procが死んでいるかチェック
      if( pGameManager->GetProcManager()->IsRegisterProcess( m_pProc ) == false )
      {
        if( m_appParam.out.procResultType == NetApp::BattleVideoRecording::PROC_RESULT_TYPE_FATAL )
        {
          //  フェイタル
          m_fatalEventParam.eFatalErrorNo = NetEvent::FatalError::FATAL_ERROR_NO_SD_CARD_NOT_RECOGNIZED;
          NetEvent::FatalError::FatalErrorEvent::StartEvent( pGameManager, m_fatalEventParam );          

          seq = SEQ_ERROR_NET_WAIT;
        }
        else
        {
          seq = SEQ_END;
        }
        
      }
    }
    else
    if( seq == SEQ_ERROR_NET_WAIT )
    {
      seq = SEQ_END;
    }

    if( seq == SEQ_END )
    {
      return GameSys::GMEVENT_RES_FINISH;
    }

    GameSys::GameEvent::SetSeqNo( seq );

    return GameSys::GMEVENT_RES_CONTINUE;
  }

GFL_NAMESPACE_END(BattleVideoRecording)
GFL_NAMESPACE_END(NetEvent)
