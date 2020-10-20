//==============================================================================
/**
 * @file   LiveEvent.cpp
 * @date   2015/07/09 Thu. 14:23:21
 * @author muto_yu
 * @brief  Live大会Event
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
/*  sound */
#include  <Sound/include/Sound.h>

#include  "System/include/GflUse.h"
#include  "System/include/HeapDefine.h"
#include  "GameSys/include/GameEventManager.h"
#include  "GameSys/include/GameProcManager.h"
#include  "GameSys/include/GameManager.h"
#include  "GameSys/include/GameData.h"
#include  "GameSys/include/DllProc.h"
#include  "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include  "NetStatic/NetEvent/include/FatalErrorEvent.h"
#include  "Battle/include/battle_proc.h"

#if PM_DEBUG
  #include  "Debug/Launcher/include/LauncherProc.h"
  #include  "Debug/StartMenu/include/StartLauncher.h"
  #include  "Debug/DebugWin/include/DebugWinSystem.h"
#endif

/*   live */
#include  "NetApp/Live/include/LiveAppParam.h"
#include  "NetApp/Live/include/LiveProc.h"
#include  "NetApp/Live/source/ApplicationSystem/LiveBuilder.h"
#include  "NetApp/Live/source/LiveFrameDef.h"

#include  "../include/LiveEvent.h"

#include "NetStatic/NetLib/include/NijiNetworkSystem.h"

// =============================================================================
/**/
//==============================================================================
/*  local  */
namespace {
  static const u32  BGMID               = STRM_BGM_RANDOM_MATCH;
  static const u32  EventLocalHeapSize  = 0x00100000;

  /**
    @brief  seqNo
  */
  enum  Sequence
  {
    SEQ_Initial,
    SEQ_ProcRunning_Live,

    SEQ_TeamSelect,
    SEQ_Battle,
    SEQ_BattleReplay,
    SEQ_QRReader,

    SEQ_Exit,
    SEQ_Exit_Finalizing,
    SEQ_End,

    /*  fatalerror  */
    SEQ_FatalError,

    SEQ_MAX,
    SEQ_DEFAULT = SEQ_Initial
  };
}


namespace NetEvent  {
namespace Live {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     StartEvent
  *  @brief    LiveEventの開始
  */
//------------------------------------------------------------------
void LiveEvent::StartEvent(GameSys::GameManager* pGameManager)
{
  GameSys::GameEventManager*  pGameEventManager = pGameManager->GetGameEventManager();

  GAMESYSTEM_EVENT_CHANGE<GameSys::GameEventManager, LiveEvent>( pGameEventManager );
}


//------------------------------------------------------------------
/**
  *  @func     LiveEvent
  *  @brief    ctor
  */
//------------------------------------------------------------------
LiveEvent::LiveEvent( gfl2::heap::HeapBase* pHeap )
  : NetEvent::NetEventBase(pHeap)
  , m_pHeapBase(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ))
  , m_pLiveNet(NULL)
  , m_pEventLocalHeapBase(NULL)
  , m_ProcParam()
  , m_pProc(NULL)
  , m_pMyBattleParty(NULL)
  /*  regulation  */
  , m_pSavedRegulation(NULL)
  , m_pReceivedRegulation(NULL)
  /*  teamselect  */
  , m_pRegulationDrawInfo(NULL)
  , m_TeamSelectParam()
  /*  networkBattle  */
  , m_NetworkBattleParam()
  /*  replay  */
  , m_pBattleRecorderData(NULL)
  , m_BattleSetupParam()
  /*  qrreader  */
  , m_QRReaderParam()
{
  memset(m_RegulationCoredata, 0, sizeof(m_RegulationCoredata));
  memset(&m_PGLRecordSet,      0, sizeof(m_PGLRecordSet));
}



//------------------------------------------------------------------
/**
  *  @func     ~LiveEvent
  *  @brief    dtor
  */
//------------------------------------------------------------------
LiveEvent::~LiveEvent()
{
}


/*
    GameEvent I/F
*/

//------------------------------------------------------------------
/**
  *  @func    BootChk
  *  @brief   起動判定
  */
//------------------------------------------------------------------
bool  LiveEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


//------------------------------------------------------------------
/**
  *  @func    InitFunc
  *  @brief   初期化
  */
//------------------------------------------------------------------
void  LiveEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  GFL_PRINT("%s:\n", __FUNCTION__);

