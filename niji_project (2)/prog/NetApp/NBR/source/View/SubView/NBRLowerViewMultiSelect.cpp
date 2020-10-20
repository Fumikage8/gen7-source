// ============================================================================
/*
 * @file NBRLowerViewMultiSelect.cpp
 * @brief 通信対戦受付アプリ 下画面：チーム選択画面
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include "NetApp/NBR/source/View/SubView/NBRLowerViewMultiSelect.h"
#include "NetApp/NBR/source/NBRResourceID.h"

// Pane取得ヘルパー
#include  "AppLib/include/Util/app_util_GetPaneHelper.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)

NBRLowerViewMultiSelect::NBRLowerViewMultiSelect( NetAppLib::System::ApplicationWorkBase* pWorkBase,
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
  InitializeText();

  SetInputListener( this );
  // 最初は非表示
  SetVisible(ALL_VIEW_OFF);
}


NBRLowerViewMultiSelect::~NBRLowerViewMultiSelect()
{
  ReleaseCursor();
  Delete2D();
}

//--------------------------------------------------------------------------
/**
* @breif  イベントリスナーの登録
* @param  pNBRLowerViewMultiSelectListener リスナーのポインタ
*/
//--------------------------------------------------------------------------
void NBRLowerViewMultiSelect::SetListener( NBRLowerViewMultiSelectListener* pNBRLowerViewMultiSelectListener )
{
  m_pListener = pNBRLowerViewMultiSelectListener;
}

//--------------------------------------------------------------------------
/**
* @breif  イベントリスナーの解除
*/
//--------------------------------------------------------------------------
void NBRLowerViewMultiSelect::RemoveListener()
{
  m_pListener = NULL;
}

//--------------------------------------------------------------------------
/**
* @breif  ビュー全体の表示・非表示
* @param  visibleMode   表示モード定義
*/
//--------------------------------------------------------------------------
void NBRLowerViewMultiSelect::SetVisible( const VisibleMode visibleMode )
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
  case TEAM_DECIDE_BUTTON_PASSIVE:   //! チーム決定ボタンをパッシブ
    GetButtonManager()->SetButtonPassiveForce( BUTTON_ID_DECIDE );
    return;
  case TEAM_DECIDE_BUTTON_ACTIVE:    //! チーム決定ボタンをアクティブ
    GetButtonManager()->SetButtonActiveForce(  BUTTON_ID_DECIDE  );
    return;
  }

  // 親ペイン
  GetG2DUtil()->SetPaneVisible( 0, PANENAME_NBR_SLC_LOW_000_PANE_MULTISELECT, isRootVisible );

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
    SetButtonEnable( BUTTON_ID_NO_1,        isRootVisible, false );
    SetButtonEnable( BUTTON_ID_NO_2,        isRootVisible, false );
    SetButtonEnable( BUTTON_ID_NO_3,        isRootVisible, false );
    SetButtonEnable( BUTTON_ID_NO_4,        isRootVisible, false );
    SetButtonEnable( BUTTON_ID_DECIDE,      isRootVisible, false );
    SetButtonEnable( BUTTON_ID_RETURN,      isReturnButtonVisible, false );
  }
}

//--------------------------------------------------------------------------
/**
* @breif  テキスト初期化
*/
//--------------------------------------------------------------------------
void NBRLowerViewMultiSelect::InitializeText()
{
  app::util::GetPaneHelper helper( GetG2DUtil()->GetLayoutWork(0), GetG2DUtil()->GetLayoutResourceID() );

  // １ばんめ
  GetG2DUtil()->SetTextBoxPaneString( 
    helper.Clear().Push( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONM_00 ).GetTextBoxPane( PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_00 ), 
    msg_nbr_sel_13 );
  // 点線
  GetG2DUtil()->SetTextBoxPaneString( 
    helper.Clear().Push( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONM_00 ).GetTextBoxPane( PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_01 ), 
    msg_nbr_sel_17 );
  // ２ばんめ
  GetG2DUtil()->SetTextBoxPaneString( 
    helper.Clear().Push( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONM_01 ).GetTextBoxPane( PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_00 ), 
    msg_nbr_sel_14 );
  // 点線
  GetG2DUtil()->SetTextBoxPaneString( 
    helper.Clear().Push( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONM_01 ).GetTextBoxPane( PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_01 ), 
    msg_nbr_sel_17 );
  // ３ばんめ
  GetG2DUtil()->SetTextBoxPaneString( 
    helper.Clear().Push( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONM_02 ).GetTextBoxPane( PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_00 ), 
    msg_nbr_sel_15 );
  // 点線
  GetG2DUtil()->SetTextBoxPaneString( 
    helper.Clear().Push( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONM_02 ).GetTextBoxPane( PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_01 ), 
    msg_nbr_sel_17 );
  // ４ばんめ
  GetG2DUtil()->SetTextBoxPaneString( 
    helper.Clear().Push( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONM_03 ).GetTextBoxPane( PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_00 ), 
    msg_nbr_sel_16 );
  // 点線
  GetG2DUtil()->SetTextBoxPaneString( 
    helper.Clear().Push( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONM_03 ).GetTextBoxPane( PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_01 ), 
    msg_nbr_sel_17 );


  // このチームでけってい
  GetG2DUtil()->SetTextBoxPaneString( 
    helper.Clear().Push( PANENAME_NBR_SLC_LOW_000_PANE_BUTTON_GREEN_01 ).GetTextBoxPane( PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00 ), 
    msg_nbr_sel_19 );

}

