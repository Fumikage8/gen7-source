// ============================================================================
/*
 * @file KisekaeResourceID.h
 * @brief きせかえアプリで使用するリソースのIDです。
 * @date  2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( KISEKAE_RESOURCEID_H_INCLUDE )
#define KISEKAE_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)

enum ResourceID
{
  RESOURCE_ID_LAYOUT,
  RESOURCE_ID_MESSAGE,
  RESOURCE_ID_MESSAGE_DRESS_ITEM,
  RESOURCE_ID_MESSAGE_DRESS_COLOR,
  RESOURCE_ID_TIMER_ICON_LYT,
  RESOURCE_MAX,
};

GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)

#endif // NETAPPSKELETONRESOURCEID_H_INCLUDE
