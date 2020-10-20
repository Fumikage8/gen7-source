
//==============================================================================
/**
 * @file   QRReaderApplicationSystem.cpp
 * @date   2016/01/14 Thu. 17:09:51
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./QRReaderWork.h"
#include  "./QRReaderApplicationSystem.h"
#include  "../QRReaderResource.h"
#include  "../Views/QRReaderViewContainer.h"

// =============================================================================
/**/
//==============================================================================
namespace
{
  enum  LoadState
  {
    LOADSTATE_ApplicationSystem,
    LOADSTATE_CommonIconData,
    
    
    LOADSTATE_END,
    LOADSTATE_
  };
}



namespace NetApp  {
namespace QRReader    {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
ApplicationSystem::ApplicationSystem(QRReaderWork* pWork)
  : NetAppLib::System::ApplicationSystemBase()
  , m_pWork(pWork)
  , m_pViewContainer(NULL)
  , m_pPokeModelUtil(NULL)
  , m_LoadState(LOADSTATE_ApplicationSystem)
  , m_CommonIconData()
  , m_IsCommonIconDataTerminated(false)
#if PM_DEBUG
  , m_Debug(pWork)
#endif
{
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
  GFL_SAFE_DELETE(m_pPokeModelUtil);

  /*    */
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool ApplicationSystem::Loading()
{
  switch(m_LoadState)
  {
    case  LOADSTATE_ApplicationSystem:
      /*  super  */
      if(!NetAppLib::System::ApplicationSystemBase::Loading())
      {
        m_LoadState = LOADSTATE_CommonIconData;
      }
      break;

    case  LOADSTATE_CommonIconData:
      if(m_CommonIconData.IsReady())
      {
        m_LoadState = LOADSTATE_END;
      }
      break;
    
    case  LOADSTATE_END:  /*  fall through  */
    default:
      /*  NOP  */
      break;
  }


  return  (m_LoadState != LOADSTATE_END);
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
  pResourceManager->AddLoadRequest(RESOURCE_Message,                print::GetMessageArcId(), QRREADER_MESSAGE_GRAC_ID,                 false,  true);
  pResourceManager->AddLoadRequest(RESOURCE_Message_PlaceName,      print::GetMessageArcId(), QRREADER_MESSAGE_PLACENAME_ARCDAT,        false,  true);
  pResourceManager->AddLoadRequest(RESOURCE_Layout,                 ARCID_QRREADER,           GARC_QRReader_QRReader_applyt_COMP,       true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_LayoutCommonWindow,     ARCID_MENU_WINDOW,        GARC_MenuWindow_MenuWindow_applyt_COMP,   true,   true);
//  pResourceManager->AddLoadRequest(RESOURCE_LayoutMenuWindow2,      ARCID_MENU_WINDOW2,       GARC_MenuWindow2_MenuWindow2_applyt_COMP, true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_LayoutCommonMenuCursor, ARCID_MENU_CURSOR,        GARC_MenuCursor_MenuCursor_applyt_COMP,   true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_FieldResident,          ARCID_FIELD_RESIDENT,     GARC_field_resident_field_resident_COMP,  true,   true);

  /*  font  */
//  pResourceManager->FontLoadRequest(NetAppLib::System::ResourceManager::FONT_NUMBER);


  /*  commonGrpIcon  */
  {
    m_CommonIconData.Initialize(m_pWork->GetAppHeap());
    m_pWork->m_pCommonIconData  = &m_CommonIconData;
  }
  
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  ApplicationSystem::CreateMessageData()
{
  m_pWork->GetMessageUtility()->LoadFromMemory(QRREADER_MESSAGE_GRAC_ID,          m_pWork->GetResourceManager()->GetResourceBuffer(RESOURCE_Message));
  m_pWork->GetMessageUtility()->LoadFromMemory(QRREADER_MESSAGE_PLACENAME_ARCDAT, m_pWork->GetResourceManager()->GetResourceBuffer(RESOURCE_Message_PlaceName));
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
  
  m_pViewContainer  = GFL_NEW(pHeapBase) QRReaderViewContainer(m_pWork);
  m_pWork->m_pViewContainer = m_pViewContainer;


  /*  pokemodel  */
  {
    const NetAppLib::Util::NetAppPokeModelUtility::InFrameParam inFrameParam  = 
    {
      System::Camera::CModelViewerInFrame::SCREEN_UPPER,
      0,  0,  128,  128,
      System::Camera::CModelViewerInFrame::TURN_DIRECTION_FREE,   /*  TURN_DIRECTION_LEFT, TURN_DIRECTION_RIGHT  */
      System::Camera::CModelViewerInFrame::DRAW_POSITION_BOTTOM,  /*  DRAW_POSITION_CENTER  */    /*  centerは例外的Modeとのこと  */
      app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY,
    };
    
    
    {
      m_pPokeModelUtil  = GFL_NEW(pHeapBase)  NetAppLib::Util::NetAppPokeModelUtility(m_pWork);
      m_pPokeModelUtil->Setup(2, &inFrameParam);
      m_pWork->m_pPokeModelUtil = m_pPokeModelUtil;
    }
  }


  Sound::PlaySE(SEQ_SE_SYS_NJ_022);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void ApplicationSystem::UpdateView()
{
  /*  pokemodelUtil  */
  if(m_pPokeModelUtil)
  {
    m_pPokeModelUtil->Update();
  }

  m_pViewContainer->Update();

#if PM_DEBUG
  m_Debug.OnUpdate();
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
  /*  pokemodelUtil  */
  if(m_pPokeModelUtil)
  {
    m_pPokeModelUtil->Draw(displayNo);
  }
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
  if(m_pPokeModelUtil)
  {
    m_pPokeModelUtil->DebugDraw(displayNo);
  }
  m_Debug.OnDraw(displayNo);
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
  if(m_pViewContainer->IsDrawingAny())
  {
    isStable  = false;
  }

  /*  pokemodel  */
  if(m_pPokeModelUtil)
  {
    m_pPokeModelUtil->CleanupRequest();
    m_pPokeModelUtil->Update();
    if(!m_pPokeModelUtil->IsStable())
    {
      isStable  = false;
    }
  }

  /*  commonGrpIcon  */
  if(!m_IsCommonIconDataTerminated)
  {
    m_CommonIconData.Terminate();
    m_IsCommonIconDataTerminated  = true;
  }

  return isStable;
}

} /*  namespace QRReader   */
} /*  namespace NetApp        */
