// ============================================================================
/*
 * @file ConnectionResourceID.h
 * @brief インターネット接続アプリで使用するリソースのIDです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( CONNECTION_RESOURCEID_H_INCLUDE )
#define CONNECTION_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)


enum ConnectionResourceID
{
  CONNECTION_RESOURCE_ID_LAYOUT,
  CONNECTION_RESOURCE_ID_MESSAGE_CONNECTION,
  CONNECTION_RESOURCE_ID_MESSAGE_FATAL_ERROR,
  CONNECTION_RESOURCE_ID_MENU_CURSOR,
  CONNECTION_RESOURCE_MAX,
};

GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // CONNECTIONRESOURCEID_H_INCLUDE
