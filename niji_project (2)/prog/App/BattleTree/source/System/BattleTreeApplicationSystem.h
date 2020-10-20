// ============================================================================
/*
 * @file BattleTreeApplicationSystem.h
 * @brief バトルツリーアプリのアプリケーションシステムです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#if !defined( BATTLE_TREE_APPLICATIONSYSTEM_H_INCLUDE )
#define BATTLE_TREE_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(System)

  //  前方宣言
  class BattleTreeWork;
  class BattleTreeBuilder;
  class ViewList;
  class ViewObjects;
  class ViewSystemAccessor;

  class BattleTreeApplicationSystem : public NetAppLib::System::ApplicationSystemBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleTreeApplicationSystem );

  public:
    BattleTreeApplicationSystem();
    virtual~BattleTreeApplicationSystem();

    void Build( App::BattleTree::System::BattleTreeWork* pBattleTreeWork, App::BattleTree::System::BattleTreeBuilder* pApplicationSystemBuilder );

    //! @brief ローディング処理を行います。
    //! @retval true ロード中
    //! @retval false ロード完了
    virtual bool Loading();

    //! @brief アプリケーションの終了処理を行います。
    //! @retval true 終了処理中
    //! @retval false 終了処理完了
    virtual bool End();

  private:
    virtual void LoadResourceRequest();
    virtual void CreateMessageData();
    virtual void CreateView();
    virtual void UpdateView();
    virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
    virtual bool IsExitView();

    //! @brief ビューの作成が完了したか
    virtual bool IsCreateViewComplete();

  private:

    void _Clear()
    {
      m_pWork                 = NULL;
      m_pViewList             = NULL;
      m_pViewObjects          = NULL;
      m_pViewSystemAccessor   = NULL;

      m_endSeq  = 0;
    }

    App::BattleTree::System::BattleTreeWork*         m_pWork;
    App::BattleTree::System::ViewList*               m_pViewList;
    App::BattleTree::System::ViewObjects*            m_pViewObjects;
    App::BattleTree::System::ViewSystemAccessor*     m_pViewSystemAccessor;

    u32 m_endSeq;
  };


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)


#endif // BATTLE_TREE_APPLICATIONSYSTEM_H_INCLUDE
