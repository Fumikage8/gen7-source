// ============================================================================
/*
 * @file GTSFrameSwitcher.h
 * @brief GTSアプリのフレームを切り替えるクラスです。
 * @date 2015.03.26
 */
// ============================================================================
#if !defined( GTSSEQUENCECONTROLLER_H_INCLUDE )
#define GTSSEQUENCECONTROLLER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameSwitcher.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


class GTSFrameSwitcher: public NetAppLib::System::IApplicationFrameSwitcher
{
public:
  virtual NetAppLib::System::FrameID GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult );
};


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSSEQUENCECONTROLLER_H_INCLUDE
