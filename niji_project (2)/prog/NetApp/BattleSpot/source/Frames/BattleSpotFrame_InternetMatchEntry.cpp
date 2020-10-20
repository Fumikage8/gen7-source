//==============================================================================
/**
 * @file   BattleSpotFrame_InternetMatchEntry.cpp
 * @date   2015/10/02 Fri. 13:31:23
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include  "../System/BattleSpotWork.h"

#include  "../BattleSpotResource.h"
#include  "../Views/BattleSpotViewContainer.h"

#include "NetStatic/NetAppLib/include/GameSync/GameSyncRequestFacade.h"
#include  "./BattleSpotFrame_InternetMatchEntry.h"
#include  "GameSys/include/GameData.h"
#include <Savedata/include/GameSyncSave.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include "PokeTool/include/PokeToolBattleParty.h"

#include <gflnet2/include/boss/gflnet2_BossManager.h>
#include <gflnet2/include/util/gflnet2_Base64Utility.h>
#include "NetStatic/NetAppLib/include/Util/NetAppBossUtility.h"

#include "NetStatic/NetAppLib/include/Util/NetAppPackFile.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,

    STATE_Selection,

    STATE_Join,
    STATE_PGLResult,
    STATE_MessageWait,
    STATE_MessageWaitDownloaded,
    STATE_RegulationCheckFor234,
    STATE_RegulationCheck,
    STATE_RegNotFound,
    STATE_NextEntryWork2,
    STATE_BattleBoxUpload,
    STATE_NextEntryWork3,
    STATE_GameSync,
    STATE_GameSyncWait,
    STATE_NextEntryWork4,
    STATE_Retire,
    STATE_RetireUnlock,
    STATE_UpdateEntryFlg,
    STATE_UnlockSave,
    STATE_RetireDateIsOver,
    STATE_Exit,

    STATE_     /*  terminator  */
  };
} /*  namespace  */


namespace NetApp      {
namespace BattleSpot  {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     Frame_InternetMatchEntry
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_InternetMatchEntry::Frame_InternetMatchEntry(BattleSpotWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->m_pViewContainer)
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_pUpperView(NULL)
  , m_pLowerView(NULL)
  , m_pglState(0)
  , m_wait(0)
#if defined(GF_PLATFORM_CTR)
  , m_pglConnector(NULL)
#endif
  , m_nextWifiMatchupState(0)
  , m_nextGpfEntryFlg(0)
  , m_updateStatusSeq(0)
  , m_teamLockRequest(false)
  , m_regSeq(0)
  , m_SerialID(0)
  , m_communityID(0)
  , m_wifiMatchUpID(0)
  , m_firstPglCheckDone(false)
  , m_pEncodeBuffer(NULL)
{
#if defined(GF_PLATFORM_CTR)

  GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->SetAppHeap( m_pWork->GetDeviceHeap() );
  GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->AddEventListener( this );

  m_pMatchMakeClient = NULL;

  m_pglConnector = GFL_NEW( m_pWork->GetDeviceHeap() ) NetLib::Pgl::PGLConnector();

  GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  Savedata::MyStatus* myst = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatus();
  gflnet2::nex::NexManager* nexMgr = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);

  m_pglConnector->Initialize( m_pWork->GetDeviceHeap(), myst->GetNexUniqueID() , nexMgr->GetAuthenticationToken(), static_cast<nn::cfg::CTR::CfgRegionCode>( myst->GetRegionCode() ) );
  m_pglConnector->SetPGLConnectorListener( this );

  if( game_data->GetBattleSpotData()->GetInternetLockTeamIndex() != 0xFF )
  {
    m_pWork->m_pAppParam->m_pTeamSelectEventParam->out.teamIdx = game_data->GetBattleSpotData()->GetInternetLockTeamIndex();//View用にインデックスを入れておく
  }
#endif
}


//------------------------------------------------------------------
/**
  *  @func     Frame_InternetMatchEntry
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_InternetMatchEntry::~Frame_InternetMatchEntry()
{
#if defined(GF_PLATFORM_CTR)
  GflHeapSafeFreeMemory(m_pEncodeBuffer);

  if( m_pglConnector )
  {
    m_pglConnector->RemovePGLConnectorListener();
    m_pglConnector->Finalize();
    GFL_SAFE_DELETE( m_pglConnector );
  }

  GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->RemoveEventListener( this );
  GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->RemoveAppHeap();
#endif
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  Frame_InternetMatchEntry::startup()
{
#if defined(GF_PLATFORM_CTR)
  m_pViewContainer->SetAllInputEnabled( true );
  GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  u32 lockIndex = pGameData->GetBattleSpotData()->GetInternetLockTeamIndex();
  //大会チームをロード
  if( lockIndex != 0xFF && pGameData->GetBOX()->IsTeamLock( lockIndex ) )
  {
    PokeTool::BattleParty tempParty( m_pWork->GetDeviceHeap()->GetLowerHandle() );
    tempParty.CreateBattleParty( pGameData->GetBOX() , pGameData->GetBoxPokemon() , lockIndex , m_pWork->GetDeviceHeap()->GetLowerHandle() );
    m_pWork->m_pAppParam->m_pPersonalData[0]->pPokeParty->CopyFrom( *tempParty.GetPokeParty() );//0番にロック済みパーティーを入れる
    m_pWork->m_pAppParam->m_pTeamSelectEventParam->out.pPokeParty->CopyFrom( *tempParty.GetPokeParty() );

    m_pWork->m_pViewContainer->m_pLowerView_Card->ResetPokeParty();
  }

  m_pViewContainer->SetScene(BattleSpotViewContainer::SCENE_InternetMatchEntry);

  /*  views  */
  {
    m_pUpperView  = m_pViewContainer->m_pUpperView_Information;
    m_pUpperView->SetViewMode( UpperView_Information::VIEWMODE_InternetMatch );

    m_pLowerView  = m_pViewContainer->m_pLowerView_Selection;

    if( m_pWork->m_retireRequest == true )
    {//リタイアシーケンス
      m_pWork->m_retireRequest = false;

      m_nextGpfEntryFlg = m_pWork->m_gpfEntryFlg;
      m_nextWifiMatchupState = ENTRY_END;
      m_State = STATE_RegNotFound;
    }
    else if( m_pWork->m_pAppParam->m_teamSelected == false )
    {//通常
      m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().DeleteData();
    }
    else
    {//チーム選択戻り
      m_firstPglCheckDone = true;
      m_pWork->m_pAppParam->m_teamSelected = false;

      //時間確認
      //開催時間チェック
      gflnet2::nex::NexManager* nexMgr = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
      nn::nex::DateTime dateTime;
      if( nexMgr->GetNgsFacade()->GetGameServerTime( &dateTime ) == false )
      {//エラー
        ExitSelf();
        return true;
      }

      GFL_PRINT("nn::nex::DateTime[%d][%d][%d][%d]\n" , dateTime.GetYear(),dateTime.GetMonth(),dateTime.GetDay(),dateTime.GetHour() );

      if( *(m_pWork->GetCommunityEndDateTime( BattleSpotWork::COMMUNITY_INTERNET_CUP_END )) < dateTime )
      {//終了済み
        GFL_PRINT("終了済み\n");
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_Connecting);
        m_nextGpfEntryFlg = m_pWork->m_gpfEntryFlg;
        m_nextWifiMatchupState = ENTRY_END;
        m_State = STATE_RetireDateIsOver;
      }
      else if( *(m_pWork->GetCommunityEndDateTime( BattleSpotWork::COMMUNITY_INTERNET_CUP_START )) > dateTime )
      {//開催前
        GFL_PRINT("開催前\n");
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_TournamentNotStarted);
        m_State = STATE_MessageWait;
      }
      else
      {//開催中
        GFL_PRINT("開催中\n");
        m_teamLockRequest = true;
        m_nextGpfEntryFlg = m_pWork->m_gpfEntryFlg;
        m_nextWifiMatchupState = ENTRY_BATTLEBOX;
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_Connecting);

