//==============================================================================
/**
 * @file   BattleSpotEvent.cpp
 * @date   2015/10/02 Fri. 19:50:19
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "System/include/HeapDefine.h"
#include  "GameSys/include/GameEventManager.h"
#include  "GameSys/include/GameProcManager.h"
#include  "GameSys/include/GameData.h"
#include  "GameSys/include/DllProc.h"
#include  "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include  "NetStatic/NetLib/Include/NijiNetworkSystem.h"
#include  "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#include  "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

/*   BattleSpot */
#include  "NetApp/BattleSpot/include/BattleSpotProcParam.h"
#include  "NetApp/BattleSpot/include/BattleSpotProc.h"
#include  "NetApp/BattleSpot/source/System/BattleSpotWork.h"
#include  "NetApp/BattleSpot/source/System/BattleSpotBuilder.h"
#include  "NetApp/BattleSpot/source/BattleSpotFrameDef.h"

#include  "../include/BattleSpotEvent.h"

#include "Sound/include/Sound.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"

#include "battle/Regulation/include/Regulation.h"

#include "Savedata/include/PokeDiarySave.h"
#include "Savedata/include/BattleSpotSave.h"
#include "PokeTool/include/PokeToolBattleParty.h"

// バトルビデオ録画イベント
#include "NetStatic/NetEvent/include/BattleVideoRecordingEvent.h"

// =============================================================================
/**/
//==============================================================================
/*  local  */
namespace {
  /**
    @brief  seqNo
  */
  enum  Sequence
  {
    SEQ_ProcRunning_BattleSpot,
    SEQ_TeamSelectReturn,
    SEQ_QRReturn,
    SEQ_BattleReturn,
    SEQ_VideoReturn,
    SEQ_ErrorDisconnect,
    SEQ_Exit,

    SEQ_MAX,
  };
}


