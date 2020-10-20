// ============================================================================
/*
 * @file JoinFestaPlayerDetailResourceID.h
 * @brief 詳細アプリで使用するリソースのIDです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERDETAIL_RESOURCEID_H_INCLUDE )
#define JOINFESTAPLAYERDETAIL_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)


enum JoinFestaPlayerDetailResourceID
{
  JOINFESTAPLAYERDETAIL_RESOURCE_ID_LAYOUT,
  JOINFESTAPLAYERDETAIL_RESOURCE_ID_MESSAGE_JOINFESTA_PLAYER_LIST,
  JOINFESTAPLAYERDETAIL_RESOURCE_ID_MESSAGE_JOINFESTA_QUESTION_DATA,
  JOINFESTAPLAYERDETAIL_RESOURCE_ID_MENU_CURSOR,
  JOINFESTAPLAYERDETAIL_RESOURCE_MAX,
};

GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERDETAILRESOURCEID_H_INCLUDE
