// ============================================================================
/*
 * @file BattleVideoCallEvent.cpp
 * @brief バトルビデオアプリ起動イベント
 * @date 2015.12.28
 * @author yuto_uchida
 */
// ============================================================================
#include "NetStatic/NetEvent/include/BattleVideoCallEvent.h"

//  nijiのインクルード
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "System/include/HeapDefine.h"
#include "GameSys/include/DllProc.h"
#include "GameSys/include/GameData.h"
#include "Battle/Regulation/include/PokeRegulation.h"
#include "PokeTool/include/PokeToolBattleParty.h"

#include "Field/FieldStatic/include/Event/EventProcessCall.h"

//  proc
#include <Battle/include/battle_proc.h>

//  ローカルと通信でprocを分ける
#include "NetApp/BattleVideoPlayer/include/BattleVideoPlayerLocalProc.h"
#include "NetApp/BattleVideoPlayer/include/BattleVideoPlayerNetProc.h"

#include "NetStatic/NetAppLib/include/UI/NetAppRegulationDrawInfo.h"

//  イベントのインクルード
#include "NetStatic/NetEvent/include/TeamSelectEvent.h"

//  リソース
#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(BattleVideoCall)

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
  void Event::StartEvent(GameSys::GameManager* gmgr, EVENT_PARAM* pEventParam )
  {
    GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
    Event*  pEvent  = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Event>(pGameEventManager);
    pEvent->SetParam( pEventParam );
  }

  void Event::InitFunc( GameSys::GameManager* pGameManager )
  {
    UCHIDA_PRINT( "Video Call Event Start!\n" );

    m_pParam->out.Clear();

    // 戦闘用BGMを鳴らす
    Sound::StartBattleBGMReq( m_pParam->in.bgmDefaultId );

    {
      btl::BATTLE_PROC_PARAM  param;
      //  バトルはヒープデバイスを使う、でないと動かない
      param.pAppDeviceMemory  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
      param.pAppMemory        = param.pAppDeviceMemory;
      param.pGameManager      = pGameManager;
      param.pSetupParam       = &m_pParam->in.btlSetupParam;

      GameSys::CallBattleProc( &param );
    }
  }

  void Event::EndFunc( GameSys::GameManager* pGameManager )
  {
    UCHIDA_PRINT( "Video Call Event End!\n" );

    //  バトル実行前のＢＧＭを再生しなおす
    {
      Sound::EndBattleBGMReq( Sound::SOUND_ITEM_ID_NONE );
    }
  }

  GameSys::GMEVENT_RESULT Event::MainFunc( GameSys::GameManager* pGameManager )
  {
    s32 seq = GameSys::GameEvent::GetSeqNo();

    if( seq == 0 )
    {
      Field::EventProcessCall::CallRoEventCall<btl::BattleProc>(pGameManager);
      ++seq;
    }
    else
    if( seq == 1 )
    {
      //  バトル終了結果
      m_pParam->out.bCompPlay = ( m_pParam->in.btlSetupParam.recPlayCompleteFlag == 1 );
      m_pParam->out.bEventEnd = true;

      return GameSys::GMEVENT_RES_FINISH;
    }

    GameSys::GameEvent::SetSeqNo( seq );

    return GameSys::GMEVENT_RES_CONTINUE;
  }

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetEvent)
