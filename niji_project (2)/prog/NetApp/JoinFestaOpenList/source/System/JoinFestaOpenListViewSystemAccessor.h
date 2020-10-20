// ============================================================================
/*
 * @file JoinFestaOpenListViewSystemAccessor.h
 * @brief 開催選択アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAOPENLIST_VIEWSYSTEMACCESSOR_H_INCLUDE )
#define JOINFESTAOPENLIST_VIEWSYSTEMACCESSOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaOpenListViewList;
class JoinFestaOpenListViewObjects;

class JoinFestaOpenListViewSystemAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaOpenListViewSystemAccessor );

public:

  JoinFestaOpenListViewSystemAccessor(
    NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewList* pJoinFestaOpenListViewList,
    NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewObjects* pJoinFestaOpenListViewObjects );

  virtual~JoinFestaOpenListViewSystemAccessor();

  NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewList*  GetJoinFestaOpenListViewList() { return m_pJoinFestaOpenListViewList; }
  NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewObjects*  GetJoinFestaOpenListViewObjects() { return m_pJoinFestaOpenListViewObjects; }

private:

  NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewList*  m_pJoinFestaOpenListViewList;
  NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewObjects*  m_pJoinFestaOpenListViewObjects;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAOPENLIST_VIEWSYSTEMACCESSOR_H_INCLUDE
