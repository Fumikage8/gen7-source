
//==============================================================================
/**
 * @file   LiveFrame_Matching.cpp
 * @date   2015/07/28 Tue. 11:21:19
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <NetStatic/NetLib/include/P2P/P2pConnectionManager.h>
#include  "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include  "../ApplicationSystem/LiveWork.h"
#include  "../LiveResource.h"

#include  "./LiveFrame_Matching.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,

    STATE_Connecting,
    STATE_CheatCheckDone,
    STATE_Disconnect,
    STATE_Disconnecting,
    STATE_ShowingInstructionMessage,
    STATE_WaitForInstruction,

    STATE_StartMatching,
    STATE_Matching,
    STATE_ExchangeBattleInfo,


    STATE_RegulationNotMatchInformation,


    STATE_Exit,


    STATE_     /*  terminator  */
  };

  enum  MatchingCheckResult
  {
    MATCHING_CHECK_RESULT_OK,
    MATCHING_CHECK_RESULT_NG,
    MATCHING_CHECK_RESULT_CheatCheck,

    MATCHING_CHECK_RESULT_
  };



  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  MatchingCheckResult  MatchingCheck(gfl2::heap::HeapBase* pHeapBase, const NetApp::Live::LiveBattleInfo& rMyLiveBattleInfo, const NetApp::Live::LiveBattleInfo& rOpponentLiveBattleInfo)
  {
    Regulation*             pRegulation_Mine  = GFL_NEW(pHeapBase) Regulation(pHeapBase);
    Regulation*             pRegulation_Oppo  = GFL_NEW(pHeapBase) Regulation(pHeapBase);
    MatchingCheckResult     result            = MATCHING_CHECK_RESULT_NG;


    pRegulation_Mine->Deserialize(&(rMyLiveBattleInfo.serializedRegulation),        sizeof(rMyLiveBattleInfo.serializedRegulation));
    pRegulation_Oppo->Deserialize(&(rOpponentLiveBattleInfo.serializedRegulation),  sizeof(rOpponentLiveBattleInfo.serializedRegulation));


    {
      const u32 competitionID_Mine  = NetApp::Live::APP_PARAM::GetCompetitionIDFromRegulation(*pRegulation_Mine);
      const u32 competitionID_Oppo  = NetApp::Live::APP_PARAM::GetCompetitionIDFromRegulation(*pRegulation_Oppo);

      GFL_PRINT("%s: compeID (mine, oppo), (%u, %u)\n", __FUNCTION__, competitionID_Mine, competitionID_Oppo);

#if PM_DEBUG
      pRegulation_Mine->DebugPrint();
      pRegulation_Oppo->DebugPrint();
#endif

      /*  ID == 0 は不可とする  */
      if(competitionID_Mine && competitionID_Oppo)
      {
        if(competitionID_Mine == competitionID_Oppo)
        {
          result  = MATCHING_CHECK_RESULT_OK;
        }
        else if(competitionID_Oppo == Savedata::LiveMatchData::CUPID_FOR_VALIDITY_CHECK_ROM)
        {
          /*  不正チェックROM  */
          result  = MATCHING_CHECK_RESULT_CheatCheck;
        }
      }
    }

    GFL_SAFE_DELETE(pRegulation_Mine);
    GFL_SAFE_DELETE(pRegulation_Oppo);

    return result;
  }
} /*  namespace  */


