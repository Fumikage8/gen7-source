// ============================================================================
/*
 * @file IApplicationFrameSwitcher.h
 * @brief アプリケーションのフレーム(プロセス)を切り替える為のインターフェースクラスです。
 * @date 2015.03.26
 */
// ============================================================================
#if !defined( IAPPLICATIONSEQUENCECONTROLLER_H_INCLUDE )
#define IAPPLICATIONSEQUENCECONTROLLER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemTypedef.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)


class IApplicationFrameSwitcher
{
public:
  virtual NetAppLib::System::FrameID GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult ) = 0;
};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)

#endif // IAPPLICATIONSEQUENCECONTROLLER_H_INCLUDE
