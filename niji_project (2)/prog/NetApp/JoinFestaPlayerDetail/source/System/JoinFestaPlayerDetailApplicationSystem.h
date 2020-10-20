// ============================================================================
/*
 * @file JoinFestaPlayerDetailApplicationSystem.h
 * @brief 詳細アプリのアプリケーションシステムです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERDETAIL_APPLICATIONSYSTEM_H_INCLUDE )
#define JOINFESTAPLAYERDETAIL_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaPlayerDetailWork;
class JoinFestaPlayerDetailBuilder;
class JoinFestaPlayerDetailViewList;
class JoinFestaPlayerDetailViewObjects;
class JoinFestaPlayerDetailViewSystemAccessor;

class JoinFestaPlayerDetailApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerDetailApplicationSystem );

public:
  JoinFestaPlayerDetailApplicationSystem();
  virtual~JoinFestaPlayerDetailApplicationSystem();

  void Build( NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailWork* pJoinFestaPlayerDetailWork, NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailBuilder* pApplicationSystemBuilder );


private:
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();

private:

  static const u32    MESSAGE_NUM   = 1;

  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailWork*                  m_pJoinFestaPlayerDetailWork;
  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewList*              m_pJoinFestaPlayerDetailViewList;
  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewObjects*           m_pJoinFestaPlayerDetailViewObjects;
  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewSystemAccessor*    m_pJoinFestaPlayerDetailViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAPLAYERDETAIL_APPLICATIONSYSTEM_H_INCLUDE