namespace NetApp  {
namespace Live {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     Frame_Matching
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_Matching::Frame_Matching(LiveWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_pUpperView_Main(NULL)
  , m_pLowerView_Selection(NULL)
  , m_battleSetupParam()
  , m_NetworkBattleParam()

{
  m_pUpperView_Main       = m_pViewContainer->m_pUpperView_Main;
  m_pLowerView_Selection  = m_pViewContainer->m_pLowerView_Selection;
  
  m_NetworkBattleParam.pPersonalData[NetApp::Live::LiveNet::ClientID_1of2]              = GFL_NEW(m_pWork->GetDeviceHeap()) NetEvent::NetworkBattle::PersonalData;
  m_NetworkBattleParam.pPersonalData[NetApp::Live::LiveNet::ClientID_1of2]->pPokeParty  = GFL_NEW(m_pWork->GetDeviceHeap()) pml::PokeParty( m_pWork->GetDeviceHeap() );
  m_NetworkBattleParam.pPersonalData[NetApp::Live::LiveNet::ClientID_2of2]              = GFL_NEW(m_pWork->GetDeviceHeap()) NetEvent::NetworkBattle::PersonalData;
  m_NetworkBattleParam.pPersonalData[NetApp::Live::LiveNet::ClientID_2of2]->pPokeParty  = GFL_NEW(m_pWork->GetDeviceHeap()) pml::PokeParty( m_pWork->GetDeviceHeap() );
}


//------------------------------------------------------------------
/**
  *  @func     Frame_Matching
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_Matching::~Frame_Matching()
{
  GFL_SAFE_DELETE(m_NetworkBattleParam.pPersonalData[NetApp::Live::LiveNet::ClientID_1of2]->pPokeParty);
  GFL_SAFE_DELETE(m_NetworkBattleParam.pPersonalData[NetApp::Live::LiveNet::ClientID_2of2]->pPokeParty);
  GFL_SAFE_DELETE(m_NetworkBattleParam.pPersonalData[NetApp::Live::LiveNet::ClientID_1of2]);
  GFL_SAFE_DELETE(m_NetworkBattleParam.pPersonalData[NetApp::Live::LiveNet::ClientID_2of2]);

  // バトルセットアップ初期化で確保していたワークを解放
  BATTLE_SETUP_Clear( &m_battleSetupParam );

  //一旦止める
//  NetLib::NijiNetworkSystem::SetupLiveRecSender( 0xFF , NULL );
  NetLib::NijiNetworkSystem::StopLiveRecSender();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  Frame_Matching::startup()
{
  m_pViewContainer->SetScene(LiveViewContainer::SCENE_Matching);
  m_pUpperView_Main->SetViewMode(UpperView_Main::VIEWMODE_TournamentName);
  m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Matching_Searching);

  return true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Matching::setListener()
{
  m_pWork->m_pAppParam->m_pLiveNet->SetEventHandler(this);
  m_pLowerView_Selection->SetEventHandler(this);
  m_pViewContainer->SetAllInputEnable(true);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Matching::removeListener()
{
  m_pWork->m_pAppParam->m_pLiveNet->SetEventHandler(NULL);
  m_pLowerView_Selection->SetEventHandler(NULL);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Matching::updateSequence()
{
  m_pLowerView_Selection->SetAButtonEnable(false);
  m_pLowerView_Selection->SetBButtonEnable(false);
  switch(m_State)
  {
    case  STATE_Entry:
      m_pWork->m_pAppParam->m_pLiveNet->Connect();
      m_State = STATE_Connecting;
      /*  fallthrough  */

    case  STATE_Connecting:
    {
      if(m_pWork->m_pAppParam->m_pLiveNet->IsConnected())
      {
        m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Matching_Connecting);   /*  通信しています  */
        m_State = STATE_StartMatching;
      }
      else
      {
        m_pLowerView_Selection->SetBButtonEnable(true);
      }
    }
      break;


    case  STATE_CheatCheckDone:
      break;


    case  STATE_Disconnect:
      m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Matching_Cancel);        /*  通信を中断しています  */
      m_pWork->m_pAppParam->m_pLiveNet->Disconnect();
      m_State = STATE_Disconnecting;
      /*  fallthrough  */

    case  STATE_Disconnecting:
    {
      if(m_pWork->m_pAppParam->m_pLiveNet->IsDisconnected())
      {
        ExitFrame();
      }
    }
      break;

    case  STATE_ShowingInstructionMessage:
      break;

    case  STATE_WaitForInstruction:
      m_pLowerView_Selection->SetAButtonEnable(true);
      break;

    case  STATE_StartMatching:
      m_State = STATE_Matching;
      break;

    case  STATE_Matching:
    {
      APP_PARAM*  pAppParam = m_pWork->m_pAppParam;
      LiveNet*    pLiveNet  = pAppParam->m_pLiveNet;

      pLiveNet->SetupMyBattleInfo(*(pAppParam->m_pSavedRegulation), *(pAppParam->m_pMyBattleParty->GetPokeParty()));
      pLiveNet->ExchangeBattleInfo();
      m_State = STATE_ExchangeBattleInfo;
    }
      break;

    case  STATE_ExchangeBattleInfo:
      break;

    case  STATE_RegulationNotMatchInformation:
      break;


