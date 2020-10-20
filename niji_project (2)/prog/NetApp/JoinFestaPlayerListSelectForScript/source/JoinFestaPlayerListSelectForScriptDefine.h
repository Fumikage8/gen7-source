// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptDefine.h
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)で使用する定数等の定義ファイルです。
 * @date 2015.08.21
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTFORSCRIPTDEFINE_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTFORSCRIPTDEFINE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)


static const u32  JOINFESTAPLAYERLISTSELECTFORSCRIPT_APP_HEAP_SIZE                     = 1024 * 1024 *  1;   //  1 MB
//static const u32  JOINFESTAPLAYERLISTSELECTFORSCRIPT_APP_DEVICE_HEAP_SIZE              = 1024 * 1024 * 20;   // 20 MB
static const u32  JOINFESTAPLAYERLISTSELECTFORSCRIPT_APP_DEVICE_HEAP_SIZE              = 1024 * 1024 * 10;   // 20 MB


enum BootMode
{
  BOOT_MODE_DEFAULT,          //! 仕様書のモード４
  BOOT_MODE_LIMIT,            //! 仕様書のモード５(リストのサイズに指定があるバージョン)
  BOOT_MODE_BattleFesCircle   /**<  BattleFesCircle用  */
};


GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECTFORSCRIPTDEFINE_H_INCLUDE
