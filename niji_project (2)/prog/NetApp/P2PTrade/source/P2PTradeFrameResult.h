// ============================================================================
/*
 * @file P2PTradeFrameResult.h
 * @brief P2P交換アプリのリザルトコードを定義します。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#if !defined( P2PTRADE_FRAMERESULT_H_INCLUDE )
#define P2PTRADE_FRAMERESULT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)


enum WarningFrameResultCode
{
  WARNING_FRAME_RESULT_BACK
};


enum SaveFrameResultCode
{
  SAVE_FRAME_RESULT_BACK
};


enum SyncSaveFrameResultCode
{
  SYNC_SAVE_FRAME_RESULT_BACK
};


GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

#endif // P2PTRADE_FRAMERESULT_H_INCLUDE
