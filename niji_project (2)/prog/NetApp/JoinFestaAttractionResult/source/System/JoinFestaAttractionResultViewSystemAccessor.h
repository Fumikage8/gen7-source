// ============================================================================
/*
 * @file JoinFestaAttractionResultViewSystemAccessor.h
 * @brief アトラクション結果アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONRESULT_VIEWSYSTEMACCESSOR_H_INCLUDE )
#define JOINFESTAATTRACTIONRESULT_VIEWSYSTEMACCESSOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaAttractionResultViewList;
class JoinFestaAttractionResultViewObjects;

class JoinFestaAttractionResultViewSystemAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionResultViewSystemAccessor );

public:

  JoinFestaAttractionResultViewSystemAccessor(
    NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewList* pJoinFestaAttractionResultViewList,
    NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewObjects* pJoinFestaAttractionResultViewObjects );

  virtual~JoinFestaAttractionResultViewSystemAccessor();

  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewList*  GetJoinFestaAttractionResultViewList() { return m_pJoinFestaAttractionResultViewList; }
  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewObjects*  GetJoinFestaAttractionResultViewObjects() { return m_pJoinFestaAttractionResultViewObjects; }

private:

  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewList*  m_pJoinFestaAttractionResultViewList;
  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewObjects*  m_pJoinFestaAttractionResultViewObjects;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAATTRACTIONRESULT_VIEWSYSTEMACCESSOR_H_INCLUDE
