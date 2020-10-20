// ============================================================================
/*
 * @file gflnet2_CecResultCode.h
 * @brief すれ違い通信のリザルトコードです
 * @date 2015.02.16
 */
// ============================================================================
#if !defined( GFLNET2_CECRESULTCODE_H_INCLUDE )
#define GFLNET2_CECRESULTCODE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)


enum ResultCode
{
  SUCCESS,      // 成功
  FAILURE       // 失敗
};


GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_CECRESULTCODE_H_INCLUDE
