//==============================================================================
/**
 * @file   EvolutionApplicationSystem.cpp
 * @date   2015/08/28 Fri. 17:32:18
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <model/include/gfl2_BaseModel.h>
#include  <model/include/gfl2_BaseCamera.h>
#include  <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include  <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include  <AppLib/include/Util/AppRenderingManager.h>
#include  "./EvolutionWork.h"
#include  "./EvolutionApplicationSystem.h"
#include  "../EvolutionResource.h"
#include  "../Views/EvolutionViewContainer.h"


// =============================================================================
/**/
//==============================================================================
namespace NetApp  {
namespace Evolution    {

// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
ApplicationSystem::ApplicationSystem(EvolutionWork* pWork)
  : NetAppLib::System::ApplicationSystemBase()
  , m_pWork(pWork)
  , m_pViewContainer(NULL)
  , m_pSceneGraphResourceNode(NULL)
  , m_pBGModel(NULL)
{
  m_UpperCameraInfo.pDrawEnvNode = NULL;
  m_UpperCameraInfo.pCamera      = NULL;
  m_LowerCameraInfo.pDrawEnvNode = NULL;
  m_LowerCameraInfo.pCamera      = NULL;

  NonUseFadeRequest();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
ApplicationSystem::~ApplicationSystem()
{
  GFL_SAFE_DELETE(m_pViewContainer);

  /*    */
  GFL_SAFE_DELETE(m_UpperCameraInfo.pDrawEnvNode);
  GFL_SAFE_DELETE(m_UpperCameraInfo.pCamera);
  GFL_SAFE_DELETE(m_LowerCameraInfo.pDrawEnvNode);
  GFL_SAFE_DELETE(m_LowerCameraInfo.pCamera);

  GFL_SAFE_DELETE(m_pBGModel);
  GFL_SAFE_DELETE(m_pSceneGraphResourceNode);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  ApplicationSystem::LoadResourceRequest()
{
  NetAppLib::System::ResourceManager* pResourceManager  = m_pWork->GetResourceManager();
//  gfl2::heap::HeapBase*               pHeapBase         = m_pWork->GetAppHeap()->GetDeviceHeap();


  /*
    resourceID
  
    void AddLoadRequest(
          NetAppLib::System::ResourceID resourceID,
          gfl2::fs::ArcFile::ARCID                    loadArcID,
          gfl2::fs::ArcFile::ARCDATID                 loadDataID,
          bool                                        compressed,
          bool                                        useDeviceHeap
    );
  */
  pResourceManager->AddLoadRequest(RESOURCE_Message,                print::GetMessageArcId(), EVOLUTION_MESSAGE_GRAC_ID,                false,  true);
  pResourceManager->AddLoadRequest(RESOURCE_Layout,                 ARCID_EVOLUTION,          GARC_Evolution_Evolution_applyt_COMP,     true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_LayoutCommonWindow,     ARCID_MENU_WINDOW,        GARC_MenuWindow_MenuWindow_applyt_COMP,   true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_LayoutMenuWindow2,      ARCID_MENU_WINDOW2,       GARC_MenuWindow2_MenuWindow2_applyt_COMP, true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_LayoutCommonMenuCursor, ARCID_MENU_CURSOR,        GARC_MenuCursor_MenuCursor_applyt_COMP,   true,   true);

  /*  font  */
  pResourceManager->FontLoadRequest(NetAppLib::System::ResourceManager::FONT_NUMBER);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  ApplicationSystem::CreateMessageData()
{
  m_pWork->GetMessageUtility().LoadFromMemory(EVOLUTION_MESSAGE_GRAC_ID, m_pWork->GetResourceManager()->GetResourceBuffer(RESOURCE_Message));
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  ApplicationSystem::CreateView()
{
  gfl2::heap::HeapBase* pHeapBase = m_pWork->GetAppHeap()->GetDeviceHeap();
  
  m_pViewContainer  = GFL_NEW(pHeapBase) EvolutionViewContainer(m_pWork);
  m_pWork->SetViewContainer(m_pViewContainer);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void ApplicationSystem::UpdateView()
{
  m_pViewContainer->Update();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void ApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pViewContainer->Draw(displayNo);
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool ApplicationSystem::IsExitView()
{
  bool  isStable  = true;
  /*  views  */
  if(m_pViewContainer->IsDrawingAny())
  {
    isStable  = false;
  }

  /*  demo  */
  {
    App::AppDemo::AppDemoUtility*   pDemoUtil = m_pViewContainer->m_pDemoUtil;

    pDemoUtil->ExitRequest();
    if(!pDemoUtil->IsStable())
    {
      pDemoUtil->SendCommand(App::AppDemo::AppDemoUtility::COMMAND_SystemSync);
      pDemoUtil->Update();
      isStable  = false;
    }
  }



  return isStable;
}

} /*  namespace Evolution   */
} /*  namespace NetApp      */
