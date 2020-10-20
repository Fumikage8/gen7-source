// ============================================================================
/*
 * @file JoinFestaSelectWordDefine.h
 * @brief 挨拶選択アプリで使用する定数等の定義ファイルです。
 * @date 2015.08.21
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORDDEFINE_H_INCLUDE )
#define JOINFESTASELECTWORDDEFINE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)


static const u32  JOINFESTASELECTWORD_APP_HEAP_SIZE                     = 1024 * 1024 *  1;   //  1 MB
static const u32  JOINFESTASELECTWORD_APP_DEVICE_HEAP_SIZE              = 1024 * 1024 * 10;   // 10 MB



// JoinFestaProfileDefine.h の EditMenuID と同じにする( 挨拶選択アプリで使用する部分だけ )
enum EditMenuID
{
  EDIT_MENU_ID_MEETING_MESSAGE,             // 出会いの挨拶
  EDIT_MENU_ID_GOODBY_MESSAGE,              // 別れの挨拶
  EDIT_MENU_ID_THROB,                       // 感動した時
  EDIT_MENU_ID_REGRETFUL,                   // 残念な時
  EDIT_MENU_ID_MY_SECRET                    // 私の秘密
};

GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASELECTWORDDEFINE_H_INCLUDE
