// ============================================================================
/*
 * @file GameSyncResourceID.h
 * @brief ゲームシンクアプリで使用するリソースのIDです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNC_RESOURCEID_H_INCLUDE )
#define GAMESYNC_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)


enum GameSyncResourceID
{
  GAMESYNC_RESOURCE_ID_LAYOUT,
  GAMESYNC_RESOURCE_ID_MESSAGE_GAMESYNC,
  GAMESYNC_RESOURCE_ID_MENU_CURSOR,
  GAMESYNC_RESOURCE_MAX,
};

GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

#endif // GAMESYNCRESOURCEID_H_INCLUDE
