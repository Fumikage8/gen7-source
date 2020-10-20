// ============================================================================
/*
 * @file JoinFestaSubScreenInvitationLowerView.cpp
 * @brief ジョインフェスタ下画面のお誘い画面を表示するクラスです。
 * @date 2015.10.29
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenInvitationLowerView.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenResourceID.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenSoundDefine.h"

#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h" // NijiNetworkSystem

#include <arc_index/message.gaix>
#include <niji_conv_header/app/join_festa_subscreen/join_festa_subscreen.h>
#include <niji_conv_header/app/join_festa_subscreen/join_festa_subscreen_anim_list.h>
#include <niji_conv_header/app/join_festa_subscreen/join_festa_subscreen_pane.h>
#include <niji_conv_header/message/msg_jf_menu.h>

// Pane取得ヘルパー
#include  "AppLib/include/Util/app_util_GetPaneHelper.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(View)


JoinFestaSubScreenInvitationLowerView::JoinFestaSubScreenInvitationLowerView( NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork ) :
  NetAppLib::System::NetApplicationViewBase( 
    pJoinFestaSubScreenWork,
    JOINFESTASUBSCREEN_RESOURCE_ID_LAYOUT,
    LYTRES_JOIN_FESTA_SUBSCREEN_JFSUB_JIN_LOW_000_BFLYT,
    LA_JOIN_FESTA_SUBSCREEN_JFSUB_JIN_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pJoinFestaSubScreenWork->GetMessageUtility(),
    GARC_message_jf_menu_DAT ),
  m_pJoinFestaSubScreenWork( pJoinFestaSubScreenWork ),
  m_pListener( NULL ),
  m_autoCloseTimeout()
{
  setupTextMessage();
  setupButton();

  SetInputListener( this );
  SetInputEnabled( false ); // 初期値は入力不許可
}


JoinFestaSubScreenInvitationLowerView::~JoinFestaSubScreenInvitationLowerView()
{
}


void JoinFestaSubScreenInvitationLowerView::setupTextMessage()
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTASUBSCREEN );
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    // もうしこむ
    {
      gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_JIN_LOW_000_PANE_BUTTON_00 );
      gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( pButton, PANENAME_FESTA_BTN_LOW_000_PANE_TEXTBOX_00, &resourceID );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, jf_menu_sel_06 );
    }
    // おことわりリストへ
    {
      gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_JIN_LOW_000_PANE_BUTTON_A );
      gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( pButton, PANENAME_COMMON_BTN_LOW_017_PANE_TEXTBOX_00, &resourceID );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, jf_menu_sel_07 );
    }
    // 残り時間
    SetTimeLimit(99);
    pG2DUtil->SetTextBoxPaneString( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_JIN_LOW_000_PANE_TEXTBOX_00, jf_menu_cap_01 );
  }
}


void JoinFestaSubScreenInvitationLowerView::setupButton()
{
  app::util::G2DUtil* pG2DUtil = GetG2DUtil();
  gfl2::lyt::LytMultiResID& resourceID = pG2DUtil->GetLayoutResourceID(LAYOUT_RESOURCE_ID_JOINFESTASUBSCREEN);
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork(LAYOUT_WORK_ID_LOWER);

  // ボタン情報
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {// 申し込みOKボタン
      BUTTON_ID_OK, NULL, NULL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_JIN_LOW_000__BUTTON_00_TOUCH,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_JIN_LOW_000__BUTTON_00_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_JIN_LOW_000__BUTTON_00_CANSEL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_JIN_LOW_000__BUTTON_00_TOUCH_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_JIN_LOW_000__BUTTON_00_ACTIVE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_JIN_LOW_000__BUTTON_00_PASSIVE,
    },
    {// お断りリストボタン
      BUTTON_ID_BLACKLIST, NULL, NULL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_JIN_LOW_000__BUTTON_A_TOUCH,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_JIN_LOW_000__BUTTON_A_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_JIN_LOW_000__BUTTON_A_CANSEL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_JIN_LOW_000__BUTTON_A_TOUCH_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_JIN_LOW_000__BUTTON_A_ACTIVE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_JIN_LOW_000__BUTTON_A_PASSIVE,
    },
    {// 戻るボタン
      BUTTON_ID_BACK, NULL, NULL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_JIN_LOW_000__BACKBUTTON_TOUCH,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_JIN_LOW_000__BACKBUTTON_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_JIN_LOW_000__BACKBUTTON_CANSEL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_JIN_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_JIN_LOW_000__BACKBUTTON_ACTIVE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_JIN_LOW_000__BACKBUTTON_PASSIVE,
    },
  };

  // ピクチャペインとバウンディングボックスは後から入れる
  {// 申し込みOKボタン
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_JIN_LOW_000_PANE_BUTTON_00 );
    buttonInfoTable[ BUTTON_ID_OK ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_OK ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_FESTA_BTN_LOW_000_PANE_BOUND_00, &resourceID );
  }
  {// お断りリストボタン
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_JIN_LOW_000_PANE_BUTTON_A );
    buttonInfoTable[ BUTTON_ID_BLACKLIST ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_BLACKLIST ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_COMMON_BTN_LOW_017_PANE_BOUND_00, &resourceID );
  }
  {// 戻るボタン
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_JIN_LOW_000_PANE_BACKBUTTON );
    buttonInfoTable[ BUTTON_ID_BACK ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_BACK ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &resourceID );
  }

  // ボタン生成
  CreateButtonManager( m_pJoinFestaSubScreenWork->GetAppHeap(), pLower, buttonInfoTable, BUTTON_MAX );
  GetButtonManager()->SetInputEnable( false );


  app::tool::ButtonManager * pButtonManager = GetButtonManager();
  // SE設定
  pButtonManager->SetButtonSelectSE( BUTTON_ID_OK,          SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BLACKLIST,   SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BACK,        SE_RETURN );
  // ボタンキーアサイン
  pButtonManager->SetButtonBindKey( BUTTON_ID_OK, gfl2::ui::BUTTON_Y );
  pButtonManager->SetButtonBindKey( BUTTON_ID_BACK, gfl2::ui::BUTTON_X );
}


void JoinFestaSubScreenInvitationLowerView::Update( void )
{
  u32 elapsedTime; // 経過時間(ms)
  // 自動で閉じる制限時間更新
  bool isTimeout = m_autoCloseTimeout.IsTimeOut( &elapsedTime );
  if( isTimeout == false )
  {
    // 時間表示の更新
    // 残り時間にする為に制限時間を経過時間で引く
    SetTimeLimit( AUTO_CLOSE_TIME - elapsedTime );
  }

  app::ui::UIView::Update();
}


void JoinFestaSubScreenInvitationLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
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
::app::ui::UIInputListener::ListenerResult JoinFestaSubScreenInvitationLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ICHI_PRINT("JoinFestaSubScreenInvitationLowerView::OnLayoutPaneEvent %d \n", painId );
  if( m_pListener == NULL )
  {
    return ENABLE_ACCESS;
  }

  switch( painId ){
  case BUTTON_ID_OK:          m_pListener->OnOkButtonAction(); break;
  case BUTTON_ID_BLACKLIST:   m_pListener->OnBlackListButtonAction(); break;
  case BUTTON_ID_BACK:        m_pListener->OnNgButtonAction(); break;
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
::app::ui::UIInputListener::ListenerResult JoinFestaSubScreenInvitationLowerView::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  return ENABLE_ACCESS;
}

//------------------------------------------------------------------
/**
 * @brief 一時停止
 */
