// ============================================================================
/*
 * @file JoinFestaGlobalAttractionFrameBase.cpp
 * @brief グローバルアトラクションアプリのフレームの基底クラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaGlobalAttraction/source/Frame/JoinFestaGlobalAttractionFrameBase.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionViewSystemAccessor.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaGlobalAttractionFrameBase::JoinFestaGlobalAttractionFrameBase(
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork,
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewSystemAccessor* pJoinFestaGlobalAttractionViewSystemAccessor ) :
  NetAppLib::System::NetAppFrameBase( pJoinFestaGlobalAttractionWork ),
  m_pJoinFestaGlobalAttractionWork( pJoinFestaGlobalAttractionWork ),
  m_pJoinFestaGlobalAttractionViewList( pJoinFestaGlobalAttractionViewSystemAccessor->GetJoinFestaGlobalAttractionViewList() ),
  m_pJoinFestaGlobalAttractionViewObjects( pJoinFestaGlobalAttractionViewSystemAccessor->GetJoinFestaGlobalAttractionViewObjects() )
{
}


JoinFestaGlobalAttractionFrameBase::~JoinFestaGlobalAttractionFrameBase()
{
}


void JoinFestaGlobalAttractionFrameBase::exitFrame( NetAppLib::System::FrameResult result )
{
  NetAppLib::System::NetAppFrameBase::exitFrame( result );

  m_pJoinFestaGlobalAttractionViewObjects->InputDisableAll();
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)
