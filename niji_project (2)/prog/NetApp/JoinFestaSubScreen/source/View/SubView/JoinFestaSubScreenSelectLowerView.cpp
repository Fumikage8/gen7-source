// ============================================================================
/*
 * @file JoinFestaSubScreenSelectLowerView.cpp
 * @brief ジョインフェスタ下画面の選択画面の入力イベントを受け取るリスナーです。
 * @date 2015.09.01
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenSelectLowerView.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenResourceID.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenSoundDefine.h"

#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

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


JoinFestaSubScreenSelectLowerView::JoinFestaSubScreenSelectLowerView( NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork ) :
  NetAppLib::System::NetApplicationViewBase( 
    pJoinFestaSubScreenWork,
    JOINFESTASUBSCREEN_RESOURCE_ID_LAYOUT,
    LYTRES_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000_BFLYT,
    LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pJoinFestaSubScreenWork->GetMessageUtility(),
    GARC_message_jf_menu_DAT ),
  m_pJoinFestaSubScreenWork( pJoinFestaSubScreenWork ),
  m_pListener( NULL ),
  m_MessageID( 0xffffffff ),
  m_MessageWindowManipulator()
{
  setupMessageWindowManipulator();
  SetInputListener( this );
  SetInputEnabled( false ); // 初期値は入力不許可
}


JoinFestaSubScreenSelectLowerView::~JoinFestaSubScreenSelectLowerView()
{
}


void JoinFestaSubScreenSelectLowerView::setupMessageWindowManipulator( void )
{
  NetAppLib::UI::NetAppCommonMessageWindowManipulator::SSetupInfo setupInfo;

  setupInfo.pHeap                = m_pJoinFestaSubScreenWork->GetAppHeap();
  setupInfo.pTargetView          = this;
  setupInfo.layoutWorkID         = LAYOUT_WORK_ID_LOWER;
  setupInfo.pHandler             = this;
  setupInfo.animID_TimerIcon     = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__MESSAGEA_00__TIMERICON_KEEP;
  setupInfo.animID_TimerIcon_In  = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__MESSAGEA_00__TIMERICON_IN;
  setupInfo.animID_TimerIcon_Out = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__MESSAGEA_00__TIMERICON_OUT;
  setupInfo.animID_MsgCursor     = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__MESSAGEA_00__CURSOR_00_KEEP;
  setupInfo.paneID_WindowParts   = PANENAME_JFSUB_SLC_LOW_000_PANE_MESSAGEA_00;
  setupInfo.paneID_TextBox2      = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2;
  setupInfo.paneID_NullText1     = PANENAME_COMMON_MSG_LOW_000_PANE_NULL_TEXT1;
  setupInfo.paneID_TextBox1a     = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1A;
  setupInfo.paneID_TextBox2a     = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1B;
  setupInfo.paneID_TimerIcon     = PANENAME_COMMON_MSG_LOW_000_PANE_TIMERICON;
  setupInfo.paneID_MsgCursor     = PANENAME_COMMON_MSG_LOW_000_PANE_CURSOR_00;

  m_MessageWindowManipulator.Setup( setupInfo );

  // @fix GFNMCat[3838]：メッセージカーソルが初期表示状態なので、非表示にする
  this->GetG2DUtil()->SetMsgCursorUserVisible( false, false );
}


void JoinFestaSubScreenSelectLowerView::SetVisibleSelectButton( bool bVisible )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONA_00 );
  pButton->SetVisible( bVisible );

  // 戻るボタンも連動
  gfl2::lyt::LytParts* pBuckButton = pLower->GetPartsPane( PANENAME_JFSUB_SLC_LOW_000_PANE_BACKBUTTON );
  pBuckButton->SetVisible( bVisible );
}


void JoinFestaSubScreenSelectLowerView::SetStreamMessage( u32 messageID, bool isShowTimerIcon )
{
  m_MessageWindowManipulator.SetFinishMode_None();
  showMessage( messageID, false, isShowTimerIcon );
}


void JoinFestaSubScreenSelectLowerView::showMessage( u32 messageID, bool bImmediate, bool isShowTimerIcon )
{
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaSubScreenWork->GetMessageUtility();
  m_MessageWindowManipulator.ShowMessage( &pMessageUtility->GetString( GARC_message_jf_menu_DAT, messageID ), bImmediate, isShowTimerIcon );
  m_MessageID = messageID;
}


void JoinFestaSubScreenSelectLowerView::CommonMessageWindowManipulator_OnEvent( const NetAppLib::UI::NetAppCommonMessageWindowManipulator::EventID eEventID )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTASUBSCREEN );
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  app::util::GetPaneHelper helper( pLower, &resourceID );

  switch( eEventID )
  {
    case NetAppLib::UI::NetAppCommonMessageWindowManipulator::EVENT_OnEOM:
    {
      switch( m_MessageID )
      {
        case jf_menu_win_11: // お断りリストに登録すると・・・追加しますか？
        {
          SetVisibleSelectButton( true );

          // ついか　する
          pG2DUtil->SetTextBoxPaneStringExpand( helper.Clear().Push( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONA_00 ).GetTextBoxPane( PANENAME_FESTA_BTN_LOW_000_PANE_TEXTBOX_00 ), jf_menu_sel_21 );
        }
        break;

        case jf_menu_win_12: // お断りリストを更新しました
          {
            if( m_pListener != NULL )
            {
              m_pListener->OnBlackListUpdateMessageFinish();
            }
          }
          break;

        case jf_menu_win_26: // 通信が中断されました
          {
            if( m_pListener != NULL )
            {
              m_pListener->OnNetworkErrorMessageFinish();
            }
          }
          break;
      }
    }
    break;
  }
}


void JoinFestaSubScreenSelectLowerView::Update( void )
{
  app::ui::UIView::Update();

  if( IsVisible() )
  {
    m_MessageWindowManipulator.UpdateWindow();
  }
}


void JoinFestaSubScreenSelectLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pJoinFestaSubScreenWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
}


::app::ui::UIInputListener::ListenerResult JoinFestaSubScreenSelectLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ICHI_PRINT("JoinFestaSubScreenSelectLowerView::OnLayoutPaneEvent %d \n", painId );
  if( m_pListener == NULL )
  {
    return ENABLE_ACCESS;
  }

  switch( painId ){
  case BUTTON_ID_1BUTTON:     m_pListener->OnButtonAction( BUTTON_ID_1BUTTON ); break;
  case BUTTON_ID_3BUTTON_01:  m_pListener->OnButtonAction( BUTTON_ID_3BUTTON_01 ); break;
  case BUTTON_ID_3BUTTON_02:  m_pListener->OnButtonAction( BUTTON_ID_3BUTTON_02 ); break;
  case BUTTON_ID_3BUTTON_03:  m_pListener->OnButtonAction( BUTTON_ID_3BUTTON_03 ); break;
  case BUTTON_ID_4BUTTON_01:  m_pListener->OnButtonAction( BUTTON_ID_4BUTTON_01 ); break;
  case BUTTON_ID_4BUTTON_02:  m_pListener->OnButtonAction( BUTTON_ID_4BUTTON_02 ); break;
  case BUTTON_ID_4BUTTON_03:  m_pListener->OnButtonAction( BUTTON_ID_4BUTTON_03 ); break;
  case BUTTON_ID_4BUTTON_04:  m_pListener->OnButtonAction( BUTTON_ID_4BUTTON_04 ); break;
  case BUTTON_ID_RETURN:      m_pListener->OnBackButtonAction(); break;
  }

  return DISABLE_ACCESS;
}

//------------------------------------------------------------------
/**
 * @brief 一時停止
 */
