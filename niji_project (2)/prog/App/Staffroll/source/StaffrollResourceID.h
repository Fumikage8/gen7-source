// ============================================================================
/*
 * @file StaffrollResourceID.h
 * @brief スタッフロールアプリで使用するリソースのIDです。
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( STAFFROLL_RESOURCEID_H_INCLUDE )
#define STAFFROLL_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)


enum StaffrollResourceID
{
  STAFFROLL_RESOURCE_ID_LAYOUT,
  STAFFROLL_RESOURCE_ID_GRPFONT_LAYOUT,

  STAFFROLL_RESOURCE_ID_MESSAGE,
  STAFFROLL_RESOURCE_ID_EVENT_TEXT_DATA,
  STAFFROLL_RESOURCE_ID_EVENT_TEXT_CONFIG_DATA,
  STAFFROLL_RESOURCE_ID_EVENT_IMG_DATA,

  STAFFROLL_RESOURCE_MAX,
};

GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)

#endif // STAFFROLLRESOURCEID_H_INCLUDE

