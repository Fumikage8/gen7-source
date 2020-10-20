// ============================================================================
/*
 * @file JoinFestaPlayerDetailFrameBase.cpp
 * @brief 詳細アプリのフレームの基底クラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerDetail/source/Frame/JoinFestaPlayerDetailFrameBase.h"
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailViewSystemAccessor.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaPlayerDetailFrameBase::JoinFestaPlayerDetailFrameBase(
  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailWork* pJoinFestaPlayerDetailWork,
  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewSystemAccessor* pJoinFestaPlayerDetailViewSystemAccessor ) :
  NetAppLib::System::NetAppFrameBase( pJoinFestaPlayerDetailWork ),
  m_pJoinFestaPlayerDetailWork( pJoinFestaPlayerDetailWork ),
  m_pJoinFestaPlayerDetailViewList( pJoinFestaPlayerDetailViewSystemAccessor->GetJoinFestaPlayerDetailViewList() ),
  m_pJoinFestaPlayerDetailViewObjects( pJoinFestaPlayerDetailViewSystemAccessor->GetJoinFestaPlayerDetailViewObjects() )
{
}


JoinFestaPlayerDetailFrameBase::~JoinFestaPlayerDetailFrameBase()
{
}


void JoinFestaPlayerDetailFrameBase::exitFrame( NetAppLib::System::FrameResult result )
{
  NetAppLib::System::NetAppFrameBase::exitFrame( result );

  m_pJoinFestaPlayerDetailViewObjects->InputDisableAll();
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)
