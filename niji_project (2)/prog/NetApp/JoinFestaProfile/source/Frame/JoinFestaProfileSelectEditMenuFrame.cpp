// ============================================================================
/*
 * @file JoinFestaProfileSelectEditMenuFrame.cpp
 * @brief 編集メニューを選択するプロセスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaProfile/source/Frame/JoinFestaProfileSelectEditMenuFrame.h"
#include "NetApp/JoinFestaProfile/source/JoinFestaProfileFrameResult.h"
#include "NetApp/JoinFestaProfile/source/View/JoinFestaProfileUpperView.h"
#include "NetApp/JoinFestaProfile/source/View/JoinFestaProfileLowerView.h"
#include "NetApp/JoinFestaProfile/source/View/JoinFestaProfileMessageMenuLowerView.h"

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaProfileSelectEditMenuFrame::JoinFestaProfileSelectEditMenuFrame(
  NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork,
  NetApp::JoinFestaProfile::System::JoinFestaProfileViewSystemAccessor* pJoinFestaProfileViewSystemAccessor ) :
  NetApp::JoinFestaProfile::Frame::JoinFestaProfileFrameBase( pJoinFestaProfileWork, pJoinFestaProfileViewSystemAccessor ),
  m_JoinFestaProfileLowerViewListener(),
  m_JoinFestaProfileMessageMenuLowerViewListener()
{
}


JoinFestaProfileSelectEditMenuFrame::~JoinFestaProfileSelectEditMenuFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool JoinFestaProfileSelectEditMenuFrame::startup()
{
  m_pJoinFestaProfileViewObjects->InputDisableAll();
  m_pJoinFestaProfileViewList->RemoveAllView();

  m_pJoinFestaProfileViewList->AddUpperView( m_pJoinFestaProfileViewObjects->GetJoinFestaPlayerListBGUpperView() );
  m_pJoinFestaProfileViewList->AddUpperView( m_pJoinFestaProfileViewObjects->GetJoinFestaProfileUpperView() );

  m_pJoinFestaProfileViewList->AddLowerView( m_pJoinFestaProfileViewObjects->GetJoinFestaPlayerListBGLowerView() );
  m_pJoinFestaProfileViewList->AddLowerView( m_pJoinFestaProfileViewObjects->GetJoinFestaProfileLowerView() );

  if( m_pJoinFestaProfileWork->IsFadeInRequest() )
  {
    FadeInRequest();
    m_pJoinFestaProfileWork->ResetFadeInRequest();
  }

  NetApp::JoinFestaProfile::System::EditMenuInfo* pSelectEditMenu = m_pJoinFestaProfileWork->GetSelectEditMenu();
  if( pSelectEditMenu )
  {
    switch( pSelectEditMenu->editMenuID )
    {
#if defined( JOINFESTAPROFILEDEFINE_LIKE_POKEMON_HONOR_ENABLE )
      // 好きなポケモン(6文字)
      case EDIT_MENU_ID_LIKE_POKEMON:
      {
        m_pJoinFestaProfileViewObjects->GetJoinFestaProfileLowerView()->UpdateLikePokemonTextPane();
      }
      break;
    
      // 肩書き(8文字)
      case EDIT_MENU_ID_HONOR:
      {
        m_pJoinFestaProfileViewObjects->GetJoinFestaProfileLowerView()->UpdateHonorTextPane();
      }
      break;
#endif // defined( JOINFESTAPROFILEDEFINE_LIKE_POKEMON_HONOR_ENABLE )

      // ジョインフェスタの名前(8文字)
      case EDIT_MENU_ID_NAME:
      {
        m_pJoinFestaProfileViewObjects->GetJoinFestaProfileUpperView()->UpdateFestaNameTextPane();
        m_pJoinFestaProfileViewObjects->GetJoinFestaProfileLowerView()->UpdateFestaNameTextPane();
      }
      break;
    }
  }

  return true;
}


void JoinFestaProfileSelectEditMenuFrame::setListener()
{
  m_pJoinFestaProfileViewObjects->GetJoinFestaProfileLowerView()->SetListener( &m_JoinFestaProfileLowerViewListener );
  m_pJoinFestaProfileViewObjects->GetJoinFestaProfileMessageMenuLowerView()->SetListener( &m_JoinFestaProfileMessageMenuLowerViewListener );

  m_pJoinFestaProfileViewObjects->GetJoinFestaProfileLowerView()->SetInputEnabled( true );
}


void JoinFestaProfileSelectEditMenuFrame::removeListener()
{
  m_pJoinFestaProfileViewObjects->GetJoinFestaProfileLowerView()->RemoveListener();
  m_pJoinFestaProfileViewObjects->GetJoinFestaProfileMessageMenuLowerView()->RemoveListener();
}


void JoinFestaProfileSelectEditMenuFrame::updateSequence()
{
  {
    u32 inputEventID       = m_JoinFestaProfileLowerViewListener.GetInputEventID();
    bool bTouchBackButton  = ( inputEventID == NetApp::JoinFestaProfile::View::JoinFestaProfileLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
    bool bSelectEditMenu   = ( inputEventID == NetApp::JoinFestaProfile::View::JoinFestaProfileLowerViewListener::INPUT_EVENT_ID_SELECT_EDIT_MENU ) ? true : false;
    bool bTouchInfoButton  = ( inputEventID == NetApp::JoinFestaProfile::View::JoinFestaProfileLowerViewListener::INPUT_EVENT_ID_TOUCH_INFO_BUTTON ) ? true : false;

    if( bTouchBackButton )
    {
      exitFrame( NetApp::JoinFestaProfile::SELECT_EDIT_MENU_FRAME_RESULT_BACK );
    }
    else
    if( bSelectEditMenu )
    {
      NetApp::JoinFestaProfile::System::EditMenuInfo* pSelectEditMenu = m_JoinFestaProfileLowerViewListener.GetSelectEditMenu();

      if( pSelectEditMenu )
      {
        switch( pSelectEditMenu->editMenuID )
        {
          case EDIT_MENU_ID_MEETING_MESSAGE:
          case EDIT_MENU_ID_THROB:
          case EDIT_MENU_ID_REGRETFUL:
          case EDIT_MENU_ID_GOODBY_MESSAGE:
          case EDIT_MENU_ID_MY_SECRET:
          {
            m_pJoinFestaProfileWork->SetSelectEditMenu( pSelectEditMenu );

            m_pJoinFestaProfileWork->SetBootProcID( NetApp::JoinFestaProfile::BOOT_PROC_ID_SELECT_WORD );

            exitFrame( NetApp::JoinFestaProfile::SELECT_EDIT_MENU_FRAME_RESULT_BACK );
          }
          break;

#if defined( JOINFESTAPROFILEDEFINE_LIKE_POKEMON_HONOR_ENABLE )
          case EDIT_MENU_ID_LIKE_POKEMON:
          case EDIT_MENU_ID_HONOR:
#endif // defined( JOINFESTAPROFILEDEFINE_LIKE_POKEMON_HONOR_ENABLE )
          case EDIT_MENU_ID_NAME:
          {
            m_pJoinFestaProfileWork->SetSelectEditMenu( pSelectEditMenu );

            FadeOutRequest();
            exitFrame( NetApp::JoinFestaProfile::SELECT_EDIT_MENU_FRAME_RESULT_TO_NAME_INPUT );
          }
          break;

          case EDIT_MENU_ID_BATTLE:
          case EDIT_MENU_ID_TRADE:
          case EDIT_MENU_ID_ATTRACTION:
          case EDIT_MENU_ID_GUEST:
          case EDIT_MENU_ID_VIP:
          case EDIT_MENU_ID_FRIEND:
          {
            m_pJoinFestaProfileViewList->AddLowerViewInputEnable( m_pJoinFestaProfileViewObjects->GetJoinFestaProfileMessageMenuLowerView() );
            m_pJoinFestaProfileViewObjects->GetJoinFestaProfileMessageMenuLowerView()->SetMessageMenuBlackFilter( pSelectEditMenu->selectMessageID, pSelectEditMenu->selectMenuID0, pSelectEditMenu->selectMenuID1, pSelectEditMenu->selectMenuID2 );
          }
          break;
        }
      }
    }
    else
    if( bTouchInfoButton )
    {
      m_pJoinFestaProfileWork->SetDescriptionType( NetApp::JoinFestaProfile::System::JoinFestaProfileWork::DESCRIPTION_TYPE_FIRST_BOOT_DESCRIPTION );
      m_pJoinFestaProfileWork->SetDescriptionOpenAnimeFlag( true );
      exitFrame( NetApp::JoinFestaProfile::SELECT_EDIT_MENU_FRAME_RESULT_TO_DESCRIPTION );
    }

    m_JoinFestaProfileLowerViewListener.ResetInputEvent();
  }

  {
    u32 inputEventID  = m_JoinFestaProfileMessageMenuLowerViewListener.GetInputEventID();
    u32 messageID     = m_JoinFestaProfileMessageMenuLowerViewListener.GetMessageID();
    u32 selectMenuID  = m_JoinFestaProfileMessageMenuLowerViewListener.GetSelectMenuID();
    bool bTouchScreen = ( inputEventID == NetApp::JoinFestaProfile::View::JoinFestaProfileMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN ) ? true : false;
    bool bSelectMenu  = ( inputEventID == NetApp::JoinFestaProfile::View::JoinFestaProfileMessageMenuLowerViewListener::INPUT_EVENT_ID_SELECT_MENU ) ? true : false;

    if( bSelectMenu )
    {
      NetApp::JoinFestaProfile::System::EditMenuInfo* pSelectEditMenu = m_JoinFestaProfileLowerViewListener.GetSelectEditMenu();

      if( pSelectEditMenu )
      {
        if( messageID == pSelectEditMenu->selectMessageID )
        {
          if( selectMenuID == pSelectEditMenu->selectMenuID0 )
          {
            m_pJoinFestaProfileViewObjects->GetJoinFestaProfileMessageMenuLowerView()->SetStreamMessageBlackFilter( pSelectEditMenu->selectMenuMessageID0 );
          }
          else
          if( selectMenuID == pSelectEditMenu->selectMenuID1 )
          {
            m_pJoinFestaProfileViewObjects->GetJoinFestaProfileMessageMenuLowerView()->SetStreamMessageBlackFilter( pSelectEditMenu->selectMenuMessageID1 );
          }
          else
          if( selectMenuID == pSelectEditMenu->selectMenuID2 )
          {
            m_pJoinFestaProfileWork->SetSelectEditMenu( pSelectEditMenu );
            m_pJoinFestaProfileWork->SetDescriptionType( NetApp::JoinFestaProfile::System::JoinFestaProfileWork::DESCRIPTION_TYPE_MENU_DESCRIPTION );
            m_pJoinFestaProfileWork->SetDescriptionOpenAnimeFlag( false );
            exitFrame( NetApp::JoinFestaProfile::SELECT_EDIT_MENU_FRAME_RESULT_TO_DESCRIPTION );
          }
        }
      }
    }

    if( bTouchScreen )
    {
      NetApp::JoinFestaProfile::System::EditMenuInfo* pSelectEditMenu = m_JoinFestaProfileLowerViewListener.GetSelectEditMenu();

      if( pSelectEditMenu )
      {
        if( messageID == pSelectEditMenu->selectMenuMessageID0 )
        {
          onSelectYes( pSelectEditMenu );

          m_pJoinFestaProfileViewList->RemoveLowerView();

          m_pJoinFestaProfileViewObjects->GetJoinFestaProfileLowerView()->UpdateOKNGTextPane();
        }
        else
        if( messageID == pSelectEditMenu->selectMenuMessageID1 )
        {
          onSelectNo( pSelectEditMenu );

          m_pJoinFestaProfileViewList->RemoveLowerView();

          m_pJoinFestaProfileViewObjects->GetJoinFestaProfileLowerView()->UpdateOKNGTextPane();
        }
      }
    }

    m_JoinFestaProfileMessageMenuLowerViewListener.ResetInputEvent();
  }
}


void JoinFestaProfileSelectEditMenuFrame::onSelectYes( NetApp::JoinFestaProfile::System::EditMenuInfo* pSelectEditMenu )
{
  Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  bool bSetFlag = false;

  switch( pSelectEditMenu->editMenuID )
  {
    case EDIT_MENU_ID_BATTLE:
    {
      pJoinFestaDataSave->SetAlwaysBattleOk( true );
      bSetFlag = true;
    }
    break;

    case EDIT_MENU_ID_TRADE:
    {
      pJoinFestaDataSave->SetAlwaysTradeOk( true );
      bSetFlag = true;
    }
    break;

    case EDIT_MENU_ID_ATTRACTION:
    {
      pJoinFestaDataSave->SetAlwaysAttractionOk( true );
      bSetFlag = true;
    }
    break;

    case EDIT_MENU_ID_GUEST:
    {
      pJoinFestaDataSave->SetRejectGuest( false );
      bSetFlag = true;
    }
    break;

    case EDIT_MENU_ID_VIP:
    {
      pJoinFestaDataSave->SetRejectVip( false );
      bSetFlag = true;
    }
    break;

    case EDIT_MENU_ID_FRIEND:
    {
      pJoinFestaDataSave->SetRejectFriend( false );
      bSetFlag = true;
    }
    break;
  }

  if( bSetFlag )
  {
#if defined( GF_PLATFORM_CTR )

    NetAppLib::JoinFesta::JoinFestaPacketManager* pJoinFestaPacketManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPacketManager );

    pJoinFestaPacketManager->SetPacketGameStatusWithMe();

    pJoinFestaPacketManager->SendMyData();

#endif // defined( GF_PLATFORM_CTR )
  }

}


void JoinFestaProfileSelectEditMenuFrame::onSelectNo( NetApp::JoinFestaProfile::System::EditMenuInfo* pSelectEditMenu )
{
  Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);

  bool bSetFlag = false;

  switch( pSelectEditMenu->editMenuID )
  {
    case EDIT_MENU_ID_BATTLE:
    {
      pJoinFestaDataSave->SetAlwaysBattleOk( false );
      bSetFlag = true;
    }
    break;

    case EDIT_MENU_ID_TRADE:
    {
      pJoinFestaDataSave->SetAlwaysTradeOk( false );
      bSetFlag = true;
    }
    break;

    case EDIT_MENU_ID_ATTRACTION:
    {
      pJoinFestaDataSave->SetAlwaysAttractionOk( false );
      bSetFlag = true;
    }
    break;

    case EDIT_MENU_ID_GUEST:
    {
      pJoinFestaDataSave->SetRejectGuest( true );
      bSetFlag = true;
    }
    break;

    case EDIT_MENU_ID_VIP:
    {
      pJoinFestaDataSave->SetRejectVip( true );
      bSetFlag = true;
    }
    break;

    case EDIT_MENU_ID_FRIEND:
    {
      pJoinFestaDataSave->SetRejectFriend( true );
      bSetFlag = true;
    }
    break;
  }

  if( bSetFlag )
  {
#if defined( GF_PLATFORM_CTR )

    NetAppLib::JoinFesta::JoinFestaPacketManager* pJoinFestaPacketManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPacketManager );

    pJoinFestaPacketManager->SetPacketGameStatusWithMe();

    pJoinFestaPacketManager->SendMyData();

#endif // defined( GF_PLATFORM_CTR )
  }

}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)
