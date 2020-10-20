// ============================================================================
/*
 * @file RegulationSelectFrameFactory.h
 * @brief レギュレーション選択アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONSELECT_FRAMEFACTORY_H_INCLUDE )
#define REGULATIONSELECT_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


class RegulationSelectViewObjects;


class RegulationSelectFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationSelectFrameFactory );
public:
  RegulationSelectFrameFactory( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork, NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects* pRegulationSelectViewObjects );
  virtual~RegulationSelectFrameFactory();

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork*            m_pRegulationSelectWork;
  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects*     m_pRegulationSelectViewObjects;

};


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONSELECT_FRAMEFACTORY_H_INCLUDE
