// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptResourceID.h
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)で使用するリソースのIDです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCEID_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)


enum JoinFestaPlayerListSelectForScriptResourceID
{
  JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_LAYOUT,
  JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_MESSAGE_JOINFESTA_PLAYER_LIST,
  JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_MESSAGE_JOINFESTA_QUESTION_DATA,
  JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_ID_MENU_CURSOR,
  JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_MAX,
};

GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECTFORSCRIPTRESOURCEID_H_INCLUDE