    case  STATE_Exit:
      /*
        workにFrameResultを設定することでSwitcherが呼ばれる
      */
      m_pWork->SetFrameResult( m_Result );
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  Frame_Matching::cleanup()
{
  return true;
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
/*  implement LowerView_Selection::IEventHandler  */
void  Frame_Matching::LowerView_Selection_OnEvent(const EventCode eventCode)
{
  switch(eventCode)
  {
    /*
    */
    case  LowerView_Selection::IEventHandler::EVENT_MessageFinish:
    {
      if(m_State == STATE_CheatCheckDone)
      {
        m_State = STATE_Disconnect;
      }
      else if(m_State == STATE_ShowingInstructionMessage)
      {
        m_State = STATE_WaitForInstruction;
      }
      else if(m_State == STATE_RegulationNotMatchInformation)
      {
        m_State = STATE_Disconnect;
      }
    }
      break;

    case  LowerView_Selection::IEventHandler::EVENT_PushA:
      m_pLowerView_Selection->SetAButtonEnable(false);
      if(m_State == STATE_WaitForInstruction)
      {
        m_pWork->m_pAppParam->m_Result  = APPRESULT_CallBattle;

        //m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Matching_Matching);
        Sound::PlaySE(SEID_DECIDE);
        ExitFrame(RESULT_MATCHING_Success);
      }
      break;

    case  LowerView_Selection::IEventHandler::EVENT_Matching_ConfirmCancel_Yes:
      m_State = STATE_Disconnect;
      break;

    case  LowerView_Selection::IEventHandler::EVENT_Matching_ConfirmCancel_No:
      /*  nop  */
      break;

    case  LowerView_Selection::IEventHandler::EVENT_Exit:
      m_State = STATE_Disconnect;
      break;

    default:
      break;
  }

}
/*  endimplement LowerView_Selection::IEventHandler  */


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Matching::OnReceive_LiveBattleInfo( LiveNet* pSender, const LiveBattleInfo& rMyLiveBattleInfo, const LiveBattleInfo& rOpponentLiveBattleInfo)
{
//  ExitFrame(RESULT_MATCHING_Success);

GFL_PRINT("%s: received battleinfos\n", __FUNCTION__);


  switch(MatchingCheck(m_pWork->GetDeviceHeap()->GetLowerHandle(), rMyLiveBattleInfo, rOpponentLiveBattleInfo))
  {
    case  MATCHING_CHECK_RESULT_OK:
      /*  OK  */
      m_pUpperView_Main->SetViewMode(UpperView_Main::VIEWMODE_TournamentNameWithPlayerInfo);
      m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Matching_WaitingInstruction);
      m_State = STATE_ShowingInstructionMessage;

      /*  観戦用BattleSetupParamの送信  */
      {
        bool  bDoSendBattleSetupParam = m_pWork->m_pAppParam->m_pSavedRegulation->GetBoolParam( Regulation::WATCH );

#if PM_DEBUG
        if(*NetEvent::NetworkBattle::NetworkBattleEvent::GetDbgLiveRecSendFlagPrt())
        {
          /*  デバッグ時：Regulation.watchににかかわらず強制送信  */
          bDoSendBattleSetupParam = true;
        }
#endif

        if(bDoSendBattleSetupParam)
        {
          SendLiveRecInitParam();
        }
      }
      break;

    case  MATCHING_CHECK_RESULT_CheatCheck:
      /*  不正チェックROM  */
      m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Matching_SendDataToCheatCheckROM);
      m_State = STATE_CheatCheckDone;
      break;

    case  MATCHING_CHECK_RESULT_NG:
    default:
      /*  NG  */
      m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Matching_RegulationNotMatched);
      m_State = STATE_RegulationNotMatchInformation;
      break;
  }
}





//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_Matching::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
  m_State   = STATE_Exit;
  m_pViewContainer->SetAllInputEnable(false);
}


