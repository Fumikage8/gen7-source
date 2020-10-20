// ============================================================================
/*
 * @file JoinFestaSubScreenFrameSwitcher.h
 * @brief ジョインフェスタ下画面のフレームを切り替えるクラスです。
 * @date 2015.06.10
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_FRAMESWITCHER_H_INCLUDE )
#define JOINFESTASUBSCREEN_FRAMESWITCHER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameID.h"
#include "NetStatic/NetAppLib/include/System/IApplicationFrameSwitcher.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


class JoinFestaSubScreenFrameSwitcher: public NetAppLib::System::IApplicationFrameSwitcher
{
public:
  virtual NetAppLib::System::FrameID GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult );

private:
  //! お誘い画面での戻り先フレーム
  NetAppLib::System::FrameID m_invitationReturnFrame;

};


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_FRAMESWITCHER_H_INCLUDE
