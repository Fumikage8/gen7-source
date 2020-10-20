// ============================================================================
/*
 * @file GameSyncConfirmPlayStatusResponseListener.h
 * @brief 
 * @date 2015.12.03
 */
// ============================================================================
#if !defined( GAMESYNCCONFIRMPLAYSTATUSRESPONSELISTENER_H_INCLUDE )
#define GAMESYNCCONFIRMPLAYSTATUSRESPONSELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

#if defined(GF_PLATFORM_CTR)
#include "NetStatic/NetLib/include/PGL/PGLDefine.h"
#endif // defined(GF_PLATFORM_CTR)

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(GameSync)


class GameSyncConfirmPlayStatusResponseListener
{
public:
  virtual void OnConfirmPlayStatusSuccess() = 0;
  virtual void OnConfirmPlayStatusError( u32 errorCode ) = 0;

#if defined(GF_PLATFORM_CTR)
  virtual void OnConfirmPlayStatusError( u32 errorCode, NetLib::Pgl::PGL_COMMON_RESULT ePglCommonResult ){}
#endif // defined(GF_PLATFORM_CTR)

};


GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetAppLib)

#endif // GAMESYNCCONFIRMPLAYSTATUSRESPONSELISTENER_H_INCLUDE
