// ============================================================================
/*
 * @file JoinFestaSubScreenApplicationSystem.h
 * @brief ジョインフェスタ下画面のアプリケーションシステムです。
 * @date 2015.06.10
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_APPLICATIONSYSTEM_H_INCLUDE )
#define JOINFESTASUBSCREEN_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(View)
class JoinFestaSubScreenParentView;
GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(ApplicationSystem)

class JoinFestaSubScreenBuilder;

class JoinFestaSubScreenApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSubScreenApplicationSystem );

public:
  JoinFestaSubScreenApplicationSystem();
  virtual~JoinFestaSubScreenApplicationSystem();

  void Build( app::util::Heap* pAppHeap, NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenBuilder* pApplicationSystemBuilder );


public:

  //! 一時停止
  virtual void Suspend();
  //! 再起動
  virtual void Resume();

private:
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();

private:

  NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView*           m_pJoinFestaSubScreenParentView;

};


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTASUBSCREEN_APPLICATIONSYSTEM_H_INCLUDE
