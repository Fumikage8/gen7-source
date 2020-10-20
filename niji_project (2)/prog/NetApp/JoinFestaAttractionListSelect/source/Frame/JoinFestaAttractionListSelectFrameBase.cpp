// ============================================================================
/*
 * @file JoinFestaAttractionListSelectFrameBase.cpp
 * @brief アトラクション開催参加アプリのフレームの基底クラスです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionListSelect/source/Frame/JoinFestaAttractionListSelectFrameBase.h"
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectViewSystemAccessor.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaAttractionListSelectFrameBase::JoinFestaAttractionListSelectFrameBase(
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectWork* pJoinFestaAttractionListSelectWork,
  NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewSystemAccessor* pJoinFestaAttractionListSelectViewSystemAccessor ) :
  NetAppLib::System::NetAppFrameBase( pJoinFestaAttractionListSelectWork ),
  m_pJoinFestaAttractionListSelectWork( pJoinFestaAttractionListSelectWork ),
  m_pJoinFestaAttractionListSelectViewList( pJoinFestaAttractionListSelectViewSystemAccessor->GetJoinFestaAttractionListSelectViewList() ),
  m_pJoinFestaAttractionListSelectViewObjects( pJoinFestaAttractionListSelectViewSystemAccessor->GetJoinFestaAttractionListSelectViewObjects() )
{
}


JoinFestaAttractionListSelectFrameBase::~JoinFestaAttractionListSelectFrameBase()
{
}

void JoinFestaAttractionListSelectFrameBase::exitFrame( NetAppLib::System::FrameResult result )
{
  NetAppLib::System::NetAppFrameBase::exitFrame( result );

  m_pJoinFestaAttractionListSelectViewObjects->InputDisableAll();
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)
