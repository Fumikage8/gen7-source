// ============================================================================
/*
 * @file KisekaeProc.h
 * @brief きせかえアプリのプロッククラスです。
 * @date 2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( KISEKAE_PROC_H_INCLUDE )
#define KISEKAE_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "App/Kisekae/include/KisekaeAppParam.h"



GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)

GFL_NAMESPACE_BEGIN(System)
class ApplicationSystem;
class Work;
GFL_NAMESPACE_END(System)


class Proc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( Proc );
public:
  Proc( App::Kisekae::APP_PARAM* pAppParam );
  virtual~Proc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pProcManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
  virtual void DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo );

private:

  enum
  {
    INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM,
    INIT_SEQUENCE_RESOURCE_LOADING,
  };

  App::Kisekae::APP_PARAM*                     m_pAppParam;
  App::Kisekae::System::Work*                  m_pWork;
  App::Kisekae::System::ApplicationSystem*     m_pApplicationSystem;

};


GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)

#endif // KISEKAE_PROC_H_INCLUDE
