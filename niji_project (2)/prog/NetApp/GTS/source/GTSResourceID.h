// ============================================================================
/*
 * @file GTSResourceID.h
 * @brief GTSアプリで使用するリソースのIDです。
 * @date 2015.03.27
 */
// ============================================================================
#if !defined( GTSRESOURCEID_H_INCLUDE )
#define GTSRESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)


enum GTSResourceID
{
  GTS_RESOURCE_ID_LAYOUT_GTS,
  GTS_RESOURCE_ID_LAYOUT_BOX,
  GTS_RESOURCE_ID_MESSAGE_GTS,
  GTS_RESOURCE_ID_MESSAGE_INITIAL,
  GTS_RESOURCE_ID_MESSAGE_BOX,
  GTS_RESOURCE_ID_MENU_CURSOR,
  GTS_RESOURCE_ID_BG_MODEL,
  GTS_RESOURCE_ID_BG_MOTION,
  GTS_RESOURCE_ID_MESSAGE_MONSNAME_JAPAN,
  GTS_RESOURCE_ID_MESSAGE_MONSNAME_ENGLISH,
  GTS_RESOURCE_MAX,
};

GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSRESOURCEID_H_INCLUDE
