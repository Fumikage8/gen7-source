// ============================================================================
/*
 * @file JoinFestaPlayerListSelectFrame.cpp
 * @brief プレイヤー単体選択アプリの選択プロセスです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelect/source/Frame/JoinFestaPlayerListSelectFrame.h"
#include "NetApp/JoinFestaPlayerListSelect/source/JoinFestaPlayerListSelectFrameResult.h"

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListPlayerInfoUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListSelectLowerViewBase.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerView.h"

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"

#include "NetStatic/NetLib/include/NijiNetworkSystem.h"

#include <niji_conv_header/message/msg_jf_playerlist.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)
GFL_NAMESPACE_BEGIN(Frame)


static const u32 PUSH_UPDATE_BUTTON_MESSAGE_ID    = jf_playerlist_win_02;
static const u32 PLAYER_SELECT_ONLINE_MESSAGE_ID  = jf_playerlist_win_03;
static const u32 PLAYER_SELECT_OFFLINE_MESSAGE_ID = jf_playerlist_win_01;

JoinFestaPlayerListSelectFrame::JoinFestaPlayerListSelectFrame(
  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectWork* pJoinFestaPlayerListSelectWork,
  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewSystemAccessor* pJoinFestaPlayerListSelectViewSystemAccessor ) :
  NetApp::JoinFestaPlayerListSelect::Frame::JoinFestaPlayerListSelectFrameBase( pJoinFestaPlayerListSelectWork, pJoinFestaPlayerListSelectViewSystemAccessor ),
  m_JoinFestaPlayerListSelectLowerViewListener(),
  m_JoinFestaPlayerListMessageMenuLowerViewListener()
{
}


JoinFestaPlayerListSelectFrame::~JoinFestaPlayerListSelectFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool JoinFestaPlayerListSelectFrame::startup()
{
  NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& joinFestaPlayerListManager = m_pJoinFestaPlayerListSelectWork->GetJoinFestaPlayerListManager();

  joinFestaPlayerListManager.UpdateList( true );
  joinFestaPlayerListManager.SortList();

  m_pJoinFestaPlayerListSelectViewObjects->InputDisableAll();
  m_pJoinFestaPlayerListSelectViewList->RemoveAllView();

  m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleUpdateButton( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() );
  m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleDecideButton( false );
  m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleMenuCursor( true );
  m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleCheckBox( false );

  m_pJoinFestaPlayerListSelectViewList->AddUpperView( m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListBGUpperView() );
  m_pJoinFestaPlayerListSelectViewList->AddUpperView( m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView() );

  m_pJoinFestaPlayerListSelectViewList->AddLowerView( m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListBGLowerView() );
  m_pJoinFestaPlayerListSelectViewList->AddLowerView( m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListSelectLowerViewBase() );

  setupList();

  return true;
}


void JoinFestaPlayerListSelectFrame::setupList()
{
  NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& joinFestaPlayerListManager = m_pJoinFestaPlayerListSelectWork->GetJoinFestaPlayerListManager();

  bool bShowVip = m_pJoinFestaPlayerListSelectWork->GetShowVip();

  if( bShowVip )
  {
    NetAppLib::JoinFestaUI::JoinFestPlayerDataList& vipList = joinFestaPlayerListManager.GetVipList();
    m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetList( &vipList, jf_playerlist_cap_13, NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase::LIST_TYPE_VIP );

    NetAppLib::JoinFestaUI::JoinFestaPlayerData* pVipActivePlayerData = m_pJoinFestaPlayerListSelectWork->GetVipActivePlayerData();
    
    if( pVipActivePlayerData == NULL )
    {
      NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList it = vipList.Begin();
      pVipActivePlayerData = ( vipList.Size() > 0 ) ? &(*it) : NULL;
    }
    m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->SetJoinFestaPlayerData( pVipActivePlayerData );
    setActivePlayerData( pVipActivePlayerData );
  }
  else
  {
    NetAppLib::JoinFestaUI::JoinFestPlayerDataList& guestList = joinFestaPlayerListManager.GetGuestList();
    m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetList( &guestList, jf_playerlist_cap_12, NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase::LIST_TYPE_GUEST );

    NetAppLib::JoinFestaUI::JoinFestaPlayerData* pGuestActivePlayerData = m_pJoinFestaPlayerListSelectWork->GetGuestActivePlayerData();

    if( pGuestActivePlayerData == NULL )
    {
      NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList it = guestList.Begin();
      pGuestActivePlayerData = ( guestList.Size() > 0 ) ? &(*it) : NULL;
    }
    m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->SetJoinFestaPlayerData( pGuestActivePlayerData );
    setActivePlayerData( pGuestActivePlayerData );
  }
}


void JoinFestaPlayerListSelectFrame::setActivePlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pActivePlayerData )
{
  bool bShowVip = m_pJoinFestaPlayerListSelectWork->GetShowVip();

  if( bShowVip )
  {
    m_pJoinFestaPlayerListSelectWork->SetVipActivePlayerData( pActivePlayerData );
  }
  else
  {
    m_pJoinFestaPlayerListSelectWork->SetGuestActivePlayerData( pActivePlayerData );
  }
}


void JoinFestaPlayerListSelectFrame::setListener()
{
  m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetListener( &m_JoinFestaPlayerListSelectLowerViewListener );
  m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetInputEnabled( true );

  m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetListener( &m_JoinFestaPlayerListMessageMenuLowerViewListener );

#if defined( GF_PLATFORM_CTR )
  GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPacketManager )->RegistPersonalListener( this );
#endif // defined( GF_PLATFORM_CTR )
}


void JoinFestaPlayerListSelectFrame::removeListener()
{
  m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->RemoveListener();
  m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->RemoveListener();

#if defined( GF_PLATFORM_CTR )
  GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPacketManager )->RemovePersonalListener( this );
#endif // defined( GF_PLATFORM_CTR )
}


void JoinFestaPlayerListSelectFrame::OnWLANSwitchDisable()
{
  m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleUpdateButton( false );
}


void JoinFestaPlayerListSelectFrame::updateSequence()
{
  {
    u32 inputEventID         = m_JoinFestaPlayerListSelectLowerViewListener.GetInputEventID();
    bool bTouchBackButton    = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
    bool bTouchUpdateButton  = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_UPDATE_BUTTON ) ? true : false;
    bool bPushLButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_L_BUTTON ) ? true : false;
    bool bPushRButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_R_BUTTON ) ? true : false;
    bool bSelectPlayer       = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewListener::INPUT_EVENT_ID_SELECT_PLAYER ) ? true : false;
    bool bIsUpdateUpperView  = m_JoinFestaPlayerListSelectLowerViewListener.IsUpdateUpperView();

    if( bIsUpdateUpperView )
    {
      m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->SetJoinFestaPlayerData( m_JoinFestaPlayerListSelectLowerViewListener.GetActivePlayer() );
      setActivePlayerData( m_JoinFestaPlayerListSelectLowerViewListener.GetActivePlayer() );
    }

    if( bTouchBackButton )
    {
      exitFrame( NetApp::JoinFestaPlayerListSelect::SELECT_FRAME_RESULT_BACK );
    }
    else
    if( bTouchUpdateButton )
    {
      if( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
      {
        m_pJoinFestaPlayerListSelectViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
        m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( PUSH_UPDATE_BUTTON_MESSAGE_ID );
      }
      else
      {
        m_pJoinFestaPlayerListSelectViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
        m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_31 );
      }
    }
    else
    if( bPushRButton || bPushLButton )
    {
      bool bShowVip = m_pJoinFestaPlayerListSelectWork->GetShowVip();
      bShowVip = ( bShowVip ) ? false : true;
      m_pJoinFestaPlayerListSelectWork->SetShowVip( bShowVip );
      setupList();
    }
    else
    if( bSelectPlayer )
    {
      if( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
      {
        NetAppLib::JoinFestaUI::JoinFestaPlayerData* pSelectPlayer = m_JoinFestaPlayerListSelectLowerViewListener.GetSelectPlayer();

        if( pSelectPlayer )
        {
          NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& joinFestaPlayerListManager = m_pJoinFestaPlayerListSelectWork->GetJoinFestaPlayerListManager();

          if( joinFestaPlayerListManager.IsOnline( pSelectPlayer ) )
          {
            m_pJoinFestaPlayerListSelectViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
            m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageYesNoBlackFilter( PLAYER_SELECT_ONLINE_MESSAGE_ID, pSelectPlayer->GetName() );
          }
          else
          {
            pSelectPlayer->SetPassive( true );

            m_pJoinFestaPlayerListSelectViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
            m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( PLAYER_SELECT_OFFLINE_MESSAGE_ID, pSelectPlayer->GetName() );
          }
        }
      }
      else
      {
        m_pJoinFestaPlayerListSelectViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
        m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_31 );
      }
    }

    m_JoinFestaPlayerListSelectLowerViewListener.ResetInputEvent();
  }

  {
    u32 messageID         = m_JoinFestaPlayerListMessageMenuLowerViewListener.GetMessageID();
    u32 inputEventID      = m_JoinFestaPlayerListMessageMenuLowerViewListener.GetInputEventID();
    bool bTouchScreen     = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN ) ? true : false;
    bool bTouchYesButton  = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_YES_BUTTON ) ? true : false;
    bool bTouchNoButton   = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_NO_BUTTON ) ? true : false;

    switch( messageID )
    {
      case PUSH_UPDATE_BUTTON_MESSAGE_ID:
      {
        if( bTouchScreen )
        {
          m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->ResetCurrentJoinFestaFriendKey();

          m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleUpdateButton( false );

          NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& joinFestaPlayerListManager = m_pJoinFestaPlayerListSelectWork->GetJoinFestaPlayerListManager();

          joinFestaPlayerListManager.UpdateList( false );
          joinFestaPlayerListManager.SortList();

          m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->ResetPaneListCursorInfo();
          m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->UpdateList();
          m_pJoinFestaPlayerListSelectWork->ResetActivePlayerData();

          m_pJoinFestaPlayerListSelectViewList->RemoveLowerView();
        }
      }
      break;

      case PLAYER_SELECT_ONLINE_MESSAGE_ID:
      {
        if( bTouchYesButton )
        {
          NetAppLib::JoinFestaUI::JoinFestaPlayerData* pSelectPlayer = m_JoinFestaPlayerListSelectLowerViewListener.GetSelectPlayer();

          m_pJoinFestaPlayerListSelectWork->SetSelectPlayer( pSelectPlayer );

          exitFrame( NetApp::JoinFestaPlayerListSelect::SELECT_FRAME_RESULT_BACK );
        }
        else
        if( bTouchNoButton )
        {
          m_pJoinFestaPlayerListSelectViewList->RemoveLowerView();
        }
      }
      break;

      case PLAYER_SELECT_OFFLINE_MESSAGE_ID:
      {
        if( bTouchScreen )
        {
          m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetPassive();

          m_pJoinFestaPlayerListSelectViewList->RemoveLowerView();
        }
      }
      break;

      case jf_playerlist_win_31:
      {
        if( bTouchScreen )
        {
          m_pJoinFestaPlayerListSelectViewList->RemoveLowerView();
        }
      }
      break;
    }

    m_JoinFestaPlayerListMessageMenuLowerViewListener.ResetInputEvent();
  }
}


void JoinFestaPlayerListSelectFrame::OnAddJoinFestaPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleUpdateButton( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() );
}


void JoinFestaPlayerListSelectFrame::OnUpdateJoinFesatPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person, bool isUpdate )
{
  m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleUpdateButton( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() );
}


void JoinFestaPlayerListSelectFrame::OnDeleteJoinFestaPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  m_pJoinFestaPlayerListSelectViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleUpdateButton( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() );
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)
