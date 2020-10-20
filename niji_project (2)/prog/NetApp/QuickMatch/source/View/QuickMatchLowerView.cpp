//=============================================================================
/**
 * @file    QuickMatchLowerView.cpp
 * @brief   クイックマッチ画面
 * @author  kousaka_shin
 * @date    
 */
//=============================================================================

// module

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "Savedata/include/MyStatus.h"

#include "NetApp/QuickMatch/source/System/QuickMatchWork.h"
#include "NetApp/QuickMatch/source/QuickMatchResourceID.h"
#include "NetApp/QuickMatch/source/View/QuickMatchUpperView.h"

#include "QuickMatchLowerView.h"
#include "heap/include/gfl2_heap_manager.h"
#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"
// niji
#include "Sound/include/Sound.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/QuickMatch.gaix"
#include "arc_index/message.gaix"
#include "niji_conv_header/app/quickmatch/QuickMatch.h"
#include "niji_conv_header/app/quickmatch/QuickMatch_pane.h"
#include "niji_conv_header/app/quickmatch/QuickMatch_anim_list.h"
#include <niji_conv_header/message/msg_quickmatch.h>

#if defined(GF_PLATFORM_CTR)
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#endif

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(View)




//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
QuickMatchLowerView::QuickMatchLowerView( NetApp::QuickMatch::System::QuickMatchWork* pQuickMatchWork , NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu , 
                                          app::util::Heap * pHeap , NetApp::QuickMatch::View::QuickMatchUpperView* pUpperView )
  : app::ui::UIView( pHeap , pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  , m_heap( pHeap )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_isDrawEnable( true )
  , m_pQuickMatchWork( pQuickMatchWork )
  , m_netAppMessageMenu( netAppMessageMenu ) 
  , m_result( RESULT_NONE )
  , m_pUpperView( pUpperView )
  , m_cursorPos( 0 )
  , m_pMenuCursor( NULL )
  , m_pMenuCursorBuffer( NULL )
  , m_isListHide( true )
  , m_pKeyRepeatController( NULL )
  , m_playerNetIDForName( PLAYER_NET_ID_NULL )
{
  CreateMenuCursor();
  CreateLayout();
  CreateButton();

  m_netAppMessageMenu.SetEventHandler( this );
  AddSubView( &m_netAppMessageMenu );

  SetupFixedLayout();

  //使わない
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BG_MENU , false );

  SetInputListener( this );

  m_pKeyRepeatController = GFL_NEW( m_heap->GetDeviceHeap() ) app::util::KeyRepeatController( BUTTON_LIST_MAX - 1 , 0 );

  if( m_pQuickMatchWork->m_pAppParam->m_isBattleTreeMode )
  {//バトルツリーモード
    HideAllLayout();
  }
  else
  {
    //背景アニメ
    UIView::GetG2DUtil()->StartAnime( 0, LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BG_00_BG_LOOP_00 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
QuickMatchLowerView::~QuickMatchLowerView()
{
  GFL_SAFE_DELETE( m_pKeyRepeatController );
  SetInputListener( NULL );

  m_netAppMessageMenu.RemoveFromSuperView();
  m_netAppMessageMenu.RemoveEventHandler();
  DeleteLayout();
  DeleteMenuCursor();
}


void QuickMatchLowerView::SetupSelectMode( void )
{
  m_netAppMessageMenu.HideMessage();
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_00 , true );
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_01 , true );
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_02 , true );
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_03 , true );
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_04 , false );//ロイヤル削除
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_MESSAGE_L1 , true );

  m_isListHide = false;

  m_result = RESULT_NONE;
  MoveMenuCursor( 0 );
}

void QuickMatchLowerView::SetupYesNoMode( void )
{
  m_pMenuCursor->SetVisible( false );
  SetRegisterPlayerName();
  m_result = RESULT_NONE;
  m_netAppMessageMenu.SetYNListMode( true );
  m_netAppMessageMenu.SetFinishMode_None();
  m_netAppMessageMenu.ShowMessageWithListMenu( GARC_message_quickmatch_DAT, msg_quickmatch_win_03, false, GARC_message_quickmatch_DAT, msg_quickmatch_sel_06, msg_quickmatch_sel_07 );
}

