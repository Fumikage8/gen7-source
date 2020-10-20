//======================================================================
/**
 * @file FieldMoveCodeUtility.h
 * @date 2015/09/08 15:34:33
 * @author saita_kazuki
 * @brief 使いまわせそうな関数を定義
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_MOVE_CODE_UTILITY_H_INCLUDED__
#define __FIELD_MOVE_CODE_UTILITY_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( MoveModel )

class FieldMoveCodeUtility
{
public:

  /**
   *  @brief  コンストラクタ
   */
  FieldMoveCodeUtility( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldMoveCodeUtility( void );

  /**
   * @brief SE再生
   */
  static void SafePlaySE( u32 soundID, u32 fadein_frame = 0, s32 controlID = Sound::NOT_CONTROL, u32 playerID = Sound::DEFAULT_PLAYER );

  /**
   * @brief SE停止
   */
  static void SafeStopSE( u32 soundID, u32 fadeout_frame = 0, s32 controlID = Sound::NOT_CONTROL );
};

GFL_NAMESPACE_END( MoveModel )
GFL_NAMESPACE_END( Field )

#endif // __FIELD_MOVE_CODE_UTILITY_H_INCLUDED__