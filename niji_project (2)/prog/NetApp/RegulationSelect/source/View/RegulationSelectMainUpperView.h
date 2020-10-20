// ============================================================================
/*
 * @file RegulationSelectMainUpperView.h
 * @brief レギュレーション選択の上画面を表示するクラスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONSELECTMAINUPPERVIEW_H_INCLUDE )
#define REGULATIONSELECTMAINUPPERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"
#include "NetStatic/NetAppLib/include/UI/NetAppRegulationUpperView.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Draw)


class RegulationSelectMainUpperView :
  public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationSelectMainUpperView );

public:

  RegulationSelectMainUpperView( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork );
  virtual~RegulationSelectMainUpperView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo ){}

  void SetRegulationDrawInfo( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo );
  void ResetRegulationDrawInfo();

private:
  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork*  m_pRegulationSelectWork;
  NetAppLib::UI::NetAppRegulationUpperView                            m_NetAppRegulationUpperView;

};


GFL_NAMESPACE_END(Draw)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONSELECTMAINUPPERVIEW_H_INCLUDE
