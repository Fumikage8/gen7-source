// ============================================================================
/*
 * @file MiracleTradeFrameID.h
 * @brief ミラクル交換フレームのIDを定義します。
 * @date 2015.05.25
 */
// ============================================================================
#if !defined( MIRACLETRADEFRAMEID_H_INCLUDE )
#define MIRACLETRADEFRAMEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)


enum MiracleTradeFrameID
{
  MIRACLETRADE_FRAME_ID_START_CONFIRM,
  MIRACLETRADE_FRAME_ID_MATCHING,
};

GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEFRAMEID_H_INCLUDE