namespace NetEvent  {
namespace BattleSpot {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     バトルスポット起動判定
  */
//------------------------------------------------------------------
static bool s_isEnterBattleSpot = false;
bool BattleSpotEvent::IsEnterBattleSpot( void )
{
  return s_isEnterBattleSpot;
}
  

//------------------------------------------------------------------
/**
  *  @func     バトルスポット切断判定
  */
//------------------------------------------------------------------
static bool s_isSessionLeft = false;
void BattleSpotEvent::SetSessionLeftFlag( bool b )
{
  s_isSessionLeft = b;
}
bool BattleSpotEvent::IsSessionLeftFlag( void )
{
  return s_isSessionLeft;
}


//------------------------------------------------------------------
/**
  *  @func     StartEvent
  *  @brief    BattleSpotEventの開始
  */
//------------------------------------------------------------------
void BattleSpotEvent::StartEvent(GameSys::GameManager* pGameManager, BattleSpotEventParam& rEventParam)
{
  BattleSpotEvent*   pEvent  = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, BattleSpotEvent>( pGameManager->GetGameEventManager() );

  pEvent->m_EventParam  = rEventParam;
}


//------------------------------------------------------------------
/**
  *  @func     BattleSpotEvent
  *  @brief    ctor
  */
//------------------------------------------------------------------
BattleSpotEvent::BattleSpotEvent( gfl2::heap::HeapBase* pHeap )
  : NetEvent::NetEventBase(pHeap)
  , m_ProcParam()
  , m_pProc(NULL)
  , m_teamSelectEventParam()
  , m_networkBattleParam()
  , m_regulationDrawInfo( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ) )
  , m_isRecRecord( false )
  , m_recParam1( 0 )
  , m_recParam2( 0 )
  , m_recParam3( 0 )
  , m_pBattleRecorderSavedata( NULL )
{
  ::std::memset( &m_qrReaderEventParam , 0 , sizeof(m_qrReaderEventParam) );
  ::std::memset( &m_teamSelectEventParam , 0 , sizeof(m_teamSelectEventParam) );
  ::std::memset( &m_networkBattleParam , 0 , sizeof(m_networkBattleParam) );
  ::std::memset( &m_ProcParam , 0 , sizeof(m_ProcParam) );
  ::std::memset( &m_battleVideoRecordingParam , 0 , sizeof(m_battleVideoRecordingParam) );

  m_ProcParam.m_pNetworkBattleParam = &m_networkBattleParam;

  // 対戦に必要な個人データ作成
  for( int i=0; i<BTL_CLIENT_NUM; i++ ){
    m_personalData[i].pPokeParty = GFL_NEW( pHeap ) pml::PokeParty( pHeap );
    m_personalData[i].pMyStatus = GFL_NEW( pHeap ) Savedata::MyStatus();
  }

  m_teamSelectEventParam.out.pPokeParty = GFL_NEW( pHeap ) pml::PokeParty( pHeap );
  m_qrReaderEventParam.pDst_Regulation = &m_regulationDrawInfo.GetRegulation();

  m_pBattleRecorderSavedata = GFL_NEW( pHeap ) ExtSavedata::BattleRecorderSaveData( pHeap , GFL_SINGLETON_INSTANCE( GameSys::GameManager ) );

  m_networkBattleParam.pBattleRecorderSaveData = m_pBattleRecorderSavedata;

  NetLib::NijiNetworkSystem::SwitchErrorDetectToBattleSpot();

  s_isEnterBattleSpot = true;
  s_isSessionLeft = false;
}



//------------------------------------------------------------------
/**
  *  @func     ~BattleSpotEvent
  *  @brief    dtor
  */
//------------------------------------------------------------------
BattleSpotEvent::~BattleSpotEvent()
{
  NetLib::NijiNetworkSystem::SwitchErrorDetectToDefault();

  GFL_SAFE_DELETE( m_pBattleRecorderSavedata );
  GFL_SAFE_DELETE( m_teamSelectEventParam.out.pPokeParty );

  // 対戦に必要な個人データ破棄
  for( int i=0; i<BTL_CLIENT_NUM; i++ ){
    GFL_DELETE m_personalData[i].pMyStatus;
    GFL_DELETE m_personalData[i].pPokeParty;
  }

  s_isEnterBattleSpot = false;
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
bool  BattleSpotEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


//------------------------------------------------------------------
/**
  *  @func    InitFunc
  *  @brief   初期化
  */
//------------------------------------------------------------------
void  BattleSpotEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  GFL_PRINT("%s:\n", __FUNCTION__);

  Sound::ChangeBGMReq( STRM_BGM_RANDOM_MATCH, Sound::BGM_FADE_VERY_FAST );

  /*  setup appparam  */
  {
    m_ProcParam.m_pRegulationDrawInfo = &m_regulationDrawInfo;
    for( int i=0; i<BTL_CLIENT_NUM; i++ )
    {
      m_ProcParam.m_pPersonalData[i] = &m_personalData[i];
    }
  }

  m_ProcParam.m_pTeamSelectEventParam = &m_teamSelectEventParam;


  /*  bootProc  */
  m_pProc = GameSys::CallBattleSpotProc( &m_ProcParam );
  SetSeqNo(SEQ_ProcRunning_BattleSpot);
}


//------------------------------------------------------------------
/**
  *  @func    EndFunc
  *  @brief   終了
  */
//------------------------------------------------------------------
void  BattleSpotEvent::EndFunc( GameSys::GameManager* pGameManager )
{
  // フィールドBGM再生
  Field::FieldSound* pFieldSound = pGameManager->GetGameData()->GetFieldSound();
  Sound::ChangeBGMReq( pFieldSound->GetFieldBGM(), Sound::BGM_FADE_FAST, Sound::BGM_FADE_FAST );

  if( m_isRecRecord )
  {
    PokeDiary::Set( PDID_COUNT_BSPOT , m_recParam1 , m_recParam2 , m_recParam3 );
  }

  GFL_PRINT("%s:\n", __FUNCTION__);
}


//------------------------------------------------------------------
/**
  *  @func    MainFunc
  *  @brief   更新
  */
//------------------------------------------------------------------
GameSys::GMEVENT_RESULT BattleSpotEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result  = GameSys::GMEVENT_RES_CONTINUE;

#ifdef GF_PLATFORM_CTR

