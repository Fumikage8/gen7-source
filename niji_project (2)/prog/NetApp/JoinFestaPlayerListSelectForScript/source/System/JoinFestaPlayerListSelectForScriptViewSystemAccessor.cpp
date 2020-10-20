// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptViewSystemAccessor.cpp
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)のViewシステム関連のクラスを取得するクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelectForScript/source/System/JoinFestaPlayerListSelectForScriptViewSystemAccessor.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListSelectForScriptViewSystemAccessor::JoinFestaPlayerListSelectForScriptViewSystemAccessor(
  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewList* pJoinFestaPlayerListSelectForScriptViewList,
  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewObjects* pJoinFestaPlayerListSelectForScriptViewObjects ) :
  m_pJoinFestaPlayerListSelectForScriptViewList( pJoinFestaPlayerListSelectForScriptViewList ),
  m_pJoinFestaPlayerListSelectForScriptViewObjects( pJoinFestaPlayerListSelectForScriptViewObjects )
{
}


JoinFestaPlayerListSelectForScriptViewSystemAccessor::~JoinFestaPlayerListSelectForScriptViewSystemAccessor()
{
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)
