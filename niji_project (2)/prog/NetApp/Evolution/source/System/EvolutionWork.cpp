
//==============================================================================
/**
 * @file   EvolutionWork.cpp
 * @date   2015/08/28 Fri. 17:51:40
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


// =============================================================================
/**/
//==============================================================================
namespace {
  static const u32  APPHEAP_SIZE  = 0x00080000;
  static const u32  DEVHEAP_SIZE  = 0x00400000;
} /*  namespace  */



/*
*/
namespace NetApp    {
namespace Evolution {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     Evolution
  *  @brief    ctor
  */
//------------------------------------------------------------------
EvolutionWork::EvolutionWork(void)
  : NetAppLib::System::ApplicationWorkBase(APPHEAP_SIZE, DEVHEAP_SIZE)
  , m_pAppParam(NULL)
  , m_bDemoStarted(false)
  , m_MessageUtility(GetAppHeap()->GetDeviceHeap(), 2)
  , m_pEvolutionViewContainer(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @func     Evolution
  *  @brief    ctor
  */
//------------------------------------------------------------------
EvolutionWork::~EvolutionWork()
{
}


} /*  namespace Evolution    */
} /*  namespace NetApp  */
