// ============================================================================
/*
 * @file NBRLowerViewTeamSelect.cpp
 * @brief 通信対戦受付アプリ 下画面：チーム選択画面
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include "NetApp/NBR/source/View/SubView/NBRLowerViewTeamSelect.h"
#include "NetApp/NBR/source/NBRResourceID.h"

// Pane取得ヘルパー
#include  "AppLib/include/Util/app_util_GetPaneHelper.h"

#include "NetStatic/NetAppLib/include/BgmSelect/NetworkBattleBgmSelect.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)

NBRLowerViewTeamSelect::NBRLowerViewTeamSelect( NetAppLib::System::ApplicationWorkBase* pWorkBase,
                                app::util::G2DUtil* pG2dUtil,
                                app::util::G2DUtil::LAYOUT_WORK* pLayoutWork ) :
  NBRLowerViewBase( pWorkBase ),
  m_pListener(NULL)
{
  // 親ビューのレイアウトを使う
  Create2D( m_pHeap, pG2dUtil->GetLayoutSystem(), 1,
            pLayoutWork, 1,
            pG2dUtil->GetLayoutResourceID(),
            pG2dUtil->GetMsgData(), pG2dUtil->GetWordSet() );

  setupButton();
  initializeText();

  SetInputListener( this );
  // 最初は非表示
  SetVisible(ALL_VIEW_OFF);
}


NBRLowerViewTeamSelect::~NBRLowerViewTeamSelect()
{
  ReleaseCursor();
  Delete2D();
}

//--------------------------------------------------------------------------
/**
* @breif  イベントリスナーの登録
* @param  pNBRLowerViewTeamSelectListener リスナーのポインタ
*/
//--------------------------------------------------------------------------
void NBRLowerViewTeamSelect::SetListener( NBRLowerViewTeamSelectListener* pNBRLowerViewTeamSelectListener )
{
  m_pListener = pNBRLowerViewTeamSelectListener;
}

//--------------------------------------------------------------------------
/**
* @breif  イベントリスナーの解除
*/
//--------------------------------------------------------------------------
void NBRLowerViewTeamSelect::RemoveListener()
{
  m_pListener = NULL;
}

//--------------------------------------------------------------------------
/**
* @breif  ビュー全体の表示・非表示
* @param  visibleMode   表示モード定義
*/
//--------------------------------------------------------------------------
void NBRLowerViewTeamSelect::SetVisible( const VisibleMode visibleMode )
{
  ICHI_PRINT("NBRLowerViewTeamSelect::SetVisible mode:%d \n", visibleMode );

  bool isRootVisible = false;         // 親ペイン
  bool isReturnButtonVisible = false; // 戻るボタン

  switch( visibleMode ){
  case ALL_VIEW_ON:            //! 全て表示
    isRootVisible = true;
    isReturnButtonVisible = true;
    break;
  case ALL_VIEW_OFF:           //! 全て非表示
    isRootVisible = false;
    isReturnButtonVisible = false;
    break;
  case RETURN_BUTTON_ONLY_ON:  //! 戻るボタンのみ表示
    isRootVisible = false;
    isReturnButtonVisible = true;
    break;
  case CURSOR_OFF:             //! カーソル非表示
    SetCursorVisible( false );
    return;
  case CURSOR_ON:              //! カーソル表示
    SetCursorVisible( true );
    return;
  case BATTLE_START_BUTTON_PASSIVE:  //! 勝負開始ボタンをパッシブ
    GetButtonManager()->SetButtonPassiveForce( BUTTON_ID_BATTLE_START, true );
    GetButtonManager()->SetButtonPassiveAnimeFrameLast( BUTTON_ID_BATTLE_START );
    GetButtonManager()->SetButtonActiveForce( BUTTON_ID_CANNOT_JOIN, true );
    GetButtonManager()->SetButtonActiveAnimeFrameLast( BUTTON_ID_CANNOT_JOIN );
    return;
  case CANNOT_JOIN_BUTTON_PASSIVE:   //! 参加出来ないボタンをパッシブ
    GetButtonManager()->SetButtonPassiveForce( BUTTON_ID_CANNOT_JOIN, true );
    GetButtonManager()->SetButtonPassiveAnimeFrameLast( BUTTON_ID_CANNOT_JOIN );
    GetButtonManager()->SetButtonActiveForce( BUTTON_ID_BATTLE_START, true );
    GetButtonManager()->SetButtonActiveAnimeFrameLast( BUTTON_ID_BATTLE_START );
    return;
  }

  // 親ペイン
  GetG2DUtil()->SetPaneVisible( 0, PANENAME_NBR_SLC_LOW_000_PANE_TEAMSELECT, isRootVisible );

  // 戻るボタン
  {
    gfl2::lyt::LytWk* pLower = GetG2DUtil()->GetLayoutWork(0);
    gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_NBR_SLC_LOW_000_PANE_BACKBUTTON );
    pBtnBack->SetVisible( isReturnButtonVisible );
  }
  // カーソル
  {
    SetCursorVisible( isRootVisible );
  }

  // ボタンは表示だけでなく、個別に有効・無効設定しないといけない
  {
    SetButtonEnable( BUTTON_ID_TEAM,        isRootVisible, false );
    SetButtonEnable( BUTTON_ID_CANNOT_JOIN, isRootVisible, false );
    SetButtonEnable( BUTTON_ID_BATTLE_START,isRootVisible, false );
    SetButtonEnable( BUTTON_ID_BGM_L,       isRootVisible, false );
    SetButtonEnable( BUTTON_ID_BGM_R,       isRootVisible, false );
    SetButtonEnable( BUTTON_ID_RETURN,      isReturnButtonVisible, false );
  }
}


