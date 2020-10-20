// ============================================================================
/*
 * @file JoinFestaAttractionListSelectBuilder.cpp
 * @brief アトラクション開催参加アプリを構築するクラスです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectBuilder.h"
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectFrameSwitcher.h"
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectFrameFactory.h"
#include "NetApp/JoinFestaAttractionListSelect/source/JoinFestaAttractionListSelectResourceID.h"
#include "NetApp/JoinFestaAttractionListSelect/source/JoinFestaAttractionListSelectFrameID.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)
GFL_NAMESPACE_BEGIN(System)


JoinFestaAttractionListSelectBuilder::JoinFestaAttractionListSelectBuilder( NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectWork* pJoinFestaAttractionListSelectWork ) :
  m_pJoinFestaAttractionListSelectWork( pJoinFestaAttractionListSelectWork ),
  m_pJoinFestaAttractionListSelectViewSystemAccessor( NULL )
{
}


JoinFestaAttractionListSelectBuilder::~JoinFestaAttractionListSelectBuilder()
{
}


NetAppLib::System::IApplicationFrameSwitcher* JoinFestaAttractionListSelectBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pJoinFestaAttractionListSelectWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectFrameSwitcher();
}


NetAppLib::System::IApplicationFrameFactory* JoinFestaAttractionListSelectBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pJoinFestaAttractionListSelectWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectFrameFactory( m_pJoinFestaAttractionListSelectWork, m_pJoinFestaAttractionListSelectViewSystemAccessor );
}


NetAppLib::System::ApplicationWorkBase* JoinFestaAttractionListSelectBuilder::GetApplicationWork()
{
  return m_pJoinFestaAttractionListSelectWork;
}


NetAppLib::System::FrameID JoinFestaAttractionListSelectBuilder::GetApplicationStartFrameID()
{
  return NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_FRAME_ID_OPEN;
}


u32 JoinFestaAttractionListSelectBuilder::GetApplicationResourceCount()
{
  return NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_RESOURCE_MAX;
}


bool JoinFestaAttractionListSelectBuilder::IsCreateMessageUtility()
{
  return true;
}


bool JoinFestaAttractionListSelectBuilder::IsCreateNetAppMessageMenu()
{
  return true;
}


bool JoinFestaAttractionListSelectBuilder::IsEnableInputExclusiveKeyMode()
{
  return true;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)
