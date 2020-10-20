// ============================================================================
/*
 * @file GameSyncFrameResult.h
 * @brief ゲームシンクアプリのリザルトコードを定義します。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNC_FRAMERESULT_H_INCLUDE )
#define GAMESYNC_FRAMERESULT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)


enum MainFrameResultCode
{
  MAIN_FRAME_RESULT_BACK,
  MAIN_FRAME_RESULT_GO_TO_DESCRIPTION
};

enum DescriptionFrameResultCode
{
  DESCRIPTION_FRAME_RESULT_BACK
};


GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

#endif // GAMESYNC_FRAMERESULT_H_INCLUDE
