//==============================================================================
/**
 * @file   GiftFrame_ReceiveInternet.cpp
 * @date   2015/08/21 Fri. 13:05:25
 * @author muto_yu
 * @brief  受信：Internet
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include  "../ApplicationSystem/GiftWork.h"
#include  "../GiftResource.h"

#include  "./GiftFrame_ReceiveInternet.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    /*  init  */
    STATE_Entry = 0,

    STATE_InternetConfirm,


    /*  pre  */
    STATE_ConnectStart,
    STATE_Connecting,

    /*  receive  */
    STATE_StartReceive,
    STATE_Receiving,              /**<  データ受信中                  */

    /*  confirm */
    STATE_CancelConfirm,

    /*  終了処理  */
    STATE_Quit_Request,
    STATE_Quit_Prepare,           /**<  deliveryManagerのprepare待ち  */
    STATE_Quit_ShowError,         /**<  エラー表示                    */


    /*  exit  */
    STATE_Update_Exit,

    STATE_     /*  terminator  */
  };
} /*  namespace  */


namespace NetApp  {
namespace Gift {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     Frame_ReceiveInternet
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_ReceiveInternet::Frame_ReceiveInternet(GiftWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_DelivHelper(this)
  , m_Progress(0)
  , m_IsReceived(0)
  , m_IsError(false)
  , m_ErrorViewMode(LowerView_Back::VIEWMODE_Error_ReceiveFailed)
  , m_pLowerView_Back(NULL)

{
  m_pLowerView_Back  = m_pViewContainer->m_pLowerView_Back;
}


//------------------------------------------------------------------
/**
  *  @func     Frame_ReceiveInternet
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_ReceiveInternet::~Frame_ReceiveInternet()
{
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  Frame_ReceiveInternet::startup()
{
  m_pViewContainer->SetScene(GiftViewContainer::SCENE_Receive_Internet);
  m_pViewContainer->SetAllInputEnable(true);

  m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_None);

  m_pWork->m_pAppParam->m_bReceiveOneData  = false;

  return true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_ReceiveInternet::setListener()
{
  m_pLowerView_Back->SetEventHandler(this);
  m_pLowerView_Back->SetMessageMenuValueProvider(this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_ReceiveInternet::removeListener()
{
  m_pLowerView_Back->SetEventHandler(NULL);
  m_pLowerView_Back->SetMessageMenuValueProvider(NULL);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_ReceiveInternet::updateSequence()
{
  m_DelivHelper.Update();

  switch(m_State)
  {
    case  STATE_Entry:
      m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Serial_InternetConfirm);   /*  インターネットに接続します Y/N  */
      m_State = STATE_InternetConfirm;
     /*  fallthrough  */

    case  STATE_InternetConfirm:
      break;


    /*  receive  */
    case  STATE_StartReceive:
      m_DelivHelper.Start(DeliveryUtil::MODE_Boss, false);
      m_State = STATE_Receiving;
      break;

    case  STATE_Receiving:              /**<  データ受信中                  */
      if(!(DeliveryUtil::GetInstance()->GetWirelessSwitchState()))
      {
        /*  無線OFF  */
        NetError(LowerView_Back::VIEWMODE_Error_WirelessOff);
      }


      if(m_IsError)
      {
        ExitFrame();
      }
      else if(m_IsReceived)
      {
        ExitFrame(RESULT_RECEIVE_INTERNET_Received);
      }
      break;



    /*  confirm */
    case  STATE_CancelConfirm:
      break;



    /*  終了処理  */
    case  STATE_Quit_Request:
      /*  「終了しています」はここで出す  */
      if(m_ErrorViewMode == LowerView_Back::VIEWMODE_Finalizing)
      {
        m_pLowerView_Back->SetViewMode(m_ErrorViewMode);
      }
      m_DelivHelper.RequestFinalize(false);
      m_State = STATE_Quit_Prepare;
      break;

    case  STATE_Quit_Prepare:            /**<  deliveryManagerのprepare待ち  */
      if(m_DelivHelper.GetStatus() == DeliveryUtilRetryHelper::STATUS_Finalized)
      {
        if(m_IsError)
        {
          /*  「終了しています」は表示済み  */
          if(m_ErrorViewMode == LowerView_Back::VIEWMODE_Finalizing)
          {
            m_State = STATE_Update_Exit;
          }
          else
          {
            m_pLowerView_Back->SetViewMode(m_ErrorViewMode);
            m_State = STATE_Quit_ShowError;
          }
        }
        else
        {
          m_State = STATE_Update_Exit;
        }
      }
      break;

    case  STATE_Quit_ShowError:              /**<  エラー表示                    */
      break;


    case  STATE_Update_Exit:
      /*
        workにFrameResultを設定することでSwitcherが呼ばれる
      */
      m_pWork->SetFrameResult( m_Result );
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  Frame_ReceiveInternet::cleanup()
{
  bool  isFinish  = true;

  if(m_DelivHelper.GetStatus() != DeliveryUtilRetryHelper::STATUS_Finalized)
  {
    m_DelivHelper.Update();
    isFinish  = false;
  }

  return isFinish;
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
/*  implement LowerView_Back::IEventHandler  */
void  Frame_ReceiveInternet::LowerView_Back_OnEvent(const LowerView_Back::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {



    case  LowerView_Back::IEventHandler::EVENT_Exit:
      if(m_State == STATE_Receiving)
      {
#if 0
        /*  「さがしています」なら中断確認  */
        m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_ReceiveCancelConfirm);
        m_State = STATE_CancelConfirm;
#else
        /*  confirm不要とのこと  */
        NetError(LowerView_Back::VIEWMODE_Finalizing);  /*  終了しています  */
        ExitFrame();
#endif
      }
      break;


    /*  internetconfirm  */
    case  LowerView_Back::IEventHandler::EVENT_InternetConfirm_Yes:
      if(m_State == STATE_InternetConfirm)
      {
        m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Searching);
        m_State  = STATE_StartReceive;
      }
      break;


    case  LowerView_Back::IEventHandler::EVENT_InternetConfirm_No:
      if(m_State == STATE_InternetConfirm)
      {
        ExitFrame();
      }
      break;


    /*  confirm:yes  */
    case  LowerView_Back::IEventHandler::EVENT_ReceiveCancelConfirm_Yes:
      if(m_State == STATE_CancelConfirm)
      {
        /*  見つかりませんでした  */
        NetError(LowerView_Back::VIEWMODE_Error_NotFound); /*  見つかりませんでした  */
        ExitFrame();
     }
      break;
    
    /*  confirm:no  */
    case  LowerView_Back::IEventHandler::EVENT_ReceiveCancelConfirm_No:
      if(m_State == STATE_CancelConfirm)
      {
        /*  「さがしています（うけとるのをやますか）」なら受信継続  */
        m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Searching);
        m_State = STATE_Receiving;
      }
      break;


    case  LowerView_Back::IEventHandler::EVENT_MessageFinish:
      if(m_State == STATE_Quit_ShowError)
      {
        m_State = STATE_Update_Exit;
      }
      break;

    default:
      break;
  }

}
/*  endimplement LowerView_Back::IEventHandler  */


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
/*  implement DeliveryUtil::IRecvListener I/F  */
bool  Frame_ReceiveInternet::DeliveryUtilRecvListener_OnRecvComplete(const CompleteResult completeResult, const void* pData , size_t dataSize )
{
  GFL_PRINT("%s:\n", __FUNCTION__);
  m_Progress  = 100;

  switch(completeResult)
  {
    case  COMPLETE_RESULT_OK:
      if(m_pWork->m_pAppParam->SetReceivedDeliveryData(pData, dataSize))
      {
        m_IsReceived  = true;
      }
      else
      {
        /*  失敗扱い  */
        DeliveryUtilRecvListener_OnRecvComplete(COMPLETE_RESULT_ReceiveFailed);
      }
      break;

    case  COMPLETE_RESULT_NotFound:               /**<  見つからなかった  */
      NetError(LowerView_Back::VIEWMODE_Error_NotFound);                /**<  見つからなかった          */
      break;

    case  COMPLETE_RESULT_FoundButCannotReceive:  /**<  見つけたが受け取れない  */
      NetError(LowerView_Back::VIEWMODE_Error_FoundButCannotReceive);   /**<  みつけたけどうけとれない    */
      break;

    case  COMPLETE_RESULT_ReceiveFailed:          /**<  受け取りに失敗          */
    default:
      NetError(LowerView_Back::VIEWMODE_Error_ReceiveFailed); /**<  うけとりに失敗しました  */
      break;
  }

  return false;   /*  確定  */
}


void  Frame_ReceiveInternet::DeliveryUtilRecvListener_OnUpdateProgress( const u32 progress )
{
  GFL_PRINT("%s: %d%%\n", __FUNCTION__, progress);

  m_Progress  = progress;
  if(m_State != STATE_CancelConfirm)
  {
    m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Searching);
  }
}


