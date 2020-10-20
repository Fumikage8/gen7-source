// ============================================================================
/*
 * @file RegulationSelectDefine.h
 * @brief レギュレーション選択アプリで使用する定数等の定義ファイルです。
 * @date 2015.06.29
 */
// ============================================================================
#if !defined( REGULATIONSELECTDEFINE_H_INCLUDE )
#define REGULATIONSELECTDEFINE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)


static const u32  REGULATION_SELECT_APP_HEAP_SIZE                     = 1024 * 1024 *  1;   //  1 MB
static const u32  REGULATION_SELECT_APP_DEVICE_HEAP_SIZE              = 1024 * 1024 * 10;   // 10 MB


//! アプリの起動モード
enum BootMode
{
  BOOT_MODE_SELECT_P2P,               //! 選択モード：P2P通信対戦用
  BOOT_MODE_SELECT_BATTLE_CENTER,     //! 選択モード：バトル施設用
  BOOT_MODE_SELECT_BATTLE_ROYAL,      //! 選択モード：バトルロイヤル用
  BOOT_MODE_DOWNLOAD_P2P,             //! ダウンロードモード：P2P通信対戦用
  BOOT_MODE_DETAIL_BATTLE_TEAM        //! 詳細表示モード：バトルチーム選択アプリ用
};


//! 詳細画面を起動した Frame もしくは Proc
enum DetailBootFrame
{
  DETAIL_BOOT_FRAME_SELECT_MAIN,
  DETAIL_BOOT_FRAME_DL_SELECT,
  DETAIL_BOOT_FRAME_DL_DELETE,
  DETAIL_BOOT_PROC_BATTLE_TEAM_SELECT
};


static const u32 PRESET_LIST_MAX      = 32;
static const u32 DOWNLOAD_LIST_MAX    = 32;
static const u32 DOWNLOADED_LIST_MAX  = 32;


GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONSELECTDEFINE_H_INCLUDE
