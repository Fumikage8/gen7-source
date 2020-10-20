// ============================================================================
/*
 * @file SuperTrainingFrameSwitcher.h
 * @brief すごい特訓アプリのフレームを切り替えるクラスです。
 * @date 2015.12.04
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( SUPER_TRAINIG_FRAMESWITCHER_H_INCLUDE )
#define SUPER_TRAINIG_FRAMESWITCHER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameSwitcher.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)
GFL_NAMESPACE_BEGIN(System)


  class SuperTrainingFrameSwitcher: public NetAppLib::System::IApplicationFrameSwitcher
  {
  public:
    virtual NetAppLib::System::FrameID GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult );
  };


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)

#endif // SUPER_TRAINIG_FRAMESWITCHER_H_INCLUDE
