// ============================================================================
/*
 * @file JoinFestaPlayerListSelectViewSystemAccessor.cpp
 * @brief プレイヤー単体選択アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectViewSystemAccessor.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListSelectViewSystemAccessor::JoinFestaPlayerListSelectViewSystemAccessor(
  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewList* pJoinFestaPlayerListSelectViewList,
  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewObjects* pJoinFestaPlayerListSelectViewObjects ) :
  m_pJoinFestaPlayerListSelectViewList( pJoinFestaPlayerListSelectViewList ),
  m_pJoinFestaPlayerListSelectViewObjects( pJoinFestaPlayerListSelectViewObjects )
{
}


JoinFestaPlayerListSelectViewSystemAccessor::~JoinFestaPlayerListSelectViewSystemAccessor()
{
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)
