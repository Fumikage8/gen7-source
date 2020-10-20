// ============================================================================
/*
 * @file BattleRoyalResultApplicationSystem.h
 * @brief バトルロイヤルの結果アプリのアプリケーションシステムです。
 * @date 2015.10.23
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( BATTLE_ROYAL_RESULT_APPLICATIONSYSTEM_H_INCLUDE )
#define BATTLE_ROYAL_RESULT_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)
GFL_NAMESPACE_BEGIN(System)

  //  前方宣言
  class BattleRoyalResultWork;
  class BattleRoyalResultBuilder;
  class BattleRoyalResultViewList;
  class BattleRoyalResultViewObjects;
  class BattleRoyalResultViewSystemAccessor;

  class BattleRoyalResultApplicationSystem : public NetAppLib::System::ApplicationSystemBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleRoyalResultApplicationSystem );

  public:
    BattleRoyalResultApplicationSystem();
    virtual~BattleRoyalResultApplicationSystem();

    void Build( App::BattleRoyalResult::System::BattleRoyalResultWork* pBattleRoyalResultWork, App::BattleRoyalResult::System::BattleRoyalResultBuilder* pApplicationSystemBuilder );

    virtual bool End();

  private:
    virtual void LoadResourceRequest();
    virtual void CreateMessageData();
    virtual void CreateView();
    virtual void UpdateView();
    virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
    virtual bool IsExitView();
    virtual bool IsCreateViewComplete();

  private:

    void _Clear()
    {
      m_pWork = NULL;
      m_pViewList = NULL;
      m_pViewObjects = NULL;
      m_pViewSystemAccessor = NULL;

      m_initSeq = m_endSeq  = 0;
    }

    App::BattleRoyalResult::System::BattleRoyalResultWork*                   m_pWork;
    App::BattleRoyalResult::System::BattleRoyalResultViewList*               m_pViewList;
    App::BattleRoyalResult::System::BattleRoyalResultViewObjects*            m_pViewObjects;
    App::BattleRoyalResult::System::BattleRoyalResultViewSystemAccessor*     m_pViewSystemAccessor;

    u32 m_initSeq, m_endSeq;
  };

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)


#endif // BATTLE_ROYAL_RESULT_APPLICATIONSYSTEM_H_INCLUDE
