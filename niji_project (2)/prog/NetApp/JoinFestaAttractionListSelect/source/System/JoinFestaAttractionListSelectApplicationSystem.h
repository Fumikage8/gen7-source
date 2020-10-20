// ============================================================================
/*
 * @file JoinFestaAttractionListSelectApplicationSystem.h
 * @brief アトラクション開催参加アプリのアプリケーションシステムです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONLISTSELECT_APPLICATIONSYSTEM_H_INCLUDE )
#define JOINFESTAATTRACTIONLISTSELECT_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaAttractionListSelectWork;
class JoinFestaAttractionListSelectBuilder;
class JoinFestaAttractionListSelectViewList;
class JoinFestaAttractionListSelectViewObjects;
class JoinFestaAttractionListSelectViewSystemAccessor;

class JoinFestaAttractionListSelectApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionListSelectApplicationSystem );

public:
  JoinFestaAttractionListSelectApplicationSystem();
  virtual~JoinFestaAttractionListSelectApplicationSystem();

  void Build( NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectWork* pJoinFestaAttractionListSelectWork, NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectBuilder* pApplicationSystemBuilder );


private:
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();

private:

  static const u32    MESSAGE_NUM   = 1;

  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectWork*                  m_pJoinFestaAttractionListSelectWork;
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewList*              m_pJoinFestaAttractionListSelectViewList;
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewObjects*           m_pJoinFestaAttractionListSelectViewObjects;
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewSystemAccessor*    m_pJoinFestaAttractionListSelectViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAATTRACTIONLISTSELECT_APPLICATIONSYSTEM_H_INCLUDE
