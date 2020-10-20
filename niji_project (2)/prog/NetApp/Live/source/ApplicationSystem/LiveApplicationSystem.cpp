//==============================================================================
/**
 * @file   LiveApplicationSystem.cpp
 * @date   2015/07/09 Thu. 14:30:58
 * @author muto_yu
 * @brief  Live大会ApplicationSystem
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
#include  "./LiveWork.h"
#include  "./LiveApplicationSystem.h"
#include  "../LiveResource.h"
#include  "../Views/LiveViewContainer.h"


// =============================================================================
/**/
//==============================================================================
namespace NetApp  {
namespace Live {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
ApplicationSystem::ApplicationSystem(LiveWork* pWork)
  : NetAppLib::System::ApplicationSystemBase()
  , m_pWork(pWork)
  , m_pViewContainer(NULL)
{
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
  gfl2::heap::HeapBase*               pHeapBase         = m_pWork->GetAppHeap()->GetDeviceHeap();


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
  pResourceManager->AddLoadRequest(RESOURCE_Message,                print::GetMessageArcId(), GARC_message_live_tournament_DAT,       false,  true);
  pResourceManager->AddLoadRequest(RESOURCE_Layout,                 ARCID_LIVE,               GARC_Live_Live_applyt_COMP,             true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_LayoutCommonWindow,     ARCID_MENU_WINDOW,        GARC_MenuWindow_MenuWindow_applyt_COMP, true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_LayoutCommonMenuCursor, ARCID_MENU_CURSOR,        GARC_MenuCursor_MenuCursor_applyt_COMP, true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_BG,                     ARCID_LIVE,               GARC_Live_Live_BGEmit_000_bptcl_COMP,   true,   true);

  /*  font  */
  pResourceManager->FontLoadRequest(NetAppLib::System::ResourceManager::FONT_NUMBER);
//  pResourceManager->FontLoadRequest(NetAppLib::System::ResourceManager::FONT_BATTLE);

}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  ApplicationSystem::CreateMessageData()
{
  m_pWork->GetMessageUtility().LoadFromMemory(LIVE_MESSAGE_GRAC_ID, m_pWork->GetResourceManager()->GetResourceBuffer(RESOURCE_Message));
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
  
  
  m_pWork->m_pTrainerIconUtil = m_pNetAppTrainerIconUtility;
  
  m_pViewContainer  = GFL_NEW(pHeapBase) LiveViewContainer(m_pWork);
  m_pWork->SetViewContainer(m_pViewContainer);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  ApplicationSystem::AddModel()
{
  app::util::Heap*      pHeap     = m_pWork->GetAppHeap();
  gfl2::heap::HeapBase* pHeapBase = pHeap->GetDeviceHeap();


}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  ApplicationSystem::RemoveModel()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  ApplicationSystem::SetupCamera()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  ApplicationSystem::UnsetCamera()
{
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


  return isStable;
}

} /*  namespace Live */
} /*  namespace NetApp      */
