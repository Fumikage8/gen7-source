// ============================================================================
/*
 * @file JoinFestaGlobalAttractionFrameSwitcher.h
 * @brief グローバルアトラクションアプリのフレームを切り替えるクラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAGLOBALATTRACTION_FRAMESWITCHER_H_INCLUDE )
#define JOINFESTAGLOBALATTRACTION_FRAMESWITCHER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameSwitcher.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaGlobalAttractionFrameSwitcher: public NetAppLib::System::IApplicationFrameSwitcher
{
public:
  virtual NetAppLib::System::FrameID GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult );
};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAGLOBALATTRACTION_FRAMESWITCHER_H_INCLUDE
