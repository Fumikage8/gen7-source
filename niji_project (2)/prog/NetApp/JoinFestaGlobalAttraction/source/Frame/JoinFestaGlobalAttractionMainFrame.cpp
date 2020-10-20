// ============================================================================
/*
 * @file JoinFestaGlobalAttractionMainFrame.cpp
 * @brief グローバルアトラクションアプリの○○プロセスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaGlobalAttraction/source/Frame/JoinFestaGlobalAttractionMainFrame.h"
#include "NetApp/JoinFestaGlobalAttraction/source/JoinFestaGlobalAttractionFrameResult.h"

#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionBGUpperView.h"
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionInfoUpperView.h"
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionBGLowerView.h"
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionSelectListLowerView.h"
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionDetailLowerView.h"
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionMessageMenuLowerView.h"

#include "NetApp/JoinFestaGlobalAttraction/source/RequestSequence/JoinFestaGlobalAttractionRequestFacade.h"

#include "NetStatic/NetAppLib/include/GameSync/GameSyncRequestFacade.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include <niji_conv_header/message/msg_jf_attraction.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaGlobalAttractionMainFrame::JoinFestaGlobalAttractionMainFrame(
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork,
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewSystemAccessor* pJoinFestaGlobalAttractionViewSystemAccessor ) :
  NetApp::JoinFestaGlobalAttraction::Frame::JoinFestaGlobalAttractionFrameBase( pJoinFestaGlobalAttractionWork, pJoinFestaGlobalAttractionViewSystemAccessor ),
  m_JoinFestaGlobalAttractionMessageMenuLowerViewListener(),
  m_JoinFestaGlobalAttractionDetailLowerViewListener(),
  m_JoinFestaGlobalAttractionSelectListLowerViewListener(),
  m_pEntryComfirmGlobalAttractionInfo( NULL ),
  m_bDownloadError( false )
{
}


JoinFestaGlobalAttractionMainFrame::~JoinFestaGlobalAttractionMainFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool JoinFestaGlobalAttractionMainFrame::startup()
{
  m_pJoinFestaGlobalAttractionViewObjects->InputDisableAll();
  m_pJoinFestaGlobalAttractionViewList->RemoveAllView();

  m_pJoinFestaGlobalAttractionViewList->AddUpperView( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionBGUpperView() );

  m_pJoinFestaGlobalAttractionViewList->AddLowerView( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionBGLowerView() );
  m_pJoinFestaGlobalAttractionViewList->AddLowerView( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionMessageMenuLowerView() );

  NetApp::JoinFestaGlobalAttraction::RequestSequence::JoinFestaGlobalAttractionRequestFacade::AddDownloadRequest( this, m_pJoinFestaGlobalAttractionWork );

  m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionMessageMenuLowerView()->SetMessage( jf_attraction_win_01 );

  return true;
}


void JoinFestaGlobalAttractionMainFrame::setListener()
{
  m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionMessageMenuLowerView()->SetListener( &m_JoinFestaGlobalAttractionMessageMenuLowerViewListener );
  m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionDetailLowerView()->SetListener( &m_JoinFestaGlobalAttractionDetailLowerViewListener );
  m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionSelectListLowerView()->SetListener( &m_JoinFestaGlobalAttractionSelectListLowerViewListener );

  m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionMessageMenuLowerView()->SetInputEnabled( true );
}


void JoinFestaGlobalAttractionMainFrame::removeListener()
{
  m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionMessageMenuLowerView()->RemoveListener();
  m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionDetailLowerView()->RemoveListener();
  m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionSelectListLowerView()->RemoveListener();
}


void JoinFestaGlobalAttractionMainFrame::updateSequence()
{
  // ------------------------------------------------------------------------------------------
  {
    u32  inputEventID    = m_JoinFestaGlobalAttractionMessageMenuLowerViewListener.GetInputEventID();
    u32  messageID       = m_JoinFestaGlobalAttractionMessageMenuLowerViewListener.GetMessageID();
    bool bTouchScreen    = ( inputEventID == NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN ) ? true : false;
    bool bTouchYesButton = ( inputEventID == NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_YES_BUTTON ) ? true : false;
    bool bTouchNoButton  = ( inputEventID == NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_NO_BUTTON ) ? true : false;

    switch( messageID )
    {
      case jf_attraction_win_03:
      {
        if( bTouchYesButton )
        {
          bool bPGLAccessLockFlag = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync()->CheckLockGameSyncSaveFor5xxError();

          if( bPGLAccessLockFlag )
          {
            m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_attraction_win_09 );
          }
          else
          {
            registCompleteGlobalAttraction( m_pEntryComfirmGlobalAttractionInfo );
          }
        }
        else
        if( bTouchNoButton )
        {
          setupMainScreen();
        }
      }
      break;

      case jf_attraction_win_04:
      {
        if( bTouchScreen )
        {
          checkNotGetReward();
        }
      }
      break;

      case jf_attraction_win_05:
      {
        if( bTouchScreen )
        {
          gameSyncRequest();
        }
      }
      break;

      case jf_attraction_win_06:
      {
        if( bTouchScreen )
        {
          if( m_bDownloadError )
          {
            exitFrame( NetApp::JoinFestaGlobalAttraction::MAIN_FRAME_RESULT_BACK );
          }
          else
          {
            if( m_pJoinFestaGlobalAttractionWork->IsOpenGlobalAttractionHistory() )
            {
              gameSyncRequest();
            }
            else
            {
              exitFrame( NetApp::JoinFestaGlobalAttraction::MAIN_FRAME_RESULT_BACK );
            }
          }
        }
      }
      break;

      case jf_attraction_win_07:
      case jf_attraction_win_08:
      case jf_attraction_win_09:
      {
        if( bTouchScreen )
        {
          setupMainScreen();
        }
      }
      break;

    }
  }

  // ------------------------------------------------------------------------------------------
  {
    u32  inputEventID              = m_JoinFestaGlobalAttractionSelectListLowerViewListener.GetInputEventID();
    bool bIsSelectGlobalAttraction = ( inputEventID == NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionSelectListLowerViewListener::INPUT_EVENT_ID_SELECT_GLOBAL_ATTRACTION ) ? true : false;
    bool bTouchBackButton          = ( inputEventID == NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionSelectListLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
    NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pActiveGlobalAttractionInfo = m_JoinFestaGlobalAttractionSelectListLowerViewListener.GetActiveGlobalAttractionInfo();

    if( pActiveGlobalAttractionInfo )
    {
      m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionInfoUpperView()->SetGlobalAttractionInfo( pActiveGlobalAttractionInfo );
    }

    if( bTouchBackButton )
    {
      exitFrame( NetApp::JoinFestaGlobalAttraction::MAIN_FRAME_RESULT_BACK );
    }
    else
    if( bIsSelectGlobalAttraction )
    {
      NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pSelectGlobalAttractionInfo = m_JoinFestaGlobalAttractionSelectListLowerViewListener.GetSelectGlobalAttractionInfo();

      if( pSelectGlobalAttractionInfo )
      {
        if( m_pJoinFestaGlobalAttractionWork->IsOpenGlobalAttraction( pSelectGlobalAttractionInfo ) && !isRegistEntry( pSelectGlobalAttractionInfo ) )
        {
          entryComfirmGlobalAttraction( pSelectGlobalAttractionInfo );
        }
        else
        {
          setupDetailScreen( pSelectGlobalAttractionInfo );
        }
      }
    }
  }

  // ------------------------------------------------------------------------------------------
  {
    u32 inputEventID = m_JoinFestaGlobalAttractionDetailLowerViewListener.GetInputEventID();
    bool bTouchBackButton = ( inputEventID == NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionDetailLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;

    if( bTouchBackButton )
    {
      setupMainScreen();
    }
  }

  m_JoinFestaGlobalAttractionMessageMenuLowerViewListener.ResetInputEvent();
  m_JoinFestaGlobalAttractionDetailLowerViewListener.ResetInputEvent();
  m_JoinFestaGlobalAttractionSelectListLowerViewListener.ResetInputEvent();
}


void JoinFestaGlobalAttractionMainFrame::gameSyncRequest()
{
  GFL_PRINT( "JoinFestaGlobalAttractionMainFrame::gameSyncRequest \n" );

  NetAppLib::GameSync::GAMESYNC_PARAM param;
  std::memset( &param , 0 , sizeof(param) );
  param.pWork = m_pJoinFestaGlobalAttractionWork;

  NetAppLib::GameSync::GameSyncRequestFacade::AddSaveDataSyncRequest( this, &param );

  m_pJoinFestaGlobalAttractionViewList->RemoveAllView();

  m_pJoinFestaGlobalAttractionViewList->AddUpperView( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionBGUpperView() );

  m_pJoinFestaGlobalAttractionViewList->AddLowerView( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionBGLowerView() );
  m_pJoinFestaGlobalAttractionViewList->AddLowerViewInputEnable( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionMessageMenuLowerView() );

  m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionMessageMenuLowerView()->SetMessage( jf_attraction_win_02 );
}


void JoinFestaGlobalAttractionMainFrame::checkNotGetReward()
{
  GFL_PRINT( "JoinFestaGlobalAttractionMainFrame::checkNotGetReward \n" );

  bool bIsNotGetReward = false;

  Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pDownloadGlobalAttractionInfoList = m_pJoinFestaGlobalAttractionWork->GetDownloadGlobalAttractionInfoList();

  for( int i = 0; i < NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::RECORD_MAX; ++i )
  {
    NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData* pGlobalAttractionSaveData = pJoinFestaDataSave->GetGlobalAttractionData( static_cast<u8>(i) );
    
    if( pGlobalAttractionSaveData )
    {
      // 受け取れるご褒美の計算
      {
        // 開催終了で集計完了
        if( pGlobalAttractionSaveData->openState == NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::OPEN_STATE_TOTAL_FINISH )
        {
          // 参加登録した
          if( pGlobalAttractionSaveData->participateState == NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::PARTICIPATE_STATE_OK )
          {
            // @fix GFNMCat[4026] もらえる報酬の値が未設定の時のみ更新(2回目以降に変動させない)
            if( pGlobalAttractionSaveData->rewardValue == 0 )
            {
              if( i < static_cast<int>( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::DATA_MAX ) )
              {
                u16 rewardValue = 0;

                // 達成できた
                if( pGlobalAttractionSaveData->totalRecord >= pDownloadGlobalAttractionInfoList[i].successRecord )
                {
                  rewardValue = pDownloadGlobalAttractionInfoList[i].successReward;
                }
                // 達成できなかった
                else
                {
                  rewardValue = pDownloadGlobalAttractionInfoList[i].failureReward;
                }

                // PGL会員の時は2倍
                if( pGlobalAttractionSaveData->isPglMember == 1 )
                {
                  rewardValue *= 2;
                }

                pGlobalAttractionSaveData->rewardValue = rewardValue;

                pJoinFestaDataSave->SetGlobalAttractionData( static_cast<u8>(i), pGlobalAttractionSaveData );
              }
            }
          }
        }
      }

      // ご褒美が受け取れる状態
      if( pGlobalAttractionSaveData->rewardValue > 0 && pGlobalAttractionSaveData->rewardState == 0 )
      {
        bIsNotGetReward = true;
      }
    }
  }

  if( bIsNotGetReward )
  {
    notGetRewardMessage();
  }
  else
  {
    setupMainScreen();
  }
}


//! 参加していないかチェック
bool JoinFestaGlobalAttractionMainFrame::isRegistEntry( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pGlobalAttractionInfo )
{
  GFL_PRINT( "JoinFestaGlobalAttractionMainFrame::isRegistEntry \n" );

  bool bIsEntry = false;

  int index = m_pJoinFestaGlobalAttractionWork->IDToIndex( pGlobalAttractionInfo );

  if( index >= 0 )
  {
    Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

    NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData* pGlobalAttractionSaveData = pJoinFestaDataSave->GetGlobalAttractionData( static_cast<u8>( index ) );

    if( pGlobalAttractionSaveData )
    {
      GFL_PRINT( "index = %d, participateState = %d \n", index, pGlobalAttractionSaveData->participateState );

      if( pGlobalAttractionSaveData->participateState == NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::PARTICIPATE_STATE_NULL )
      {
        bIsEntry = false;
      }
      else
      {
        bIsEntry = true;
      }
    }
  }

  return bIsEntry;
}


void JoinFestaGlobalAttractionMainFrame::entryComfirmGlobalAttraction( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pGlobalAttractionInfo )
{
  GFL_PRINT( "JoinFestaGlobalAttractionMainFrame::entryComfirmGlobalAttraction \n" );

  m_pEntryComfirmGlobalAttractionInfo = pGlobalAttractionInfo;

  m_pJoinFestaGlobalAttractionViewList->RemoveAllView();

  m_pJoinFestaGlobalAttractionViewList->AddUpperView( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionBGUpperView() );
  m_pJoinFestaGlobalAttractionViewList->AddUpperView( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionInfoUpperView() );

  m_pJoinFestaGlobalAttractionViewList->AddLowerView( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionBGLowerView() );
  m_pJoinFestaGlobalAttractionViewList->AddLowerViewInputEnable( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionMessageMenuLowerView() );

  m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionInfoUpperView()->SetGlobalAttractionInfo( pGlobalAttractionInfo );
  m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionMessageMenuLowerView()->SetMessageYesNoBlackFilter( jf_attraction_win_03 );

}


void JoinFestaGlobalAttractionMainFrame::registCompleteGlobalAttraction( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pGlobalAttractionInfo )
{
  GFL_PRINT( "JoinFestaGlobalAttractionMainFrame::registCompleteGlobalAttraction \n" );

  // 参加登録状態にする
  {
    int index = m_pJoinFestaGlobalAttractionWork->IDToIndex( pGlobalAttractionInfo );

    if( index >= 0 )
    {
      Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

      NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData* pGlobalAttractionSaveData = pJoinFestaDataSave->GetGlobalAttractionData( static_cast<u8>( index ) );

      if( pGlobalAttractionSaveData )
      {
        NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData copyData;
        
        gfl2::std::MemCopy( pGlobalAttractionSaveData, &copyData, sizeof( NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData ) );

        copyData.participateState = NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::PARTICIPATE_STATE_OK;

        pJoinFestaDataSave->SetGlobalAttractionData( static_cast<u8>( index ), &copyData );

        GFL_PRINT( "index = %d, participateState = %d \n", index, copyData.participateState );
      }
    }
  }


  m_pJoinFestaGlobalAttractionViewList->RemoveAllView();

  m_pJoinFestaGlobalAttractionViewList->AddUpperView( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionBGUpperView() );

  m_pJoinFestaGlobalAttractionViewList->AddLowerView( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionBGLowerView() );
  m_pJoinFestaGlobalAttractionViewList->AddLowerViewInputEnable( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionMessageMenuLowerView() );

  m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionMessageMenuLowerView()->SetStreamMessage( jf_attraction_win_05 );
}


void JoinFestaGlobalAttractionMainFrame::notGetRewardMessage()
{
  GFL_PRINT( "JoinFestaGlobalAttractionMainFrame::notGetRewardMessage \n" );

  m_pJoinFestaGlobalAttractionViewList->RemoveAllView();

  m_pJoinFestaGlobalAttractionViewList->AddUpperView( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionBGUpperView() );

  m_pJoinFestaGlobalAttractionViewList->AddLowerView( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionBGLowerView() );
  m_pJoinFestaGlobalAttractionViewList->AddLowerViewInputEnable( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionMessageMenuLowerView() );

  m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionMessageMenuLowerView()->SetStreamMessage( jf_attraction_win_07 );
}


void JoinFestaGlobalAttractionMainFrame::setupMainScreen()
{
  GFL_PRINT( "JoinFestaGlobalAttractionMainFrame::setupMainScreen \n" );

  m_pJoinFestaGlobalAttractionViewList->RemoveAllView();

  m_pJoinFestaGlobalAttractionViewList->AddUpperView( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionBGUpperView() );
  m_pJoinFestaGlobalAttractionViewList->AddUpperView( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionInfoUpperView() );

  m_pJoinFestaGlobalAttractionViewList->AddLowerView( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionBGLowerView() );
  m_pJoinFestaGlobalAttractionViewList->AddLowerViewInputEnable( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionSelectListLowerView() );

  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pDownloadGlobalAttractionInfoList = m_pJoinFestaGlobalAttractionWork->GetDownloadGlobalAttractionInfoList();
  u32 downloadGlobalAttractionInfoListSize = m_pJoinFestaGlobalAttractionWork->GetDownloadGlobalAttractionInfoListSize();

  m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionSelectListLowerView()->SetList( pDownloadGlobalAttractionInfoList, downloadGlobalAttractionInfoListSize );

  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pActiveGlobalAttractionInfo = m_JoinFestaGlobalAttractionSelectListLowerViewListener.GetActiveGlobalAttractionInfo();

  if( pActiveGlobalAttractionInfo )
  {
    m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionInfoUpperView()->SetGlobalAttractionInfo( pActiveGlobalAttractionInfo );
  }
  else
  {
    m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionInfoUpperView()->SetGlobalAttractionInfo( &pDownloadGlobalAttractionInfoList[0] );
  }

}


void JoinFestaGlobalAttractionMainFrame::setupDetailScreen( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pGlobalAttractionInfo )
{
  GFL_PRINT( "JoinFestaGlobalAttractionMainFrame::setupDetailScreen \n" );

  m_pJoinFestaGlobalAttractionViewList->RemoveAllView();

  m_pJoinFestaGlobalAttractionViewList->AddUpperView( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionBGUpperView() );
  m_pJoinFestaGlobalAttractionViewList->AddUpperView( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionInfoUpperView() );

  m_pJoinFestaGlobalAttractionViewList->AddLowerView( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionBGLowerView() );
  m_pJoinFestaGlobalAttractionViewList->AddLowerViewInputEnable( m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionDetailLowerView() );

  m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionInfoUpperView()->SetGlobalAttractionInfo( pGlobalAttractionInfo );
  m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionDetailLowerView()->SetGlobalAttractionInfo( pGlobalAttractionInfo );
}


void JoinFestaGlobalAttractionMainFrame::OnDownloadSuccess()
{
  GFL_PRINT( "JoinFestaGlobalAttractionMainFrame::OnDownloadSuccess \n" );

  bool bIsOpenGlobalAttraction = m_pJoinFestaGlobalAttractionWork->IsOpenGlobalAttraction();

  if( bIsOpenGlobalAttraction )
  {
    NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pOpenGlobalAttractionInfo = m_pJoinFestaGlobalAttractionWork->GetOpenGlobalAttractionInfo();

    if( isRegistEntry( pOpenGlobalAttractionInfo ) )
    {
      bool bPGLAccessLockFlag = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync()->CheckLockGameSyncSaveFor5xxError();

      if( bPGLAccessLockFlag )
      {
        m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionMessageMenuLowerView()->SetStreamMessage( jf_attraction_win_08 );
      }
      else
      {
        gameSyncRequest();
      }
    }
    else
    {
      entryComfirmGlobalAttraction( pOpenGlobalAttractionInfo );
    }
  }
  else
  {
    m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionMessageMenuLowerView()->SetStreamMessage( jf_attraction_win_06 );
  }
}


void JoinFestaGlobalAttractionMainFrame::OnDownloadError()
{
  GFL_PRINT( "JoinFestaGlobalAttractionMainFrame::OnDownloadError \n" );

  m_bDownloadError = true;

  m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionMessageMenuLowerView()->SetStreamMessage( jf_attraction_win_06 );
}


void JoinFestaGlobalAttractionMainFrame::OnDownloadCancel()
{
  GFL_PRINT( "JoinFestaGlobalAttractionMainFrame::OnDownloadCancel \n" );

  exitFrame( NetApp::JoinFestaGlobalAttraction::MAIN_FRAME_RESULT_BACK );
}


void JoinFestaGlobalAttractionMainFrame::OnSaveDataSyncSuccess()
{
  GFL_PRINT( "JoinFestaGlobalAttractionMainFrame::OnSaveDataSyncSuccess \n" );

  Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pDownloadGlobalAttractionInfoList = m_pJoinFestaGlobalAttractionWork->GetDownloadGlobalAttractionInfoList();

  for( int i = 0; i < NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::RECORD_MAX; ++i )
  {
    NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData* pGlobalAttractionSaveData = pJoinFestaDataSave->GetGlobalAttractionData( static_cast<u8>( i ) );

    if( pGlobalAttractionSaveData )
    {
      if( pGlobalAttractionSaveData->openState == NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::OPEN_STATE_NOW )
      {
        if( !m_pJoinFestaGlobalAttractionWork->IsOpenGlobalAttraction( &pDownloadGlobalAttractionInfoList[i] ) )
        {
          NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData copyData;
        
          gfl2::std::MemCopy( pGlobalAttractionSaveData, &copyData, sizeof( NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData ) );

          copyData.openState = NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::OPEN_STATE_TOTAL_NOW;

          pJoinFestaDataSave->SetGlobalAttractionData( static_cast<u8>( i ), &copyData );
        }
      }
    }
  }

  m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionMessageMenuLowerView()->SetStreamMessage( jf_attraction_win_04 );

  // @fix GFNMCat[5596] ゲームシンクを行って集計日時が更新されたけど入りなおさないと表示が更新されないので、入りなおさなくても表示を更新するようにしました
  m_pJoinFestaGlobalAttractionViewObjects->GetJoinFestaGlobalAttractionInfoUpperView()->UpdateGlobalAttractionInfo();
}


void JoinFestaGlobalAttractionMainFrame::OnSaveDataSyncError( u32 errorCode )
{
  GFL_PRINT( "JoinFestaGlobalAttractionMainFrame::OnSaveDataSyncError \n" );

  exitFrame( NetApp::JoinFestaGlobalAttraction::MAIN_FRAME_RESULT_BACK );
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)
