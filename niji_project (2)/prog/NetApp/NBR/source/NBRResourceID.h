// ============================================================================
/*
 * @file NBRResourceID.h
 * @brief 通信対戦受付アプリで使用するリソースのIDです。
  * @date 2015.06.22
 * @author ichiraku_katsuhiko
*/
// ============================================================================
#if !defined( NBR_RESOURCEID_H_INCLUDE )
#define NBR_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)


enum NBRResourceID
{
  NBR_RESOURCE_ID_NBR,              //! NBRレイアウト
  NBR_RESOURCE_ID_COMMON_WINDOW,    //! メッセージウィンドウ部品
  NBR_RESOURCE_ID_MENU_CURSOR,      //! 共通メニューカーソル部品
  NBR_RESOURCE_ID_REGULATIONSELECT, //! レギュレーション選択アプリのレイアウト
  NBR_RESOURCE_MAX,
};

GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBRRESOURCEID_H_INCLUDE
