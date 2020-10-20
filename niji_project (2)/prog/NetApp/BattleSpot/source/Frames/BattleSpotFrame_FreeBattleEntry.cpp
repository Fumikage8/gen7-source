//==============================================================================
/**
 * @file   BattleSpotFrame_FreeBattleEntry.cpp
 * @date   2015/10/02 Fri. 13:32:57
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

#include  "./BattleSpotFrame_FreeBattleEntry.h"

#include <gflnet2/include/boss/gflnet2_BossManager.h>

#include "NetStatic/NetAppLib/include/Util/NetAppBossUtility.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
#include "NetStatic/NetAppLib/include/Util/NetAppPackFile.h"
// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,

    STATE_Selection,
    STATE_LegendSelection,
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
  *  @func     Frame_FreeBattleEntry
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_FreeBattleEntry::Frame_FreeBattleEntry(BattleSpotWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->m_pViewContainer)
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_pUpperView(NULL)
  , m_pLowerView(NULL)
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
}


//------------------------------------------------------------------
/**
  *  @func     Frame_FreeBattleEntry
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_FreeBattleEntry::~Frame_FreeBattleEntry()
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
bool  Frame_FreeBattleEntry::startup()
{
  m_pViewContainer->SetAllInputEnabled( true );
  m_pViewContainer->SetScene(BattleSpotViewContainer::SCENE_FreeBattleEntry);

  /*  views  */
  {
    m_pUpperView  = m_pViewContainer->m_pUpperView_Information;
    m_pLowerView  = m_pViewContainer->m_pLowerView_Selection;

    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_SDCard);
    m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_None);
  }

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
void  Frame_FreeBattleEntry::setListener()
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
void  Frame_FreeBattleEntry::removeListener()
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
void  Frame_FreeBattleEntry::updateSequence()
{
  switch(m_State)
  {
    case  STATE_Entry:
      {
        if( UpdateRegulation() )
        {
          m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_FreeBattle);
          m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_FreeBattle_SelectRule);
          m_State =  STATE_Selection;
        }
      }
      break;

    case  STATE_Selection:
      break;

    case  STATE_LegendSelection:
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
bool  Frame_FreeBattleEntry::cleanup()
{
  bool  bAllDone = true;

#ifdef  GF_PLATFORM_CTR
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
void  Frame_FreeBattleEntry::LowerView_Selection_OnEvent(LowerView_Selection* pSender, const LowerView_Selection::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Selection::IEventHandler::EVENT_FreeBattle_Rule_SingleBattle:          
      {
        m_pWork->m_pAppParam->m_freeRuleCurorIndex = 0;
        m_pWork->SetRule( BattleSpotWork::RULE_FREE_SINGLE );
        m_State =  STATE_LegendSelection;
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_FreeBattle_PokemonRule);
      }
    break;
    case  LowerView_Selection::IEventHandler::EVENT_FreeBattle_Rule_DoubleBattle:      
      {
        m_pWork->m_pAppParam->m_freeRuleCurorIndex = 1;
        m_pWork->SetRule( BattleSpotWork::RULE_FREE_DOUBLE );
        m_State =  STATE_LegendSelection;
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_FreeBattle_PokemonRule);
      }
    break;
    case  LowerView_Selection::IEventHandler::EVENT_FreeBattle_Rule_BattleRoyal:   
      {
        m_pWork->m_pAppParam->m_freeRuleCurorIndex = 2;
        m_pWork->SetRule( BattleSpotWork::RULE_FREE_ROYAL );
        m_State =  STATE_LegendSelection;
        m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_FreeBattle_PokemonRule);
      }
    break;
    case  LowerView_Selection::IEventHandler::EVENT_FreeBattle_Pokemon_SpecialPokemonEnable:   
      {
        m_pWork->m_pAppParam->m_freeLegendCurorIndex = 0;
        m_pWork->SetLegendEnable( true );
        {
          //m_pWork->m_pAppParam->m_pRegulationDrawInfo->SetRegulation(); 
          switch( m_pWork->GetRule() )
          {
          case BattleSpotWork::RULE_FREE_DOUBLE:
            {
              u8* tempBuffer = GFL_NEW_ARRAY( m_pWork->GetDeviceHeap()->GetLowerHandle() ) u8[m_pRegulation[REGULATION_DOUBLE_L]->GetSerializeDataSize()];
              m_pRegulation[REGULATION_DOUBLE_L]->Serialize( tempBuffer );
              m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().Deserialize( tempBuffer , m_pRegulation[REGULATION_DOUBLE_L]->GetSerializeDataSize() );
              GFL_SAFE_DELETE_ARRAY( tempBuffer );
            }
          break;
          case BattleSpotWork::RULE_FREE_ROYAL:
            {
              u8* tempBuffer = GFL_NEW_ARRAY( m_pWork->GetDeviceHeap()->GetLowerHandle() ) u8[m_pRegulation[REGULATION_ROYAL_L]->GetSerializeDataSize()];
              m_pRegulation[REGULATION_ROYAL_L]->Serialize( tempBuffer );
              m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().Deserialize( tempBuffer , m_pRegulation[REGULATION_ROYAL_L]->GetSerializeDataSize() );
              GFL_SAFE_DELETE_ARRAY( tempBuffer );
            }
          break;
          case BattleSpotWork::RULE_FREE_SINGLE:
          default:
            {
              u8* tempBuffer = GFL_NEW_ARRAY( m_pWork->GetDeviceHeap()->GetLowerHandle() ) u8[m_pRegulation[REGULATION_SINGLE_L]->GetSerializeDataSize()];
              m_pRegulation[REGULATION_SINGLE_L]->Serialize( tempBuffer );
              m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().Deserialize( tempBuffer , m_pRegulation[REGULATION_SINGLE_L]->GetSerializeDataSize() );
              GFL_SAFE_DELETE_ARRAY( tempBuffer );
            }
          break;
          }
        }
        m_pWork->m_pAppParam->nextApp = NEXT_APP_TEAM_SELECT;
        m_pWork->m_pAppParam->nextFrame = FRAME_FreeBattleMatching;
        ExitSelf(RESULT_FreeBattleEntry_Next);
      }
    break;
    case  LowerView_Selection::IEventHandler::EVENT_FreeBattle_Pokemon_SpecialPokemonDisable:   
      {
        m_pWork->m_pAppParam->m_freeLegendCurorIndex = 1;
        m_pWork->SetLegendEnable( false );
        {
          //m_pWork->m_pAppParam->m_pRegulationDrawInfo->SetRegulation(); 
          switch( m_pWork->GetRule() )
          {
          case BattleSpotWork::RULE_FREE_DOUBLE:
            {
              u8* tempBuffer = GFL_NEW_ARRAY( m_pWork->GetDeviceHeap()->GetLowerHandle() ) u8[m_pRegulation[REGULATION_DOUBLE]->GetSerializeDataSize()];
              m_pRegulation[REGULATION_DOUBLE]->Serialize( tempBuffer );
              m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().Deserialize( tempBuffer , m_pRegulation[REGULATION_DOUBLE]->GetSerializeDataSize() );
              GFL_SAFE_DELETE_ARRAY( tempBuffer );
            }
          break;
          case BattleSpotWork::RULE_FREE_ROYAL:
            {
              u8* tempBuffer = GFL_NEW_ARRAY( m_pWork->GetDeviceHeap()->GetLowerHandle() ) u8[m_pRegulation[REGULATION_ROYAL]->GetSerializeDataSize()];
              m_pRegulation[REGULATION_ROYAL]->Serialize( tempBuffer );
              m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().Deserialize( tempBuffer , m_pRegulation[REGULATION_ROYAL]->GetSerializeDataSize() );
              GFL_SAFE_DELETE_ARRAY( tempBuffer );
            }
          break;
          case BattleSpotWork::RULE_FREE_SINGLE:
          default:
            {
              u8* tempBuffer = GFL_NEW_ARRAY( m_pWork->GetDeviceHeap()->GetLowerHandle() ) u8[m_pRegulation[REGULATION_SINGLE]->GetSerializeDataSize()];
              m_pRegulation[REGULATION_SINGLE]->Serialize( tempBuffer );
              m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().Deserialize( tempBuffer , m_pRegulation[REGULATION_SINGLE]->GetSerializeDataSize() );
              GFL_SAFE_DELETE_ARRAY( tempBuffer );
            }
          break;
          }
        }
        m_pWork->m_pAppParam->nextApp = NEXT_APP_TEAM_SELECT;
        m_pWork->m_pAppParam->nextFrame = FRAME_FreeBattleMatching;
        ExitSelf(RESULT_FreeBattleEntry_Next);
      }
    break;
    case  LowerView_Selection::IEventHandler::EVENT_Message_EOM:
      if( m_regSeq == REGSEQ_RETURN_WAIT )
      {
        ExitSelf();
      }
      else
      {
        m_regSeq = REGSEQ_OK;
      }
    break;
    case  LowerView_Selection::IEventHandler::EVENT_Back:             
      {
        ExitSelf();
      }
    break;
  }

  
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_FreeBattleEntry::ExitSelf(const FrameResult frameResult)
{
  m_pViewContainer->SetAllInputEnabled( false );
  m_Result  = frameResult;
  m_State   = STATE_Exit;
}