//------------------------------------------------------------------
void JoinFestaSubScreenSelectLowerView::Suspend()
{
  // マスク表示
  GetG2DUtil()->SetPaneVisible( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_SLC_LOW_000_PANE_BG_BLACK, true );
  // 入力無効
  SetInputEnabled( false );
  if( GetButtonManager() != NULL) { GetButtonManager()->SetInputEnable( false ); }
  m_MessageWindowManipulator.SetSuspendKey( true );
}

//------------------------------------------------------------------
/**
 * @brief 再起動
 */
//------------------------------------------------------------------
void JoinFestaSubScreenSelectLowerView::Resume()
{
  // マスク非表示
  GetG2DUtil()->SetPaneVisible( LAYOUT_WORK_ID_LOWER, PANENAME_JFSUB_SLC_LOW_000_PANE_BG_BLACK, false );
  // 入力有効
  SetInputEnabled( true );
  if( GetButtonManager() != NULL) { GetButtonManager()->SetInputEnable( true ); }
  m_MessageWindowManipulator.SetSuspendKey( false );
}

//--------------------------------------------------------------------------
/**
 * @breif  ボタン構築
 * @param  mode  ButtonTextMode
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenSelectLowerView::SetupButton( const ButtonTextMode mode )
{
  app::util::G2DUtil* pG2DUtil = GetG2DUtil();
  gfl2::lyt::LytMultiResID& resourceID = pG2DUtil->GetLayoutResourceID(LAYOUT_RESOURCE_ID_JOINFESTASUBSCREEN);
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork(LAYOUT_WORK_ID_LOWER);

  // 親ペイン一旦全て非表示
  pG2DUtil->SetPaneVisible( 0, PANENAME_JFSUB_SLC_LOW_000_PANE_TIPE_A, false );
  pG2DUtil->SetPaneVisible( 0, PANENAME_JFSUB_SLC_LOW_000_PANE_TIPE_B, false );
  pG2DUtil->SetPaneVisible( 0, PANENAME_JFSUB_SLC_LOW_000_PANE_TIPE_C, false );

  // ボタン作成
  {
    app::ui::ButtonInfoEx buttonInfoTable[ BUTTON_ID_MAX ];
    u8 buttonTableId = 0;

    switch( mode ){
    case BUTTON_TEXT_MODE_ASK_END:     //! 「終了する」のボタン
    case BUTTON_TEXT_MODE_ADD:         //! 「追加する」のボタン
    case BUTTON_TEXT_MODE_ATTRACTION_END: //! アトラクション中専用「終了する」のボタン
      {
        {
          gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONA_00 );
          buttonInfoTable[ buttonTableId ].button_id              = BUTTON_ID_1BUTTON;
          buttonInfoTable[ buttonTableId ].picture_pane           = pButton;
          buttonInfoTable[ buttonTableId ].bound_pane             = pLower->GetBoundingPane( pButton, PANENAME_FESTA_BTN_LOW_000_PANE_BOUND_00, &resourceID );
          buttonInfoTable[ buttonTableId ].touch_anime_index      = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONA_00_TOUCH;
          buttonInfoTable[ buttonTableId ].release_anime_index    = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONA_00_RELEASE;
          buttonInfoTable[ buttonTableId ].cancel_anime_index     = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONA_00_CANSEL;
          buttonInfoTable[ buttonTableId ].key_select_anime_index = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONA_00_TOUCH_RELEASE;
          buttonInfoTable[ buttonTableId ].active_anime_index     = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONA_00_ACTIVE;
          buttonInfoTable[ buttonTableId ].passive_anime_index    = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONA_00_PASSIVE;
          buttonTableId++;
          // @fix GFNMCat[3663]：戻りボタンが非表示で終わるので作成時に表示にする
          pButton->SetVisible( true );
        }

        // 親ペイン表示
        pG2DUtil->SetPaneVisible( 0, PANENAME_JFSUB_SLC_LOW_000_PANE_TIPE_A, true );
      }
      break;

    case BUTTON_TEXT_MODE_TRADE:       //! 「通信交換/GTS/ミラクル交換」のボタン
    case BUTTON_TEXT_MODE_BATTLE:      //! 「通信対戦/バトルスポット/ルールDL」のボタン
    case BUTTON_TEXT_MODE_BOSYUU:      //! 「相手を選んで申し込む/募集誰でも/募集フレンド」のボタン
      {
        {
          gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONB_00 );
          buttonInfoTable[ buttonTableId ].button_id              = BUTTON_ID_3BUTTON_01;
          buttonInfoTable[ buttonTableId ].picture_pane           = pButton;
          buttonInfoTable[ buttonTableId ].bound_pane             = pLower->GetBoundingPane( pButton, PANENAME_FESTA_BTN_LOW_000_PANE_BOUND_00, &resourceID );
          buttonInfoTable[ buttonTableId ].touch_anime_index      = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONB_00_TOUCH;
          buttonInfoTable[ buttonTableId ].release_anime_index    = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONB_00_RELEASE;
          buttonInfoTable[ buttonTableId ].cancel_anime_index     = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONB_00_CANSEL;
          buttonInfoTable[ buttonTableId ].key_select_anime_index = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONB_00_TOUCH_RELEASE;
          buttonInfoTable[ buttonTableId ].active_anime_index     = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONB_00_ACTIVE;
          buttonInfoTable[ buttonTableId ].passive_anime_index    = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONB_00_PASSIVE;
          buttonTableId++;
        }
        {
          gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONB_01 );
          buttonInfoTable[ buttonTableId ].button_id              = BUTTON_ID_3BUTTON_02;
          buttonInfoTable[ buttonTableId ].picture_pane           = pButton;
          buttonInfoTable[ buttonTableId ].bound_pane             = pLower->GetBoundingPane( pButton, PANENAME_FESTA_BTN_LOW_000_PANE_BOUND_00, &resourceID );
          buttonInfoTable[ buttonTableId ].touch_anime_index      = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONB_01_TOUCH;
          buttonInfoTable[ buttonTableId ].release_anime_index    = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONB_01_RELEASE;
          buttonInfoTable[ buttonTableId ].cancel_anime_index     = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONB_01_CANSEL;
          buttonInfoTable[ buttonTableId ].key_select_anime_index = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONB_01_TOUCH_RELEASE;
          buttonInfoTable[ buttonTableId ].active_anime_index     = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONB_01_ACTIVE;
          buttonInfoTable[ buttonTableId ].passive_anime_index    = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONB_01_PASSIVE;
          buttonTableId++;
        }
        {
          gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONB_02 );
          buttonInfoTable[ buttonTableId ].button_id              = BUTTON_ID_3BUTTON_03;
          buttonInfoTable[ buttonTableId ].picture_pane           = pButton;
          buttonInfoTable[ buttonTableId ].bound_pane             = pLower->GetBoundingPane( pButton, PANENAME_FESTA_BTN_LOW_000_PANE_BOUND_00, &resourceID );
          buttonInfoTable[ buttonTableId ].touch_anime_index      = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONB_02_TOUCH;
          buttonInfoTable[ buttonTableId ].release_anime_index    = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONB_02_RELEASE;
          buttonInfoTable[ buttonTableId ].cancel_anime_index     = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONB_02_CANSEL;
          buttonInfoTable[ buttonTableId ].key_select_anime_index = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONB_02_TOUCH_RELEASE;
          buttonInfoTable[ buttonTableId ].active_anime_index     = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONB_02_ACTIVE;
          buttonInfoTable[ buttonTableId ].passive_anime_index    = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONB_02_PASSIVE;
          buttonTableId++;
        }

        // 親ペイン表示
        pG2DUtil->SetPaneVisible( 0, PANENAME_JFSUB_SLC_LOW_000_PANE_TIPE_B, true );
      }
      break;
    case BUTTON_TEXT_MODE_RULE:      //! 「シングル/ダブル/マルチ/ロイヤル」のボタン
      {
        { // ボタン1
          gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONC_00 );
          buttonInfoTable[ buttonTableId ].button_id              = BUTTON_ID_4BUTTON_01;
          buttonInfoTable[ buttonTableId ].picture_pane           = pButton;
          buttonInfoTable[ buttonTableId ].bound_pane             = pLower->GetBoundingPane( pButton, PANENAME_FESTA_BTN_LOW_000_PANE_BOUND_00, &resourceID );
          buttonInfoTable[ buttonTableId ].touch_anime_index      = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_00_TOUCH;
          buttonInfoTable[ buttonTableId ].release_anime_index    = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_00_RELEASE;
          buttonInfoTable[ buttonTableId ].cancel_anime_index     = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_00_CANSEL;
          buttonInfoTable[ buttonTableId ].key_select_anime_index = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_00_TOUCH_RELEASE;
          buttonInfoTable[ buttonTableId ].active_anime_index     = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_00_ACTIVE;
          buttonInfoTable[ buttonTableId ].passive_anime_index    = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_00_PASSIVE;
          buttonTableId++;
        }
        { // ボタン2
          gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONC_01 );
          buttonInfoTable[ buttonTableId ].button_id              = BUTTON_ID_4BUTTON_02;
          buttonInfoTable[ buttonTableId ].picture_pane           = pButton;
          buttonInfoTable[ buttonTableId ].bound_pane             = pLower->GetBoundingPane( pButton, PANENAME_FESTA_BTN_LOW_000_PANE_BOUND_00, &resourceID );
          buttonInfoTable[ buttonTableId ].touch_anime_index      = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_01_TOUCH;
          buttonInfoTable[ buttonTableId ].release_anime_index    = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_01_RELEASE;
          buttonInfoTable[ buttonTableId ].cancel_anime_index     = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_01_CANSEL;
          buttonInfoTable[ buttonTableId ].key_select_anime_index = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_01_TOUCH_RELEASE;
          buttonInfoTable[ buttonTableId ].active_anime_index     = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_01_ACTIVE;
          buttonInfoTable[ buttonTableId ].passive_anime_index    = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_01_PASSIVE;
          buttonTableId++;
        }
        { // ボタン3
          gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONC_02 );
          buttonInfoTable[ buttonTableId ].button_id              = BUTTON_ID_4BUTTON_03;
          buttonInfoTable[ buttonTableId ].picture_pane           = pButton;
          buttonInfoTable[ buttonTableId ].bound_pane             = pLower->GetBoundingPane( pButton, PANENAME_FESTA_BTN_LOW_000_PANE_BOUND_00, &resourceID );
          buttonInfoTable[ buttonTableId ].touch_anime_index      = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_02_TOUCH;
          buttonInfoTable[ buttonTableId ].release_anime_index    = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_02_RELEASE;
          buttonInfoTable[ buttonTableId ].cancel_anime_index     = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_02_CANSEL;
          buttonInfoTable[ buttonTableId ].key_select_anime_index = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_02_TOUCH_RELEASE;
          buttonInfoTable[ buttonTableId ].active_anime_index     = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_02_ACTIVE;
          buttonInfoTable[ buttonTableId ].passive_anime_index    = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_02_PASSIVE;
          buttonTableId++;
        }
        { // ボタン4
          gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONC_03 );
          buttonInfoTable[ buttonTableId ].button_id              = BUTTON_ID_4BUTTON_04;
          buttonInfoTable[ buttonTableId ].picture_pane           = pButton;
          buttonInfoTable[ buttonTableId ].bound_pane             = pLower->GetBoundingPane( pButton, PANENAME_FESTA_BTN_LOW_000_PANE_BOUND_00, &resourceID );
          buttonInfoTable[ buttonTableId ].touch_anime_index      = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_03_TOUCH;
          buttonInfoTable[ buttonTableId ].release_anime_index    = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_03_RELEASE;
          buttonInfoTable[ buttonTableId ].cancel_anime_index     = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_03_CANSEL;
          buttonInfoTable[ buttonTableId ].key_select_anime_index = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_03_TOUCH_RELEASE;
          buttonInfoTable[ buttonTableId ].active_anime_index     = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_03_ACTIVE;
          buttonInfoTable[ buttonTableId ].passive_anime_index    = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BUTTONC_03_PASSIVE;
          buttonTableId++;
        }

        // 親ペイン表示
        pG2DUtil->SetPaneVisible( 0, PANENAME_JFSUB_SLC_LOW_000_PANE_TIPE_C, true );
      }
      break;
    case BUTTON_TEXT_MODE_RETURN:
      // 何も作らない
      break;
    }

    // 共通の戻るボタン
    {
      gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_SLC_LOW_000_PANE_BACKBUTTON );
      buttonInfoTable[ buttonTableId ].button_id              = BUTTON_ID_RETURN;
      buttonInfoTable[ buttonTableId ].picture_pane           = pButton;
      buttonInfoTable[ buttonTableId ].bound_pane             = pLower->GetBoundingPane( pButton, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &resourceID );
      buttonInfoTable[ buttonTableId ].touch_anime_index      = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BACKBUTTON_TOUCH;
      buttonInfoTable[ buttonTableId ].release_anime_index    = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BACKBUTTON_RELEASE;
      buttonInfoTable[ buttonTableId ].cancel_anime_index     = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BACKBUTTON_CANSEL;
      buttonInfoTable[ buttonTableId ].key_select_anime_index = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BACKBUTTON_TOUCH_RELEASE;
      buttonInfoTable[ buttonTableId ].active_anime_index     = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BACKBUTTON_ACTIVE;
      buttonInfoTable[ buttonTableId ].passive_anime_index    = LA_JOIN_FESTA_SUBSCREEN_JFSUB_SLC_LOW_000__BACKBUTTON_PASSIVE;
      buttonTableId++;
      // @fix GFNMCat[3663]：戻りボタンが非表示で終わるので作成時に表示にする
      pButton->SetVisible( true );
    }

    // ボタン生成
    ReloadButtonManager( m_pJoinFestaSubScreenWork->GetAppHeap(), pLower, buttonInfoTable, buttonTableId );
    GetButtonManager()->SetInputEnable( false );
  }

  // SE設定
  {
    app::tool::ButtonManager * pButtonManager = GetButtonManager();
    switch( mode ){
    case BUTTON_TEXT_MODE_ASK_END:     //! 「終了する」のボタン
    case BUTTON_TEXT_MODE_ADD:         //! 「追加する」のボタン
    case BUTTON_TEXT_MODE_ATTRACTION_END: //! アトラクション中専用「終了する」のボタン
      pButtonManager->SetButtonSelectSE( BUTTON_ID_1BUTTON,     SE_DECIDE );
      break;
    case BUTTON_TEXT_MODE_TRADE:       //! 「通信交換/GTS/ミラクル交換」のボタン
    case BUTTON_TEXT_MODE_BATTLE:      //! 「通信対戦/バトルスポット/ルールDL」のボタン
    case BUTTON_TEXT_MODE_BOSYUU:      //! 「相手を選んで申し込む/募集誰でも/募集フレンド」のボタン
      pButtonManager->SetButtonSelectSE( BUTTON_ID_3BUTTON_01,  SE_DECIDE );
      pButtonManager->SetButtonSelectSE( BUTTON_ID_3BUTTON_02,  SE_DECIDE );
      pButtonManager->SetButtonSelectSE( BUTTON_ID_3BUTTON_03,  SE_DECIDE );
      break;
    case BUTTON_TEXT_MODE_RULE:      //! 「シングル/ダブル/マルチ/ロイヤル」のボタン
      pButtonManager->SetButtonSelectSE( BUTTON_ID_4BUTTON_01,  SE_DECIDE );
      pButtonManager->SetButtonSelectSE( BUTTON_ID_4BUTTON_02,  SE_DECIDE );
      pButtonManager->SetButtonSelectSE( BUTTON_ID_4BUTTON_03,  SE_DECIDE );
      pButtonManager->SetButtonSelectSE( BUTTON_ID_4BUTTON_04,  SE_DECIDE );
      break;
    case BUTTON_TEXT_MODE_RETURN:
      // 何もしない
      break;
    }
    pButtonManager->SetButtonSelectSE( BUTTON_ID_RETURN,  SE_RETURN );
  }

  // ボタンキーアサイン
  {
    app::tool::ButtonManager * pButtonManager = GetButtonManager();
    // 全モード
    pButtonManager->SetButtonBindKey( BUTTON_ID_1BUTTON, gfl2::ui::BUTTON_Y );
    pButtonManager->SetButtonBindKey( BUTTON_ID_RETURN, gfl2::ui::BUTTON_X );
  }


  // ボタンテキスト
  setButtonText( mode );

}

//--------------------------------------------------------------------------
/**
 * @breif  ボタンテキスト設定
 * @param  mode  ButtonTextMode
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenSelectLowerView::setButtonText( const ButtonTextMode mode )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTASUBSCREEN );
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  app::util::GetPaneHelper helper( pLower, &resourceID );

  switch( mode ){
  case BUTTON_TEXT_MODE_ASK_END:     //! 「終了する」のボタン
    {
      gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONA_00 );
      pButton->SetVisible( true );

      // しゅうりょうする
      pG2DUtil->SetTextBoxPaneStringExpand( helper.Clear().Push( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONA_00 ).GetTextBoxPane( PANENAME_FESTA_BTN_LOW_000_PANE_TEXTBOX_00 ), jf_menu_sel_03 );
      
      // ジョインフェスタを しゅうりょう　しますか？
      {
        showMessage( jf_menu_win_01, true, false );
      }
    }
    break;

  case BUTTON_TEXT_MODE_ADD:         //! 「追加する」のボタン
    {
      SetVisibleSelectButton( false );
    }
    break;

  case BUTTON_TEXT_MODE_ATTRACTION_END: //! アトラクション中専用「終了する」のボタン
    {
      gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONA_00 );
      pButton->SetVisible( true );

      // リタイアする
      pG2DUtil->SetTextBoxPaneStringExpand( helper.Clear().Push( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONA_00 ).GetTextBoxPane( PANENAME_FESTA_BTN_LOW_000_PANE_TEXTBOX_00 ), jf_menu_att_cap_11 );
      
      // アトラクションを　リタイアしますか？
      {
        showMessage( jf_menu_att_win_01, true, false );
      }
    }
    break;

  case BUTTON_TEXT_MODE_TRADE:       //! 「通信交換/GTS/ミラクル交換」のボタン
    {
      // つうしんこうかん
      pG2DUtil->SetTextBoxPaneStringExpand( helper.Clear().Push( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONB_00 ).GetTextBoxPane( PANENAME_FESTA_BTN_LOW_000_PANE_TEXTBOX_00 ), jf_menu_sel_08 );
      // GTS
      pG2DUtil->SetTextBoxPaneStringExpand( helper.Clear().Push( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONB_01 ).GetTextBoxPane( PANENAME_FESTA_BTN_LOW_000_PANE_TEXTBOX_00 ), jf_menu_sel_09 );
      // ミラクルこうかん
      pG2DUtil->SetTextBoxPaneStringExpand( helper.Clear().Push( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONB_02 ).GetTextBoxPane( PANENAME_FESTA_BTN_LOW_000_PANE_TEXTBOX_00 ), jf_menu_sel_10 );

      // なにをしますか
      {
        gfl2::lyt::LytTextBox* pTextBox = helper.Clear().Push( PANENAME_JFSUB_SLC_LOW_000_PANE_MESSAGEB_00 ).GetTextBoxPane( PANENAME_COMMON_MSG_LOW_003_PANE_TEXTBOX_00 );
        pTextBox->SetVisible( true );
        pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, jf_menu_win_07 );
      }
    }
    break;

  case BUTTON_TEXT_MODE_BATTLE:      //! 「通信対戦/バトルスポット/ルールDL」のボタン
    {
      // 通信対戦
      pG2DUtil->SetTextBoxPaneStringExpand( helper.Clear().Push( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONB_00 ).GetTextBoxPane( PANENAME_FESTA_BTN_LOW_000_PANE_TEXTBOX_00 ), jf_menu_sel_11 );
      // バトルスポット
      pG2DUtil->SetTextBoxPaneStringExpand( helper.Clear().Push( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONB_01 ).GetTextBoxPane( PANENAME_FESTA_BTN_LOW_000_PANE_TEXTBOX_00 ), jf_menu_sel_12 );
      // ルールDL
      pG2DUtil->SetTextBoxPaneStringExpand( helper.Clear().Push( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONB_02 ).GetTextBoxPane( PANENAME_FESTA_BTN_LOW_000_PANE_TEXTBOX_00 ), jf_menu_sel_13 );

      // なにをしますか？
      {
        gfl2::lyt::LytTextBox* pTextBox = helper.Clear().Push( PANENAME_JFSUB_SLC_LOW_000_PANE_MESSAGEB_00 ).GetTextBoxPane( PANENAME_COMMON_MSG_LOW_003_PANE_TEXTBOX_00 );
        pTextBox->SetVisible( true );
        pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, jf_menu_win_08 );
      }
    }
    break;

  case BUTTON_TEXT_MODE_RULE:      //! 「シングル/ダブル/マルチ/ロイヤル」のボタン
    {
      // シングルバトル
      pG2DUtil->SetTextBoxPaneStringExpand( helper.Clear().Push( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONC_00 ).GetTextBoxPane( PANENAME_FESTA_BTN_LOW_000_PANE_TEXTBOX_00 ), jf_menu_sel_17 );
      // ダブルバトル
      pG2DUtil->SetTextBoxPaneStringExpand( helper.Clear().Push( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONC_01 ).GetTextBoxPane( PANENAME_FESTA_BTN_LOW_000_PANE_TEXTBOX_00 ), jf_menu_sel_18 );
      // マルチバトル
      pG2DUtil->SetTextBoxPaneStringExpand( helper.Clear().Push( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONC_02 ).GetTextBoxPane( PANENAME_FESTA_BTN_LOW_000_PANE_TEXTBOX_00 ), jf_menu_sel_19 );
      // ロイヤルバトル
      pG2DUtil->SetTextBoxPaneStringExpand( helper.Clear().Push( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONC_03 ).GetTextBoxPane( PANENAME_FESTA_BTN_LOW_000_PANE_TEXTBOX_00 ), jf_menu_sel_20 );

      // どの　たいせんを　しますか？
      {
        gfl2::lyt::LytTextBox* pTextBox = helper.Clear().Push( PANENAME_JFSUB_SLC_LOW_000_PANE_MESSAGEC_00 ).GetTextBoxPane( PANENAME_COMMON_MSG_LOW_003_PANE_TEXTBOX_00 );
        pTextBox->SetVisible( true );
        pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, jf_menu_win_10 );
      }
    }
    break;

  case BUTTON_TEXT_MODE_BOSYUU:      //! 「相手を選んで申し込む/募集誰でも/募集フレンド」のボタン
    {
      // あいてを　えらんで　もうしこむ
      pG2DUtil->SetTextBoxPaneStringExpand( helper.Clear().Push( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONB_00 ).GetTextBoxPane( PANENAME_FESTA_BTN_LOW_000_PANE_TEXTBOX_00 ), jf_menu_sel_14 );
      // ぼしゅうする　（だれでもOK）
      pG2DUtil->SetTextBoxPaneStringExpand( helper.Clear().Push( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONB_01 ).GetTextBoxPane( PANENAME_FESTA_BTN_LOW_000_PANE_TEXTBOX_00 ), jf_menu_sel_15 );
      // ぼしゅうする　（フレンドのみ）
      pG2DUtil->SetTextBoxPaneStringExpand( helper.Clear().Push( PANENAME_JFSUB_SLC_LOW_000_PANE_BUTTONB_02 ).GetTextBoxPane( PANENAME_FESTA_BTN_LOW_000_PANE_TEXTBOX_00 ), jf_menu_sel_16 );

      // ほうほうを　きめてください ？
      {
        gfl2::lyt::LytTextBox* pTextBox = helper.Clear().Push( PANENAME_JFSUB_SLC_LOW_000_PANE_MESSAGEB_00 ).GetTextBoxPane( PANENAME_COMMON_MSG_LOW_003_PANE_TEXTBOX_00 );
        pTextBox->SetVisible( true );
        pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, jf_menu_win_09 );
      }
    }
    break;
  }
}



GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)
