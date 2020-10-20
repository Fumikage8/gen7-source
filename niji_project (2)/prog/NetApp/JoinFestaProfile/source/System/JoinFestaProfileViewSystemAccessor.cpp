// ============================================================================
/*
 * @file JoinFestaProfileViewSystemAccessor.cpp
 * @brief プロフィールアプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileViewSystemAccessor.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(System)


JoinFestaProfileViewSystemAccessor::JoinFestaProfileViewSystemAccessor(
  NetApp::JoinFestaProfile::System::JoinFestaProfileViewList* pJoinFestaProfileViewList,
  NetApp::JoinFestaProfile::System::JoinFestaProfileViewObjects* pJoinFestaProfileViewObjects ) :
  m_pJoinFestaProfileViewList( pJoinFestaProfileViewList ),
  m_pJoinFestaProfileViewObjects( pJoinFestaProfileViewObjects )
{
}


JoinFestaProfileViewSystemAccessor::~JoinFestaProfileViewSystemAccessor()
{
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)
