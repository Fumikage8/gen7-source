// ============================================================================
/*
 * @file JoinFestaPlayerListShowDefine.h
 * @brief 閲覧アプリで使用する定数等の定義ファイルです。
 * @date 2015.08.21
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSHOWDEFINE_H_INCLUDE )
#define JOINFESTAPLAYERLISTSHOWDEFINE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)


static const u32  JOINFESTAPLAYERLISTSHOW_APP_HEAP_SIZE                     = 1024 * 1024 * 1;   // 1 MB
static const u32  JOINFESTAPLAYERLISTSHOW_APP_DEVICE_HEAP_SIZE              = 1024 * 1024 * 6;   // 6 MB


enum ShowType
{
  SHOW_TYPE_GUEST,
  SHOW_TYPE_VIP,
  SHOW_TYPE_BLACK
};


GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSHOWDEFINE_H_INCLUDE