bool Frame_FreeBattleEntry::UpdateRegulation()
{

#ifdef  GF_PLATFORM_CTR

  switch( m_regSeq )
  {
  case REGSEQ_INIT:
    {
      nn::Result nnResult = GFL_SINGLETON_INSTANCE( gflnet2::boss::BossManager )->StartImmidiateTask( gfl2::thread::Thread::THREAD_PRI_NORMAL + 1, NetAppLib::Util::BossUtility::GetFreeRagulationFileName() , NULL , NetAppLib::Util::BossUtility::GetRegionURL() );

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
  case REGSEQ_DELETED:
  case REGSEQ_UPDATE:
  break;
  case REGSEQ_RETURN_WAIT:
  break;
  case REGSEQ_ERROR:
    {
      m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_ConnectionError);
      m_regSeq = REGSEQ_RETURN_WAIT;
    }
  break;
  case REGSEQ_SD_FULL:
    {
      m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_Regulation_SDFull);
      m_regSeq = REGSEQ_RETURN_WAIT;
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

#ifdef  GF_PLATFORM_CTR

void Frame_FreeBattleEntry::OnBossNewArrivalData( const u32 serialId , const bool isFinalData )
{
  if( m_SerialID != 0 )
  {
    return;
  }

  u32 serialMax = NetAppLib::Util::BossUtility::GetSerialIdMax( NetAppLib::Util::BossUtility::BOSS_APP_TYPE_regulation );
  u32 serialMin = NetAppLib::Util::BossUtility::GetSerialIdMin( NetAppLib::Util::BossUtility::BOSS_APP_TYPE_regulation );

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


void Frame_FreeBattleEntry::OnBossNewArrivalDataEmpty( void )
{
  m_regSeq = REGSEQ_ERROR;
}


bool Frame_FreeBattleEntry::OnBossReadData( const nn::boss::NsDataHeaderInfo* pHeader , const size_t bodySize , const u8* pBody )
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

      GFL_PRINT("Frame_FreeBattleEntry::OnBossReadData fileCount[%d]\n",fileCount);

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
        ExtSavedata::RegulationExtSaveData::REGULATION_TYPE type = static_cast<ExtSavedata::RegulationExtSaveData::REGULATION_TYPE>( ExtSavedata::RegulationExtSaveData::REGULATION_TYPE_BATTLESPOT_FREE_SINGLE + index );
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
      }

      m_regSeq = REGSEQ_OK;
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


void Frame_FreeBattleEntry::OnBossRecvFileList( const char* pTaskName , const nn::boss::NsaList::NsaInformation* pFileList , const u32 fileNum )
{
  m_regSeq = REGSEQ_ERROR;
}


void Frame_FreeBattleEntry::OnBossRecvFileListFailed( const char* pTaskName , const nn::Result& result )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultBossError( result );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  m_regSeq = REGSEQ_ERROR;
}


void Frame_FreeBattleEntry::OnBossError( const nn::Result& result )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultBossError( result );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  m_regSeq = REGSEQ_ERROR;
}


void Frame_FreeBattleEntry::OnBossTaskError( const char* pTaskName , const u32 errorCode )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultBossTaskError( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  m_regSeq = REGSEQ_ERROR;
}

#endif

} /*  namespace Live    */
} /*  namespace NetApp  */


