#if PM_DEBUG

//======================================================================
/**
 * @file FieldDebugDraw.cpp
 * @date 2015/011/27
 * @author saita_kazuki
 * @brief デバッグ用描画
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/Debug/FieldDebugDraw.h"

// gfl2
#include <gfx/include/gfl2_DrawUtil.h>
#include <gfx/include/gfl2_GFGL.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// field_debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
 *  @brief  カメラ構図アシスト描画
 */
void DebugDraw::DrawCameraCompositionAssist(  gfl2::gfx::CtrDisplayNo displayNo)
{
  if( FIELD_DEBUG_SWITCH( DebugTypes::Switch::VISIBLE_CAMERA_COMPOSITION_ASSIST) == false )
  {
    return;
  }

  static const f32 WIDTH_MIN              = 0.0f;
  static const f32 WIDTH_MAX              = 400.0f;
  static const f32 HEIGHT_MIN             = 0.0f;
  static const f32 HEIGHT_MAX             = 240.0f;
  static const f32 LINE_WIDTH             = 3.0f;
  static const gfl2::math::Vector4 COLOR  = gfl2::math::Vector4( 1.0f, 1.0f, 1.0f, 1.0f);
  static const f32 WIDTH_OFFSET           = WIDTH_MAX / 3.0f;
  static const f32 HEIGHT_OFFSET          = HEIGHT_MAX / 3.0f;
  static const f32 LINE_WIDTH_HARF        = LINE_WIDTH / 2.0f;
  static const u32 WIDTH_LINE_MAX         = 2;
  static const u32 HEIGHT_LINE_MAX        = 2;

  switch( displayNo)
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:

    gfl2::gfx::GFGL::BeginScene();
    gfl2::gfx::DrawUtil::BeginRender();

    gfl2::gfx::DrawUtil::SetScreenSize( WIDTH_MAX, HEIGHT_MAX);
    gfl2::gfx::GFGL::SetViewport( 0, 0, WIDTH_MAX, HEIGHT_MAX);

    gfl2::gfx::DrawUtil::SetMaterialAlpha( 1.0f);
    gfl2::gfx::DrawUtil::SetDepthTestEnable( false);

    // 横線
    for( u32 i = 0; i < WIDTH_LINE_MAX; ++i)
    {
      f32 y = HEIGHT_MIN + (HEIGHT_OFFSET * (i+1)) - LINE_WIDTH_HARF;
      gfl2::gfx::DrawUtil::DrawRect( WIDTH_MIN, y, WIDTH_MAX, LINE_WIDTH, COLOR);
    }

    // 縦線
    for( u32 i = 0; i < HEIGHT_LINE_MAX; ++i)
    {
      f32 x = WIDTH_MIN + (WIDTH_OFFSET * (i+1)) - LINE_WIDTH_HARF;
      gfl2::gfx::DrawUtil::DrawRect( x, HEIGHT_MIN, LINE_WIDTH, HEIGHT_MAX, COLOR);
    }

    gfl2::gfx::DrawUtil::EndRender();
    gfl2::gfx::GFGL::EndScene();
    break;

  default:
    // 何もしない
    break;
  }
}

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG

