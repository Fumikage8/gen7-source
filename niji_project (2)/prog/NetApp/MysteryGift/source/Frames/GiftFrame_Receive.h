
#if !defined(GIFTFRAME_RECEIVE_H_INCLUDED)
#define GIFTFRAME_RECEIVE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftFrame_Receive.h
 * @date   2015/08/04 Tue. 12:52:38
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
#include  "../Common/GiftDeliveryUtil.h"
#include  "../Common/DeliveryUtilRetryHelper.h"

#include  "../Views/GiftViewContainer.h"


namespace NetApp  {
namespace Gift    {
  class GiftWork;
  class GiftViewContainer;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  Frame_Receive
  *  @brief  
  */
//==============================================================================
class Frame_Receive
  : public    NetAppLib::System::NetAppFrameBase
  , protected DeliveryUtil::IRecvListener
  , protected NetAppLib::UI::NetAppMessageMenu::IEventHandler
  , protected LowerView_Selection::IEventHandler
  , protected LowerView_Back::IEventHandler
  , protected GiftMessageMenu::IValueProvider
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_Receive );

public:

  //------------------------------------------------------------------
  /**
    *  @func     Frame_Receive
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  Frame_Receive(GiftWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @func     ~Frame_Receive
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~Frame_Receive();



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
  void  ExitFrame(const FrameResult = RESULT_Receive_Exit);


  /*
      implement LowerView_Selection::IEventHandler I/F
  */
  virtual void  LowerView_Selection_OnEvent(const LowerView_Selection::IEventHandler::EventCode eventCode);

  /*
      implement LowerView_Back::IEventHandler I/F
  */
  virtual void  LowerView_Back_OnEvent(const LowerView_Back::IEventHandler::EventCode eventCode);


  /*
      implement NetAppLib::UI::NetAppMessageMenu::IEventHandler I/F
  */
  virtual void  MessageMenuView_OnEvent(const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eventCode);


  /*
      implement DeliveryUtil::IRecvListener I/F
  */
  virtual bool  DeliveryUtilRecvListener_OnRecvComplete(const CompleteResult completeResult, const void* pData=NULL, size_t dataSize=0);
  virtual void  DeliveryUtilRecvListener_OnUpdateProgress( const u32 progress );
  virtual void  DeliveryUtilRecvListener_OnNintendoZoneConnected();
  virtual void  DeliveryUtilRecvListener_OnNetworkError( const ResultInfo& rResultInfo );
  virtual void  DeliveryUtilRecvListener_OnWifiError( const RecvWifiErrorCode errorCode );
  /*
      end implement DeliveryUtil::IRecvListener I/F
  */

  /*    */
  virtual u32   ValueProvider_AmountSpace(GiftMessageMenu* pSender);          /**<  あとn件       */
  virtual u32   ValueProvider_ReceiveProgress(GiftMessageMenu* pSender);      /**<  受信Progress  */


  /*
    members
  */
  GiftWork*                 m_pWork;
  GiftViewContainer*        m_pViewContainer;
  u32                       m_State;
  FrameResult               m_Result;

  DeliveryUtilRetryHelper   m_DelivHelper;
  u32                       m_Progress;
  bool                      m_IsError;                /**<  エラーが発生した  */
  LowerView_Back::ViewMode  m_ErrorViewMode;
  bool                      m_IsReceivedDataViaUDS;   /**<  local通信でのデータ受信に成功したか  */
  bool                      m_WirelessSwitchState;

  /*  view  */
  UpperView_Message*        m_pUpperView;
  LowerView_Selection*      m_pLowerView;
  LowerView_Back*           m_pLowerView_Back;
};  /*  class GiftFrame  */





} /*  namespace Gift */
} /*  namespace NetApp      */
#endif  /*  #if !defined(GIFTFRAME_RECEIVE_H_INCLUDED)  */
