
//==============================================================================
/**
 * @file   GiftBuilder.cpp
 * @date   2015/07/31 Fri. 15:46:41
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./GiftWork.h"
#include  "../GiftResource.h"

#include  "./GiftFrameSwitcher.h"
#include  "./GiftFrameFactory.h"
#include  "./GiftBuilder.h"


namespace NetApp  {
namespace Gift {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     GiftBuilder
  *  @brief    ctor
  */
//------------------------------------------------------------------
GiftBuilder::GiftBuilder(GiftWork* pWork, NetAppLib::System::FrameID  startFrameID)
  : m_pWork(pWork)
  , m_StartFrameID(startFrameID)
{
}

//------------------------------------------------------------------
/**
  *  @func     ~GiftBuilder
  *  @brief    dtor
  */
//------------------------------------------------------------------
GiftBuilder::~GiftBuilder()
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
NetAppLib::System::IApplicationFrameSwitcher*  GiftBuilder::CreateApplicationFrameSwitcher(void)
{
  gfl2::heap::HeapBase*   pHeapBase = m_pWork->GetAppHeap()->GetDeviceHeap();

  return GFL_NEW( pHeapBase ) FrameSwitcher(m_pWork);
}


//------------------------------------------------------------------
/**
  *  @func     CreateApplicationFrameFactory
  *  @brief    FrameFactoryの生成
  */
//------------------------------------------------------------------
NetAppLib::System::IApplicationFrameFactory*  GiftBuilder::CreateApplicationFrameFactory(void)
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
NetAppLib::System::ApplicationWorkBase*  GiftBuilder::GetApplicationWork()
{
  return m_pWork;
}


//------------------------------------------------------------------
/**
  *  @func     GetApplicationStartFrameID
  *  @brief    StartFrameIDを返す
  */
//------------------------------------------------------------------
NetAppLib::System::FrameID  GiftBuilder::GetApplicationStartFrameID(void)
{
  return m_StartFrameID;
}

//------------------------------------------------------------------
/**
  *  @func     GetApplicationResourceCount
  *  @brief    リソース数を返す
  */
//------------------------------------------------------------------
u32 GiftBuilder::GetApplicationResourceCount()
{
  return RESOURCE_MAX;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
app::util::AppRenderingManager::Description*  GiftBuilder::CreateAppRenderingManagerDescription()
{
  app::util::Heap* pAppHeap = m_pWork->GetAppHeap();

  app::util::AppRenderingManager::Description* pDescription = GFL_NEW( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description(); 
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1_EFFECT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].renderOrder = app::util::AppRenderingManager::RenderOrderType::NONE;
  pDescription->isUpperDisplayStereo                                                                     = false;
  return pDescription;
}


} /*  namespace Gift */
} /*  namespace NetApp      */
