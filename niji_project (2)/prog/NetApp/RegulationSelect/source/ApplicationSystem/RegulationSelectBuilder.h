// ============================================================================
/*
 * @file RegulationSelectBuilder.h
 * @brief レギュレーション選択アプリを構築するクラスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONSELECT_BUILDER_H_INCLUDE )
#define REGULATIONSELECT_BUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(ApplicationSystem)

class RegulationSelectViewObjects;


class RegulationSelectBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationSelectBuilder );
public:
  RegulationSelectBuilder( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork );
  virtual~RegulationSelectBuilder();

  void SetDrawManager( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects* pRegulationSelectViewObjects ) { m_pRegulationSelectViewObjects = pRegulationSelectViewObjects; }

  virtual NetAppLib::System::IApplicationFrameSwitcher* CreateApplicationFrameSwitcher();
  virtual NetAppLib::System::IApplicationFrameFactory* CreateApplicationFrameFactory();

  virtual NetAppLib::System::ApplicationWorkBase* GetApplicationWork();
  virtual NetAppLib::System::FrameID GetApplicationStartFrameID();
  virtual u32 GetApplicationResourceCount();

  virtual bool IsCreateMessageUtility();
  virtual void GetMessageUtilityParameter( u32& dataNum, u32& wordMax, u32& bufLen );
  virtual bool IsCreateNetAppMessageMenu();

  virtual bool IsEnableInputExclusiveKeyMode();

private:
  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork*            m_pRegulationSelectWork;
  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects*     m_pRegulationSelectViewObjects;

};


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEBUILDER_H_INCLUDE