        m_State = STATE_BattleBoxUpload;

        {
          GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
          Savedata::MyStatus    *sv_mystatus = game_data->GetPlayerStatus();

          // 自分のプリンシパルID
          u32 pid = sv_mystatus->GetPrincipalId();

          // セーブするデータをセット
          Savedata::BattleSpotData::ENTRY_COMMUNITY entry_community;
          entry_community.community_id = m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::DISCRIMINANT_ID );
          entry_community.principalId = pid;

          // セーブにデータセット
          m_pWork->m_pBattleSpotSave->SetEntryCommunity( entry_community, Savedata::BattleSpotData::ENTRY_COMMUNITY_NET );
        }

        //BOXアップロード
        u32 partydataSize   = ( pml::pokepara::CoreParam::DATASIZE * pml::PokeParty::MAX_MEMBERS ) + sizeof(u32);
        u8* pSerializeBuffer = GFL_NEW_ARRAY( m_pWork->GetDeviceHeap()->GetLowerHandle() ) u8[partydataSize];

        u32 pokeNum = m_pWork->m_pAppParam->m_pPersonalData[0]->pPokeParty->GetMemberCount();
        u8* pSerializePtr = pSerializeBuffer;
        for( u32 index = 0 ; index < pokeNum ; ++index )
        {
          m_pWork->m_pAppParam->m_pPersonalData[0]->pPokeParty->GetMemberPointer( index )->Serialize_Core( pSerializePtr );
          pSerializePtr += pml::pokepara::CoreParam::DATASIZE;
        }

        u32* lockFlagPtr = reinterpret_cast<u32*>( &pSerializeBuffer[pml::pokepara::CoreParam::DATASIZE * pml::PokeParty::MAX_MEMBERS] );
        *lockFlagPtr = 1;//ロックフラグ kujira/sango時代とフォーマットを合わせる為

        u32 partydataEncodeSize = gflnet2::util::Base64Utility::GetNeedEncodeBufferSize( partydataSize );

        m_pEncodeBuffer   = GflHeapAllocMemory( m_pWork->GetDeviceHeap()->GetLowerHandle(), partydataEncodeSize );

        bool encodeResult = gflnet2::util::Base64Utility::Encode( m_pEncodeBuffer, pSerializeBuffer,     partydataSize );


        GFL_PRINT("battle box crc[0x%x]\n",gfl2::math::Crc::Crc16( 0, reinterpret_cast<const u8*>(pSerializeBuffer), partydataSize ) );


        GFL_SAFE_DELETE_ARRAY( pSerializeBuffer );

        GFL_PRINT("battle box size[%d][%d][%d]\n",partydataSize,partydataEncodeSize,encodeResult);

        bool ret = m_pglConnector->UploadBattleBox( static_cast<char*>( m_pEncodeBuffer ) , partydataSize , NetLib::Pgl::PGLConnector::BATTLE_TYPE_INTERNET );
        if( ret == false )
        {//エラー
          ExitSelf();
        }
      }
    }

    
  }

  m_pMatchMakeClient = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexMatchMakingClient);
  if( m_pMatchMakeClient )
  {
    m_pMatchMakeClient->SetCommunityListener( this );
    if( !m_pMatchMakeClient->Initialize(GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->GetNgsFacade()) )
    {//失敗
      GFL_PRINT("ProcUpdateRatingbattle::InitFunc m_pMatchMakeClient->Initialize failed\n");
      GFL_ASSERT(0);//@fix
      m_State = STATE_Exit;
    }
  }

