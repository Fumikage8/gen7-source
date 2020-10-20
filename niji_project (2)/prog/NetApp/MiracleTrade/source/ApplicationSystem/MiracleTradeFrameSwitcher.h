// ============================================================================
/*
 * @file MiracleTradeFrameSwitcher.h
 * @brief ミラクル交換アプリのフレームを切り替えるクラスです。
 * @date 2015.05.25
 */
// ============================================================================
#if !defined( MIRACLETRADSEQUENCECONTROLLER_H_INCLUDE )
#define MIRACLETRADSEQUENCECONTROLLER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameSwitcher.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


class MiracleTradeFrameSwitcher: public NetAppLib::System::IApplicationFrameSwitcher
{
public:
  virtual NetAppLib::System::FrameID GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult );
};


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADSEQUENCECONTROLLER_H_INCLUDE
