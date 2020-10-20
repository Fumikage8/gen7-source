// ============================================================================
/*
 * @file JoinFestaAttractionListSelectViewSystemAccessor.cpp
 * @brief アトラクション開催参加アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectViewSystemAccessor.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)
GFL_NAMESPACE_BEGIN(System)


JoinFestaAttractionListSelectViewSystemAccessor::JoinFestaAttractionListSelectViewSystemAccessor(
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewList* pJoinFestaAttractionListSelectViewList,
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewObjects* pJoinFestaAttractionListSelectViewObjects ) :
  m_pJoinFestaAttractionListSelectViewList( pJoinFestaAttractionListSelectViewList ),
  m_pJoinFestaAttractionListSelectViewObjects( pJoinFestaAttractionListSelectViewObjects )
{
}


JoinFestaAttractionListSelectViewSystemAccessor::~JoinFestaAttractionListSelectViewSystemAccessor()
{
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)