#endif

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
void  Frame_InternetMatchEntry::setListener()
{
//  m_pUpperView->SetEventHandler(this);
  m_pLowerView->SetEventHandler(this);
  m_pViewContainer->m_pLowerView_Card->SetEventHandler(this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_InternetMatchEntry::removeListener()
{
//  m_pUpperView->SetEventHandler(NULL);
  m_pLowerView->SetEventHandler(NULL);
  m_pViewContainer->m_pLowerView_Card->SetEventHandler(NULL);
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_InternetMatchEntry::updateSequence()
{
  switch(m_State)
  {
    case  STATE_Entry:
      {
        UpdatePGLCheck();
      }
      break;
    case  STATE_Selection:
      break;
    case STATE_Join:
      UpdatePGLCheck();
      break;
    case STATE_PGLResult:
      {
        GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
        if( game_data->GetSaveData()->GetGameSync()->IsSignedPGL() )
        {//登録OK

          if( m_firstPglCheckDone )
          {//参加する/対戦する後のフロー

            //PGLエントリーワークチェック
            if( m_pWork->m_gpfEntryFlg == 1 )
            {
              //@fix [#3993 前回参加した大会の対戦回数が表示される]　対戦回数クリア処理を追加
              //大会参加時に必ず最初に1回通る場所なのでここでクリア
              m_pWork->m_pBattleSpotSave->SetInternetMatchCount( 0 );//対戦回数初期化

              m_nextGpfEntryFlg = 0;
              m_nextWifiMatchupState = ENTRY_FINISH;
              m_State = STATE_UpdateEntryFlg;
              break;
            }

            //ロックが残っていたら解除しなければいけないパターン
            switch( m_pWork->m_pBattleSpotSave->GetInternetEntryWork() )
            {
            case ENTRY_NOT:
            case ENTRY_RETIRE:
            case ENTRY_END:
            case ENTRY_CANCEL:
              if( m_pWork->m_pBattleSpotSave->GetInternetLockTeamIndex() != 0xFF )
              {//ロックされている
                //バトルボックスロック解除
                GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
                u8 lockIndex = pGameData->GetBattleSpotData()->GetInternetLockTeamIndex();
                if( lockIndex != 0xFF )
                {
                  pGameData->GetBOX()->SetTeamLock( lockIndex , false );
                }

                pGameData->GetBattleSpotData()->SetInternetLockTeamIndex( Savedata::BattleSpotData::INVALID_LOCK_INDEX );
                UpdateSave();
                m_State = STATE_UnlockSave;
                return;
              }
            break;
            }

            //ロックが解除されていたらリタイヤしないといけないパターン
            switch( m_pWork->m_pBattleSpotSave->GetInternetEntryWork() )
            {
            case ENTRY_BATTLEBOX:
            case ENTRY_UPLOAD:
              if( m_pWork->m_pBattleSpotSave->GetInternetLockTeamIndex() == 0xFF )
              {//ロックされていない
                m_nextGpfEntryFlg = m_pWork->m_gpfEntryFlg;
                m_nextWifiMatchupState = ENTRY_RETIRE;
                m_State = STATE_RetireUnlock;
                return;
              }
              break;
            }

            //ワークによる分岐
            switch( m_pWork->m_pBattleSpotSave->GetInternetEntryWork() )
            {
            case ENTRY_NOT://         = 0,      ///< 未エントリー
            case ENTRY_RETIRE://      = 6,      ///< リタイア
              m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_NoTournament);
              m_State =  STATE_MessageWait;
              break;
            case ENTRY_FINISH://      = 1,      ///< エントリー完了
              m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_DownloadingCard);
              m_regSeq = 0;
              m_State =  STATE_RegulationCheck;
              break;
            case ENTRY_REGULATION://  = 2,      ///< レギュレーション取得済み
              if( m_pWork->m_pBattleSpotSave->GetInternetLockTeamIndex() != 0xFF )
              {//すでにロックされている
                {//チーム選択戻りと同じフロー
                  m_nextGpfEntryFlg = m_pWork->m_gpfEntryFlg;
                  m_nextWifiMatchupState = ENTRY_BATTLEBOX;
                  //m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_Connecting);
                  m_State = STATE_NextEntryWork3;
                }
              }
              else
              {//ロックされていない
                m_pWork->m_pAppParam->nextApp = NEXT_APP_TEAM_SELECT_CUP;
                m_pWork->m_pAppParam->nextFrame = FRAME_InternetMatchEntry;
                ExitSelf(RESULT_InternetMatchEntry_TeamSelect);
              }
              break;
            case ENTRY_BATTLEBOX://   = 3,      ///< バトルボックス登録完了
              m_nextGpfEntryFlg = m_pWork->m_gpfEntryFlg;
              m_nextWifiMatchupState = ENTRY_UPLOAD;
              m_State = STATE_GameSync;
              break;
            case ENTRY_UPLOAD://      = 4,      ///< 全セーブデータアップ完了
              ExitSelf(RESULT_InternetMatchEntry_Next);
              break;
            case ENTRY_END://         = 5,      ///< 大会終了
              m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_TournamentIsOver);
              m_State =  STATE_MessageWait;
              break;
            case ENTRY_CANCEL://      = 7       ///< 参加解除（失格など）
              m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_CannotEntry);
              m_State =  STATE_MessageWait;
              break;
            }
          }
          else
          {//インターネット大会最初のフロー
            m_firstPglCheckDone = true;
            switch( m_pWork->m_pBattleSpotSave->GetInternetEntryWork() )
            {
            case ENTRY_NOT://         = 0,      ///< 未エントリー
            case ENTRY_RETIRE://      = 6,      ///< リタイア
            case ENTRY_FINISH://      = 1,      ///< エントリー完了
            case ENTRY_END://         = 5,      ///< 大会終了
            case ENTRY_CANCEL://      = 7       ///< 参加解除（失格など）
              SetViewModeForWork();
              m_State = STATE_Selection;
              break;
            case ENTRY_REGULATION://  = 2,      ///< レギュレーション取得済み
            case ENTRY_BATTLEBOX://   = 3,      ///< バトルボックス登録完了
            case ENTRY_UPLOAD://      = 4,      ///< 全セーブデータアップ完了
              m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_DownloadingCard);
              m_regSeq = 0;
              m_State =  STATE_RegulationCheckFor234;
              break;
            }
          }
        }
        else
        {//未登録
          m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_HowToEntry);
          m_State =  STATE_MessageWait;
        }
      }
      break;
    case  STATE_MessageWait:
    case  STATE_MessageWaitDownloaded:
      break;
    case STATE_RegulationCheckFor234:
      if( UpdateRegulation() )
      {
        SetViewModeForWork();
        m_State = STATE_Selection;
      }
      break;
    case  STATE_RegulationCheck:
      if( UpdateRegulation() )
      {
        m_nextGpfEntryFlg = m_pWork->m_gpfEntryFlg;
        m_nextWifiMatchupState = ENTRY_REGULATION;
        m_State = STATE_NextEntryWork2;
      }
      break;
    case STATE_RegNotFound:
      if( UpdateWorldBattleStatus() )
      {
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_TournamentIsOverUnlock);//終了したので解除しました
        m_State =  STATE_MessageWait;
      }
      break;
    case STATE_NextEntryWork2:
      if( UpdateWorldBattleStatus() )
      {
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_DownloadedCard);
        m_State =  STATE_MessageWaitDownloaded;
      }
      break;
    case STATE_BattleBoxUpload:
      {

      }
    break;
    case STATE_NextEntryWork3:
      if( UpdateWorldBattleStatus() )
      {
        m_nextGpfEntryFlg = m_pWork->m_gpfEntryFlg;
        m_nextWifiMatchupState = ENTRY_UPLOAD;
        m_State = STATE_GameSync;
      }
      break;
    case STATE_GameSync:
      {
        NetAppLib::GameSync::GAMESYNC_PARAM param;
        std::memset( &param , 0 , sizeof(param) );
        param.pWork = m_pWork;
        param.pTrainerIconUtility = m_pWork->m_pTrainerIconUtility;
        param.pReplacePane = m_pViewContainer->m_pUpperView_Information->GetIconPane();

        NetAppLib::GameSync::GameSyncRequestFacade::AddSaveDataSyncRequest( this, &param );
        m_State = STATE_GameSyncWait;
      }
      break;
    case STATE_GameSyncWait:
      break;
    case STATE_NextEntryWork4:
      if( UpdateWorldBattleStatus() )
      {
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_GameSync);
        m_State =  STATE_MessageWaitDownloaded;
      }
      break;
    case STATE_UnlockSave:
      if( UpdateSave() )
      {
        m_State = STATE_PGLResult;
      }
      break;
    case STATE_Retire:
      {
        if( UpdateWorldBattleStatus() )
        {
          m_pLowerView->SetViewMode( LowerView_Selection::VIEWMODE_Message_Unregistered );
          m_State = STATE_MessageWait;
        }
      }
      break;
    case STATE_RetireUnlock:
      {
        if( UpdateWorldBattleStatus() )
        {
          m_pLowerView->SetViewMode( LowerView_Selection::VIEWMODE_Message_Retired );
          m_State = STATE_MessageWait;
        }
      }
      break;
    case STATE_UpdateEntryFlg:
      if( UpdateWorldBattleStatus() )
      {
        m_State = STATE_PGLResult;
      }
    break;
    case STATE_RetireDateIsOver:
      if( UpdateWorldBattleStatus() )
      {
        m_pglState = PGL_INIT;
        m_State = STATE_Join;
      }
    break;
    case  STATE_Exit:
      exitFrame(m_Result);
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  Frame_InternetMatchEntry::cleanup()
{
  bool  bAllDone = true;
#if defined(GF_PLATFORM_CTR)
  if( m_pglConnector )
  {
    m_pglConnector->CancelConnecting();
    if( m_pglConnector->IsConnecting() )
    {
      bAllDone = false;
    }
    else
    {
      m_pglConnector->RemovePGLConnectorListener();
      m_pglConnector->Finalize();
      GFL_SAFE_DELETE( m_pglConnector );
    }
  }

  if( m_pMatchMakeClient )
  {
    if( m_pMatchMakeClient->IsConnecting() )
    {
      m_pMatchMakeClient->CancelConnecting();

      return false;
    }
    m_pMatchMakeClient->SetCommunityListener( NULL );
    m_pMatchMakeClient->Finalize();
    m_pMatchMakeClient = NULL;
  }

  if( GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->IsRunningThread() == true )
  {
    GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->CancelImmidiateTask();
    bAllDone = false;
  }

#endif
  return  bAllDone;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
/*  implement LowerView_Selection::IEventHandler  */
void  Frame_InternetMatchEntry::LowerView_Selection_OnEvent(LowerView_Selection* pSender, const LowerView_Selection::IEventHandler::EventCode eventCode)
{
  GFL_PRINT("LowerView_Selection_OnEvent:[%d]\n",eventCode);

  switch(eventCode)
  {
  case  LowerView_Selection::IEventHandler::EVENT_Back:             
    ExitSelf();
    break;
  case  LowerView_Selection::IEventHandler::EVENT_InternetMatch_Entry:/**<  モード選択：大会に参加する  */
    m_pglState = PGL_INIT;
    m_State = STATE_Join;
    break;
  case  LowerView_Selection::IEventHandler::EVENT_InternetMatch_Match: /**<  モード選択：対戦する        */
    {
      m_pglState = PGL_INIT;
      m_State = STATE_Join;
    }
    break;
  case  LowerView_Selection::IEventHandler::EVENT_InternetMatch_WatchCard:/**<  モード選択：選手証を見る    */
    {
      m_pViewContainer->SetScene(BattleSpotViewContainer::SCENE_InternetMatch_Matching);
      
      if( m_pWork->m_pBattleSpotSave->GetInternetLockTeamIndex() != 0xFF )
      {//ロック済み
        m_pViewContainer->m_pLowerView_Card->SetViewMode(LowerView_Card::VIEWMODE_InternetMatch_TeamCard);
      }
      else
      {//ロック前
        m_pViewContainer->m_pLowerView_Card->SetViewMode(LowerView_Card::VIEWMODE_InternetMatch_Card);
      }

      m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_InternetMatch_Card);
    }
    break;
  case  LowerView_Selection::IEventHandler::EVENT_InternetMatch_PreviousTournament:/**<  モード選択：前回大会        */
    {
      m_pViewContainer->SetScene(BattleSpotViewContainer::SCENE_InternetMatch_Matching);

      m_pViewContainer->m_pLowerView_Card->SetViewMode(LowerView_Card::VIEWMODE_InternetMatch_Card);
      m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_InternetMatch_Card);
    }
    break;
  case  LowerView_Selection::IEventHandler::EVENT_InternetMatch_Retire:/**<  モード選択：参加解除        */
    {
      m_pLowerView->SetViewMode( LowerView_Selection::VIEWMODE_Message_RetireConfirm );
    }
    break;      
  case  LowerView_Selection::IEventHandler::EVENT_Retire1_Yes:                        /**<  参加解除する？：はい    */
    {
      m_pLowerView->SetViewMode( LowerView_Selection::VIEWMODE_Message_RetireConfirm2 );
    }
  break;
  case  LowerView_Selection::IEventHandler::EVENT_Retire1_No:                         /**<  参加解除する？：いいえ  */
    {
      SetViewModeForWork();
    }
  break;
  case  LowerView_Selection::IEventHandler::EVENT_Retire2_Yes:                        /**<  ほんとうに参加解除する？：はい    */
    {
      {
        m_nextGpfEntryFlg = m_pWork->m_gpfEntryFlg;
        m_nextWifiMatchupState = ENTRY_RETIRE;
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_Connecting);
        m_State = STATE_Retire;
      }
    }
  break;
  case  LowerView_Selection::IEventHandler::EVENT_Retire2_No:                         /**<  ほんとうに参加解除する？：いいえ  */
    {      
      {
        SetViewModeForWork();
      }
    }
  break;
  case LowerView_Selection::IEventHandler::EVENT_Message_EOM:
    {
      switch( m_State )
      {
      case STATE_MessageWait:
        {
          ExitSelf(RESULT_InternetMatchEntry_Back);
        }
        break;
      case STATE_MessageWaitDownloaded:
        m_pglState = PGL_INIT;
        m_State = STATE_Join;
        break;
      }
    }
  break;
  }
}

