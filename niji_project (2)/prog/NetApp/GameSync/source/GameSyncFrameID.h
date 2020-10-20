// ============================================================================
/*
 * @file GameSyncFrameID.h
 * @brief ゲームシンクアプリのフレームIDを定義します。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNC_FRAMEID_H_INCLUDE )
#define GAMESYNC_FRAMEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)


enum GameSyncFrameID
{
  GAMESYNC_FRAME_ID_MAIN,
  GAMESYNC_FRAME_ID_DESCRIPTION
};

GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

#endif // GAMESYNC_FRAMEID_H_INCLUDE
