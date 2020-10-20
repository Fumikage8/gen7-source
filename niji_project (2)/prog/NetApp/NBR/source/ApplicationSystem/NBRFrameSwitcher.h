// ============================================================================
/*
 * @file NBRFrameSwitcher.h
 * @brief 通信対戦受付アプリのフレームを切り替えるクラスです。
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_FRAMESWITCHER_H_INCLUDE )
#define NBR_FRAMESWITCHER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameSwitcher.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRWork.h"
#include "NetApp/NBR/include/NBRAppParam.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


class NBRFrameSwitcher: public NetAppLib::System::IApplicationFrameSwitcher
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRFrameSwitcher );
public:
  NBRFrameSwitcher( APP_PARAM* pAppParam, NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork ) :
    m_pAppParam(pAppParam)
   ,m_pNBRWork( pNBRWork )
  {};
  virtual~NBRFrameSwitcher(){}

  virtual NetAppLib::System::FrameID GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult );

private:
  APP_PARAM*  m_pAppParam;
  NetApp::NBR::ApplicationSystem::NBRWork* m_pNBRWork;
};


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_FRAMESWITCHER_H_INCLUDE
