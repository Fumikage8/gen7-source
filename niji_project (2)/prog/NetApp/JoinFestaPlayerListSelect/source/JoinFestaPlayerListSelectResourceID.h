// ============================================================================
/*
 * @file JoinFestaPlayerListSelectResourceID.h
 * @brief プレイヤー単体選択アプリで使用するリソースのIDです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECT_RESOURCEID_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECT_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)


enum JoinFestaPlayerListSelectResourceID
{
  JOINFESTAPLAYERLISTSELECT_RESOURCE_ID_LAYOUT,
  JOINFESTAPLAYERLISTSELECT_RESOURCE_ID_MESSAGE_JOINFESTAPLAYERLISTSELECT,
  JOINFESTAPLAYERLISTSELECT_RESOURCE_ID_MENU_CURSOR,
  JOINFESTAPLAYERLISTSELECT_RESOURCE_MAX,
};

GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECTRESOURCEID_H_INCLUDE
