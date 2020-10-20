// ============================================================================
/*
 * @file JoinFestaBeaconFrameSwitcher.h
 * @brief ビーコンアプリのフレームを切り替えるクラスです。
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( JOINFESTABEACON_FRAMESWITCHER_H_INCLUDE )
#define JOINFESTABEACON_FRAMESWITCHER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameSwitcher.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaBeaconFrameSwitcher: public NetAppLib::System::IApplicationFrameSwitcher
{
public:
  virtual NetAppLib::System::FrameID GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult );
};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTABEACON_FRAMESWITCHER_H_INCLUDE
