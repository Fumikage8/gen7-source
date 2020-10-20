// ============================================================================
/*
 * @file EndRollFrameFactory.h
 * @brief エンドロールアプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2016.03.16
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( APPENDROLL_FRAMEFACTORY_H_INCLUDE )
#define APPENDROLL_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "App/EndRoll/source/System/EndRollWork.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(EndRoll)
GFL_NAMESPACE_BEGIN(System)


class EndRollFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( EndRollFrameFactory );
public:
  EndRollFrameFactory( App::EndRoll::System::EndRollWork* pEndRollWork );

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  App::EndRoll::System::EndRollWork*                m_pEndRollWork;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(EndRoll)
GFL_NAMESPACE_END(App)

#endif // APPENDROLL_FRAMEFACTORY_H_INCLUDE
