// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptBuilder.cpp
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)を構築するクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelectForScript/source/System/JoinFestaPlayerListSelectForScriptBuilder.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/System/JoinFestaPlayerListSelectForScriptFrameSwitcher.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/System/JoinFestaPlayerListSelectForScriptFrameFactory.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/JoinFestaPlayerListSelectForScriptResourceID.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/JoinFestaPlayerListSelectForScriptFrameID.h"

#include "../../include/JoinFestaPlayerListSelectForScriptAppParam.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListSelectForScriptBuilder::JoinFestaPlayerListSelectForScriptBuilder( NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptWork* pJoinFestaPlayerListSelectForScriptWork, const APP_PARAM& rParam) :
  m_pJoinFestaPlayerListSelectForScriptWork( pJoinFestaPlayerListSelectForScriptWork ),
  m_pJoinFestaPlayerListSelectForScriptViewSystemAccessor( NULL )
{
  m_StartFrameID  = (rParam.bootMode == BOOT_MODE_BattleFesCircle) ? JOINFESTAPLAYERLISTSELECTFORSCRIPT_FRAME_ID_ForBattleFesCircle : JOINFESTAPLAYERLISTSELECTFORSCRIPT_FRAME_ID_SELECT;
}


JoinFestaPlayerListSelectForScriptBuilder::~JoinFestaPlayerListSelectForScriptBuilder()
{
}


NetAppLib::System::IApplicationFrameSwitcher* JoinFestaPlayerListSelectForScriptBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pJoinFestaPlayerListSelectForScriptWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptFrameSwitcher();
}


NetAppLib::System::IApplicationFrameFactory* JoinFestaPlayerListSelectForScriptBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pJoinFestaPlayerListSelectForScriptWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptFrameFactory( m_pJoinFestaPlayerListSelectForScriptWork, m_pJoinFestaPlayerListSelectForScriptViewSystemAccessor );
}


app::util::AppRenderingManager::Description* JoinFestaPlayerListSelectForScriptBuilder::CreateAppRenderingManagerDescription()
{
  app::util::Heap* pAppHeap = m_pJoinFestaPlayerListSelectForScriptWork->GetAppHeap();
  app::util::AppRenderingManager::Description* pDescription = GFL_NEW( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description(); 
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL0_LAYOUT_RADERCHART_POKEMODEL1;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL_LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN ].renderOrder    = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
  pDescription->isUpperDisplayStereo                                                                     = false;
  return pDescription;
}


NetAppLib::System::ApplicationWorkBase* JoinFestaPlayerListSelectForScriptBuilder::GetApplicationWork()
{
  return m_pJoinFestaPlayerListSelectForScriptWork;
}


NetAppLib::System::FrameID JoinFestaPlayerListSelectForScriptBuilder::GetApplicationStartFrameID()
{
  return m_StartFrameID;
}


u32 JoinFestaPlayerListSelectForScriptBuilder::GetApplicationResourceCount()
{
  return NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_RESOURCE_MAX;
}


bool JoinFestaPlayerListSelectForScriptBuilder::IsCreateMessageUtility()
{
  return true;
}


bool JoinFestaPlayerListSelectForScriptBuilder::IsCreateNetAppMessageMenu()
{
  return true;
}


bool JoinFestaPlayerListSelectForScriptBuilder::IsCreateAppCountryRegionNameUtility()
{
  return true;
}


bool JoinFestaPlayerListSelectForScriptBuilder::IsCreateAppCommonGrpIconData()
{
  return true;
}


bool JoinFestaPlayerListSelectForScriptBuilder::IsCreateAppToolTrainerIconRendering()
{
  return true;
}


u32  JoinFestaPlayerListSelectForScriptBuilder::GetTrainerIconNum()
{
  return 1;
}


app::util::G2DUtil::SETUP_DISPLAY_TYPE JoinFestaPlayerListSelectForScriptBuilder::GetTrainerIconDrawDisplayType()
{
  return app::util::G2DUtil::SETUP_UPPER;
}


bool JoinFestaPlayerListSelectForScriptBuilder::IsEnableInputExclusiveKeyMode()
{
  return true;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)
