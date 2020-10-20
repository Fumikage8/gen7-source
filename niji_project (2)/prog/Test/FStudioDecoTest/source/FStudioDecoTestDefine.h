#if PM_DEBUG

#if !defined(__FINDER_STUDIO_DECO_TEST_DEFINE_H_INCLUDED__)
#define __FINDER_STUDIO_DECO_TEST_DEFINE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FStudioDecoTestDefine.h
 * @date    2016/10/28 15:00:02
 * @author  fukushima_yuya
 * @brief   ファインダースタジオデコテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FSDeco )

//------------------------------------------------------------------------------
// @brief   デコアイテム
//------------------------------------------------------------------------------
static const f32 LOW_CANVAS_SCALE_X = 0.7f;
static const f32 LOW_CANVAS_SCALE_Y = 0.7f;

static const u32 DECO_MAX = 100;  //!< デコアイテムを配置できる個数
static const u32 PASSIVE_ITEM_ID = 0xFFFF;

GFL_NAMESPACE_END( FSDeco )
GFL_NAMESPACE_END( Test )

enum DecoItemKind
{
  DECO_ITEM_KIND_STAMP_S,
  DECO_ITEM_KIND_STAMP_L,
  DECO_ITEM_KIND_TEXT,
  DECO_ITEM_KIND_FRAME,
  DECO_ITEM_KIND_NUM,
  DECO_ITEM_KIND_NULL = DECO_ITEM_KIND_NUM,
};

#define CAST_DECO_TYPE(type) static_cast<DecoItemKind>( type )


#endif // __FINDER_STUDIO_DECO_TEST_DEFINE_H_INCLUDED__

#endif // #if PM_DEBUG
