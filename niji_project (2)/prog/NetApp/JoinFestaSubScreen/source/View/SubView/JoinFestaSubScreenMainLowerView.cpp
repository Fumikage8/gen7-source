// ============================================================================
/*
 * @file JoinFestaSubScreenMainLowerView.cpp
 * @brief ジョインフェスタ下画面のメイン画面を表示するクラスです。
 * @date 2015.09.01
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenMainLowerView.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenResourceID.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenSoundDefine.h"

#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h" // NijiNetworkSystem
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaInformationMessage.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h" // エラーマネージャー

#include <arc_index/message.gaix>
#include <niji_conv_header/app/join_festa_subscreen/join_festa_subscreen.h>
#include <niji_conv_header/app/join_festa_subscreen/join_festa_subscreen_anim_list.h>
#include <niji_conv_header/app/join_festa_subscreen/join_festa_subscreen_pane.h>
#include <niji_conv_header/message/msg_jf_menu.h>

// Pane取得ヘルパー
#include  "AppLib/include/Util/app_util_GetPaneHelper.h"
// アトラクションマネージャ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"

#include <math/include/gfl2_MathTriangular.h>
#include <System/include/SystemEventManager.h>

#include <GameSys/include/GameData.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(View)


JoinFestaSubScreenMainLowerView::JoinFestaSubScreenMainLowerView( NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork ) :
  NetAppLib::System::NetApplicationViewBase( 
    pJoinFestaSubScreenWork,
    JOINFESTASUBSCREEN_RESOURCE_ID_LAYOUT,
    LYTRES_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000_BFLYT,
    LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pJoinFestaSubScreenWork->GetMessageUtility(),
    GARC_message_jf_menu_DAT ),
  m_pJoinFestaSubScreenWork( pJoinFestaSubScreenWork ),
  m_pListener( NULL ),
  m_pStreamMessagePane( NULL ),
  m_ePlayMessageState( PLAY_MESSAGE_STATE_NONE ),
  m_isWLANSwitchEnable(false),
  m_TelopProcessor(),
  m_pStremMessageBgPane(NULL),
  m_bgInFrame(0)
{
  m_isWLANSwitchEnable = NetLib::NijiNetworkSystem::IsWLANSwitchEnable();
  setupTextMessage();
  setupButton();

  SetInputListener( this );
  SetInputEnabled( false ); // 初期値は入力不許可

  // WiFi強制切断リスナー登録
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->RegistForceDisconnectListener( this );
  // システムリスナ登録
  GFL_SINGLETON_INSTANCE(System::SystemEventManager)->RegistSystemEventListener(this);
}


JoinFestaSubScreenMainLowerView::~JoinFestaSubScreenMainLowerView()
{
  // WiFi強制切断リスナー解除
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->UnregistForceDisconnectListener();
  // システムリスナ解除
  GFL_SINGLETON_INSTANCE(System::SystemEventManager)->RemoveSystemEventListener(this);
}


void JoinFestaSubScreenMainLowerView::setupTextMessage()
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTASUBSCREEN );
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    // たいせん
    {
      gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_MIN_LOW_000_PANE_MODEBUTTON_BATTLE );
      gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( pButton, PANENAME_FESTA_BTN_LOW_002_PANE_TEXTBOX_00, &resourceID );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, jf_menu_sel_04 );
    }
    // こうかん
    {
      gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_MIN_LOW_000_PANE_MODEBUTTON_TRADE );
      gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( pButton, PANENAME_FESTA_BTN_LOW_002_PANE_TEXTBOX_00, &resourceID );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, jf_menu_sel_05 );
    }
    // インターネット or ローカルつうしん
    updateNetworkModeText();
  }
}


void JoinFestaSubScreenMainLowerView::setupButton()
{
  app::util::G2DUtil* pG2DUtil = GetG2DUtil();
  gfl2::lyt::LytMultiResID& resourceID = pG2DUtil->GetLayoutResourceID(LAYOUT_RESOURCE_ID_JOINFESTASUBSCREEN);
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork(LAYOUT_WORK_ID_LOWER);

  // ボタン情報
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_BATTLE, NULL, NULL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODEBUTTON_BATTLE_TOUCH,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODEBUTTON_BATTLE_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODEBUTTON_BATTLE_CANSEL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODEBUTTON_BATTLE_TOUCH_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODEBUTTON_BATTLE_ACTIVE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODEBUTTON_BATTLE_PASSIVE,
    },
    {
      BUTTON_ID_TRADE, NULL, NULL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODEBUTTON_TRADE_TOUCH,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODEBUTTON_TRADE_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODEBUTTON_TRADE_CANSEL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODEBUTTON_TRADE_TOUCH_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODEBUTTON_TRADE_ACTIVE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODEBUTTON_TRADE_PASSIVE,
    },
    {
      BUTTON_ID_STREAM_MSG, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    {
      BUTTON_ID_PLAYER_LIST, NULL, NULL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_00_TOUCH,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_00_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_00_CANSEL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_00_TOUCH_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_00_ACTIVE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_00_PASSIVE,
    },
    {
      BUTTON_ID_MY_PROFILE, NULL, NULL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_01_TOUCH,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_01_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_01_CANSEL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_01_TOUCH_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_01_ACTIVE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_01_PASSIVE,
    },
    {
      BUTTON_ID_POKELIST, NULL, NULL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_02_TOUCH,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_02_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_02_CANSEL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_02_TOUCH_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_02_ACTIVE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_02_PASSIVE,
    },
    {
      BUTTON_ID_BAG, NULL, NULL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_03_TOUCH,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_03_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_03_CANSEL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_03_TOUCH_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_03_ACTIVE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_03_PASSIVE,
    },
    {
      BUTTON_ID_INTERNET, NULL, NULL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_04_TOUCH,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_04_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_04_CANSEL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_04_TOUCH_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_04_ACTIVE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__MODETUB_04_PASSIVE,
    },
    {
      BUTTON_ID_BACK, NULL, NULL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__BACKBUTTON_TOUCH,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__BACKBUTTON_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__BACKBUTTON_CANSEL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__BACKBUTTON_ACTIVE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_MIN_LOW_000__BACKBUTTON_PASSIVE,
    },
  };

  // ピクチャペインとバウンディングボックスは後から入れる
  {// 対戦ボタン
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_MIN_LOW_000_PANE_MODEBUTTON_BATTLE );
    buttonInfoTable[ BUTTON_ID_BATTLE ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_BATTLE ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_FESTA_BTN_LOW_002_PANE_BOUND_00, &resourceID );
  }
  {// 交換ボタン
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_MIN_LOW_000_PANE_MODEBUTTON_TRADE );
    buttonInfoTable[ BUTTON_ID_TRADE ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_TRADE ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_FESTA_BTN_LOW_002_PANE_BOUND_00, &resourceID );
  }
  {// ストリーミングメッセージ
    buttonInfoTable[ BUTTON_ID_STREAM_MSG ].picture_pane = pLower->GetPane( PANENAME_JFSUB_MIN_LOW_000_PANE_TEXTBOX_00 ); 
    buttonInfoTable[ BUTTON_ID_STREAM_MSG ].bound_pane = pLower->GetBoundingPane( PANENAME_JFSUB_MIN_LOW_000_PANE_BOUND_00 );
    m_pStreamMessagePane = pLower->GetTextBoxPane( PANENAME_JFSUB_MIN_LOW_000_PANE_TEXTBOX_00 );
    m_pStreamMessagePane->SetVisible( false );
  }
  {// プレイヤーリストボタン
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_MIN_LOW_000_PANE_MODETUB_00 );
    buttonInfoTable[ BUTTON_ID_PLAYER_LIST ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_PLAYER_LIST ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_FESTA_BTN_LOW_003_PANE_BOUND_00, &resourceID );
    // 「!」アイコン非表示
    pLower->GetPicturePane( pButton, PANENAME_FESTA_BTN_LOW_003_PANE_ICON_00, &resourceID )->SetVisible( false );
  }
  {// Myプロフィールボタン
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_MIN_LOW_000_PANE_MODETUB_01 );
    buttonInfoTable[ BUTTON_ID_MY_PROFILE ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_MY_PROFILE ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_FESTA_BTN_LOW_003_PANE_BOUND_00, &resourceID );
    // 「!」アイコン非表示
    pLower->GetPicturePane( pButton, PANENAME_FESTA_BTN_LOW_003_PANE_ICON_00, &resourceID )->SetVisible( false );
  }
  {// ポケモンリストボタン
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_MIN_LOW_000_PANE_MODETUB_02 );
    buttonInfoTable[ BUTTON_ID_POKELIST ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_POKELIST ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_FESTA_BTN_LOW_003_PANE_BOUND_00, &resourceID );
    // 「!」アイコン非表示
    pLower->GetPicturePane( pButton, PANENAME_FESTA_BTN_LOW_003_PANE_ICON_00, &resourceID )->SetVisible( false );
  }
  {// バッグボタン
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_MIN_LOW_000_PANE_MODETUB_03 );
    buttonInfoTable[ BUTTON_ID_BAG ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_BAG ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_FESTA_BTN_LOW_003_PANE_BOUND_00, &resourceID );
    // 「!」アイコン非表示
    pLower->GetPicturePane( pButton, PANENAME_FESTA_BTN_LOW_003_PANE_ICON_00, &resourceID )->SetVisible( false );
  }
  {// インターネットボタン
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_MIN_LOW_000_PANE_MODETUB_04 );
    buttonInfoTable[ BUTTON_ID_INTERNET ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_INTERNET ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_FESTA_BTN_LOW_003_PANE_BOUND_00, &resourceID );
    // 「!」アイコン非表示
    pLower->GetPicturePane( pButton, PANENAME_FESTA_BTN_LOW_003_PANE_ICON_00, &resourceID )->SetVisible( false );
  }
  {// 戻るボタン
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_MIN_LOW_000_PANE_BACKBUTTON );
    buttonInfoTable[ BUTTON_ID_BACK ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_BACK ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &resourceID );
  }

  // インフォメーションメッセージの帯
  {
    m_pStremMessageBgPane = pLower->GetPane( PANENAME_JFSUB_MIN_LOW_000_PANE_WINDOWBG ); 
    // 初期は非表示
    m_pStremMessageBgPane->SetVisible(false);
    m_MessageBgPosition = m_pStremMessageBgPane->GetTranslate();
  }

  // ボタン生成
  CreateButtonManager( m_pJoinFestaSubScreenWork->GetAppHeap(), pLower, buttonInfoTable, BUTTON_MAX );
  GetButtonManager()->SetInputEnable( false );

  app::tool::ButtonManager * pButtonManager = GetButtonManager();
  // SE設定
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BATTLE,      SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_TRADE,       SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_STREAM_MSG,  SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_PLAYER_LIST, SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_MY_PROFILE,  SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_POKELIST,    SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BAG,         SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_INTERNET,    SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BACK,        SE_RETURN );

  // ボタンキーアサイン
  pButtonManager->SetButtonBindKey( BUTTON_ID_BACK, gfl2::ui::BUTTON_X );

  app::ui::UIView::UpdateG2DUtil();
}


void JoinFestaSubScreenMainLowerView::Update( void )
{
  app::ui::UIView::Update();

  // 無線スイッチ状態が変わった時にボタンメッセージメッセージ変える
  if( m_isWLANSwitchEnable != NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
  {
    m_isWLANSwitchEnable = NetLib::NijiNetworkSystem::IsWLANSwitchEnable();
    updateNetworkModeText();
  }
}


void JoinFestaSubScreenMainLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pJoinFestaSubScreenWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
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
::app::ui::UIInputListener::ListenerResult JoinFestaSubScreenMainLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ICHI_PRINT("JoinFestaSubScreenMainLowerView::OnLayoutPaneEvent %d \n", painId );
  if( m_pListener == NULL )
  {
    return ENABLE_ACCESS;
  }

  switch( painId ){
  case BUTTON_ID_BATTLE:      m_pListener->OnBattleButtonAction(); break;
  case BUTTON_ID_TRADE:       m_pListener->OnTradeButtonAction(); break;
  case BUTTON_ID_STREAM_MSG:  m_pListener->OnStreamMessageAction(); break;
  case BUTTON_ID_PLAYER_LIST: m_pListener->OnPlayerListButtonAction(); break;
  case BUTTON_ID_MY_PROFILE:  m_pListener->OnMyProfileButtonAction(); break;
  case BUTTON_ID_POKELIST:    m_pListener->OnPokelistButtonAction(); break;
  case BUTTON_ID_BAG:         m_pListener->OnBagButtonAction(); break;
  case BUTTON_ID_INTERNET:    m_pListener->OnInternetButtonAction(); break;
  case BUTTON_ID_BACK:        m_pListener->OnBackButtonAction(); break;
  }

  return DISABLE_ACCESS;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief  タッチパネルイベントの検知
 *
 * @param  pTouchPanel タッチパネル
 * @param  isTouch     タッチ状態であるならtrue、タッチされていないならfalse
 *                     (タッチトリガを調べたい場合はTouchPanel::IsTouchTrigger()にて判断可能）
 *
 * @return 同フレーム内での他入力イベントを許可するならtrue、
 *         他のイベントを排除するならfalseを返却すること。
 */
