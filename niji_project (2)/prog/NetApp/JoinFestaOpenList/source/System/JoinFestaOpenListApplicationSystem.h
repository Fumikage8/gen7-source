// ============================================================================
/*
 * @file JoinFestaOpenListApplicationSystem.h
 * @brief 開催選択アプリのアプリケーションシステムです。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAOPENLIST_APPLICATIONSYSTEM_H_INCLUDE )
#define JOINFESTAOPENLIST_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaOpenListWork;
class JoinFestaOpenListBuilder;
class JoinFestaOpenListViewList;
class JoinFestaOpenListViewObjects;
class JoinFestaOpenListViewSystemAccessor;

class JoinFestaOpenListApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaOpenListApplicationSystem );

public:
  JoinFestaOpenListApplicationSystem();
  virtual~JoinFestaOpenListApplicationSystem();

  void Build( NetApp::JoinFestaOpenList::System::JoinFestaOpenListWork* pJoinFestaOpenListWork, NetApp::JoinFestaOpenList::System::JoinFestaOpenListBuilder* pApplicationSystemBuilder );


private:
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();

private:


  NetApp::JoinFestaOpenList::System::JoinFestaOpenListWork*                  m_pJoinFestaOpenListWork;
  NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewList*              m_pJoinFestaOpenListViewList;
  NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewObjects*           m_pJoinFestaOpenListViewObjects;
  NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewSystemAccessor*    m_pJoinFestaOpenListViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAOPENLIST_APPLICATIONSYSTEM_H_INCLUDE
