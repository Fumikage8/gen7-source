//======================================================================
/**
 * @file FieldLayoutPriority.h
 * @date 2015/08/28 16:10:22
 * @author miyachi_soichi
 * @brief フィールド用レイアウト描画優先度
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_LAYOUT_PRIORITY_H_INCLUDED__
#define __FIELD_LAYOUT_PRIORITY_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(Field)

/**
 * @enum FieldLayoutPriority
 * レイアウト描画優先度
 */
enum FieldLayoutPriority
{
  FIELD_LAYOUT_PRIORITY_3D = 32,        //!< 3D直後：フェードで隠れる
  FIELD_LAYOUT_PRIORITY_FADE = 64,      //!< フェード
  FIELD_LAYOUT_PRIORITY_NORMAL = 128,   //!< 通常描画
  FIELD_LAYOUT_PRIORITY_FRONT = 255,    //!< 最前面
};

GFL_NAMESPACE_END(Field)

#endif // __FIELD_LAYOUT_PRIORITY_H_INCLUDED__
