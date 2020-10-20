// ============================================================================
/*
 * @file JoinFestaAttractionListSelectEntryFrame.cpp
 * @brief アトラクション開催参加アプリの○○プロセスです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionListSelect/source/Frame/JoinFestaAttractionListSelectEntryFrame.h"
#include "NetApp/JoinFestaAttractionListSelect/source/JoinFestaAttractionListSelectFrameResult.h"

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionInfoUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionSelectLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionUtil.h"
#include "NetStatic/NetAppLib/include/Util/NetAppCommonSaveUtility.h"

#include "NetStatic/NetLib/include/NijiNetworkSystem.h"

#include <niji_conv_header/message/msg_jf_playerlist.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)
GFL_NAMESPACE_BEGIN(Frame)


static const int SEQUENCE_DEFAULT              = 0;
static const int SEQUENCE_REQUEST_START        = 1;
static const int SEQUENCE_REQUEST_WAIT         = 2;

static bool g_bIsEndRequest = false;
static bool g_bIsTimeOut = false;

JoinFestaAttractionListSelectEntryFrame::JoinFestaAttractionListSelectEntryFrame(
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectWork* pJoinFestaAttractionListSelectWork,
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewSystemAccessor* pJoinFestaAttractionListSelectViewSystemAccessor ) :
  NetApp::JoinFestaAttractionListSelect::Frame::JoinFestaAttractionListSelectFrameBase( pJoinFestaAttractionListSelectWork, pJoinFestaAttractionListSelectViewSystemAccessor ),
  m_JoinFestaAttractionSelectLowerViewListener(),
  m_JoinFestaPlayerListMessageMenuLowerViewListener()
{
}


JoinFestaAttractionListSelectEntryFrame::~JoinFestaAttractionListSelectEntryFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool JoinFestaAttractionListSelectEntryFrame::startup()
{
  NetAppLib::JoinFestaUI::JoinFestaAttractionListManager& joinFestaAttractionListManager = m_pJoinFestaAttractionListSelectWork->GetJoinFestaAttractionListManager();

  joinFestaAttractionListManager.UpdateList( true );
  joinFestaAttractionListManager.SortList();

  m_pJoinFestaAttractionListSelectViewObjects->InputDisableAll();
  m_pJoinFestaAttractionListSelectViewList->RemoveAllView();

  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleUpdateButton( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() );
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleDecideButton( false );
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleMenuCursor( true );
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleBackButton( true );

  m_pJoinFestaAttractionListSelectViewList->AddUpperView( m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListBGUpperView() );
  m_pJoinFestaAttractionListSelectViewList->AddUpperView( m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionInfoUpperView() );

  m_pJoinFestaAttractionListSelectViewList->AddLowerView( m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListBGLowerView() );
  m_pJoinFestaAttractionListSelectViewList->AddLowerView( m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView() );


  NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& entryList = joinFestaAttractionListManager.GetEntryList();
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetEntryList( &entryList );


  NetAppLib::JoinFestaUI::JoinFestaAttractionData* pEntryActiveAttractionData = m_pJoinFestaAttractionListSelectWork->GetEntryActiveAttractionData();
  if( pEntryActiveAttractionData == NULL )
  {
    NetAppLib::JoinFestaUI::ItJoinFestaAttractionDataList it = entryList.Begin();
    pEntryActiveAttractionData = ( entryList.Size() > 0 ) ? &(*it) : NULL;
  }
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionInfoUpperView()->SetJoinFestaAttractionData( pEntryActiveAttractionData );


  return true;
}


void JoinFestaAttractionListSelectEntryFrame::setListener()
{
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetListener( &m_JoinFestaAttractionSelectLowerViewListener );
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetListener( &m_JoinFestaPlayerListMessageMenuLowerViewListener );

  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetInputEnabled( true );

#if defined( GF_PLATFORM_CTR )
  GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPacketManager )->RegistPersonalListener( this );
#endif // defined( GF_PLATFORM_CTR )
}


void JoinFestaAttractionListSelectEntryFrame::removeListener()
{
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->RemoveListener();
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->RemoveListener();

#if defined( GF_PLATFORM_CTR )
  GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPacketManager )->RemovePersonalListener( this );
#endif // defined( GF_PLATFORM_CTR )
}


void JoinFestaAttractionListSelectEntryFrame::OnWLANSwitchDisable()
{
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleUpdateButton( false );
}


void JoinFestaAttractionListSelectEntryFrame::updateSequence()
{
  switch( GetSubSeq() )
  {
    case SEQUENCE_DEFAULT:
    {
      defaultProcedure();
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

          m_pJoinFestaAttractionListSelectWork->SetSelectAttraction( pSelectAttraction );

          exitFrame( NetApp::JoinFestaAttractionListSelect::ENTRY_FRAME_RESULT_BACK );
        }
        else
        {
          m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_29 );

          GFL_RELEASE_PRINT( "SetSubSeq( SEQUENCE_DEFAULT );\n" );
          SetSubSeq( SEQUENCE_DEFAULT );
        }
      }
    }
    break;
  }

  m_JoinFestaAttractionSelectLowerViewListener.ResetInputEvent();
  m_JoinFestaPlayerListMessageMenuLowerViewListener.ResetInputEvent();

}


void JoinFestaAttractionListSelectEntryFrame::defaultProcedure()
{
  // --------------------------------------------------------------------------------------------------------------------------------
  {
    u32 inputEventID         = m_JoinFestaAttractionSelectLowerViewListener.GetInputEventID();
    bool bTouchBackButton    = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
    bool bTouchUpdateButton  = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_UPDATE_BUTTON ) ? true : false;
    bool bPushLButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_L_BUTTON ) ? true : false;
    bool bPushRButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_R_BUTTON ) ? true : false;
    bool bSelectAttraction   = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaAttractionSelectLowerViewListener::INPUT_EVENT_ID_SELECT_ATTRACTION ) ? true : false;
    bool bIsUpdateUpperView  = m_JoinFestaAttractionSelectLowerViewListener.IsUpdateUpperView();

    if( bIsUpdateUpperView )
    {
      m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionInfoUpperView()->SetJoinFestaAttractionData( m_JoinFestaAttractionSelectLowerViewListener.GetActiveAttraction() );
      m_pJoinFestaAttractionListSelectWork->SetEntryActiveAttractionData( m_JoinFestaAttractionSelectLowerViewListener.GetActiveAttraction() );
    }

    if( bTouchBackButton )
    {
      exitFrame( NetApp::JoinFestaAttractionListSelect::ENTRY_FRAME_RESULT_BACK );
    }
    else
    if( bTouchUpdateButton )
    {
      if( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
      {
        m_pJoinFestaAttractionListSelectViewList->AddLowerViewInputEnable( m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
        m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_02 );
      }
      else
      {
        m_pJoinFestaAttractionListSelectViewList->AddLowerViewInputEnable( m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
        m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_31 );
      }
    }
    else
    if( bPushLButton || bPushRButton )
    {
      exitFrame( NetApp::JoinFestaAttractionListSelect::ENTRY_FRAME_RESULT_TO_OPEN );
    }
    else
    if( bSelectAttraction )
    {
      if( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
      {
        NetAppLib::JoinFestaUI::JoinFestaAttractionData* pSelectAttraction = m_JoinFestaAttractionSelectLowerViewListener.GetSelectAttraction();
        if( pSelectAttraction )
        {
          m_pJoinFestaAttractionListSelectViewList->AddLowerViewInputEnable( m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
          m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetMessageMenuBlackFilter( jf_playerlist_win_21, jf_playerlist_sel_09, jf_playerlist_sel_10, jf_playerlist_sel_11 );
        }
      }
      else
      {
        m_pJoinFestaAttractionListSelectViewList->AddLowerViewInputEnable( m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
        m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_31 );
      }
    }
  }

  // --------------------------------------------------------------------------------------------------------------------------------
  {
    u32 inputEventID       = m_JoinFestaPlayerListMessageMenuLowerViewListener.GetInputEventID();
    u32 messageID          = m_JoinFestaPlayerListMessageMenuLowerViewListener.GetMessageID();
    u32 selectMenuID       = m_JoinFestaPlayerListMessageMenuLowerViewListener.GetSelectMenuID();
    bool bSelectedMenu     = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener::INPUT_EVENT_ID_SELECT_MENU ) ? true : false;
    bool bTouchScreen      = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN ) ? true : false;
    
    switch( messageID )
    {
      // ----------------------------------------------------------------------------------------
      case jf_playerlist_win_02:
      {
        if( bTouchScreen )
        {
          NetAppLib::JoinFestaUI::JoinFestaAttractionListManager& joinFestaAttractionListManager = m_pJoinFestaAttractionListSelectWork->GetJoinFestaAttractionListManager();

          joinFestaAttractionListManager.UpdateList( false );
          joinFestaAttractionListManager.SortList();

          NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& entryList = joinFestaAttractionListManager.GetEntryList();

          m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->ResetPaneListCursorInfo();
          m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetEntryList( &entryList );
          m_pJoinFestaAttractionListSelectWork->ResetActiveAttractionData();

          if( m_JoinFestaAttractionSelectLowerViewListener.IsUpdateUpperView() )
          {
            m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionInfoUpperView()->SetJoinFestaAttractionData( m_JoinFestaAttractionSelectLowerViewListener.GetActiveAttraction() );
            m_pJoinFestaAttractionListSelectWork->SetEntryActiveAttractionData( m_JoinFestaAttractionSelectLowerViewListener.GetActiveAttraction() );
          }

          m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleUpdateButton( false );

          m_pJoinFestaAttractionListSelectViewList->RemoveLowerView();
        }
      }
      break;

      // ----------------------------------------------------------------------------------------
      case jf_playerlist_win_13:
      {
        if( bTouchScreen )
        {
          m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetPassive();

          m_pJoinFestaAttractionListSelectViewList->RemoveLowerView();
        }
      }
      break;

      // ----------------------------------------------------------------------------------------
      case jf_playerlist_win_21:
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
                  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetMessageBlackFilter( jf_playerlist_win_26 );

                  m_Timer.TimeOutStart();

                  GFL_RELEASE_PRINT( "SetSubSeq( SEQUENCE_REQUEST_START );\n" );
                  SetSubSeq( SEQUENCE_REQUEST_START );
                }
                else
                {
                  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_32 );
                }
              }
              else
              {
                m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_30 );
              }
            }
            break;
            
            case jf_playerlist_sel_10:
            {
              m_pJoinFestaAttractionListSelectWork->SetDescriptionAttraction( m_JoinFestaAttractionSelectLowerViewListener.GetSelectAttraction() );
              m_pJoinFestaAttractionListSelectWork->SetDescriptionBootFrameID( NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_FRAME_ID_ENTRY );
              exitFrame( NetApp::JoinFestaAttractionListSelect::ENTRY_FRAME_RESULT_TO_DESCRIPTION );
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

      // ----------------------------------------------------------------------------------------
      case jf_playerlist_win_29:
      {
        if( bTouchScreen )
        {
          m_pJoinFestaAttractionListSelectViewList->RemoveLowerView();
        }
      }
      break;

      // ----------------------------------------------------------------------------------------
      case jf_playerlist_win_30:
      {
        if( bTouchScreen )
        {
          NetAppLib::JoinFestaUI::JoinFestaAttractionListManager& joinFestaAttractionListManager = m_pJoinFestaAttractionListSelectWork->GetJoinFestaAttractionListManager();

          joinFestaAttractionListManager.UpdateList( false );
          joinFestaAttractionListManager.SortList();

          NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& entryList = joinFestaAttractionListManager.GetEntryList();

          m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->ResetPaneListCursorInfo();
          m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetEntryList( &entryList );
          m_pJoinFestaAttractionListSelectWork->ResetActiveAttractionData();

          if( m_JoinFestaAttractionSelectLowerViewListener.IsUpdateUpperView() )
          {
            m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionInfoUpperView()->SetJoinFestaAttractionData( m_JoinFestaAttractionSelectLowerViewListener.GetActiveAttraction() );
            m_pJoinFestaAttractionListSelectWork->SetEntryActiveAttractionData( m_JoinFestaAttractionSelectLowerViewListener.GetActiveAttraction() );
          }

          m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleUpdateButton( false );

          m_pJoinFestaAttractionListSelectViewList->RemoveLowerView();
        }
      }
      break;

      // ----------------------------------------------------------------------------------------
      case jf_playerlist_win_31:
      {
        if( bTouchScreen )
        {
          m_pJoinFestaAttractionListSelectViewList->RemoveLowerView();
        }
      }
      break;

      // ----------------------------------------------------------------------------------------
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

}


void JoinFestaAttractionListSelectEntryFrame::OnAddJoinFestaPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleUpdateButton( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() );
}


void JoinFestaAttractionListSelectEntryFrame::OnUpdateJoinFesatPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person, bool isUpdate )
{
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleUpdateButton( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() );
}


void JoinFestaAttractionListSelectEntryFrame::OnDeleteJoinFestaPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  m_pJoinFestaAttractionListSelectViewObjects->GetJoinFestaAttractionSelectLowerView()->SetVisibleUpdateButton( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() );
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)
