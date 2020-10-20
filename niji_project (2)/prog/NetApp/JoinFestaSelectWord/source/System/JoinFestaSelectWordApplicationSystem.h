// ============================================================================
/*
 * @file JoinFestaSelectWordApplicationSystem.h
 * @brief 挨拶選択アプリのアプリケーションシステムです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORD_APPLICATIONSYSTEM_H_INCLUDE )
#define JOINFESTASELECTWORD_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaSelectWordWork;
class JoinFestaSelectWordBuilder;
class JoinFestaSelectWordViewList;
class JoinFestaSelectWordViewObjects;
class JoinFestaSelectWordViewSystemAccessor;

class JoinFestaSelectWordApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSelectWordApplicationSystem );

public:
  JoinFestaSelectWordApplicationSystem();
  virtual~JoinFestaSelectWordApplicationSystem();

  void Build( NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork, NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordBuilder* pApplicationSystemBuilder );


private:
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();

private:

  static const u32    MESSAGE_NUM   = 1;

  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork*                  m_pJoinFestaSelectWordWork;
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewList*              m_pJoinFestaSelectWordViewList;
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewObjects*           m_pJoinFestaSelectWordViewObjects;
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewSystemAccessor*    m_pJoinFestaSelectWordViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTASELECTWORD_APPLICATIONSYSTEM_H_INCLUDE
