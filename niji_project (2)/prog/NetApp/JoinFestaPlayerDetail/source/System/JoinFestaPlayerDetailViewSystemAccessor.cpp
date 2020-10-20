// ============================================================================
/*
 * @file JoinFestaPlayerDetailViewSystemAccessor.cpp
 * @brief 詳細アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailViewSystemAccessor.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerDetailViewSystemAccessor::JoinFestaPlayerDetailViewSystemAccessor(
  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewList* pJoinFestaPlayerDetailViewList,
  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewObjects* pJoinFestaPlayerDetailViewObjects ) :
  m_pJoinFestaPlayerDetailViewList( pJoinFestaPlayerDetailViewList ),
  m_pJoinFestaPlayerDetailViewObjects( pJoinFestaPlayerDetailViewObjects )
{
}


JoinFestaPlayerDetailViewSystemAccessor::~JoinFestaPlayerDetailViewSystemAccessor()
{
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)
