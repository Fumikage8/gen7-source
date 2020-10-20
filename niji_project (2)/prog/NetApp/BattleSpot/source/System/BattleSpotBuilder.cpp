//==============================================================================
/**
 * @file   BattleSpotBuilder.cpp
 * @date   2015/10/02 Fri. 12:45:34
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./BattleSpotWork.h"
#include  "../BattleSpotResource.h"

#include  "./BattleSpotFrameSwitcher.h"
#include  "./BattleSpotFrameFactory.h"
#include  "./BattleSpotBuilder.h"


namespace NetApp      {
namespace BattleSpot  {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     BattleSpotBuilder
  *  @brief    ctor
  */
//------------------------------------------------------------------
BattleSpotBuilder::BattleSpotBuilder(BattleSpotWork* pWork, NetAppLib::System::FrameID  startFrameID)
  : m_pWork(pWork)
  , m_StartFrameID(startFrameID)
{
}

//------------------------------------------------------------------
/**
  *  @func     ~BattleSpotBuilder
  *  @brief    dtor
  */
//------------------------------------------------------------------
BattleSpotBuilder::~BattleSpotBuilder()
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
NetAppLib::System::IApplicationFrameSwitcher*  BattleSpotBuilder::CreateApplicationFrameSwitcher(void)
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
NetAppLib::System::IApplicationFrameFactory*  BattleSpotBuilder::CreateApplicationFrameFactory(void)
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
NetAppLib::System::ApplicationWorkBase*  BattleSpotBuilder::GetApplicationWork()
{
  return m_pWork;
}


//------------------------------------------------------------------
/**
  *  @func     GetApplicationStartFrameID
  *  @brief    StartFrameIDを返す
  */
//------------------------------------------------------------------
NetAppLib::System::FrameID  BattleSpotBuilder::GetApplicationStartFrameID(void)
{
  return m_StartFrameID;
}

//------------------------------------------------------------------
/**
  *  @func     GetApplicationResourceCount
  *  @brief    リソース数を返す
  */
//------------------------------------------------------------------
u32 BattleSpotBuilder::GetApplicationResourceCount()
{
  return RESOURCE_MAX;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
app::util::AppRenderingManager::Description*    BattleSpotBuilder::CreateAppRenderingManagerDescription()
{
  app::util::Heap* pAppHeap = m_pWork->GetAppHeap();

  app::util::AppRenderingManager::Description* pDescription = GFL_NEW( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description(); 
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL_EFFECT_LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL_EFFECT_LAYOUT;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
  pDescription->isUpperDisplayStereo                                                                     = false;
  return pDescription;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  BattleSpotBuilder::IsCreateMessageUtility()
{
  return true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  BattleSpotBuilder::IsCreateAppToolTrainerIconRendering()
{
  return true;
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
u32 BattleSpotBuilder::GetTrainerIconNum()
{
  return  TRAINER_ICON_MAX;
}



} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
