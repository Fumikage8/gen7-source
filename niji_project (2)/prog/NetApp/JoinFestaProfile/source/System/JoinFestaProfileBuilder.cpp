// ============================================================================
/*
 * @file JoinFestaProfileBuilder.cpp
 * @brief プロフィールアプリを構築するクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileBuilder.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileFrameSwitcher.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileFrameFactory.h"
#include "NetApp/JoinFestaProfile/source/JoinFestaProfileResourceID.h"
#include "NetApp/JoinFestaProfile/source/JoinFestaProfileFrameID.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
// Save
#include "Savedata/include/JoinFestaDataSave.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(System)


JoinFestaProfileBuilder::JoinFestaProfileBuilder( NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork ) :
  m_pJoinFestaProfileWork( pJoinFestaProfileWork ),
  m_pJoinFestaProfileViewSystemAccessor( NULL )
{
}


JoinFestaProfileBuilder::~JoinFestaProfileBuilder()
{
}


NetAppLib::System::IApplicationFrameSwitcher* JoinFestaProfileBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pJoinFestaProfileWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaProfile::System::JoinFestaProfileFrameSwitcher();
}


NetAppLib::System::IApplicationFrameFactory* JoinFestaProfileBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pJoinFestaProfileWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaProfile::System::JoinFestaProfileFrameFactory( m_pJoinFestaProfileWork, m_pJoinFestaProfileViewSystemAccessor );
}


NetAppLib::System::ApplicationWorkBase* JoinFestaProfileBuilder::GetApplicationWork()
{
  return m_pJoinFestaProfileWork;
}


NetAppLib::System::FrameID JoinFestaProfileBuilder::GetApplicationStartFrameID()
{
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  NetAppLib::System::FrameID eStartFrameID = NetApp::JoinFestaProfile::JOINFESTAPROFILE_FRAME_ID_SELECT_EDIT_MENU;

  bool bIsFirstBootFlag = !pSave->GetProfileAppTutorialFlag(); // 初回起動フラグをセーブデータから参照する

  if( bIsFirstBootFlag )
  {
    pSave->SetProfileAppTutorialFlag();
    eStartFrameID = NetApp::JoinFestaProfile::JOINFESTAPROFILE_FRAME_ID_DESCRIPTION;
  }

  return eStartFrameID;
}


u32 JoinFestaProfileBuilder::GetApplicationResourceCount()
{
  return NetApp::JoinFestaProfile::JOINFESTAPROFILE_RESOURCE_MAX;
}


bool JoinFestaProfileBuilder::IsCreateMessageUtility()
{
  return true;
}


bool JoinFestaProfileBuilder::IsCreateNetAppMessageMenu()
{
  return true;
}


bool JoinFestaProfileBuilder::IsEnableInputExclusiveKeyMode()
{
  return true;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)
