// ============================================================================
/*
 * @file RegulationSelectFrameSwitcher.h
 * @brief レギュレーション選択アプリのフレームを切り替えるクラスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONSELECT_FRAMESWITCHER_H_INCLUDE )
#define REGULATIONSELECT_FRAMESWITCHER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameSwitcher.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


class RegulationSelectFrameSwitcher: public NetAppLib::System::IApplicationFrameSwitcher
{
public:
  virtual NetAppLib::System::FrameID GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult );
};


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONSELECT_FRAMESWITCHER_H_INCLUDE
