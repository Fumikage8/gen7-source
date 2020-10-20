//==============================================================================
/**
 * @file   EggHatchingFrameFactory.cpp
 * @date   2015/11/24 Tue. 15:34:36
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../EggHatchingFrameDef.h"
#include  "./EggHatchingWork.h"

/*  frames  */
#include  "../Frames/EggHatchingFrame_Demo.h"

#include  "./EggHatchingFrameFactory.h"


namespace NetApp    {
namespace EggHatching {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     FrameFactory
  *  @brief    ctor
  */
//------------------------------------------------------------------
FrameFactory::FrameFactory(EggHatchingWork* pWork)
  : m_pWork(pWork)
{
}


//------------------------------------------------------------------
/**
  *  @func     ~FrameFactory
  *  @brief    dtor
  */
//------------------------------------------------------------------
FrameFactory::~FrameFactory()
{
}


/*
  implement IApplciationFrameFactory
*/
//------------------------------------------------------------------
/**
  *  @func     CreateFrame
  *  @brief    Frameの生成
  */
//------------------------------------------------------------------
NetAppLib::System::NetAppFrameBase* FrameFactory::CreateNetAppFrame(NetAppLib::System::FrameID  frameID)
{
  NetAppLib::System::NetAppFrameBase* pFrame    = NULL;
  gfl2::heap::HeapBase*               pHeapBase = m_pWork->GetAppHeap()->GetDeviceHeap();


  switch(frameID)
  {
    case  FRAME_Demo:  pFrame  = GFL_NEW(pHeapBase)  Frame_Demo(m_pWork); break;

    default:
      GFL_PRINT("%s(%d):[warning]: unknown frameID(%d)\n", __FILE__, __LINE__, static_cast<u32>(frameID));
      break;
  }


  return pFrame;
}



} /*  namespace Evolution */
} /*  namespace NetApp    */


