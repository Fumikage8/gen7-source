// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiFrame.cpp
 * @brief プレイヤー複数選択アプリの選択プロセスです。
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelectMulti/source/Frame/JoinFestaPlayerListSelectMultiFrame.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/JoinFestaPlayerListSelectMultiFrameResult.h"

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListPlayerInfoUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerView.h"

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"

#include "NetApp/JoinFestaPlayerListSelectMulti/source/View/JoinFestaPlayerListSelectMultiLowerView.h"

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"

#include "NetStatic/NetLib/include/NijiNetworkSystem.h"

#include <niji_conv_header/message/msg_jf_playerlist.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_BEGIN(Frame)


static const u32 PUSH_UPDATE_BUTTON_MESSAGE_ID             = jf_playerlist_win_02;
static const u32 PLAYER_SELECT_OFFLINE_MESSAGE_ID          = jf_playerlist_win_01;
static const u32 LOOP_PLAYER_SELECT_OFFLINE_MESSAGE_ID     = jf_playerlist_win_04;
static const u32 LOOP_END_PLAYER_SELECT_OFFLINE_MESSAGE_ID = jf_playerlist_win_05;


JoinFestaPlayerListSelectMultiFrame::JoinFestaPlayerListSelectMultiFrame(
  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiWork* pJoinFestaPlayerListSelectMultiWork,
  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewSystemAccessor* pJoinFestaPlayerListSelectMultiViewSystemAccessor ) :
  NetApp::JoinFestaPlayerListSelectMulti::Frame::JoinFestaPlayerListSelectMultiFrameBase( pJoinFestaPlayerListSelectMultiWork, pJoinFestaPlayerListSelectMultiViewSystemAccessor ),
  m_JoinFestaPlayerListSelectLowerViewListener(),
  m_JoinFestaPlayerListMessageMenuLowerViewListener(),
  m_SelectOffLineList( pJoinFestaPlayerListSelectMultiWork->GetDeviceHeap(), NetApp::JoinFestaPlayerListSelectMulti::SELECT_PLAYER_MAX )
{
}


JoinFestaPlayerListSelectMultiFrame::~JoinFestaPlayerListSelectMultiFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool JoinFestaPlayerListSelectMultiFrame::startup()
{
  NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& joinFestaPlayerListManager = m_pJoinFestaPlayerListSelectMultiWork->GetJoinFestaPlayerListManager();

  joinFestaPlayerListManager.UpdateList( true );
  joinFestaPlayerListManager.SortList();

  m_pJoinFestaPlayerListSelectMultiViewObjects->InputDisableAll();
  m_pJoinFestaPlayerListSelectMultiViewList->RemoveAllView();

  m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView()->SetVisibleUpdateButton( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() );
  m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView()->SetVisibleDecideButton( true );
  m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView()->SetVisibleMenuCursor( true );
  m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView()->SetVisibleCheckBox( true );

  m_pJoinFestaPlayerListSelectMultiViewList->AddUpperView( m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListBGUpperView() );
  m_pJoinFestaPlayerListSelectMultiViewList->AddUpperView( m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView() );

  m_pJoinFestaPlayerListSelectMultiViewList->AddLowerView( m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListBGLowerView() );
  m_pJoinFestaPlayerListSelectMultiViewList->AddLowerView( m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView() );

  m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView()->SetPassiveDecideButton();

  setupList();

  return true;
}


void JoinFestaPlayerListSelectMultiFrame::setupList()
{
  NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& joinFestaPlayerListManager = m_pJoinFestaPlayerListSelectMultiWork->GetJoinFestaPlayerListManager();

  bool bShowVip = m_pJoinFestaPlayerListSelectMultiWork->GetShowVip();

  if( bShowVip )
  {
    NetAppLib::JoinFestaUI::JoinFestPlayerDataList& vipList = joinFestaPlayerListManager.GetVipList();
    m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView()->SetList( &vipList, jf_playerlist_cap_13, NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase::LIST_TYPE_VIP );

    NetAppLib::JoinFestaUI::JoinFestaPlayerData* pVipActivePlayerData = m_pJoinFestaPlayerListSelectMultiWork->GetVipActivePlayerData();

    if( pVipActivePlayerData == NULL )
    {
      NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList it = vipList.Begin();
      pVipActivePlayerData = ( vipList.Size() > 0 ) ? &(*it) : NULL;
    }
    m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->SetJoinFestaPlayerData( pVipActivePlayerData );
    setActivePlayerData( pVipActivePlayerData );
  }
  else
  {
    NetAppLib::JoinFestaUI::JoinFestPlayerDataList& guestList = joinFestaPlayerListManager.GetGuestList();
    m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView()->SetList( &guestList, jf_playerlist_cap_12, NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase::LIST_TYPE_GUEST );

    NetAppLib::JoinFestaUI::JoinFestaPlayerData* pGuestActivePlayerData = m_pJoinFestaPlayerListSelectMultiWork->GetGuestActivePlayerData();

    if( pGuestActivePlayerData == NULL )
    {
      NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList it = guestList.Begin();
      pGuestActivePlayerData = ( guestList.Size() > 0 ) ? &(*it) : NULL;
    }
    m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->SetJoinFestaPlayerData( pGuestActivePlayerData );
    setActivePlayerData( pGuestActivePlayerData );
  }
}