void  Frame_InternetMatchEntry::LowerView_Card_OnEvent(LowerView_Card* pSender, const LowerView_Card::IEventHandler::EventCode eventCode)
{
  GFL_PRINT("LowerView_Card_OnEvent:[%d]\n",eventCode);
  switch(eventCode)
  {
  case  LowerView_Card::IEventHandler::EVENT_Back:
    if( m_pViewContainer->m_pLowerView_Card->GetViewMode() == LowerView_Card::VIEWMODE_InternetMatch_TournamentInfo )
    {
      if( m_pWork->m_pBattleSpotSave->GetInternetLockTeamIndex() != 0xFF )
      {//ロック済み
        m_pViewContainer->m_pLowerView_Card->SetViewMode(LowerView_Card::VIEWMODE_InternetMatch_TeamCard);
      }
      else
      {//ロック前
        m_pViewContainer->m_pLowerView_Card->SetViewMode(LowerView_Card::VIEWMODE_InternetMatch_Card);
      }

      m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_InternetMatch_Card);
    }
    else
    {
      m_pViewContainer->SetScene(BattleSpotViewContainer::SCENE_InternetMatchEntry);

      m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_InternetMatch);
    }
    break;
  case LowerView_Card::IEventHandler::EVENT_Info:
    {
      m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_InternetMatch_Information);

      m_pViewContainer->m_pLowerView_Card->SetViewMode(LowerView_Card::VIEWMODE_InternetMatch_TournamentInfo);
    }
    break;
  case  LowerView_Card::IEventHandler::EVENT_Message_EOM:
    break;
  case  LowerView_Card::IEventHandler::EVENT_Message_Finish: 
    break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_InternetMatchEntry::ExitSelf(const FrameResult frameResult)
{
  m_pViewContainer->SetAllInputEnabled( false );
  m_Result  = frameResult;
  m_State   = STATE_Exit;
}

//------------------------------------------------------------------
/**
  *  @func     ワークに応じてView設定 
  */
