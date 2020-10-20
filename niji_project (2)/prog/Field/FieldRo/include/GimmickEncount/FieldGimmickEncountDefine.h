//======================================================================
/**
 * @file FieldGimmickEncountDefine.h
 * @date 2015/08/20 15:32:43
 * @author saita_kazuki
 * @brief ギミックエンカウントの外部参照もする定義
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_GIMMICK_ENCOUNT_DEFINE_H_INCLUDED__
#define __FIELD_GIMMICK_ENCOUNT_DEFINE_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( GimmickEncount )

// ギミックエンカウント型
enum GimmickType
{
  GIMMICK_TYPE_STANDBY,   ///< 待機型
  GIMMICK_TYPE_CHASE,     ///< 接近型
  GIMMICK_TYPE_PATROL,    ///< 巡回型
  GIMMICK_TYPE_ESCAPE,    ///< 逃走型

  GIMMICK_TYPE_MAX,       ///< 最大値
  GIMMICK_TYPE_INVALID,   ///< 不正値
};

// イベント通知状態
enum NotifyState
{
  NOTIFY_STATE_NONE,                          ///< なし
  NOTIFY_STATE_ACTION_COLLISION_HIT,          ///< アクション用コリジョンに当たった
  NOTIFY_STATE_ACTION_COLLISION_2_HIT,        ///< アクション用コリジョン2に当たった
  NOTIFY_STATE_ENCOUNT_COLLISION_HIT,         ///< エンカウント用コリジョンに当たった

  NOTIFY_STATE_UNUSE,                         ///< 未使用状態
};

GFL_NAMESPACE_END( GimmickEncount )
GFL_NAMESPACE_END( Field )

#endif // __FIELD_GIMMICK_ENCOUNT_DEFINE_H_INCLUDED__
