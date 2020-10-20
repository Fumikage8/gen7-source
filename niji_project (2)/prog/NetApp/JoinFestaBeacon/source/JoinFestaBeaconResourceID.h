// ============================================================================
/*
 * @file JoinFestaBeaconResourceID.h
 * @brief ビーコンアプリで使用するリソースのIDです。
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( JOINFESTABEACON_RESOURCEID_H_INCLUDE )
#define JOINFESTABEACON_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)


enum JoinFestaBeaconResourceID
{
  JOINFESTABEACON_RESOURCE_ID_LAYOUT,
  JOINFESTABEACON_RESOURCE_ID_LAYOUT_BG,
  JOINFESTABEACON_RESOURCE_ID_MSG,
  JOINFESTABEACON_RESOURCE_MAX,
};

GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTABEACONRESOURCEID_H_INCLUDE