//------------------------------------------------------------------
void Frame_InternetMatchEntry::SetViewModeForWork( void )
{
  switch( m_pWork->m_pBattleSpotSave->GetInternetEntryWork() )
  {
  case ENTRY_NOT://         = 0,      ///< 未エントリー
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_InternetMatch_Work0);
    break;
  case ENTRY_FINISH://      = 1,      ///< エントリー完了
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_InternetMatch_Work1);
    break;
  case ENTRY_REGULATION://  = 2,      ///< レギュレーション取得済み
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_InternetMatch_Work2);
    break;
  case ENTRY_BATTLEBOX://   = 3,      ///< バトルボックス登録完了
    if( m_pWork->m_pBattleSpotSave->GetInternetLockTeamIndex() != 0xFF )
    {//ロックされている
      m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_InternetMatch_Work3_TeamLocked);
    }
    else
    {
      m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_InternetMatch_Work3_TeamUnlocked);
    }
    break;
  case ENTRY_UPLOAD://      = 4,      ///< 全セーブデータアップ完了
    if( m_pWork->m_pBattleSpotSave->GetInternetLockTeamIndex() != 0xFF )
    {//ロックされている
      m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_InternetMatch_Work4_TeamLocked);
    }
    else
    {
      m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_InternetMatch_Work4_TeamUnlocked);
    }
    break;
  case ENTRY_END://         = 5,      ///< 大会終了
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_InternetMatch_Work5);
    break;
  case ENTRY_RETIRE://      = 6,      ///< リタイア
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_InternetMatch_Work6);
    break;
  case ENTRY_CANCEL://      = 7       ///< 参加解除（失格など）
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_InternetMatch_Work7);
    break;
  }
}



void Frame_InternetMatchEntry::UpdatePGLCheck( void )
{
  if( m_wait == 0 )
  {
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_Checking);
  }

  if( m_wait++ < DUMMY_WAIT )
  {
    return;
  }

  switch( m_pglState )
  {
  case PGL_INIT:
    {
      NetAppLib::GameSync::GETPLAYSTATUS_PARAM param;
      std::memset( &param , 0 , sizeof(param) );
      param.pWork = m_pWork;
      param.mode = NetAppLib::GameSync::GETPLAYSTATUS_MODE_CUP;

      NetAppLib::GameSync::GameSyncRequestFacade::AddConfirmPlayStatusRequest( this, &param );
      m_pglState = PGL_WAIT;
    }
    break;
  case PGL_WAIT:
    break;
  case PGL_GET_WORLD_STATUS:
#if defined(GF_PLATFORM_CTR)
    m_pglConnector->GetWorldBattleStatus( NetLib::Pgl::PGLConnector::BATTLE_TYPE_INTERNET );
#endif
    m_pglState = PGL_BATTLESTATUS_WAIT;
    break;
  case PGL_BATTLESTATUS_WAIT:
    break;
  }

  return;
}

void Frame_InternetMatchEntry::OnConfirmPlayStatusSuccess()
{
  GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  if( game_data->GetSaveData()->GetGameSync()->IsSignedPGL() )
  {//サインインずみ
    m_pglState = PGL_GET_WORLD_STATUS;
  }
  else
  {//サインインしてない
    m_wait = 0;
    m_State = STATE_PGLResult;
  }
}

void Frame_InternetMatchEntry::OnConfirmPlayStatusError( u32 errorCode )
{
#if defined(GF_PLATFORM_CTR)
  // @fix [#4309 PGLがサービス終了する際に、インターネット大会、仲間大会でロックしていたバトルチームがそのままにロック状態となる]
  if( errorCode == NetLib::Pgl::E_RET_SERVICE_END )
  {
    // PGLサービス終了
    if( m_pWork->m_pBattleSpotSave->GetInternetLockTeamIndex() != 0xFF )
    {//ロックされている
      //バトルボックスロック解除
      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
      u8 lockIndex = pGameData->GetBattleSpotData()->GetInternetLockTeamIndex();
      if( lockIndex != 0xFF )
      {
        pGameData->GetBOX()->SetTeamLock( lockIndex , false );
      }

      pGameData->GetBattleSpotData()->SetInternetLockTeamIndex( Savedata::BattleSpotData::INVALID_LOCK_INDEX );
      UpdateSave();
    }
  }
#endif
  m_wait = 0;
  ExitSelf();
}

void Frame_InternetMatchEntry::OnSaveDataSyncSuccess( void )
{
  GFL_PRINT("Frame_InternetMatchEntry::OnSaveDataSyncSuccess\n");
  m_State = STATE_NextEntryWork4;
}

void Frame_InternetMatchEntry::OnSaveDataSyncError( u32 errorCode )
{
  ExitSelf();
}

bool Frame_InternetMatchEntry::UpdateRegulation( void )
{
#if defined(GF_PLATFORM_CTR)

  switch( m_regSeq )
  {
  case REGSEQ_INIT:
    {
      char attr1[ NetAppLib::Util::BossUtility::ATTRIBUTE_BUFFER_SIZE ];
      char attr2[ NetAppLib::Util::BossUtility::ATTRIBUTE_BUFFER_SIZE ];
      NetAppLib::Util::BossUtility::CreateXYBossAttribute1( NetAppLib::Util::BossUtility::BOSS_APP_TYPE_cup, attr1 );
      ::std::snprintf( attr2 , NetAppLib::Util::BossUtility::ATTRIBUTE_BUFFER_SIZE , "%u", m_wifiMatchUpID );

      // 検査条件
      gflnet2::boss::BossManager::BOSS_SEARCH_ATTRIBUTES attributes;
      attributes.pAttribute1 = attr1;  // 属性1    認証ＩＤ_リージョンＩＤ
      attributes.pAttribute2 = attr2;  // 属性2    大会ID

      bool result = GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->StartDownloadFileList( gfl2::thread::Thread::THREAD_PRI_NORMAL + 1, NULL , &attributes );

      if( result == true )
      {
        m_regSeq = REGSEQ_WAIT;
      }
      else
      {
        m_regSeq = REGSEQ_ERROR;
      }
    }
    break;
  case REGSEQ_WAIT:
  case REGSEQ_LEAVE_COMMUNITY_WAIT:
  case REGSEQ_JOIN_COMMUNITY_WAIT:

    break;
  case REGSEQ_ERROR:
    {
      m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_ConnectionError);
      m_State = STATE_MessageWait;
    }
    break;
  case REGSEQ_LEAVE_COMMUNITY:
    {
      u32 netCommunityID = m_pWork->m_pBattleSpotSave->GetEntryCommunityId( Savedata::BattleSpotData::ENTRY_COMMUNITY_NET );
      if( netCommunityID != 0 )
      {//古いコミュニティから脱退
        GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
        if( !m_pMatchMakeClient->FindCommunityByParticipant( game_data->GetPlayerStatus()->GetPrincipalId() , nn::nex::ResultRange() ) )
        {
          GFL_PRINT("FindCommunityByParticipant is failed[%u]\n",game_data->GetPlayerStatus()->GetPrincipalId());
          m_regSeq = REGSEQ_ERROR;
        }
        else
        {
          m_regSeq = REGSEQ_LEAVE_COMMUNITY_WAIT;
        }
      }
      else
      {//
        m_regSeq = REGSEQ_JOIN_COMMUNITY;
      }
    }
    break;
  case REGSEQ_JOIN_COMMUNITY:
    {
      const int COMMUNITY_SEARCH_NUM = 100;
      if( !m_pMatchMakeClient->FindOfficialCommunity( false , nn::nex::ResultRange( 0, COMMUNITY_SEARCH_NUM ) ) )
      {//エラー
        GFL_PRINT("ProcUpdateRatingbattle::UpdateFunc : FindOfficialCommunity is failed\n");
        m_regSeq = REGSEQ_ERROR;
      }
      else
      {
        m_regSeq = REGSEQ_JOIN_COMMUNITY_WAIT;
      }
    }
    break;
  case REGSEQ_OK:
    return true;
  }

