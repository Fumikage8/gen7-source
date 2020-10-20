// ============================================================================
/*
 * @file JoinFestaPlayerDetailViewSystemAccessor.h
 * @brief 詳細アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERDETAIL_VIEWSYSTEMACCESSOR_H_INCLUDE )
#define JOINFESTAPLAYERDETAIL_VIEWSYSTEMACCESSOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaPlayerDetailViewList;
class JoinFestaPlayerDetailViewObjects;

class JoinFestaPlayerDetailViewSystemAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerDetailViewSystemAccessor );

public:

  JoinFestaPlayerDetailViewSystemAccessor(
    NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewList* pJoinFestaPlayerDetailViewList,
    NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewObjects* pJoinFestaPlayerDetailViewObjects );

  virtual~JoinFestaPlayerDetailViewSystemAccessor();

  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewList*  GetJoinFestaPlayerDetailViewList() { return m_pJoinFestaPlayerDetailViewList; }
  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewObjects*  GetJoinFestaPlayerDetailViewObjects() { return m_pJoinFestaPlayerDetailViewObjects; }

private:

  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewList*  m_pJoinFestaPlayerDetailViewList;
  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewObjects*  m_pJoinFestaPlayerDetailViewObjects;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAPLAYERDETAIL_VIEWSYSTEMACCESSOR_H_INCLUDE
