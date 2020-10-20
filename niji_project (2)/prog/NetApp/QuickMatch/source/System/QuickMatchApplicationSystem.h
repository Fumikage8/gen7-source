// ============================================================================
/*
 * @file QuickMatchApplicationSystem.h
 * @brief クイックマッチアプリのアプリケーションシステムです。
 * @date 201X.XX.XX
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( QUICKMATCH_APPLICATIONSYSTEM_H_INCLUDE )
#define QUICKMATCH_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "NetApp/QuickMatch/source/System/QuickMatchWork.h"
#include "NetApp/QuickMatch/source/View/QuickMatchUpperView.h"
#include "NetApp/QuickMatch/source/View/QuickMatchLowerView.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(System)

class QuickMatchBuilder;

class QuickMatchApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( QuickMatchApplicationSystem );

public:
  QuickMatchApplicationSystem();
  virtual~QuickMatchApplicationSystem();

  void Build( NetApp::QuickMatch::System::QuickMatchWork* pQuickMatchWork , NetApp::QuickMatch::System::QuickMatchBuilder* pApplicationSystemBuilder );


private:
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();

private:
  NetApp::QuickMatch::System::QuickMatchWork* m_pQuickMatchWork;
  NetApp::QuickMatch::View::QuickMatchUpperView* m_pQuickMatchUpperView;
  NetApp::QuickMatch::View::QuickMatchLowerView* m_pQuickMatchLowerView;
};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)


#endif // QUICKMATCH_APPLICATIONSYSTEM_H_INCLUDE
