

//==============================================================================
/**
 * @file   EvolutionFrameFactory.cpp
 * @date   2015/08/28 Fri. 17:46:45
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../EvolutionFrameDef.h"
#include  "./EvolutionWork.h"

/*  frames  */
#include  "../Frames/EvolutionFrame_EvolutionDemo.h"
#include  "../Frames/EvolutionFrame_CheckZukan.h"
#include  "../Frames/EvolutionFrame_CheckWaza.h"
#include  "../Frames/EvolutionFrame_WaitForDemoEnd.h"

#include  "./EvolutionFrameFactory.h"


namespace NetApp    {
namespace Evolution {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     FrameFactory
  *  @brief    ctor
  */
//------------------------------------------------------------------
FrameFactory::FrameFactory(EvolutionWork* pWork)
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
    case  FRAME_EvolutionDemo:  pFrame  = GFL_NEW(pHeapBase)  Frame_EvolutionDemo(m_pWork);   break;
    case  FRAME_CheckZukan:     pFrame  = GFL_NEW(pHeapBase)  Frame_CheckZukan(m_pWork);      break;
    case  FRAME_CheckWaza:      pFrame  = GFL_NEW(pHeapBase)  Frame_CheckWaza(m_pWork);       break;
    case  FRAME_WaitForDemoEnd: pFrame  = GFL_NEW(pHeapBase)  Frame_WaitForDemoEnd(m_pWork);  break;

    default:
      GFL_PRINT("%s(%d):[warning]: unknown frameID(%d)\n", __FILE__, __LINE__, static_cast<u32>(frameID));
      break;
  }


  return pFrame;
}



} /*  namespace Evolution */
} /*  namespace NetApp    */


