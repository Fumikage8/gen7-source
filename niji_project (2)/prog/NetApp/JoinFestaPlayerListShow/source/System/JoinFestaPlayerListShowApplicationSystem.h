// ============================================================================
/*
 * @file JoinFestaPlayerListShowApplicationSystem.h
 * @brief 閲覧アプリのアプリケーションシステムです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSHOW_APPLICATIONSYSTEM_H_INCLUDE )
#define JOINFESTAPLAYERLISTSHOW_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaPlayerListShowWork;
class JoinFestaPlayerListShowBuilder;
class JoinFestaPlayerListShowViewList;
class JoinFestaPlayerListShowViewObjects;
class JoinFestaPlayerListShowViewSystemAccessor;

class JoinFestaPlayerListShowApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListShowApplicationSystem );

public:
  JoinFestaPlayerListShowApplicationSystem();
  virtual~JoinFestaPlayerListShowApplicationSystem();

  void Build( NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowWork* pJoinFestaPlayerListShowWork, NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowBuilder* pApplicationSystemBuilder );


private:
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();

private:

  static const u32    MESSAGE_NUM   = 1;

  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowWork*                  m_pJoinFestaPlayerListShowWork;
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewList*              m_pJoinFestaPlayerListShowViewList;
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewObjects*           m_pJoinFestaPlayerListShowViewObjects;
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewSystemAccessor*    m_pJoinFestaPlayerListShowViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAPLAYERLISTSHOW_APPLICATIONSYSTEM_H_INCLUDE
