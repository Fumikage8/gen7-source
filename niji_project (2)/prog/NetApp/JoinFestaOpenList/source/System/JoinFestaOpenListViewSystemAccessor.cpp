// ============================================================================
/*
 * @file JoinFestaOpenListViewSystemAccessor.cpp
 * @brief 開催選択アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListViewSystemAccessor.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)
GFL_NAMESPACE_BEGIN(System)


JoinFestaOpenListViewSystemAccessor::JoinFestaOpenListViewSystemAccessor(
  NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewList* pJoinFestaOpenListViewList,
  NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewObjects* pJoinFestaOpenListViewObjects ) :
  m_pJoinFestaOpenListViewList( pJoinFestaOpenListViewList ),
  m_pJoinFestaOpenListViewObjects( pJoinFestaOpenListViewObjects )
{
}


JoinFestaOpenListViewSystemAccessor::~JoinFestaOpenListViewSystemAccessor()
{
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)
