// ============================================================================
/*
 * @file JoinFestaSubScreenResourceID.h
 * @brief ジョインフェスタ下画面で使用するリソースのIDです。
 * @date 2015.06.10
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_RESOURCEID_H_INCLUDE )
#define JOINFESTASUBSCREEN_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)


enum JoinFestaSubScreenResourceID
{
  JOINFESTASUBSCREEN_RESOURCE_ID_LAYOUT,
  JOINFESTASUBSCREEN_RESOURCE_ID_ROTOM_FADE,
  JOINFESTASUBSCREEN_RESOURCE_ID_MESSAGE,
  JOINFESTASUBSCREEN_RESOURCE_MAX,
};

GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREENRESOURCEID_H_INCLUDE
