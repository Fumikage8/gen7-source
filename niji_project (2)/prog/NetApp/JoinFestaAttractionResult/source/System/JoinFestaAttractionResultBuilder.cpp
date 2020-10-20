// ============================================================================
/*
 * @file JoinFestaAttractionResultBuilder.cpp
 * @brief アトラクション結果アプリを構築するクラスです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultBuilder.h"
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultFrameSwitcher.h"
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultFrameFactory.h"
#include "NetApp/JoinFestaAttractionResult/source/JoinFestaAttractionResultResourceID.h"
#include "NetApp/JoinFestaAttractionResult/source/JoinFestaAttractionResultFrameID.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)
GFL_NAMESPACE_BEGIN(System)


JoinFestaAttractionResultBuilder::JoinFestaAttractionResultBuilder( NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultWork* pJoinFestaAttractionResultWork ) :
  m_pJoinFestaAttractionResultWork( pJoinFestaAttractionResultWork ),
  m_pJoinFestaAttractionResultViewSystemAccessor( NULL )
{
}


JoinFestaAttractionResultBuilder::~JoinFestaAttractionResultBuilder()
{
}


NetAppLib::System::IApplicationFrameSwitcher* JoinFestaAttractionResultBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pJoinFestaAttractionResultWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultFrameSwitcher();
}


NetAppLib::System::IApplicationFrameFactory* JoinFestaAttractionResultBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pJoinFestaAttractionResultWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultFrameFactory( m_pJoinFestaAttractionResultWork, m_pJoinFestaAttractionResultViewSystemAccessor );
}


NetAppLib::System::ApplicationWorkBase* JoinFestaAttractionResultBuilder::GetApplicationWork()
{
  return m_pJoinFestaAttractionResultWork;
}


NetAppLib::System::FrameID JoinFestaAttractionResultBuilder::GetApplicationStartFrameID()
{
  return NetApp::JoinFestaAttractionResult::JOINFESTAATTRACTIONRESULT_FRAME_ID_RESULT;
}


u32 JoinFestaAttractionResultBuilder::GetApplicationResourceCount()
{
  return NetApp::JoinFestaAttractionResult::JOINFESTAATTRACTIONRESULT_RESOURCE_MAX;
}


bool JoinFestaAttractionResultBuilder::IsCreateMessageUtility()
{
  return true;
}


bool JoinFestaAttractionResultBuilder::IsCreateNetAppMessageMenu()
{
  return true;
}


bool JoinFestaAttractionResultBuilder::IsEnableInputExclusiveKeyMode()
{
  return true;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)
