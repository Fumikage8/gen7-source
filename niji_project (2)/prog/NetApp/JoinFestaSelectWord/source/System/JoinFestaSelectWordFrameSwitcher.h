// ============================================================================
/*
 * @file JoinFestaSelectWordFrameSwitcher.h
 * @brief 挨拶選択アプリのフレームを切り替えるクラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORD_FRAMESWITCHER_H_INCLUDE )
#define JOINFESTASELECTWORD_FRAMESWITCHER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameSwitcher.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaSelectWordFrameSwitcher: public NetAppLib::System::IApplicationFrameSwitcher
{
public:
  virtual NetAppLib::System::FrameID GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult );
};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASELECTWORD_FRAMESWITCHER_H_INCLUDE
