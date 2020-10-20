// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptApplicationSystem.h
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)のアプリケーションシステムです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTFORSCRIPT_APPLICATIONSYSTEM_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTFORSCRIPT_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <NetStatic/NetAppLib/include/System/ApplicationSystemBase.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Tool/AppCommonGrpIconData.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaPlayerListSelectForScriptWork;
class JoinFestaPlayerListSelectForScriptBuilder;
class JoinFestaPlayerListSelectForScriptViewList;
class JoinFestaPlayerListSelectForScriptViewObjects;
class JoinFestaPlayerListSelectForScriptViewSystemAccessor;

class JoinFestaPlayerListSelectForScriptApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectForScriptApplicationSystem );

public:
  JoinFestaPlayerListSelectForScriptApplicationSystem();
  virtual~JoinFestaPlayerListSelectForScriptApplicationSystem();

  void Build( NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptWork* pJoinFestaPlayerListSelectForScriptWork, NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptBuilder* pApplicationSystemBuilder );

  /*
    super
  */
//  virtual void Build( NetAppLib::System::IApplicationSystemBuilder* pApplicationSystemBuilder );
  virtual bool Loading();
//  virtual bool Update();
//  virtual void PreDraw(){}
//  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo ){}
//  virtual bool End();

private:
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();
  virtual bool IsCreateViewComplete();

private:

  static const u32    MESSAGE_NUM   = 1;

  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptWork*                  m_pJoinFestaPlayerListSelectForScriptWork;
  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewList*              m_pJoinFestaPlayerListSelectForScriptViewList;
  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewObjects*           m_pJoinFestaPlayerListSelectForScriptViewObjects;
  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewSystemAccessor*    m_pJoinFestaPlayerListSelectForScriptViewSystemAccessor;

  u32                                       m_LoadSeq;
  app::tool::AppCommonGrpIconData           m_CommonIconData;
  bool                                      m_IsCommonIconDataTerminated;
};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAPLAYERLISTSELECTFORSCRIPT_APPLICATIONSYSTEM_H_INCLUDE
