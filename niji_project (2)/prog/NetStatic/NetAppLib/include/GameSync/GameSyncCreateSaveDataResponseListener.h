// ============================================================================
/*
 * @file GameSyncCreateSaveDataResponseListener.h
 * @brief 
 * @date 2015.12.03
 */
// ============================================================================
#if !defined( GAMESYNCCREATESAVEDATARESPONSELISTENER_H_INCLUDE )
#define GAMESYNCCREATESAVEDATARESPONSELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(GameSync)


class GameSyncCreateSaveDataResponseListener
{
public:
  virtual void OnCreateSaveDataSuccess() = 0;
  virtual void OnCreateSaveDataError() = 0;
};


GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetAppLib)

#endif // GAMESYNCCREATESAVEDATARESPONSELISTENER_H_INCLUDE
