// ============================================================================
/*
 * @file JoinFestaAttractionResultViewSystemAccessor.cpp
 * @brief アトラクション結果アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultViewSystemAccessor.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)
GFL_NAMESPACE_BEGIN(System)


JoinFestaAttractionResultViewSystemAccessor::JoinFestaAttractionResultViewSystemAccessor(
  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewList* pJoinFestaAttractionResultViewList,
  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewObjects* pJoinFestaAttractionResultViewObjects ) :
  m_pJoinFestaAttractionResultViewList( pJoinFestaAttractionResultViewList ),
  m_pJoinFestaAttractionResultViewObjects( pJoinFestaAttractionResultViewObjects )
{
}


JoinFestaAttractionResultViewSystemAccessor::~JoinFestaAttractionResultViewSystemAccessor()
{
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)
