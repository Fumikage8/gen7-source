// ============================================================================
/*
 * @file gflnet2_CecTypes.h
 * @brief すれ違い通信で使用する型
 * @date 2015.02.16
 */
// ============================================================================
#if !defined( GFLNET2_CECTYPES_H_INCLUDE )
#define GFLNET2_CECTYPES_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)


enum MessageBoxType
{
  RECV_BOX,     // 受信ボックス
  SEND_BOX,     // 送信ボックス
  CORE_BOX      // 本体ボックス
};


GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_CECTYPES_H_INCLUDE
