// ============================================================================
/*
 * @file JoinFestaGlobalAttractionBuilder.cpp
 * @brief グローバルアトラクションアプリを構築するクラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionBuilder.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionFrameSwitcher.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionFrameFactory.h"
#include "NetApp/JoinFestaGlobalAttraction/source/JoinFestaGlobalAttractionResourceID.h"
#include "NetApp/JoinFestaGlobalAttraction/source/JoinFestaGlobalAttractionFrameID.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(System)


JoinFestaGlobalAttractionBuilder::JoinFestaGlobalAttractionBuilder( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork ) :
  m_pJoinFestaGlobalAttractionWork( pJoinFestaGlobalAttractionWork ),
  m_pJoinFestaGlobalAttractionViewSystemAccessor( NULL )
{
}


JoinFestaGlobalAttractionBuilder::~JoinFestaGlobalAttractionBuilder()
{
}


NetAppLib::System::IApplicationFrameSwitcher* JoinFestaGlobalAttractionBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pJoinFestaGlobalAttractionWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionFrameSwitcher();
}


NetAppLib::System::IApplicationFrameFactory* JoinFestaGlobalAttractionBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pJoinFestaGlobalAttractionWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionFrameFactory( m_pJoinFestaGlobalAttractionWork, m_pJoinFestaGlobalAttractionViewSystemAccessor );
}


NetAppLib::System::ApplicationWorkBase* JoinFestaGlobalAttractionBuilder::GetApplicationWork()
{
  return m_pJoinFestaGlobalAttractionWork;
}


NetAppLib::System::FrameID JoinFestaGlobalAttractionBuilder::GetApplicationStartFrameID()
{
  return NetApp::JoinFestaGlobalAttraction::JOINFESTAGLOBALATTRACTION_FRAME_ID_MAIN;
}


u32 JoinFestaGlobalAttractionBuilder::GetApplicationResourceCount()
{
  return NetApp::JoinFestaGlobalAttraction::JOINFESTAGLOBALATTRACTION_RESOURCE_MAX;
}


bool JoinFestaGlobalAttractionBuilder::IsCreateMessageUtility()
{
  return true;
}


void JoinFestaGlobalAttractionBuilder::GetMessageUtilityParameter( u32& dataNum, u32& wordMax, u32& bufLen )
{
  dataNum = 4;
  wordMax = 16;
  bufLen  = 64;
}


bool JoinFestaGlobalAttractionBuilder::IsCreateNetAppMessageMenu()
{
  return true;
}


bool JoinFestaGlobalAttractionBuilder::IsEnableInputExclusiveKeyMode()
{
  return true;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)
