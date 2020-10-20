//======================================================================
/**
 * @file BattleUIViewDrawOrder.h
 * @date 2015/08/26 16:08:00
 * @author uchida_yuto
 * @brief バトルUIの描画優先順位定義
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BATTLE_UI_VIEW_DRAW_ORDER_H_INCLUDED__
#define __BATTLE_UI_VIEW_DRAW_ORDER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <Layout/include/gfl2_LayoutRenderPath.h>

GFL_NAMESPACE_BEGIN(btl)
GFL_NAMESPACE_BEGIN(app)

  //  下画面の描画優先割り当て
  enum
  {
    LOW_VIEW_ORDER_BG           = gfl2::lyt::ORDER_NORMAL - 10,
    LOW_VIEW_ORDER_TOP,
    LOW_VIEW_ORDER_BG_FRONT,
    LOW_VIEW_ORDER_OTHER        = gfl2::lyt::ORDER_NORMAL
  };

GFL_NAMESPACE_END(app)
GFL_NAMESPACE_END(btl)

#endif // __BATTLE_UI_VIEW_DRAW_ORDER_H_INCLUDED__