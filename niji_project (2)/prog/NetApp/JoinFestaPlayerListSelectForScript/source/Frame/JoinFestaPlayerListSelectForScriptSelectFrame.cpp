// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptSelectFrame.cpp
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)の○○プロセスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelectForScript/source/Frame/JoinFestaPlayerListSelectForScriptSelectFrame.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/JoinFestaPlayerListSelectForScriptFrameResult.h"

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListPlayerInfoUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListSelectLowerViewBase.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerView.h"

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"

#include <niji_conv_header/message/msg_jf_playerlist.h>
#include <GameSys/include/GameData.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaPlayerListSelectForScriptSelectFrame::JoinFestaPlayerListSelectForScriptSelectFrame(
  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptWork* pJoinFestaPlayerListSelectForScriptWork,
  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewSystemAccessor* pJoinFestaPlayerListSelectForScriptViewSystemAccessor ) :
  NetApp::JoinFestaPlayerListSelectForScript::Frame::JoinFestaPlayerListSelectForScriptFrameBase( pJoinFestaPlayerListSelectForScriptWork, pJoinFestaPlayerListSelectForScriptViewSystemAccessor ),
  m_JoinFestaPlayerListSelectLowerViewListener(),
  m_JoinFestaPlayerListMessageMenuLowerViewListener()
{
}


JoinFestaPlayerListSelectForScriptSelectFrame::~JoinFestaPlayerListSelectForScriptSelectFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool JoinFestaPlayerListSelectForScriptSelectFrame::startup()
{
  NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& joinFestaPlayerListManager = m_pJoinFestaPlayerListSelectForScriptWork->GetJoinFestaPlayerListManager();

  u32 listSize = 0;

  if( m_pJoinFestaPlayerListSelectForScriptWork->GetBootMode() == NetApp::JoinFestaPlayerListSelectForScript::BOOT_MODE_LIMIT )
  {
    listSize = m_pJoinFestaPlayerListSelectForScriptWork->GetListSize();
  }

  joinFestaPlayerListManager.UpdateListMode4And5( true, listSize );

  m_pJoinFestaPlayerListSelectForScriptViewObjects->InputDisableAll();
  m_pJoinFestaPlayerListSelectForScriptViewList->RemoveAllView();

  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleUpdateButton( false );
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleDecideButton( false );
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleMenuCursor( true );
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleCheckBox( false );

  m_pJoinFestaPlayerListSelectForScriptViewList->AddUpperView( m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListBGUpperView() );
  m_pJoinFestaPlayerListSelectForScriptViewList->AddUpperView( m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView() );

  m_pJoinFestaPlayerListSelectForScriptViewList->AddLowerView( m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListBGLowerView() );
  m_pJoinFestaPlayerListSelectForScriptViewList->AddLowerView( m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase() );

  {
    const bool isPhotoViewAvailable = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldMenu()->IsMenuIconEnable(Savedata::FieldMenu::ICON_ID_ALBUM);
    m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetPhotoEnable(isPhotoViewAvailable, false);
  }

  setupList();

  m_pJoinFestaPlayerListSelectForScriptWork->SetSelectPlayer( NULL );

  return true;
}


