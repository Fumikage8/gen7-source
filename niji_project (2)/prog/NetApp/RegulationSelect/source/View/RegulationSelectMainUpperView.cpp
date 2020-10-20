// ============================================================================
/*
 * @file RegulationSelectMainUpperView.cpp
 * @brief レギュレーション選択の上画面を表示するクラスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/View/RegulationSelectMainUpperView.h"
#include "NetApp/RegulationSelect/source/RegulationSelectResourceID.h"

#include <arc_index/message.gaix>
#include <niji_conv_header/app/regulation/RegulationSelect.h>
#include <niji_conv_header/app/regulation/RegulationSelect_anim_list.h>
#include <niji_conv_header/app/regulation/RegulationSelect_pane.h>
#include <niji_conv_header/message/msg_regulation.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Draw)

RegulationSelectMainUpperView::RegulationSelectMainUpperView( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork ) :
  NetAppLib::System::NetApplicationViewBase( pRegulationSelectWork->GetAppHeap() ),
  m_pRegulationSelectWork( pRegulationSelectWork ),
  m_NetAppRegulationUpperView( pRegulationSelectWork, NetApp::RegulationSelect::REGULATIONSELECT_RESOURCE_ID_LAYOUT, pRegulationSelectWork->GetMessageUtility() )
{
  AddSubView( &m_NetAppRegulationUpperView );
}


RegulationSelectMainUpperView::~RegulationSelectMainUpperView()
{
  m_NetAppRegulationUpperView.RemoveFromSuperView();
}


void RegulationSelectMainUpperView::SetRegulationDrawInfo( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo )
{
  m_NetAppRegulationUpperView.SetRegulationDrawInfo( pRegulationDrawInfo, m_pRegulationSelectWork->GetBootMode() );
}


void RegulationSelectMainUpperView::ResetRegulationDrawInfo()
{
  m_NetAppRegulationUpperView.ResetRegulationDrawInfo();
}


GFL_NAMESPACE_END(Draw)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)
