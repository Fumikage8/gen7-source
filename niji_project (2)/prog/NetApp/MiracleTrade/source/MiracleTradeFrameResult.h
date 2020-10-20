// ============================================================================
/*
 * @file MiracleTradeFrameResult.h
 * @brief ミラクル交換フレームのリザルトコードを定義します。
 * @date 2015.05.25
 */
// ============================================================================
#if !defined( MIRACLETRADEFRAMERESULT_H_INCLUDE )
#define MIRACLETRADEFRAMERESULT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)


enum StartConfirmResultCode
{
  START_CONFIRM_RESULT_BACK,
  START_CONFIRM_RESULT_START,
};


enum MatchingResultCode
{
  MATCHING_RESULT_TRADE_SUCCESS,
  MATCHING_RESULT_TRADE_ERROR,
};


GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEFRAMERESULT_H_INCLUDE
