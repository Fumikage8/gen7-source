// ============================================================================
/*
 * @file JoinFestaProfileFrameBase.cpp
 * @brief プロフィールアプリのフレームの基底クラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaProfile/source/Frame/JoinFestaProfileFrameBase.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileViewSystemAccessor.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaProfileFrameBase::JoinFestaProfileFrameBase(
  NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork,
  NetApp::JoinFestaProfile::System::JoinFestaProfileViewSystemAccessor* pJoinFestaProfileViewSystemAccessor ) :
  NetAppLib::System::NetAppFrameBase( pJoinFestaProfileWork ),
  m_pJoinFestaProfileWork( pJoinFestaProfileWork ),
  m_pJoinFestaProfileViewList( pJoinFestaProfileViewSystemAccessor->GetJoinFestaProfileViewList() ),
  m_pJoinFestaProfileViewObjects( pJoinFestaProfileViewSystemAccessor->GetJoinFestaProfileViewObjects() )
{
}


JoinFestaProfileFrameBase::~JoinFestaProfileFrameBase()
{
}


void JoinFestaProfileFrameBase::exitFrame( NetAppLib::System::FrameResult result )
{
  NetAppLib::System::NetAppFrameBase::exitFrame( result );

  m_pJoinFestaProfileViewObjects->InputDisableAll();
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)