  GameSys::GameData*        pGameData       = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  Savedata::LiveMatchData*  pLiveMatchData  = pGameData->GetLiveMatchData();


  /*  BGM  */
  Sound::StartBGMReq(BGMID);

  /*  event local heap  */
  {
    m_pEventLocalHeapBase = GFL_CREATE_LOCAL_HEAP( m_pHeapBase, EventLocalHeapSize, gfl2::heap::HEAP_TYPE_EXP, false );

    m_pLiveNet            = GFL_NEW(m_pEventLocalHeapBase) NetApp::Live::LiveNet(m_pEventLocalHeapBase);
    m_pSavedRegulation    = GFL_NEW(m_pEventLocalHeapBase) Regulation(m_pEventLocalHeapBase);
    m_pReceivedRegulation = GFL_NEW(m_pEventLocalHeapBase) Regulation(m_pEventLocalHeapBase);
    m_pMyBattleParty      = GFL_NEW(m_pEventLocalHeapBase) NetAppLib::Util::NetAppBattleParty(m_pEventLocalHeapBase);
    m_pBattleRecorderData = GFL_NEW(m_pEventLocalHeapBase) ExtSavedata::BattleRecorderSaveData(m_pEventLocalHeapBase, pGameManager);

    /*  create party  */
    if(pLiveMatchData->IsValidPartyIndex())
    {
      m_pMyBattleParty->RecreateBattleParty(pGameData->GetBOX(), pGameData->GetBoxPokemon(), pLiveMatchData->GetPartyIndex(), m_pEventLocalHeapBase);
    }

    m_pLiveNet->Initialize();
  }

  /*  makeparam  */
  {
    m_ProcParam.m_BootMode            = NetApp::Live::BOOTMODE_Top;
    m_ProcParam.m_pLiveNet            = m_pLiveNet;
    m_ProcParam.m_pLiveMatchData      = pGameData->GetLiveMatchData();
    m_ProcParam.m_pBattleRecorderData = m_pBattleRecorderData;
    m_ProcParam.m_pBattleSetupParam   = &m_BattleSetupParam;
    m_ProcParam.m_pMyBattleParty      = m_pMyBattleParty;
    m_ProcParam.m_pSavedRegulation    = m_pSavedRegulation;
    m_ProcParam.m_pReceivedRegulation = m_pReceivedRegulation;

    /*  battle用party  */
    m_NetworkBattleParam.pPersonalData[NetApp::Live::LiveNet::ClientID_1of2]              = GFL_NEW(m_pEventLocalHeapBase) NetEvent::NetworkBattle::PersonalData;
    m_NetworkBattleParam.pPersonalData[NetApp::Live::LiveNet::ClientID_1of2]->pPokeParty  = GFL_NEW(m_pEventLocalHeapBase) pml::PokeParty(m_pEventLocalHeapBase);
    m_NetworkBattleParam.pPersonalData[NetApp::Live::LiveNet::ClientID_2of2]              = GFL_NEW(m_pEventLocalHeapBase) NetEvent::NetworkBattle::PersonalData;
    m_NetworkBattleParam.pPersonalData[NetApp::Live::LiveNet::ClientID_2of2]->pPokeParty  = GFL_NEW(m_pEventLocalHeapBase) pml::PokeParty(m_pEventLocalHeapBase);

    /*  現在のSaveDataからRegulationを構築  */
    m_pSavedRegulation->Deserialize(m_ProcParam.m_pLiveMatchData->GetSerializedRegulationData(), m_ProcParam.m_pLiveMatchData->GetSerializedRegulationDataSize());
    
    /*  qrreader  */
    m_QRReaderParam.mode            = NetEvent::QRReader::QRReaderEventParam::MODE_Live;
    m_QRReaderParam.pDst_Regulation = m_pReceivedRegulation;
  }

