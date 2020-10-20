// ============================================================================
/*
 * @file NBRLowerView2Button.cpp
 * @brief 通信対戦受付アプリ 下画面：ルール誰が選ぶか画面
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include "NetApp/NBR/source/View/SubView/NBRLowerView2Button.h"
#include "NetApp/NBR/source/NBRResourceID.h"

// Pane取得ヘルパー
#include  "AppLib/include/Util/app_util_GetPaneHelper.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)

NBRLowerView2Button::NBRLowerView2Button( NetAppLib::System::ApplicationWorkBase* pWorkBase,
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

  SetInputListener( this );

  // 最初は非表示
  SetVisible( ALL_VIEW_OFF );
}


NBRLowerView2Button::~NBRLowerView2Button()
{
  ReleaseCursor();
  Delete2D();
}

//--------------------------------------------------------------------------
/**
* @breif  イベントリスナーの登録
* @param  pNBRLowerView2ButtonListener リスナーのポインタ
*/
//--------------------------------------------------------------------------
void NBRLowerView2Button::SetListener( NBRLowerView2ButtonListener* pNBRLowerView2ButtonListener )
{
  m_pListener = pNBRLowerView2ButtonListener;
}

//--------------------------------------------------------------------------
/**
* @breif  イベントリスナーの解除
*/
//--------------------------------------------------------------------------
void NBRLowerView2Button::RemoveListener()
{
  m_pListener = NULL;
}

//--------------------------------------------------------------------------
/**
* @breif  ビュー全体の表示・非表示
* @param  visibleMode   表示モード定義
*/
//--------------------------------------------------------------------------
void NBRLowerView2Button::SetVisible( const VisibleMode visibleMode )
{
  bool isRootVisible = false;         // 親ペイン
  bool isReturnButtonVisible = false; // 戻るボタン

  switch(visibleMode){
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
  case TWO_BUTTON_ONLY_ON:       //! 2ボタンのみ非表示
    isRootVisible = true;
    isReturnButtonVisible = false;
    break;
  case CURSOR_OFF:             //! カーソル非表示
    SetCursorVisible( false );
    return;
  case CURSOR_ON:              //! カーソル表示
    SetCursorVisible( true );
    return;
  }

  // 親ペイン
  GetG2DUtil()->SetPaneVisible( 0, PANENAME_NBR_SLC_LOW_000_PANE_2BUTTON, isRootVisible );
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
    SetButtonEnable( BUTTON_ID_MYSELF, isRootVisible, false );
    SetButtonEnable( BUTTON_ID_OTHERS, isRootVisible, false );
    SetButtonEnable( BUTTON_ID_RETURN, isReturnButtonVisible, false );
  }

  ICHI_PRINT("NBRLowerView2Button::SetVisible mode:%d \n", visibleMode );
}

//--------------------------------------------------------------------------
/**
 * @breif  ボタンのテキスト設定
 * @param  mode  ButtonTextMode
 */
//--------------------------------------------------------------------------
void NBRLowerView2Button::SetButtonText( ButtonTextMode mode )
{
  u32 button1msgId, button2msgId;

  switch( mode ){
  //! 「自分が決める/他の人に決める」のボタン
  case BUTTON_TEXT_MODE_WHO_DECIDE:
    button1msgId = msg_nbr_sel_01; // 自分が決める
    button2msgId = msg_nbr_sel_02; // 他の人におまかせ
    break;

  //! 「同じルールで遊ぶ/ルールを変えて遊ぶ」のボタン
  case BUTTON_TEXT_MODE_RULE:
    button1msgId = msg_nbr_sel_04; // おなじルールであそぶ
    button2msgId = msg_nbr_sel_03; // ルールをかえてあそぶ
    break;

  //! 「選びなおす/確認する」のボタン
  case BUTTON_TEXT_MODE_POKE_UNKNOWN:
    button1msgId = msg_nbr_sel_25; // 選びなおす
    button2msgId = msg_nbr_sel_26; // 確認する
    break;

  default:
    GFL_ASSERT(0);
    button1msgId = msg_nbr_sel_01; // 自分が決める
    button2msgId = msg_nbr_sel_02; // 他の人におまかせ
    break;
  }

  app::util::GetPaneHelper helper( GetG2DUtil()->GetLayoutWork(0), GetG2DUtil()->GetLayoutResourceID() );

  // ボタン１
  GetG2DUtil()->SetTextBoxPaneString( 
    helper.Clear().Push( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONS_00 ).GetTextBoxPane( PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00 ), 
    button1msgId );

  // ボタン２
  GetG2DUtil()->SetTextBoxPaneString( 
    helper.Clear().Push( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONS_01 ).GetTextBoxPane( PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00 ), 
    button2msgId );
}




