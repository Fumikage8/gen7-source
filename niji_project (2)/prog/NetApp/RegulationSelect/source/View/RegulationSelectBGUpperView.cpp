// ============================================================================
/*
 * @file RegulationSelectBGUpperView.cpp
 * @brief レギュレーション選択の上画面（背景）を表示するクラスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/View/RegulationSelectBGUpperView.h"
#include "NetApp/RegulationSelect/source/RegulationSelectResourceID.h"

#include <arc_index/message.gaix>
#include <niji_conv_header/app/regulation/RegulationSelect.h>
#include <niji_conv_header/app/regulation/RegulationSelect_anim_list.h>
#include <niji_conv_header/app/regulation/RegulationSelect_pane.h>
#include <niji_conv_header/message/msg_regulation.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Draw)

RegulationSelectBGUpperView::RegulationSelectBGUpperView( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pRegulationSelectWork,
    0,
    LYTRES_REGULATIONSELECT_REGULATION_BG_UPP_000_BFLYT,
    LA_REGULATIONSELECT_REGULATION_BG_UPP_000___NUM,
    app::util::G2DUtil::SETUP_UPPER,
    pRegulationSelectWork->GetMessageUtility(),
    GARC_message_regulation_DAT ),
  m_pRegulationSelectWork( pRegulationSelectWork )
{
  setupLayout( 0, LA_REGULATIONSELECT_REGULATION_BG_UPP_000__BG_00_BG_LOOP_00 );
}


RegulationSelectBGUpperView::~RegulationSelectBGUpperView()
{
}


void RegulationSelectBGUpperView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pRegulationSelectWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, 0 );
}


GFL_NAMESPACE_END(Draw)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)