  NetLib::NijiNetworkSystem::CreateLiveRecSender();

  SetSeqNo(SEQ_DEFAULT);
}


//------------------------------------------------------------------
/**
  *  @func    EndFunc
  *  @brief   終了
  */
//------------------------------------------------------------------
void  LiveEvent::EndFunc( GameSys::GameManager* pGameManager )
{
  NetLib::NijiNetworkSystem::DeleteLiveRecSender();

  /*  event local heap  */
  {
    m_pLiveNet->Finalize();

    /*  battle用party  */
    GFL_SAFE_DELETE(m_NetworkBattleParam.pPersonalData[NetApp::Live::LiveNet::ClientID_1of2]->pPokeParty);
    GFL_SAFE_DELETE(m_NetworkBattleParam.pPersonalData[NetApp::Live::LiveNet::ClientID_2of2]->pPokeParty);
    GFL_SAFE_DELETE(m_NetworkBattleParam.pPersonalData[NetApp::Live::LiveNet::ClientID_1of2]);
    GFL_SAFE_DELETE(m_NetworkBattleParam.pPersonalData[NetApp::Live::LiveNet::ClientID_2of2]);
  
    GFL_SAFE_DELETE(m_pLiveNet);
    GFL_SAFE_DELETE(m_pSavedRegulation);
    GFL_SAFE_DELETE(m_pReceivedRegulation);
    GFL_SAFE_DELETE(m_pMyBattleParty);
    GFL_SAFE_DELETE(m_pBattleRecorderData);
    GFL_SAFE_DELETE(m_pRegulationDrawInfo);

    gfl2::heap::Manager::DeleteHeap(m_pEventLocalHeapBase);
    m_pEventLocalHeapBase = NULL;
  }
}


//------------------------------------------------------------------
/**
  *  @func    MainFunc
  *  @brief   更新
  */
//------------------------------------------------------------------
#if 0
  enum GMEVENT_RESULT{
    GMEVENT_RES_CONTINUE = 0, ///<イベント継続中
    GMEVENT_RES_FINISH,       ///<イベント終了
    GMEVENT_RES_OFF,       ///<イベント終了中
    GMEVENT_RES_CONTINUE_DIRECT = 33,  ///<イベント継続（もう一周する）
  };
