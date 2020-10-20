// ============================================================================
/*
 * @file JoinFestaSelectWordResourceID.h
 * @brief 挨拶選択アプリで使用するリソースのIDです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORD_RESOURCEID_H_INCLUDE )
#define JOINFESTASELECTWORD_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)


enum JoinFestaSelectWordResourceID
{
  JOINFESTASELECTWORD_RESOURCE_ID_LAYOUT,
  JOINFESTASELECTWORD_RESOURCE_ID_MESSAGE_SET_PHRASE,
  JOINFESTASELECTWORD_RESOURCE_ID_MESSAGE_JF_PHRASE,
  JOINFESTASELECTWORD_RESOURCE_ID_MENU_CURSOR,
  JOINFESTASELECTWORD_RESOURCE_ID_INFO_TABLE,
  JOINFESTASELECTWORD_RESOURCE_ID_CATEGORY_TABLE,
  JOINFESTASELECTWORD_RESOURCE_MAX,
};

GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASELECTWORDRESOURCEID_H_INCLUDE
