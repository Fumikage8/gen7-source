#if PM_DEBUG

//======================================================================
/**
 * @file FieldDebugDraw.h
 * @date 2015/011/27
 * @author saita_kazuki
 * @brief デバッグ用描画
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_DEBUG_FIELD_DEBUG_DRAW_H_INCLUDED__
#define __FIELD_DEBUG_FIELD_DEBUG_DRAW_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
 * @class DebugDraw
 * @brief クラスの説明
 */
class DebugDraw
{
public:

  /**
   *  @brief  カメラ構図アシスト描画
   */
  static void DrawCameraCompositionAssist( gfl2::gfx::CtrDisplayNo displayNo);
  
};

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_DEBUG_FIELD_DEBUG_DRAW_H_INCLUDED__

#endif // PM_DEBUG