#endif
GameSys::GMEVENT_RESULT LiveEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GameData*        pGameData       = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  Savedata::LiveMatchData*  pLiveMatchData  = pGameData->GetLiveMatchData();
  GameSys::GMEVENT_RESULT   result          = GameSys::GMEVENT_RES_CONTINUE;

  if(m_pLiveNet)
  {
    m_pLiveNet->Update();
  }

  switch(GetSeqNo())
  {
    case  SEQ_Initial:
      m_pProc = GameSys::CallLiveProc(&m_ProcParam);
      SetSeqNo(SEQ_ProcRunning_Live);
      break;

    case  SEQ_ProcRunning_Live:
      if( !pGameManager->GetProcManager()->IsRegisterProcess(m_pProc) )
      {
        m_pProc = NULL;
        /*  proc結果により分岐  */
        ResultHandler();
      }
      break;

    case  SEQ_TeamSelect:
    {

      /*  チーム選択からの戻り  */
      const bool  isSelected  = (m_TeamSelectParam.out.result == NetApp::TeamSelect::EVENT_APP_RESULT_TEAM_SELECT);
      
      GFL_SAFE_DELETE(m_pRegulationDrawInfo);
      m_ProcParam.m_BootMode            = isSelected ? NetApp::Live::BOOTMODE_TeamSelect_Selected : NetApp::Live::BOOTMODE_TeamSelect_Canceled;
      m_ProcParam.m_SelectedPartyIndex  = static_cast<u16>(m_TeamSelectParam.out.teamIdx);
      
      if(isSelected)  m_pMyBattleParty->RecreateBattleParty(pGameData->GetBOX(), pGameData->GetBoxPokemon(), m_ProcParam.m_SelectedPartyIndex, m_pEventLocalHeapBase);
      else            m_pMyBattleParty->Clear();

      m_pProc                           = GameSys::CallLiveProc(&m_ProcParam);
      SetSeqNo(SEQ_ProcRunning_Live);
    }
      break;


    case  SEQ_Battle:
    {
      //一旦止める
//      NetLib::NijiNetworkSystem::SetupLiveRecSender( 0xFF , NULL );
      NetLib::NijiNetworkSystem::StopLiveRecSender();

      /*  バトルからの戻り  */
      {
        bool                                  isWin           = false;
        bool                                  isDraw          = false;
        const Savedata::MyStatus*             pMyStatus_Mine  = m_pLiveNet->GetMyStatus_Mine();
        const Savedata::MyStatus*             pMyStatus_Oppo  = m_pLiveNet->GetMyStatus_Opponent();
        pml::PokeParty                        party_Mine(m_pEventLocalHeapBase);
        pml::PokeParty                        party_Oppo(m_pEventLocalHeapBase);
        Savedata::LiveMatchData::LIVE_RECORD  liveRecord;
        
        party_Mine.DeserializeCore(&m_pLiveNet->GetBattleInfo_Mine().serializedPokeParty);
        party_Oppo.DeserializeCore(&m_pLiveNet->GetBattleInfo_Opponent().serializedPokeParty);

        
        /*
          result
          ※ORASに準拠
        */
        {
          switch( m_NetworkBattleParam.battleResult ){
          case BTL_RESULT_WIN:         ///< 勝った
          case BTL_RESULT_RUN_ENEMY:   ///< 相手が逃げた（野生のみ）
            isWin  = true;
            isDraw = false;
            break;
          case BTL_RESULT_LOSE:        ///< 負けた
          case BTL_RESULT_RUN:         ///< 逃げた
            isWin   = false;
            isDraw  = false;
            break;
          case BTL_RESULT_DRAW:        ///< ひきわけ
            isWin   = false;
            isDraw  = true;
            break;

          case  BTL_RESULT_CAPTURE:     ///< 捕まえた（野生のみ）
          case  BTL_RESULT_COMM_ERROR:  ///< 通信エラーによる
          default:
            GFL_ASSERT(0); // @check
            break;
          }

          pLiveMatchData->AddLiveResultCountUp(isDraw, isWin);
        }

        
        /*  record  */
        gfl2::std::MemClear(&liveRecord, sizeof(liveRecord));
        liveRecord.reporter_id    = pMyStatus_Mine->GetPrincipalId();
        liveRecord.opponent_id    = pMyStatus_Oppo->GetPrincipalId();
        /*  残りポケモン数  */
        liveRecord.reporter_poke  = m_NetworkBattleParam.battlePartyMembers[m_pLiveNet->GetClientID_Mine()];
        liveRecord.opponent_poke  = m_NetworkBattleParam.battlePartyMembers[m_pLiveNet->GetClientID_Opponent()];
        for(u32 index = 0; index < GFL_NELEMS(m_PGLRecordSet.myParty); ++index)
        {
          if(m_PGLRecordSet.myParty[index].deadPokeNumber != MONSNO_NULL) --liveRecord.reporter_poke;
        }
        for(u32 index = 0; index < GFL_NELEMS(m_PGLRecordSet.enemysParty); ++index)
        {
          if(m_PGLRecordSet.enemysParty[index].deadPokeNumber != MONSNO_NULL) --liveRecord.opponent_poke;
        }

        liveRecord.battle_result          = m_NetworkBattleParam.recHeader.result1;
        liveRecord.battle_result_reason   = m_NetworkBattleParam.recHeader.result2;
        liveRecord.unconformity           = m_NetworkBattleParam.isIllegalBattle;
        
        pLiveMatchData->SetLiveRecord(&liveRecord);
      }

      /*  proc  */
      m_ProcParam.m_BootMode  = NetApp::Live::BOOTMODE_BattleResult;
      m_pProc                 = GameSys::CallLiveProc(&m_ProcParam);
      SetSeqNo(SEQ_ProcRunning_Live);
    }
      break;

    case  SEQ_BattleReplay:
    {
      if(!pGameManager->IsProcessExists())
      {
        /*  battle再生終了→Live起動  */
        m_pBattleRecorderData->ClearBattleSetupParam( &m_BattleSetupParam );
        
        /*  BGM  */
        {
          Sound::EndBattleBGMReq( Sound::SOUND_ITEM_ID_NONE );
        }
        m_ProcParam.m_BootMode  = NetApp::Live::BOOTMODE_MatchMenu;
        m_pProc                 = GameSys::CallLiveProc(&m_ProcParam);
        SetSeqNo(SEQ_ProcRunning_Live);
      }
    }
      break;

    /*
      QRReader
    */
    case  SEQ_QRReader:
    {
      if(!pGameManager->IsProcessExists())
      {
        /*  QRReader終了→Live起動  */
        const bool  isRead  = (m_QRReaderParam.result == NetEvent::QRReader::RESULT_Live_Decoded);

        m_ProcParam.m_BootMode  = isRead ? NetApp::Live::BOOTMODE_CeckRegulation_FromQRRead : NetApp::Live::BOOTMODE_Top;
        m_pProc                 = GameSys::CallLiveProc(&m_ProcParam);
        SetSeqNo(SEQ_ProcRunning_Live);
      }
    }
      break;

    /*
      Fatal
    */
    case  SEQ_FatalError:
      /*  nop */
      break;

    case  SEQ_Exit:
      m_pLiveNet->Disconnect();
      SetSeqNo(SEQ_Exit_Finalizing);
      break;

    case  SEQ_Exit_Finalizing:
      if(m_pLiveNet->IsDisconnected())
      {
        m_pLiveNet->Finalize();
        GFL_SAFE_DELETE( m_pLiveNet );
        System::GflUse::SetSoftResetRequest();    //!< ソフトウェアリセット
        SetSeqNo(SEQ_End);
      }
      break;

    case  SEQ_End:
    default:
      break;
  }


  return  result;
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LiveEvent::ResultHandler(void)
{
  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager);

  switch(m_ProcParam.m_Result)
  {
    /*  チーム選択呼出し  */
    case  NetApp::Live::APPRESULT_CallTeamSelect:
      /*    */
      m_pSavedRegulation->Serialize(&m_RegulationCoredata);
      m_pRegulationDrawInfo = GFL_NEW(m_pEventLocalHeapBase) NetAppLib::UI::RegulationDrawInfo(m_pEventLocalHeapBase, &m_RegulationCoredata, sizeof(m_RegulationCoredata));

      m_TeamSelectParam.in.pRegulationDrawInfo    = m_pRegulationDrawInfo;
      m_TeamSelectParam.in.bAddLocalHavePokeParty = false;
      m_TeamSelectParam.in.bAddQRPokeParty        = false;
      m_TeamSelectParam.in.tournamentType         = NetApp::TeamSelect::TOURNAMENT_TYPE_LIVE;
      m_TeamSelectParam.out.pPokeParty            = NULL;

      NetEvent::TeamSelect::TeamSelectEvent::StartEvent(pGameManager, &m_TeamSelectParam);
      SetSeqNo(SEQ_TeamSelect);
      break;

    case  NetApp::Live::APPRESULT_CallBattle:
    {
      const BTL_CLIENT_ID                     myClientID            = m_pLiveNet->GetClientID_Mine();
      const BTL_CLIENT_ID                     opponentClientID      = m_pLiveNet->GetClientID_Opponent();
      const NetApp::Live::LiveBattleInfo&     rMyBattleInfo         = m_pLiveNet->GetBattleInfo_Mine();
      const NetApp::Live::LiveBattleInfo&     rOpponentBattleInfo   = m_pLiveNet->GetBattleInfo_Opponent();
      NetEvent::NetworkBattle::PersonalData&  rMyPersonalData       = *(m_NetworkBattleParam.pPersonalData[myClientID]);
      NetEvent::NetworkBattle::PersonalData&  rOpponentPersonalData = *(m_NetworkBattleParam.pPersonalData[opponentClientID]);

      rMyPersonalData.pPokeParty->DeserializeCore(&rMyBattleInfo.serializedPokeParty);
      rMyPersonalData.pMyStatus         = m_pLiveNet->GetMyStatus_Mine();
      rMyPersonalData.qrTeam            = false;

      rOpponentPersonalData.pPokeParty->DeserializeCore(&rOpponentBattleInfo.serializedPokeParty);
      rOpponentPersonalData.pMyStatus   = m_pLiveNet->GetMyStatus_Opponent();
      rOpponentPersonalData.qrTeam      = false;

      m_NetworkBattleParam.pRegulation              = m_pSavedRegulation;
      m_NetworkBattleParam.standingPos              = m_pLiveNet->GetClientID_Mine();
      // @fix NMCat[3459]:WCS決勝の場合に専用曲にする
      {
        if( m_pSavedRegulation->IsWcs() && (m_pSavedRegulation->GetBackGroundType() == Regulation::BACKGROUND_WCS2) )
        {
          m_NetworkBattleParam.bgm = STRM_BGM_VS_SECRET_01; // WCS決勝専用曲
        }
        else
        {
          m_NetworkBattleParam.bgm = 0;
        }
      }
      m_NetworkBattleParam.pglRecord                = &m_PGLRecordSet;
      m_NetworkBattleParam.pBattleRecorderSaveData  = m_pBattleRecorderData;

      NetEvent::NetworkBattle::NetworkBattleEvent::StartEvent(pGameManager, &m_NetworkBattleParam);
      SetSeqNo(SEQ_Battle);
    }
      break;


    case  NetApp::Live::APPRESULT_CallBattleReplay:
    {
      /*  battle  */
      btl::BATTLE_PROC_PARAM  param;

      param.pAppDeviceMemory  = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      param.pAppMemory        = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      param.pGameManager      = pGameManager;
      param.pSetupParam       = &m_BattleSetupParam;

      /*  BGM  */
      {
        Sound::StartBattleBGMReq(m_pBattleRecorderData->GetBattleRecData()->upload.body.setupSubset.battleEffect.bgm_default);
      }
      GameSys::CallBattleProc( &param );
      SetSeqNo(SEQ_BattleReplay);
    }
      break;

    /*
      QR
    */
    case  NetApp::Live::APPRESULT_CallQRReader:
    {
      m_QRReaderParam.result  = NetEvent::QRReader::RESULT_Invalid;

      NetEvent::QRReader::QRReaderEvent::StartEvent(pGameManager, m_QRReaderParam);
      SetSeqNo(SEQ_QRReader);
    }
      break;

    /*
      fatal
    */
    case  NetApp::Live::APPRESULT_Exit_Fatal:
    {
      /*  fatalEventへ遷移し、以降戻らない  */
      NetEvent::FatalError::EventParam eventParam;
      eventParam.eFatalErrorNo = NetEvent::FatalError::FATAL_ERROR_NO_SD_CARD_NOT_RECOGNIZED;
      NetEvent::FatalError::FatalErrorEvent::StartEvent( pGameManager, eventParam );

      SetSeqNo(SEQ_FatalError);
    }
      break;

    default:
      SetSeqNo(SEQ_Exit);
      break;
  }
}


} /*  namespace Live      */
} /*  namespace NetEvent  */
