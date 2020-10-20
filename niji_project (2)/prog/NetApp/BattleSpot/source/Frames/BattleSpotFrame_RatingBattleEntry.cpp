//==============================================================================
/**
 * @file   BattleSpotFrame_RatingBattleEntry.cpp
 * @date   2015/10/02 Fri. 13:32:21
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

#include  "./BattleSpotFrame_RatingBattleEntry.h"

#include <gflnet2/include/boss/gflnet2_BossManager.h>
#include "NetStatic/NetAppLib/include/Util/NetAppBossUtility.h"

#include "NetStatic/NetAppLib/include/GameSync/GameSyncRequestFacade.h"
#include  "GameSys/include/GameData.h"
#include <Savedata/include/GameSyncSave.h>

#include "NetStatic/NetAppLib/include/Util/NetAppPackFile.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,
    STATE_PGLResult,
    STATE_BattleVideoSendSetting,
    STATE_RegulationCheckInit,
    STATE_RegulationCheck,
    STATE_Selection,
    STATE_MessageWait,
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
  *  @func     Frame_RatingBattleEntry
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_RatingBattleEntry::Frame_RatingBattleEntry(BattleSpotWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->m_pViewContainer)
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_pUpperView(NULL)
  , m_pLowerView(NULL)
  , m_pglState(0)
  , m_regSeq(0)
  , m_SerialID(0)
  , m_pTempRegulation(NULL)
  , m_RegulationExtSaveData( m_pWork->GetDeviceHeap()->GetLowerHandle() )
  , m_isDeleteComplete(false)
  , m_isNewExists(false)
{
#ifdef  GF_PLATFORM_CTR
  GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->SetAppHeap( m_pWork->GetDeviceHeap() );
  GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->AddEventListener( this );
#endif
  for( u32 index = 0 ; index < REGULATION_NUM ; ++index )
  {
    m_pRegulation[index] = GFL_NEW( m_pWork->GetDeviceHeap()->GetLowerHandle() ) Regulation( m_pWork->GetDeviceHeap()->GetLowerHandle() );
  }
  m_pTempRegulation = GFL_NEW( m_pWork->GetDeviceHeap()->GetLowerHandle() ) Regulation( m_pWork->GetDeviceHeap()->GetLowerHandle() );

  std::memset( m_communityIDArray , 0 , sizeof(m_communityIDArray) );

#if defined(GF_PLATFORM_CTR)
  m_pMatchMakeClient = NULL;//コミュニティ参加用

  std::memset( m_myCommunityIDArray , 0 , sizeof(m_myCommunityIDArray) );//自分が参加しているコミュニティID
  std::memset( m_joinGatheringID , 0 , sizeof(m_joinGatheringID) );//参加すべきコミュニティのギャザリングID
  std::memset( m_joinCommunityID , 0 , sizeof(m_joinCommunityID) );//参加すべきコミュニティのコミュニティID
  std::memset( m_leaveGatheringID , 0 , sizeof(m_leaveGatheringID) );//脱退すべきコミュニティのギャザリングID
  m_joinCommunityNum = 0;
  m_leaveCommunityNum = 0;
#endif
}


//------------------------------------------------------------------
/**
  *  @func     Frame_RatingBattleEntry
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_RatingBattleEntry::~Frame_RatingBattleEntry()
{
#ifdef  GF_PLATFORM_CTR
  GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->RemoveEventListener( this );
  GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->RemoveAppHeap();
#endif
  for( u32 index = 0 ; index < REGULATION_NUM ; ++index )
  {
    GFL_SAFE_DELETE( m_pRegulation[index] );
  }
  GFL_SAFE_DELETE( m_pTempRegulation );
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  Frame_RatingBattleEntry::startup()
{
#ifdef  GF_PLATFORM_CTR
  m_pViewContainer->SetAllInputEnabled( true );
  m_pViewContainer->SetScene(BattleSpotViewContainer::SCENE_RatingBattleEntry);

  /*  views  */
  {
    m_pUpperView  = m_pViewContainer->m_pUpperView_Information; 
    m_pLowerView  = m_pViewContainer->m_pLowerView_Selection;

    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_Connecting);
    m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_None);
    
  }


  m_pMatchMakeClient = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexMatchMakingClient);
  if( m_pMatchMakeClient )
  {
    m_pMatchMakeClient->SetCommunityListener( this );
    if( !m_pMatchMakeClient->Initialize(GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->GetNgsFacade()) )
    {//失敗
      GFL_PRINT("ProcUpdateRatingbattle::InitFunc m_pMatchMakeClient->Initialize failed\n");

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
void  Frame_RatingBattleEntry::setListener()
{
//  m_pUpperView->SetEventHandler(this);
  m_pLowerView->SetEventHandler(this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_RatingBattleEntry::removeListener()
{
//  m_pUpperView->SetEventHandler(NULL);
  m_pLowerView->SetEventHandler(NULL);
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_RatingBattleEntry::updateSequence()
{
  switch(m_State)
  {
  case  STATE_Entry:
    {
      UpdatePGLCheck();
    }
  break;
  case STATE_PGLResult:
    {
      GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
      if( game_data->GetSaveData()->GetGameSync()->IsSignedPGL() )
      {//登録OK
        m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_RatingBattle_BattleVideoConfirm);
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_BattleVideoAutoUploadConfirm);
        m_State =  STATE_BattleVideoSendSetting;
      }
      else
      {//未登録
        m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_None);
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_PGLNoRegist);
        m_State =  STATE_MessageWait;
      }
    }
  break;
  case STATE_BattleVideoSendSetting://リスナー待ち
  break;
  case STATE_RegulationCheckInit:
    m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_None);
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_SDCard);
    m_State =  STATE_RegulationCheck;
  break;
  case  STATE_RegulationCheck:
    if( UpdateRegulation() )
    {
      m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_RatingBattle);
      m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_RatingBattle_SelectRule);
      m_State =  STATE_Selection;
    }
  break;
  case  STATE_Selection:
  break;
  case  STATE_MessageWait:
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
bool  Frame_RatingBattleEntry::cleanup()
{
  bool  bAllDone = true;


#ifdef  GF_PLATFORM_CTR
  if( GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->IsRunningThread() == true )
  {
    GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->CancelImmidiateTask();
    bAllDone = false;
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
void  Frame_RatingBattleEntry::LowerView_Selection_OnEvent(LowerView_Selection* pSender, const LowerView_Selection::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Selection::IEventHandler::EVENT_Back:
      {
        ExitSelf();
      }
    break;
    case  LowerView_Selection::IEventHandler::EVENT_RatingBattle_Rule_SingleBattle:
      {
        m_pWork->m_pAppParam->m_rateRuleCurorIndex = 0;
        m_pWork->SetRule( BattleSpotWork::RULE_RATE_SINGLE );
        u8* tempBuffer = GFL_NEW_ARRAY( m_pWork->GetDeviceHeap()->GetLowerHandle() ) u8[m_pRegulation[REGULATION_SINGLE]->GetSerializeDataSize()];
        m_pRegulation[REGULATION_SINGLE]->Serialize( tempBuffer );
        m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().Deserialize( tempBuffer , m_pRegulation[REGULATION_SINGLE]->GetSerializeDataSize() );
        GFL_SAFE_DELETE_ARRAY( tempBuffer );

        m_pWork->m_pAppParam->nextApp = NEXT_APP_TEAM_SELECT;
        m_pWork->m_pAppParam->nextFrame = FRAME_RatingBattleMatching;
        ExitSelf(RESULT_RatingBattleEntry_Next);
      }
    break;
    case  LowerView_Selection::IEventHandler::EVENT_RatingBattle_Rule_DoubleBattle:
      {
        m_pWork->m_pAppParam->m_rateRuleCurorIndex = 1;
        m_pWork->SetRule( BattleSpotWork::RULE_RATE_DOUBLE );
        u8* tempBuffer = GFL_NEW_ARRAY( m_pWork->GetDeviceHeap()->GetLowerHandle() ) u8[m_pRegulation[REGULATION_DOUBLE]->GetSerializeDataSize()];
        m_pRegulation[REGULATION_DOUBLE]->Serialize( tempBuffer );
        m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().Deserialize( tempBuffer , m_pRegulation[REGULATION_DOUBLE]->GetSerializeDataSize() );
        GFL_SAFE_DELETE_ARRAY( tempBuffer );

        m_pWork->m_pAppParam->nextApp = NEXT_APP_TEAM_SELECT;
        m_pWork->m_pAppParam->nextFrame = FRAME_RatingBattleMatching;
        ExitSelf(RESULT_RatingBattleEntry_Next);
      }
    break;
    case  LowerView_Selection::IEventHandler::EVENT_RatingBattle_Rule_SpecialBattle:
      {
        m_pWork->m_pAppParam->m_rateRuleCurorIndex = 2;
        m_pWork->SetRule( BattleSpotWork::RULE_RATE_SPECIAL );
        u8* tempBuffer = GFL_NEW_ARRAY( m_pWork->GetDeviceHeap()->GetLowerHandle() ) u8[m_pRegulation[REGULATION_SPECIAL]->GetSerializeDataSize()];
        m_pRegulation[REGULATION_SPECIAL]->Serialize( tempBuffer );
        m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().Deserialize( tempBuffer , m_pRegulation[REGULATION_SPECIAL]->GetSerializeDataSize() );
        GFL_SAFE_DELETE_ARRAY( tempBuffer );

        m_pWork->m_pAppParam->nextApp = NEXT_APP_TEAM_SELECT;
        m_pWork->m_pAppParam->nextFrame = FRAME_RatingBattleMatching;
        ExitSelf(RESULT_RatingBattleEntry_Next);
      }
    break;
    case LowerView_Selection::IEventHandler::EVENT_RatingBattle_Rule_WCSBattle:
      {
        m_pWork->m_pAppParam->m_rateRuleCurorIndex = 3;
        m_pWork->SetRule( BattleSpotWork::RULE_RATE_WCS );

        u8* tempBuffer = GFL_NEW_ARRAY( m_pWork->GetDeviceHeap()->GetLowerHandle() ) u8[m_pRegulation[REGULATION_WCS]->GetSerializeDataSize()];
        m_pRegulation[REGULATION_WCS]->Serialize( tempBuffer );
        m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().Deserialize( tempBuffer , m_pRegulation[REGULATION_WCS]->GetSerializeDataSize() );
        GFL_SAFE_DELETE_ARRAY( tempBuffer );

        m_pWork->m_pAppParam->nextApp = NEXT_APP_TEAM_SELECT;
        m_pWork->m_pAppParam->nextFrame = FRAME_RatingBattleMatching;
        ExitSelf(RESULT_RatingBattleEntry_Next);
      }
    break;
    case LowerView_Selection::IEventHandler::EVENT_Message_EOM:
      {
        if( m_State == STATE_MessageWait )
        {
          ExitSelf(RESULT_RatingBattleEntry_Back);
        }
        else if( m_regSeq == REGSEQ_DELETED || m_regSeq == REGSEQ_UPDATE )
        {
          m_regSeq = REGSEQ_OK;
        }
      }
    break;
    case EVENT_BattleVideoAutoUpload_Yes:
      m_pWork->SetBattleVideoAutoUpload(true);
      m_State =  STATE_RegulationCheckInit;
    break;
    case EVENT_BattleVideoAutoUpload_No:
      m_pWork->SetBattleVideoAutoUpload(false);
      m_State =  STATE_RegulationCheckInit;
    break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_RatingBattleEntry::ExitSelf(const FrameResult frameResult)
{
  m_pViewContainer->SetAllInputEnabled( false );
  m_Result  = frameResult;
  m_State   = STATE_Exit;
}


static u32 s_wait = 0;

bool Frame_RatingBattleEntry::UpdateRegulation( void )
{
#ifdef  GF_PLATFORM_CTR

  switch( m_regSeq )
  {
  case REGSEQ_INIT:
    {
      nn::Result nnResult = GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->StartImmidiateTask( gfl2::thread::Thread::THREAD_PRI_NORMAL + 1, NetAppLib::Util::BossUtility::GetRateRegulationFileName() , NULL , NetAppLib::Util::BossUtility::GetRegionURL() );

      if( nnResult.IsSuccess() )
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
    break;
  case REGSEQ_FIND_MY_COMMUNITY:
    {
      GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
      if( !m_pMatchMakeClient->FindCommunityByParticipant( game_data->GetPlayerStatus()->GetPrincipalId() , nn::nex::ResultRange() ) )
      {
        GFL_PRINT("FindCommunityByParticipant is failed[%u]\n",game_data->GetPlayerStatus()->GetPrincipalId());
        m_regSeq = REGSEQ_ERROR;
      }
      else
      {
        m_regSeq = REGSEQ_FIND_MY_COMMUNITY_WAIT;
      }
    }
    break;
  case REGSEQ_LEAVE_COMMUNITY:
    {
      if( m_leaveCommunityNum > 0 )
      {
        //脱退すべきコミュニティがある
        m_leaveCommunityNum--;
        if( !m_pMatchMakeClient->LeaveCommunity( m_leaveGatheringID[m_leaveCommunityNum] ) )
        {
          GFL_PRINT("ProcUpdateRatingbattle::UpdateFunc : LeaveCommunity is failed[%d][%d]\n",m_leaveCommunityNum,m_leaveGatheringID[m_leaveCommunityNum]);
          m_regSeq = REGSEQ_ERROR;
        }
        else
        {
          m_regSeq = REGSEQ_LEAVE_COMMUNITY_WAIT;
        }
      }
      else
      {//全脱退完了
        if( m_joinCommunityNum > 0 )
        {//参加シーケンスへ
          m_regSeq = REGSEQ_FIND_OFFICIALCOMMUNITY;
        }
        else
        {//更新完了
          m_regSeq = REGSEQ_OK;
        }
      }
    }
  break;
  case REGSEQ_FIND_OFFICIALCOMMUNITY:
    {
      const int COMMUNITY_SEARCH_NUM = 100;
      if( !m_pMatchMakeClient->FindOfficialCommunity( true , nn::nex::ResultRange( 0, COMMUNITY_SEARCH_NUM ) ) )
      {//エラー
        GFL_PRINT("ProcUpdateRatingbattle::UpdateFunc : FindOfficialCommunity is failed\n");
        m_regSeq = REGSEQ_ERROR;
      }
      else
      {
        m_regSeq = REGSEQ_FIND_OFFICIALCOMMUNITY_WAIT;
      }
    }
  break;
  case REGSEQ_JOIN_COMMUNITY:
    {
      if( m_joinCommunityNum > 0 )
      {
        //参加すべきコミュニティがある
        m_joinCommunityNum--;
        if( !m_pMatchMakeClient->JoinCommunity( m_joinGatheringID[m_joinCommunityNum] ) )
        {//エラー
          GFL_PRINT("ProcUpdateRatingbattle::UpdateFunc : JoinCommunity is failed\n");
          m_regSeq = REGSEQ_ERROR;
        }
        else
        {
          m_regSeq = REGSEQ_JOIN_COMMUNITY_WAIT;
        }
      }
      else
      {//全参加完了
        m_regSeq = REGSEQ_OK;
      }
    }
  break;
  case REGSEQ_DELETED:
  case REGSEQ_UPDATE:
    break;
  case REGSEQ_ERROR:
    {
      m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_ConnectionError);
      m_State = STATE_MessageWait;
    }
    break;
  case REGSEQ_SD_FULL:
    {
      m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_Regulation_SDFull);
      m_State = STATE_MessageWait;
    }
    break;
  case REGSEQ_OK:

    if( m_isNewExists || m_isDeleteComplete )
    {
      m_isNewExists = false;
      m_isDeleteComplete = false;
      m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_RegulationUpdated);
      m_regSeq = REGSEQ_UPDATE;
      break;
    }

    return true;
  }

#endif

  return false;
}

/*
if( m_isDeleteComplete )
{
m_regSeq = REGSEQ_OK;
}
else if( m_isNewExists )
{
m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_RegulationUpdated);
m_regSeq = REGSEQ_UPDATE;
}
else
{
m_regSeq = REGSEQ_OK;
}
*/

void Frame_RatingBattleEntry::UpdatePGLCheck( void )
{
  if( s_wait++ < 90 )
  {
    return;
  }

  enum
  {
    PGL_INIT = 0,
    PGL_WAIT,
  };

  switch( m_pglState )
  {
  case PGL_INIT:
    {
      NetAppLib::GameSync::GETPLAYSTATUS_PARAM param;
      std::memset( &param , 0 , sizeof(param) );
      param.pWork = m_pWork;
      param.mode = NetAppLib::GameSync::GETPLAYSTATUS_MODE_RATING;

      NetAppLib::GameSync::GameSyncRequestFacade::AddConfirmPlayStatusRequest( this, &param );
      m_pglState = PGL_WAIT;
    }
  break;
  case PGL_WAIT:
  break;
  }
}


void Frame_RatingBattleEntry::OnConfirmPlayStatusSuccess()
{
  s_wait = 0;
  m_State = STATE_PGLResult;
}

void Frame_RatingBattleEntry::OnConfirmPlayStatusError( u32 errorCode )
{
  s_wait = 0;
  ExitSelf();
}


#ifdef  GF_PLATFORM_CTR

void Frame_RatingBattleEntry::OnBossNewArrivalData( const u32 serialId , const bool isFinalData )
{
  if( m_SerialID != 0 )
  {
    return;
  }

  u32 serialMax = NetAppLib::Util::BossUtility::GetSerialIdMax( NetAppLib::Util::BossUtility::BOSS_APP_TYPE_regulationRate );
  u32 serialMin = NetAppLib::Util::BossUtility::GetSerialIdMin( NetAppLib::Util::BossUtility::BOSS_APP_TYPE_regulationRate );

  if( serialMax < serialId || serialId < serialMin )
  {
    if( isFinalData )
    {
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


void Frame_RatingBattleEntry::OnBossNewArrivalDataEmpty( void )
{
  m_regSeq = REGSEQ_ERROR;
}


bool Frame_RatingBattleEntry::OnBossReadData( const nn::boss::NsDataHeaderInfo* pHeader , const size_t bodySize , const u8* pBody )
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

      GFL_PRINT("Frame_RatingBattleEntry::OnBossReadData fileCount[%d]\n",fileCount);

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

        m_pRegulation[i]->SetData( pBuffer, fileSize );
      }


      //ＳＤカード処理
      for( u32 index = 0 ; index < REGULATION_NUM ; ++index )
      {
        // 受信レギュレーションCRCチェック
        if( m_pRegulation[index]->CheckCrc() == false )
        {//CRC不正
          m_regSeq = REGSEQ_ERROR;
          return true;
        }

        bool isNew = true;
        ExtSavedata::RegulationExtSaveData::REGULATION_TYPE type = static_cast<ExtSavedata::RegulationExtSaveData::REGULATION_TYPE>( ExtSavedata::RegulationExtSaveData::REGULATION_TYPE_BATTLESPOT_RATE_SINGLE + index );
        bool result = m_RegulationExtSaveData.LoadRegulationData( type , *m_pTempRegulation );

        //データ受け渡し
        if( !result )
        {//ロード失敗
          GFL_PRINT("OnBossReadData:error LoadRegulationData is failed[%d]\n",index);
          if( m_RegulationExtSaveData.IsFatalError() )
          {//Fatal
            // 即座に終了させてFatalErrorイベントにする
            m_regSeq = REGSEQ_ERROR;
            return true;
          }
          else
          {//こわれている
            //ファイルを削除
            if( m_RegulationExtSaveData.DeleteRegulationData( type ) == false )
            {//失敗
              if( m_RegulationExtSaveData.IsFatalError() )
              {//Fatal
                // 即座に終了させてFatalErrorイベントにする
                m_regSeq = REGSEQ_ERROR;
                return true;
              }
            }
            else
            {//成功
              m_isDeleteComplete = true;
            }
          }
        }
        else
        {//ロード成功
          if( m_pRegulation[index]->GetVersion() == m_pTempRegulation->GetVersion() )
          {//同じレギュレーション
            isNew = false;
          }
        }

        if( isNew )
        {//新しいレギュレーション
          bool saveResult = m_RegulationExtSaveData.SaveRegulationData( type , *m_pRegulation[index] );
          if( !saveResult )
          {//セーブ失敗
            GFL_PRINT("ProcUpdateFreebattle::OnBossReadData:error SaveRegulationData is failed[%d]\n",index);
            if( m_RegulationExtSaveData.IsFatalError() )
            {//Fatal
              // 即座に終了させてFatalErrorイベントにする
              m_regSeq = REGSEQ_ERROR;

              return true;
            }
            else
            {//空き容量足りない

              m_regSeq = REGSEQ_SD_FULL;
              return true;
            }
          }
          else
          {//更新された
            m_isNewExists = true;
          }
        }

        m_communityIDArray[index] = m_pRegulation[index]->GetValueParam( Regulation::DISCRIMINANT_ID );
      }

      m_regSeq = REGSEQ_FIND_MY_COMMUNITY;
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


void Frame_RatingBattleEntry::OnBossRecvFileList( const char* pTaskName , const nn::boss::NsaList::NsaInformation* pFileList , const u32 fileNum )
{
  m_regSeq = REGSEQ_ERROR;
}


void Frame_RatingBattleEntry::OnBossRecvFileListFailed( const char* pTaskName , const nn::Result& result )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultBossError( result );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  m_regSeq = REGSEQ_ERROR;
}


void Frame_RatingBattleEntry::OnBossError( const nn::Result& result )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultBossError( result );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  m_regSeq = REGSEQ_ERROR;
}


void Frame_RatingBattleEntry::OnBossTaskError( const char* pTaskName , const u32 errorCode )
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
void Frame_RatingBattleEntry::OnCommunitySearchSuccess( nn::nex::qList<nn::nex::Community>* communityList )
{
  GFL_PRINT("ProcUpdateRatingbattle::OnCommunitySearchSuccess\n");

  if( communityList == NULL )
  {//リストがNULL
    GFL_PRINT("ProcUpdateRatingbattle::OnCommunitySearchSuccess:error List is NULL\n");
    m_regSeq = REGSEQ_ERROR;
  }
  else if( m_regSeq == REGSEQ_FIND_MY_COMMUNITY_WAIT )
  {//自分のコミュニティ検索結果
    CheckMyCommunityList( communityList );
  }
  else if( m_regSeq == REGSEQ_FIND_OFFICIALCOMMUNITY_WAIT )
  {//公式コミュニティ検索結果
    CheckOfficialCommunityList( communityList );
  }
  else
  {//シーケンスがおかしい
    GFL_PRINT("ProcUpdateRatingbattle::OnCommunitySearchSuccess:error Invalid SEQ[%d]\n",m_regSeq);
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
void Frame_RatingBattleEntry::OnCommunityJoinSuccess( const nn::nex::GatheringID& gatheringId )
{
  GFL_PRINT("ProcUpdateRatingbattle::OnCommunityJoinSuccess[%u]\n",gatheringId);

  m_regSeq = REGSEQ_JOIN_COMMUNITY;
}

//------------------------------------------------------------------
/**
* @brief コミュニティの退出完了通知
*
* @param[in] gatheringId 退出したギャザリングID
*/
//------------------------------------------------------------------
void Frame_RatingBattleEntry::OnCommunityLeaveSuccess( const nn::nex::GatheringID& gatheringId )
{
  GFL_PRINT("ProcUpdateRatingbattle::OnCommunityLeaveSuccess[%u]\n",gatheringId);

  m_regSeq = REGSEQ_LEAVE_COMMUNITY;
}


//------------------------------------------------------------------
/**
* @brief コミュニティAPIのエラー終了通知
*
* @param[in] result    エラー理由
* @param[in] errorCode エラーアプレットに渡すエラーコード
*/
//------------------------------------------------------------------
void Frame_RatingBattleEntry::OnNexCommunityError( const nn::nex::qResult& result, u32 errorCode )
{
  GFL_PRINT("ProcUpdateRatingbattle::OnNexCommunityError\n");
  nn::nex::StringStream ss;
  ss << _T("errorCode:") << errorCode << _T("\n") << result;
  GFL_PRINT("OnNexCommunityError[%ls]\n", ss.CStr());

  m_regSeq = REGSEQ_ERROR;

  // エラーアプレット表示
  NetLib::Error::NijiNetworkError error;
  error.SetResultNex( result );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
}


//------------------------------------------------------------------
/**
* @brief 自分が参加しているコミュニティの検索結果をチェック
*
* @param[in] communityList 検索されたコミュニティリスト
*/
//------------------------------------------------------------------
void Frame_RatingBattleEntry::CheckMyCommunityList( nn::nex::qList<nn::nex::Community>* communityList )
{
  if( communityList && !communityList->empty() )
  {//コミュニティが空ではない
    m_leaveCommunityNum = 0;
    u32 count = 0;
    nn::nex::qList<nn::nex::Community>::const_iterator it = communityList->begin();
    while(it != communityList->end())
    {
      GFL_PRINT( "FindMyGatheringID[%u]" , it->GetID() );
      u32 communityID = it->GetAttribute( AT_INDEX_communityID );
      GFL_PRINT( "FindMyCommunityID[%u]\n" , communityID );
      if( count >= MAX_MY_COMMUNITY_NUM )
      {//コミュニティ最大数オーバー
        GFL_PRINT("ProcUpdateRatingbattle::_CheckMyCommunityList:error Invalid count[%d]\n",count);
        m_regSeq = REGSEQ_ERROR;
        count = 0;
        break;
      }
      m_myCommunityIDArray[count] = communityID;

      //脱退が必要なコミュニティの判定
      bool isExists = false;
      for( u32 index = 0 ; index < REGULATION_NUM ; ++index )
      {
        if( m_communityIDArray[index] == communityID )
        {//まだ有効なコミュニティ
          isExists = true;
          //コミュニティが有効だったのでギャザリングIDを保存
          GFL_PRINT("ProcUpdateRatingbattle::_CheckMyCommunityList : m_net->SetGatheringID([%d],[%u])\n",index,it->GetID());
          m_pWork->SetGatheringID( index, it->GetID() );

          //時間を保存しておく
          nn::nex::DateTime* pDateTime = m_pWork->GetCommunityEndDateTime( index );
          *pDateTime = it->GetParticipationEndDate();

          GFL_PRINT("nn::nex::DateTime[%d][%d][%d][%d]\n" , pDateTime->GetYear(),pDateTime->GetMonth(),pDateTime->GetDay(),pDateTime->GetHour() );

          break;
        }
      }

      // インターネット大会のコミュニティは脱退しない
      {
        // インターネット大会のコミュニティＩＤを取得する
        u32 netCommunityID = m_pWork->m_pBattleSpotSave->GetEntryCommunityId( Savedata::BattleSpotData::ENTRY_COMMUNITY_NET );

        if( communityID == netCommunityID )
        {
          GFL_PRINT("%d は大会用のコミュニティなので脱退しない\n", communityID );
          isExists = true;
        }
      }

      if( !isExists )
      {//脱退リストに追加
        GFL_PRINT("ProcUpdateRatingbattle::OnCommunitySearchSuccess:Add Leave List[%d][%d]\n",communityID,it->GetID());
        if( m_leaveCommunityNum < MAX_MY_COMMUNITY_NUM )
        {
          m_leaveGatheringID[m_leaveCommunityNum] = it->GetID();
          m_leaveCommunityNum++;
        }
      }

      ++it;
      ++count;
    }
  }

  //参加が必要なコミュニティの判定
  for( u32 regulationIndex = 0 ; regulationIndex < REGULATION_NUM ; ++regulationIndex )
  {
    if( m_communityIDArray[regulationIndex] == 0 )//コミュニティID０は使用禁止
    {
      continue;
    }

    bool isExists = false;
    for( u32 index = 0 ; index < MAX_MY_COMMUNITY_NUM ; ++index )
    {
      if( m_communityIDArray[regulationIndex] == m_myCommunityIDArray[index] )
      {//レギュレーションにあるコミュニティに参加済み
        isExists = true;
        break;
      }
    }

    if( !isExists )
    {//参加リストに追加
      GFL_PRINT("ProcUpdateRatingbattle::OnCommunitySearchSuccess:Add Join List[%d]\n",m_communityIDArray[regulationIndex]);
      m_joinCommunityID[m_joinCommunityNum] = m_communityIDArray[regulationIndex];
      m_joinCommunityNum++;
    }
  }

  if( m_leaveCommunityNum > 0 )
  {//退出シーケンスへ
    m_regSeq = REGSEQ_LEAVE_COMMUNITY;
  }
  else if( m_joinCommunityNum > 0 )
  {//参加シーケンスへ
    m_regSeq = REGSEQ_FIND_OFFICIALCOMMUNITY;
  }
  else
  {//更新完了
    m_regSeq = REGSEQ_OK;
  }
}


//------------------------------------------------------------------
/**
* @brief 参加可能な公式コミュニティの検索結果をチェック
*
* @param[in] communityList 検索されたコミュニティリスト
*/
//------------------------------------------------------------------
void Frame_RatingBattleEntry::CheckOfficialCommunityList( nn::nex::qList<nn::nex::Community>* communityList )
{
  if( communityList && !communityList->empty() )
  {//コミュニティが空ではない
    nn::nex::qList<nn::nex::Community>::const_iterator it = communityList->begin();
    u32 count = 0;
    while(it != communityList->end())
    {
      GFL_PRINT( "FindGatheringID[%u]" , it->GetID() );
      u32 communityID = it->GetAttribute( AT_INDEX_communityID );
      GFL_PRINT( "FindCommunityID[%u]\n" , communityID );
      for( u32 i = 0 ; i < m_joinCommunityNum ; ++i )
      {
        if( m_joinCommunityID[i] == communityID )
        {//参加すべきコミュニティを発見
          GFL_PRINT( "UpdateGatheringID[%u][%d]\n" , communityID,it->GetID() );
          m_joinGatheringID[i] = it->GetID();
          count++;

          for( u32 index = 0 ; index < REGULATION_NUM ; ++index )
          {//レギュレーションの中のコミュニティから探す
            if( m_communityIDArray[index] == communityID )
            {//レギュレーションの中のコミュニティと一致
              //ギャザリングIDを保存
              GFL_PRINT("ProcUpdateRatingbattle::_CheckOfficialCommunityList : m_net->SetGatheringID([%d],[%u])\n",index,it->GetID());
              m_pWork->SetGatheringID( index, it->GetID() );

              //時間を保存しておく
              nn::nex::DateTime* pDateTime = m_pWork->GetCommunityEndDateTime( index );
              *pDateTime = it->GetParticipationEndDate();

              GFL_PRINT("nn::nex::DateTime[%d][%d][%d][%d]\n" , pDateTime->GetYear(),pDateTime->GetMonth(),pDateTime->GetDay(),pDateTime->GetHour() );

              break;
            }
          }
        }
      }
      ++it;
    }

    if( count == m_joinCommunityNum )
    {//参加シーケンスへ
      m_regSeq = REGSEQ_JOIN_COMMUNITY;
    }
    else
    {//数が合わないけど進行（大会コミュニティなので）
      GFL_PRINT("ProcUpdateRatingbattle::OnCommunitySearchSuccess not match count[%d][%d]\n",count,m_joinCommunityNum);
      m_regSeq = REGSEQ_JOIN_COMMUNITY;
    }
  }
  else
  {//エラー
    GFL_PRINT("ProcUpdateRatingbattle::OnCommunitySearchSuccess empty\n");
    m_regSeq = REGSEQ_ERROR;
  }
}

#endif


} /*  namespace Live    */
} /*  namespace NetApp  */


