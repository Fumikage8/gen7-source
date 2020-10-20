// ============================================================================
/*
 * @file JoinFestaPlayerListSelectFrameBase.cpp
 * @brief プレイヤー単体選択アプリのフレームの基底クラスです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelect/source/Frame/JoinFestaPlayerListSelectFrameBase.h"
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectViewSystemAccessor.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaPlayerListSelectFrameBase::JoinFestaPlayerListSelectFrameBase(
  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectWork* pJoinFestaPlayerListSelectWork,
  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewSystemAccessor* pJoinFestaPlayerListSelectViewSystemAccessor ) :
  NetAppLib::System::NetAppFrameBase( pJoinFestaPlayerListSelectWork ),
  m_pJoinFestaPlayerListSelectWork( pJoinFestaPlayerListSelectWork ),
  m_pJoinFestaPlayerListSelectViewList( pJoinFestaPlayerListSelectViewSystemAccessor->GetJoinFestaPlayerListSelectViewList() ),
  m_pJoinFestaPlayerListSelectViewObjects( pJoinFestaPlayerListSelectViewSystemAccessor->GetJoinFestaPlayerListSelectViewObjects() )
{
}


JoinFestaPlayerListSelectFrameBase::~JoinFestaPlayerListSelectFrameBase()
{
}


void JoinFestaPlayerListSelectFrameBase::exitFrame( NetAppLib::System::FrameResult result )
{
  NetAppLib::System::NetAppFrameBase::exitFrame( result );

  m_pJoinFestaPlayerListSelectViewObjects->InputDisableAll();
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)
