// ============================================================================
/*
 * @file JoinFestaOpenListResourceID.h
 * @brief 開催選択アプリで使用するリソースのIDです。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAOPENLIST_RESOURCEID_H_INCLUDE )
#define JOINFESTAOPENLIST_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)


enum JoinFestaOpenListResourceID
{
  JOINFESTAOPENLIST_RESOURCE_ID_LAYOUT,
  JOINFESTAOPENLIST_RESOURCE_ID_MESSAGE_JOINFESTA_PLAYER_LIST,
  JOINFESTAOPENLIST_RESOURCE_ID_MESSAGE_JOINFESTA_ATTRACTION,
  JOINFESTAOPENLIST_RESOURCE_ID_MENU_CURSOR,
  JOINFESTAOPENLIST_RESOURCE_MAX,
};

GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAOPENLISTRESOURCEID_H_INCLUDE
