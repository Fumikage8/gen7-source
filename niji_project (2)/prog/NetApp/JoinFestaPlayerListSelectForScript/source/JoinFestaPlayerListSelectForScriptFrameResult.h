// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptFrameResult.h
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)のリザルトコードを定義します。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTFORSCRIPT_FRAMERESULT_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTFORSCRIPT_FRAMERESULT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)


enum SelectFrameResultCode
{
  SELECT_FRAME_RESULT_BACK,
  SELECT_FRAME_RESULT_SELECT_PLAYER,
  SELECT_FRAME_RESULT_TO_DETAIL
};


enum DetailFrameResultCode
{
  DETAIL_FRAME_RESULT_BACK
};


GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECTFORSCRIPT_FRAMERESULT_H_INCLUDE
