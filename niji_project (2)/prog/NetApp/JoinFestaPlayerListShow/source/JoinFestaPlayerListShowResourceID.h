// ============================================================================
/*
 * @file JoinFestaPlayerListShowResourceID.h
 * @brief 閲覧アプリで使用するリソースのIDです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSHOW_RESOURCEID_H_INCLUDE )
#define JOINFESTAPLAYERLISTSHOW_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)


enum JoinFestaPlayerListShowResourceID
{
  JOINFESTAPLAYERLISTSHOW_RESOURCE_ID_LAYOUT,
  JOINFESTAPLAYERLISTSHOW_RESOURCE_ID_MESSAGE_JOINFESTA_PLAYER_LIST,
  JOINFESTAPLAYERLISTSHOW_RESOURCE_ID_MESSAGE_JOINFESTA_QUESTION_DATA,
  JOINFESTAPLAYERLISTSHOW_RESOURCE_ID_MENU_CURSOR,
  JOINFESTAPLAYERLISTSHOW_RESOURCE_MAX,
};

GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSHOWRESOURCEID_H_INCLUDE
