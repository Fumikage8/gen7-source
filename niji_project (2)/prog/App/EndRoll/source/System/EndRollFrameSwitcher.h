// ============================================================================
/*
 * @file EndRollFrameSwitcher.h
 * @brief エンドロールアプリのフレームを切り替えるクラスです。
 * @date 2016.03.16
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( APPENDROLL_FRAMESWITCHER_H_INCLUDE )
#define APPENDROLL_FRAMESWITCHER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameSwitcher.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(EndRoll)
GFL_NAMESPACE_BEGIN(System)


class EndRollFrameSwitcher: public NetAppLib::System::IApplicationFrameSwitcher
{
public:
  virtual NetAppLib::System::FrameID GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult );
};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(EndRoll)
GFL_NAMESPACE_END(App)

#endif // APPENDROLL_FRAMESWITCHER_H_INCLUDE
