// ============================================================================
/*
 * @file JoinFestaOpenListSelectFrame.cpp
 * @brief 開催選択アプリの○○プロセスです。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaOpenList/source/Frame/JoinFestaOpenListSelectFrame.h"
#include "NetApp/JoinFestaOpenList/source/JoinFestaOpenListFrameResult.h"

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListPlayerInfoUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionInfoUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionSelectLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAlertLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionUtil.h"
#include "NetStatic/NetAppLib/include/Util/NetAppEntryChecker.h"
#include "NetStatic/NetAppLib/include/Util/NetAppCommonSaveUtility.h"

#include "NetStatic/NetLib/include/NijiNetworkSystem.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include <niji_conv_header/message/msg_jf_playerlist.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)
GFL_NAMESPACE_BEGIN(Frame)

static const int SEQUENCE_DEFAULT              = 0;
static const int SEQUENCE_REQUEST_START        = 1;
static const int SEQUENCE_REQUEST_WAIT         = 2;

static bool g_bIsEndRequest = false;
static bool g_bIsTimeOut = false;

JoinFestaOpenListSelectFrame::JoinFestaOpenListSelectFrame(
  NetApp::JoinFestaOpenList::System::JoinFestaOpenListWork* pJoinFestaOpenListWork,
  NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewSystemAccessor* pJoinFestaOpenListViewSystemAccessor ) :
  NetApp::JoinFestaOpenList::Frame::JoinFestaOpenListFrameBase( pJoinFestaOpenListWork, pJoinFestaOpenListViewSystemAccessor ),
  m_JoinFestaAttractionSelectLowerViewListener(),
  m_JoinFestaPlayerListMessageMenuLowerViewListener(),
  m_JoinFestaAlertLowerViewListener(),
  m_Timer()
{
}


JoinFestaOpenListSelectFrame::~JoinFestaOpenListSelectFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool JoinFestaOpenListSelectFrame::startup()
{
  NetAppLib::JoinFestaUI::JoinFestaAttractionListManager& joinFestaAttractionListManager = m_pJoinFestaOpenListWork->GetJoinFestaAttractionListManager();

  joinFestaAttractionListManager.UpdateList( true );
  joinFestaAttractionListManager.SortList();

  m_pJoinFestaOpenListViewObjects->InputDisableAll();
  m_pJoinFestaOpenListViewList->RemoveAllView();

  m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleUpdateButton( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() );
  m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleDecideButton( false );
  m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleMenuCursor( true );
  m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleArrowIcon( false );

  m_pJoinFestaOpenListViewList->AddUpperView( m_pJoinFestaOpenListViewObjects->GetJoinFestaPlayerListBGUpperView() );
  m_pJoinFestaOpenListViewList->AddUpperView( m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionInfoUpperView() );

  m_pJoinFestaOpenListViewList->AddLowerView( m_pJoinFestaOpenListViewObjects->GetJoinFestaPlayerListBGLowerView() );
  m_pJoinFestaOpenListViewList->AddLowerView( m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionSelectLowerView() );

  setupList();

  return true;
}


void JoinFestaOpenListSelectFrame::setupList()
{
  NetAppLib::JoinFestaUI::JoinFestaAttractionListManager& joinFestaAttractionListManager = m_pJoinFestaOpenListWork->GetJoinFestaAttractionListManager();

  NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& entryList = joinFestaAttractionListManager.GetEntryList();
  NetAppLib::JoinFestaUI::ItJoinFestaAttractionDataList it = entryList.Begin();

  m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionSelectLowerView()->SetEntryList( &entryList );

  NetAppLib::JoinFestaUI::JoinFestaAttractionData* pJoinFestaAttractionData = ( entryList.Size() > 0 ) ? &(*it) : NULL;

  NetAppLib::JoinFestaUI::JoinFestaAttractionData* pDescriptionAttraction = m_pJoinFestaOpenListWork->GetDescriptionAttraction();

  if( pDescriptionAttraction )
  {
    pJoinFestaAttractionData = pDescriptionAttraction;
    m_pJoinFestaOpenListWork->SetDescriptionAttraction( NULL );
  }

  m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionInfoUpperView()->SetJoinFestaAttractionData( pJoinFestaAttractionData );
}


void JoinFestaOpenListSelectFrame::setListener()
{
  m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionSelectLowerView()->SetListener( &m_JoinFestaAttractionSelectLowerViewListener );
  m_pJoinFestaOpenListViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetListener( &m_JoinFestaPlayerListMessageMenuLowerViewListener );

  m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionSelectLowerView()->SetInputEnabled( true );

#if defined( GF_PLATFORM_CTR )
  GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPacketManager )->RegistPersonalListener( this );
#endif // defined( GF_PLATFORM_CTR )
}


void JoinFestaOpenListSelectFrame::removeListener()
{
  m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionSelectLowerView()->RemoveListener();
  m_pJoinFestaOpenListViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->RemoveListener();

#if defined( GF_PLATFORM_CTR )
  GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPacketManager )->RemovePersonalListener( this );
#endif // defined( GF_PLATFORM_CTR )
}


void JoinFestaOpenListSelectFrame::OnWLANSwitchDisable()
{
  m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleUpdateButton( false );
}


void JoinFestaOpenListSelectFrame::updateSequence()
{
  switch( GetSubSeq() )
  {
    case SEQUENCE_DEFAULT:
    {
      attractionSelectEventProcedure();
      messageEventProcedure();
      alertEventProcedure();
    }
    break;

    case SEQUENCE_REQUEST_START:
    {
      NetAppLib::JoinFesta::JoinFestaAttractionManager* pJoinFestaAttractionManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaAttractionManager );

      NetAppLib::JoinFestaUI::JoinFestaAttractionData* pSelectAttraction = m_JoinFestaAttractionSelectLowerViewListener.GetSelectAttraction();

      pJoinFestaAttractionManager->JoinRequest( *pSelectAttraction );

      GFL_RELEASE_PRINT( "SetSubSeq( SEQUENCE_REQUEST_WAIT );\n" );
      SetSubSeq( SEQUENCE_REQUEST_WAIT );

      g_bIsEndRequest = pJoinFestaAttractionManager->IsEndRequest();
      g_bIsTimeOut = m_Timer.IsTimeOut();

      GFL_RELEASE_PRINT( "g_bIsEndRequest = %d\n", g_bIsEndRequest );
      GFL_RELEASE_PRINT( "g_bIsTimeOut = %d\n", g_bIsTimeOut );
    }
    break;

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

          m_pJoinFestaOpenListWork->SetSelectAttraction( pSelectAttraction );

          exitFrame( NetApp::JoinFestaOpenList::SELECT_FRAME_RESULT_BACK );
        }
        else
        {
          m_pJoinFestaOpenListViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_29 );

          GFL_RELEASE_PRINT( "SetSubSeq( SEQUENCE_DEFAULT );\n" );
          SetSubSeq( SEQUENCE_DEFAULT );
        }
      }
    }
    break;
  }

  m_JoinFestaAttractionSelectLowerViewListener.ResetInputEvent();
  m_JoinFestaPlayerListMessageMenuLowerViewListener.ResetInputEvent();
  m_JoinFestaAlertLowerViewListener.ResetInputEvent();

}


void JoinFestaOpenListSelectFrame::attractionSelectEventProcedure()
{
  u32 inputEventID         = m_JoinFestaAttractionSelectLowerViewListener.GetInputEventID();
  bool bTouchBackButton    = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaAttractionSelectLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
  bool bTouchUpdateButton  = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaAttractionSelectLowerViewListener::INPUT_EVENT_ID_TOUCH_UPDATE_BUTTON ) ? true : false;
  bool bSelectAttraction   = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaAttractionSelectLowerViewListener::INPUT_EVENT_ID_SELECT_ATTRACTION ) ? true : false;
  bool bIsUpdateUpperView  = m_JoinFestaAttractionSelectLowerViewListener.IsUpdateUpperView();

  if( bIsUpdateUpperView )
  {
    m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionInfoUpperView()->SetJoinFestaAttractionData( m_JoinFestaAttractionSelectLowerViewListener.GetActiveAttraction() );
  }

  if( bTouchBackButton )
  {
    exitFrame( NetApp::JoinFestaOpenList::SELECT_FRAME_RESULT_BACK );
  }
  else
  if( bTouchUpdateButton )
  {
    if( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
    {
      m_pJoinFestaOpenListViewList->AddLowerViewInputEnable( m_pJoinFestaOpenListViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
      m_pJoinFestaOpenListViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_02 );
    }
    else
    {
      m_pJoinFestaOpenListViewList->AddLowerViewInputEnable( m_pJoinFestaOpenListViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
      m_pJoinFestaOpenListViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_31 );
    }
  }
  else
  if( bSelectAttraction )
  {
    if( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
    {
      NetAppLib::JoinFestaUI::JoinFestaAttractionData* pSelectAttraction = m_JoinFestaAttractionSelectLowerViewListener.GetSelectAttraction();
      if( pSelectAttraction )
      {
        m_pJoinFestaOpenListViewList->AddLowerViewInputEnable( m_pJoinFestaOpenListViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
        m_pJoinFestaOpenListViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetMessageMenuBlackFilter( jf_playerlist_win_19, jf_playerlist_sel_09, jf_playerlist_sel_10, jf_playerlist_sel_11 );
      }
    }
    else
    {
      m_pJoinFestaOpenListViewList->AddLowerViewInputEnable( m_pJoinFestaOpenListViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
      m_pJoinFestaOpenListViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_31 );
    }
  }

}


void JoinFestaOpenListSelectFrame::messageEventProcedure()
{
  u32 inputEventID       = m_JoinFestaPlayerListMessageMenuLowerViewListener.GetInputEventID();
  u32 messageID          = m_JoinFestaPlayerListMessageMenuLowerViewListener.GetMessageID();
  u32 selectMenuID       = m_JoinFestaPlayerListMessageMenuLowerViewListener.GetSelectMenuID();
  bool bTouchScreen      = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN ) ? true : false;
  bool bSelectedMenu     = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener::INPUT_EVENT_ID_SELECT_MENU ) ? true : false;
    
  switch( messageID )
  {
    // ----------------------------------------------------------------------------------------
    case jf_playerlist_win_02:
    {
      if( bTouchScreen )
      {
        m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleUpdateButton( false );

        NetAppLib::JoinFestaUI::JoinFestaAttractionListManager& joinFestaAttractionListManager = m_pJoinFestaOpenListWork->GetJoinFestaAttractionListManager();

        joinFestaAttractionListManager.UpdateList( false );
        joinFestaAttractionListManager.SortList();

        m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionSelectLowerView()->ResetPaneListCursorInfo();
        setupList();

        m_pJoinFestaOpenListViewList->RemoveLowerView();
      }
    }
    break;

    // ----------------------------------------------------------------------------------------
    case jf_playerlist_win_13:
    {
      if( bTouchScreen )
      {
        m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionSelectLowerView()->SetPassive();

        m_pJoinFestaOpenListViewList->RemoveLowerView();
      }
    }
    break;

    // ----------------------------------------------------------------------------------------
    case jf_playerlist_win_19:
    {
      if( bSelectedMenu )
      {
        switch( selectMenuID )
        {
          case jf_playerlist_sel_09:
          {
            NetAppLib::JoinFestaUI::JoinFestaAttractionData* pSelectAttraction = m_JoinFestaAttractionSelectLowerViewListener.GetSelectAttraction();

            bool bCheckEntryTime = NetAppLib::JoinFesta::JoinFestaPacketUtil::CheckAttractionJoinEnable( pSelectAttraction );

            if( bCheckEntryTime )
            {
              if( NetAppLib::JoinFesta::JoinFestaAttractionUtil::CheckAttractionPlayEnable( m_JoinFestaAttractionSelectLowerViewListener.GetSelectAttraction()->GetAttractionID() ) )
              {
                m_pJoinFestaOpenListViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetMessageBlackFilter( jf_playerlist_win_26 );

                m_Timer.TimeOutStart();

                GFL_RELEASE_PRINT( "SetSubSeq( SEQUENCE_REQUEST_START );\n" );
                SetSubSeq( SEQUENCE_REQUEST_START );
              }
              else
              {
                m_pJoinFestaOpenListViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_32 );
              }
            }
            else
            {
              m_pJoinFestaOpenListViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_30 );
            }
          }
          break;

          case jf_playerlist_sel_10:
          {
            m_pJoinFestaOpenListWork->SetDescriptionAttraction( m_JoinFestaAttractionSelectLowerViewListener.GetSelectAttraction() );
            exitFrame( NetApp::JoinFestaOpenList::SELECT_FRAME_RESULT_TO_DESCRIPTION );
          }
          break;

          case jf_playerlist_sel_11:
          {
            m_pJoinFestaOpenListViewList->RemoveLowerView();
          }
          break;
        }
      }
    }
    break;

    // ----------------------------------------------------------------------------------------
    case jf_playerlist_win_29:
    {
      if( bTouchScreen )
      {
        m_pJoinFestaOpenListViewList->RemoveLowerView();
      }
    }
    break;

    // ----------------------------------------------------------------------------------------
    case jf_playerlist_win_30:
    {
      if( bTouchScreen )
      {
        m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleUpdateButton( false );

        NetAppLib::JoinFestaUI::JoinFestaAttractionListManager& joinFestaAttractionListManager = m_pJoinFestaOpenListWork->GetJoinFestaAttractionListManager();

        joinFestaAttractionListManager.UpdateList( false );
        joinFestaAttractionListManager.SortList();

        m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionSelectLowerView()->ResetPaneListCursorInfo();
        setupList();

        m_pJoinFestaOpenListViewList->RemoveLowerView();
      }
    }
    break;

    // ----------------------------------------------------------------------------------------
    case jf_playerlist_win_31:
    {
      if( bTouchScreen )
      {
        m_pJoinFestaOpenListViewList->RemoveLowerView();
      }
    }
    break;

    // ----------------------------------------------------------------------------------------
    case jf_playerlist_win_32:
    {
      if( bTouchScreen )
      {
        m_pJoinFestaOpenListViewList->RemoveLowerView();
      }
    }
    break;
  }

}


void JoinFestaOpenListSelectFrame::alertEventProcedure()
{
  u32 inputEventID          = m_JoinFestaAlertLowerViewListener.GetInputEventID();
  bool bTouchBackButton     = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaAlertLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;

  if( bTouchBackButton )
  {
    m_pJoinFestaOpenListViewList->RemoveLowerView();
  }

}


void JoinFestaOpenListSelectFrame::OnAddJoinFestaPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleUpdateButton( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() );
}


void JoinFestaOpenListSelectFrame::OnUpdateJoinFesatPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person, bool isUpdate )
{
  m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleUpdateButton( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() );
}


void JoinFestaOpenListSelectFrame::OnDeleteJoinFestaPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  m_pJoinFestaOpenListViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleUpdateButton( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() );
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)
