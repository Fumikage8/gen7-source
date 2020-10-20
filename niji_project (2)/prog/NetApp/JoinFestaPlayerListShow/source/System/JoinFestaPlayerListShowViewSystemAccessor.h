// ============================================================================
/*
 * @file JoinFestaPlayerListShowViewSystemAccessor.h
 * @brief 閲覧アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSHOW_VIEWSYSTEMACCESSOR_H_INCLUDE )
#define JOINFESTAPLAYERLISTSHOW_VIEWSYSTEMACCESSOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaPlayerListShowViewList;
class JoinFestaPlayerListShowViewObjects;

class JoinFestaPlayerListShowViewSystemAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListShowViewSystemAccessor );

public:

  JoinFestaPlayerListShowViewSystemAccessor(
    NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewList* pJoinFestaPlayerListShowViewList,
    NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewObjects* pJoinFestaPlayerListShowViewObjects );

  virtual~JoinFestaPlayerListShowViewSystemAccessor();

  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewList*  GetJoinFestaPlayerListShowViewList() { return m_pJoinFestaPlayerListShowViewList; }
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewObjects*  GetJoinFestaPlayerListShowViewObjects() { return m_pJoinFestaPlayerListShowViewObjects; }

private:

  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewList*  m_pJoinFestaPlayerListShowViewList;
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewObjects*  m_pJoinFestaPlayerListShowViewObjects;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAPLAYERLISTSHOW_VIEWSYSTEMACCESSOR_H_INCLUDE
