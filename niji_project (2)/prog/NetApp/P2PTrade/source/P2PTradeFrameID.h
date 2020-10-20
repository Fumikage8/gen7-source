// ============================================================================
/*
 * @file P2PTradeFrameID.h
 * @brief P2P交換アプリのフレームIDを定義します。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#if !defined( P2PTRADE_FRAMEID_H_INCLUDE )
#define P2PTRADE_FRAMEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)


enum P2PTradeFrameID
{
  P2PTRADE_FRAME_ID_WARNING,
  P2PTRADE_FRAME_ID_SYNC_SAVE,
  P2PTRADE_FRAME_ID_SAVE
};

GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

#endif // P2PTRADE_FRAMEID_H_INCLUDE
