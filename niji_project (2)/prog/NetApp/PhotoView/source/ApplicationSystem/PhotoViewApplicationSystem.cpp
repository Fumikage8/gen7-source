//==============================================================================
/**
 * @file   PhotoViewApplicationSystem.cpp
 * @date   2017/02/07 Tue. 12:55:48
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
#include  "./PhotoViewWork.h"
#include  "./PhotoViewApplicationSystem.h"
#include  "../PhotoViewResource.h"
#include  "../Views/PhotoViewViewContainer.h"


namespace NetApp {
namespace PhotoView {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_PhotoViewApplicationSystem
namespace FILELOCAL {
}

// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
ApplicationSystem::ApplicationSystem(PhotoViewWork* pWork)
  : NetAppLib::System::ApplicationSystemBase()
  , m_pWork(pWork)
  , m_pViewContainer(NULL)
{
  /**
    @fix  MMcat[496:   【フェスサークル】特定のタイミングでスライドショー画面から抜けると、次の写真が一瞬表示される
      アプリ終了時に、TYPE_CROSSで表示していた前の画像が透明の黒となり、裏読みしていた次の写真が一瞬表示されてしまう
      FrameWorkに任せていたFadeを抑制し、手動で行う
  */
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
  pResourceManager->AddLoadRequest(RESOURCE_Message,                print::GetMessageArcId(),     MESSAGE_GRAC_ID,  false,  true);
  pResourceManager->AddLoadRequest(RESOURCE_PhotoLocation,          print::GetMessageArcId(),     LOCATION_GRAC_ID, false,  true);
  pResourceManager->AddLoadRequest(RESOURCE_Layout,                 ARCID_JOIN_FESTA_PLAYER_LIST, GARC_join_festa_playerlist_join_festa_playerlist_applyt_COMP,   true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_LayoutCommonWindow,     ARCID_MENU_WINDOW,            GARC_MenuWindow_MenuWindow_applyt_COMP, true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_LayoutCommonMenuCursor, ARCID_MENU_CURSOR,            GARC_MenuCursor_MenuCursor_applyt_COMP, true,   true);

  /*  font  */
//  pResourceManager->FontLoadRequest(NetAppLib::System::ResourceManager::FONT_NUMBER);
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
  m_pWork->GetMessageUtility().LoadFromMemory(MESSAGE_GRAC_ID,  m_pWork->GetResourceManager()->GetResourceBuffer(RESOURCE_Message));
  m_pWork->GetMessageUtility().LoadFromMemory(LOCATION_GRAC_ID, m_pWork->GetResourceManager()->GetResourceBuffer(RESOURCE_PhotoLocation));
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
  
  m_pViewContainer  = GFL_NEW(pHeapBase) PhotoViewViewContainer(m_pWork);
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

}  /*  namespace PhotoView  */
}  /*  namespace NetApp  */
