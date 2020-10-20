// ============================================================================
/*
 * @file RegulationSelectBuilder.cpp
 * @brief レギュレーション選択アプリを構築するクラスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectBuilder.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectFrameSwitcher.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectFrameFactory.h"
#include "NetApp/RegulationSelect/source/RegulationSelectResourceID.h"
#include "NetApp/RegulationSelect/source/RegulationSelectFrameID.h"
#include "NetApp/RegulationSelect/source/RegulationSelectDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


RegulationSelectBuilder::RegulationSelectBuilder( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork ) :
  m_pRegulationSelectWork( pRegulationSelectWork ),
  m_pRegulationSelectViewObjects( NULL )
{
}


RegulationSelectBuilder::~RegulationSelectBuilder()
{
}


NetAppLib::System::IApplicationFrameSwitcher* RegulationSelectBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pRegulationSelectWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::RegulationSelect::ApplicationSystem::RegulationSelectFrameSwitcher();
}


NetAppLib::System::IApplicationFrameFactory* RegulationSelectBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pRegulationSelectWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::RegulationSelect::ApplicationSystem::RegulationSelectFrameFactory( m_pRegulationSelectWork, m_pRegulationSelectViewObjects );
}


NetAppLib::System::ApplicationWorkBase* RegulationSelectBuilder::GetApplicationWork()
{
  return m_pRegulationSelectWork;
}


NetAppLib::System::FrameID RegulationSelectBuilder::GetApplicationStartFrameID()
{
  NetAppLib::System::FrameID startFrameID = NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_SELECT_MAIN;
  switch( m_pRegulationSelectWork->GetBootMode() )
  {
    case NetApp::RegulationSelect::BOOT_MODE_SELECT_P2P:
      startFrameID = NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_SELECT_LOAD;
    break;
    
    case NetApp::RegulationSelect::BOOT_MODE_SELECT_BATTLE_CENTER:
      startFrameID = NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_SELECT_MAIN;
    break;
    
    case NetApp::RegulationSelect::BOOT_MODE_SELECT_BATTLE_ROYAL:
      startFrameID = NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_SELECT_MAIN;
    break;
    
    case NetApp::RegulationSelect::BOOT_MODE_DOWNLOAD_P2P:
      startFrameID = NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_DL_LOAD;
    break;
    
    case NetApp::RegulationSelect::BOOT_MODE_DETAIL_BATTLE_TEAM:
      startFrameID = NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_DETAIL;
    break;
  }
  return startFrameID;
}


u32 RegulationSelectBuilder::GetApplicationResourceCount()
{
  return NetApp::RegulationSelect::REGULATIONSELECT_RESOURCE_MAX;
}


bool RegulationSelectBuilder::IsCreateMessageUtility()
{
  return true;
}


void RegulationSelectBuilder::GetMessageUtilityParameter( u32& dataNum, u32& wordMax, u32& bufLen )
{
  dataNum = 4;
  wordMax = 16;
  bufLen  = 64;
}


bool RegulationSelectBuilder::IsCreateNetAppMessageMenu()
{
  return true;
}


bool RegulationSelectBuilder::IsEnableInputExclusiveKeyMode()
{
  return true;
}


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)
