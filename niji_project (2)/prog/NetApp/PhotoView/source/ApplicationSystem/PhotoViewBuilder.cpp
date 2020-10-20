
//==============================================================================
/**
 * @file   PhotoViewBuilder.cpp
 * @date   2017/02/07 Tue. 13:03:37
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================

// =============================================================================
/**/
//==============================================================================
#include  "./PhotoViewWork.h"
#include  "../PhotoViewResource.h"

#include  "./PhotoViewFrameSwitcher.h"
#include  "./PhotoViewFrameFactory.h"
#include  "./PhotoViewBuilder.h"


namespace NetApp {
namespace PhotoView {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_PhotoViewBuilder
namespace FILELOCAL {
}

// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     PhotoViewBuilder
  *  @brief    ctor
  */
//------------------------------------------------------------------
PhotoViewBuilder::PhotoViewBuilder(PhotoViewWork* pWork, NetAppLib::System::FrameID  startFrameID)
  : m_pWork(pWork)
  , m_StartFrameID(startFrameID)
{
}

//------------------------------------------------------------------
/**
  *  @func     ~PhotoViewBuilder
  *  @brief    dtor
  */
//------------------------------------------------------------------
PhotoViewBuilder::~PhotoViewBuilder()
{
}


/*
  implement IApplicationSystemBuilder I/F
*/


//------------------------------------------------------------------
/**
  *  @func     CreateApplicationFrameSwitcher
  *  @brief    FrameSwitcherの生成
  */
//------------------------------------------------------------------
NetAppLib::System::IApplicationFrameSwitcher*  PhotoViewBuilder::CreateApplicationFrameSwitcher(void)
{
  gfl2::heap::HeapBase*   pHeapBase = m_pWork->GetAppHeap()->GetDeviceHeap();

  return GFL_NEW( pHeapBase ) FrameSwitcher();
}


//------------------------------------------------------------------
/**
  *  @func     CreateApplicationFrameFactory
  *  @brief    FrameFactoryの生成
  */
//------------------------------------------------------------------
NetAppLib::System::IApplicationFrameFactory*  PhotoViewBuilder::CreateApplicationFrameFactory(void)
{
  gfl2::heap::HeapBase*   pHeapBase = m_pWork->GetAppHeap()->GetDeviceHeap();

  return GFL_NEW( pHeapBase ) FrameFactory(m_pWork);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppLib::System::ApplicationWorkBase*  PhotoViewBuilder::GetApplicationWork()
{
  return m_pWork;
}


//------------------------------------------------------------------
/**
  *  @func     GetApplicationStartFrameID
  *  @brief    StartFrameIDを返す
  */
//------------------------------------------------------------------
NetAppLib::System::FrameID  PhotoViewBuilder::GetApplicationStartFrameID(void)
{
  return m_StartFrameID;
}

//------------------------------------------------------------------
/**
  *  @func     GetApplicationResourceCount
  *  @brief    リソース数を返す
  */
//------------------------------------------------------------------
u32 PhotoViewBuilder::GetApplicationResourceCount()
{
  return RESOURCE_MAX;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
app::util::AppRenderingManager::Description*  PhotoViewBuilder::CreateAppRenderingManagerDescription()
{
  app::util::Heap* pAppHeap = m_pWork->GetAppHeap();

  app::util::AppRenderingManager::Description* pDescription = GFL_NEW( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description(); 
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL_LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
//  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
  pDescription->isUpperDisplayStereo                                                                     = false;
  return pDescription;
}



}  /*  namespace PhotoView  */
}  /*  namespace NetApp  */