  switch(GetSeqNo())
  {
    case  SEQ_ProcRunning_BattleSpot:
      if( !pGameManager->GetProcManager()->IsRegisterProcess(m_pProc) )
      {
        if( IsFatalError() )
        {
          // FatalError呼び出し
          NetEvent::FatalError::EventParam eventParam;
          eventParam.eFatalErrorNo = NetEvent::FatalError::FATAL_ERROR_NO_SD_CARD_NOT_RECOGNIZED;
          NetEvent::FatalError::FatalErrorEvent::StartEvent( pGameManager, eventParam );

          return GameSys::GMEVENT_RES_CONTINUE;
        }
        else if( IsSessionLeft() )
        {
          GFL_SINGLETON_INSTANCE( NetLib::Error::NijiNetworkErrorManager )->Clear();
          m_ProcParam.m_isSessionLeft = true;
        }
        else if( IsError() )
        {
          SetSeqNo(SEQ_ErrorDisconnect);
          break;
        }

        m_pProc = NULL;
        switch( m_ProcParam.nextApp )
        {
        case NetApp::BattleSpot::NEXT_APP_BATTLE:
          {
            // 呼び出しパラメータ設定
            SetupNetworkBattleParam();

            //ダイアリー登録
            {
              m_isRecRecord = true; 

              m_recParam1 = 0;
              m_recParam2 = RECORD_SINGLE;
              if( m_networkBattleParam.pRegulation->GetBattleRule() == Regulation::BATTLE_RULE_DOUBLE )
              {
                m_recParam2 = RECORD_DOUBLE;
              }
              else if( m_networkBattleParam.pRegulation->GetBattleRule() == Regulation::BATTLE_RULE_ROYAL )
              {
                m_recParam2 = RECORD_ROYAL;
              }

              m_recParam3 = RECORD_FREE;
              if( m_networkBattleParam.pRegulation->GetBattleCategory() == Regulation::BATTLE_CATEGORY_SPOT_RATE ||
                m_networkBattleParam.pRegulation->GetBattleCategory() == Regulation::BATTLE_CATEGORY_SPOT_SP
                )
              {
                m_recParam3 = RECORD_RATE;
                m_recParam1 = m_ProcParam.m_rankData.rate;
              }
              else if( m_networkBattleParam.pRegulation->GetBattleCategory() == Regulation::BATTLE_CATEGORY_LIVE_CUP ||
                m_networkBattleParam.pRegulation->GetBattleCategory() == Regulation::BATTLE_CATEGORY_NET_CUP
                )
              {
                m_recParam3 = RECORD_CUP;
              }
              else if( m_networkBattleParam.pRegulation->GetBattleCategory() == Regulation::BATTLE_CATEGORY_GROUP_CUP )
              {
                m_recParam3 = RECORD_FRIEND;
              }
           }

            if( m_ProcParam.m_isSessionLeft == false )
            {//P2P切断してたらバトルにいかない
              NetEvent::NetworkBattle::NetworkBattleEvent::StartEvent( pGameManager, &m_networkBattleParam );
            }
            else
            {
              m_ProcParam.m_isDisconnectBeforeBattle = true;
            }
            
            SetSeqNo(SEQ_BattleReturn);
          }
        break;
        case NetApp::BattleSpot::NEXT_APP_TEAM_SELECT:
          m_teamSelectEventParam.in.pRegulationDrawInfo = &m_regulationDrawInfo;
          m_teamSelectEventParam.in.bAddLocalHavePokeParty = true;
          m_teamSelectEventParam.in.tournamentType = NetApp::TeamSelect::TOURNAMENT_TYPE_NONE;
          m_teamSelectEventParam.in.bAddQRPokeParty = m_regulationDrawInfo.GetRegulation().GetBoolParam( Regulation::QR_TEAM );

          NetEvent::TeamSelect::TeamSelectEvent::StartEvent( pGameManager, &m_teamSelectEventParam );
          SetSeqNo(SEQ_TeamSelectReturn);
        break;
        case NetApp::BattleSpot::NEXT_APP_TEAM_SELECT_CUP:

          m_teamSelectEventParam.in.pRegulationDrawInfo = &m_regulationDrawInfo;
          m_teamSelectEventParam.in.bAddLocalHavePokeParty = false;
          m_teamSelectEventParam.in.bAddQRPokeParty = false;
          m_teamSelectEventParam.in.tournamentType = NetApp::TeamSelect::TOURNAMENT_TYPE_NET;

          NetEvent::TeamSelect::TeamSelectEvent::StartEvent( pGameManager, &m_teamSelectEventParam );
          SetSeqNo(SEQ_TeamSelectReturn);
          break;
        case NetApp::BattleSpot::NEXT_APP_TEAM_SELECT_FRIEND_CUP:

          m_teamSelectEventParam.in.pRegulationDrawInfo = &m_regulationDrawInfo;
          m_teamSelectEventParam.in.bAddLocalHavePokeParty = false;
          m_teamSelectEventParam.in.bAddQRPokeParty = m_regulationDrawInfo.GetRegulation().GetBoolParam( Regulation::QR_TEAM );
          m_teamSelectEventParam.in.tournamentType = NetApp::TeamSelect::TOURNAMENT_TYPE_NET;

          NetEvent::TeamSelect::TeamSelectEvent::StartEvent( pGameManager, &m_teamSelectEventParam );
          SetSeqNo(SEQ_TeamSelectReturn);
          break;
        case NetApp::BattleSpot::NEXT_APP_QR:
          m_qrReaderEventParam.mode = NetEvent::QRReader::QRReaderEventParam::MODE_FriendMatch;
          NetEvent::QRReader::QRReaderEvent::StartEvent( pGameManager, m_qrReaderEventParam );
          SetSeqNo(SEQ_QRReturn);
        break;
        case NetApp::BattleSpot::NEXT_APP_BATTLEVIDEO_SAVE:

          if( m_networkBattleParam.isIllegalBattle == 0 && m_ProcParam.m_isSessionLeft == false )
          {//不整合でなければ録画
            // バトルで使用したバトルサーバーバージョンを渡す
            m_battleVideoRecordingParam.in.appInParam.saveDataServerVersion = m_networkBattleParam.commServerVer;
            // 切断が発生していたら録画できなくする
            m_battleVideoRecordingParam.in.appInParam.bRecNG = m_networkBattleParam.isDisconnectOccur;

            m_battleVideoRecordingParam.in.appInParam.pRecorderSaveData = m_pBattleRecorderSavedata;

            BattleVideoRecording::Event::StartEvent( pGameManager, &m_battleVideoRecordingParam );
          }
          else if( m_ProcParam.m_isDisconnectBeforeBattle == true )
          {//対戦前に切断した
            m_ProcParam.nextFrame = NetApp::BattleSpot::FRAME_Welcome;
            m_ProcParam.m_isDisconnectBeforeBattle = false;
          }

          m_ProcParam.m_isSessionLeft = false;

          SetSeqNo(SEQ_VideoReturn);
          break;
        case NetApp::BattleSpot::NEXT_APP_NONE:
        default:
          SetSeqNo(SEQ_Exit);
        break;
        };
      }
      break;
    case SEQ_TeamSelectReturn:
      {
        if( IsError() )
        {
          SetSeqNo(SEQ_ErrorDisconnect);
          break;
        }

        if( m_teamSelectEventParam.out.result == NetApp::TeamSelect::EVENT_APP_RESULT_CANCEL )
        {//キャンセル
          m_ProcParam.nextFrame = NetApp::BattleSpot::FRAME_Welcome;
          m_ProcParam.m_teamSelected = false;
        }
        else
        {//選択
          //自分のを一旦0番に入れる
          m_ProcParam.m_pPersonalData[0]->pPokeParty->CopyFrom( *m_teamSelectEventParam.out.pPokeParty );
          if( m_ProcParam.nextApp == NetApp::BattleSpot::NEXT_APP_TEAM_SELECT_CUP ||
              m_ProcParam.nextApp == NetApp::BattleSpot::NEXT_APP_TEAM_SELECT_FRIEND_CUP)
          {
            m_ProcParam.m_teamSelected = true;
          }
          else
          {
            m_ProcParam.m_teamSelected = false;
          }
          

          //QRかどうか
          if( m_teamSelectEventParam.out.teamIdx == NetApp::TeamSelect::TEAM_SELECT_QR )
          {
            m_ProcParam.m_isQRTeam = true;
          }
          else
          {
            m_ProcParam.m_isQRTeam = false;
          }
        }
        
        m_pProc = GameSys::CallBattleSpotProc( &m_ProcParam );
        SetSeqNo(SEQ_ProcRunning_BattleSpot);
      }
    break;
    case SEQ_QRReturn:
      if( IsError() )
      {
        SetSeqNo(SEQ_ErrorDisconnect);
        break;
      }

      if( m_qrReaderEventParam.result == NetEvent::QRReader::RESULT_FriendMatch_Decoded )
      {
        m_ProcParam.nextFrame = NetApp::BattleSpot::FRAME_FriendMatchEntry;
        m_ProcParam.m_qrDecoded = true;
      }
      else
      {
        m_ProcParam.nextFrame = NetApp::BattleSpot::FRAME_Welcome;
      }

      m_pProc = GameSys::CallBattleSpotProc( &m_ProcParam );
      SetSeqNo(SEQ_ProcRunning_BattleSpot);
    break;
    case SEQ_BattleReturn:
      {
        if( IsSessionLeft() )
        {
          GFL_SINGLETON_INSTANCE( NetLib::Error::NijiNetworkErrorManager )->Clear();
          m_ProcParam.m_isSessionLeft = true;
        }
        else if( IsError() )
        {
          SetSeqNo(SEQ_ErrorDisconnect);
          break;
        }

        m_pProc = GameSys::CallBattleSpotProc( &m_ProcParam );
        SetSeqNo(SEQ_ProcRunning_BattleSpot);
      }
    break;
    case  SEQ_VideoReturn:
      {
        if( IsError() )
        {
          SetSeqNo(SEQ_ErrorDisconnect);
          break;
        }

        {//ポケパーティークリア
          for( int i=0; i<BTL_CLIENT_NUM; i++ )
          {
            m_ProcParam.m_pPersonalData[i]->pPokeParty->Clear();
          }

          //自分のを一旦0番に入れる
          m_ProcParam.m_pPersonalData[0]->pPokeParty->CopyFrom( *m_teamSelectEventParam.out.pPokeParty );
        }

        m_pProc = GameSys::CallBattleSpotProc( &m_ProcParam );
        SetSeqNo(SEQ_ProcRunning_BattleSpot);
      }
    break;
    case  SEQ_ErrorDisconnect:
      {
#if defined(GF_PLATFORM_CTR)
        NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
        pP2pConnectionManager->DisconnectStart( true );//エラーなので必ずスキップ
        if( pP2pConnectionManager->IsDisconnectSuccess() == false )
        {
          break;
        }
        pP2pConnectionManager->Terminate();
        SetSeqNo(SEQ_Exit);
#endif
      }/*  fallthrough  */
    case  SEQ_Exit:   /*  fallthrough  */
    default:
      // @fix GFNMCat[5754]：通信エラー発生時は、NetworkBattleEventのEndBattleBGMReqコールしてから、すぐにBattleSpotのChangeBgmが呼ばれていた。
      //                     その場合はサウンドのコマンドをかき消す条件を満たしているため、EndBattleBGMReqが処理されていなかった。
      //  対処方法：サウンドのコマンド(EndBattleBGMReq)を消化するまで待機
      if( Sound::HaveCommand() )
      {
        GFL_RELEASE_PRINT("HaveCommand Wait\n");
        break;
      }
      result  = GameSys::GMEVENT_RES_FINISH;
      break;
  }

#endif

