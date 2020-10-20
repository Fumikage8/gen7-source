// ============================================================================
/*
 * @file P2PTradeResourceID.h
 * @brief P2P交換アプリで使用するリソースのIDです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#if !defined( P2PTRADE_RESOURCEID_H_INCLUDE )
#define P2PTRADE_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)


enum P2PTradeResourceID
{
  P2PTRADE_RESOURCE_ID_LAYOUT_CONNECTION,
  P2PTRADE_RESOURCE_ID_MESSAGE_NET_SAVE,
  P2PTRADE_RESOURCE_MAX
};

GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

#endif // P2PTRADERESOURCEID_H_INCLUDE
