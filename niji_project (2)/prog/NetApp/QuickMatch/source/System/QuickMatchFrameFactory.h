// ============================================================================
/*
 * @file QuickMatchFrameFactory.h
 * @brief クイックマッチアプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( QUICKMATCH_FRAMEFACTORY_H_INCLUDE )
#define QUICKMATCH_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "NetApp/QuickMatch/source/System/QuickMatchWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(System)


class QuickMatchFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( QuickMatchFrameFactory );
public:
  QuickMatchFrameFactory( NetApp::QuickMatch::System::QuickMatchWork* pQuickMatchWork );
  virtual~QuickMatchFrameFactory();

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  NetApp::QuickMatch::System::QuickMatchWork*                m_pQuickMatchWork;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)

#endif // QUICKMATCH_FRAMEFACTORY_H_INCLUDE
