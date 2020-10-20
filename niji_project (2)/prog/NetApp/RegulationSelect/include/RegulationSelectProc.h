// ============================================================================
/*
 * @file RegulationSelectProc.h
 * @brief レギュレーション選択アプリのプロッククラスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONSELECT_PROC_H_INCLUDE )
#define REGULATIONSELECT_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "NetApp/RegulationSelect/include/RegulationSelectAppParam.h"


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
class RegulationSelectApplicationSystem;
GFL_NAMESPACE_END(ApplicationSystem)


class RegulationSelectProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationSelectProc );

public:

  RegulationSelectProc( NetApp::RegulationSelect::APP_PARAM* pAppParam );
  virtual~RegulationSelectProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pProcManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* pProcManager );
  virtual void DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo );

  void SetAppResult( NetApp::RegulationSelect::APP_RESULT* pAppResult ){ m_pAppResult = pAppResult; }
  void SetAppResultLocal( NetApp::RegulationSelect::APP_RESULT_LOCAL* pAppResultLocal ){ m_pAppResultLocal = pAppResultLocal; }

private:

  enum
  {
    INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM,
    INIT_SEQUENCE_RESOURCE_LOADING,
  };

  NetApp::RegulationSelect::APP_PARAM*                                                m_pAppParam;
  NetApp::RegulationSelect::APP_RESULT*                                               m_pAppResult;
  NetApp::RegulationSelect::APP_RESULT_LOCAL*                                         m_pAppResultLocal;
  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork*                  m_pRegulationSelectWork;
  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectApplicationSystem*     m_pApplicationSystem;

};


GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONSELECT_PROC_H_INCLUDE
