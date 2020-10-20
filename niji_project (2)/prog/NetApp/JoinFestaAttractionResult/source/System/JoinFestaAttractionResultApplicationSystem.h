// ============================================================================
/*
 * @file JoinFestaAttractionResultApplicationSystem.h
 * @brief アトラクション結果アプリのアプリケーションシステムです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONRESULT_APPLICATIONSYSTEM_H_INCLUDE )
#define JOINFESTAATTRACTIONRESULT_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaAttractionResultWork;
class JoinFestaAttractionResultBuilder;
class JoinFestaAttractionResultViewList;
class JoinFestaAttractionResultViewObjects;
class JoinFestaAttractionResultViewSystemAccessor;

class JoinFestaAttractionResultApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionResultApplicationSystem );

public:
  JoinFestaAttractionResultApplicationSystem();
  virtual~JoinFestaAttractionResultApplicationSystem();

  void Build( NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultWork* pJoinFestaAttractionResultWork, NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultBuilder* pApplicationSystemBuilder );


private:
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();

private:


  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultWork*                  m_pJoinFestaAttractionResultWork;
  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewList*              m_pJoinFestaAttractionResultViewList;
  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewObjects*           m_pJoinFestaAttractionResultViewObjects;
  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewSystemAccessor*    m_pJoinFestaAttractionResultViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAATTRACTIONRESULT_APPLICATIONSYSTEM_H_INCLUDE
