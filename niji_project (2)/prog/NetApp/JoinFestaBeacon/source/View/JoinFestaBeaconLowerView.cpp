//=============================================================================
/**
 * @file    JoinFestaBeaconLowerView.cpp
 * @brief   ビーコン下画面
 * @author  kousaka_shin
 * @date    
 */
//=============================================================================

// module

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "Savedata/include/MyStatus.h"

#include "NetApp/JoinFestaBeacon/source/System/JoinFestaBeaconWork.h"
#include "NetApp/JoinFestaBeacon/source/JoinFestaBeaconResourceID.h"
#include "NetApp/JoinFestaBeacon/source/View/JoinFestaBeaconUpperView.h"

#include "JoinFestaBeaconLowerView.h"
#include "heap/include/gfl2_heap_manager.h"
#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"
// niji
#include "Sound/include/Sound.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/join_festa_beacon.gaix"
#include "arc_index/message.gaix"
#include "niji_conv_header/app/join_festa_beacon/join_festa_beacon.h"
#include "niji_conv_header/app/join_festa_beacon/join_festa_beacon_pane.h"
#include "niji_conv_header/app/join_festa_beacon/join_festa_beacon_anim_list.h"
#include <niji_conv_header/message/msg_jf_menu.h>

#if defined(GF_PLATFORM_CTR)
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#endif

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)
GFL_NAMESPACE_BEGIN(View)




//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
JoinFestaBeaconLowerView::JoinFestaBeaconLowerView( NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork* pJoinFestaBeaconWork , NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu , 
                                          app::util::Heap * pHeap , NetApp::JoinFestaBeacon::View::JoinFestaBeaconUpperView* pUpperView )
  : app::ui::UIView( pHeap , pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  , m_heap( pHeap )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_isDrawEnable( true )
  , m_pJoinFestaBeaconWork( pJoinFestaBeaconWork )
  , m_netAppMessageMenu( netAppMessageMenu ) 
  , m_result( RESULT_NONE )
  , m_pUpperView( pUpperView )
  , m_isButtonHide( false )
  , m_playerNetIDForName( PLAYER_NET_ID_NULL )
{
  CreateLayout();
  CreateButton();

  m_netAppMessageMenu.SetEventHandler( this );
  AddSubView( &m_netAppMessageMenu );

  SetupFixedLayout();

  HideButton();

  SetInputListener( this );
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
JoinFestaBeaconLowerView::~JoinFestaBeaconLowerView()
{
  SetInputListener( NULL );

  m_netAppMessageMenu.RemoveFromSuperView();
  m_netAppMessageMenu.RemoveEventHandler();
  DeleteLayout();
}


void JoinFestaBeaconLowerView::SetupSelectMode( void )
{
  m_netAppMessageMenu.HideMessage();

  m_result = RESULT_NONE;
}

void JoinFestaBeaconLowerView::SetupYesNoMode( void )
{
  m_result = RESULT_NONE;
  m_netAppMessageMenu.SetYNListMode( true );
  m_netAppMessageMenu.SetFinishMode_None();
  m_netAppMessageMenu.ShowMessageWithListMenu( GARC_message_jf_menu_DAT, jf_menu_win_24, false, GARC_message_jf_menu_DAT, jf_menu_sel_22, jf_menu_sel_23 );
}

void JoinFestaBeaconLowerView::SetupMessageStreamMode( u32 messageID )
{
  m_result = RESULT_NONE;
  m_netAppMessageMenu.SetFinishMode_None();
  ShowMessage( messageID , false , false , true );
}

void JoinFestaBeaconLowerView::SetupMessageTimerMode( u32 messageID , bool isBGVisible )
{
  m_result = RESULT_NONE;
  m_netAppMessageMenu.SetFinishMode_Timer( TIMER_WAIT_FRAME );
  ShowMessage( messageID , true , true , isBGVisible );
}

void JoinFestaBeaconLowerView::SetRegisterPlayerName( u32 index )
{
#if defined(GF_PLATFORM_CTR)
  NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = m_pJoinFestaBeaconWork->GetAppParam()->m_pSelectedPersonalDataArray[index];

  gfl2::str::StrBuf name( pPersonalData->GetName() , m_heap->GetDeviceHeap() );
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaBeaconWork->GetMessageUtility();
  pMessageUtility->GetWordSet()->RegisterWord( 0 , name , pMessageUtility->GetWordSet()->GrammerFromSex( pPersonalData->GetSex() ) );
#endif
}

void JoinFestaBeaconLowerView::SetPlayerNetIDforName( s32 playerNetIDForName )
{
  m_playerNetIDForName = playerNetIDForName;
}

NetApp::JoinFestaBeacon::View::JoinFestaBeaconLowerView::SELECT_RESULT JoinFestaBeaconLowerView::GetResult( void )
{
  return m_result;
}

void JoinFestaBeaconLowerView::HideButton( void )
{
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_JFB_SLC_LOW_000_PANE_BUTTONA_00 , false );

  m_isButtonHide = true;
}

void JoinFestaBeaconLowerView::HideReturn( void )
{
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_JFB_SLC_LOW_000_PANE_BACKBUTTON , false );
}

