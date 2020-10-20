// ============================================================================
/*
 * @file JoinFestaProfileFrameID.h
 * @brief プロフィールアプリのフレームIDを定義します。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPROFILE_FRAMEID_H_INCLUDE )
#define JOINFESTAPROFILE_FRAMEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)


enum JoinFestaProfileFrameID
{
  JOINFESTAPROFILE_FRAME_ID_SELECT_EDIT_MENU,
  JOINFESTAPROFILE_FRAME_ID_DESCRIPTION,
  JOINFESTAPROFILE_FRAME_ID_NAME_INPUT
};

GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPROFILE_FRAMEID_H_INCLUDE