#endif

  return false;
}

bool Frame_InternetMatchEntry::UpdateWorldBattleStatus( void )
{
#if defined(GF_PLATFORM_CTR)

  enum
  {
    UPDATE_STATUS_INIT = 0,
    UPDATE_STATUS_SAVEWAIT,
    UPDATE_STATUS_UPDATEWAIT,
  };

  switch( m_updateStatusSeq )
  {
  case UPDATE_STATUS_INIT:
    if( m_teamLockRequest )
    {//バトルボックスロック
      m_teamLockRequest = false;
      u32 lockIndex = m_pWork->m_pAppParam->m_pTeamSelectEventParam->out.teamIdx;
      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
      pGameData->GetBOX()->SetTeamLock( lockIndex , true );
      m_pWork->m_pBattleSpotSave->SetInternetLockTeamIndex( lockIndex );
      m_pWork->m_pBattleSpotSave->SetInternetMatchCount( 0 );//対戦回数初期化
    }

    //バトルボックスロック解除
    if( m_nextWifiMatchupState == ENTRY_RETIRE || m_nextWifiMatchupState == ENTRY_END || m_nextWifiMatchupState == ENTRY_FINISH )
    {
      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
      u8 lockIndex = pGameData->GetBattleSpotData()->GetInternetLockTeamIndex();
      if( lockIndex != 0xFF )
      {
        pGameData->GetBOX()->SetTeamLock( lockIndex , false );
      }

      pGameData->GetBattleSpotData()->SetInternetLockTeamIndex( Savedata::BattleSpotData::INVALID_LOCK_INDEX );
    }

    m_pWork->m_pBattleSpotSave->SetInternetEntryWork( m_nextWifiMatchupState );
    UpdateSave();
    m_updateStatusSeq = UPDATE_STATUS_SAVEWAIT;
  break;
  case UPDATE_STATUS_SAVEWAIT:
    if( UpdateSave() )
    {
      m_pglConnector->UpdateWorldBattleStatus( m_nextGpfEntryFlg , m_nextWifiMatchupState , NetLib::Pgl::PGLConnector::BATTLE_TYPE_INTERNET );
      m_updateStatusSeq = UPDATE_STATUS_UPDATEWAIT;
    }
  break;
  case UPDATE_STATUS_UPDATEWAIT:
    if( m_wait++ < DUMMY_WAIT )
    {
      break;
    }

    if( m_nextGpfEntryFlg == m_pWork->m_gpfEntryFlg &&
      m_nextWifiMatchupState == m_pWork->m_internetTempEntryWork )
    {
      m_updateStatusSeq = 0;
      m_wait = 0;
      return true;
    }
  break;
  }

#endif
  return false;
}

//------------------------------------------------------------------
/**
* @brief PGLバトルエントリー確認通信の完了イベントを検知
*
* @param[in]  wifiMatchUpID     大会ID
* @param[in]  gpfEntryFlg       PGLエントリーワーク（更新通信に必要）
* @param[in]  wifiMatchupState  大会の進行情報（更新通信に必要）
* @param[in]  isSignIn          連携サイトへの登録有無（true:登録済み）
* @param[in]  timeZone          タイムゾーンオフセット
*/
//------------------------------------------------------------------
void Frame_InternetMatchEntry::OnPGLGetWorldBattleStatusSuccess( u32 wifiMatchUpID, u16 gpfEntryFlg, u16 wifiMatchupState, bool isSignIn, s8 timezone )
{
  GFL_PRINT("ProcPglStateCheck::OnPGLGetWorldBattleStatusSuccess\n");
  GFL_PRINT("大会ID %d\n", wifiMatchUpID );
  GFL_PRINT("PGLエントリーワーク %d \n", gpfEntryFlg );
  GFL_PRINT("大会参加ワーク %d \n", wifiMatchupState );
  GFL_PRINT("登録有無 %d \n", isSignIn );
  GFL_PRINT("タイムゾーン %d \n", timezone );

  GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();

  m_pWork->m_pBattleSpotSave->SetInternetEntryWork( wifiMatchupState );
  m_pWork->m_pBattleSpotSave->SetInternetTimeZone( timezone );

  m_pWork->m_gpfEntryFlg = gpfEntryFlg;

  game_data->GetSaveData()->GetGameSync()->SetSignedPGL( isSignIn );

  m_wait = 0;

  m_wifiMatchUpID = wifiMatchUpID;

  m_State = STATE_PGLResult;
}


//------------------------------------------------------------------
/**
* @brief PGLバトルエントリー状態更新通信の完了イベントを検知
*/
//------------------------------------------------------------------
void Frame_InternetMatchEntry::OnPGLUpdateWorldBattleStatusSuccess( void )
{
  GFL_PRINT("OnPGLUpdateWorldBattleStatusSuccess[%d][%d]\n",m_nextGpfEntryFlg,m_nextWifiMatchupState);
  m_pWork->m_gpfEntryFlg = m_nextGpfEntryFlg;
  m_pWork->m_internetTempEntryWork = m_nextWifiMatchupState;
}

//------------------------------------------------------------------
/**
* @brief PGLバトルボックス登録通信の完了イベントを検知
*/
//------------------------------------------------------------------
void Frame_InternetMatchEntry::OnPGLUploadBattleBoxSuccess( void )
{
  GFL_PRINT("Frame_InternetMatchEntry::OnPGLUploadBattleBoxSuccess\n");

  GflHeapSafeFreeMemory(m_pEncodeBuffer);

  m_State = STATE_NextEntryWork3;
}

#if defined(GF_PLATFORM_CTR)

//------------------------------------------------------------------
/**
* @brief PGL通信のエラーイベントを検知
*
* @note  HTTP通信のステータスコードが200番台外の場合のエラー通知
*
* @param[in] commandType 完了したPGL通信コマンド
* @param[in] httpStatus サーバから返却されたHTTPステータスコード
*/
//------------------------------------------------------------------
void Frame_InternetMatchEntry::OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const s32 httpStatus )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultPGL5xx( httpStatus );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  ExitSelf();
}