void JoinFestaPlayerListSelectForScriptSelectFrame::setupList()
{
  NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& joinFestaPlayerListManager = m_pJoinFestaPlayerListSelectForScriptWork->GetJoinFestaPlayerListManager();

  bool bShowVip = m_pJoinFestaPlayerListSelectForScriptWork->GetShowVip();

  if( bShowVip )
  {
    NetAppLib::JoinFestaUI::JoinFestPlayerDataList& vipList = joinFestaPlayerListManager.GetVipList();
    m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetList( &vipList, jf_playerlist_cap_13, NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase::LIST_TYPE_VIP );

    NetAppLib::JoinFestaUI::JoinFestaPlayerData* pVipActivePlayerData = m_pJoinFestaPlayerListSelectForScriptWork->GetVipActivePlayerData();

    if( pVipActivePlayerData == NULL )
    {
      NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList it = vipList.Begin();
      pVipActivePlayerData = ( vipList.Size() > 0 ) ? &(*it) : NULL;
    }
    m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->SetJoinFestaPlayerData( pVipActivePlayerData );
    setActivePlayerData( pVipActivePlayerData );
  }
  else
  {
    NetAppLib::JoinFestaUI::JoinFestPlayerDataList& guestList = joinFestaPlayerListManager.GetGuestList();
    m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetList( &guestList, jf_playerlist_cap_12, NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase::LIST_TYPE_GUEST );

    NetAppLib::JoinFestaUI::JoinFestaPlayerData* pGuestActivePlayerData = m_pJoinFestaPlayerListSelectForScriptWork->GetGuestActivePlayerData();

    if( pGuestActivePlayerData == NULL )
    {
      NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList it = guestList.Begin();
      pGuestActivePlayerData = ( guestList.Size() > 0 ) ? &(*it) : NULL;
    }
    m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->SetJoinFestaPlayerData( pGuestActivePlayerData );
    setActivePlayerData( pGuestActivePlayerData );
  }
}


void JoinFestaPlayerListSelectForScriptSelectFrame::setActivePlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pActivePlayerData )
{
  bool bShowVip = m_pJoinFestaPlayerListSelectForScriptWork->GetShowVip();

  if( bShowVip )
  {
    m_pJoinFestaPlayerListSelectForScriptWork->SetVipActivePlayerData( pActivePlayerData );
  }
  else
  {
    m_pJoinFestaPlayerListSelectForScriptWork->SetGuestActivePlayerData( pActivePlayerData );
  }
}


void JoinFestaPlayerListSelectForScriptSelectFrame::setListener()
{
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetListener( &m_JoinFestaPlayerListSelectLowerViewListener );
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetInputEnabled( true );

  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetListener( &m_JoinFestaPlayerListMessageMenuLowerViewListener );
}


void JoinFestaPlayerListSelectForScriptSelectFrame::removeListener()
{
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->RemoveListener();
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->RemoveListener();
}