//--------------------------------------------------------------------------------------------
::app::ui::UIInputListener::ListenerResult JoinFestaSubScreenMainLowerView::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  return ENABLE_ACCESS;
}

// NijiNetworkForceDisconnectListener
//------------------------------------------------------------------
/**
* @brief WiFi強制切断通知
*/
//------------------------------------------------------------------
void JoinFestaSubScreenMainLowerView::OnForceDisconnect()
{
  ICHI_PRINT("OnForceDisconnect\n");
  // 通信モードのテキスト変更
  updateNetworkModeText();
}

// System::SystemEventListener
//------------------------------------------------------------------
/**
 * @brief スリープからの復帰時イベント（スリープ禁止の場合は呼ばれない）
*/
//------------------------------------------------------------------
void JoinFestaSubScreenMainLowerView::OnResumeWithSleep( void )
{
  ICHI_PRINT("OnResumeWithSleep\n");
  // 通信モードのテキスト変更
  updateNetworkModeText();
}

//------------------------------------------------------------------
/**
* @brief ホームメニューからの復帰時イベント
*/
//------------------------------------------------------------------
void JoinFestaSubScreenMainLowerView::OnResumeWithHomeMenu( void )
{
  ICHI_PRINT("OnResumeWithHomeMenu\n");
  // 通信モードのテキスト変更
  updateNetworkModeText();
}

