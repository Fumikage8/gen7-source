// ============================================================================
/*
 * @file JoinFestaProfileUpperView.h
 * @brief プロフィールアプリの上画面を表示するクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPROFILEUPPERVIEW_H_INCLUDE )
#define JOINFESTAPROFILEUPPERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaProfileUpperView :
  public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaProfileUpperView );
public:
  JoinFestaProfileUpperView( NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork );
  virtual~JoinFestaProfileUpperView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  void UpdateFestaNameTextPane();

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JoinFestaProfile,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_UPPER,
  };

  void setupTextMessage();

private:

  NetApp::JoinFestaProfile::System::JoinFestaProfileWork*                         m_pJoinFestaProfileWork;
  gfl2::str::StrBuf                                                               m_JoinFestaName;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPROFILEUPPERVIEW_H_INCLUDE
