// ============================================================================
/*
 * @file NBRUpperViewRegulation.h
 * @brief 通信対戦受付アプリ 上画面：レギュレーション詳細
 * @date 2015.07.29
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_UPPER_VIEW_REGULATION_H_INCLUDE )
#define NBR_UPPER_VIEW_REGULATION_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

// ビューベース 
#include "NetApp/NBR/source/View/NBRUpperViewBase.h"

// レギュレーション詳細View
#include "NetStatic/NetAppLib/include/UI/NetAppRegulationUpperView.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)


class NBRUpperViewRegulation : public NBRUpperViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRUpperViewRegulation );
public:
  NBRUpperViewRegulation( NetAppLib::System::ApplicationWorkBase* pWorkBase,
                  app::util::G2DUtil* pG2dUtil,
                  app::util::G2DUtil::LAYOUT_WORK* pLayoutWork,
                  NetAppLib::Util::NetAppTrainerIconUtility& netAppTrainerIconUtility );
  virtual~NBRUpperViewRegulation();

public:
  void SetRegulationDrawInfo( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo )
  {
    // @fix GFNMCat[3294]：リセットを呼ばないと渡したレギュレーションで更新されない
    m_NetAppRegulationUpperView.ResetRegulationDrawInfo();
    m_NetAppRegulationUpperView.SetRegulationDrawInfo( pRegulationDrawInfo, NetApp::RegulationSelect::BOOT_MODE_SELECT_P2P );
  }

private:
  NetAppLib::UI::NetAppRegulationUpperView   m_NetAppRegulationUpperView;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_UPPER_VIEW_REGULATION_H_INCLUDE
