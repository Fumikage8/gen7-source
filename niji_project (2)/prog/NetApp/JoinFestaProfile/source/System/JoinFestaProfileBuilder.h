// ============================================================================
/*
 * @file JoinFestaProfileBuilder.h
 * @brief プロフィールアプリを構築するクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPROFILE_BUILDER_H_INCLUDE )
#define JOINFESTAPROFILE_BUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaProfileViewSystemAccessor;


class JoinFestaProfileBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaProfileBuilder );
public:
  JoinFestaProfileBuilder( NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork );
  virtual~JoinFestaProfileBuilder();

  void SetViewSystemAccessor( NetApp::JoinFestaProfile::System::JoinFestaProfileViewSystemAccessor* pJoinFestaProfileViewSystemAccessor )
  {
    m_pJoinFestaProfileViewSystemAccessor = pJoinFestaProfileViewSystemAccessor;
  }

  virtual NetAppLib::System::IApplicationFrameSwitcher* CreateApplicationFrameSwitcher();
  virtual NetAppLib::System::IApplicationFrameFactory* CreateApplicationFrameFactory();

  virtual NetAppLib::System::ApplicationWorkBase* GetApplicationWork();
  virtual NetAppLib::System::FrameID GetApplicationStartFrameID();
  virtual u32 GetApplicationResourceCount();

  virtual bool IsCreateMessageUtility();
  virtual bool IsCreateNetAppMessageMenu();

  virtual bool IsEnableInputExclusiveKeyMode();

private:
  NetApp::JoinFestaProfile::System::JoinFestaProfileWork*                m_pJoinFestaProfileWork;
  NetApp::JoinFestaProfile::System::JoinFestaProfileViewSystemAccessor*  m_pJoinFestaProfileViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEBUILDER_H_INCLUDE
