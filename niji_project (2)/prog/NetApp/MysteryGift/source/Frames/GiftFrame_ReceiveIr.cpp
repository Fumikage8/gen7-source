//==============================================================================
/**
 * @file   GiftFrame_ReceiveIr.cpp
 * @date   2015/08/05 Wed. 12:29:33
 * @author muto_yu
 * @brief  
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
#include  "../Common/GiftDeliveryUtil.h"

#include  "./GiftFrame_ReceiveIr.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    /*  init  */
    STATE_Entry = 0,

    /*  pre  */
    STATE_FirstConfirm,

    /*  receive  */
    STATE_StartReceive,
    STATE_Receiving,              /**<  データ受信中                  */

    /*  confirm */
    STATE_CancelConfirm,

    /*  終了処理  */
    STATE_Quit_Request,
    STATE_Quit_Prepare,            /**<  deliveryManagerのprepare待ち  */
    STATE_Quit_ShowError,          /**<  エラー表示                    */

    STATE_Update_Exit,

    STATE_     /*  terminator  */
  };
} /*  namespace  */


namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     Frame_ReceiveIr
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_ReceiveIr::Frame_ReceiveIr(GiftWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_DelivHelper(this)
  , m_IsReceived(false)
  /*  error  */
  , m_IsError(false)
  , m_ErrorViewMode(LowerView_Back::VIEWMODE_Error_ReceiveFailed)

  , m_pLowerView_Back(NULL)
  , m_Progress(0)

{
}


//------------------------------------------------------------------
/**
  *  @func     Frame_ReceiveIr
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_ReceiveIr::~Frame_ReceiveIr()
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
bool  Frame_ReceiveIr::startup()
{
  m_pViewContainer->SetScene(GiftViewContainer::SCENE_Receive_Ir);
  m_pViewContainer->SetAllInputEnable(true);

  m_pLowerView_Back    = m_pViewContainer->m_pLowerView_Back;
  m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_None);

  m_pWork->m_pAppParam->m_bReceiveOneData  = true;  /**< Irは一件ずつしか受け取れない  */

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
void  Frame_ReceiveIr::setListener()
{
  m_pLowerView_Back->SetMessageMenuValueProvider(this);
  m_pLowerView_Back->SetEventHandler(this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_ReceiveIr::removeListener()
{
  m_pLowerView_Back->SetMessageMenuValueProvider(NULL);
  m_pLowerView_Back->SetEventHandler(NULL);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_ReceiveIr::updateSequence()
{
  m_DelivHelper.Update();

  switch(m_State)
  {
    case  STATE_Entry:
      m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_IrConfirm);
      m_State = STATE_FirstConfirm;
      /*  fallthrough  */

    case  STATE_FirstConfirm:
      break;

    /*
      receive
    */
    case  STATE_StartReceive:
    {
      m_DelivHelper.Start(DeliveryUtil::MODE_Ir, false);
      m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Searching);
      m_State = STATE_Receiving;
    }
      break;

    case  STATE_Receiving:
      if(m_IsError)
      {
        ExitFrame();
      }
      else if(m_IsReceived)
      {
        ExitFrame(RESULT_RECEIVE_IR_Received);
      }
      break;

    /*
      confirm
    */
    case  STATE_CancelConfirm:
      break;

    /*
      以下終了シーケンス
      ExitFrame()はここから
    */
    case  STATE_Quit_Request:
      /*  「終了しています」はここで出す  */
      if(m_ErrorViewMode == LowerView_Back::VIEWMODE_Finalizing)
      {
        m_pLowerView_Back->SetViewMode(m_ErrorViewMode);
      }
      m_DelivHelper.RequestFinalize(false);
      m_State = STATE_Quit_Prepare;
      break;

    case  STATE_Quit_Prepare:
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


    case  STATE_Quit_ShowError:
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
bool  Frame_ReceiveIr::cleanup()
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
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
/*  implement LowerView_Back::IEventHandler I/F  */
void  Frame_ReceiveIr::LowerView_Back_OnEvent(const LowerView_Back::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Back::IEventHandler::EVENT_Exit:
      if(m_State  == STATE_Receiving)
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

    /*  confirm:yes  */
    case  LowerView_Back::IEventHandler::EVENT_ReceiveCancelConfirm_Yes:
      if(m_State  == STATE_CancelConfirm)
      {
        NetError(LowerView_Back::VIEWMODE_Error_NotFound); /*  見つかりませんでした  */
        ExitFrame();
      }
      break;

    /*  confirm:no  */
    case  LowerView_Back::IEventHandler::EVENT_ReceiveCancelConfirm_No:
      if(m_State  == STATE_CancelConfirm)
      {
        m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Searching);
        m_State = STATE_Receiving;
      }
      break;


    /*  Ir  */
    case  LowerView_Back::IEventHandler::EVENT_IrConfirm_Yes:
      if(m_State  == STATE_FirstConfirm)
      {
        m_State = STATE_StartReceive;
      }
      break;

    case  LowerView_Back::IEventHandler::EVENT_IrConfirm_No:
      if(m_State  == STATE_FirstConfirm)
      {
        /*  直接終了  */
        m_Result  = RESULT_RECEIVE_IR_Exit;
        m_State   = STATE_Update_Exit;
      }
      break;


    case  LowerView_Back::IEventHandler::EVENT_MessageFinish:
      if(m_State  == STATE_Quit_ShowError)
      {
        m_State = STATE_Update_Exit;
      }
      break;
  }
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
/*  implement DeliveryUtil::IRecvListener I/F  */
bool Frame_ReceiveIr::DeliveryUtilRecvListener_OnRecvComplete(const CompleteResult completeResult, const void* pData, size_t dataSize )
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


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void Frame_ReceiveIr::DeliveryUtilRecvListener_OnUpdateProgress( const u32 progress )
{
  GFL_PRINT("%s: %d%%\n", __FUNCTION__, progress);
  m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Searching);
  m_Progress  = progress;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void Frame_ReceiveIr::DeliveryUtilRecvListener_OnNintendoZoneConnected()
{
  GFL_PRINT("%s:\n", __FUNCTION__);
  GFL_ASSERT(false);    /*  irでは想定外  */

  NetError(LowerView_Back::VIEWMODE_Finalizing);  /*  終了しています  */
  ExitFrame();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void Frame_ReceiveIr::DeliveryUtilRecvListener_OnNetworkError( const ResultInfo& rResultInfo )
{
  GFL_PRINT("%s:\n", __FUNCTION__);
  NetError(LowerView_Back::VIEWMODE_Finalizing);  /*  終了しています  */
  ExitFrame();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void Frame_ReceiveIr::DeliveryUtilRecvListener_OnWifiError( const RecvWifiErrorCode errorCode )
{
  GFL_PRINT("%s:\n", __FUNCTION__);
  GFL_ASSERT(false);    /*  irでは想定外  */
  NetError(LowerView_Back::VIEWMODE_Finalizing);  /*  終了しています  */
  ExitFrame();
}





//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_ReceiveIr::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
  m_State   = STATE_Quit_Request;
  m_pViewContainer->SetAllInputEnable(false);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
u32   Frame_ReceiveIr::ValueProvider_ReceiveProgress(GiftMessageMenu* pSender)
{
  return m_Progress;
}



} /*  namespace Gift    */
} /*  namespace NetApp  */
