// ============================================================================
/*
 * @file JoinFestaAttractionListSelectViewSystemAccessor.h
 * @brief アトラクション開催参加アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONLISTSELECT_VIEWSYSTEMACCESSOR_H_INCLUDE )
#define JOINFESTAATTRACTIONLISTSELECT_VIEWSYSTEMACCESSOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaAttractionListSelectViewList;
class JoinFestaAttractionListSelectViewObjects;

class JoinFestaAttractionListSelectViewSystemAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionListSelectViewSystemAccessor );

public:

  JoinFestaAttractionListSelectViewSystemAccessor(
    NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewList* pJoinFestaAttractionListSelectViewList,
    NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewObjects* pJoinFestaAttractionListSelectViewObjects );

  virtual~JoinFestaAttractionListSelectViewSystemAccessor();

  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewList*  GetJoinFestaAttractionListSelectViewList() { return m_pJoinFestaAttractionListSelectViewList; }
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewObjects*  GetJoinFestaAttractionListSelectViewObjects() { return m_pJoinFestaAttractionListSelectViewObjects; }

private:

  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewList*  m_pJoinFestaAttractionListSelectViewList;
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewObjects*  m_pJoinFestaAttractionListSelectViewObjects;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAATTRACTIONLISTSELECT_VIEWSYSTEMACCESSOR_H_INCLUDE
