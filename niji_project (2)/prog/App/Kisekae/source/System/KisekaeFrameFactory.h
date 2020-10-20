// ============================================================================
/*
 * @file KisekaeFrameFactory.h
 * @brief きせかえアプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( KISEKAE_FRAMEFACTORY_H_INCLUDE )
#define KISEKAE_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "App/Kisekae/source/System/KisekaeWork.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(System)


class FrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( FrameFactory );
public:
  FrameFactory( App::Kisekae::System::Work* pNetAppSkeletonWork );

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  App::Kisekae::System::Work*                m_pWork;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)

#endif // KISEKAE_FRAMEFACTORY_H_INCLUDE