//--------------------------------------------------------------------------
/**
* @breif      プレイヤー決定
* @param[in]  pos         何番目か(0～3)
* @param[in]  pMyStatus   表示するプレイヤーのマイステータス
*/
//--------------------------------------------------------------------------
void NBRLowerViewMultiSelect::DecidePosPlayer( u8 pos, Savedata::MyStatus* pMyStatus )
{
  // 立ち位置Indexをレイアウトの並びに合うように変換
  if( pos == 1 ){ pos = 2; }
  else if( pos == 2 ){ pos = 1; }

  app::util::GetPaneHelper helper( GetG2DUtil()->GetLayoutWork(0), GetG2DUtil()->GetLayoutResourceID() );
  switch( pos ){
  case 0:
    helper.Clear().Push( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONM_00 ).GetTextBoxPane( PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_01 ); 
    break;
  case 1:
    helper.Clear().Push( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONM_01 ).GetTextBoxPane( PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_01 ); 
    break;
  case 2:
    helper.Clear().Push( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONM_02 ).GetTextBoxPane( PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_01 ); 
    break;
  case 3:
    helper.Clear().Push( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONM_03 ).GetTextBoxPane( PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_01 ); 
    break;
  default:
    GFL_ASSERT(0);
    helper.Clear().Push( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONM_00 );
    break;
  }

  if( pMyStatus == NULL )
  {
    // 点線
    GetG2DUtil()->SetTextBoxPaneString( helper.GetTextBoxPane( PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_01 ) , msg_nbr_sel_17 );
  }
  else
  {
    // プレイヤー名レジスト
    GetG2DUtil()->SetRegisterPlayerName( 0, pMyStatus );
    // プレイヤー名
    GetG2DUtil()->SetTextBoxPaneStringExpand( helper.GetTextBoxPane( PANENAME_GTS_BTN_LOW_004_PANE_TEXTBOX_01 ) , msg_nbr_sel_18 );
  }
}

