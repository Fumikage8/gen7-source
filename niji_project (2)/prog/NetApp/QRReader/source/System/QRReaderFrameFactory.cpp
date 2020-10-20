
//==============================================================================
/**
 * @file   QRReaderFrameFactory.cpp
 * @date   2016/01/14 Thu. 17:16:20
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../QRReaderFrameDef.h"
#include  "./QRReaderWork.h"

/*  frames  */
#include  "../Frames/QRReaderFrame_Tutorial.h"
#include  "../Frames/QRReaderFrame_QRReadDummy.h"
#include  "../Frames/QRReaderFrame_FieldScan.h"
#include  "../Frames/QRReaderFrame_FieldScanRunning.h"


//#if defined(GF_PLATFORM_WIN32)
#if defined(GF_PLATFORM_CTR)
  #include  "../Frames/QRReaderFrame_QRRead.h"
  #include  "../Frames/QRReaderFrame_QRSimpleRead.h"
#endif


#include  "./QRReaderFrameFactory.h"


namespace NetApp    {
namespace QRReader {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     FrameFactory
  *  @brief    ctor
  */
//------------------------------------------------------------------
FrameFactory::FrameFactory(QRReaderWork* pWork)
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
    case  FRAME_Tutorial:         pFrame  = GFL_NEW(pHeapBase)  Frame_Tutorial(m_pWork);          break;


#if defined(GF_PLATFORM_CTR)
    case  FRAME_QRRead:           pFrame  = GFL_NEW(pHeapBase)  Frame_QRRead(m_pWork);            break;
    case  FRAME_QRSimpleRead:     pFrame  = GFL_NEW(pHeapBase)  Frame_QRSimpleRead(m_pWork);      break;
#else
    case  FRAME_QRRead:           pFrame  = GFL_NEW(pHeapBase)  Frame_QRReadDummy(m_pWork);       break;
    case  FRAME_QRSimpleRead:     pFrame  = GFL_NEW(pHeapBase)  Frame_QRReadDummy(m_pWork);       break;
#endif

    case  FRAME_FieldScan:        pFrame  = GFL_NEW(pHeapBase)  Frame_FieldScan(m_pWork);         break;
    case  FRAME_FieldScanRunning: pFrame  = GFL_NEW(pHeapBase)  Frame_FieldScanRunning(m_pWork);  break;


    default:
      GFL_PRINT("%s(%d):[warning]: unknown frameID(%d)\n", __FILE__, __LINE__, static_cast<u32>(frameID));
      break;
  }


  return pFrame;
}



} /*  namespace QRReader  */
} /*  namespace NetApp    */

