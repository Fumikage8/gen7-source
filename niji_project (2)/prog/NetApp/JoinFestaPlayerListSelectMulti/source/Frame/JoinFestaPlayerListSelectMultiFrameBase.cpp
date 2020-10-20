// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiFrameBase.cpp
 * @brief プレイヤー複数選択アプリのフレームの基底クラスです。
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelectMulti/source/Frame/JoinFestaPlayerListSelectMultiFrameBase.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/System/JoinFestaPlayerListSelectMultiViewSystemAccessor.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaPlayerListSelectMultiFrameBase::JoinFestaPlayerListSelectMultiFrameBase(
  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiWork* pJoinFestaPlayerListSelectMultiWork,
  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewSystemAccessor* pJoinFestaPlayerListSelectMultiViewSystemAccessor ) :
  NetAppLib::System::NetAppFrameBase( pJoinFestaPlayerListSelectMultiWork ),
  m_pJoinFestaPlayerListSelectMultiWork( pJoinFestaPlayerListSelectMultiWork ),
  m_pJoinFestaPlayerListSelectMultiViewList( pJoinFestaPlayerListSelectMultiViewSystemAccessor->GetJoinFestaPlayerListSelectMultiViewList() ),
  m_pJoinFestaPlayerListSelectMultiViewObjects( pJoinFestaPlayerListSelectMultiViewSystemAccessor->GetJoinFestaPlayerListSelectMultiViewObjects() )
{
}


JoinFestaPlayerListSelectMultiFrameBase::~JoinFestaPlayerListSelectMultiFrameBase()
{
}


void JoinFestaPlayerListSelectMultiFrameBase::exitFrame( NetAppLib::System::FrameResult result )
{
  NetAppLib::System::NetAppFrameBase::exitFrame( result );

  m_pJoinFestaPlayerListSelectMultiViewObjects->InputDisableAll();
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)
