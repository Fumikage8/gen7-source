// ============================================================================
/*
 * @file JoinFestaAttractionListSelectOpenFrame.cpp
 * @brief アトラクション開催参加アプリの○○プロセスです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionListSelect/source/Frame/JoinFestaAttractionListSelectOpenFrame.h"
#include "NetApp/JoinFestaAttractionListSelect/source/JoinFestaAttractionListSelectFrameResult.h"

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionInfoUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionSelectLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAlertLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionUtil.h"

#include "NetStatic/NetLib/include/NijiNetworkSystem.h"

#if defined( GF_PLATFORM_CTR )
#include "NetStatic/NetLib/include/Wifi/SubscriptionManager.h"
#endif // defined( GF_PLATFORM_CTR )

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Savedata/include/MyItemSave.h"
#include "Sound/include/Sound.h"

#include <niji_conv_header/message/msg_jf_playerlist.h>
#include <niji_conv_header/poke_lib/item/itemsym.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)
GFL_NAMESPACE_BEGIN(Frame)


static const int SEQUENCE_DEFAULT              = 0;
static const int SEQUENCE_FIRST_SAVE           = 1;
static const int SEQUENCE_REQUEST_START        = 2;
static const int SEQUENCE_REQUEST_WAIT         = 3;
static const int SEQUENCE_SECOND_SAVE          = 4;

static bool g_bIsEndRequest = false;
static bool g_bIsTimeOut = false;

JoinFestaAttractionListSelectOpenFrame::JoinFestaAttractionListSelectOpenFrame(
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectWork* pJoinFestaAttractionListSelectWork,
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewSystemAccessor* pJoinFestaAttractionListSelectViewSystemAccessor ) :
  NetApp::JoinFestaAttractionListSelect::Frame::JoinFestaAttractionListSelectFrameBase( pJoinFestaAttractionListSelectWork, pJoinFestaAttractionListSelectViewSystemAccessor ),
  m_JoinFestaAttractionSelectLowerViewListener(),
  m_JoinFestaPlayerListMessageMenuLowerViewListener(),
  m_JoinFestaAlertLowerViewListener(),
  m_NetAppCommonSaveUtility(),
  m_Timer()
{
}


JoinFestaAttractionListSelectOpenFrame::~JoinFestaAttractionListSelectOpenFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool JoinFestaAttractionListSelectOpenFrame::startup()
{
  NetAppLib::JoinFestaUI::JoinFestaAttractionListManager& joinFestaAttractionListManager = m_pJoinFestaAttractionListSelectWork->GetJoinFestaAttractionListManager();

  joinFestaAttractionListManager.UpdateList( true );

  m_pJoinFestaAttractionListSelectViewObjects->InputDisableAll();
  m_pJoinFestaAttractionListSelectViewList->RemoveAllView();

  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleUpdateButton( false );
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleDecideButton( false );
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleMenuCursor( true );
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleBackButton( true );

  m_pJoinFestaAttractionListSelectViewList->AddUpperView( m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListBGUpperView() );
  m_pJoinFestaAttractionListSelectViewList->AddUpperView( m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionInfoUpperView() );

  m_pJoinFestaAttractionListSelectViewList->AddLowerView( m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListBGLowerView() );
  m_pJoinFestaAttractionListSelectViewList->AddLowerView( m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView() );

  
  NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& openList = joinFestaAttractionListManager.GetOpenList();
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetOpenList( &openList );

  
  NetAppLib::JoinFestaUI::JoinFestaAttractionData* pOpenActiveAttractionData = m_pJoinFestaAttractionListSelectWork->GetOpenActiveAttractionData();
  if( pOpenActiveAttractionData == NULL )
  {
    NetAppLib::JoinFestaUI::ItJoinFestaAttractionDataList it = openList.Begin();
    pOpenActiveAttractionData = &(*it);
  }
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionInfoUpperView()->SetJoinFestaAttractionData( pOpenActiveAttractionData );

  bool bIsOpenAttraction = false;

#if defined( GF_PLATFORM_CTR )

  NetLib::Wifi::SubscriptionManager* pSubscriptionManager = GFL_SINGLETON_INSTANCE( NetLib::Wifi::SubscriptionManager );

  bIsOpenAttraction = pSubscriptionManager->CanOpenAttraction();

#endif // defined( GF_PLATFORM_CTR )

  if( bIsOpenAttraction )
  {
    Savedata::MyItem* pMyItem = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMyItem();

    bool bTicketCheck = pMyItem->Check( ITEM_FESUTIKETTO, 1 );

    if( bTicketCheck )
    {
      u32 waitTime = 0;

#if defined( GF_PLATFORM_CTR )

      waitTime = pSubscriptionManager->GetOpenAttractionPreviousWaitTime();

#endif // defined( GF_PLATFORM_CTR )

      bool bTimeCheck = ( waitTime == 0 ) ? true : false;

      if( !bTimeCheck )
      {
        m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleBackButton( false );
        m_pJoinFestaAttractionListSelectViewList->AddLowerViewInputEnable( m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAlertLowerView() );
        m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAlertLowerView()->SetMessage( jf_playerlist_exp_07, waitTime );
      }
    }
    else
    {
      m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleBackButton( false );
      m_pJoinFestaAttractionListSelectViewList->AddLowerViewInputEnable( m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAlertLowerView() );
      m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAlertLowerView()->SetMessage( jf_playerlist_exp_06 );
    }
  }
  else
  {
    m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleBackButton( false );
    m_pJoinFestaAttractionListSelectViewList->AddLowerView( m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAlertLowerView() );
    m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAlertLowerView()->SetMessage( jf_playerlist_exp_05 );
  }

  return true;
}


void JoinFestaAttractionListSelectOpenFrame::setListener()
{
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetListener( &m_JoinFestaAttractionSelectLowerViewListener );
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetListener( &m_JoinFestaPlayerListMessageMenuLowerViewListener );
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAlertLowerView()->SetListener( &m_JoinFestaAlertLowerViewListener );

  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetInputEnabled( true );
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAlertLowerView()->SetInputEnabled( true );
}


void JoinFestaAttractionListSelectOpenFrame::removeListener()
{
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->RemoveListener();
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->RemoveListener();
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAlertLowerView()->RemoveListener();
}


void JoinFestaAttractionListSelectOpenFrame::updateSequence()
{
  switch( GetSubSeq() )
  {
    // ---------------------------------------------------------------------------------------
    case SEQUENCE_DEFAULT:
    {
      defaultProcedure();
    }
    break;

    // ---------------------------------------------------------------------------------------
    case SEQUENCE_FIRST_SAVE:
    {
      if( m_NetAppCommonSaveUtility.SaveWait() )
      {
        Sound::PlaySE( SEQ_SE_SAVE );

        m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetMessageBlackFilter( jf_playerlist_win_26 );

        m_Timer.TimeOutStart();

        GFL_RELEASE_PRINT( "SetSubSeq( SEQUENCE_REQUEST_START );\n" );
        SetSubSeq( SEQUENCE_REQUEST_START );
      }
    }
    break;

    // ---------------------------------------------------------------------------------------
    case SEQUENCE_REQUEST_START:
    {
      NetAppLib::JoinFesta::JoinFestaAttractionManager* pJoinFestaAttractionManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaAttractionManager );

      NetAppLib::JoinFestaUI::JoinFestaAttractionData* pSelectAttraction = m_JoinFestaAttractionSelectLowerViewListener.GetSelectAttraction();

      pJoinFestaAttractionManager->OpenRequest( pSelectAttraction->GetAttractionID() );

      GFL_RELEASE_PRINT( "SetSubSeq( SEQUENCE_REQUEST_WAIT );\n" );
      SetSubSeq( SEQUENCE_REQUEST_WAIT );

      g_bIsEndRequest = pJoinFestaAttractionManager->IsEndRequest();
      g_bIsTimeOut = m_Timer.IsTimeOut();

      GFL_RELEASE_PRINT( "g_bIsEndRequest = %d\n", g_bIsEndRequest );
      GFL_RELEASE_PRINT( "g_bIsTimeOut = %d\n", g_bIsTimeOut );
    }
    break;

    // ---------------------------------------------------------------------------------------
    case SEQUENCE_REQUEST_WAIT:
    {
      NetAppLib::JoinFesta::JoinFestaAttractionManager* pJoinFestaAttractionManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaAttractionManager );

      if( g_bIsEndRequest != pJoinFestaAttractionManager->IsEndRequest() )
      {
        g_bIsEndRequest = pJoinFestaAttractionManager->IsEndRequest();
        GFL_RELEASE_PRINT( "g_bIsEndRequest = %d\n", g_bIsEndRequest );
      }

      if( g_bIsTimeOut != m_Timer.IsTimeOut() )
      {
        g_bIsTimeOut = m_Timer.IsTimeOut();
        GFL_RELEASE_PRINT( "g_bIsTimeOut = %d\n", g_bIsTimeOut );
      }

      if( pJoinFestaAttractionManager->IsEndRequest() && m_Timer.IsTimeOut() )
      {
        NetAppLib::JoinFesta::JoinFestaAttractionManager::RequestResult requestResult = pJoinFestaAttractionManager->GetRequestResult();
        
        if( requestResult == NetAppLib::JoinFesta::JoinFestaAttractionManager::REQUEST_SUCCESS )
        {
          NetAppLib::JoinFestaUI::JoinFestaAttractionData* pSelectAttraction = m_JoinFestaAttractionSelectLowerViewListener.GetSelectAttraction();

          m_pJoinFestaAttractionListSelectWork->SetSelectAttraction( pSelectAttraction );

          exitFrame( NetApp::JoinFestaAttractionListSelect::OPEN_FRAME_RESULT_BACK );
        }
        else
        {
          m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_33 );

          GFL_RELEASE_PRINT( "SetSubSeq( SEQUENCE_DEFAULT );\n" );
          SetSubSeq( SEQUENCE_DEFAULT );
        }
      }
    }
    break;

    // ---------------------------------------------------------------------------------------
    case SEQUENCE_SECOND_SAVE:
    {
      if( m_NetAppCommonSaveUtility.SaveWait() )
      {
        m_pJoinFestaAttractionListSelectViewList->RemoveLowerView();

        Sound::PlaySE( SEQ_SE_SAVE );

        GFL_RELEASE_PRINT( "SetSubSeq( SEQUENCE_DEFAULT );\n" );
        SetSubSeq( SEQUENCE_DEFAULT );
      }
    }
    break;
  }

  m_JoinFestaAttractionSelectLowerViewListener.ResetInputEvent();
  m_JoinFestaPlayerListMessageMenuLowerViewListener.ResetInputEvent();
  m_JoinFestaAlertLowerViewListener.ResetInputEvent();

}


void JoinFestaAttractionListSelectOpenFrame::defaultProcedure()
{
  // --------------------------------------------------------------------------------------------------
  {
    u32 inputEventID         = m_JoinFestaAttractionSelectLowerViewListener.GetInputEventID();
    bool bTouchBackButton    = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
    bool bPushLButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_L_BUTTON ) ? true : false;
    bool bPushRButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_R_BUTTON ) ? true : false;
    bool bSelectAttraction   = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaAttractionSelectLowerViewListener::INPUT_EVENT_ID_SELECT_ATTRACTION ) ? true : false;
    bool bIsUpdateUpperView  = m_JoinFestaAttractionSelectLowerViewListener.IsUpdateUpperView();

    if( bIsUpdateUpperView )
    {
      m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionInfoUpperView()->SetJoinFestaAttractionData( m_JoinFestaAttractionSelectLowerViewListener.GetActiveAttraction() );
      m_pJoinFestaAttractionListSelectWork->SetOpenActiveAttractionData( m_JoinFestaAttractionSelectLowerViewListener.GetActiveAttraction() );
    }

    if( bTouchBackButton )
    {
      exitFrame( NetApp::JoinFestaAttractionListSelect::OPEN_FRAME_RESULT_BACK );
    }
    else
    if( bPushLButton || bPushRButton )
    {
      exitFrame( NetApp::JoinFestaAttractionListSelect::OPEN_FRAME_RESULT_TO_ENTRY );
    }
    else
    if( bSelectAttraction )
    {
      if( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
      {
        m_pJoinFestaAttractionListSelectViewList->AddLowerViewInputEnable( m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
        m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetMessageMenuBlackFilter( jf_playerlist_win_22, jf_playerlist_sel_12, jf_playerlist_sel_10, jf_playerlist_sel_11 );
      }
      else
      {
        m_pJoinFestaAttractionListSelectViewList->AddLowerViewInputEnable( m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
        m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_31 );
      }
    }

  }

  // --------------------------------------------------------------------------------------------------
  {
    u32 inputEventID       = m_JoinFestaPlayerListMessageMenuLowerViewListener.GetInputEventID();
    u32 messageID          = m_JoinFestaPlayerListMessageMenuLowerViewListener.GetMessageID();
    u32 selectMenuID       = m_JoinFestaPlayerListMessageMenuLowerViewListener.GetSelectMenuID();
    bool bTouchScreen      = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN ) ? true : false;
    bool bSelectedMenu     = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener::INPUT_EVENT_ID_SELECT_MENU ) ? true : false;
    bool bTouchYesButton   = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_YES_BUTTON ) ? true : false;
    bool bTouchNoButton    = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_NO_BUTTON ) ? true : false;

    switch( messageID )
    {
      // ------------------------------------------------------------
      case jf_playerlist_win_22:
      {
        if( bSelectedMenu )
        {
          switch( selectMenuID )
          {
            case jf_playerlist_sel_12:
            {
              if( NetAppLib::JoinFesta::JoinFestaAttractionUtil::CheckAttractionPlayEnable( m_JoinFestaAttractionSelectLowerViewListener.GetSelectAttraction()->GetAttractionID() ) )
              {
                m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageYesNoBlackFilter( jf_playerlist_win_23 );
              }
              else
              {
                m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_32 );
              }
            }
            break;

            case jf_playerlist_sel_10:
            {
              m_pJoinFestaAttractionListSelectWork->SetDescriptionAttraction( m_JoinFestaAttractionSelectLowerViewListener.GetSelectAttraction() );
              m_pJoinFestaAttractionListSelectWork->SetDescriptionBootFrameID( NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_FRAME_ID_OPEN );
              exitFrame( NetApp::JoinFestaAttractionListSelect::OPEN_FRAME_RESULT_TO_DESCRIPTION );
            }
            break;
            
            case jf_playerlist_sel_11:
            {
              m_pJoinFestaAttractionListSelectViewList->RemoveLowerView();
            }
            break;
          }
        }
      }
      break;
      
      // ------------------------------------------------------------
      case jf_playerlist_win_23:
      {
        if( bTouchYesButton )
        {
          Savedata::MyItem* pMyItem = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMyItem();

          pMyItem->Sub( ITEM_FESUTIKETTO, 1 );

          m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetMessageBlackFilter( jf_playerlist_win_24, true );

          GFL_RELEASE_PRINT( "SetSubSeq( SEQUENCE_FIRST_SAVE );\n" );
          SetSubSeq( SEQUENCE_FIRST_SAVE );

        }
        else
        if( bTouchNoButton )
        {
          m_pJoinFestaAttractionListSelectViewList->RemoveLowerView();
        }
      }
      break;
      
      // ------------------------------------------------------------
      case jf_playerlist_win_33:
      {
        if( bTouchScreen )
        {
          Savedata::MyItem* pMyItem = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMyItem();

          pMyItem->Add( ITEM_FESUTIKETTO, 1, false );

          m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetMessageBlackFilter( jf_playerlist_win_24, true );

          GFL_RELEASE_PRINT( "SetSubSeq( SEQUENCE_SECOND_SAVE );\n" );
          SetSubSeq( SEQUENCE_SECOND_SAVE );
        }
      }
      break;

      // ------------------------------------------------------------
      case jf_playerlist_win_31:
      {
        if( bTouchScreen )
        {
          m_pJoinFestaAttractionListSelectViewList->RemoveLowerView();
        }
      }
      break;

      // ------------------------------------------------------------
      case jf_playerlist_win_32:
      {
        if( bTouchScreen )
        {
          m_pJoinFestaAttractionListSelectViewList->RemoveLowerView();
        }
      }
      break;

    }

  }

  // --------------------------------------------------------------------------------------------------
  {
    u32 inputEventID          = m_JoinFestaAlertLowerViewListener.GetInputEventID();
    bool bTouchBackButton     = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaAlertLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
    bool bPushLButton         = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaAlertLowerViewListener::INPUT_EVENT_ID_PUSH_L_BUTTON ) ? true : false;
    bool bPushRButton         = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaAlertLowerViewListener::INPUT_EVENT_ID_PUSH_R_BUTTON ) ? true : false;

    if( bTouchBackButton )
    {
      exitFrame( NetApp::JoinFestaAttractionListSelect::OPEN_FRAME_RESULT_BACK );
    }
    else
    if( bPushLButton || bPushRButton )
    {
      exitFrame( NetApp::JoinFestaAttractionListSelect::OPEN_FRAME_RESULT_TO_ENTRY );
    }
  }
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)
