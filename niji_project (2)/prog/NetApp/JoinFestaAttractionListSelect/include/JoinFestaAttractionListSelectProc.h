// ============================================================================
/*
 * @file JoinFestaAttractionListSelectProc.h
 * @brief アトラクション開催参加アプリのプロッククラスです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONLISTSELECT_PROC_H_INCLUDE )
#define JOINFESTAATTRACTIONLISTSELECT_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "NetApp/JoinFestaAttractionListSelect/include/JoinFestaAttractionListSelectAppParam.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)

GFL_NAMESPACE_BEGIN(System)
class JoinFestaAttractionListSelectApplicationSystem;
class JoinFestaAttractionListSelectWork;
GFL_NAMESPACE_END(System)


class JoinFestaAttractionListSelectProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionListSelectProc );
public:
  JoinFestaAttractionListSelectProc( NetApp::JoinFestaAttractionListSelect::APP_PARAM* pAppParam );
  virtual~JoinFestaAttractionListSelectProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pProcManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* pProcManager );
  virtual void DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo );

  void SetAppResult( NetApp::JoinFestaAttractionListSelect::APP_RESULT* pAppResult ){ m_pAppResult = pAppResult; }

private:

  enum
  {
    INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM,
    INIT_SEQUENCE_RESOURCE_LOADING,
  };

  NetApp::JoinFestaAttractionListSelect::APP_PARAM                                                   m_AppParam;
  NetApp::JoinFestaAttractionListSelect::APP_RESULT*                                                 m_pAppResult;
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectWork*                  m_pJoinFestaAttractionListSelectWork;
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectApplicationSystem*     m_pApplicationSystem;

};


GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAATTRACTIONLISTSELECT_PROC_H_INCLUDE
