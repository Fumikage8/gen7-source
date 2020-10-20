
//==============================================================================
/**
 * @file   QRReaderBuilder.cpp
 * @date   2016/01/14 Thu. 17:13:59
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
#include  "../QRReaderResource.h"

#include  "./QRReaderFrameSwitcher.h"
#include  "./QRReaderFrameFactory.h"
#include  "./QRReaderBuilder.h"


namespace NetApp   {
namespace QRReader {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     QRReaderBuilder
  *  @brief    ctor
  */
//------------------------------------------------------------------
QRReaderBuilder::QRReaderBuilder(QRReaderWork* pWork, NetAppLib::System::FrameID  startFrameID)
  : m_pWork(pWork)
  , m_StartFrameID(startFrameID)
{
}

//------------------------------------------------------------------
/**
  *  @func     ~QRReaderBuilder
  *  @brief    dtor
  */
//------------------------------------------------------------------
QRReaderBuilder::~QRReaderBuilder()
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
NetAppLib::System::IApplicationFrameSwitcher*  QRReaderBuilder::CreateApplicationFrameSwitcher(void)
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
NetAppLib::System::IApplicationFrameFactory*  QRReaderBuilder::CreateApplicationFrameFactory(void)
{
  gfl2::heap::HeapBase*   pHeapBase = m_pWork->GetAppHeap()->GetDeviceHeap();

  return GFL_NEW( pHeapBase ) FrameFactory(m_pWork);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
app::util::AppRenderingManager::Description*    QRReaderBuilder::CreateAppRenderingManagerDescription()
{
  app::util::Heap* pAppHeap = m_pWork->GetAppHeap();

  app::util::AppRenderingManager::Description* pDescription = GFL_NEW( pAppHeap->GetDeviceHeap()->GetLowerHandle() ) app::util::AppRenderingManager::Description(); 
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder        = app::util::AppRenderingManager::RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1;
  pDescription->renderDesc[ app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].useDistortionPath  = true;

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
NetAppLib::System::ApplicationWorkBase*  QRReaderBuilder::GetApplicationWork()
{
  return m_pWork;
}


//------------------------------------------------------------------
/**
  *  @func     GetApplicationStartFrameID
  *  @brief    StartFrameIDを返す
  */
//------------------------------------------------------------------
NetAppLib::System::FrameID  QRReaderBuilder::GetApplicationStartFrameID(void)
{
  return m_StartFrameID;
}

//------------------------------------------------------------------
/**
  *  @func     GetApplicationResourceCount
  *  @brief    リソース数を返す
  */
//------------------------------------------------------------------
u32 QRReaderBuilder::GetApplicationResourceCount()
{
  return RESOURCE_MAX;
}




} /*  namespace QRReader */
} /*  namespace NetApp    */