void QuickMatchLowerView::SetupMessageStreamMode( u32 messageID )
{
  m_pMenuCursor->SetVisible( false );
  SetRegisterPlayerName();
  m_result = RESULT_NONE;
  //m_netAppMessageMenu.SetFinishMode_UserInput();
  m_netAppMessageMenu.SetFinishMode_None();
  ShowMessage( messageID , false , false , true );
}

void QuickMatchLowerView::SetupMessageTimerMode( u32 messageID , bool isBGVisible )
{
  m_pMenuCursor->SetVisible( false );
  SetRegisterPlayerName();
  m_result = RESULT_NONE;
  m_netAppMessageMenu.SetFinishMode_Timer( TIMER_WAIT_FRAME );
  ShowMessage( messageID , true , true , isBGVisible );
}

void QuickMatchLowerView::SetRegisterPlayerName( void )
{
#if defined(GF_PLATFORM_CTR)
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);

  s32 playerNetIDForName = 1 - pP2pConnectionManager->GetNetID();
  if( m_playerNetIDForName != PLAYER_NET_ID_NULL )
  {
    playerNetIDForName = m_playerNetIDForName;
    m_playerNetIDForName = PLAYER_NET_ID_NULL;
  }

  Savedata::MyStatus* pStatus = m_pQuickMatchWork->GetMyStatus( playerNetIDForName );
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pQuickMatchWork->GetMessageUtility();
  pMessageUtility->GetWordSet()->RegisterPlayerName( 0 , pStatus );
#endif
}

void QuickMatchLowerView::SetPlayerNetIDforName( s32 playerNetIDForName )
{
  m_playerNetIDForName = playerNetIDForName;
}

NetApp::QuickMatch::View::QuickMatchLowerView::SELECT_RESULT QuickMatchLowerView::GetResult( void )
{
  return m_result;
}

void QuickMatchLowerView::HideListLayout( void )
{
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_00 , false );
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_01 , false );
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_02 , false );
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_03 , false );
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_04 , false );
  //m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BACKBUTTON , false );
  //m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BG_MENU , false );
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_MESSAGE_L1 , false );

  m_cursorPos = 0;
  m_isListHide = true;
}


void QuickMatchLowerView::HideAllLayout( void )
{
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BG_00 , false );
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_00 , false );
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_01 , false );
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_02 , false );
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_03 , false );
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_04 , false );
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BACKBUTTON , false );
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BG_MENU , false );
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_LOW_000_PANE_MESSAGE_L1 , false );
}

void QuickMatchLowerView::ShowMessage( u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG )
{
  m_netAppMessageMenu.HideMessage();
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pQuickMatchWork->GetMessageUtility();
  m_netAppMessageMenu.App::Tool::MessageMenuView::ShowMessage( &pMessageUtility->GetExpandedString( GARC_message_quickmatch_DAT, messageID ), bImmediate, isShowTimerIcon, isShowBG );
}


void QuickMatchLowerView::SetupFixedLayout( void )
{
  gfl2::lyt::LytWk * lytwk = m_g2dUtil->GetLayoutWork( LYTID_MAIN );
  gfl2::lyt::LytMultiResID res_id = m_g2dUtil->GetLayoutResourceID( 0 );

  m_g2dUtil->SetTextBoxPaneString( lytwk->GetTextBoxPane(lytwk->GetPartsPane(PANENAME_QUICKMATCH_MIN_LOW_000_PANE_MESSAGE_L1),
    PANENAME_COMMON_MSG_LOW_003_PANE_TEXTBOX_00,&res_id),static_cast<u32>( msg_quickmatch_win_02 ) );
  m_g2dUtil->SetPaneVisible( lytwk->GetTextBoxPane(lytwk->GetPartsPane(PANENAME_QUICKMATCH_MIN_LOW_000_PANE_MESSAGE_L1),PANENAME_COMMON_MSG_LOW_003_PANE_TEXTBOX_00 ,&res_id), true );

  m_g2dUtil->SetTextBoxPaneString( lytwk->GetTextBoxPane(lytwk->GetPartsPane(PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_00),
    PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00,&res_id),static_cast<u32>( msg_quickmatch_sel_02 ) );

  m_g2dUtil->SetTextBoxPaneString( lytwk->GetTextBoxPane(lytwk->GetPartsPane(PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_01),
    PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00,&res_id),static_cast<u32>( msg_quickmatch_sel_03 ) );

  m_g2dUtil->SetTextBoxPaneString( lytwk->GetTextBoxPane(lytwk->GetPartsPane(PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_02),
    PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00,&res_id),static_cast<u32>( msg_quickmatch_sel_01 ) );

  m_g2dUtil->SetTextBoxPaneString( lytwk->GetTextBoxPane(lytwk->GetPartsPane(PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_03),
    PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00,&res_id),static_cast<u32>( msg_quickmatch_sel_05 ) );

  //m_g2dUtil->SetTextBoxPaneString( lytwk->GetTextBoxPane(lytwk->GetPartsPane(PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_04),
  //  PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00,&res_id),static_cast<u32>( msg_quickmatch_sel_05 ) );
}


