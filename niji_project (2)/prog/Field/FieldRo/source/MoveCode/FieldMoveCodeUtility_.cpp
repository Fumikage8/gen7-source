//======================================================================
/**
 * @file FieldMoveCodeUtility.cpp
 * @date 2015/09/08 15:34:33
 * @author saita_kazuki
 * @brief 使いまわせそうな関数を定義
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeUtility.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( MoveModel )

/**
 * @brief SE再生
 */
void FieldMoveCodeUtility::SafePlaySE( u32 soundID, u32 fadein_frame, s32 controlID, u32 playerID )
{
  if( soundID != SMID_NULL && Sound::IsSEPlaying( soundID) == false)
  {
    Sound::PlaySE( soundID, fadein_frame, controlID, playerID);
  }
}

/**
 * @brief SE停止
 */
void FieldMoveCodeUtility::SafeStopSE( u32 soundID, u32 fadeout_frame, s32 controlID )
{
  if( soundID != SMID_NULL && Sound::IsSEPlaying( soundID) == true)
  {
    Sound::StopSE( soundID, fadeout_frame, controlID);
  }
}

GFL_NAMESPACE_END( MoveModel )
GFL_NAMESPACE_END( Field )
