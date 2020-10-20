// ============================================================================
/*
 * @file JoinFestaProfileViewSystemAccessor.h
 * @brief プロフィールアプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPROFILE_VIEWSYSTEMACCESSOR_H_INCLUDE )
#define JOINFESTAPROFILE_VIEWSYSTEMACCESSOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(System)

class JoinFestaProfileViewList;
class JoinFestaProfileViewObjects;

class JoinFestaProfileViewSystemAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaProfileViewSystemAccessor );

public:

  JoinFestaProfileViewSystemAccessor(
    NetApp::JoinFestaProfile::System::JoinFestaProfileViewList* pJoinFestaProfileViewList,
    NetApp::JoinFestaProfile::System::JoinFestaProfileViewObjects* pJoinFestaProfileViewObjects );

  virtual~JoinFestaProfileViewSystemAccessor();

  NetApp::JoinFestaProfile::System::JoinFestaProfileViewList*  GetJoinFestaProfileViewList() { return m_pJoinFestaProfileViewList; }
  NetApp::JoinFestaProfile::System::JoinFestaProfileViewObjects*  GetJoinFestaProfileViewObjects() { return m_pJoinFestaProfileViewObjects; }

private:

  NetApp::JoinFestaProfile::System::JoinFestaProfileViewList*  m_pJoinFestaProfileViewList;
  NetApp::JoinFestaProfile::System::JoinFestaProfileViewObjects*  m_pJoinFestaProfileViewObjects;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAPROFILE_VIEWSYSTEMACCESSOR_H_INCLUDE
