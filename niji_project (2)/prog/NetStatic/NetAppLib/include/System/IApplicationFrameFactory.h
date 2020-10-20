// ============================================================================
/*
 * @file IApplicationFrameFactory.h
 * @brief アプリケーションのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.03.26
 */
// ============================================================================
#if !defined( IAPPLICATIONFRAMEFACTORY_H_INCLUDE )
#define IAPPLICATIONFRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetStatic/NetAppLib/include/System/ApplicationSystemTypedef.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)


class IApplicationFrameFactory
{
public:
  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID ) = 0;
};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)

#endif // IAPPLICATIONFRAMEFACTORY_H_INCLUDE
