// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiViewSystemAccessor.cpp
 * @brief プレイヤー複数選択アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelectMulti/source/System/JoinFestaPlayerListSelectMultiViewSystemAccessor.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListSelectMultiViewSystemAccessor::JoinFestaPlayerListSelectMultiViewSystemAccessor(
  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewList* pJoinFestaPlayerListSelectMultiViewList,
  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewObjects* pJoinFestaPlayerListSelectMultiViewObjects ) :
  m_pJoinFestaPlayerListSelectMultiViewList( pJoinFestaPlayerListSelectMultiViewList ),
  m_pJoinFestaPlayerListSelectMultiViewObjects( pJoinFestaPlayerListSelectMultiViewObjects )
{
}


JoinFestaPlayerListSelectMultiViewSystemAccessor::~JoinFestaPlayerListSelectMultiViewSystemAccessor()
{
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)
