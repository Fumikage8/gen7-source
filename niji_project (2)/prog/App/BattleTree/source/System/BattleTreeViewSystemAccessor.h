// ============================================================================
/*
 * @file BattleTreeViewSystemAccessor.h
 * @brief バトルツリーアプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#if !defined( BATTLE_TREE_VIEWSYSTEMACCESSOR_H_INCLUDE )
#define BATTLE_TREE_VIEWSYSTEMACCESSOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

//  View関連のインクルード
#include "App/BattleTree/source/System/BattleTreeViewObjects.h"
#include "App/BattleTree/source/System/BattleTreeViewList.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(System)

  class ViewSystemAccessor
  {
    GFL_FORBID_COPY_AND_ASSIGN( ViewSystemAccessor );

  public:

    ViewSystemAccessor(
      App::BattleTree::System::ViewList* pViewList,
      App::BattleTree::System::ViewObjects* pViewObjects );

    virtual~ViewSystemAccessor();

    App::BattleTree::System::ViewList*  GetViewList() { return m_pViewList; }
    App::BattleTree::System::ViewObjects*  GetViewObjects() { return m_pViewObjects; }

  private:

    App::BattleTree::System::ViewList*     m_pViewList;
    App::BattleTree::System::ViewObjects*  m_pViewObjects;
  };

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)


#endif // BATTLE_TREE_VIEWSYSTEMACCESSOR_H_INCLUDE
