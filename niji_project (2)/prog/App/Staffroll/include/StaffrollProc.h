// ============================================================================
/*
 * @file StaffrollProc.h
 * @brief スタッフロールアプリのプロッククラスです。
 * @date 2015.10.23
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( STAFFROLL_PROC_H_INCLUDE )
#define STAFFROLL_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)

GFL_NAMESPACE_BEGIN(System)
  class StaffrollApplicationSystem;
  class StaffrollWork;
GFL_NAMESPACE_END(System)

  class StaffrollProc : public GameSys::GameProc
  {
    GFL_FORBID_COPY_AND_ASSIGN( StaffrollProc );
  public:
    StaffrollProc();
    virtual~StaffrollProc();

    //  初期化パラメータ
    void SetupParam( const u8 version, const bool bFirstEvent );

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

    void _Clear()
    {
      m_bFirstEvent = true;
      m_version     = 0;
      m_pStaffrollWork  = NULL;
      m_pApplicationSystem  = NULL;
    }

    App::Staffroll::System::StaffrollWork*                  m_pStaffrollWork;
    App::Staffroll::System::StaffrollApplicationSystem*     m_pApplicationSystem;

    bool  m_bFirstEvent;
    u8    m_version;
  };


GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)

#endif // STAFFROLL_PROC_H_INCLUDE
