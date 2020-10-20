// ============================================================================
/*
 * @file RegulationSelectApplicationSystem.h
 * @brief レギュレーション選択アプリのアプリケーションシステムです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONSELECT_APPLICATIONSYSTEM_H_INCLUDE )
#define REGULATIONSELECT_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)
class NetAppEmptyView;
GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(ApplicationSystem)
class RegulationSelectWork;
GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(ApplicationSystem)

class RegulationSelectBuilder;
class RegulationSelectViewObjects;

class RegulationSelectApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationSelectApplicationSystem );

public:
  RegulationSelectApplicationSystem();
  virtual~RegulationSelectApplicationSystem();

  void Build( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork, NetApp::RegulationSelect::ApplicationSystem::RegulationSelectBuilder* pApplicationSystemBuilder );


private:
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();

private:

  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork*                  m_pRegulationSelectWork;
  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects*           m_pRegulationSelectViewObjects;
  NetAppLib::UI::NetAppEmptyView*                                                     m_pRootUpperView;
  NetAppLib::UI::NetAppEmptyView*                                                     m_pRootLowerView;

};


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)


#endif // REGULATIONSELECT_APPLICATIONSYSTEM_H_INCLUDE
