
//==============================================================================
/**
 * @file   GiftFrame_Receive.cpp
 * @date   2015/07/31 Fri. 16:26:50
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

#include  "./GiftFrame_Receive.h"


// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,


    STATE_Information_Full,             /**<  いっぱいで受け取れない表示    */
    STATE_Information_NearlyFull,       /**<  あとn件まで受け取れます       */

    STATE_WaitUsersChoice,              /**<  選択待ち        */
    STATE_SearchingViaUDS,              /**<  さがしています  */

    STATE_CancelConfirm,

    STATE_Quit_Request,
    STATE_Quit_Prepare,                 /**<  deliveryManagerのprepare待ち  */
    STATE_Quit_ShowError,               /**<  エラー表示                    */

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
  *  @func     Frame_Receive
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_Receive::Frame_Receive(GiftWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_DelivHelper(this)
  , m_Progress(0)
  , m_IsError(false)
  , m_ErrorViewMode(LowerView_Back::VIEWMODE_Error_ReceiveFailed)
  , m_IsReceivedDataViaUDS(false)
  , m_WirelessSwitchState(false)
  , m_pUpperView(NULL)
  , m_pLowerView(NULL)
  , m_pLowerView_Back(NULL)
{
  m_pUpperView        = m_pViewContainer->m_pUpperView_Message;
  m_pLowerView        = m_pViewContainer->m_pLowerView_Selection;
  m_pLowerView_Back   = m_pViewContainer->m_pLowerView_Back;
}


//------------------------------------------------------------------
/**
  *  @func     Frame_Receive
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_Receive::~Frame_Receive()
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
bool  Frame_Receive::startup()
{
  {
    m_pViewContainer->SetScene(GiftViewContainer::SCENE_Receive_Selection);
    m_pViewContainer->SetAllInputEnable(true);

    m_pUpperView->SetViewMode(UpperView_Message::VIEWMODE_Receive_ReceiveMethod);
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Receive_ReceivingMethod);
    m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_None);

    m_pLowerView->ClearDeliveryEventInformation();
    m_pLowerView->HideDeliveryEventInformation();

    m_State = STATE_WaitUsersChoice;
  }

  m_pWork->m_pAppParam->m_bReceiveOneData  = false;

  /*    */
  m_DelivHelper.Start(DeliveryUtil::MODE_UDS, true);
  m_Progress  = 0;

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
void  Frame_Receive::setListener()
{
  m_pLowerView->SetEventHandler(this);
  m_pLowerView->HideDeliveryEventInformation();
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
void  Frame_Receive::removeListener()
{
  m_pLowerView->SetEventHandler(NULL);
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
void  Frame_Receive::updateSequence()
{
  m_WirelessSwitchState = DeliveryUtil::GetInstance()->GetWirelessSwitchState();
  m_DelivHelper.Update();

  switch(m_State)
  {
    case  STATE_Entry:
      /*  fallthrough  */


    case  STATE_Information_Full:             /**<  いっぱいで受け取れない表示    */
    case  STATE_Information_NearlyFull:       /**<  あとn件まで受け取れます       */
    case  STATE_WaitUsersChoice:              /**<  選択待ち        */
      break;


    case  STATE_SearchingViaUDS:              /**<  さがしています  */
    {
      if(!m_WirelessSwitchState)
      {
        /*  無線OFF  */
        NetError(LowerView_Back::VIEWMODE_Error_WirelessOff);
      }

      if(m_IsError)
      {
        ExitFrame();
      }
      else if(m_IsReceivedDataViaUDS)
      {
        /*  先行受信済みなら終了  */
        ExitFrame(RESULT_Receive_UDS);
      }
    }
      break;

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

    case  STATE_Quit_Prepare:                 /**<  deliveryManagerのprepare待ち  */
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

    case  STATE_Quit_ShowError:               /**<  エラー表示                    */
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
bool  Frame_Receive::cleanup()
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
/*  implement LowerView_Selection::IEventHandler  */
void  Frame_Receive::LowerView_Selection_OnEvent(const LowerView_Selection::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    /*  終了  */
    case  LowerView_Selection::IEventHandler::EVENT_Exit:
      ExitFrame();
      break;

    /*  赤外線  */
    case  LowerView_Selection::IEventHandler::EVENT_Receive_Selected_Ir:
      ExitFrame(RESULT_Receive_Ir);
      break;

    /*  ローカル通信  */
    case  LowerView_Selection::IEventHandler::EVENT_Receive_Selected_UDS:
      if(m_IsReceivedDataViaUDS)
      {
        /*  先行受信済みなら終了  */
        ExitFrame(RESULT_Receive_UDS);
      }
      else
      {
        if(m_WirelessSwitchState)
        {
          /*  未受信なら待ちへ  */
          m_pViewContainer->SetScene(GiftViewContainer::SCENE_Receive_SearchingViaUDS);
          m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Searching);
          m_State = STATE_SearchingViaUDS;
        }
        else
        {
          m_pLowerView->ShowMessage_Error_WirelessOff();   /**<  無線OFFです  */
        }
      }
      break;

    /*  インターネット(BOSS)  */
    case  LowerView_Selection::IEventHandler::EVENT_Receive_Selected_Internet:
      if(m_WirelessSwitchState) ExitFrame(RESULT_Receive_Internet);
      else                      m_pLowerView->ShowMessage_Error_WirelessOff();   /**<  無線OFFです  */

      break;

    /*  シリアルコード  */
    case  LowerView_Selection::IEventHandler::EVENT_Receive_Selected_Serial:
      if(m_WirelessSwitchState) ExitFrame(RESULT_Receive_Serial);
      else                      m_pLowerView->ShowMessage_Error_WirelessOff();   /**<  無線OFFです  */
      break;

    /*  合言葉  */
    case  LowerView_Selection::IEventHandler::EVENT_Receive_Selected_Password:
      /*  仕様削除  */
      GFL_ASSERT(false);
//      ExitFrame(RESULT_Receive_Password);
      break;


    default:
      GFL_PRINT("%s:[warning]:exitCode unknwon (%d)\n", __FUNCTION__, static_cast<u32>(eventCode));
      break;
  }
}
/*  endimplement LowerView_Selection::IEventHandler  */


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
/*  implement LowerView_Back::IEventHandler I/F */
void  Frame_Receive::LowerView_Back_OnEvent(const LowerView_Back::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Back::IEventHandler::EVENT_Exit:
    {
#if 0
      if(m_State == STATE_SearchingViaUDS)
      {
        /*  「さがしています」なら中断確認  */
        m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_ReceiveCancelConfirm);
        m_State = STATE_CancelConfirm;
      }
#else
      /*  confirm不要とのこと  */
      NetError(LowerView_Back::VIEWMODE_Finalizing);  /*  終了しています  */
      ExitFrame();
#endif
    }
      break;

    /*  confirm:yes  */
    case  LowerView_Back::IEventHandler::EVENT_ReceiveCancelConfirm_Yes:
      if(m_State == STATE_CancelConfirm)
      {
        NetError(LowerView_Back::VIEWMODE_Error_NotFound); /*  見つかりませんでした  */
        ExitFrame();
      }
      break;
    
    /*  confirm:no  */
    case  LowerView_Back::IEventHandler::EVENT_ReceiveCancelConfirm_No:
      if(m_State == STATE_CancelConfirm)
      {
        /*  「さがしています（うけとるのをやますか）」なら受信継続  */
        m_pViewContainer->SetScene(GiftViewContainer::SCENE_Receive_SearchingViaUDS);
        m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Searching);
        m_State = STATE_SearchingViaUDS;
      }
      break;

    case  LowerView_Back::IEventHandler::EVENT_MessageFinish:
      if(m_State == STATE_Quit_ShowError)
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
/*  implement NetAppLib::UI::NetAppMessageMenu::IEventHandler I/F */
void  Frame_Receive::MessageMenuView_OnEvent(const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    /*
      Y/N:Yes
    */
    case  NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnSelectedYes:
      break;

    /*
      Y/N:No
    */
    case  NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnSelectedNo:
      break;



    /*
    */
//    case  NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnFinish:
    case  NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnEOM:
    {
      switch(m_State)
      {
        case  STATE_Information_Full:        /**<  いっぱいで受け取れない表示    */
          ExitFrame();
          break;


        case  STATE_Information_NearlyFull:  /**<  あとn件まで受け取れます       */
          m_pViewContainer->SetScene(GiftViewContainer::SCENE_Receive_Selection);
          m_pUpperView->SetViewMode(UpperView_Message::VIEWMODE_Receive_ReceiveMethod);
          m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Receive_ReceivingMethod);
          m_State = STATE_WaitUsersChoice;
          break;


        case  STATE_Quit_ShowError:  /**<  エラー表示       */
          m_State = STATE_Update_Exit;
          break;
      }
    }
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
/*
    implement DeliveryUtil::IRecvListener I/F
*/
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool Frame_Receive::DeliveryUtilRecvListener_OnRecvComplete(const CompleteResult completeResult,  const void* pData, size_t dataSize )
{
  GFL_PRINT("%s:\n", __FUNCTION__);
  m_Progress  = 100;


  switch(completeResult)
  {
    case  COMPLETE_RESULT_OK:
      if(m_pWork->m_pAppParam->SetReceivedDeliveryData(pData, dataSize))
      {
        m_IsReceivedDataViaUDS = true;
        if(m_State == STATE_WaitUsersChoice)
        {
          /*  受信方法選択待ち中なら速報表示  */
          {
            const u32 receivedDataNum = m_pWork->m_pAppParam->m_ReceiveDataInfo.dataNum;
            m_pLowerView->ClearDeliveryEventInformation();
            for(u32 index = 0; index < receivedDataNum; ++index)
            {
              m_pLowerView->SetDeliveryEventInformation(index, m_pWork->m_pAppParam->GetDeliveryEventName(index));
            }
            m_pLowerView->ShowDeliveryEventInformation();
          }
        }
        return false;   /*  確定  */

        /*  それ以外はエラーハンドリングと交錯する可能性があるので、遷移なしとして扱う  */
      }
      else
      {
        /*  失敗扱い  */
        DeliveryUtilRecvListener_OnRecvComplete(COMPLETE_RESULT_ReceiveFailed);
      }
      break;

    
    case  COMPLETE_RESULT_NotFound:               /**<  見つからなかった  */
      if(m_State == STATE_SearchingViaUDS)
      {
        NetError(LowerView_Back::VIEWMODE_Error_NotFound);                /**<  見つからなかった          */
      }
      else
      {
        /*  reinit  */
        GFL_PRINT("  reinit\n");
        m_DelivHelper.RequestFinalize(true);
      }
      break;

    case  COMPLETE_RESULT_FoundButCannotReceive:  /**<  見つけたが受け取れない  */
      if(m_State == STATE_SearchingViaUDS)
      {
        NetError(LowerView_Back::VIEWMODE_Error_FoundButCannotReceive);   /**<  みつけたけどうけとれない    */
      }
      else
      {
        /*  reinit  */
        GFL_PRINT("  reinit\n");
        m_DelivHelper.RequestFinalize(true);
      }
      break;

    case  COMPLETE_RESULT_ReceiveFailed:          /**<  受け取りに失敗          */
    default:
      if(m_State == STATE_SearchingViaUDS)
      {
        NetError(LowerView_Back::VIEWMODE_Error_ReceiveFailed);           /**<  うけとりに失敗しました  */
      }
      else
      {
        /*  reinit  */
        GFL_PRINT("  reinit\n");
        m_DelivHelper.RequestFinalize(true);
      }
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
void Frame_Receive::DeliveryUtilRecvListener_OnUpdateProgress( const u32 progress )
{
  GFL_PRINT("%s: %d%%\n", __FUNCTION__, progress);

  m_Progress  = progress;
  if(m_State != STATE_CancelConfirm)
  {
    m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Searching);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void Frame_Receive::DeliveryUtilRecvListener_OnNintendoZoneConnected()
{
  GFL_PRINT("%s:\n", __FUNCTION__);
  /*  nop  */
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void Frame_Receive::DeliveryUtilRecvListener_OnNetworkError( const ResultInfo& rResultInfo )
{
  GFL_PRINT("%s:\n", __FUNCTION__);

  if(m_State == STATE_SearchingViaUDS)
  {
    /*  さがしています中ならエラーステートへ  */
    NetError(LowerView_Back::VIEWMODE_Error_ReceiveFailed); /**<  うけとりに失敗しました  */
  }
  else
  {
    /*  そうでなければ自動リトライ  */
    GFL_PRINT("  reinit\n");
    m_DelivHelper.RequestFinalize(true);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void Frame_Receive::DeliveryUtilRecvListener_OnWifiError( const RecvWifiErrorCode errorCode )
{
  GFL_PRINT("%s:\n", __FUNCTION__);

  /*  nop  */
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_Receive::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
  m_State   = STATE_Quit_Request;
  m_pViewContainer->SetAllInputEnable(false);
}


//------------------------------------------------------------------
/**
  *  @brief    あとn件
  */
//------------------------------------------------------------------
u32   Frame_Receive::ValueProvider_AmountSpace(GiftMessageMenu* pSender)
{
  return 0;
}


//------------------------------------------------------------------
/**
  *  @brief    受信Progress
  */
//------------------------------------------------------------------
u32   Frame_Receive::ValueProvider_ReceiveProgress(GiftMessageMenu* pSender)
{
  return m_Progress;
}



} /*  namespace Gift    */
} /*  namespace NetApp  */
