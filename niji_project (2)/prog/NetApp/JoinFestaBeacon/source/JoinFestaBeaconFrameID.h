// ============================================================================
/*
 * @file JoinFestaBeaconFrameID.h
 * @brief ビーコンアプリのフレームIDを定義します。
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( JOINFESTABEACON_FRAMEID_H_INCLUDE )
#define JOINFESTABEACON_FRAMEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)


enum JoinFestaBeaconFrameID
{
  JOINFESTABEACON_FRAME_ID_INVITE_SINGLE = 0,
  JOINFESTABEACON_FRAME_ID_WAIT_INVITED,
};

GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTABEACON_FRAMEID_H_INCLUDE
