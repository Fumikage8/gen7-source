// ============================================================================
/*
 * @file GlobalAttractionOpenCheckResponseListener.h
 * @brief 
 * @date 2016.01.19
 */
// ============================================================================
#if !defined( GLOBAL_ATTRACTION_OPEN_CHECK_RESPONSE_LISTENER_H_INCLUDE )
#define GLOBAL_ATTRACTION_OPEN_CHECK_RESPONSE_LISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(GlobalAttraction)


class GlobalAttractionOpenCheckResponseListener
{
public:
  virtual void OnOpenCheckSuccess( bool bIsOpen ) = 0;
  virtual void OnOpenCheckError() = 0;
  virtual void OnOpenCheckCancel() = 0;
};


GFL_NAMESPACE_END(GlobalAttraction)
GFL_NAMESPACE_END(NetAppLib)

#endif // GLOBAL_ATTRACTION_OPEN_CHECK_RESPONSE_LISTENER_H_INCLUDE
