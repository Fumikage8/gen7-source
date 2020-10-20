// ============================================================================
/*
 * @file MiracleTradePokemonInfoUpperView.cpp
 * @brief ミラクル交換のポケモン情報を表示するクラスです。
 * @date 2015.05.25
 */
// ============================================================================
#include "NetApp/MiracleTrade/source/Matching/MiracleTradePokemonInfoUpperView.h"
#include "NetApp/MiracleTrade/source/MiracleTradeResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include <arc_index/message.gaix>
#include <niji_conv_header/app/miracletrade/MiracleTrade.h>
#include <niji_conv_header/app/miracletrade/MiracleTrade_anim_list.h>
#include <niji_conv_header/app/miracletrade/MiracleTrade_pane.h>
#include <niji_conv_header/message/msg_miracletrade.h>
#include <PokeTool/include/PokeTool.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(Matching)


MiracleTradePokemonInfoUpperView::MiracleTradePokemonInfoUpperView( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork ) :
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
  setupLayout( LAYOUT_WORK_ID_UPPER, LA_MIRACLETRADE_MIRACLETRADE_MIN_UPP_PATERN_01 );
  setupTextMessage();
}


MiracleTradePokemonInfoUpperView::~MiracleTradePokemonInfoUpperView()
{
}


void MiracleTradePokemonInfoUpperView::setupTextMessage()
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID miracleTradeResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_MIRACLE_TRADE );

  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );

  app::util::Heap* pAppHeap = m_pMiracleTradeWork->GetAppHeap();

  // 画面上のテキスト
  {
    gfl2::lyt::LytParts* pMessage = pUpper->GetPartsPane( PANENAME_MIRACLETRADE_MIN_LOW_PANE_MESSAGE );
    gfl2::lyt::LytTextBox* pTextBox00 = pUpper->GetTextBoxPane( pMessage, PANENAME_COMMON_MSG_UPP_001_PANE_TEXTBOX_00, &miracleTradeResourceID );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox00, msg_miracletrade_cap_01 );
  }

  pml::pokepara::CoreParam& selectPokemonCoreParam = m_pMiracleTradeWork->GetSelectPokemonCoreParam();

  // ポケモンのニックネーム
  {
    gfl2::lyt::LytTextBox* pTextBox01 = pUpper->GetTextBoxPane( PANENAME_MIRACLETRADE_MIN_UPP_PANE_TEXTBOX_01 );
    pG2DUtil->SetRegisterPokeNickName( 0, &selectPokemonCoreParam );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox01, msg_miracletrade_cap_02 );
  }

  // ポケモンのレベル
  {
    gfl2::lyt::LytTextBox* pTextLv = pUpper->GetTextBoxPane( PANENAME_MIRACLETRADE_MIN_UPP_PANE_TEXT_LV );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextLv, msg_miracletrade_cap_13 );

    gfl2::lyt::LytTextBox* pTextLv999 = pUpper->GetTextBoxPane( PANENAME_MIRACLETRADE_MIN_UPP_PANE_TEXTLV_999 );
    pG2DUtil->SetRegisterNumber( 0, selectPokemonCoreParam.GetLevel(), 3 );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextLv999, msg_miracletrade_cap_03 );
  }

  // ポケモンが所持しているアイテムの名前
  {
    gfl2::lyt::LytTextBox* pTextBox02 = pUpper->GetTextBoxPane( PANENAME_MIRACLETRADE_MIN_UPP_PANE_TEXTBOX_02 );
    pG2DUtil->SetRegisterItemName( 0, selectPokemonCoreParam.GetItem() );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox02, msg_miracletrade_cap_06 );
  }

  // 性別アイコン
  {
    gfl2::lyt::LytParts* pParts = pUpper->GetPartsPane( PANENAME_MIRACLETRADE_MIN_UPP_PANE_ICON_GENDER_00 );
    pml::Sex drawSex = PokeTool::GetDrawSex( &selectPokemonCoreParam );
    if( drawSex == pml::SEX_UNKNOWN )
    {
      pParts->SetVisible( false );
    }
    else
    {
      u32 animeID = ( drawSex == pml::SEX_MALE ) ? LA_MIRACLETRADE_MIRACLETRADE_MIN_UPP__ICON_GENDER_00_ICON_MALE : LA_MIRACLETRADE_MIRACLETRADE_MIN_UPP__ICON_GENDER_00_ICON_FEMALE;
      pParts->SetVisible( true );
      pG2DUtil->StartAnime( LAYOUT_WORK_ID_UPPER, animeID );
    }
  }

}


void MiracleTradePokemonInfoUpperView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pMiracleTradeWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_UPPER );
}



GFL_NAMESPACE_END(Matching)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)
