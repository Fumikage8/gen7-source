//==============================================================================
/**
 * @file   BattleSpotFrame_FriendMatchEntry.cpp
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
#include  "./BattleSpotFrame_FriendMatchEntry.h"
#include  "GameSys/include/GameData.h"
#include <Savedata/include/GameSyncSave.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include "PokeTool/include/PokeToolBattleParty.h"

#include <gflnet2/include/util/gflnet2_Base64Utility.h>


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
    STATE_UnlockMessage,
    STATE_NextEntryWork2,
    STATE_BattleBoxUpload,
    STATE_NextEntryWork3,
    STATE_GameSync,
    STATE_GameSyncWait,
    STATE_NextEntryWork4,
    STATE_Retire,
    STATE_RetireForNewCup,
    STATE_RetireUnlock,
    STATE_EndUnlock,
    STATE_UnlockSave,
    STATE_UpdateEntryFlg,
    STATE_End,

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
  *  @func     Frame_FriendMatchEntry
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_FriendMatchEntry::Frame_FriendMatchEntry(BattleSpotWork* pWork)
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
  , m_wifiMatchUpID(0)
  , m_pEncodeBuffer(NULL)
{
#if defined(GF_PLATFORM_CTR)
  m_pglConnector = GFL_NEW( m_pWork->GetDeviceHeap() ) NetLib::Pgl::PGLConnector();

  GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  Savedata::MyStatus* myst = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatus();
  gflnet2::nex::NexManager* nexMgr = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);

  m_pglConnector->Initialize( m_pWork->GetDeviceHeap(), myst->GetNexUniqueID() , nexMgr->GetAuthenticationToken(), static_cast<nn::cfg::CTR::CfgRegionCode>( myst->GetRegionCode() ) );
  m_pglConnector->SetPGLConnectorListener( this );

  if( game_data->GetBattleSpotData()->GetFriendLockTeamIndex() != 0xFF )
  {
      m_pWork->m_pAppParam->m_pTeamSelectEventParam->out.teamIdx = game_data->GetBattleSpotData()->GetFriendLockTeamIndex();//View用にインデックスを入れておく
  }

#endif
}


//------------------------------------------------------------------
/**
  *  @func     Frame_FriendMatchEntry
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_FriendMatchEntry::~Frame_FriendMatchEntry()
{
#if defined(GF_PLATFORM_CTR)
  GflHeapSafeFreeMemory(m_pEncodeBuffer);

  if( m_pglConnector )
  {
    m_pglConnector->RemovePGLConnectorListener();
    m_pglConnector->Finalize();
    GFL_SAFE_DELETE( m_pglConnector );
  }
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
bool  Frame_FriendMatchEntry::startup()
{
#if defined(GF_PLATFORM_CTR)
  m_pViewContainer->SetAllInputEnabled( true );
  GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  u32 lockIndex = pGameData->GetBattleSpotData()->GetFriendLockTeamIndex();
  //大会チームをロード
  if( lockIndex != 0xFF && pGameData->GetBOX()->IsTeamLock( lockIndex ) )
  {
    PokeTool::BattleParty tempParty( m_pWork->GetDeviceHeap()->GetLowerHandle() );
    tempParty.CreateBattleParty( pGameData->GetBOX() , pGameData->GetBoxPokemon() , lockIndex , m_pWork->GetDeviceHeap()->GetLowerHandle() );
    m_pWork->m_pAppParam->m_pPersonalData[0]->pPokeParty->CopyFrom( *tempParty.GetPokeParty() );//0番にロック済みパーティーを入れる
    m_pWork->m_pAppParam->m_pTeamSelectEventParam->out.pPokeParty->CopyFrom( *tempParty.GetPokeParty() );

    m_pWork->m_pViewContainer->m_pLowerView_Card->ResetPokeParty();
  }

  m_pViewContainer->SetScene(BattleSpotViewContainer::SCENE_FriendMatchEntry);

  /*  views  */
  {
    m_pUpperView  = m_pViewContainer->m_pUpperView_Information;
    m_pUpperView->SetViewMode( UpperView_Information::VIEWMODE_FriendMatch );

    m_pLowerView  = m_pViewContainer->m_pLowerView_Selection;

    if( m_pWork->m_pAppParam->m_teamSelected == true )
    {//チーム選択戻り
      m_pWork->m_pAppParam->m_teamSelected = false;
      
      //開催時間チェック
      gflnet2::nex::NexManager* nexMgr = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
      nn::nex::DateTime dateTime;
      if( nexMgr->GetNgsFacade()->GetGameServerTime( &dateTime ) == false )
      {//エラー
        ExitSelf(RESULT_FriendMatchEntry_Back);
        return true;
      }

      GFL_PRINT("nn::nex::DateTime[%d][%d][%d][%d]\n" , dateTime.GetYear(),dateTime.GetMonth(),dateTime.GetDay(),dateTime.GetHour() );

      u64 qrTime = static_cast<u64>( m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::END_YEAR ) ) * static_cast<u64>(10000) +
        static_cast<u64>( m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::END_MONTH ) ) * static_cast<u64>( 100 ) +
        static_cast<u64>( m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::END_DAY ) );
      u64 serverTime = static_cast<u64>( dateTime.GetYear() ) * static_cast<u64>( 10000 ) +
        static_cast<u64>( dateTime.GetMonth() ) * static_cast<u64>( 100 ) +
        static_cast<u64>( dateTime.GetDay() );

      GFL_PRINT("Time Compare[%llu][%llu]\n",qrTime,serverTime);
      // @fix [#4302 インターネットでの仲間大会で事前エントリーした際に、 大会期間終了時の処理（大会エントリー解除、バトルボックスロック解除）が走ってしまう。]　の修正
      if( ( serverTime > qrTime ) || ( ( serverTime == qrTime ) && m_pWork->IsRegulationTimeOver( dateTime.GetHour() ) ) )
      {//大会終了後だったらリタイヤ
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_Connecting);
        m_nextGpfEntryFlg = m_pWork->m_gpfEntryFlg;
        m_nextWifiMatchupState = ENTRY_END;
        m_State = STATE_End;
      }
      else if( serverTime < qrTime )
      {//大会終了前だったらはじく
        m_pLowerView->SetViewMode( LowerView_Selection::VIEWMODE_Message_TournamentNotStarted );
        m_State = STATE_MessageWait;
      }
      else
      {//開催中
        m_teamLockRequest = true;
        m_nextGpfEntryFlg = m_pWork->m_gpfEntryFlg;
        m_nextWifiMatchupState = ENTRY_BATTLEBOX;
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_Connecting);


        GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
        Savedata::MyStatus    *sv_mystatus = game_data->GetPlayerStatus();
        m_pWork->m_pBattleSpotSave->SetFriendMatchPID( sv_mystatus->GetPrincipalId() );
        m_pWork->m_pBattleSpotSave->SetFriendMatchCommunityID( m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::DISCRIMINANT_ID ) );

        m_State = STATE_BattleBoxUpload;


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

        bool ret = m_pglConnector->UploadBattleBox( static_cast<char*>( m_pEncodeBuffer ) , partydataSize , NetLib::Pgl::PGLConnector::BATTLE_TYPE_FRIEND );
        if( ret == false )
        {//エラー
          ExitSelf();
        }
      }
    }
    else if( m_pWork->m_pAppParam->m_qrDecoded == true )
    {//QR戻り
      m_pWork->m_pAppParam->m_qrDecoded = false;

      m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_Connecting);
      m_pglState = PGL_INIT;
      m_State = STATE_Join;
    }
    else
    {//通常
      if( pGameData->GetBattleSpotData()->GetFriendLockTeamIndex() == 0xFF )
      {
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_FriendMatch_Work0);
      }
      else
      {
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_FriendMatch_Work0_1);
      }
      
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
void  Frame_FriendMatchEntry::setListener()
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
void  Frame_FriendMatchEntry::removeListener()
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
void  Frame_FriendMatchEntry::updateSequence()
{
  switch(m_State)
  {
    case  STATE_Entry:
      m_State =  STATE_Selection;
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
          u32 cupID = m_pWork->m_pBattleSpotSave->GetFriendMatchCommunityID();
          u32 qrCupID = m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::DISCRIMINANT_ID );
          if( m_wifiMatchUpID != qrCupID )
          {//QRとサーバが不一致
            m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_CannotEntry);
            m_State =  STATE_MessageWait;
            return;
          }
          else if( m_wifiMatchUpID != cupID )
          {//ROMとサーバが不一致
            if( m_pWork->m_pBattleSpotSave->GetFriendLockTeamIndex() != 0xFF )
            {//ロックされている
              //バトルボックスロック解除
              GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
              u8 lockIndex = pGameData->GetBattleSpotData()->GetFriendLockTeamIndex();
              if( lockIndex != 0xFF )
              {
                pGameData->GetBOX()->SetTeamLock( lockIndex , false );
              }

              pGameData->GetBattleSpotData()->SetFriendLockTeamIndex( Savedata::BattleSpotData::INVALID_LOCK_INDEX );

              //状態を初期化
              pGameData->GetBattleSpotData()->SetFriendMatchCommunityID( 0 );
              pGameData->GetBattleSpotData()->SetFriendMatchPID( 0 );


              m_State = STATE_RetireForNewCup;

              UpdateSave();
              return;
            }
          }

          //PGLエントリーワークチェック
          if( m_pWork->m_gpfEntryFlg == 1 )
          {
            m_nextGpfEntryFlg = 0;
            m_nextWifiMatchupState = ENTRY_FINISH;
            m_State = STATE_UpdateEntryFlg;
            return;
          }

          //ワーク3/4の処理
          switch( m_pWork->m_pBattleSpotSave->GetFriendEntryWork() )
          {
          case ENTRY_BATTLEBOX://3
          case ENTRY_UPLOAD://4
            {
              #if defined(GF_PLATFORM_CTR)
              gflnet2::nex::NexManager* nexMgr = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
              nn::nex::DateTime dateTime;
              if( nexMgr->GetNgsFacade()->GetGameServerTime( &dateTime ) == false )
              {//エラー
                ExitSelf(RESULT_FriendMatchEntry_Back);
                return;
              }

              GFL_PRINT("nn::nex::DateTime[%d][%d][%d][%d]\n" , dateTime.GetYear(),dateTime.GetMonth(),dateTime.GetDay(),dateTime.GetHour() );

              u64 qrTime = static_cast<u64>( m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::END_YEAR ) ) * static_cast<u64>(10000) +
                static_cast<u64>( m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::END_MONTH ) ) * static_cast<u64>( 100 ) +
                static_cast<u64>( m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::END_DAY ) );
              u64 serverTime = static_cast<u64>( dateTime.GetYear() ) * static_cast<u64>( 10000 ) +
                static_cast<u64>( dateTime.GetMonth() ) * static_cast<u64>( 100 ) +
                static_cast<u64>( dateTime.GetDay() );

              GFL_PRINT("Time Compare[%llu][%llu]\n",qrTime,serverTime);
              // @fix [#4302 インターネットでの仲間大会で事前エントリーした際に、 大会期間終了時の処理（大会エントリー解除、バトルボックスロック解除）が走ってしまう。]　の修正
              if( ( serverTime > qrTime ) || ( ( serverTime == qrTime ) && m_pWork->IsRegulationTimeOver( dateTime.GetHour() ) ) )
              {//大会終了後だったらリタイヤ
                m_nextGpfEntryFlg = m_pWork->m_gpfEntryFlg;
                m_nextWifiMatchupState = ENTRY_END;
                m_State = STATE_EndUnlock;
                return;
              }

              //ロックが解除されていたらリタイヤしないといけないパターン
              if( m_pWork->m_pBattleSpotSave->GetFriendLockTeamIndex() == 0xFF )
              {//解除されている
                m_nextGpfEntryFlg = m_pWork->m_gpfEntryFlg;
                m_nextWifiMatchupState = ENTRY_RETIRE;
                m_State = STATE_RetireUnlock;
                return;
              }
              break;
              #endif
            }
          }

          //ワークによる分岐
          switch( m_pWork->m_pBattleSpotSave->GetFriendEntryWork() )
          {
          case ENTRY_NOT://         = 0,      ///< 未エントリー
            m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_NoTournament);
            m_State =  STATE_MessageWait;
          break;
          case ENTRY_RETIRE://      = 6,      ///< リタイア
          case ENTRY_CANCEL://      = 7       ///< 参加解除（失格など）
            {
              //バトルボックスロック解除
              GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
              u8 lockIndex = pGameData->GetBattleSpotData()->GetFriendLockTeamIndex();
              if( lockIndex != 0xFF )
              {
                pGameData->GetBOX()->SetTeamLock( lockIndex , false );
              }

              pGameData->GetBattleSpotData()->SetFriendLockTeamIndex( Savedata::BattleSpotData::INVALID_LOCK_INDEX );

              //状態を初期化
              pGameData->GetBattleSpotData()->SetFriendMatchCommunityID( 0 );
              pGameData->GetBattleSpotData()->SetFriendMatchPID( 0 );

              UpdateSave();
              m_State = STATE_UnlockSave;
            }
          break;
          case ENTRY_FINISH://      = 1,      ///< エントリー完了
            m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_Connecting);
            if( m_pWork->m_pBattleSpotSave->GetFriendEntryWork() == ENTRY_FINISH )
            {
              m_nextGpfEntryFlg = m_pWork->m_gpfEntryFlg;
              m_nextWifiMatchupState = ENTRY_REGULATION;
              m_State = STATE_NextEntryWork2;
            }
          break;
          case ENTRY_REGULATION://  = 2,      ///< レギュレーション取得済み
            if( m_pWork->m_pBattleSpotSave->GetFriendLockTeamIndex() != 0xFF )
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
              m_pWork->m_pAppParam->nextApp = NEXT_APP_TEAM_SELECT_FRIEND_CUP;
              m_pWork->m_pAppParam->nextFrame = FRAME_FriendMatchEntry;
              ExitSelf(RESULT_FriendMatchEntry_TeamSelect);
            }
          break;
          case ENTRY_BATTLEBOX://   = 3,      ///< バトルボックス登録完了
            m_nextGpfEntryFlg = m_pWork->m_gpfEntryFlg;
            m_nextWifiMatchupState = ENTRY_UPLOAD;
            m_State = STATE_GameSync;
          break;
          case ENTRY_UPLOAD://      = 4,      ///< 全セーブデータアップ完了
            ExitSelf(RESULT_FriendMatchEntry_Next);
          break;
          case ENTRY_END://         = 5,      ///< 大会終了
            m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_TournamentIsOver);
            m_State =  STATE_MessageWait;
          break;
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
    case STATE_NextEntryWork2:
      if( UpdateWorldBattleStatus() )
      {
        m_pglState = PGL_INIT;
        m_State = STATE_Join;
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
      {
      }
    break;
    case STATE_NextEntryWork4:
      if( UpdateWorldBattleStatus() )
      {
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_GameSync);
        m_State =  STATE_MessageWaitDownloaded;
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
    case STATE_RetireForNewCup:
      if( UpdateSave() )
      {
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_Retire_friend_NewCup);
        m_State = STATE_UnlockMessage;
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
    case STATE_EndUnlock:
      {
        if( UpdateWorldBattleStatus() )
        {
          m_pLowerView->SetViewMode( LowerView_Selection::VIEWMODE_Message_TournamentIsOverUnlock );
          m_State = STATE_MessageWait;
        }
      }
      break;
    case STATE_End:
      {
        if( UpdateWorldBattleStatus() )
        {
          m_pLowerView->SetViewMode( LowerView_Selection::VIEWMODE_Message_TournamentIsOver );
          m_State = STATE_MessageWait;
        }
      }
      break;
    case STATE_UnlockSave: 
      {
        if( UpdateSave() )
        {
          m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_CannotEntry);
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
bool  Frame_FriendMatchEntry::cleanup()
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
void  Frame_FriendMatchEntry::LowerView_Selection_OnEvent(LowerView_Selection* pSender, const LowerView_Selection::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
  case  LowerView_Selection::IEventHandler::EVENT_Back:             
    ExitSelf();
    break;
  case  LowerView_Selection::IEventHandler::EVENT_Friendmatch_QRRead:/**<  モード選択：大会に参加する  */
    m_pWork->m_pAppParam->nextApp = NEXT_APP_QR;
    ExitSelf(RESULT_FriendMatchEntry_QR);
    break;
  case  LowerView_Selection::IEventHandler::EVENT_FriendMatch_Match:              /**<  モード選択：対戦する        */
    m_pglState = PGL_INIT;
    m_State = STATE_Join;
    break;
  case  LowerView_Selection::IEventHandler::EVENT_FriendMatch_WatchCard:              /**<  モード選択：選手証を見る    */
    m_pViewContainer->SetScene(BattleSpotViewContainer::SCENE_FriendMatch_Matching);

    m_pViewContainer->m_pLowerView_Card->SetViewMode(LowerView_Card::VIEWMODE_FriendMatch_Card);
    m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_FriendMatch_Card);
    break;
  case  LowerView_Selection::IEventHandler::EVENT_FriendMatch_PreviousTournament:     /**<  モード選択：前回大会        */
    m_pViewContainer->SetScene(BattleSpotViewContainer::SCENE_FriendMatch_Matching);

    m_pViewContainer->m_pLowerView_Card->SetViewMode(LowerView_Card::VIEWMODE_FriendMatch_Card);
    m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_FriendMatch_Card);
    break;
  case  LowerView_Selection::IEventHandler::EVENT_FriendMatch_Retire:             /**<  モード選択：参加解除        */
    m_pLowerView->SetViewMode( LowerView_Selection::VIEWMODE_Message_RetireConfirm_friend );
    break;
  case  LowerView_Selection::IEventHandler::EVENT_Retire1_Yes:                        /**<  参加解除する？：はい    */
    {
      m_pLowerView->SetViewMode( LowerView_Selection::VIEWMODE_Message_RetireConfirm2_friend );
    }
    break;
  case  LowerView_Selection::IEventHandler::EVENT_Retire1_No:                         /**<  参加解除する？：いいえ  */
    {
      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
      if( pGameData->GetBattleSpotData()->GetFriendLockTeamIndex() == 0xFF )
      {
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_FriendMatch_Work0);
      }
      else
      {
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_FriendMatch_Work0_1);
      }
    }
    break;
  case  LowerView_Selection::IEventHandler::EVENT_Retire2_Yes:                        /**<  ほんとうに参加解除する？：はい    */
    {
      m_nextGpfEntryFlg = m_pWork->m_gpfEntryFlg;
      m_nextWifiMatchupState = ENTRY_RETIRE;
      m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_Connecting);
      m_State = STATE_Retire;
    }
    break;
  case  LowerView_Selection::IEventHandler::EVENT_Retire2_No:                         /**<  ほんとうに参加解除する？：いいえ  */
    {
      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
      if( pGameData->GetBattleSpotData()->GetFriendLockTeamIndex() == 0xFF )
      {
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_FriendMatch_Work0);
      }
      else
      {
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_FriendMatch_Work0_1);
      }
    }
    break;
  case LowerView_Selection::IEventHandler::EVENT_Message_EOM:
    {
      switch( m_State )
      {
      case STATE_MessageWait:
        ExitSelf(RESULT_FriendMatchEntry_Back);
        break;
      case STATE_MessageWaitDownloaded:
        m_pglState = PGL_INIT;
        m_State = STATE_Join;
        break;
      case STATE_UnlockMessage:
        m_State = STATE_PGLResult;
      break;
      }
    }
  break;
  }
}

