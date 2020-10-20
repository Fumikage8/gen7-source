//==============================================================================
/**
 * @file   EggHatchingBuilder.cpp
 * @date   2015/11/24 Tue. 12:35:51
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
#include  "../EggHatchingResource.h"

#include  "./EggHatchingFrameSwitcher.h"
#include  "./EggHatchingFrameFactory.h"
#include  "./EggHatchingBuilder.h"


namespace NetApp    {
namespace EggHatching {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     EggHatchingBuilder
  *  @brief    ctor
  */
//------------------------------------------------------------------
EggHatchingBuilder::EggHatchingBuilder(EggHatchingWork* pWork, NetAppLib::System::FrameID  startFrameID)
  : m_pWork(pWork)
  , m_StartFrameID(startFrameID)
{
}

//------------------------------------------------------------------
/**
  *  @func     ~EggHatchingBuilder
  *  @brief    dtor
  */
//------------------------------------------------------------------
EggHatchingBuilder::~EggHatchingBuilder()
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
NetAppLib::System::IApplicationFrameSwitcher*  EggHatchingBuilder::CreateApplicationFrameSwitcher(void)
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
NetAppLib::System::IApplicationFrameFactory*  EggHatchingBuilder::CreateApplicationFrameFactory(void)
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
NetAppLib::System::ApplicationWorkBase*  EggHatchingBuilder::GetApplicationWork()
{
  return m_pWork;
}


//------------------------------------------------------------------
/**
  *  @func     GetApplicationStartFrameID
  *  @brief    StartFrameIDを返す
  */
//------------------------------------------------------------------
NetAppLib::System::FrameID  EggHatchingBuilder::GetApplicationStartFrameID(void)
{
  return m_StartFrameID;
}

//------------------------------------------------------------------
/**
  *  @func     GetApplicationResourceCount
  *  @brief    リソース数を返す
  */
//------------------------------------------------------------------
u32 EggHatchingBuilder::GetApplicationResourceCount()
{
  return RESOURCE_MAX;
}




} /*  namespace EggHatching */
} /*  namespace NetApp    */

