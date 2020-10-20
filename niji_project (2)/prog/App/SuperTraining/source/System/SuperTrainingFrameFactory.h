// ============================================================================
/*
 * @file SuperTrainingFrameFactory.h
 * @brief すごい特訓アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.12.04
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( SUPER_TRAINIG_FRAMEFACTORY_H_INCLUDE )
#define SUPER_TRAINIG_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "App/SuperTraining/source/System/SuperTrainingWork.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)
GFL_NAMESPACE_BEGIN(System)


  class SuperTrainingFrameFactory : public NetAppLib::System::IApplicationFrameFactory
  {
    GFL_FORBID_COPY_AND_ASSIGN( SuperTrainingFrameFactory );
  public:
    SuperTrainingFrameFactory( App::SuperTraining::System::SuperTrainingWork* pSuperTrainingWork );

    virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

  private:
    App::SuperTraining::System::SuperTrainingWork*                m_pSuperTrainingWork;

  };


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)

#endif // SUPER_TRAINIG_FRAMEFACTORY_H_INCLUDE
