// ============================================================================
/*
 * @file SuperTrainingProc.h
 * @brief すごい特訓アプリのプロッククラスです。
 * @date 2015.12.04
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( SUPER_TRAINIG_PROC_H_INCLUDE )
#define SUPER_TRAINIG_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "App/SuperTraining/include/SuperTrainingAppParam.h"



GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)

GFL_NAMESPACE_BEGIN(System)
class SuperTrainingApplicationSystem;
class SuperTrainingWork;
GFL_NAMESPACE_END(System)


class SuperTrainingProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( SuperTrainingProc );
public:
  SuperTrainingProc( App::SuperTraining::APP_PARAM* pAppParam );
  virtual~SuperTrainingProc();

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

  App::SuperTraining::APP_PARAM*                                   m_pAppParam;
  App::SuperTraining::System::SuperTrainingWork*                  m_pSuperTrainingWork;
  App::SuperTraining::System::SuperTrainingApplicationSystem*     m_pApplicationSystem;

};


GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)

#endif // SUPER_TRAINIG_PROC_H_INCLUDE