//------------------------------------------------------------------
/**
 * @brief 一時停止
 */
//------------------------------------------------------------------
void JoinFestaSubScreenMainLowerView::Suspend()
{
  // マスク表示
  GetG2DUtil()->SetPaneVisible( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_MIN_LOW_000_PANE_BG_BLACK, true );
  // 入力無効
  SetInputEnabled( false );
  if( GetButtonManager() != NULL) { GetButtonManager()->SetInputEnable( false ); }
}

//------------------------------------------------------------------
/**
 * @brief 再起動
 */
//------------------------------------------------------------------
void JoinFestaSubScreenMainLowerView::Resume()
{
  // マスク非表示
  GetG2DUtil()->SetPaneVisible( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_MIN_LOW_000_PANE_BG_BLACK, false );
  // 入力有効
  SetInputEnabled( true );
  if( GetButtonManager() != NULL) { GetButtonManager()->SetInputEnable( true ); }
}

//------------------------------------------------------------------
/**
 * @brief メッセージ再生更新
 */
//------------------------------------------------------------------
void JoinFestaSubScreenMainLowerView::PlayMessageUpdate()
{
#if defined(GF_PLATFORM_CTR)
  // 新規メッセージチェック
  if( !IsPlayMessage() )
  {
    NetAppLib::JoinFesta::JoinFestaPacketManager* pJoinFestaPacketManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPacketManager );

    NetAppLib::JoinFesta::JoinFestaInformationMessage* pJoinFestaInformationMessage = pJoinFestaPacketManager->GetJoinFestaInformationMessage();

    if( pJoinFestaInformationMessage )
    {
      // メッセージ存在チェック
      if( pJoinFestaInformationMessage->IsMessageExist() )
      {
        NetAppLib::JoinFesta::InformationMessageData data;
        if( pJoinFestaInformationMessage->PopMessage( data ) )
        {
          PlayMessage( data );
        }
      }
    }
  }

  // インフォメーションテキスト更新
  updatePlayMessage();
