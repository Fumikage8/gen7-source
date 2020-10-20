// ============================================================================
/*
 * @file JoinFestaOpenListFrameBase.cpp
 * @brief 開催選択アプリのフレームの基底クラスです。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaOpenList/source/Frame/JoinFestaOpenListFrameBase.h"
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListViewSystemAccessor.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaOpenListFrameBase::JoinFestaOpenListFrameBase(
  NetApp::JoinFestaOpenList::System::JoinFestaOpenListWork* pJoinFestaOpenListWork,
  NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewSystemAccessor* pJoinFestaOpenListViewSystemAccessor ) :
  NetAppLib::System::NetAppFrameBase( pJoinFestaOpenListWork ),
  m_pJoinFestaOpenListWork( pJoinFestaOpenListWork ),
  m_pJoinFestaOpenListViewList( pJoinFestaOpenListViewSystemAccessor->GetJoinFestaOpenListViewList() ),
  m_pJoinFestaOpenListViewObjects( pJoinFestaOpenListViewSystemAccessor->GetJoinFestaOpenListViewObjects() )
{
}


JoinFestaOpenListFrameBase::~JoinFestaOpenListFrameBase()
{
}


void JoinFestaOpenListFrameBase::exitFrame( NetAppLib::System::FrameResult result )
{
  NetAppLib::System::NetAppFrameBase::exitFrame( result );

  m_pJoinFestaOpenListViewObjects->InputDisableAll();
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)