//--------------------------------------------------------------------------
/**
 * @breif  BGM選択部分の表示・非表示
 * @param  isVisible         true = 表示, false = 非表示
 */
//--------------------------------------------------------------------------
void NBRLowerViewTeamSelect::SetVisibleBgmSelect( const bool isVisible )
{
  GetG2DUtil()->SetPaneVisible( 0, PANENAME_NBR_SLC_LOW_000_PANE_BGMSELECT, isVisible );
  ICHI_PRINT("NBRLowerViewTeamSelect::SetVisibleBgmSelect %d \n", isVisible );

  // @fix NMCat[1937]：LRボタン有効、無効
  SetButtonEnable( BUTTON_ID_BGM_L, isVisible, false );
  SetButtonEnable( BUTTON_ID_BGM_R, isVisible, false );
}

//--------------------------------------------------------------------------
/**
 * @breif  チーム名の設定
 * @param  teamName   チーム名の文字
 */
//--------------------------------------------------------------------------
void NBRLowerViewTeamSelect::SetTeamName( gfl2::str::StrBuf* teamName )
{
  app::util::GetPaneHelper helper( GetG2DUtil()->GetLayoutWork(0), GetG2DUtil()->GetLayoutResourceID() );

  // チーム名
  GetG2DUtil()->SetTextBoxPaneString( 
    helper.Clear().Push( PANENAME_NBR_SLC_LOW_000_PANE_BUTTON_L2 ).GetTextBoxPane( PANENAME_GTS_BTN_LOW_003_PANE_TEXTBOX_01 ), 
    teamName );

  // カーソルを勝負開始に合わせる
  CursorMoveToButton( BUTTON_ID_BATTLE_START );
}

//--------------------------------------------------------------------------
/**
 * @breif  チーム名クリア（初期値「ーーー」）にする
 */
//--------------------------------------------------------------------------
void NBRLowerViewTeamSelect::ClearTeamName()
{
  app::util::GetPaneHelper helper( GetG2DUtil()->GetLayoutWork(0), GetG2DUtil()->GetLayoutResourceID() );
  // チーム名「-----」
  GetG2DUtil()->SetTextBoxPaneString( 
    helper.Clear().Push( PANENAME_NBR_SLC_LOW_000_PANE_BUTTON_L2 ).GetTextBoxPane( PANENAME_GTS_BTN_LOW_003_PANE_TEXTBOX_01 ), 
    msg_nbr_sel_12 );

  // カーソルをチーム選択に合わせる
  CursorMoveToButton( BUTTON_ID_TEAM );
}

//--------------------------------------------------------------------------
/**
 * @breif  BGMテキスト変更
 * @param  id   
 */
//--------------------------------------------------------------------------
void NBRLowerViewTeamSelect::ChangeBgmText( u8 id, const bool isNiji )
{
  u32 bgmTextLabel = NetApp::BgmSelect::NetworkBattleBgmSelect::GetBgmTextLabel( id, isNiji );
  // NBRはNBR用mstxtなのでオフセットとしてずらす。
  bgmTextLabel += msg_nbr_cap_bgm_01;

  ICHI_PRINT( ">ICHI ChangeBgmLabel %d \n", bgmTextLabel );

  // BGM
  GetG2DUtil()->SetTextBoxPaneString( 0, PANENAME_NBR_SLC_LOW_000_PANE_TEXT_BGM, bgmTextLabel );
}

