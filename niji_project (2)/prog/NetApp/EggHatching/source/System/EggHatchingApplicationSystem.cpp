
//==============================================================================
/**
 * @file   EggHatchingApplicationSystem.cpp
 * @date   2015/11/24 Tue. 12:38:31
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./EggHatchingWork.h"
#include  "./EggHatchingApplicationSystem.h"
#include  "../EggHatchingResource.h"
#include  "../Views/EggHatchingViewContainer.h"


namespace NetApp  {
namespace EggHatching    {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
ApplicationSystem::ApplicationSystem(EggHatchingWork* pWork)
  : NetAppLib::System::ApplicationSystemBase()
  , m_pWork(pWork)
  , m_pViewContainer(NULL)
{
  NonUseFadeRequest();
}

/*
void ApplicationSystem::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pAppFrameManager->Draw( displayNo );

  DrawView( displayNo );

  if( m_pAppPlayerIconRendering )
  {
    m_pAppPlayerIconRendering->Draw( displayNo );
  }

//  m_pAppRenderingManager->Draw( displayNo );
}

*/

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
  pResourceManager->AddLoadRequest(RESOURCE_Message,                print::GetMessageArcId(), EGGHATCHING_MESSAGE_GRAC_ID,              false,  true);
  pResourceManager->AddLoadRequest(RESOURCE_Layout,                 ARCID_EVOLUTION,          GARC_Evolution_Evolution_applyt_COMP,     true,   true);
//  pResourceManager->AddLoadRequest(RESOURCE_LayoutCommonWindow,     ARCID_MENU_WINDOW,        GARC_MenuWindow_MenuWindow_applyt_COMP,   true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_LayoutMenuWindow2,      ARCID_MENU_WINDOW2,       GARC_MenuWindow2_MenuWindow2_applyt_COMP, true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_LayoutCommonMenuCursor, ARCID_MENU_CURSOR,        GARC_MenuCursor_MenuCursor_applyt_COMP,   true,   true);

  /*  font  */
//  pResourceManager->FontLoadRequest(NetAppLib::System::ResourceManager::FONT_NUMBER);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  ApplicationSystem::CreateMessageData()
{
  m_pWork->GetMessageUtility().LoadFromMemory(EGGHATCHING_MESSAGE_GRAC_ID, m_pWork->GetResourceManager()->GetResourceBuffer(RESOURCE_Message));
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
  
  m_pViewContainer  = GFL_NEW(pHeapBase) EggHatchingViewContainer(m_pWork);
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

} /*  namespace EggHatching   */
} /*  namespace NetApp        */
