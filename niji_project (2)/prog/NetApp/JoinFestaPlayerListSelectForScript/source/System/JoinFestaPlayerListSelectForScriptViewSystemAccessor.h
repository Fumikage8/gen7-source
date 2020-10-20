// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptViewSystemAccessor.h
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)のViewシステム関連のクラスを取得するクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTFORSCRIPT_VIEWSYSTEMACCESSOR_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTFORSCRIPT_VIEWSYSTEMACCESSOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaPlayerListSelectForScriptViewList;
class JoinFestaPlayerListSelectForScriptViewObjects;

class JoinFestaPlayerListSelectForScriptViewSystemAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectForScriptViewSystemAccessor );

public:

  JoinFestaPlayerListSelectForScriptViewSystemAccessor(
    NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewList* pJoinFestaPlayerListSelectForScriptViewList,
    NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewObjects* pJoinFestaPlayerListSelectForScriptViewObjects );

  virtual~JoinFestaPlayerListSelectForScriptViewSystemAccessor();

  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewList*  GetJoinFestaPlayerListSelectForScriptViewList() { return m_pJoinFestaPlayerListSelectForScriptViewList; }
  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewObjects*  GetJoinFestaPlayerListSelectForScriptViewObjects() { return m_pJoinFestaPlayerListSelectForScriptViewObjects; }

private:

  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewList*  m_pJoinFestaPlayerListSelectForScriptViewList;
  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewObjects*  m_pJoinFestaPlayerListSelectForScriptViewObjects;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAPLAYERLISTSELECTFORSCRIPT_VIEWSYSTEMACCESSOR_H_INCLUDE