  return  result;
}


void BattleSpotEvent::SetupNetworkBattleParam( void )
{
#ifdef GF_PLATFORM_CTR

  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
  GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();

  int myID = pP2pConnectionManager->GetNetID();

  // 個々人のデータ
  for( int i=0; i<BTL_CLIENT_NUM; i++ ) 
  {
    if( pP2pConnectionManager->GetMyStatus( i ) )
    {
      m_personalData[i].pMyStatus->CopyFrom( *pP2pConnectionManager->GetMyStatus( i ) );
    }

    m_networkBattleParam.pPersonalData[i] = &m_personalData[i];
  }
  // レギュレーション
  m_networkBattleParam.pRegulation = &m_regulationDrawInfo.GetRegulation();

  GFL_PRINT("SetupNetworkBattleParam : BattleCategory [%d]\n",m_networkBattleParam.pRegulation->GetBattleCategory());

  // 立ち位置
  m_networkBattleParam.standingPos = static_cast<BTL_CLIENT_ID>(myID);
  // BGM
  m_networkBattleParam.bgm = m_ProcParam.m_bgm;
  // レート値
  {
    // レーティングバトルか
    if( m_networkBattleParam.pRegulation->IsRateBattle() )
    {
      if( myID == 0 )
      {
        m_networkBattleParam.pPersonalData[0]->rate = m_ProcParam.m_rankData.rate;
        m_networkBattleParam.pPersonalData[1]->rate = m_ProcParam.m_opponentRate;
      }
      else
      {
        m_networkBattleParam.pPersonalData[0]->rate = m_ProcParam.m_opponentRate;
        m_networkBattleParam.pPersonalData[1]->rate = m_ProcParam.m_rankData.rate;
      }

      std::memset( &m_ProcParam.m_pglRecord , 0 , sizeof(m_ProcParam.m_pglRecord) );
      m_networkBattleParam.pglRecord = &m_ProcParam.m_pglRecord;
    }
  }
  m_networkBattleParam.pPersonalData[ myID ]->rate = m_ProcParam.m_rankData.rate;
#endif
}

bool BattleSpotEvent::IsSessionLeft( void )
{
  return GFL_SINGLETON_INSTANCE( NetLib::Error::NijiNetworkErrorManager )->IsSessionLeft();
}


} /*  namespace BattleSpot    */
} /*  namespace NetApp  */
