//=================================================================================================
/**
*  @file FieldMoveCodeGimmickEncount_define.h
*  @brief ギミックエンカウント動作コードの共通定義
*  @author saita_kazuki
*  @date 2015.09.05
*/
//=================================================================================================

#if !defined __FIELD_MOVE_CODE_GIMMICK_ENCOUNT_DEFINE_H_INCLUDED__
#define __FIELD_MOVE_CODE_GIMMICK_ENCOUNT_DEFINE_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( MoveModel )

/**
 *  @brief モーションインデックス
 */
enum MotionIndex
{
  MOTION_INDEX_WAIT,   ///< 待機時
  MOTION_INDEX_MOVE,   ///< 移動時
};

GFL_NAMESPACE_END( MoveModel )
GFL_NAMESPACE_END( Field )

#endif // __FIELD_MOVE_CODE_GIMMICK_ENCOUNT_DEFINE_H_INCLUDED__