void QuickMatchLowerView::MoveMenuCursor( s32 mv )
{
  GFL_PRINT("QuickMatchLowerView::MoveMenuCursor[%d]\n",mv);

  m_cursorPos += mv;
  if( m_cursorPos < 0 )
  {
    m_cursorPos = BUTTON_LIST_MAX - 1;
  }
  else if( m_cursorPos >= BUTTON_LIST_MAX )
  {
    m_cursorPos = 0;
  }

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_MAIN );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  static const gfl2::lyt::LytPaneIndex LIST_TABLE[] =
  {
    PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_00,
    PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_01,
    PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_02,
    PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_03
  };

  m_pMenuCursor->SetVisible( true );
  m_pMenuCursor->Put(
    lytwk,
    lytwk->GetPane( lytwk->GetPartsPane( LIST_TABLE[m_cursorPos] ), PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR, &res_id ) );

  m_pUpperView->SetCursorIndex( m_cursorPos );
}


//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuCursorLayout
 * @brief   メニューカーソル用レイアウト作成
 * @date    2015.06.02
 */
//-----------------------------------------------------------------------------
void QuickMatchLowerView::CreateMenuCursorLayout(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  m_pMenuCursor->CreateLayoutWork(
    g2d->GetLayoutSystem(),
    g2d->GetLayoutWorkSetup( gfl2::lyt::DISPLAY_LOWER ),
    g2d->GetAppLytAccessor(),
    app::tool::MenuCursor::SIZE_SMALL,
    false );
//  m_pMenuCursor->SetVisible( false );
  m_pMenuCursor->StartAnime();
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMenuCursorLayout
 * @brief   メニューカーソル用レイアウト削除
 * @date    2015.06.02
 */
//-----------------------------------------------------------------------------
bool QuickMatchLowerView::DeleteMenuCursorLayout(void)
{
  return m_pMenuCursor->DeleteLayoutWork();
}


void QuickMatchLowerView::Update(void)
{
  app::ui::UIView::Update(); 
  m_pMenuCursor->Update();

  UpdateMain();
}



void QuickMatchLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pQuickMatchWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LYTID_MAIN );

  m_pMenuCursor->Draw( pAppRenderingManager, displayNo, gfl2::lyt::DISPLAY_LOWER );
}

void QuickMatchLowerView::MessageMenuView_OnEvent( const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eEventCode )
{
  GFL_PRINT("QuickMatchLowerView::MessageMenuView_OnEvent : [%d]\n", eEventCode );

  switch( eEventCode )
  {
  case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnFinish:
    {
      m_result = RESULT_MESSAGE_END;
    }
  break;
  case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnSelectedYes:
    {
      m_result = RESULT_YES;
    }
  break;
  case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnSelectedNo:
    {
      m_result = RESULT_NO;
    }
  break;
  case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnEOM:
    {
      m_result = RESULT_EOM;
    }
  break;
  case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnSelectedItem:
  break;
  }
}

