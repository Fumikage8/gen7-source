// ============================================================================
/*
 * @file ConnectionFrameID.h
 * @brief インターネット接続アプリのフレームIDを定義します。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( CONNECTION_FRAMEID_H_INCLUDE )
#define CONNECTION_FRAMEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)


enum ConnectionFrameID
{
  CONNECTION_FRAME_ID_CHANGE_INTERNET_MODE,
  CONNECTION_FRAME_ID_CHANGE_LOCAL_MODE,
  CONNECTION_FRAME_ID_INTERNET_CONNECTION,
  CONNECTION_FRAME_ID_INTERNET_DISCONNECTION,
  CONNECTION_FRAME_ID_FATAL_ERROR
};

GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // CONNECTION_FRAMEID_H_INCLUDE
