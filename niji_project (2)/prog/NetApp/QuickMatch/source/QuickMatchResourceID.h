// ============================================================================
/*
 * @file QuickMatchResourceID.h
 * @brief クイックマッチアプリで使用するリソースのIDです。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( QUICKMATCH_RESOURCEID_H_INCLUDE )
#define QUICKMATCH_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)


enum QuickMatchResourceID
{
  QUICKMATCH_RESOURCE_ID_LAYOUT,
  QUICKMATCH_RESOURCE_ID_MSG,
  QUICKMATCH_RESOURCE_MAX,
};

GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)

#endif // QUICKMATCHRESOURCEID_H_INCLUDE
