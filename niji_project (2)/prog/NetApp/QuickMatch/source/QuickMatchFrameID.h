// ============================================================================
/*
 * @file QuickMatchFrameID.h
 * @brief クイックマッチアプリのフレームIDを定義します。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( QUICKMATCH_FRAMEID_H_INCLUDE )
#define QUICKMATCH_FRAMEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)


enum QuickMatchFrameID
{
  QUICKMATCH_FRAME_ID_INIT,
  QUICKMATCH_FRAME_ID_HOST,
  QUICKMATCH_FRAME_ID_CLIENT,
  QUICKMATCH_FRAME_ID_CANCEL_EXIT,
  QUICKMATCH_FRAME_ID_BATTLE_TREE
};

GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)

#endif // QUICKMATCH_FRAMEID_H_INCLUDE
