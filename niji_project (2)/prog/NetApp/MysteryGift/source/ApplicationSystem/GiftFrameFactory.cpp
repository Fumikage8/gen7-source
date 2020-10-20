//==============================================================================
/**
 * @file   GiftFrameFactory.cpp
 * @date   2015/07/31 Fri. 15:47:40
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../GiftFrameDef.h"
#include  "./GiftWork.h"

/*  frames  */
#include  "../Frames/GiftFrame_Main.h"
#include  "../Frames/GiftFrame_Instruction.h"
#include  "../Frames/GiftFrame_Album.h"
#include  "../Frames/GiftFrame_Receive.h"
#include  "../Frames/GiftFrame_ReceiveIr.h"
#include  "../Frames/GiftFrame_ReceiveInternet.h"
#include  "../Frames/GiftFrame_ReceiveSerial.h"
#include  "../Frames/GiftFrame_SelectData.h"
#include  "../Frames/GiftFrame_ReceiveResult.h"
/*  receiveSerial  */
#include  "../Frames/ReceiveSerial/GiftFrame_ReceiveSerial_Connection.h"
#include  "../Frames/ReceiveSerial/GiftFrame_ReceiveSerial_InputCode.h"
#include  "../Frames/ReceiveSerial/GiftFrame_ReceiveSerial_Receiving.h"

#include  "./GiftFrameFactory.h"


namespace NetApp  {
namespace Gift {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     FrameFactory
  *  @brief    ctor
  */
//------------------------------------------------------------------
FrameFactory::FrameFactory(GiftWork* pWork)
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
  gfl2::heap::HeapBase*   pHeapBase = m_pWork->GetAppHeap()->GetDeviceHeap();

  switch(frameID)
  {
    case  FRAME_Main:                     return GFL_NEW(pHeapBase)  Frame_Main(m_pWork);
    case  FRAME_Instruction:              return GFL_NEW(pHeapBase)  Frame_Instruction(m_pWork);
    case  FRAME_CardAlbum:                return GFL_NEW(pHeapBase)  Frame_Album(m_pWork);
    case  FRAME_Receive:                  return GFL_NEW(pHeapBase)  Frame_Receive(m_pWork);
    case  FRAME_ReceiveIr:                return GFL_NEW(pHeapBase)  Frame_ReceiveIr(m_pWork);
    case  FRAME_ReceiveInternet:          return GFL_NEW(pHeapBase)  Frame_ReceiveInternet(m_pWork);
    case  FRAME_ReceiveSerial:            return GFL_NEW(pHeapBase)  Frame_ReceiveSerial(m_pWork);
    case  FRAME_SelectData:               return GFL_NEW(pHeapBase)  Frame_SelectData(m_pWork);
    case  FRAME_ReceiveResult:            return GFL_NEW(pHeapBase)  Frame_ReceiveResult(m_pWork);
    /*  receiveSerial  */
    case  FRAME_ReceiveSerial_Connection: return GFL_NEW(pHeapBase)  Frame_ReceiveSerial_Connection(m_pWork);
    case  FRAME_ReceiveSerial_InputCode:  return GFL_NEW(pHeapBase)  Frame_ReceiveSerial_InputCode(m_pWork);
    case  FRAME_ReceiveSerial_Receiving:  return GFL_NEW(pHeapBase)  Frame_ReceiveSerial_Receiving(m_pWork);

    default:
      GFL_PRINT("%s(%d):[warning]: unknown frameID(%d)\n", __FILE__, __LINE__, static_cast<u32>(frameID));
      return NULL;
  }
}





} /*  namespace Gift */
} /*  namespace NetApp      */
