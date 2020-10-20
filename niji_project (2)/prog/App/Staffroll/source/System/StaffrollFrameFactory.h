// ============================================================================
/*
 * @file StaffrollFrameFactory.h
 * @brief スタッフロールアプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( STAFFROLL_FRAMEFACTORY_H_INCLUDE )
#define STAFFROLL_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "App/Staffroll/source/System/StaffrollWork.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)
GFL_NAMESPACE_BEGIN(System)


class StaffrollFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( StaffrollFrameFactory );
public:
  StaffrollFrameFactory( App::Staffroll::System::StaffrollWork* pStaffrollWork );

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  App::Staffroll::System::StaffrollWork*                m_pStaffrollWork;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)

#endif // STAFFROLL_FRAMEFACTORY_H_INCLUDE