#endif // GF_PLATFORM_CTR
}

//------------------------------------------------------------------
/**
 * @brief メッセージ再生
 */
//------------------------------------------------------------------
void JoinFestaSubScreenMainLowerView::PlayMessage( NetAppLib::JoinFesta::InformationMessageData& data )
{
  if( m_ePlayMessageState == PLAY_MESSAGE_STATE_NONE )
  {
    u32 messageID = data.GetMessageID();
    if( messageID < msg_jf_menu_max )
    {
      if( messageID == jf_menu_str_06 )
      {
        NetAppLib::JoinFesta::JoinFestaAttractionManager* pAttractionManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager);
        NetAppLib::JoinFesta::JoinFestaAttractionDefine::CoreData coreData = pAttractionManager->GetAttractionData( static_cast<JoinFestaScript::AttractionId>(data.GetPersonalData().GetPacketAttraction().attractionID) );

        gfl2::str::StrBuf* pTempStrBuf = GetG2DUtil()->GetTempStrBuf(1);
        print::WordSet* pWordSet = GetG2DUtil()->GetWordSet();

        pWordSet->RegisterAttractionName( 0, coreData.attractionName );
        pWordSet->Expand( pTempStrBuf, &m_pJoinFestaSubScreenWork->GetMessageUtility()->GetString( GARC_message_jf_menu_DAT, messageID ) );
        
        print::SetStringForLytTextBox(m_pStreamMessagePane, pTempStrBuf );
      }
      else
      {
        GFL_ASSERT(0); // 仕様削除
        /*gfl2::str::StrBuf* pTempStrBuf = GetG2DUtil()->GetTempStrBuf(0);
        pTempStrBuf->SetStr( data.GetPersonalData().GetName() );
        GetG2DUtil()->SetRegisterWord( 0, *pTempStrBuf, GetG2DUtil()->GetWordSet()->GrammerFromSex( static_cast<u8>( data.GetPersonalData().GetSex() ) ) );
        GetG2DUtil()->SetTextBoxPaneStringExpand( m_pStreamMessagePane, m_pJoinFestaSubScreenWork->GetMessageUtility()->GetString( GARC_message_jf_menu_DAT, messageID ) );
        */
      }
    }

    m_ePlayMessageState = PLAY_MESSAGE_STATE_START;
  }
}


