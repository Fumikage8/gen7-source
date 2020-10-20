// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiApplicationSystem.h
 * @brief プレイヤー複数選択アプリのアプリケーションシステムです。
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTMULTI_APPLICATIONSYSTEM_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTMULTI_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaPlayerListSelectMultiWork;
class JoinFestaPlayerListSelectMultiBuilder;
class JoinFestaPlayerListSelectMultiViewList;
class JoinFestaPlayerListSelectMultiViewObjects;
class JoinFestaPlayerListSelectMultiViewSystemAccessor;

class JoinFestaPlayerListSelectMultiApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectMultiApplicationSystem );

public:
  JoinFestaPlayerListSelectMultiApplicationSystem();
  virtual~JoinFestaPlayerListSelectMultiApplicationSystem();

  void Build( NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiWork* pJoinFestaPlayerListSelectMultiWork, NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiBuilder* pApplicationSystemBuilder );


private:
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();

private:

  static const u32    MESSAGE_NUM   = 1;

  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiWork*                  m_pJoinFestaPlayerListSelectMultiWork;
  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewList*              m_pJoinFestaPlayerListSelectMultiViewList;
  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewObjects*           m_pJoinFestaPlayerListSelectMultiViewObjects;
  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewSystemAccessor*    m_pJoinFestaPlayerListSelectMultiViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAPLAYERLISTSELECTMULTI_APPLICATIONSYSTEM_H_INCLUDE
