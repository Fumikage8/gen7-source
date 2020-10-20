// ============================================================================
/*
 * @file JoinFestaAttractionResultResourceID.h
 * @brief アトラクション結果アプリで使用するリソースのIDです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONRESULT_RESOURCEID_H_INCLUDE )
#define JOINFESTAATTRACTIONRESULT_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)


enum JoinFestaAttractionResultResourceID
{
  JOINFESTAATTRACTIONRESULT_RESOURCE_ID_LAYOUT,
  JOINFESTAATTRACTIONRESULT_RESOURCE_ID_MESSAGE_JOINFESTA_PLAYER_LIST,
  JOINFESTAATTRACTIONRESULT_RESOURCE_ID_MESSAGE_JOINFESTA_ATTRACTION,
  JOINFESTAATTRACTIONRESULT_RESOURCE_ID_MENU_CURSOR,
  JOINFESTAATTRACTIONRESULT_RESOURCE_MAX,
};

GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAATTRACTIONRESULTRESOURCEID_H_INCLUDE
