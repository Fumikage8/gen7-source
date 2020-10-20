// ============================================================================
/*
 * @file JoinFestaSubScreenAttractionLowerView.cpp
 * @brief ジョインフェスタ下画面のお誘い画面を表示するクラスです。
 * @date 2015.10.29
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenAttractionLowerView.h"
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

// アトラクションマネージャ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(View)


JoinFestaSubScreenAttractionLowerView::JoinFestaSubScreenAttractionLowerView( NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork ) :
  NetAppLib::System::NetApplicationViewBase( 
    pJoinFestaSubScreenWork,
    JOINFESTASUBSCREEN_RESOURCE_ID_LAYOUT,
    LYTRES_JOIN_FESTA_SUBSCREEN_JFSUB_ATR_LOW_000_BFLYT,
    LA_JOIN_FESTA_SUBSCREEN_JFSUB_ATR_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pJoinFestaSubScreenWork->GetMessageUtility(),
    GARC_message_jf_menu_DAT ),
  m_pJoinFestaSubScreenWork( pJoinFestaSubScreenWork ),
  m_pListener( NULL )
{
  setupTextMessage();
  setupButton();

  SetInputListener( this );
  SetInputEnabled( false ); // 初期値は入力不許可
}


JoinFestaSubScreenAttractionLowerView::~JoinFestaSubScreenAttractionLowerView()
{
}


void JoinFestaSubScreenAttractionLowerView::setupTextMessage()
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTASUBSCREEN );
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  // キャプション
  {
    // 残り時間
    pG2DUtil->SetTextBoxPaneString( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_ATR_LOW_000_PANE_TIME_CAP_TEXT, jf_menu_att_cap_02 );
    // トータルスコア
    pG2DUtil->SetTextBoxPaneString( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_ATR_LOW_000_PANE_T_SCORE_CAP_TEXT, jf_menu_att_cap_03 );
    // 個人スコア
    pG2DUtil->SetTextBoxPaneString( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_ATR_LOW_000_PANE_P_SCORE_CAP_TEXT, jf_menu_att_cap_04 );
    // さんかにんずう
    pG2DUtil->SetTextBoxPaneString( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_ATR_LOW_000_PANE_JOIN_CAP_TEXT, jf_menu_att_cap_05 );

    // 目標
    pG2DUtil->SetTextBoxPaneString( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_ATR_LOW_000_PANE_JOIN_TARGET_00, jf_menu_att_cap_14 );
    // 目標
    pG2DUtil->SetTextBoxPaneString( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_ATR_LOW_000_PANE_JOIN_TARGET_01, jf_menu_att_cap_14 );
    
  }

  // アトラクション名
  if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->IsPlaying() )
  {
    gfl2::heap::HeapBase* pHeap = m_pJoinFestaSubScreenWork->GetDeviceHeap()->GetLowerHandle();
    gfl2::str::MsgData* pMsgData = GFL_NEW( pHeap ) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_jf_attraction_DAT, pHeap );

    NetAppLib::JoinFesta::JoinFestaAttractionDefine::CoreData coreData = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->GetPlayAttractionData(); 
    u32 attractionMsgId = coreData.attractionName;
    pG2DUtil->GetWordSet()->RegisterWord( 0, pMsgData, attractionMsgId );
    pG2DUtil->SetTextBoxPaneStringExpand( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_ATR_LOW_000_PANE_ATTRACTION_TEXT, jf_menu_att_cap_01 );

    /*
      目標
    */
    {
      const u32                   digits_Personal = 3;
      const u32                   digits_Total    = 5;
      const print::NumberDispType dispType        = print::NUM_DISP_SPACE;

      /*  個人  */
      pG2DUtil->SetRegisterNumber(0, coreData.manEvaluationStar5, digits_Personal, dispType);
      pG2DUtil->SetTextBoxPaneStringExpand( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_ATR_LOW_000_PANE_P_SCORE_VALUE_TEXT_00, jf_menu_att_cap_08 );

      /*  トータル  */
      pG2DUtil->SetRegisterNumber(0, coreData.totalEvaluationStar5, digits_Total, dispType);
      pG2DUtil->SetTextBoxPaneStringExpand( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_ATR_LOW_000_PANE_P_SCORE_VALUE_TEXT_01, jf_menu_att_cap_08 );
    }

    GFL_SAFE_DELETE( pMsgData );
  }
}


void JoinFestaSubScreenAttractionLowerView::setupButton()
{
  app::util::G2DUtil* pG2DUtil = GetG2DUtil();
  gfl2::lyt::LytMultiResID& resourceID = pG2DUtil->GetLayoutResourceID(LAYOUT_RESOURCE_ID_JOINFESTASUBSCREEN);
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork(LAYOUT_WORK_ID_LOWER);

  // ボタン情報
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {// 戻るボタン
      BUTTON_ID_STOP, NULL, NULL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_ATR_LOW_000__BACKBUTTON_TOUCH,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_ATR_LOW_000__BACKBUTTON_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_ATR_LOW_000__BACKBUTTON_CANSEL,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_ATR_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_ATR_LOW_000__BACKBUTTON_ACTIVE,
      LA_JOIN_FESTA_SUBSCREEN_JFSUB_ATR_LOW_000__BACKBUTTON_PASSIVE,
    }
  };

  // ピクチャペインとバウンディングボックスは後から入れる
  {// 戻るボタン
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_ATR_LOW_000_PANE_BACKBUTTON );
    buttonInfoTable[ BUTTON_ID_STOP ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_STOP ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &resourceID );
  }

  // ボタン生成
  CreateButtonManager( m_pJoinFestaSubScreenWork->GetAppHeap(), pLower, buttonInfoTable, BUTTON_MAX );
  GetButtonManager()->SetInputEnable( false );


  app::tool::ButtonManager * pButtonManager = GetButtonManager();
  // SE設定
  pButtonManager->SetButtonSelectSE( BUTTON_ID_STOP,        SE_RETURN );
  // ボタンキーアサイン
  pButtonManager->SetButtonBindKey( BUTTON_ID_STOP, gfl2::ui::BUTTON_X );
}