void  Frame_Matching::SendLiveRecInitParam( void )
{
  /*
    ×
    BattleVideoは常に
      BTL_CLIENT_PLAYER : 自分
      BTL_CLIENT_ENEMY1 ; 相手
    である

    ○
    コマンドが逆転するのでClientIDで揃える
  */


  const BTL_CLIENT_ID                     myClientID            = m_pWork->m_pAppParam->m_pLiveNet->GetClientID_Mine();
  const BTL_CLIENT_ID                     opponentClientID      = m_pWork->m_pAppParam->m_pLiveNet->GetClientID_Opponent();
  const NetApp::Live::LiveBattleInfo&     rMyBattleInfo         = m_pWork->m_pAppParam->m_pLiveNet->GetBattleInfo_Mine();
  const NetApp::Live::LiveBattleInfo&     rOpponentBattleInfo   = m_pWork->m_pAppParam->m_pLiveNet->GetBattleInfo_Opponent();
  NetEvent::NetworkBattle::PersonalData&  rMyPersonalData       = *(m_NetworkBattleParam.pPersonalData[myClientID]);
  NetEvent::NetworkBattle::PersonalData&  rOpponentPersonalData = *(m_NetworkBattleParam.pPersonalData[opponentClientID]);


  rMyPersonalData.pPokeParty->DeserializeCore(&rMyBattleInfo.serializedPokeParty);
  rMyPersonalData.pMyStatus         = m_pWork->m_pAppParam->m_pLiveNet->GetMyStatus_Mine();
  rMyPersonalData.qrTeam            = false;

  rOpponentPersonalData.pPokeParty->DeserializeCore(&rOpponentBattleInfo.serializedPokeParty);
  rOpponentPersonalData.pMyStatus   = m_pWork->m_pAppParam->m_pLiveNet->GetMyStatus_Opponent();
  rOpponentPersonalData.qrTeam      = false;

  gfl2::std::MemClear( &m_battleSetupParam, sizeof(BATTLE_SETUP_PARAM) );

  BTL_FIELD_SITUATION bfs;
  BATTLE_SETUP_FIELD_SITUATION_Init( &bfs );
  bfs.player_rotate = 0;

  // 通信タイプ
  BtlCommMode commMode = BTL_COMM_IR;

  // 通信対戦セットアップ
  BATTLE_SETUP_Comm(
    &m_battleSetupParam, 
    GFL_SINGLETON_INSTANCE( GameSys::GameManager ),
    &bfs,
    commMode,
    BTL_CLIENT_PLAYER,
    m_pWork->GetDeviceHeap()->GetLowerHandle()
  );

  // 設定したポケパーティをセット
  BATTLE_PARAM_SetPokeParty( &m_battleSetupParam, rMyPersonalData.pPokeParty,       myClientID);
  BATTLE_PARAM_SetPokeParty( &m_battleSetupParam, rOpponentPersonalData.pPokeParty, opponentClientID);


  // レギュレーション通りのバトル設定にする
  BATTLE_PARAM_SetRegulation( &m_battleSetupParam, m_pWork->m_pAppParam->m_pSavedRegulation, m_pWork->GetDeviceHeap()->GetLowerHandle() );

  //お手入れ禁止
  BATTLE_PARAM_SetBtlStatusFlag( &m_battleSetupParam, BTL_STATUS_FLAG_OTEIRE_DISABLE );

  //プレイヤーステータス設定
  //@fix [#5638 【観戦機能】観戦ONのとき、送信される情報の中に、対戦相手のプレイヤーの性別、着せ替え情報が正しく反映されていません]
  if(myClientID == BTL_CLIENT_PLAYER)
  {
    m_battleSetupParam.playerStatus[ BTL_CLIENT_PLAYER ] = rMyPersonalData.pMyStatus;
    m_battleSetupParam.playerStatus[ BTL_CLIENT_ENEMY1 ]->CopyFrom( *rOpponentPersonalData.pMyStatus );
  }
  else
  {
    m_battleSetupParam.playerStatus[ BTL_CLIENT_PLAYER ] = rOpponentPersonalData.pMyStatus;
    m_battleSetupParam.playerStatus[ BTL_CLIENT_ENEMY1 ]->CopyFrom( *rMyPersonalData.pMyStatus );
  }


  // 見せ合いデータ(niji新規)
  //SetupMiseaiData();

  //@fix[fix]NMCat[4203]:[#4203 【観戦機能】対戦側と観戦側で別のBGMが再生される]
  NetLib::NijiNetworkSystem::SetRegulation( m_pWork->m_pAppParam->m_pSavedRegulation );
  NetLib::NijiNetworkSystem::SetupLiveRecSender( myClientID, &m_battleSetupParam );
  NetLib::NijiNetworkSystem::ResetLiveRecSender();
}


} /*  namespace Live    */
} /*  namespace NetApp  */
