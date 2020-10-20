// ============================================================================
/*
 * @file GameSyncSaveDataSyncResponseListener.h
 * @brief 
 * @date 2015.12.03
 */
// ============================================================================
#if !defined( GAMESYNCSAVEDATASYNCRESPONSELISTENER_H_INCLUDE )
#define GAMESYNCSAVEDATASYNCRESPONSELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

#if defined(GF_PLATFORM_CTR)
#include "NetStatic/NetLib/include/PGL/PGLDefine.h"
#endif // defined(GF_PLATFORM_CTR)

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(GameSync)


class GameSyncSaveDataSyncResponseListener
{
public:
  virtual void OnSaveDataSyncSuccess() = 0;
  virtual void OnSaveDataSyncError( u32 errorCode ) = 0;

#if defined(GF_PLATFORM_CTR)
  virtual void OnSaveDataSyncError( u32 errorCode, NetLib::Pgl::PGL_COMMON_RESULT ePglCommonResult ){}
#endif // defined(GF_PLATFORM_CTR)

};


GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetAppLib)

#endif // GAMESYNCSAVEDATASYNCRESPONSELISTENER_H_INCLUDE
