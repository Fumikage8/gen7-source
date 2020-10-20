//==============================================================================
/**
 * @file   EggHatchingWork.cpp
 * @date   2015/11/24 Tue. 16:29:52
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


// =============================================================================
/**/
//==============================================================================
namespace {
  static const u32  APPHEAP_SIZE  = 0x00080000;
  static const u32  DEVHEAP_SIZE  = 0x00400000;
} /*  namespace  */

/*
*/
namespace NetApp      {
namespace EggHatching {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     EggHatching
  *  @brief    ctor
  */
//------------------------------------------------------------------
EggHatchingWork::EggHatchingWork(void)
  : NetAppLib::System::ApplicationWorkBase(APPHEAP_SIZE, DEVHEAP_SIZE)
  , m_pAppParam(NULL)
  , m_bDemoStarted(false)
  , m_MessageUtility(GetAppHeap()->GetDeviceHeap(), 2)
  , m_pViewContainer(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @func     EggHatching
  *  @brief    ctor
  */
//------------------------------------------------------------------
EggHatchingWork::~EggHatchingWork()
{
}


} /*  namespace EggHatching */
} /*  namespace NetApp      */
