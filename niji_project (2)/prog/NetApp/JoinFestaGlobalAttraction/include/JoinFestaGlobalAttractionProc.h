// ============================================================================
/*
 * @file JoinFestaGlobalAttractionProc.h
 * @brief グローバルアトラクションアプリのプロッククラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAGLOBALATTRACTION_PROC_H_INCLUDE )
#define JOINFESTAGLOBALATTRACTION_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "NetApp/JoinFestaGlobalAttraction/include/JoinFestaGlobalAttractionAppParam.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)

GFL_NAMESPACE_BEGIN(System)
class JoinFestaGlobalAttractionApplicationSystem;
class JoinFestaGlobalAttractionWork;
GFL_NAMESPACE_END(System)


class JoinFestaGlobalAttractionProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaGlobalAttractionProc );
public:
  JoinFestaGlobalAttractionProc( NetApp::JoinFestaGlobalAttraction::APP_PARAM* pAppParam );
  virtual~JoinFestaGlobalAttractionProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pProcManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* pProcManager );
  virtual void DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo );

private:

  enum
  {
    INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM,
    INIT_SEQUENCE_RESOURCE_LOADING,
  };

  NetApp::JoinFestaGlobalAttraction::APP_PARAM                                         m_AppParam;
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork*                  m_pJoinFestaGlobalAttractionWork;
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionApplicationSystem*     m_pApplicationSystem;

};


GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAGLOBALATTRACTION_PROC_H_INCLUDE