void  Frame_FriendMatchEntry::LowerView_Card_OnEvent(LowerView_Card* pSender, const LowerView_Card::IEventHandler::EventCode eventCode)
{
  GFL_PRINT("LowerView_Card_OnEvent:[%d]\n",eventCode);
  switch(eventCode)
  {
  case  LowerView_Card::IEventHandler::EVENT_Back:
    m_pViewContainer->SetScene(BattleSpotViewContainer::SCENE_FriendMatchEntry);

    m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_FriendMatch);
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
void  Frame_FriendMatchEntry::ExitSelf(const FrameResult frameResult)
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
void Frame_FriendMatchEntry::SetViewModeForWork( void )
{
  //未使用になった
  GFL_ASSERT( 0 );//@fix
  switch( m_pWork->m_pBattleSpotSave->GetFriendEntryWork() )
  {
  case ENTRY_NOT://         = 0,      ///< 未エントリー
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_FriendMatch_Work0);
    break;
  case ENTRY_FINISH://      = 1,      ///< エントリー完了
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_FriendMatch_Work1);
    break;
  case ENTRY_REGULATION://  = 2,      ///< レギュレーション取得済み
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_FriendMatch_Work2);
    break;
  case ENTRY_BATTLEBOX://   = 3,      ///< バトルボックス登録完了
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_FriendMatch_Work3_TeamLocked);
    break;
  case ENTRY_UPLOAD://      = 4,      ///< 全セーブデータアップ完了
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_FriendMatch_Work4_TeamLocked);
    break;
  case ENTRY_END://         = 5,      ///< 大会終了
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_FriendMatch_Work5);
    break;
  case ENTRY_RETIRE://      = 6,      ///< リタイア
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_FriendMatch_Work6);
    break;
  case ENTRY_CANCEL://      = 7       ///< 参加解除（失格など）
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_FriendMatch_Work6);
    break;
  };
}

