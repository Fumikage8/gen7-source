// ============================================================================
/*
 * @file RegulationDetailLowerView.cpp
 * @brief レギュレーション詳細の下画面です。
 * @date 2015.06.18
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/View/RegulationDetailLowerView.h"
#include "NetApp/RegulationSelect/source/RegulationSelectResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/regulation/RegulationSelect.h>
#include <niji_conv_header/app/regulation/RegulationSelect_anim_list.h>
#include <niji_conv_header/app/regulation/RegulationSelect_pane.h>
#include <niji_conv_header/message/msg_regulation.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Draw)


RegulationDetailLowerView::RegulationDetailLowerView( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pRegulationSelectWork,
    NetApp::RegulationSelect::REGULATIONSELECT_RESOURCE_ID_LAYOUT,
    LYTRES_REGULATIONSELECT_REGULATION_SLC_LOW_000_BFLYT,
    LA_REGULATIONSELECT_REGULATION_SLC_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pRegulationSelectWork->GetMessageUtility(),
    GARC_message_regulation_DAT ),
  m_pRegulationSelectWork( pRegulationSelectWork )
{
  setupLayout( LAYOUT_WORK_ID_LOWER, LA_REGULATIONSELECT_REGULATION_SLC_LOW_000_PATERN_00 );

  UIView::GetG2DUtil()->StartAnime( 0, LA_REGULATIONSELECT_REGULATION_SLC_LOW_000__BG_ANIM_BG_LOOP_00 );
}


RegulationDetailLowerView::~RegulationDetailLowerView()
{
}


void RegulationDetailLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pRegulationSelectWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
}


GFL_NAMESPACE_END(Draw)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)