void JoinFestaPlayerListSelectForScriptSelectFrame::updateSequence()
{
  {
    u32 inputEventID         = m_JoinFestaPlayerListSelectLowerViewListener.GetInputEventID();
    bool bTouchBackButton    = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
    bool bPushLButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_L_BUTTON ) ? true : false;
    bool bPushRButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_R_BUTTON ) ? true : false;
    bool bSelectPlayer       = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewListener::INPUT_EVENT_ID_SELECT_PLAYER ) ? true : false;
    bool bIsUpdateUpperView  = m_JoinFestaPlayerListSelectLowerViewListener.IsUpdateUpperView();

    if( bIsUpdateUpperView )
    {
      m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->SetJoinFestaPlayerData( m_JoinFestaPlayerListSelectLowerViewListener.GetActivePlayer() );
      setActivePlayerData( m_JoinFestaPlayerListSelectLowerViewListener.GetActivePlayer() );
    }

    if( bTouchBackButton )
    {
      m_pJoinFestaPlayerListSelectForScriptWork->SetSelectPlayer( NULL );

      m_pJoinFestaPlayerListSelectForScriptWork->SelectCancel( true );
      
      exitFrame( NetApp::JoinFestaPlayerListSelectForScript::SELECT_FRAME_RESULT_BACK );
    }
    else
    if( bPushRButton || bPushLButton )
    {
      bool bShowVip = m_pJoinFestaPlayerListSelectForScriptWork->GetShowVip();
      bShowVip = ( bShowVip ) ? false : true;
      m_pJoinFestaPlayerListSelectForScriptWork->SetShowVip( bShowVip );
      setupList();
    }
    else
    if( bSelectPlayer )
    {
      NetAppLib::JoinFestaUI::JoinFestaPlayerData* pSelectPlayer = m_JoinFestaPlayerListSelectLowerViewListener.GetSelectPlayer();

      if( pSelectPlayer )
      {
        m_pJoinFestaPlayerListSelectForScriptViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
        m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetMessageMenuBlackFilter( jf_playerlist_win_08, pSelectPlayer->GetName(), jf_playerlist_sel_08, jf_playerlist_sel_03, jf_playerlist_sel_05 );
      }
    }

    m_JoinFestaPlayerListSelectLowerViewListener.ResetInputEvent();
  }

  {
    u32 messageID         = m_JoinFestaPlayerListMessageMenuLowerViewListener.GetMessageID();
    u32 selectMenuID      = m_JoinFestaPlayerListMessageMenuLowerViewListener.GetSelectMenuID();
    u32 inputEventID      = m_JoinFestaPlayerListMessageMenuLowerViewListener.GetInputEventID();
    bool bTouchScreen     = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN ) ? true : false;
    bool bTouchYesButton  = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_YES_BUTTON ) ? true : false;
    bool bTouchNoButton   = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_NO_BUTTON ) ? true : false;
    bool bSelectMenu      = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener::INPUT_EVENT_ID_SELECT_MENU ) ? true : false;

    switch( messageID )
    {
      // ---------------------------------------------------------------------------------------------
      case jf_playerlist_win_08:
      {
        if( bSelectMenu )
        {
          switch( selectMenuID )
          {
            case jf_playerlist_sel_08:
            {
              m_pJoinFestaPlayerListSelectForScriptWork->SetSelectPlayer( m_JoinFestaPlayerListSelectLowerViewListener.GetSelectPlayer() );

              m_pJoinFestaPlayerListSelectForScriptViewList->RemoveLowerView();

              // @fix GFNMcat[1395]：デフォルトモード呼び出し時は選択済みフラグを立てない
              if( m_pJoinFestaPlayerListSelectForScriptWork->GetBootMode() != BOOT_MODE_DEFAULT )
              {
                setScriptSelectedFlag();
              }

              m_pJoinFestaPlayerListSelectForScriptWork->SelectCancel( false );

              exitFrame( NetApp::JoinFestaPlayerListSelectForScript::SELECT_FRAME_RESULT_SELECT_PLAYER );
            }
            break;

            case jf_playerlist_sel_03:
            {
              m_pJoinFestaPlayerListSelectForScriptWork->SetSelectPlayer( m_JoinFestaPlayerListSelectLowerViewListener.GetSelectPlayer() );

              m_pJoinFestaPlayerListSelectForScriptViewList->RemoveLowerView();

              m_pJoinFestaPlayerListSelectForScriptWork->SelectCancel( true );

              exitFrame( NetApp::JoinFestaPlayerListSelectForScript::SELECT_FRAME_RESULT_TO_DETAIL );
            }
            break;
            
            case jf_playerlist_sel_05:
            {
              m_pJoinFestaPlayerListSelectForScriptViewList->RemoveLowerView();
            }
            break;
          }
        }
      }
      break;
    }

    m_JoinFestaPlayerListMessageMenuLowerViewListener.ResetInputEvent();
  }
}


void JoinFestaPlayerListSelectForScriptSelectFrame::setScriptSelectedFlag()
{
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPersonalDataManager );
  NetAppLib::JoinFestaUI::JoinFestaPlayerData* pSelectPlayer = m_JoinFestaPlayerListSelectLowerViewListener.GetSelectPlayer();

  if( pSelectPlayer )
  {
    if( pSelectPlayer->IsVip() )
    {
      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pVipList = pJoinFestaPersonalDataManager->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP );
      
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = pVipList->Begin(); it != pVipList->End(); ++it )
      {
        if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey( pSelectPlayer->GetJoinFestaFriendKey(), (*it)->GetJoinFestaFriendKey() ) )
        {
          (*it)->SetScriptListSelected( true );
          
          break;
        }
      }
    }
    else
    if( pSelectPlayer->IsGuest() )
    {
      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pGuestList = pJoinFestaPersonalDataManager->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );

      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = pGuestList->Begin(); it != pGuestList->End(); ++it )
      {
        if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey( pSelectPlayer->GetJoinFestaFriendKey(), (*it)->GetJoinFestaFriendKey() ) )
        {
          (*it)->SetScriptListSelected( true );
          
          break;
        }
      }
    }
  }

}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)
