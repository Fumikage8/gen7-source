// ============================================================================
/*
 * @file NBRUpperViewRegulation.cpp
 * @brief 通信対戦受付アプリ 上画面：レギュレーション詳細
 * @date 2015.07.29
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/View/SubView/NBRUpperViewRegulation.h"
#include "NetApp/NBR/source/NBRResourceID.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)


NBRUpperViewRegulation::NBRUpperViewRegulation( NetAppLib::System::ApplicationWorkBase* pWorkBase,
                                                app::util::G2DUtil* pG2dUtil,
                                                app::util::G2DUtil::LAYOUT_WORK* pLayoutWork,
                                                NetAppLib::Util::NetAppTrainerIconUtility& netAppTrainerIconUtility ) :
  NBRUpperViewBase( pWorkBase, netAppTrainerIconUtility ),
    m_NetAppRegulationUpperView( pWorkBase->GetAppHeap(),
                                 pWorkBase->GetAppRenderingManager(),
                                 pWorkBase->GetResourceManager()->GetResourceBuffer( NetApp::NBR::NBR_RESOURCE_ID_REGULATIONSELECT ) )

{
  AddSubView( &m_NetAppRegulationUpperView );
  // 最初は非表示
  SetVisible(false);
}


NBRUpperViewRegulation::~NBRUpperViewRegulation()
{
  m_NetAppRegulationUpperView.RemoveFromSuperView();
}

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)
