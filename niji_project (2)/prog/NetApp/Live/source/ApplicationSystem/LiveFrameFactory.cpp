
//==============================================================================
/**
 * @file   LiveFrameFactory.cpp
 * @date   2015/07/09 Thu. 14:42:07
 * @author muto_yu
 * @brief  Live大会FrameFactory
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../LiveFrameDef.h"
#include  "./LiveWork.h"

/*  frames  */
#include  "../Frames/LiveFrame_Main.h"
#include  "../Frames/LiveFrame_ReceivePlayerPass.h"
#include  "../Frames/LiveFrame_Registration.h"
#include  "../Frames/LiveFrame_CheckRemainingMatches.h"
#include  "../Frames/LiveFrame_MatchMenu.h"
#include  "../Frames/LiveFrame_ReplayMatch.h"
#include  "../Frames/LiveFrame_Deregistration.h"
#include  "../Frames/LiveFrame_Matching.h"
#include  "../Frames/LiveFrame_MatchResult.h"

#include  "./LiveFrameFactory.h"


namespace NetApp  {
namespace Live {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     FrameFactory
  *  @brief    ctor
  */
//------------------------------------------------------------------
FrameFactory::FrameFactory(LiveWork* pWork)
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
    case  FRAME_Main:                   pFrame  = GFL_NEW(pHeapBase)  Frame_Main(m_pWork);                  break;
    case  FRAME_ReceivePlayerPass:      pFrame  = GFL_NEW(pHeapBase)  Frame_ReceivePlayerPass(m_pWork);     break;
    case  FRAME_Registration:           pFrame  = GFL_NEW(pHeapBase)  Frame_Registration(m_pWork);          break;
    case  FRAME_CheckRemainingMatches:  pFrame  = GFL_NEW(pHeapBase)  Frame_CheckRemainingMatches(m_pWork); break;
    case  FRAME_MatchMenu:              pFrame  = GFL_NEW(pHeapBase)  Frame_MatchMenu(m_pWork);             break;
    case  FRAME_ReplayMatch:            pFrame  = GFL_NEW(pHeapBase)  Frame_ReplayMatch(m_pWork);           break;
    case  FRAME_Deregistration:         pFrame  = GFL_NEW(pHeapBase)  Frame_Deregistration(m_pWork);        break;
    case  FRAME_Matching:               pFrame  = GFL_NEW(pHeapBase)  Frame_Matching(m_pWork);              break;
    case  FRAME_MatchResult:            pFrame  = GFL_NEW(pHeapBase)  Frame_MatchResult(m_pWork);           break;

    default:
      GFL_PRINT("%s(%d):[warning]: unknown frameID(%d)\n", __FILE__, __LINE__, static_cast<u32>(frameID));
      break;
  }


  return pFrame;
}





} /*  namespace Live */
} /*  namespace NetApp      */