//--------------------------------------------------------------------------
/**
* @breif  ボタン設定
*/
//--------------------------------------------------------------------------
void NBRLowerView2Button::setupButton()
{
  app::util::G2DUtil* pG2DUtil = GetG2DUtil();
  gfl2::lyt::LytMultiResID& resourceID = pG2DUtil->GetLayoutResourceID(0);
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork(0);

  // ボタン情報
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_MYSELF, NULL, NULL,
      LA_NBR_NBR_SLC_LOW_000__BUTTONS_00_TOUCH,
      LA_NBR_NBR_SLC_LOW_000__BUTTONS_00_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BUTTONS_00_CANSEL,
      LA_NBR_NBR_SLC_LOW_000__BUTTONS_00_TOUCH_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BUTTONS_00_ACTIVE,
      LA_NBR_NBR_SLC_LOW_000__BUTTONS_00_PASSIVE,
    },
    {
      BUTTON_ID_OTHERS, NULL, NULL,
      LA_NBR_NBR_SLC_LOW_000__BUTTONS_01_TOUCH,
      LA_NBR_NBR_SLC_LOW_000__BUTTONS_01_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BUTTONS_01_CANSEL,
      LA_NBR_NBR_SLC_LOW_000__BUTTONS_01_TOUCH_RELEASE,
      LA_NBR_NBR_SLC_LOW_000__BUTTONS_01_ACTIVE,
      LA_NBR_NBR_SLC_LOW_000__BUTTONS_01_PASSIVE,
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
  {// 自分ボタン
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONS_00 );
    buttonInfoTable[ BUTTON_ID_MYSELF ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_MYSELF ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00, &resourceID );
  }
  {// 他人ボタン
    gfl2::lyt::LytParts* pButton = pLower->GetPartsPane( PANENAME_NBR_SLC_LOW_000_PANE_BUTTONS_01 );
    buttonInfoTable[ BUTTON_ID_OTHERS ].picture_pane = pButton;
    buttonInfoTable[ BUTTON_ID_OTHERS ].bound_pane = pLower->GetBoundingPane( pButton, PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00, &resourceID );
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
      PANENAME_NBR_SLC_LOW_000_PANE_BUTTONS_00,
      PANENAME_NBR_SLC_LOW_000_PANE_BUTTONS_01,
    };
    const u32 chooseFunctionCursorPaneIDTable[GFL_NELEMS(chooseFunctionCursorPartsIDTable)] = {
      PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR,
      PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR,
    };
    const app::tool::ButtonManager::ButtonId  chooseFunctionCursorUIButtonIDTable[GFL_NELEMS(chooseFunctionCursorPartsIDTable)] = {
      BUTTON_ID_MYSELF,
      BUTTON_ID_OTHERS,
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
  pButtonManager->SetButtonSelectSE( BUTTON_ID_MYSELF, SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_OTHERS, SE_DECIDE );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_RETURN, SE_RETURN );

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
::app::ui::UIInputListener::ListenerResult NBRLowerView2Button::OnLayoutPaneEvent( const u32 painId )
{
  ICHI_PRINT("NBRLowerView2Button::OnLayoutPaneEvent %d \n", painId );
  if( m_pListener == NULL )
  {
    return ENABLE_ACCESS;
  }

  switch( painId ){
  case BUTTON_ID_MYSELF: m_pListener->On2ButtonAction( BUTTON_ID_MYSELF ); break;
  case BUTTON_ID_OTHERS: m_pListener->On2ButtonAction( BUTTON_ID_OTHERS ); break;
  case BUTTON_ID_RETURN: m_pListener->OnBackButtonAction(); break;
  }

  return DISABLE_ACCESS;
}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)
