// ============================================================================
/*
 * @file JoinFestaAttractionListSelectResourceID.h
 * @brief アトラクション開催参加アプリで使用するリソースのIDです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONLISTSELECT_RESOURCEID_H_INCLUDE )
#define JOINFESTAATTRACTIONLISTSELECT_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)


enum JoinFestaAttractionListSelectResourceID
{
  JOINFESTAATTRACTIONLISTSELECT_RESOURCE_ID_LAYOUT,
  JOINFESTAATTRACTIONLISTSELECT_RESOURCE_ID_MESSAGE_JOINFESTA_PLAYER_LIST,
  JOINFESTAATTRACTIONLISTSELECT_RESOURCE_ID_MESSAGE_JOINFESTA_ATTRACTION,
  JOINFESTAATTRACTIONLISTSELECT_RESOURCE_ID_MENU_CURSOR,
  JOINFESTAATTRACTIONLISTSELECT_RESOURCE_MAX,
};

GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAATTRACTIONLISTSELECTRESOURCEID_H_INCLUDE