//--------------------------------------------------------------------------
/**
* @breif  ボタン設定
*/
//--------------------------------------------------------------------------
void NBRLowerViewTeamSelect::setupButton()
{
  app::util::G2DUtil* pG2DUtil = GetG2DUtil();
  gfl2::lyt::LytMultiResID& resourceID = pG2DUtil->GetLayoutResourceID(0);
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork(0);

  // ボタン情報
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_TEAM, NULL, NULL,
      LA_NBR_NBR_SLC_LOW_000__BUTTON_L2_TOUCH,
      LA_NBR_NBR_SLC_LOW_000__BUTTON_L2_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BUTTON_L2_CANSEL,
      LA_NBR_NBR_SLC_LOW_000__BUTTON_L2_TOUCH_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BUTTON_L2_ACTIVE,
      LA_NBR_NBR_SLC_LOW_000__BUTTON_L2_PASSIVE,
    },
    {
      BUTTON_ID_CANNOT_JOIN, NULL, NULL,
      LA_NBR_NBR_SLC_LOW_000__BUTTONS_02_TOUCH,
      LA_NBR_NBR_SLC_LOW_000__BUTTONS_02_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BUTTONS_02_CANSEL,
      LA_NBR_NBR_SLC_LOW_000__BUTTONS_02_TOUCH_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BUTTONS_02_ACTIVE,
      LA_NBR_NBR_SLC_LOW_000__BUTTONS_02_PASSIVE,
    },
    {
      BUTTON_ID_BATTLE_START, NULL, NULL,
      LA_NBR_NBR_SLC_LOW_000__BUTTON_GREEN_00_TOUCH,
      LA_NBR_NBR_SLC_LOW_000__BUTTON_GREEN_00_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BUTTON_GREEN_00_CANSEL,
      LA_NBR_NBR_SLC_LOW_000__BUTTON_GREEN_00_TOUCH_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BUTTON_GREEN_00_ACTIVE,
      LA_NBR_NBR_SLC_LOW_000__BUTTON_GREEN_00_PASSIVE,
    },
    {
      BUTTON_ID_BGM_L, NULL, NULL,
      LA_NBR_NBR_SLC_LOW_000__CURSOR_L_TOUCH,
      LA_NBR_NBR_SLC_LOW_000__CURSOR_L_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__CURSOR_L_CANSEL,
      LA_NBR_NBR_SLC_LOW_000__CURSOR_L_TOUCH_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__CURSOR_L_ACTIVE,
      LA_NBR_NBR_SLC_LOW_000__CURSOR_L_PASSIVE,
    },
    {
      BUTTON_ID_BGM_R, NULL, NULL,
      LA_NBR_NBR_SLC_LOW_000__CURSOR_R_TOUCH,
      LA_NBR_NBR_SLC_LOW_000__CURSOR_R_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__CURSOR_R_CANSEL,
      LA_NBR_NBR_SLC_LOW_000__CURSOR_R_TOUCH_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__CURSOR_R_ACTIVE,
      LA_NBR_NBR_SLC_LOW_000__CURSOR_R_PASSIVE,
    },
    {
      BUTTON_ID_RETURN, NULL, NULL,
      LA_NBR_NBR_SLC_LOW_000__BACKBUTTON_TOUCH,
      LA_NBR_NBR_SLC_LOW_000__BACKBUTTON_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BACKBUTTON_CANSEL,
      LA_NBR_NBR_SLC_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BACKBUTTON_ACTIVE,
      LA_NBR_NBR_SLC_LOW_000__BACKBUTTON_PASSIVE,
    },
  };

  // ピクチャペインとバウンディングボックスは後から入れる
  { 
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_NBR_SLC_LOW_000_PANE_BUTTON_L2 );
    buttonInfoTable[ BUTTON_ID_TEAM ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_TEAM ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_GTS_BTN_LOW_003_PANE_BOUND_00, &resourceID );
  }
  {
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONS_02 );
    buttonInfoTable[ BUTTON_ID_CANNOT_JOIN ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_CANNOT_JOIN ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00, &resourceID );
  }
  {
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_NBR_SLC_LOW_000_PANE_BUTTON_GREEN_00 );
    buttonInfoTable[ BUTTON_ID_BATTLE_START ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_BATTLE_START ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00, &resourceID );
  }
  {
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_NBR_SLC_LOW_000_PANE_CURSOR_L );
    buttonInfoTable[ BUTTON_ID_BGM_L ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_BGM_L ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00, &resourceID );
  }
  {
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_NBR_SLC_LOW_000_PANE_CURSOR_R );
    buttonInfoTable[ BUTTON_ID_BGM_R ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_BGM_R ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00, &resourceID );
  }
  {// 戻るボタン
    gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_NBR_SLC_LOW_000_PANE_BACKBUTTON );
    buttonInfoTable[ BUTTON_ID_RETURN ].picture_pane = pBtnBack;
    buttonInfoTable[ BUTTON_ID_RETURN ].bound_pane = pLower->GetBoundingPane( pBtnBack, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &resourceID );
  }

  // ボタン生成
  CreateButtonManager( m_pHeap, pLower, buttonInfoTable, BUTTON_MAX );

  // カーソル
  {
    const u32 chooseFunctionCursorPartsIDTable[] = {
      PANENAME_NBR_SLC_LOW_000_PANE_BUTTON_L2,
      PANENAME_NBR_SLC_LOW_000_PANE_BUTTONS_02,
      PANENAME_NBR_SLC_LOW_000_PANE_BUTTON_GREEN_00,
    };
    const u32 chooseFunctionCursorPaneIDTable[GFL_NELEMS(chooseFunctionCursorPartsIDTable)] = {
      PANENAME_GTS_BTN_LOW_003_PANE_NULL_CUR,
      PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR,
      PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR,
    };
    const app::tool::ButtonManager::ButtonId  chooseFunctionCursorUIButtonIDTable[GFL_NELEMS(chooseFunctionCursorPartsIDTable)] = {
      BUTTON_ID_TEAM,         ///< 対戦チームボタン
      BUTTON_ID_CANNOT_JOIN,  ///< 参加出来ないボタン
      BUTTON_ID_BATTLE_START, ///< 勝負開始ボタン
    };

    CreateCursor(
          NBR_RESOURCE_ID_MENU_CURSOR,
          0,
          0,
          chooseFunctionCursorPartsIDTable,
          chooseFunctionCursorPaneIDTable,
          chooseFunctionCursorUIButtonIDTable,
          GFL_NELEMS(chooseFunctionCursorPartsIDTable)
        );
  }

  app::tool::ButtonManager * pButtonManager = GetButtonManager();
  // SE設定
  pButtonManager->SetButtonSelectSE( BUTTON_ID_TEAM,        SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_CANNOT_JOIN, SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BATTLE_START,SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BGM_L,       SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BGM_R,       SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_RETURN,      SE_RETURN );

  // Bボタン
  //pButtonManager->SetButtonBindKey( BUTTON_ID_RETURN, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  RegisterShortcut_R( BUTTON_ID_BGM_R );
  RegisterShortcut_L( BUTTON_ID_BGM_L );
  RegisterShortcut_Right( BUTTON_ID_BGM_R );
  RegisterShortcut_Left( BUTTON_ID_BGM_L );
  RegisterShortcut_B( BUTTON_ID_RETURN );

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
::app::ui::UIInputListener::ListenerResult NBRLowerViewTeamSelect::OnLayoutPaneEvent( const u32 painId )
{
  ICHI_PRINT("NBRLowerViewTeamSelect::OnLayoutPaneEvent %d \n", painId );
  if( m_pListener == NULL )
  {
    return ENABLE_ACCESS;
  }

  switch( painId ){
  case BUTTON_ID_TEAM:        m_pListener->OnTeamButtonAction(); break;
  case BUTTON_ID_CANNOT_JOIN: m_pListener->OnCannnotJoinButtonAction(); break;
  case BUTTON_ID_BATTLE_START:m_pListener->OnBattleStartButtonAction(); break;
  case BUTTON_ID_BGM_L:       m_pListener->OnBgmLButtonAction(); break;
  case BUTTON_ID_BGM_R:       m_pListener->OnBgmRButtonAction(); break;
  case BUTTON_ID_RETURN:      m_pListener->OnBackButtonAction(); break;
  }

  return DISABLE_ACCESS;
}

