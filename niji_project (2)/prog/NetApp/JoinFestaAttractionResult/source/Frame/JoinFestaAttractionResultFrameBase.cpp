// ============================================================================
/*
 * @file JoinFestaAttractionResultFrameBase.cpp
 * @brief アトラクション結果アプリのフレームの基底クラスです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionResult/source/Frame/JoinFestaAttractionResultFrameBase.h"
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultViewSystemAccessor.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaAttractionResultFrameBase::JoinFestaAttractionResultFrameBase(
  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultWork* pJoinFestaAttractionResultWork,
  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewSystemAccessor* pJoinFestaAttractionResultViewSystemAccessor ) :
  NetAppLib::System::NetAppFrameBase( pJoinFestaAttractionResultWork ),
  m_pJoinFestaAttractionResultWork( pJoinFestaAttractionResultWork ),
  m_pJoinFestaAttractionResultViewList( pJoinFestaAttractionResultViewSystemAccessor->GetJoinFestaAttractionResultViewList() ),
  m_pJoinFestaAttractionResultViewObjects( pJoinFestaAttractionResultViewSystemAccessor->GetJoinFestaAttractionResultViewObjects() )
{
}


JoinFestaAttractionResultFrameBase::~JoinFestaAttractionResultFrameBase()
{
}


void JoinFestaAttractionResultFrameBase::exitFrame( NetAppLib::System::FrameResult result )
{
  NetAppLib::System::NetAppFrameBase::exitFrame( result );

  m_pJoinFestaAttractionResultViewObjects->InputDisableAll();
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)
