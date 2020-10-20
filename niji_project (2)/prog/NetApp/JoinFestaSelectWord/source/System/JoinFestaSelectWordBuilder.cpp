// ============================================================================
/*
 * @file JoinFestaSelectWordBuilder.cpp
 * @brief 挨拶選択アプリを構築するクラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordBuilder.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordFrameSwitcher.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordFrameFactory.h"
#include "NetApp/JoinFestaSelectWord/source/JoinFestaSelectWordResourceID.h"
#include "NetApp/JoinFestaSelectWord/source/JoinFestaSelectWordFrameID.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
// Save
#include "Savedata/include/JoinFestaDataSave.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(System)


JoinFestaSelectWordBuilder::JoinFestaSelectWordBuilder( NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork ) :
  m_pJoinFestaSelectWordWork( pJoinFestaSelectWordWork ),
  m_pJoinFestaSelectWordViewSystemAccessor( NULL )
{
}


JoinFestaSelectWordBuilder::~JoinFestaSelectWordBuilder()
{
}


NetAppLib::System::IApplicationFrameSwitcher* JoinFestaSelectWordBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pJoinFestaSelectWordWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordFrameSwitcher();
}


NetAppLib::System::IApplicationFrameFactory* JoinFestaSelectWordBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pJoinFestaSelectWordWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordFrameFactory( m_pJoinFestaSelectWordWork, m_pJoinFestaSelectWordViewSystemAccessor );
}


NetAppLib::System::ApplicationWorkBase* JoinFestaSelectWordBuilder::GetApplicationWork()
{
  return m_pJoinFestaSelectWordWork;
}


NetAppLib::System::FrameID JoinFestaSelectWordBuilder::GetApplicationStartFrameID()
{
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  NetAppLib::System::FrameID eStartFrameID = NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_FRAME_ID_MAIN;

  bool bIsFirstBootFlag = !pSave->GetSelectWordAppTutorial(); // 初回起動フラグをセーブデータから参照する

  if( bIsFirstBootFlag )
  {
    pSave->SetSelectWordAppTutorial();
    eStartFrameID = NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_FRAME_ID_DESCRIPTION;
  }

  return eStartFrameID;
}


u32 JoinFestaSelectWordBuilder::GetApplicationResourceCount()
{
  return NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_RESOURCE_MAX;
}


bool JoinFestaSelectWordBuilder::IsCreateMessageUtility()
{
  return true;
}


bool JoinFestaSelectWordBuilder::IsCreateNetAppMessageMenu()
{
  return true;
}


bool JoinFestaSelectWordBuilder::IsEnableInputExclusiveKeyMode()
{
  return true;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)