//------------------------------------------------------------------
/**
* @brief PGL通信のレスポンスデータのエラーコードを検知
*
* @param[in]  commandType  完了したPGL通信コマンド
* @param[in]  resultCode   PGLサーバから受信したレスポンスエラーコード
*/
//------------------------------------------------------------------
void Frame_InternetMatchEntry::OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const NetLib::Pgl::PGL_COMMON_RESULT resultCode )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultPGL( resultCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  ExitSelf();
}

//------------------------------------------------------------------
/**
* @brief PGL通信のエラーイベントを検知
*
* @param[in]  commandType  完了したPGL通信コマンド
* @param[in]  result       エラー内容
* @param[in]  errorCode    ネットワークエラーコード
*/
//------------------------------------------------------------------
void Frame_InternetMatchEntry::OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const nn::nex::qResult &result, u32 errorCode )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  ExitSelf();
}


void Frame_InternetMatchEntry::OnBossNewArrivalData( const u32 serialId , const bool isFinalData )
{
  if( m_SerialID != 0 )
  {
    return;
  }

  u32 serialMax = NetAppLib::Util::BossUtility::GetSerialIdMax( NetAppLib::Util::BossUtility::BOSS_APP_TYPE_cup );
  u32 serialMin = NetAppLib::Util::BossUtility::GetSerialIdMin( NetAppLib::Util::BossUtility::BOSS_APP_TYPE_cup );

  if( serialMax < serialId || serialId < serialMin )
  {
    if( isFinalData )
    {
      GFL_ASSERT(0);//@fix
      m_regSeq = REGSEQ_ERROR;
    }
  }
  else
  {
    bool bResult = GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->RequestDataBody( serialId, gfl2::thread::Thread::THREAD_PRI_NORMAL + 1 );

    if( !bResult )
    {
      m_regSeq = REGSEQ_ERROR;
    }
    else
    {
      m_SerialID = serialId;
    }
  }
}


void Frame_InternetMatchEntry::OnBossNewArrivalDataEmpty( void )
{
  m_regSeq = REGSEQ_ERROR;
}


bool Frame_InternetMatchEntry::OnBossReadData( const nn::boss::NsDataHeaderInfo* pHeader , const size_t bodySize , const u8* pBody )
{
  if( pHeader )
  {
    if( pHeader->serialId == m_SerialID )
    {
      NetAppLib::Util::PackFile packFile( pBody );

      bool bIsBrokenRegulationPackFileResult = packFile.IsBrokenRegulationPackFile( REGULATION_NUM, REGULATION_NUM, bodySize );

      if( bIsBrokenRegulationPackFileResult )
      {
        GFL_ASSERT( NULL );
        m_regSeq = REGSEQ_ERROR;
        return true;
      }

      u32 fileCount = packFile.GetFileCount();

      GFL_PRINT("Frame_InternetMatchEntry::OnBossReadData fileCount[%d]\n",fileCount);

      if( fileCount != REGULATION_NUM )
      {
        GFL_ASSERT( NULL );
        m_regSeq = REGSEQ_ERROR;
        return true;
      }

      for( u32 i = 0; i < fileCount; ++i )
      {
        const void* pBuffer = packFile.GetFileBuffer(i);
        u32 fileSize = packFile.GetFileSize(i);

#if PM_DEBUG
        {
          Regulation regulation( m_pWork->GetDeviceHeap()->GetLowerHandle() );
          regulation.SetData( pBuffer, fileSize );
          regulation.DebugPrint();
        }
#endif // PM_DEBUG

        m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().SetData( pBuffer , fileSize );
      }



      // 受信レギュレーションCRCチェック
      if( m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().CheckCrc() == false )
      {//CRC不正
        m_regSeq = REGSEQ_ERROR;
        return true;
      }

      m_communityID = m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::DISCRIMINANT_ID );


      {//参加
        m_regSeq = REGSEQ_LEAVE_COMMUNITY;
      }
      
    }
    else
    {
      m_regSeq = REGSEQ_ERROR;
    }
  }
  else
  {
    m_regSeq = REGSEQ_ERROR;
  }

  return true;
}


void Frame_InternetMatchEntry::OnBossRecvFileList( const char* pTaskName , const nn::boss::NsaList::NsaInformation* pFileList , const u32 fileNum )
{
  if( fileNum == 0 && m_State == STATE_RegulationCheckFor234 )
  {//継続中のレギュレーションがない
    GFL_PRINT("OnBossRecvFileList : boss server file not found\n");

    m_nextGpfEntryFlg = m_pWork->m_gpfEntryFlg;
    m_nextWifiMatchupState = ENTRY_END;
    m_State = STATE_RegNotFound;
  }
  else if( fileNum == 1 )
  {
    m_SerialID = 0;

    nn::Result nnResult = GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->StartImmidiateTask( gfl2::thread::Thread::THREAD_PRI_NORMAL + 1, pFileList[0].fileName , NULL , NetAppLib::Util::BossUtility::GetRegionURL() );

    if( !nnResult.IsSuccess() )
    {
      m_regSeq = REGSEQ_ERROR;
    }
  }
  else
  {
    GFL_ASSERT(0);//@fix
    GFL_PRINT("OnBossRecvFileList : invalid boss server file num[%d]\n",fileNum);
    m_regSeq = REGSEQ_ERROR;
  }
}


void Frame_InternetMatchEntry::OnBossRecvFileListFailed( const char* pTaskName , const nn::Result& result )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultBossError( result );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  m_regSeq = REGSEQ_ERROR;
}


void Frame_InternetMatchEntry::OnBossError( const nn::Result& result )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultBossError( result );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  m_regSeq = REGSEQ_ERROR;
}


void Frame_InternetMatchEntry::OnBossTaskError( const char* pTaskName , const u32 errorCode )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultBossTaskError( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  m_regSeq = REGSEQ_ERROR;
}