void Frame_FriendMatchEntry::UpdatePGLCheck( void )
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
    m_pglConnector->GetWorldBattleStatus( NetLib::Pgl::PGLConnector::BATTLE_TYPE_FRIEND );
#endif
    m_pglState = PGL_BATTLESTATUS_WAIT;
    break;
  case PGL_BATTLESTATUS_WAIT:
    break;
  }

  return;
}

void Frame_FriendMatchEntry::OnConfirmPlayStatusSuccess()
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

void Frame_FriendMatchEntry::OnConfirmPlayStatusError( u32 errorCode )
{
#if defined(GF_PLATFORM_CTR)
  // @fix [#4309 PGLがサービス終了する際に、インターネット大会、仲間大会でロックしていたバトルチームがそのままにロック状態となる]
  if( errorCode == NetLib::Pgl::E_RET_SERVICE_END )
  {
    // PGLサービス終了
    if( m_pWork->m_pBattleSpotSave->GetFriendLockTeamIndex() != 0xFF )
    {//ロックされている
      //バトルボックスロック解除
      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
      u8 lockIndex = pGameData->GetBattleSpotData()->GetFriendLockTeamIndex();
      if( lockIndex != 0xFF )
      {
        pGameData->GetBOX()->SetTeamLock( lockIndex , false );
      }

      pGameData->GetBattleSpotData()->SetFriendLockTeamIndex( Savedata::BattleSpotData::INVALID_LOCK_INDEX );

      //状態を初期化
      pGameData->GetBattleSpotData()->SetFriendMatchCommunityID( 0 );
      pGameData->GetBattleSpotData()->SetFriendMatchPID( 0 );

      UpdateSave();
    }
  }
#endif
  m_wait = 0;
  ExitSelf();
}


