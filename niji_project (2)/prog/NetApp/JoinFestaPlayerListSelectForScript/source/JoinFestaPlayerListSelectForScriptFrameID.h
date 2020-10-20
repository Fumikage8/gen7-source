// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptFrameID.h
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)のフレームIDを定義します。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTFORSCRIPT_FRAMEID_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTFORSCRIPT_FRAMEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)


enum JoinFestaPlayerListSelectForScriptFrameID
{
  JOINFESTAPLAYERLISTSELECTFORSCRIPT_FRAME_ID_SELECT,
  JOINFESTAPLAYERLISTSELECTFORSCRIPT_FRAME_ID_DETAIL,
  JOINFESTAPLAYERLISTSELECTFORSCRIPT_FRAME_ID_ForBattleFesCircle,
};

GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECTFORSCRIPT_FRAMEID_H_INCLUDE