//--------------------------------------------------------------------------
/**
* @breif  テキスト初期化
*/
//--------------------------------------------------------------------------
void NBRLowerViewTeamSelect::initializeText()
{
  app::util::GetPaneHelper helper( GetG2DUtil()->GetLayoutWork(0), GetG2DUtil()->GetLayoutResourceID() );

  // さんかする　チーム
  GetG2DUtil()->SetTextBoxPaneString( 
    helper.Clear().Push( PANENAME_NBR_SLC_LOW_000_PANE_BUTTON_L2 ).GetTextBoxPane( PANENAME_GTS_BTN_LOW_003_PANE_TEXTBOX_00 ), 
    msg_nbr_sel_09 );

  // チーム名「-----」
  ClearTeamName();

  // さんか　できない
  GetG2DUtil()->SetTextBoxPaneString( 
    helper.Clear().Push( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONS_02 ).GetTextBoxPane( PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00 ), 
    msg_nbr_sel_11 );

  // しょうぶ　かいし！
  GetG2DUtil()->SetTextBoxPaneString( 
    helper.Clear().Push( PANENAME_NBR_SLC_LOW_000_PANE_BUTTON_GREEN_00 ).GetTextBoxPane( PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00 ), 
    msg_nbr_sel_08 );

  // BGM
  ChangeBgmText(0, true );
}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)