void Frame_FriendMatchEntry::OnSaveDataSyncSuccess( void )
{
  GFL_PRINT("Frame_FriendMatchEntry::OnSaveDataSyncSuccess\n");
  m_State = STATE_NextEntryWork4;
}

void Frame_FriendMatchEntry::OnSaveDataSyncError( u32 errorCode )
{
  ExitSelf();
}

bool Frame_FriendMatchEntry::UpdateWorldBattleStatus( void )
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
    //バトルボックスロック
    if( m_teamLockRequest )
    {
      m_teamLockRequest = false;
      u32 lockIndex = m_pWork->m_pAppParam->m_pTeamSelectEventParam->out.teamIdx;
      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
      pGameData->GetBOX()->SetTeamLock( lockIndex , true );
      m_pWork->m_pBattleSpotSave->SetFriendLockTeamIndex( lockIndex );
      m_pWork->m_pBattleSpotSave->SetFriendMatchCount( 0 );
    }

    //バトルボックスロック解除
    if( m_nextWifiMatchupState == ENTRY_RETIRE || m_nextWifiMatchupState == ENTRY_END || m_nextWifiMatchupState == ENTRY_FINISH )
    {
      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
      u8 lockIndex = pGameData->GetBattleSpotData()->GetFriendLockTeamIndex();
      if( lockIndex != 0xFF )
      {
        pGameData->GetBOX()->SetTeamLock( lockIndex , false );
      }

      pGameData->GetBattleSpotData()->SetFriendLockTeamIndex( Savedata::BattleSpotData::INVALID_LOCK_INDEX );

      //状態を初期化
      pGameData->GetBattleSpotData()->SetFriendMatchCommunityID( 0 );
      pGameData->GetBattleSpotData()->SetFriendMatchPID( 0 );
    }

    m_pWork->m_pBattleSpotSave->SetFriendEntryWork( m_nextWifiMatchupState );
    UpdateSave();
    m_updateStatusSeq = UPDATE_STATUS_SAVEWAIT;
    break;
  case UPDATE_STATUS_SAVEWAIT:
    if( UpdateSave() )
    {
      m_pglConnector->UpdateWorldBattleStatus( m_nextGpfEntryFlg , m_nextWifiMatchupState , NetLib::Pgl::PGLConnector::BATTLE_TYPE_FRIEND );
      m_updateStatusSeq = UPDATE_STATUS_UPDATEWAIT;
    }
    break;
  case UPDATE_STATUS_UPDATEWAIT:
    if( m_wait++ < DUMMY_WAIT )
    {
      break;
    }

    if( m_nextGpfEntryFlg == m_pWork->m_gpfEntryFlg &&
      m_nextWifiMatchupState == m_pWork->m_friendTempEntryWork )
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
void Frame_FriendMatchEntry::OnPGLGetWorldBattleStatusSuccess( u32 wifiMatchUpID, u16 gpfEntryFlg, u16 wifiMatchupState, bool isSignIn, s8 timezone )
{
  GFL_PRINT("Frame_FriendMatchEntry::OnPGLGetWorldBattleStatusSuccess\n");
  GFL_PRINT("大会ID %d\n", wifiMatchUpID );
  GFL_PRINT("PGLエントリーワーク %d \n", gpfEntryFlg );
  GFL_PRINT("大会参加ワーク %d \n", wifiMatchupState );
  GFL_PRINT("登録有無 %d \n", isSignIn );
  GFL_PRINT("タイムゾーン %d \n", timezone );

  GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  Savedata::MyStatus    *sv_mystatus = game_data->GetPlayerStatus();

  m_pWork->m_pBattleSpotSave->SetFriendEntryWork( wifiMatchupState );

  m_pWork->m_gpfEntryFlg = gpfEntryFlg;

  game_data->GetSaveData()->GetGameSync()->SetSignedPGL( isSignIn );
  /*
#ifdef BATTLE_MATCH_DEBUG_MENU
  // デバッグで値を設定している場合はそちらに変更する
  if( debug_entry_work > 0 )
  {
    m_common_data->entry_work = static_cast<ENTRY_WORK>(debug_entry_work);
  }
#endif
  */

  m_wait = 0;

  m_wifiMatchUpID = wifiMatchUpID;

  m_State = STATE_PGLResult;
}