//------------------------------------------------------------------
/**
* @brief コミュニティの検索完了通知
*
* @param[in] communityList 検索されたコミュニティリスト
*/
//------------------------------------------------------------------
void Frame_InternetMatchEntry::OnCommunitySearchSuccess( nn::nex::qList<nn::nex::Community>* communityList )
{
  GFL_PRINT("Frame_InternetMatchEntry::OnCommunitySearchSuccess\n");

  if( communityList == NULL )
  {//リストがNULL
    GFL_PRINT("Frame_InternetMatchEntry::OnCommunitySearchSuccess:error List is NULL\n");
    m_regSeq = REGSEQ_ERROR;
  }
  else if( m_regSeq == REGSEQ_LEAVE_COMMUNITY_WAIT )
  {//脱退用検索結果
    if( !communityList->empty() )
    {//コミュニティが空ではない
      nn::nex::qList<nn::nex::Community>::const_iterator it = communityList->begin();
      u32 count = 0;
      u32 netCommunityID = m_pWork->m_pBattleSpotSave->GetEntryCommunityId( Savedata::BattleSpotData::ENTRY_COMMUNITY_NET );
      while(it != communityList->end())
      {
        GFL_PRINT( "FindGatheringID[%u]" , it->GetID() );
        u32 communityID = it->GetAttribute( AT_INDEX_communityID );
        GFL_PRINT( "FindCommunityID[%u]\n" , communityID );

        if( netCommunityID == communityID )
        {//脱退すべきコミュニティを発見
          GFL_PRINT( "UpdateGatheringID[%u][%d]\n" , communityID,it->GetID() );
          
          if( !m_pMatchMakeClient->LeaveCommunity( it->GetID() ) )
          {
            GFL_PRINT("ProcUpdateRatingbattle::UpdateFunc : LeaveCommunity is failed[%d]\n",it->GetID());
            m_regSeq = REGSEQ_ERROR;
          }

          return;
        }

        ++it;
      }
    }

    //参加シーケンスへ
    m_regSeq = REGSEQ_JOIN_COMMUNITY;
  }
  else if( m_regSeq == REGSEQ_JOIN_COMMUNITY_WAIT )
  {//参加用検索結果
    if( !communityList->empty() )
    {//コミュニティが空ではない
      nn::nex::qList<nn::nex::Community>::const_iterator it = communityList->begin();
      u32 count = 0;
      while(it != communityList->end())
      {
        GFL_PRINT( "FindGatheringID[%u]" , it->GetID() );
        u32 communityID = it->GetAttribute( AT_INDEX_communityID );
        GFL_PRINT( "FindCommunityID[%u]\n" , communityID );

        if( m_communityID == communityID )
        {//参加すべきコミュニティを発見
          GFL_PRINT( "UpdateGatheringID[%u][%d]\n" , communityID,it->GetID() );

          //時間保存
          nn::nex::DateTime* pDateTime = m_pWork->GetCommunityEndDateTime( BattleSpotWork::COMMUNITY_INTERNET_CUP_START );
          *pDateTime = it->GetParticipationStartDate();

          GFL_PRINT("COMMUNITY_INTERNET_CUP_START[%d][%d][%d][%d]\n" , pDateTime->GetYear(),pDateTime->GetMonth(),pDateTime->GetDay(),pDateTime->GetHour() );

          pDateTime = m_pWork->GetCommunityEndDateTime( BattleSpotWork::COMMUNITY_INTERNET_CUP_END );
          *pDateTime = it->GetParticipationEndDate();

          GFL_PRINT("COMMUNITY_INTERNET_CUP_END[%d][%d][%d][%d]\n" , pDateTime->GetYear(),pDateTime->GetMonth(),pDateTime->GetDay(),pDateTime->GetHour() );

          //時間確認
          //開催時間チェック
          gflnet2::nex::NexManager* nexMgr = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
          nn::nex::DateTime dateTime;
          if( nexMgr->GetNgsFacade()->GetGameServerTime( &dateTime ) == false )
          {//エラー
            ExitSelf();
            return;
          }

          GFL_PRINT("nn::nex::DateTime[%d][%d][%d][%d]\n" , dateTime.GetYear(),dateTime.GetMonth(),dateTime.GetDay(),dateTime.GetHour() );

          if( *(m_pWork->GetCommunityEndDateTime( BattleSpotWork::COMMUNITY_INTERNET_CUP_END )) < dateTime )
          {//終了済み
            GFL_PRINT("終了済み\n");
            m_nextGpfEntryFlg = m_pWork->m_gpfEntryFlg;
            m_nextWifiMatchupState = ENTRY_END;
            m_State = STATE_RetireDateIsOver;
          }
          else if( *(m_pWork->GetCommunityEndDateTime( BattleSpotWork::COMMUNITY_INTERNET_CUP_START )) > dateTime )
          {//開催前は先に進む
            GFL_PRINT("開催前\n");
            m_regSeq = REGSEQ_OK;
          }
          else
          {//開催中
            if( !m_pMatchMakeClient->JoinCommunity( it->GetID() ) )
            {
              GFL_PRINT("ProcUpdateRatingbattle::UpdateFunc : JoinCommunity is failed[%d]\n",it->GetID());
              m_regSeq = REGSEQ_ERROR;
            }
          }

          return;
        }

        ++it;
      }
    }

    //参加するものが無いのはエラー
    GFL_ASSERT(0);//@fix
    m_regSeq = REGSEQ_ERROR;
  }
  else
  {//シーケンスがおかしい
    GFL_PRINT("Frame_InternetMatchEntry::OnCommunitySearchSuccess:error Invalid SEQ[%d]\n",m_regSeq);
    m_regSeq = REGSEQ_ERROR;
  }
}

//------------------------------------------------------------------
/**
* @brief コミュニティへの参加完了通知
*
* @param[in] gatheringId 参加したギャザリングID
*/
//------------------------------------------------------------------
void Frame_InternetMatchEntry::OnCommunityJoinSuccess( const nn::nex::GatheringID& gatheringId )
{
  GFL_PRINT("ProcUpdateRatingbattle::OnCommunityJoinSuccess[%u]\n",gatheringId);

  m_regSeq = REGSEQ_OK;
}

//------------------------------------------------------------------
/**
* @brief コミュニティの退出完了通知
*
* @param[in] gatheringId 退出したギャザリングID
*/
//------------------------------------------------------------------
void Frame_InternetMatchEntry::OnCommunityLeaveSuccess( const nn::nex::GatheringID& gatheringId )
{
  GFL_PRINT("ProcUpdateRatingbattle::OnCommunityLeaveSuccess[%u]\n",gatheringId);

  m_regSeq = REGSEQ_JOIN_COMMUNITY;
}


//------------------------------------------------------------------
/**
* @brief コミュニティAPIのエラー終了通知
*
* @param[in] result    エラー理由
* @param[in] errorCode エラーアプレットに渡すエラーコード
*/
//------------------------------------------------------------------
void Frame_InternetMatchEntry::OnNexCommunityError( const nn::nex::qResult& result, u32 errorCode )
{
  GFL_PRINT("ProcUpdateRatingbattle::OnNexCommunityError\n");

  if( m_regSeq == REGSEQ_LEAVE_COMMUNITY_WAIT )
  {//脱退はエラーでも進行
    m_regSeq = REGSEQ_JOIN_COMMUNITY;
    return;
  }

  if( errorCode == ALEADY_JOINE_ERROR )
  {//参加済みエラーコードは成功扱い
    m_regSeq = REGSEQ_OK;
    return;
  }

  nn::nex::StringStream ss;
  ss << _T("errorCode:") << errorCode << _T("\n") << result;
  GFL_PRINT("OnNexCommunityError[%ls]\n", ss.CStr());

  m_regSeq = REGSEQ_ERROR;

  // エラーアプレット表示
  NetLib::Error::NijiNetworkError error;
  error.SetResultNex( result );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
}


#endif

} /*  namespace Live    */
} /*  namespace NetApp  */


