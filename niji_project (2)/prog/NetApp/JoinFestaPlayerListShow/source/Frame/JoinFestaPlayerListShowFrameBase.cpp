// ============================================================================
/*
 * @file JoinFestaPlayerListShowFrameBase.cpp
 * @brief 閲覧アプリのフレームの基底クラスです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListShow/source/Frame/JoinFestaPlayerListShowFrameBase.h"
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowViewSystemAccessor.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaPlayerListShowFrameBase::JoinFestaPlayerListShowFrameBase(
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowWork* pJoinFestaPlayerListShowWork,
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewSystemAccessor* pJoinFestaPlayerListShowViewSystemAccessor ) :
  NetAppLib::System::NetAppFrameBase( pJoinFestaPlayerListShowWork ),
  m_pJoinFestaPlayerListShowWork( pJoinFestaPlayerListShowWork ),
  m_pJoinFestaPlayerListShowViewList( pJoinFestaPlayerListShowViewSystemAccessor->GetJoinFestaPlayerListShowViewList() ),
  m_pJoinFestaPlayerListShowViewObjects( pJoinFestaPlayerListShowViewSystemAccessor->GetJoinFestaPlayerListShowViewObjects() )
{
}


JoinFestaPlayerListShowFrameBase::~JoinFestaPlayerListShowFrameBase()
{
}


void JoinFestaPlayerListShowFrameBase::exitFrame( NetAppLib::System::FrameResult result )
{
  NetAppLib::System::NetAppFrameBase::exitFrame( result );

  m_pJoinFestaPlayerListShowViewObjects->InputDisableAll();
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)