//--------------------------------------------------------------------------
/**
* @breif  ボタン設定
*/
//--------------------------------------------------------------------------
void NBRLowerViewMultiSelect::setupButton()
{
  app::util::G2DUtil* pG2DUtil = GetG2DUtil();
  gfl2::lyt::LytMultiResID& resourceID = pG2DUtil->GetLayoutResourceID(0);
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork(0);

  // ボタン情報
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_NO_1, NULL, NULL,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_00_TOUCH,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_00_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_00_CANSEL,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_00_TOUCH_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_00_ACTIVE,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_00_PASSIVE,
    },
    {
      BUTTON_ID_NO_2, NULL, NULL,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_01_TOUCH,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_01_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_01_CANSEL,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_01_TOUCH_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_01_ACTIVE,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_01_PASSIVE,
    },
    {
      BUTTON_ID_NO_3, NULL, NULL,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_02_TOUCH,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_02_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_02_CANSEL,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_02_TOUCH_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_02_ACTIVE,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_02_PASSIVE,
    },
    {
      BUTTON_ID_NO_4, NULL, NULL,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_03_TOUCH,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_03_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_03_CANSEL,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_03_TOUCH_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_03_ACTIVE,
      LA_NBR_NBR_SLC_LOW_000__BUTTONM_03_PASSIVE,
    },
    {
      BUTTON_ID_DECIDE, NULL, NULL,
      LA_NBR_NBR_SLC_LOW_000__BUTTON_GREEN_01_TOUCH,
      LA_NBR_NBR_SLC_LOW_000__BUTTON_GREEN_01_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BUTTON_GREEN_01_CANSEL,
      LA_NBR_NBR_SLC_LOW_000__BUTTON_GREEN_01_TOUCH_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BUTTON_GREEN_01_ACTIVE,
      LA_NBR_NBR_SLC_LOW_000__BUTTON_GREEN_01_PASSIVE,
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
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONM_00 );
    buttonInfoTable[ BUTTON_ID_NO_1 ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_NO_1 ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_GTS_BTN_LOW_004_PANE_BOUND_00, &resourceID );
  }
  { 
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONM_01 );
    buttonInfoTable[ BUTTON_ID_NO_2 ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_NO_2 ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_GTS_BTN_LOW_004_PANE_BOUND_00, &resourceID );
  }
  { 
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONM_02 );
    buttonInfoTable[ BUTTON_ID_NO_3 ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_NO_3 ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_GTS_BTN_LOW_004_PANE_BOUND_00, &resourceID );
  }
  { 
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONM_03 );
    buttonInfoTable[ BUTTON_ID_NO_4 ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_NO_4 ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_GTS_BTN_LOW_004_PANE_BOUND_00, &resourceID );
  }
  {
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_NBR_SLC_LOW_000_PANE_BUTTON_GREEN_01 );
    buttonInfoTable[ BUTTON_ID_DECIDE ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_DECIDE ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00, &resourceID );
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
      PANENAME_NBR_SLC_LOW_000_PANE_BUTTONM_00,
      PANENAME_NBR_SLC_LOW_000_PANE_BUTTONM_01,
      PANENAME_NBR_SLC_LOW_000_PANE_BUTTONM_02,
      PANENAME_NBR_SLC_LOW_000_PANE_BUTTONM_03,
      PANENAME_NBR_SLC_LOW_000_PANE_BUTTON_GREEN_01,
    };
    const u32 chooseFunctionCursorPaneIDTable[GFL_NELEMS(chooseFunctionCursorPartsIDTable)] = {
      PANENAME_GTS_BTN_LOW_004_PANE_NULL_CUR,
      PANENAME_GTS_BTN_LOW_004_PANE_NULL_CUR,
      PANENAME_GTS_BTN_LOW_004_PANE_NULL_CUR,
      PANENAME_GTS_BTN_LOW_004_PANE_NULL_CUR,
      PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR,
    };
    const app::tool::ButtonManager::ButtonId  chooseFunctionCursorUIButtonIDTable[GFL_NELEMS(chooseFunctionCursorPartsIDTable)] = {
      BUTTON_ID_NO_1,         ///< 1番目のプレイヤー選択ボタン
      BUTTON_ID_NO_2,         ///< 2番目のプレイヤー選択ボタン
      BUTTON_ID_NO_3,         ///< 3番目のプレイヤー選択ボタン
      BUTTON_ID_NO_4,         ///< 4番目のプレイヤー選択ボタン
      BUTTON_ID_DECIDE,       ///< 決定ボタン
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
  pButtonManager->SetButtonSelectSE( BUTTON_ID_NO_1,        SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_NO_2,        SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_NO_3,        SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_NO_4,        SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_DECIDE,      SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_RETURN,      SE_RETURN );

  // Bボタン
  //pButtonManager->SetButtonBindKey( BUTTON_ID_RETURN, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
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
::app::ui::UIInputListener::ListenerResult NBRLowerViewMultiSelect::OnLayoutPaneEvent( const u32 painId )
{
  ICHI_PRINT("NBRLowerViewMultiSelect::OnLayoutPaneEvent %d \n", painId );
  if( m_pListener == NULL )
  {
    return ENABLE_ACCESS;
  }

  switch( painId ){
  case BUTTON_ID_NO_1:        m_pListener->OnPlayerSelectButtonAction(0); break;
  case BUTTON_ID_NO_2:        m_pListener->OnPlayerSelectButtonAction(1); break;
  case BUTTON_ID_NO_3:        m_pListener->OnPlayerSelectButtonAction(2); break;
  case BUTTON_ID_NO_4:        m_pListener->OnPlayerSelectButtonAction(3); break;
  case BUTTON_ID_DECIDE:      m_pListener->OnDecideButtonAction(); break;
  case BUTTON_ID_RETURN:      m_pListener->OnBackButtonAction(); break;
  }

  return DISABLE_ACCESS;
}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)
