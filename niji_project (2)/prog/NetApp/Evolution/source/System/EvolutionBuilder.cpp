//==============================================================================
/**
 * @file   EvolutionBuilder.cpp
 * @date   2015/08/28 Fri. 17:44:49
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./EvolutionWork.h"
#include  "../EvolutionResource.h"

#include  "./EvolutionFrameSwitcher.h"
#include  "./EvolutionFrameFactory.h"
#include  "./EvolutionBuilder.h"


namespace NetApp    {
namespace Evolution {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     EvolutionBuilder
  *  @brief    ctor
  */
//------------------------------------------------------------------
EvolutionBuilder::EvolutionBuilder(EvolutionWork* pWork, NetAppLib::System::FrameID  startFrameID)
  : m_pWork(pWork)
  , m_StartFrameID(startFrameID)
{
}

//------------------------------------------------------------------
/**
  *  @func     ~EvolutionBuilder
  *  @brief    dtor
  */
//------------------------------------------------------------------
EvolutionBuilder::~EvolutionBuilder()
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
NetAppLib::System::IApplicationFrameSwitcher*  EvolutionBuilder::CreateApplicationFrameSwitcher(void)
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
NetAppLib::System::IApplicationFrameFactory*  EvolutionBuilder::CreateApplicationFrameFactory(void)
{
  gfl2::heap::HeapBase*   pHeapBase = m_pWork->GetAppHeap()->GetDeviceHeap();

  return GFL_NEW( pHeapBase ) FrameFactory(m_pWork);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
app::util::AppRenderingManager::Description*    EvolutionBuilder::CreateAppRenderingManagerDescription()
{
  app::util::Heap* pAppHeap = m_pWork->GetAppHeap();

  app::util::AppRenderingManager::Description* pDescription = GFL_NEW( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description(); 
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].renderOrder = app::util::AppRenderingManager::RenderOrderType::NONE;
  pDescription->isUpperDisplayStereo                                                                     = false;
  return pDescription;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppLib::System::ApplicationWorkBase*  EvolutionBuilder::GetApplicationWork()
{
  return m_pWork;
}


//------------------------------------------------------------------
/**
  *  @func     GetApplicationStartFrameID
  *  @brief    StartFrameIDを返す
  */
//------------------------------------------------------------------
NetAppLib::System::FrameID  EvolutionBuilder::GetApplicationStartFrameID(void)
{
  return m_StartFrameID;
}

//------------------------------------------------------------------
/**
  *  @func     GetApplicationResourceCount
  *  @brief    リソース数を返す
  */
//------------------------------------------------------------------
u32 EvolutionBuilder::GetApplicationResourceCount()
{
  return RESOURCE_MAX;
}



} /*  namespace Evolution */
} /*  namespace NetApp    */