void JoinFestaPlayerListSelectMultiFrame::setActivePlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pActivePlayerData )
{
  bool bShowVip = m_pJoinFestaPlayerListSelectMultiWork->GetShowVip();

  if( bShowVip )
  {
    m_pJoinFestaPlayerListSelectMultiWork->SetVipActivePlayerData( pActivePlayerData );
  }
  else
  {
    m_pJoinFestaPlayerListSelectMultiWork->SetGuestActivePlayerData( pActivePlayerData );
  }
}


void JoinFestaPlayerListSelectMultiFrame::setListener()
{
  m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView()->SetListener( &m_JoinFestaPlayerListSelectLowerViewListener );
  m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView()->SetInputEnabled( true );

  m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetListener( &m_JoinFestaPlayerListMessageMenuLowerViewListener );

#if defined( GF_PLATFORM_CTR )
  GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPacketManager )->RegistPersonalListener( this );
#endif // defined( GF_PLATFORM_CTR )
}


void JoinFestaPlayerListSelectMultiFrame::removeListener()
{
  m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView()->RemoveListener();
  m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->RemoveListener();

#if defined( GF_PLATFORM_CTR )
  GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPacketManager )->RemovePersonalListener( this );
#endif // defined( GF_PLATFORM_CTR )
}


void JoinFestaPlayerListSelectMultiFrame::OnWLANSwitchDisable()
{
  m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView()->SetVisibleUpdateButton( false );
}


void JoinFestaPlayerListSelectMultiFrame::updateSequence()
{
  {
    u32 inputEventID         = m_JoinFestaPlayerListSelectLowerViewListener.GetInputEventID();
    bool bTouchBackButton    = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
    bool bTouchUpdateButton  = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_UPDATE_BUTTON ) ? true : false;
    bool bTouchDecideButton  = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_DECIDE_BUTTON ) ? true : false;
    bool bPushLButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_L_BUTTON ) ? true : false;
    bool bPushRButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_R_BUTTON ) ? true : false;
    bool bSelectPlayer       = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewListener::INPUT_EVENT_ID_SELECT_PLAYER ) ? true : false;
    bool bIsUpdateUpperView  = m_JoinFestaPlayerListSelectLowerViewListener.IsUpdateUpperView();

    if( bIsUpdateUpperView )
    {
      m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->SetJoinFestaPlayerData( m_JoinFestaPlayerListSelectLowerViewListener.GetActivePlayer() );
      setActivePlayerData( m_JoinFestaPlayerListSelectLowerViewListener.GetActivePlayer() );
    }

    if( bTouchBackButton )
    {
      m_pJoinFestaPlayerListSelectMultiWork->SelectCancel( true );
      exitFrame( NetApp::JoinFestaPlayerListSelectMulti::SELECT_FRAME_RESULT_BACK );
    }
    else
    if( bTouchUpdateButton )
    {
      if( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
      {
        m_pJoinFestaPlayerListSelectMultiViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
        m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( PUSH_UPDATE_BUTTON_MESSAGE_ID );
      }
      else
      {
        m_pJoinFestaPlayerListSelectMultiViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
        m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_31 );
      }
    }
    else
    if( bTouchDecideButton )
    {
      if( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
      {
        if( isAllOnline() )
        {
          m_pJoinFestaPlayerListSelectMultiWork->SelectCancel( false );
          exitFrame( NetApp::JoinFestaPlayerListSelectMulti::SELECT_FRAME_RESULT_BACK );
        }
        else
        {
          addMessageMenuView();
        }
      }
      else
      {
        m_pJoinFestaPlayerListSelectMultiViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
        m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_31 );
      }
    }
    else
    if( bPushRButton || bPushLButton )
    {
      bool bShowVip = m_pJoinFestaPlayerListSelectMultiWork->GetShowVip();
      bShowVip = ( bShowVip ) ? false : true;
      m_pJoinFestaPlayerListSelectMultiWork->SetShowVip( bShowVip );
      setupList();
    }
    else
    if( bSelectPlayer )
    {
      /*  listでplayerが選択された  */
      if( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
      {
        updateSelectList();
      }
      else
      {
        m_pJoinFestaPlayerListSelectMultiViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
        m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_31 );
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
          m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->ResetCurrentJoinFestaFriendKey();

          m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView()->SetVisibleUpdateButton( false );

          NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& joinFestaPlayerListManager = m_pJoinFestaPlayerListSelectMultiWork->GetJoinFestaPlayerListManager();

          joinFestaPlayerListManager.UpdateList( false );
          joinFestaPlayerListManager.SortList();

          m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView()->ResetPaneListCursorInfo();
          m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView()->UpdateList();
          m_pJoinFestaPlayerListSelectMultiWork->ResetActivePlayerData();

          m_pJoinFestaPlayerListSelectMultiViewList->RemoveLowerView();

          updateSelectList();
        }
      }
      break;

      case PLAYER_SELECT_OFFLINE_MESSAGE_ID:
      {
        if( bTouchScreen )
        {
          m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView()->SetPassive();

          m_pJoinFestaPlayerListSelectMultiViewList->RemoveLowerView();
        }
      }
      break;

      case LOOP_PLAYER_SELECT_OFFLINE_MESSAGE_ID:
      {
        if( bTouchScreen )
        {
          m_pJoinFestaPlayerListSelectMultiViewList->RemoveLowerView();

          m_SelectOffLineList.PopFront();

          addMessageMenuView();
        }
      }
      break;

      case LOOP_END_PLAYER_SELECT_OFFLINE_MESSAGE_ID:
      {
        if( bTouchScreen )
        {
          m_pJoinFestaPlayerListSelectMultiViewList->RemoveLowerView();

          NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& joinFestaPlayerListManager = m_pJoinFestaPlayerListSelectMultiWork->GetJoinFestaPlayerListManager();

          joinFestaPlayerListManager.UpdateList( false );
          joinFestaPlayerListManager.SortList();

          m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView()->UpdateList();

          updateSelectList();
        }
      }
      break;

      case jf_playerlist_win_31:
      {
        if( bTouchScreen )
        {
          m_pJoinFestaPlayerListSelectMultiViewList->RemoveLowerView();
        }
      }
      break;
    }

    m_JoinFestaPlayerListMessageMenuLowerViewListener.ResetInputEvent();
  }
}


