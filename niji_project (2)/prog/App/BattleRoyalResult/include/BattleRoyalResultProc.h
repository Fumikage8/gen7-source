// ============================================================================
/*
 * @file BattleRoyalResultProc.h
 * @brief バトルロイヤルの結果アプリのプロッククラスです。
 * @date 2015.10.23
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( BATTLE_ROYAL_RESULT_PROC_H_INCLUDE )
#define BATTLE_ROYAL_RESULT_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "App/BattleRoyalResult/include/BattleRoyalResultAppParam.h"



GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)

GFL_NAMESPACE_BEGIN(System)
  class BattleRoyalResultApplicationSystem;
  class BattleRoyalResultWork;
GFL_NAMESPACE_END(System)

  class BattleRoyalResultProc : public GameSys::GameProc
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleRoyalResultProc );
  public:
    BattleRoyalResultProc( App::BattleRoyalResult::APP_PARAM* pAppParam );
    virtual~BattleRoyalResultProc();

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

    App::BattleRoyalResult::APP_PARAM*                                      m_pAppParam;
    App::BattleRoyalResult::System::BattleRoyalResultWork*                  m_pBattleRoyalResultWork;
    App::BattleRoyalResult::System::BattleRoyalResultApplicationSystem*     m_pApplicationSystem;

  };


GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)

#endif // BATTLE_ROYAL_RESULT_PROC_H_INCLUDE
