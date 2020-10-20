// ============================================================================
/*
 * @file JoinFestaBeaconFrameResult.h
 * @brief ビーコンアプリのリザルトコードを定義します。
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( JOINFESTABEACON_FRAMERESULT_H_INCLUDE )
#define JOINFESTABEACON_FRAMERESULT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)


enum ResultCode
{
  RESULT_CONNECT_SUCCESS = 0,
  RESULT_ERROR,
  RESULT_CONNECT_NG,
  RESULT_CONNECT_CANCEL,
};


GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTABEACON_FRAMERESULT_H_INCLUDE