//------------------------------------------------------------------
/**
 * @brief メッセージ再生中か判定
 * @retval true : 再生中です
 * @retval false : 再生中じゃないです
 */
//------------------------------------------------------------------
bool JoinFestaSubScreenMainLowerView::IsPlayMessage()
{
  bool isPlay = false;

  if( m_ePlayMessageState != PLAY_MESSAGE_STATE_NONE )
  {
    isPlay = true;
  }

  return isPlay;
}

//------------------------------------------------------------------
/**
 * @brief メッセージ強制非表示
 */
//------------------------------------------------------------------
void JoinFestaSubScreenMainLowerView::ForceMessageHide()
{
  if( IsPlayMessage() )
  {
    // 非表示にして状態も無効化
    m_pStreamMessagePane->SetVisible( false );
    m_pStremMessageBgPane->SetVisible( false );
    m_MessageBgPosition = m_pStremMessageBgPane->GetTranslate();
    m_ePlayMessageState = PLAY_MESSAGE_STATE_NONE;
  }
}

//------------------------------------------------------------------
/**
 * @brief プレイヤーリストボタンの「！」アイコン表示状態更新
 */
//------------------------------------------------------------------
void JoinFestaSubScreenMainLowerView::UpdatePlayerListModeIcon()
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTASUBSCREEN );
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {// プレイヤーリストボタン
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_MIN_LOW_000_PANE_MODETUB_00 );
    const bool isPhotoViewAvailable = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldMenu()->IsMenuIconEnable(Savedata::FieldMenu::ICON_ID_ALBUM);
    const bool bCheckNewPhoto       = isPhotoViewAvailable ? NetAppLib::JoinFesta::JoinFestaPacketUtil::CheckNewPhoto() : false;
      

    // 「!」アイコン表示・非表示設定
    pLower->GetPicturePane( pButton, PANENAME_FESTA_BTN_LOW_003_PANE_ICON_00, &resourceID )->SetVisible( bCheckNewPhoto );
  }
}


//------------------------------------------------------------------
/**
 * @brief 通信モードのテキスト更新
 */
