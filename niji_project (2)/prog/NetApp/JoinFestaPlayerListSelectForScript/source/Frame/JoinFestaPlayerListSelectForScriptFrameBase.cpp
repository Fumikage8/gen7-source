// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptFrameBase.cpp
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)のフレームの基底クラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelectForScript/source/Frame/JoinFestaPlayerListSelectForScriptFrameBase.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/System/JoinFestaPlayerListSelectForScriptViewSystemAccessor.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaPlayerListSelectForScriptFrameBase::JoinFestaPlayerListSelectForScriptFrameBase(
  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptWork* pJoinFestaPlayerListSelectForScriptWork,
  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewSystemAccessor* pJoinFestaPlayerListSelectForScriptViewSystemAccessor ) :
  NetAppLib::System::NetAppFrameBase( pJoinFestaPlayerListSelectForScriptWork ),
  m_pJoinFestaPlayerListSelectForScriptWork( pJoinFestaPlayerListSelectForScriptWork ),
  m_pJoinFestaPlayerListSelectForScriptViewList( pJoinFestaPlayerListSelectForScriptViewSystemAccessor->GetJoinFestaPlayerListSelectForScriptViewList() ),
  m_pJoinFestaPlayerListSelectForScriptViewObjects( pJoinFestaPlayerListSelectForScriptViewSystemAccessor->GetJoinFestaPlayerListSelectForScriptViewObjects() )
{
}


JoinFestaPlayerListSelectForScriptFrameBase::~JoinFestaPlayerListSelectForScriptFrameBase()
{
}


void JoinFestaPlayerListSelectForScriptFrameBase::exitFrame( NetAppLib::System::FrameResult result )
{
  NetAppLib::System::NetAppFrameBase::exitFrame( result );

  m_pJoinFestaPlayerListSelectForScriptViewObjects->InputDisableAll();
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)
