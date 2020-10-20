// ============================================================================
/*
 * @file EndRollProc.h
 * @brief エンドロールアプリのプロッククラスです。
 * @date 2016.03.16
 * @author uchida_yuto
 * @note
 *      スタッフロール後に呼び出すプロセス
 */
// ============================================================================
#if !defined( APPENDROLL_PROC_H_INCLUDE )
#define APPENDROLL_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "App/EndRoll/include/EndRollAppParam.h"



GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(EndRoll)

GFL_NAMESPACE_BEGIN(System)
class EndRollApplicationSystem;
class EndRollWork;
GFL_NAMESPACE_END(System)


class EndRollProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( EndRollProc );
public:
  EndRollProc( App::EndRoll::APP_PARAM* pAppParam );
  virtual~EndRollProc();

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

  App::EndRoll::APP_PARAM*                                   m_pAppParam;
  App::EndRoll::System::EndRollWork*                  m_pEndRollWork;
  App::EndRoll::System::EndRollApplicationSystem*     m_pApplicationSystem;

};


GFL_NAMESPACE_END(EndRoll)
GFL_NAMESPACE_END(App)

#endif // APPENDROLL_PROC_H_INCLUDE
