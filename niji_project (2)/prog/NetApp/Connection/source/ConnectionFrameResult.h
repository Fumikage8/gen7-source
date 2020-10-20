// ============================================================================
/*
 * @file ConnectionFrameResult.h
 * @brief インターネット接続アプリのリザルトコードを定義します。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( CONNECTION_FRAMERESULT_H_INCLUDE )
#define CONNECTION_FRAMERESULT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)


enum ChangeInternetModeResultCode
{
  CHANGE_INTERNET_MODE_RESULT_BACK
};


enum ChangeLocalModeResultCode
{
  CHANGE_LOCAL_MODE_RESULT_BACK
};


enum InternetConnectionResultCode
{
  INTERNET_CONNECTION_RESULT_BACK,
  INTERNET_CONNECTION_RESULT_CHANGE_INTERNET_MODE
};


enum InternetDisconnectionResultCode
{
  INTERNET_DISCONNECTION_RESULT_BACK
};


GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // CONNECTION_FRAMERESULT_H_INCLUDE