void JoinFestaPlayerListSelectMultiFrame::addMessageMenuView()
{
  if( m_SelectOffLineList.Size() > 0 )
  {
    NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList it = m_SelectOffLineList.Begin();
    m_pJoinFestaPlayerListSelectMultiViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
    m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( LOOP_PLAYER_SELECT_OFFLINE_MESSAGE_ID, it->GetName() );
  }
  else
  {
    m_pJoinFestaPlayerListSelectMultiViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
    m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( LOOP_END_PLAYER_SELECT_OFFLINE_MESSAGE_ID );
  }
}


void JoinFestaPlayerListSelectMultiFrame::updateSelectList()
{
  NetAppLib::JoinFestaUI::JoinFestaPlayerListManager&   joinFestaPlayerListManager   = m_pJoinFestaPlayerListSelectMultiWork->GetJoinFestaPlayerListManager();
  NetAppLib::JoinFestaUI::JoinFestPlayerDataList&       guestList                    = joinFestaPlayerListManager.GetGuestList();
  NetAppLib::JoinFestaUI::JoinFestPlayerDataList&       vipList                      = joinFestaPlayerListManager.GetVipList();
  NetAppLib::JoinFestaUI::JoinFestPlayerDataList&       selectPlayerList             = m_pJoinFestaPlayerListSelectMultiWork->GetSelectPlayerList();

  selectPlayerList.Erase();

  for( NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList it = guestList.Begin(); it != guestList.End(); ++it )
  {
    if( (*it).IsSelect() )
    {
      selectPlayerList.PushBack( (*it) );
    }
  }

  for( NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList it = vipList.Begin(); it != vipList.End(); ++it )
  {
    if( (*it).IsSelect() )
    {
      selectPlayerList.PushBack( (*it) );
    }
  }

  if( selectPlayerList.Size() >= selectPlayerList.MaxSize() )
  {
    m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView()->SetActiveDecideButton();
  }
  else
  {
    m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView()->SetPassiveDecideButton();
  }
}


bool JoinFestaPlayerListSelectMultiFrame::isAllOnline()
{
  bool bAllOnline = true;

  NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& joinFestaPlayerListManager = m_pJoinFestaPlayerListSelectMultiWork->GetJoinFestaPlayerListManager();

  NetAppLib::JoinFestaUI::JoinFestPlayerDataList& selectPlayerList = m_pJoinFestaPlayerListSelectMultiWork->GetSelectPlayerList();

  if( selectPlayerList.Size() >= selectPlayerList.MaxSize() )
  {
    m_SelectOffLineList.Erase();

    for( NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList sel = selectPlayerList.Begin(); sel != selectPlayerList.End(); ++sel )
    {
      if( !joinFestaPlayerListManager.IsOnline( &(*sel) ) )
      {
        m_SelectOffLineList.PushBack( (*sel) );

        bAllOnline = false;
      }
    }
  }
  else
  {
    bAllOnline = false;
  }

  return bAllOnline;
}


void JoinFestaPlayerListSelectMultiFrame::OnAddJoinFestaPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView()->SetVisibleUpdateButton( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() );
}


void JoinFestaPlayerListSelectMultiFrame::OnUpdateJoinFesatPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person, bool isUpdate )
{
  m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView()->SetVisibleUpdateButton( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() );
}


void JoinFestaPlayerListSelectMultiFrame::OnDeleteJoinFestaPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  m_pJoinFestaPlayerListSelectMultiViewObjects->GetJoinFestaPlayerListSelectMultiLowerView()->SetVisibleUpdateButton( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() );
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)
