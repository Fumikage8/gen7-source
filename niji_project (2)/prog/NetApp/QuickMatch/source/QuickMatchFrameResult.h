// ============================================================================
/*
 * @file QuickMatchFrameResult.h
 * @brief クイックマッチアプリのリザルトコードを定義します。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( QUICKMATCH_FRAMERESULT_H_INCLUDE )
#define QUICKMATCH_FRAMERESULT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)


enum SampleResultCode
{
  RESULT_START_HOST,
  RESULT_START_CLIENT,
  RESULT_CANCEL_EXIT,
  RESULT_EXIT_APP
};


GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)

#endif // QUICKMATCH_FRAMERESULT_H_INCLUDE
