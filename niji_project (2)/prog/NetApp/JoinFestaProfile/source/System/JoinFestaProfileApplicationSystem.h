// ============================================================================
/*
 * @file JoinFestaProfileApplicationSystem.h
 * @brief プロフィールアプリのアプリケーションシステムです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPROFILE_APPLICATIONSYSTEM_H_INCLUDE )
#define JOINFESTAPROFILE_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaProfileWork;
class JoinFestaProfileBuilder;
class JoinFestaProfileViewList;
class JoinFestaProfileViewObjects;
class JoinFestaProfileViewSystemAccessor;

class JoinFestaProfileApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaProfileApplicationSystem );

public:
  JoinFestaProfileApplicationSystem();
  virtual~JoinFestaProfileApplicationSystem();

  void Build( NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork, NetApp::JoinFestaProfile::System::JoinFestaProfileBuilder* pApplicationSystemBuilder );


private:
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();

private:

  static const u32    MESSAGE_NUM   = 1;

  NetApp::JoinFestaProfile::System::JoinFestaProfileWork*                  m_pJoinFestaProfileWork;
  NetApp::JoinFestaProfile::System::JoinFestaProfileViewList*              m_pJoinFestaProfileViewList;
  NetApp::JoinFestaProfile::System::JoinFestaProfileViewObjects*           m_pJoinFestaProfileViewObjects;
  NetApp::JoinFestaProfile::System::JoinFestaProfileViewSystemAccessor*    m_pJoinFestaProfileViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAPROFILE_APPLICATIONSYSTEM_H_INCLUDE
