// ============================================================================
/*
 * @file JoinFestaPlayerListSelectApplicationSystem.h
 * @brief プレイヤー単体選択アプリのアプリケーションシステムです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECT_APPLICATIONSYSTEM_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECT_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaPlayerListSelectWork;
class JoinFestaPlayerListSelectBuilder;
class JoinFestaPlayerListSelectViewList;
class JoinFestaPlayerListSelectViewObjects;
class JoinFestaPlayerListSelectViewSystemAccessor;

class JoinFestaPlayerListSelectApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectApplicationSystem );

public:
  JoinFestaPlayerListSelectApplicationSystem();
  virtual~JoinFestaPlayerListSelectApplicationSystem();

  void Build( NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectWork* pJoinFestaPlayerListSelectWork, NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectBuilder* pApplicationSystemBuilder );


private:
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();

private:

  static const u32    MESSAGE_NUM   = 1;

  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectWork*                  m_pJoinFestaPlayerListSelectWork;
  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewList*              m_pJoinFestaPlayerListSelectViewList;
  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewObjects*           m_pJoinFestaPlayerListSelectViewObjects;
  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewSystemAccessor*    m_pJoinFestaPlayerListSelectViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAPLAYERLISTSELECT_APPLICATIONSYSTEM_H_INCLUDE