bool QuickMatchLowerView::CreateMenuCursor(void)
{
  m_pMenuCursor = GFL_NEW(m_heap->GetDeviceHeap()) app::tool::MenuCursor( m_heap, &m_pMenuCursorBuffer );
  m_pMenuCursor->LoadResourceSync();
  return true;
}

bool QuickMatchLowerView::DeleteMenuCursor(void)
{
  m_pMenuCursor->DeleteResource();
  GFL_SAFE_DELETE( m_pMenuCursor );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneEvent
 * @brief   ペインイベントの検知
 * @date    2015.03.05
 *
 * @param   button_id   通知されたボタンID
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult QuickMatchLowerView::OnLayoutPaneEvent( const u32 button_id )
{
  GFL_PRINT("QuickMatchLowerView::OnLayoutPaneEvent : button_id[%d]\n",button_id);

  if( button_id == BUTTON_KEY_CANCEL )
  {
    m_result = RESULT_EXIT;
    return DISABLE_ACCESS;
  }

  if( m_isListHide )
  {
    return ENABLE_ACCESS;
  }

  //リスト非表示のときは反応しないボタンたち
  switch( button_id )
  {
  case BUTTON_1:
    {
      m_result = RESULT_SINGLE;
    }
  break;
  case BUTTON_2:
    {
      m_result = RESULT_DOUBLE;
    }
  break;
  case BUTTON_3:
    {
      m_result = RESULT_TRADE;
    }
  break;
  case BUTTON_4:
    {
      m_result = RESULT_CHANGE;
    }
  break;
  }

  return DISABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneTouchTriggerEvent
 * @brief   ボタンタッチを検知
 * @date    2015.03.05
 *
 * @param   button_id   通知されたボタンID
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
void QuickMatchLowerView::OnLayoutPaneTouchTriggerEvent( const u32 button_id )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    OnKeyAction
 * @brief   キー入力の検知
 * @date    2015.03.05
 *
 * @param   button  ボタン
 * @param   key     十字キー
 * @param   stick   アナログスティック
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult QuickMatchLowerView::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
  if( IsButtonAnimation() )
  {
    return ENABLE_ACCESS;
  }

  if( button->IsTrigger(gfl2::ui::BUTTON_B) )
  {
    app::ui::UIResponder::GetButtonManager()->StartSelectedAct( BUTTON_KEY_CANCEL );
    return DISABLE_ACCESS;
  }

  if( m_isListHide )
  {
    return ENABLE_ACCESS;
  }

  m_pKeyRepeatController->UpdateInfo( key , m_cursorPos , 0 );

  if( m_pKeyRepeatController->IsRepeat(gfl2::ui::DIR_UP) )
  {
    s32 tmp = m_cursorPos;
    MoveMenuCursor( -1 );
    if( tmp != m_cursorPos )
    {
      Sound::PlaySE( SEQ_SE_SELECT1 );
    }
    return DISABLE_ACCESS;
  }
  if( m_pKeyRepeatController->IsRepeat(gfl2::ui::DIR_DOWN) )
  {
    s32 tmp = m_cursorPos;
    MoveMenuCursor( 1 );
    if( tmp != m_cursorPos )
    {
      Sound::PlaySE( SEQ_SE_SELECT1 );
    }
    return DISABLE_ACCESS;
  }

  if( button->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    app::ui::UIResponder::GetButtonManager()->StartSelectedAct( BUTTON_1+m_cursorPos );
    return DISABLE_ACCESS;
  }


  return ENABLE_ACCESS;
}



//メイン処理
void QuickMatchLowerView::UpdateMain(void)
{
}




void QuickMatchLowerView::CreateLayout(void)
{
  NetAppLib::System::ResourceManager* pResourceManager = m_pQuickMatchWork->GetResourceManager();
  void* pLayoutBuffer = pResourceManager->GetResourceBuffer( NetApp::QuickMatch::QUICKMATCH_RESOURCE_ID_LAYOUT );
  void* pMsgBuffer = pResourceManager->GetResourceBuffer( NetApp::QuickMatch::QUICKMATCH_RESOURCE_ID_MSG );

  // リソースデータ
  const app::util::G2DUtil::LytResData res_tbl[] =
  {
    { pLayoutBuffer, 0, app::util::G2DUtil::ATTACH_ROOT },
  };
  // レイアウトデータ
	const app::util::G2DUtil::LytwkData data_tbl[] =
  {
    {
      0,
      LYTRES_QUICKMATCH_QUICKMATCH_MIN_LOW_000_BFLYT,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000___NUM,
      pLayoutBuffer,
      false,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  m_pMsgData     = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::MsgData( pMsgBuffer, m_heap->GetDeviceHeap() );
  m_pWordSet     = GFL_NEW(m_heap->GetDeviceHeap()) print::WordSet( m_heap->GetDeviceHeap() );

  Create2D(
    m_heap,
    NULL,
    LYTID_MAX,
    res_tbl,
    GFL_NELEMS(res_tbl),
    data_tbl,
    GFL_NELEMS(data_tbl),
    m_pMsgData,
    m_pWordSet );

  CreateMenuCursorLayout();
}

void QuickMatchLowerView::DeleteLayout(void)
{
  DeleteMenuCursorLayout();
  Delete2D();
  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );
}





//=============================================================================
//=============================================================================



//-----------------------------------------------------------------------------
/**
 * @func    CreateButton
 * @brief   ボタン作成
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void QuickMatchLowerView::CreateButton(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  app::ui::ButtonInfoLytWk * info = GFL_NEW_LOW_ARRAY(m_heap->GetDeviceHeap()) app::ui::ButtonInfoLytWk[BUTTON_MAX];

  CreateKeyButton( info );

  app::ui::UIResponder::CreateButtonManager( m_heap, info, BUTTON_MAX );

  GFL_DELETE_ARRAY info;

  app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();

  for( u32 i=0; i<BUTTON_MAX; i++ )
  {
    if( i == BUTTON_KEY_CANCEL )
    {
      man->SetButtonSelectSE( i, SEQ_SE_CANCEL1 );
    }
    else
    {
      man->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
    }
  }
}

void QuickMatchLowerView::CreateKeyButton( app::ui::ButtonInfoLytWk * info )
{
  static const app::ui::ButtonInfoLytWk info_tbl[] =
  {
    {
      BUTTON_1, NULL, NULL, NULL,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_00_TOUCH,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_00_RELEASE,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_00_CANSEL,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_00_TOUCH_RELEASE,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_00_ACTIVE,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_00_PASSIVE,
    },
    {
      BUTTON_2, NULL, NULL, NULL,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_01_TOUCH,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_01_RELEASE,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_01_CANSEL,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_01_TOUCH_RELEASE,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_01_ACTIVE,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_01_PASSIVE,
    },
    {
      BUTTON_3, NULL, NULL, NULL,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_02_TOUCH,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_02_RELEASE,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_02_CANSEL,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_02_TOUCH_RELEASE,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_02_ACTIVE,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_02_PASSIVE,
    },
    {
      BUTTON_4, NULL, NULL, NULL,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_03_TOUCH,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_03_RELEASE,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_03_CANSEL,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_03_TOUCH_RELEASE,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_03_ACTIVE,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BUTTON_03_PASSIVE,
    },
    {
      BUTTON_KEY_CANCEL, NULL, NULL, NULL,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BACKBUTTON_TOUCH,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BACKBUTTON_RELEASE,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BACKBUTTON_CANSEL,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BACKBUTTON_ACTIVE,
      LA_QUICKMATCH_QUICKMATCH_MIN_LOW_000__BACKBUTTON_PASSIVE,
    },
  };

  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_00,
    PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_01,
    PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_02,
    PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BUTTON_03,
    PANENAME_QUICKMATCH_MIN_LOW_000_PANE_BACKBUTTON
  };

  static const gfl2::lyt::LytPaneIndex bound_tbl[] =
  {
    PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00
  };

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_MAIN );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<GFL_NELEMS(parts_tbl); i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( parts_tbl[i] );
    info[i] = info_tbl[i];
    info[i].picture_pane = parts;
    info[i].bound_pane   = lytwk->GetBoundingPane( parts, bound_tbl[i], &res_id );
    info[i].lyt_wk       = lytwk;
  }
}








GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)