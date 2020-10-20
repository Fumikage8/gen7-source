// ============================================================================
/*
 * @file BattleTreeViewSystemAccessor.h
 * @brief バトルツリーアプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#include "App/BattleTree/source/System/BattleTreeViewSystemAccessor.h"

//  gflib2のインクルード
#include <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(System)


  ViewSystemAccessor::ViewSystemAccessor(
    App::BattleTree::System::ViewList* pViewList,
    App::BattleTree::System::ViewObjects* pViewObjects )
  {
    GFL_ASSERT( pViewList );
    GFL_ASSERT( pViewObjects );

    m_pViewList     = pViewList;
    m_pViewObjects  = pViewObjects;
  }


  ViewSystemAccessor::~ViewSystemAccessor()
  {
  }

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)
