// ============================================================================
/*
 * @file JoinFestaGlobalAttractionViewSystemAccessor.cpp
 * @brief グローバルアトラクションアプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionViewSystemAccessor.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(System)


JoinFestaGlobalAttractionViewSystemAccessor::JoinFestaGlobalAttractionViewSystemAccessor(
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewList* pJoinFestaGlobalAttractionViewList,
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewObjects* pJoinFestaGlobalAttractionViewObjects ) :
  m_pJoinFestaGlobalAttractionViewList( pJoinFestaGlobalAttractionViewList ),
  m_pJoinFestaGlobalAttractionViewObjects( pJoinFestaGlobalAttractionViewObjects )
{
}


JoinFestaGlobalAttractionViewSystemAccessor::~JoinFestaGlobalAttractionViewSystemAccessor()
{
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)
