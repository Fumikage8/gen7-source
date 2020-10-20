// ============================================================================
/*
 * @file JoinFestaGlobalAttractionApplicationSystem.h
 * @brief グローバルアトラクションアプリのアプリケーションシステムです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAGLOBALATTRACTION_APPLICATIONSYSTEM_H_INCLUDE )
#define JOINFESTAGLOBALATTRACTION_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaGlobalAttractionWork;
class JoinFestaGlobalAttractionBuilder;
class JoinFestaGlobalAttractionViewList;
class JoinFestaGlobalAttractionViewObjects;
class JoinFestaGlobalAttractionViewSystemAccessor;

class JoinFestaGlobalAttractionApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaGlobalAttractionApplicationSystem );

public:
  JoinFestaGlobalAttractionApplicationSystem();
  virtual~JoinFestaGlobalAttractionApplicationSystem();

  void Build( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork, NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionBuilder* pApplicationSystemBuilder );


private:
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();

private:


  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork*                  m_pJoinFestaGlobalAttractionWork;
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewList*              m_pJoinFestaGlobalAttractionViewList;
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewObjects*           m_pJoinFestaGlobalAttractionViewObjects;
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewSystemAccessor*    m_pJoinFestaGlobalAttractionViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAGLOBALATTRACTION_APPLICATIONSYSTEM_H_INCLUDE
