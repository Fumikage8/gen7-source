
#if !defined(GIFTFRAME_RECEIVEINTERNET_H_INCLUDED)
#define GIFTFRAME_RECEIVEINTERNET_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftFrame_ReceiveInternet.h
 * @date   2015/08/21 Fri. 13:02:06
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

#include  "../Views/GiftViewContainer.h"
#include  "../Common/DeliveryUtilRetryHelper.h"

namespace NetApp  {
namespace Gift {
  class GiftWork;
  class GiftViewContainer;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  Frame_ReceiveInternet
  */
//==============================================================================
class Frame_ReceiveInternet
  : public    NetAppLib::System::NetAppFrameBase
  , protected LowerView_Back::IEventHandler
  , protected GiftMessageMenu::IValueProvider
  , protected DeliveryUtil::IRecvListener
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_ReceiveInternet );

public:

  //------------------------------------------------------------------
  /**
    *  @func     Frame_ReceiveInternet
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  Frame_ReceiveInternet(GiftWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @func     ~Frame_ReceiveInternet
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~Frame_ReceiveInternet();


private:
  /*
    override  NetAppFrameBase I/F
  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();

  /*  implement LowerView_Back::IEventHandler  */
  virtual void  LowerView_Back_OnEvent(const LowerView_Back::IEventHandler::EventCode eventCode);

  /*  implement DeliveryUtil::IRecvListener I/F  */
  virtual bool  DeliveryUtilRecvListener_OnRecvComplete(const CompleteResult completeResult, const void* pData=NULL, size_t dataSize=0);
  virtual void  DeliveryUtilRecvListener_OnUpdateProgress( const u32 progress );
  virtual void  DeliveryUtilRecvListener_OnNintendoZoneConnected();
  virtual void  DeliveryUtilRecvListener_OnNetworkError( const ResultInfo& rResultInfo );
  virtual void  DeliveryUtilRecvListener_OnWifiError( const RecvWifiErrorCode errorCode );

  /*  implement GiftMessageMenu::IValueProvider  */
  virtual u32   ValueProvider_AmountSpace(GiftMessageMenu* pSender)     {return 0;}           /**<  あとn件       */
  virtual u32   ValueProvider_ReceiveProgress(GiftMessageMenu* pSender) {return m_Progress;}  /**<  受信Progress  */


  void  NetError(const LowerView_Back::ViewMode errorViewMode)  {m_IsError  = true; m_ErrorViewMode = errorViewMode;}
  void  ExitFrame(const FrameResult = RESULT_RECEIVE_INTERNET_Exit);


  /*
    members
  */
  GiftWork*                 m_pWork;
  GiftViewContainer*        m_pViewContainer;
  u32                       m_State;
  FrameResult               m_Result;

  DeliveryUtilRetryHelper   m_DelivHelper;
  u32                       m_Progress;
  bool                      m_IsReceived;
  bool                      m_IsError;                /**<  エラーが発生した  */
  LowerView_Back::ViewMode  m_ErrorViewMode;

  LowerView_Back*           m_pLowerView_Back;
};  /*  class GiftFrame  */





} /*  namespace Gift */
} /*  namespace NetApp      */
#endif  /*  #if !defined(GIFTFRAME_RECEIVEINTERNET_H_INCLUDED)  */