void  Frame_ReceiveInternet::DeliveryUtilRecvListener_OnNintendoZoneConnected()
{
  GFL_PRINT("%s:\n", __FUNCTION__);

  /*
    Contact[811]   「ふしぎなおくりもの」でzone配信する時の属性値について
    属性をニンテンドーゾーン用に変更 （Internet(Wifi)受信のみ）
    
    ニンテンドーゾーン時は属性「2」をクリア（∴Index==1）
    #0: 属性1
    #1: 属性2
    #2: 属性3
  */
  DeliveryUtil::GetInstance()->ClearAttribute(1);
}


void  Frame_ReceiveInternet::DeliveryUtilRecvListener_OnNetworkError( const ResultInfo& rResultInfo )
{
  GFL_PRINT("%s:\n", __FUNCTION__);

  NetError(LowerView_Back::VIEWMODE_Error_ReceiveFailed); /**<  うけとりに失敗しました  */
}


void  Frame_ReceiveInternet::DeliveryUtilRecvListener_OnWifiError( const RecvWifiErrorCode errorCode )
{
  GFL_PRINT("%s:\n", __FUNCTION__);

  NetError(LowerView_Back::VIEWMODE_Error_ReceiveFailed); /**<  うけとりに失敗しました  */
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_ReceiveInternet::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
  m_State   = STATE_Quit_Request;
  m_pViewContainer->SetAllInputEnable(false);
}



} /*  namespace Gift    */
} /*  namespace NetApp  */