//------------------------------------------------------------------
void JoinFestaSubScreenMainLowerView::updateNetworkModeText()
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTASUBSCREEN );
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_MIN_LOW_000_PANE_BUTTON_00 );
  gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( pButton, PANENAME_FESTA_BTN_LOW_001_PANE_TEXTBOX_00, &resourceID );

  // 無線スイッチOFF or インターネット　or ローカル通信
  if( m_isWLANSwitchEnable == false )
  {
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, jf_menu_sel_24 );
  }
  else if( NetLib::NijiNetworkSystem::GetNetworkMode() != NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
  {
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, jf_menu_sel_01 );
  }
  else
  {
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, jf_menu_sel_02 );
  }
}

//------------------------------------------------------------------
/**
 * @brief インフォメーションテキスト更新
 */
//------------------------------------------------------------------
void JoinFestaSubScreenMainLowerView::updatePlayMessage()
{
  switch( m_ePlayMessageState )
  {
    case PLAY_MESSAGE_STATE_START:
    {
      // ストリームメッセージ流れる設定
      m_TelopProcessor.SetScrollSpeed( STREAM_MESSAGE_PLAY_PX, STREAM_MESSAGE_PLAY_FRAME );
      m_TelopProcessor.SetRectWidth( 640.0f );

      // 帯のBGせってい
      if( m_pStremMessageBgPane->IsVisible() == false )
      {
        m_pStremMessageBgPane->SetVisible(true);
        m_MessageBgPosition.y += m_pStremMessageBgPane->GetSize().height;
        m_pStremMessageBgPane->SetTranslate( m_MessageBgPosition );
        m_bgInFrame = 90;

        m_ePlayMessageState = PLAY_MESSAGE_STATE_BG_IN;
      }
      else
      {
        // BG出しっぱなし
        m_ePlayMessageState = PLAY_MESSAGE_STATE_PLAYING;
      }
    }
    break;


    case PLAY_MESSAGE_STATE_BG_IN:
    {
      f32 offsetY = gfl2::math::SinRad( GFL_MATH_DEG_TO_RAD(m_bgInFrame++) ) * 10.0f;
      m_MessageBgPosition.y -= offsetY;

      // 元の位置は106
      if( m_MessageBgPosition.y < 106.0f )
      {
        m_MessageBgPosition.y = 106.0f;
        m_ePlayMessageState = PLAY_MESSAGE_STATE_PLAYING;

      }
      m_pStremMessageBgPane->SetTranslate( m_MessageBgPosition );
    }
    break;

    case PLAY_MESSAGE_STATE_PLAYING:
    {
      // メッセージ表示
      m_pStreamMessagePane->SetVisible( true );

      // ストリームメッセージ更新
      m_pStreamMessagePane->SetTagProcessor( &m_TelopProcessor );
      m_pStreamMessagePane->SetDirtyFlag( true );
      m_TelopProcessor.Update();

#if defined( GF_PLATFORM_CTR )
      if( m_TelopProcessor.IsScrollFinish() )
      {
        m_pStreamMessagePane->SetVisible( false );

        NetAppLib::JoinFesta::JoinFestaPacketManager* pJoinFestaPacketManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPacketManager );
        NetAppLib::JoinFesta::JoinFestaInformationMessage* pJoinFestaInformationMessage = pJoinFestaPacketManager->GetJoinFestaInformationMessage();

        // メッセージ存在チェック
        if( pJoinFestaInformationMessage->IsMessageExist() )
        { // ある場合はBG出したまま
          m_ePlayMessageState = PLAY_MESSAGE_STATE_NONE;
        }
        else
        { // 無い場合はBG消す
          m_ePlayMessageState = PLAY_MESSAGE_STATE_BG_OUT;
          m_bgInFrame = 90;
        }
      }
#endif
    }
    break;

    case PLAY_MESSAGE_STATE_BG_OUT:
    {
      f32 offsetY = gfl2::math::SinRad( GFL_MATH_DEG_TO_RAD(m_bgInFrame++) ) * 10.0f;
      m_MessageBgPosition.y += offsetY;
      m_pStremMessageBgPane->SetTranslate( m_MessageBgPosition );

      // 消える位置は135
      if( m_MessageBgPosition.y > 135.0f )
      {
        m_pStremMessageBgPane->SetVisible(false);
        m_ePlayMessageState = PLAY_MESSAGE_STATE_NONE;
      }
    }
    break;
  }
}





GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)
