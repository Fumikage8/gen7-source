// ============================================================================
/*
 * @file JoinFestaPlayerListSelectFrameSwitcher.h
 * @brief プレイヤー単体選択アプリのフレームを切り替えるクラスです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECT_FRAMESWITCHER_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECT_FRAMESWITCHER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameSwitcher.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaPlayerListSelectFrameSwitcher: public NetAppLib::System::IApplicationFrameSwitcher
{
public:
  virtual NetAppLib::System::FrameID GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult );
};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECT_FRAMESWITCHER_H_INCLUDE
