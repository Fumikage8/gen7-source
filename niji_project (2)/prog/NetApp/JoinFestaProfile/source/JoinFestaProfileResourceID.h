// ============================================================================
/*
 * @file JoinFestaProfileResourceID.h
 * @brief プロフィールアプリで使用するリソースのIDです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPROFILE_RESOURCEID_H_INCLUDE )
#define JOINFESTAPROFILE_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)


enum JoinFestaProfileResourceID
{
  JOINFESTAPROFILE_RESOURCE_ID_LAYOUT,
  JOINFESTAPROFILE_RESOURCE_ID_LAYOUT_PLAYER_LIST,
  JOINFESTAPROFILE_RESOURCE_ID_MESSAGE_JOINFESTAPROFILE,
  JOINFESTAPROFILE_RESOURCE_ID_MESSAGE_JF_PHRASE,
  JOINFESTAPROFILE_RESOURCE_ID_MENU_CURSOR,
  JOINFESTAPROFILE_RESOURCE_MAX,
};

GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPROFILERESOURCEID_H_INCLUDE
