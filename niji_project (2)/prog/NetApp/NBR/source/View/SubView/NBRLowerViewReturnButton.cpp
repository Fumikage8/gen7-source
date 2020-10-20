// ============================================================================
/*
 * @file NBRLowerViewReturnButton.cpp
 * @brief 通信対戦受付アプリ 下画面：戻るボタンのみ
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include "NetApp/NBR/source/View/SubView/NBRLowerViewReturnButton.h"
#include "NetApp/NBR/source/NBRResourceID.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)

NBRLowerViewReturnButton::NBRLowerViewReturnButton( NetAppLib::System::ApplicationWorkBase* pWorkBase,
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
  SetAllVisible(false);
}


NBRLowerViewReturnButton::~NBRLowerViewReturnButton()
{
  Delete2D();
}

//--------------------------------------------------------------------------
/**
* @breif  イベントリスナーの登録
* @param  pNBRLowerViewReturnButtonListener リスナーのポインタ
*/
//--------------------------------------------------------------------------
void NBRLowerViewReturnButton::SetListener( NBRLowerViewReturnButtonListener* pNBRLowerViewReturnButtonListener )
{
  m_pListener = pNBRLowerViewReturnButtonListener;
}

//--------------------------------------------------------------------------
/**
* @breif  イベントリスナーの解除
*/
//--------------------------------------------------------------------------
void NBRLowerViewReturnButton::RemoveListener()
{
  m_pListener = NULL;
}

//--------------------------------------------------------------------------
/**
* @breif  ビュー全体の表示・非表示
* @param  isVisible         true = 表示, false = 非表示
*/
//--------------------------------------------------------------------------
void NBRLowerViewReturnButton::SetAllVisible( const bool isVisible )
{
  // 戻るボタン
  {
    gfl2::lyt::LytWk* pLower = GetG2DUtil()->GetLayoutWork(0);
    gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_NBR_SLC_LOW_000_PANE_BACKBUTTON );
    pBtnBack->SetVisible( isVisible );
    SetButtonEnable( BUTTON_ID_RETURN, isVisible, false );
  }
  ICHI_PRINT("NBRLowerViewReturnButton::SetAllVisible %d \n", isVisible );
}

//--------------------------------------------------------------------------
/**
* @breif  ボタン設定
*/
//--------------------------------------------------------------------------
void NBRLowerViewReturnButton::setupButton()
{
  app::util::G2DUtil* pG2DUtil = GetG2DUtil();
  gfl2::lyt::LytMultiResID& resourceID = pG2DUtil->GetLayoutResourceID(0);
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork(0);

  // ボタン情報
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
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
  {// 戻るボタン
    gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_NBR_SLC_LOW_000_PANE_BACKBUTTON );
    buttonInfoTable[ BUTTON_ID_RETURN ].picture_pane = pBtnBack;
    buttonInfoTable[ BUTTON_ID_RETURN ].bound_pane = pLower->GetBoundingPane( pBtnBack, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &resourceID );
  }

  // ボタン生成
  CreateButtonManager( m_pHeap, pLower, buttonInfoTable, BUTTON_MAX );

  app::tool::ButtonManager * pButtonManager = GetButtonManager();
  // SE設定
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
::app::ui::UIInputListener::ListenerResult NBRLowerViewReturnButton::OnLayoutPaneEvent( const u32 painId )
{
  ICHI_PRINT("NBRLowerViewReturnButton::OnLayoutPaneEvent %d \n", painId );
  if( m_pListener == NULL )
  {
    return ENABLE_ACCESS;
  }

  switch( painId ){
  case BUTTON_ID_RETURN: m_pListener->OnBackButtonAction(); break;
  }

  return DISABLE_ACCESS;
}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)
