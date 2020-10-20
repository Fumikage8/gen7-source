// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiViewSystemAccessor.h
 * @brief プレイヤー複数選択アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTMULTI_VIEWSYSTEMACCESSOR_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTMULTI_VIEWSYSTEMACCESSOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaPlayerListSelectMultiViewList;
class JoinFestaPlayerListSelectMultiViewObjects;

class JoinFestaPlayerListSelectMultiViewSystemAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectMultiViewSystemAccessor );

public:

  JoinFestaPlayerListSelectMultiViewSystemAccessor(
    NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewList* pJoinFestaPlayerListSelectMultiViewList,
    NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewObjects* pJoinFestaPlayerListSelectMultiViewObjects );

  virtual~JoinFestaPlayerListSelectMultiViewSystemAccessor();

  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewList*  GetJoinFestaPlayerListSelectMultiViewList() { return m_pJoinFestaPlayerListSelectMultiViewList; }
  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewObjects*  GetJoinFestaPlayerListSelectMultiViewObjects() { return m_pJoinFestaPlayerListSelectMultiViewObjects; }

private:

  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewList*  m_pJoinFestaPlayerListSelectMultiViewList;
  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewObjects*  m_pJoinFestaPlayerListSelectMultiViewObjects;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAPLAYERLISTSELECTMULTI_VIEWSYSTEMACCESSOR_H_INCLUDE
