// ============================================================================
/*
 * @file LowerBGView.h
 * @brief バトルツリーアプリの下画面を表示するクラスです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#if !defined( BATTLE_TREE_LOWER_BG_VIEW_H_INCLUDE )
#define BATTLE_TREE_LOWER_BG_VIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"

#include "App/BattleTree/source/System/BattleTreeWork.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(View)

  class LowerBGView :
    public NetAppLib::System::NetApplicationViewBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( LowerBGView );
  public:
    LowerBGView( App::BattleTree::System::BattleTreeWork* pConnectionWork );
    virtual~LowerBGView();

    // UIView
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  private:
    enum LayoutWorkID
    {
      LAYOUT_WORK_ID,
    };

    void _Clear()
    {
      m_pWork                   = NULL;
    }

    App::BattleTree::System::BattleTreeWork*     m_pWork;
  };

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)

#endif // BATTLE_TREE_LOWER_BG_VIEW_H_INCLUDE