//------------------------------------------------------------------
void JoinFestaSubScreenInvitationLowerView::Suspend()
{
  // マスク表示
  GetG2DUtil()->SetPaneVisible( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_JIN_LOW_000_PANE_BG_BLACK, true );
  // 入力無効
  SetInputEnabled( false );
  if( GetButtonManager() != NULL) { GetButtonManager()->SetInputEnable( false ); }
}

//------------------------------------------------------------------
/**
 * @brief 再起動
 */
//------------------------------------------------------------------
void JoinFestaSubScreenInvitationLowerView::Resume()
{
  // マスク非表示
  GetG2DUtil()->SetPaneVisible( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_JIN_LOW_000_PANE_BG_BLACK, false );
  // 入力有効
  SetInputEnabled( true );
  if( GetButtonManager() != NULL) { GetButtonManager()->SetInputEnable( true ); }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief  お誘いメッセージ表示
 *
 * @param  packetEventType    受信パケットイベント通知種類 
 * @param  myStatus          相手のMyStatus（作って渡す）
 */
//--------------------------------------------------------------------------------------------
void JoinFestaSubScreenInvitationLowerView::SetInvitationMessage( Define::PacketEventType packetEventType, const Savedata::MyStatus& myStatus )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTASUBSCREEN );
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  // メッセージID
  u32 msgId = jf_menu_win_03;
  {
    switch( packetEventType ){
    case Define::PACKET_EVENT_TYPE_BATTLE_S: //! バトル（シングル）招待
      msgId = jf_menu_win_03;
      break;
    case Define::PACKET_EVENT_TYPE_BATTLE_D: //! バトル（ダブル）招待
      msgId = jf_menu_win_04;
      break;
    case Define::PACKET_EVENT_TYPE_BATTLE_M: //! バトル（マルチ）招待
      msgId = jf_menu_win_05;
      break;
    case Define::PACKET_EVENT_TYPE_BATTLE_R: //! バトル（ロイヤル）招待
      msgId = jf_menu_win_06;
      break;
    case Define::PACKET_EVENT_TYPE_TRADE:    //! 交換招待
      msgId = jf_menu_win_40;
      break;
    }
  }

  // プレイヤー名
  {
    gfl2::lyt::LytParts* pParts = pLower->GetPartsPane( PANENAME_JFSUB_JIN_LOW_000_PANE_MESSAGE );
    gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( pParts, PANENAME_COMMON_MSG_LOW_005_PANE_TEXTBOX_00, &resourceID );

    // 名前を埋め込む
    gfl2::str::StrBuf* srcBuf = pG2DUtil->GetTempStrBuf(0);
    gfl2::str::StrBuf* dstBuf = pG2DUtil->GetTempStrBuf(1);
    print::WordSet*    wordset= pG2DUtil->GetWordSet();
    pG2DUtil->GetString( srcBuf, msgId );
    wordset->RegisterPlayerName( 0, &myStatus );
    wordset->Expand( dstBuf, srcBuf );

    pG2DUtil->SetTextBoxPaneString( pTextBox, dstBuf );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief  タイムアウト計測開始
 */
//--------------------------------------------------------------------------------------------
void JoinFestaSubScreenInvitationLowerView::TimeOutStart()
{
  m_autoCloseTimeout.TimeOutStart( AUTO_CLOSE_TIME );
  SetTimeLimit( AUTO_CLOSE_TIME );
}
//--------------------------------------------------------------------------------------------
/**
 * @brief  タイムアウトチェック
 * @return trueでタイムアウト
 */
//--------------------------------------------------------------------------------------------
bool JoinFestaSubScreenInvitationLowerView::IsTimeOut()
{
  return m_autoCloseTimeout.IsTimeOut();
}
//--------------------------------------------------------------------------------------------
/**
 * @brief  計測一時停止
 */
//--------------------------------------------------------------------------------------------
void JoinFestaSubScreenInvitationLowerView::TimeOutSuspend()
{
  m_autoCloseTimeout.Suspend();
}
//--------------------------------------------------------------------------------------------
/**
 * @brief  計測再開
 */
//--------------------------------------------------------------------------------------------
void JoinFestaSubScreenInvitationLowerView::TimeOutResume()
{
  m_autoCloseTimeout.Resume();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief  残り時間の設定
 *
 * @param  value      残り時（ms）
 */
//--------------------------------------------------------------------------------------------
void JoinFestaSubScreenInvitationLowerView::SetTimeLimit( u32 value )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  // msから秒にする
  pG2DUtil->SetTextBoxPaneNumber( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_JIN_LOW_000_PANE_TEXTBOX_01, jf_menu_cap_02, value/1000, 2, 0, print::NUM_DISP_SPACE );
}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)