void JoinFestaBeaconLowerView::ShowMessage( u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG )
{
  m_netAppMessageMenu.HideMessage();
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaBeaconWork->GetMessageUtility();
  m_netAppMessageMenu.App::Tool::MessageMenuView::ShowMessage( &pMessageUtility->GetExpandedString( GARC_message_jf_menu_DAT, messageID ), bImmediate, isShowTimerIcon, isShowBG );
}


void JoinFestaBeaconLowerView::SetupFixedLayout( void )
{
  gfl2::lyt::LytWk * lytwk = m_g2dUtil->GetLayoutWork( LYTID_MAIN );
  gfl2::lyt::LytMultiResID res_id = m_g2dUtil->GetLayoutResourceID( 0 );

}



void JoinFestaBeaconLowerView::Update(void)
{
  app::ui::UIView::Update(); 
}



void JoinFestaBeaconLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pJoinFestaBeaconWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LYTID_MAIN );
}

void JoinFestaBeaconLowerView::MessageMenuView_OnEvent( const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eEventCode )
{
  GFL_PRINT("JoinFestaBeaconLowerView::MessageMenuView_OnEvent : [%d]\n", eEventCode );

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
app::ui::UIInputListener::ListenerResult JoinFestaBeaconLowerView::OnLayoutPaneEvent( const u32 button_id )
{
  GFL_PRINT("JoinFestaBeaconLowerView::OnLayoutPaneEvent : button_id[%d]\n",button_id);

  if( button_id == BUTTON_KEY_CANCEL )
  {
    m_result = RESULT_EXIT;
    return DISABLE_ACCESS;
  }

  return ENABLE_ACCESS;
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
void JoinFestaBeaconLowerView::OnLayoutPaneTouchTriggerEvent( const u32 button_id )
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
app::ui::UIInputListener::ListenerResult JoinFestaBeaconLowerView::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
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

  return ENABLE_ACCESS;
}


void JoinFestaBeaconLowerView::CreateLayout(void)
{
  NetAppLib::System::ResourceManager* pResourceManager = m_pJoinFestaBeaconWork->GetResourceManager();
  void* pLayoutBuffer = pResourceManager->GetResourceBuffer( NetApp::JoinFestaBeacon::JOINFESTABEACON_RESOURCE_ID_LAYOUT );
  void* pMsgBuffer = pResourceManager->GetResourceBuffer( NetApp::JoinFestaBeacon::JOINFESTABEACON_RESOURCE_ID_MSG );

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
      LYTRES_JOIN_FESTA_BEACON_JFB_SLC_LOW_000_BFLYT,
      LA_JOIN_FESTA_BEACON_JFB_SLC_LOW_000___NUM,
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
}

void JoinFestaBeaconLowerView::DeleteLayout(void)
{
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
void JoinFestaBeaconLowerView::CreateButton(void)
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

void JoinFestaBeaconLowerView::CreateKeyButton( app::ui::ButtonInfoLytWk * info )
{
  static const app::ui::ButtonInfoLytWk info_tbl[] =
  {
    {
      BUTTON_KEY_CANCEL, NULL, NULL, NULL,
      LA_JOIN_FESTA_BEACON_JFB_SLC_LOW_000__BACKBUTTON_TOUCH,
      LA_JOIN_FESTA_BEACON_JFB_SLC_LOW_000__BACKBUTTON_RELEASE,
      LA_JOIN_FESTA_BEACON_JFB_SLC_LOW_000__BACKBUTTON_CANSEL,
      LA_JOIN_FESTA_BEACON_JFB_SLC_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_JOIN_FESTA_BEACON_JFB_SLC_LOW_000__BACKBUTTON_ACTIVE,
      LA_JOIN_FESTA_BEACON_JFB_SLC_LOW_000__BACKBUTTON_PASSIVE,
    },
  };

  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    PANENAME_JFB_SLC_LOW_000_PANE_BACKBUTTON
  };

  static const gfl2::lyt::LytPaneIndex bound_tbl[] =
  {
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
GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)