//------------------------------------------------------------------
/**
* @brief PGLバトルエントリー状態更新通信の完了イベントを検知
*/
//------------------------------------------------------------------
void Frame_FriendMatchEntry::OnPGLUpdateWorldBattleStatusSuccess( void )
{
  GFL_PRINT("Frame_FriendMatchEntry::OnPGLGetWorldBattleStatusSuccess\n");

  m_pWork->m_gpfEntryFlg = m_nextGpfEntryFlg;
  m_pWork->m_friendTempEntryWork = m_nextWifiMatchupState;
}

//------------------------------------------------------------------
/**
* @brief PGLバトルボックス登録通信の完了イベントを検知
*/
//------------------------------------------------------------------
void Frame_FriendMatchEntry::OnPGLUploadBattleBoxSuccess( void )
{
  GFL_PRINT("Frame_FriendMatchEntry::OnPGLUploadBattleBoxSuccess\n");

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
void Frame_FriendMatchEntry::OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const s32 httpStatus )
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
void Frame_FriendMatchEntry::OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const NetLib::Pgl::PGL_COMMON_RESULT resultCode )
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
void Frame_FriendMatchEntry::OnPGLConnectError( const NetLib::Pgl::PGL_COMMAND_TYPE commandType, const nn::nex::qResult &result, u32 errorCode )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  ExitSelf();
}

#endif

} /*  namespace Live    */
} /*  namespace NetApp  */


