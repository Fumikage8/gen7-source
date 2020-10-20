// ============================================================================
/*
 * @file JoinFestaPlayerListSelectViewSystemAccessor.h
 * @brief プレイヤー単体選択アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECT_VIEWSYSTEMACCESSOR_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECT_VIEWSYSTEMACCESSOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaPlayerListSelectViewList;
class JoinFestaPlayerListSelectViewObjects;

class JoinFestaPlayerListSelectViewSystemAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectViewSystemAccessor );

public:

  JoinFestaPlayerListSelectViewSystemAccessor(
    NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewList* pJoinFestaPlayerListSelectViewList,
    NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewObjects* pJoinFestaPlayerListSelectViewObjects );

  virtual~JoinFestaPlayerListSelectViewSystemAccessor();

  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewList*  GetJoinFestaPlayerListSelectViewList() { return m_pJoinFestaPlayerListSelectViewList; }
  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewObjects*  GetJoinFestaPlayerListSelectViewObjects() { return m_pJoinFestaPlayerListSelectViewObjects; }

private:

  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewList*  m_pJoinFestaPlayerListSelectViewList;
  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewObjects*  m_pJoinFestaPlayerListSelectViewObjects;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAPLAYERLISTSELECT_VIEWSYSTEMACCESSOR_H_INCLUDE
