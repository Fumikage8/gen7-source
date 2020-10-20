//==============================================================================
/**
 * @file   GiftApplicationSystem.cpp
 * @date   2015/07/31 Fri. 15:38:17
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
#include  "./GiftWork.h"
#include  "./GiftApplicationSystem.h"
#include  "../GiftResource.h"
#include  "../Views/GiftViewContainer.h"


// =============================================================================
/**/
//==============================================================================
namespace NetApp  {
namespace Gift {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
ApplicationSystem::ApplicationSystem(GiftWork* pWork)
  : NetAppLib::System::ApplicationSystemBase()
  , m_pWork(pWork)
  , m_pViewContainer(NULL)
  , m_AuthHelper(pWork)
#if PM_DEBUG
  , m_GiftDebug(m_pWork)
#endif
{
  m_pWork->m_pAuthHelper  = &m_AuthHelper;
#if PM_DEBUG
  m_pWork->m_pGiftDebug = &m_GiftDebug;
#endif

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
  pResourceManager->AddLoadRequest(RESOURCE_Message,                print::GetMessageArcId(), GIFT_MESSAGE_GRAC_ID,                     false,  true);
  pResourceManager->AddLoadRequest(RESOURCE_Message_Card,           print::GetMessageArcId(), GIFT_MESSAGE_CARD_GRAC_ID,                false,  true);
  pResourceManager->AddLoadRequest(RESOURCE_Layout,                 ARCID_MYSTERY_GIFT,       GARC_Gift_Gift_applyt_COMP,               true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_LayoutCommonWindow,     ARCID_MENU_WINDOW,        GARC_MenuWindow_MenuWindow_applyt_COMP,   true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_LayoutCommonMenuCursor, ARCID_MENU_CURSOR,        GARC_MenuCursor_MenuCursor_applyt_COMP,   true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_LayoutMenuWindow2,      ARCID_MENU_WINDOW2,       GARC_MenuWindow2_MenuWindow2_applyt_COMP, true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_EmitterSet_1,           ARCID_MYSTERY_GIFT,       GARC_Gift_Gift_CardEmit_000_bptcl_COMP,   true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_EmitterSet_2,           ARCID_MYSTERY_GIFT,       GARC_Gift_Gift_CardEmit_001_bptcl_COMP,   true,   true);

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
  m_pWork->GetMessageUtility().LoadFromMemory(GIFT_MESSAGE_GRAC_ID,       m_pWork->GetResourceManager()->GetResourceBuffer(RESOURCE_Message));
  m_pWork->GetMessageUtility().LoadFromMemory(GIFT_MESSAGE_CARD_GRAC_ID,  m_pWork->GetResourceManager()->GetResourceBuffer(RESOURCE_Message_Card));
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
  
  m_pViewContainer      = GFL_NEW(pHeapBase) GiftViewContainer(m_pWork);
  m_pWork->SetViewContainer(m_pViewContainer);

  /*  framemanager  */
  m_pWork->m_pAppFrameManager = m_pAppFrameManager;
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
#if PM_DEBUG
  m_GiftDebug.OnUpdate();
#endif
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
#if PM_DEBUG
void  ApplicationSystem::DebugDraw(const gfl2::gfx::CtrDisplayNo displayNo)
{
  m_pViewContainer->DebugDraw(displayNo);
  m_GiftDebug.OnDraw(displayNo);
}
#endif


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
  if(!m_pViewContainer->TryFinalize())
  {
    isStable  = false;
  }

  if(m_pViewContainer->IsDrawingAny())
  {
    isStable  = false;
  }
  
  return isStable;
}



} /*  namespace Gift */
} /*  namespace NetApp      */
