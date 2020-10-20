// ============================================================================
/*
 * @file JoinFestaBeaconApplicationSystem.h
 * @brief ビーコンアプリのアプリケーションシステムです。
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( JOINFESTABEACON_APPLICATIONSYSTEM_H_INCLUDE )
#define JOINFESTABEACON_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "NetApp/JoinFestaBeacon/source/View/JoinFestaBeaconUpperView.h"
#include "NetApp/JoinFestaBeacon/source/View/JoinFestaBeaconLowerView.h"

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaBeaconWork;
class JoinFestaBeaconBuilder;

class JoinFestaBeaconApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaBeaconApplicationSystem );

public:
  JoinFestaBeaconApplicationSystem();
  virtual~JoinFestaBeaconApplicationSystem();

  void Build( NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork* pJoinFestaBeaconWork, NetApp::JoinFestaBeacon::System::JoinFestaBeaconBuilder* pApplicationSystemBuilder );


private:
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();

private:

  NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork*                  m_pJoinFestaBeaconWork;
  NetApp::JoinFestaBeacon::View::JoinFestaBeaconUpperView* m_pJoinFestaBeaconUpperView;
  NetApp::JoinFestaBeacon::View::JoinFestaBeaconLowerView* m_pJoinFestaBeaconLowerView;

  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGLowerView* m_pJoinFestaPlayerListBGLowerView;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListBGUpperView* m_pJoinFestaPlayerListBGUpperView;
};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTABEACON_APPLICATIONSYSTEM_H_INCLUDE
