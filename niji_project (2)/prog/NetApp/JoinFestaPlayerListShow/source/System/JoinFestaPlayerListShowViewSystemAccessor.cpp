// ============================================================================
/*
 * @file JoinFestaPlayerListShowViewSystemAccessor.cpp
 * @brief 閲覧アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowViewSystemAccessor.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListShowViewSystemAccessor::JoinFestaPlayerListShowViewSystemAccessor(
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewList* pJoinFestaPlayerListShowViewList,
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewObjects* pJoinFestaPlayerListShowViewObjects ) :
  m_pJoinFestaPlayerListShowViewList( pJoinFestaPlayerListShowViewList ),
  m_pJoinFestaPlayerListShowViewObjects( pJoinFestaPlayerListShowViewObjects )
{
}


JoinFestaPlayerListShowViewSystemAccessor::~JoinFestaPlayerListShowViewSystemAccessor()
{
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)
