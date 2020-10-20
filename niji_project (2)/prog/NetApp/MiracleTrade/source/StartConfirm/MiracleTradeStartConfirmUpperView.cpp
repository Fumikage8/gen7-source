// ============================================================================
/*
 * @file MiracleTradeStartConfirmUpperView.cpp
 * @brief ミラクル交換の開始確認を行う画面を表示するクラスです。
 * @date 2015.05.25
 */
// ============================================================================
#include "NetApp/MiracleTrade/source/StartConfirm/MiracleTradeStartConfirmUpperView.h"
#include "NetApp/MiracleTrade/source/MiracleTradeResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include <arc_index/message.gaix>
#include <niji_conv_header/app/miracletrade/MiracleTrade.h>
#include <niji_conv_header/app/miracletrade/MiracleTrade_anim_list.h>
#include <niji_conv_header/app/miracletrade/MiracleTrade_pane.h>
#include <niji_conv_header/message/msg_miracletrade.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(StartConfirm)


MiracleTradeStartConfirmUpperView::MiracleTradeStartConfirmUpperView( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pMiracleTradeWork,
    NetApp::MiracleTrade::MIRACLETRADE_RESOURCE_ID_LAYOUT,
    LYTRES_MIRACLETRADE_MIRACLETRADE_MIN_UPP_BFLYT,
    LA_MIRACLETRADE_MIRACLETRADE_MIN_UPP___NUM,
    app::util::G2DUtil::SETUP_UPPER,
    pMiracleTradeWork->GetMessageUtility(),
    GARC_message_miracletrade_DAT ),
  m_pMiracleTradeWork( pMiracleTradeWork )
{
  setupTextMessage();
}


MiracleTradeStartConfirmUpperView::~MiracleTradeStartConfirmUpperView()
{
}


void MiracleTradeStartConfirmUpperView::setupTextMessage()
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID miracleTradeResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_MIRACLE_TRADE );

  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );

  setupLayout( LAYOUT_WORK_ID_UPPER, LA_MIRACLETRADE_MIRACLETRADE_MIN_UPP_PATERN_00 );

  // 画面上のテキスト
  {
    gfl2::lyt::LytParts* pMessage = pUpper->GetPartsPane( PANENAME_MIRACLETRADE_MIN_LOW_PANE_MESSAGE );
    gfl2::lyt::LytTextBox* pTextBox00 = pUpper->GetTextBoxPane( pMessage, PANENAME_COMMON_MSG_UPP_001_PANE_TEXTBOX_00, &miracleTradeResourceID );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox00, msg_miracletrade_cap_01 );
  }

  // 説明テキスト
  {
    gfl2::lyt::LytTextBox* pTextBox00 = pUpper->GetTextBoxPane( PANENAME_MIRACLETRADE_MIN_UPP_PANE_TEXTBOX_00 );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox00, msg_miracletrade_exp_01 );
  }

}


void MiracleTradeStartConfirmUpperView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pMiracleTradeWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_UPPER );
}



GFL_NAMESPACE_END(StartConfirm)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)
