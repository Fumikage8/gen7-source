// ============================================================================
/*
 * @file JoinFestaProfileDefine.h
 * @brief プロフィールアプリで使用する定数等の定義ファイルです。
 * @date 2015.08.21
 */
// ============================================================================
#if !defined( JOINFESTAPROFILEDEFINE_H_INCLUDE )
#define JOINFESTAPROFILEDEFINE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>


//#define JOINFESTAPROFILEDEFINE_LIKE_POKEMON_HONOR_ENABLE


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)


static const u32  JOINFESTAPROFILE_APP_HEAP_SIZE                     = 1024 * 1024 *  1;   //  1 MB
static const u32  JOINFESTAPROFILE_APP_DEVICE_HEAP_SIZE              = 1024 * 1024 * 20;   // 20 MB


// JoinFestaSelectWordDefine.h の EditMenuID と同じにする( 挨拶選択アプリで使用する部分だけ )
enum EditMenuID
{
  EDIT_MENU_ID_MEETING_MESSAGE,             // 出会いの挨拶
  EDIT_MENU_ID_GOODBY_MESSAGE,              // 別れの挨拶
  EDIT_MENU_ID_THROB,                       // 感動した時
  EDIT_MENU_ID_REGRETFUL,                   // 残念な時
  EDIT_MENU_ID_MY_SECRET,                   // 私の秘密

#if defined( JOINFESTAPROFILEDEFINE_LIKE_POKEMON_HONOR_ENABLE )
  EDIT_MENU_ID_LIKE_POKEMON,                // 好きなポケモン
  EDIT_MENU_ID_HONOR,                       // 肩書き
#endif // defined( JOINFESTAPROFILEDEFINE_LIKE_POKEMON_HONOR_ENABLE )

  EDIT_MENU_ID_NAME,                        // ジョインフェスタの名前
  EDIT_MENU_ID_BATTLE,                      // 対戦の申し込み
  EDIT_MENU_ID_TRADE,                       // 交換の申し込み
  EDIT_MENU_ID_ATTRACTION,                  // アトラクションへの意気込み
  EDIT_MENU_ID_FRIEND,                      // フレンドとの通信
  EDIT_MENU_ID_VIP,                         // VIPとの通信
  EDIT_MENU_ID_GUEST,                       // ゲストとの通信

  EDIT_MENU_ID_MAX
};

static const u32 INVALID_MESSAGE_ID = 0xffffffff;


enum BootProcID
{
  BOOT_PROC_ID_NONE,
  BOOT_PROC_ID_SELECT_WORD
};


GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPROFILEDEFINE_H_INCLUDE
