#if !defined(GIFTFRAME_RECEIVESERIAL_H_INCLUDED)
#define GIFTFRAME_RECEIVESERIAL_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftFrame_ReceiveSerial.h
 * @date   2015/08/21 Fri. 13:12:42
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


#include  "App/NumberInput/include/NumberInputAppParam.h"
#include  "App/NumberInput/include/NumberInputFrame.h"

#include  "NetStatic/NetAppLib/include/Util/NetAppSaveUtility.h"

#include  "../Common/DeliveryUtilRetryHelper.h"
#include  "../Common/AuthenticationHelper.h"

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
  *  @class  Frame_ReceiveSerial
  */
//==============================================================================
class Frame_ReceiveSerial
  : public NetAppLib::System::NetAppFrameBase
  , public LowerView_Back::IEventHandler
  , protected DeliveryUtil::IRecvListener
  , protected GiftMessageMenu::IValueProvider
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_ReceiveSerial );

public:

  //------------------------------------------------------------------
  /**
    *  @func     Frame_ReceiveSerial
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  Frame_ReceiveSerial(GiftWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @func     ~Frame_ReceiveSerial
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~Frame_ReceiveSerial();



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


  /*  implement valueProvider  */
  virtual u32   ValueProvider_AmountSpace(GiftMessageMenu* pSender)     {return 0;}            /**<  あとn件       */
  virtual u32   ValueProvider_ReceiveProgress(GiftMessageMenu* pSender) {return m_Progress;}   /**<  受信Progress  */


  void  NetError(const LowerView_Back::ViewMode errorViewMode)  {m_IsError  = true; m_ErrorViewMode = errorViewMode;}
  void  ExitFrame(const FrameResult = RESULT_RECEIVE_INTERNET_Exit);

  /*  resultHandler  */
  void  ResultHandler(const AuthenticationHelper::InternetConnectResult     helperResult);
  void  ResultHandler(const AuthenticationHelper::FriendServerConnectResult helperResult);
  bool  ResultHandler_InputSerial(void);
  void  ResultHandler(const AuthenticationHelper::PGLSerialAuthResult       helperResult);
//  void  ResultHandler(const AuthenticationHelper::CommitSaveResult          helperResult);



  /*
    members
  */
  GiftWork*                 m_pWork;
  GiftViewContainer*        m_pViewContainer;
  u32                       m_State;
  FrameResult               m_Result;

  u32                       m_Progress;
  bool                      m_IsReceived;

  bool                      m_IsError;                /**<  エラーが発生した  */
  LowerView_Back::ViewMode  m_ErrorViewMode;

  /*  views  */
  UpperView_Message*        m_pUpperView_Message;
  LowerView_Back*           m_pLowerView_Back;

  /*  input  */
  gfl2::ro::Module*                   m_pInputROModule;
  App::NumberInput::APP_PARAM         m_InputParam;
  App::NumberInput::NumberInputFrame* m_pInputFrame;
  gfl2::str::STRCODE                  m_SerialString[App::NumberInput::PART_MAX * App::NumberInput::ALPHABET_INPUT_MAX +1];
  bool                                m_IsSerialAuthPassed;
  bool                                m_IsSerialTimeoutPenalty;

  /*  helper  */
  DeliveryUtilRetryHelper             m_DelivHelper;
  AuthenticationHelper*               m_pAuthHelper;
  NetAppLib::Util::NetAppSaveUtility  m_SaveUtility;

};  /*  class GiftFrame  */





} /*  namespace Gift */
} /*  namespace NetApp      */
#endif  /*  #if !defined(GIFTFRAME_RECEIVESERIAL_H_INCLUDED)  */