void JoinFestaSubScreenAttractionLowerView::Update( void )
{
  // @fix NMCat[1213]：アトラクション情報をView側で更新する。
  // 本来はFramdeでやることだが、Suspend時に更新されないのでここで処理する。
  // 1番良い解決方法は、Suspend時も通るUpdateをフレームワークに作るべきだが影響が大きいので断念。
  {
    NetAppLib::JoinFesta::JoinFestaAttractionManager* pAttractionManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager);
    // 残り時間
    {
#ifdef  GF_PLATFORM_CTR
      const int second = pAttractionManager->GetTimeLimitFrame() / FRAME_PER_SECOND; //秒数変換
#else
      const int second = pAttractionManager->GetTimeLimitFrame() / 30; //秒数変換
#endif
      SetTimeLimit( second/60, second%60 );
    }

    // 参加人数
    SetJoinNum( pAttractionManager->GetMemberNum() );

    // 個人スコア
    SetMyScore( pAttractionManager->GetMyScore() );

    // トータルスコア
    SetTotalScore( pAttractionManager->GetTotalScore() );
  }

  app::ui::UIView::Update();
}


void JoinFestaSubScreenAttractionLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
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
::app::ui::UIInputListener::ListenerResult JoinFestaSubScreenAttractionLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ICHI_PRINT("JoinFestaSubScreenAttractionLowerView::OnLayoutPaneEvent %d \n", painId );
  if( m_pListener == NULL )
  {
    return ENABLE_ACCESS;
  }

  switch( painId ){
  case BUTTON_ID_STOP: m_pListener->OnStopButtonAction(); break;
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
::app::ui::UIInputListener::ListenerResult JoinFestaSubScreenAttractionLowerView::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  return ENABLE_ACCESS;
}

//------------------------------------------------------------------
/**
 * @brief 一時停止
 */
//------------------------------------------------------------------
void JoinFestaSubScreenAttractionLowerView::Suspend()
{
  // マスク表示
  GetG2DUtil()->SetPaneVisible( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_ATR_LOW_000_PANE_BG_BLACK, true );
  // 入力無効
  SetInputEnabled( false );
  if( GetButtonManager() != NULL) { GetButtonManager()->SetInputEnable( false ); }
}

//------------------------------------------------------------------
/**
 * @brief 再起動
 */
//------------------------------------------------------------------
void JoinFestaSubScreenAttractionLowerView::Resume()
{
  // マスク非表示
  GetG2DUtil()->SetPaneVisible( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_ATR_LOW_000_PANE_BG_BLACK, false );
  // 入力有効
  SetInputEnabled( true );
  if( GetButtonManager() != NULL) { GetButtonManager()->SetInputEnable( true ); }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief  残り時間の設定
 */
//--------------------------------------------------------------------------------------------
void JoinFestaSubScreenAttractionLowerView::SetTimeLimit( u32 minute, u32 second )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  pG2DUtil->SetTextBoxPaneNumber( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_ATR_LOW_000_PANE_MINUTE_TEXT, jf_menu_att_cap_06, minute, 2, 0, print::NUM_DISP_ZERO );
  pG2DUtil->SetTextBoxPaneNumber( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_ATR_LOW_000_PANE_SECOND_TEXT, jf_menu_att_cap_06, second, 2, 0, print::NUM_DISP_ZERO );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief  トータルスコアの設定
 */
//--------------------------------------------------------------------------------------------
void JoinFestaSubScreenAttractionLowerView::SetTotalScore( u32 score )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  pG2DUtil->SetTextBoxPaneNumber( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_ATR_LOW_000_PANE_T_SCORE_VALUE_TEXT, jf_menu_att_cap_08, score, 5 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief  個人スコアの設定
 */
//--------------------------------------------------------------------------------------------
void JoinFestaSubScreenAttractionLowerView::SetMyScore( u32 score )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  pG2DUtil->SetTextBoxPaneNumber( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_ATR_LOW_000_PANE_P_SCORE_VALUE_TEXT, jf_menu_att_cap_10, score, 3 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief  参加人数の設定
 */
//--------------------------------------------------------------------------------------------
void JoinFestaSubScreenAttractionLowerView::SetJoinNum( u32 num )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  pG2DUtil->SetTextBoxPaneNumber( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_ATR_LOW_000_PANE_JOIN_VALUE_TEXT, jf_menu_att_cap_10, num, 3 );
}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)
