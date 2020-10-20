#if !defined(GIFTFRAME_RECEIVEIR_H_INCLUDED)
#define GIFTFRAME_RECEIVEIR_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftFrame_ReceiveIr.h
 * @date   2015/08/05 Wed. 12:21:24
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include  "../GiftFrameDef.h"

#include  "../Common/DeliveryUtilRetryHelper.h"
#include  "../Views/GiftViewContainer.h"

namespace NetApp  {
namespace Gift {
  class GiftWork;
  class GiftViewContainer;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  */
//==============================================================================
class Frame_ReceiveIr
  : public    NetAppLib::System::NetAppFrameBase
  , public    DeliveryUtil::IRecvListener
  , protected LowerView_Back::IEventHandler
  , protected GiftMessageMenu::IValueProvider
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_ReceiveIr );

public:

  //------------------------------------------------------------------
  /**
    *  @func     Frame_ReceiveIr
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  Frame_ReceiveIr(GiftWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @func     ~Frame_ReceiveIr
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~Frame_ReceiveIr();




private:
  /*
    override  NetAppFrameBase I/F
  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();


  void  NetError(const LowerView_Back::ViewMode errorViewMode)  {m_IsError  = true; m_ErrorViewMode = errorViewMode;}
  void  ShowError(void);
  void  ExitFrame(const FrameResult = RESULT_RECEIVE_IR_Exit);

  /*  implement LowerView_Back::IEventHandler I/F  */
  virtual void  LowerView_Back_OnEvent(const LowerView_Back::IEventHandler::EventCode eventCode);

  /*  implement DeliveryUtil::IRecvListener I/F  */
  virtual bool  DeliveryUtilRecvListener_OnRecvComplete(const CompleteResult completeResult, const void* pData=NULL, size_t dataSize=0);
  virtual void  DeliveryUtilRecvListener_OnUpdateProgress( const u32 progress );
  virtual void  DeliveryUtilRecvListener_OnNintendoZoneConnected();
  virtual void  DeliveryUtilRecvListener_OnNetworkError( const ResultInfo& rResultInfo );
  virtual void  DeliveryUtilRecvListener_OnWifiError( const RecvWifiErrorCode errorCode );

  /*    */
  virtual u32   ValueProvider_AmountSpace(GiftMessageMenu* pSender)  {return 0;}  /**<  あとn件       */
  virtual u32   ValueProvider_ReceiveProgress(GiftMessageMenu* pSender);          /**<  受信Progress  */


  /*
    members
  */
  GiftWork*                 m_pWork;
  GiftViewContainer*        m_pViewContainer;
  u32                       m_State;
  FrameResult               m_Result;


  DeliveryUtilRetryHelper   m_DelivHelper;
  bool                      m_IsReceived;

  /*  error  */
  bool                      m_IsError;                /**<  エラーが発生した  */
  LowerView_Back::ViewMode  m_ErrorViewMode;

  LowerView_Back*           m_pLowerView_Back;

  /*  delivery  */
  u32                       m_Progress;

};  /*  class GiftFrame  */





} /*  namespace Gift */
} /*  namespace NetApp      */
#endif  /*  #if !defined(GIFTFRAME_RECEIVEIR_H_INCLUDED)  */
