// ============================================================================
/*
 * @file JoinFestaPlayerListShowFrameID.h
 * @brief 閲覧アプリのフレームIDを定義します。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSHOW_FRAMEID_H_INCLUDE )
#define JOINFESTAPLAYERLISTSHOW_FRAMEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)


enum JoinFestaPlayerListShowFrameID
{
  JOINFESTAPLAYERLISTSHOW_FRAME_ID_SELECT,
  JOINFESTAPLAYERLISTSHOW_FRAME_ID_DETAIL,
  JOINFESTAPLAYERLISTSHOW_FRAME_ID_DESCRIPTION
};

GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSHOW_FRAMEID_H_INCLUDE
