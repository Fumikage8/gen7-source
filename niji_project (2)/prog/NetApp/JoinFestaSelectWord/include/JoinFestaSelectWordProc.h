// ============================================================================
/*
 * @file JoinFestaSelectWordProc.h
 * @brief 挨拶選択アプリのプロッククラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORD_PROC_H_INCLUDE )
#define JOINFESTASELECTWORD_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "NetApp/JoinFestaSelectWord/include/JoinFestaSelectWordAppParam.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)

GFL_NAMESPACE_BEGIN(System)
class JoinFestaSelectWordApplicationSystem;
class JoinFestaSelectWordWork;
GFL_NAMESPACE_END(System)


class JoinFestaSelectWordProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSelectWordProc );
public:
  JoinFestaSelectWordProc( NetApp::JoinFestaSelectWord::APP_PARAM* pAppParam );
  virtual~JoinFestaSelectWordProc();

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

  NetApp::JoinFestaSelectWord::APP_PARAM                                         m_AppParam;
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork*                  m_pJoinFestaSelectWordWork;
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordApplicationSystem*     m_pApplicationSystem;

};


GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASELECTWORD_PROC_H_INCLUDE
