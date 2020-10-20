// ============================================================================
/*
 * @file JoinFestaSelectWordFrameBase.cpp
 * @brief 挨拶選択アプリのフレームの基底クラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaSelectWord/source/Frame/JoinFestaSelectWordFrameBase.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordViewSystemAccessor.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaSelectWordFrameBase::JoinFestaSelectWordFrameBase(
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork,
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewSystemAccessor* pJoinFestaSelectWordViewSystemAccessor ) :
  NetAppLib::System::NetAppFrameBase( pJoinFestaSelectWordWork ),
  m_pJoinFestaSelectWordWork( pJoinFestaSelectWordWork ),
  m_pJoinFestaSelectWordViewList( pJoinFestaSelectWordViewSystemAccessor->GetJoinFestaSelectWordViewList() ),
  m_pJoinFestaSelectWordViewObjects( pJoinFestaSelectWordViewSystemAccessor->GetJoinFestaSelectWordViewObjects() )
{
}


JoinFestaSelectWordFrameBase::~JoinFestaSelectWordFrameBase()
{
}


void JoinFestaSelectWordFrameBase::exitFrame( NetAppLib::System::FrameResult result )
{
  NetAppLib::System::NetAppFrameBase::exitFrame( result );

  m_pJoinFestaSelectWordViewObjects->InputDisableAll();
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)
