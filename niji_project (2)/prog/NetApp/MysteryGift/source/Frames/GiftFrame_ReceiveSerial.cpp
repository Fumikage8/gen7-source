//==============================================================================
/**
 * @file   GiftFrame_ReceiveSerial.cpp
 * @date   2015/08/21 Fri. 13:12:07
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <Fade/include/gfl2_FadeManager.h>
#include  <NetStatic/NetAppLib/include/System/ResourceManager.h>
#include  <NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h>
#include  "../ApplicationSystem/GiftWork.h"
#include  "../GiftResource.h"

#include  "./GiftFrame_ReceiveSerial.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,

    /*  confirm  */
    STATE_FirstConfirm,
    STATE_InternetConfirm,

    /*  online  */
    STATE_StartHelperInit,
    STATE_HelperInit,
    STATE_StartInternetConnect,
    STATE_InternetConnect,
    STATE_TransitToEshopConfirm,
    STATE_StartFriendServerConnect,
    STATE_FriendServerConnect,
    STATE_SerialInputConfirm,

    /*  serial  */
    STATE_StartInputSerial,
    STATE_StartInputSerial_Fade,
    STATE_WaitInputSerial,
    STATE_WaitInputSerial_Fade,
    STATE_RetryInputConfirm,

    /*  PGL auth  */
    STATE_StartPGLAuth,
    STATE_PGLAuth,
    STATE_StartSaveAfterPGLAuth,
    STATE_SavingAfterPGLAuth,

    /*  receive  */
    STATE_StartReceive,
    STATE_Receiving,              /**<  データ受信中                  */
    STATE_Quit_Request,
    STATE_Quit_Prepare,
    STATE_Quit_ShowError,         /**<  エラー表示                    */

    /*    */
    STATE_Error,
    STATE_ServerError,
    STATE_Error_TimeoutPenalty,
    STATE_Error_InvalidCode,

    /*  updateData  */
    STATE_EshopTransitionConfirm,
    STATE_EshopNotAvailable,
    STATE_InternetNotAvailable,

    /*  finalize  */
    STATE_StartHelperFinalize,
    STATE_HelperFinalize,


    STATE_Exit,

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
  *  @func     Frame_ReceiveSerial
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_ReceiveSerial::Frame_ReceiveSerial(GiftWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)

  , m_Progress(0)
  , m_IsReceived(0)

  , m_IsError(false)                /**<  エラーが発生した  */
  , m_ErrorViewMode(LowerView_Back::VIEWMODE_Error_ReceiveFailed)
  , m_pUpperView_Message(NULL)
  , m_pLowerView_Back(NULL)
  /*  input  */
  , m_pInputROModule(NULL)
  , m_InputParam()
  , m_pInputFrame(NULL)
  , m_IsSerialAuthPassed(false)
  , m_IsSerialTimeoutPenalty(false)
  /*  helper  */
  , m_DelivHelper(this)
  , m_pAuthHelper(pWork->m_pAuthHelper)
  , m_SaveUtility()
{
  m_pUpperView_Message  = m_pViewContainer->m_pUpperView_Message;
  m_pLowerView_Back     = m_pViewContainer->m_pLowerView_Back;
}


//------------------------------------------------------------------
/**
  *  @func     Frame_ReceiveSerial
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_ReceiveSerial::~Frame_ReceiveSerial()
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
bool  Frame_ReceiveSerial::startup()
{
  m_pViewContainer->SetScene(GiftViewContainer::SCENE_Receive_Serial);
  m_pViewContainer->SetAllInputEnable(true);

  m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Serial_1stConfirm);   /*  ご注意  */
  /*  input  */
  {
    m_InputParam.callMode = App::NumberInput::CALL_MODE_ALPHABET;
    m_InputParam.callType = App::NumberInput::CALL_TYPE_WONDER_GIFT;
  }

  m_pWork->m_pAppParam->m_bReceiveOneData  = true;  /**< Serialは一件ずつしか受け取れない  */
  m_DelivHelper.SetSerialCode(0);

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
void  Frame_ReceiveSerial::setListener()
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
void  Frame_ReceiveSerial::removeListener()
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
void  Frame_ReceiveSerial::updateSequence()
{
  gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
  const gfl2::Fade::DISP    fadeDisp      = gfl2::Fade::DISP_DOUBLE;


  m_DelivHelper.Update();

  switch(m_State)
  {
    case  STATE_Entry:
      m_State = STATE_FirstConfirm;
      /*  fallthrough  */

    case  STATE_FirstConfirm:
      break;

    case  STATE_InternetConfirm:
      break;


    /*
      online
    */
    /*  helperinit  */
    case  STATE_StartHelperInit:
      m_pAuthHelper->BeginInitialize();
      m_State = STATE_HelperInit;
      /*  fall through  */
    case  STATE_HelperInit:
      if(m_pAuthHelper->ObserveInitialize())
      {
        m_State = STATE_StartInternetConnect;
      }
      break;

    /*  internet  */
    case  STATE_StartInternetConnect:
      m_pAuthHelper->BeginInternetConnect();
      m_State = STATE_InternetConnect;
      /*  fall through  */
    case  STATE_InternetConnect:
      ResultHandler(m_pAuthHelper->ObserveInternetConnect());
      break;

    /*  friendServer  */
    case  STATE_StartFriendServerConnect:
      m_pAuthHelper->BeginFirendServerConnect();
      m_State = STATE_FriendServerConnect;
      /*  fall through  */
    case  STATE_FriendServerConnect:
      ResultHandler(m_pAuthHelper->ObserveFirendServerConnect());
      break;


    case  STATE_SerialInputConfirm:
      break;



    /*
      input
    */
    case  STATE_StartInputSerial:
    {
      m_pViewContainer->SetScene(GiftViewContainer::SCENE_Void);

      /*  ro module  */
      {
        gfl2::ro::RoManager* pROManager = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );

        m_pInputROModule  = pROManager->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "NumberInput.cro");
        pROManager->StartModule( m_pInputROModule, true );
      }

      /*  clear  */
      memset(m_SerialString, 0, sizeof(m_SerialString));

      /*  fade  */
      pFadeManager->ForceDefaultOut(fadeDisp);
      m_State  = STATE_StartInputSerial_Fade;
    }
      break;

    case  STATE_StartInputSerial_Fade:
    {
      if(pFadeManager->IsEnd(fadeDisp))
      {
        /*  シリアル入力  */
        m_pInputFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, App::NumberInput::NumberInputFrame>( m_pWork->m_pAppFrameManager );
        m_pInputFrame->Setup( &m_InputParam, m_pWork->GetAppRenderingManager() );
        m_pInputFrame->SetGameManager( GFL_SINGLETON_INSTANCE( GameSys::GameManager ) );
        m_State  = STATE_WaitInputSerial;
      }
    }
      break;

    /*  シリアル入力からの戻り  */
    case  STATE_WaitInputSerial:
    {
      /*  ro module  */
      {
        gfl2::ro::RoManager* pROManager = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );

        pROManager->DisposeModule( m_pInputROModule );
        m_pInputROModule  = NULL;
      }
      m_pInputFrame = NULL;

      /*  fadein  */
      m_pViewContainer->SetScene(GiftViewContainer::SCENE_BGOnly);
      pFadeManager->RequestIn(fadeDisp, gfl2::Fade::FADE_TYPE_ALPHA);
      m_State  = STATE_WaitInputSerial_Fade;
    }
      break;

    case  STATE_WaitInputSerial_Fade:
    {
      /*
        NMcat[3918]   【ふしぎなおくりもの】あいことばシリアル入力時の挙動について
          「あいことば」は16文字に満たない状態で定義され、不足分は0パディングされたシリアルとして登録される
          例）あいことばが「POKEMON」の場合 -> 「POKE MON0 0000 0000」
          
          下山田さんからの要望で、
            a) 文字アプリからの戻りが0文字だった場合→キャンセルフローへ
            b) [1,15]文字だった場合→(16-n)文字を L'0' で埋め、サーバへ
              b') 認証失敗後の再入力はパディング前の文字列から
            c) 16文字だった場合→そのままサーバ認証へ
      */

      if(pFadeManager->IsEnd(fadeDisp))
      {
        bool  bLengthNotEnough  = false;    /**<  長さが足りず、認証に進めない→キャンセル扱い  */
        

        /*  一旦L'0'で埋めた上で、結果文字列で上書きしていく  */
        {
          /*  L'0' padding  */
          for(u32 index = 0; index < (GFL_NELEMS(m_SerialString)-1); ++index)
          {
            m_SerialString[index] = L'0';
          }
        }

        /*  concatenate */
        for(u32 partIndex = 0; (partIndex < App::NumberInput::PART_MAX) && (!bLengthNotEnough); ++partIndex)
        {
          const u32 bufIndexStart = partIndex * App::NumberInput::ALPHABET_INPUT_MAX;
          for(u32 ankIndex = 0; ankIndex < App::NumberInput::ALPHABET_INPUT_MAX; ++ankIndex)
          {
            const gfl2::str::STRCODE  inputCode  = m_InputParam.inputStr[partIndex][ankIndex];
            
            /*  NULをみつけたら上書きを停止、且つ先頭であればキャンセルフローへ  */
            if(inputCode == L'\0')
            {
              bLengthNotEnough  = ((!partIndex) && (!ankIndex));
              break;
            }
            m_SerialString[bufIndexStart + ankIndex] = inputCode;
          }
        }
        m_SerialString[GFL_NELEMS(m_SerialString)-1]  = L'\0';
        

        if(!ResultHandler_InputSerial())
        {
          /*  error  */
          m_pViewContainer->SetScene(GiftViewContainer::SCENE_Receive_Serial);
        }
        else  if(bLengthNotEnough)
        {
          /*  キャンセル扱い  */
          m_pViewContainer->SetScene(GiftViewContainer::SCENE_Receive_Serial);
          NetError(LowerView_Back::VIEWMODE_Finalizing); /*  見つかりませんでした  */
          m_pLowerView_Back->SetViewMode(m_ErrorViewMode);
          m_State = STATE_Quit_ShowError;
        }
        else
        {
          m_pViewContainer->SetScene(GiftViewContainer::SCENE_Receive_Serial_Auth);
          m_pUpperView_Message->SetViewMode(UpperView_Message::VIEWMODE_Serial);
          m_pUpperView_Message->SetSerialString(m_InputParam.inputStr[0], m_InputParam.inputStr[1], m_InputParam.inputStr[2], m_InputParam.inputStr[3]);
          m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Serial_Authentication); /*  シリアルコードを確認しています  */
          m_State = STATE_StartPGLAuth;
        }
      }
    }
      break;

    case  STATE_RetryInputConfirm:
      break;

    /*  PGL auth  */
    case  STATE_StartPGLAuth:
      GFL_PRINT("PGL serialcode authentication: %ls\n", m_SerialString);
      m_pAuthHelper->BeginPGLSerialAuth(m_SerialString);
      m_IsSerialAuthPassed  = false;
      m_State               = STATE_PGLAuth;
      /*  fall through  */
    case  STATE_PGLAuth:
      ResultHandler(m_pAuthHelper->ObservePGLSerialAuth());
      break;

    case  STATE_StartSaveAfterPGLAuth:
      m_SaveUtility.SaveStart(m_pWork);
      m_State = STATE_SavingAfterPGLAuth;
      /*  fall through  */

    case  STATE_SavingAfterPGLAuth:
      if(m_SaveUtility.SaveMain())
      {
        if(m_IsSerialAuthPassed)
        {
          /*  認証OK：受信開始  */
          m_State = STATE_StartReceive;
        }
        else
        {
          /*  認証NG：  */
          m_pLowerView_Back->ShowAuthError(AuthenticationHelper::PGL_SERIAL_AUTH_RESULT_Error_InvalidCode); /*  間違っています  */

          m_State = m_IsSerialTimeoutPenalty ? STATE_Error_TimeoutPenalty : STATE_Error_InvalidCode;
        }
      }
      break;



    /*  receive  */
    case  STATE_StartReceive:
      m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Searching); /**<  おくりものを探しています      */
      m_DelivHelper.Start(DeliveryUtil::MODE_BossViaPGLAuth, false);
      m_State = STATE_Receiving;
      break;

    case STATE_Receiving:              /**<  データ受信中                  */
      if(m_IsError || m_IsReceived)
      {
        m_State  = STATE_Quit_Request;
      }
      else if(m_DelivHelper.GetStatus() != DeliveryUtilRetryHelper::STATUS_Available)
      {
        NetError(LowerView_Back::VIEWMODE_Finalizing);  /*  終了しています  */
      }
      break;

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
            ExitFrame();
          }
          else
          {
            m_pLowerView_Back->SetViewMode(m_ErrorViewMode);
            m_State = STATE_Quit_ShowError;
          }
        }
        else
        {
          ExitFrame(RESULT_RECEIVE_SERIAL_Received);
        }
      }
      break;

    case  STATE_Quit_ShowError:
      break;


    case  STATE_Error:
      /*  nop  */
      break;


    case  STATE_ServerError:
      ExitFrame();
      break;


    case  STATE_Error_TimeoutPenalty:
      break;

    case  STATE_Error_InvalidCode:
      break;


    /*  update  */
    case  STATE_EshopTransitionConfirm:
      break;

    case  STATE_EshopNotAvailable:
      break;

    case  STATE_InternetNotAvailable:
      break;


    /*  受信時以外のexitFrameはここへ  */
    /*  finalize  */
    case  STATE_StartHelperFinalize:
      m_pAuthHelper->BeginFinalize();
      m_State = STATE_HelperFinalize;
      /*  fall through  */
    case  STATE_HelperFinalize:
      if(m_pAuthHelper->ObserveFinalize())
      {
        m_State = STATE_Exit;
      }
      break;



    case  STATE_Exit:
      /*
        workにFrameResultを設定することでSwitcherが呼ばれる
      */
      m_pWork->SetFrameResult( m_Result );

      /*  SelectDataへ遷移する際はframeをまたいでOnlineなので、ここではハンドリングしない  */
      if(m_Result == RESULT_RECEIVE_SERIAL_Received)
      {
        m_pWork->m_bNoHandlingNetworkError = true;
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
bool  Frame_ReceiveSerial::cleanup()
{
  return true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
/*  implement LowerView_Back::IEventHandler  */
void  Frame_ReceiveSerial::LowerView_Back_OnEvent(const EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Back::IEventHandler::EVENT_Exit:
      if(m_State == STATE_Receiving)
      {
        NetError(LowerView_Back::VIEWMODE_Finalizing);  /*  終了しています  */
      }
      else
      {
        ExitFrame();
      }
      break;


    case  LowerView_Back::IEventHandler::EVENT_MessageFinish:
      if(m_State  == STATE_SerialInputConfirm)
      {
        /*  シリアルコードを入力してください  */
        if(ResultHandler_InputSerial())
        {
          m_State = STATE_StartInputSerial;
        }
      }
      else if(m_State  == STATE_Error)
      {
        ExitFrame();
      }
      else if(m_State == STATE_Error_InvalidCode)
      {
        /*  もう一度？  */
        if(ResultHandler_InputSerial())
        {
          m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Serial_RetryConfirm);
          m_State = STATE_RetryInputConfirm;
        }
      }
      else if(m_State == STATE_Error_TimeoutPenalty)
      {
        ExitFrame();
      }
      else if(m_State == STATE_Quit_ShowError)
      {
        ExitFrame();
      }
      else if(m_State == STATE_EshopNotAvailable)
      {
        ExitFrame();
      }
      else if(m_State == STATE_InternetNotAvailable)
      {
        ExitFrame();
      }
      break;

    /*
      yes
    */
    case  LowerView_Back::IEventHandler::EVENT_ReceiveCancelConfirm_Yes:
    {
      /*  もう一度？:Yes  */
      ResultHandler_InputSerial();
    }
      break;

    case  LowerView_Back::IEventHandler::EVENT_1stConfirm_Yes:
    {
      m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Serial_InternetConfirm);   /*  インターネットに接続します Y/N  */
      m_State = STATE_InternetConfirm;
    }
      break;

    case  LowerView_Back::IEventHandler::EVENT_InternetConfirm_Yes:
    {
      m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Serial_Connecting);        /*  接続しています  */
      m_State = STATE_StartHelperInit;
    }
      break;

    case  LowerView_Back::IEventHandler::EVENT_TransitToEshopConfirm_Yes:
    {
      /*  eshopへの遷移が可能であるかを確認  */
      if(System::GflUse::IsEshopAvailable())
      {
        m_pWork->m_pAppParam->procResult  = APP_PARAM::PROCRESULT_Exit_Eshop;
        ExitFrame(RESULT_RECEIVE_SERIAL_TransitToEshopPatchPage);
      }
      else
      {
        m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Serial_EshopNotAvailable);        /*  eshopがありません  */
        m_State = STATE_EshopNotAvailable;
      }
    }
      break;


    /*
      no
    */
    case  LowerView_Back::IEventHandler::EVENT_ReceiveCancelConfirm_No:
      /*  もう一度？:No  */
      ResultHandler_InputSerial();
      break;

    case  LowerView_Back::IEventHandler::EVENT_1stConfirm_No:
    {
      ExitFrame();
    }
      break;

    case  LowerView_Back::IEventHandler::EVENT_InternetConfirm_No:
    {
      ExitFrame();
    }
      break;

    case  LowerView_Back::IEventHandler::EVENT_TransitToEshopConfirm_No:
    {
      /*  インターネットに接続できないため終了  */
      m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Serial_InternetNotAvailable);
      m_State = STATE_InternetNotAvailable;
    }
      break;


    /*
      retry
    */
    case  EVENT_SerialRetryConfirm_Yes:
      if(m_State  == STATE_RetryInputConfirm)
      {
        m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Serial_InputSerial);  /*  シリアルコードを入力してください  */
        m_State = STATE_SerialInputConfirm;
      }
      break;

    case  EVENT_SerialRetryConfirm_No:
      if(m_State  == STATE_RetryInputConfirm)
      {
        ExitFrame();    /*  終了  */
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
bool  Frame_ReceiveSerial::DeliveryUtilRecvListener_OnRecvComplete(const CompleteResult completeResult, const void* pData , size_t dataSize )
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

  return false;
}


void  Frame_ReceiveSerial::DeliveryUtilRecvListener_OnUpdateProgress( const u32 progress )
{
  GFL_PRINT("%s: %d%%\n", __FUNCTION__, progress);

  m_Progress  = progress;
  if(m_State == STATE_Receiving)
  {
    m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Searching); /**<  おくりものを探しています      */
  }
}


void  Frame_ReceiveSerial::DeliveryUtilRecvListener_OnNintendoZoneConnected()
{
  GFL_PRINT("%s:\n", __FUNCTION__);
  /*  nop  */
}


void  Frame_ReceiveSerial::DeliveryUtilRecvListener_OnNetworkError( const ResultInfo& rResultInfo )
{
  GFL_PRINT("%s:\n", __FUNCTION__);

  NetError(LowerView_Back::VIEWMODE_Error_ReceiveFailed); /**<  うけとりに失敗しました  */
}


void  Frame_ReceiveSerial::DeliveryUtilRecvListener_OnWifiError( const RecvWifiErrorCode errorCode )
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
void  Frame_ReceiveSerial::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
  if(frameResult == RESULT_RECEIVE_SERIAL_Received)
  {
    /*  受信成功時はAuthHelperを維持して終了  */
    m_pWork->m_IsReceiveViaSerialAuth = true;
    m_State = STATE_Exit;
  }
  else
  {
    /*  それ以外はFinalize  */
    m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Finalizing);
    m_State = STATE_StartHelperFinalize;
  }

  m_pViewContainer->SetAllInputEnable(false);
}


//------------------------------------------------------------------
/**
  *  @brief    インターネット接続
  */
//------------------------------------------------------------------
void  Frame_ReceiveSerial::ResultHandler(const AuthenticationHelper::InternetConnectResult helperResult)
{
#if PM_DEBUG
  if((helperResult != AuthenticationHelper::INTERNET_CONNECT_RESULT_Processing) && (helperResult != AuthenticationHelper::INTERNET_CONNECT_RESULT_Error_RequirePatch))
  {
    if(m_pWork->m_pGiftDebug->GetEshopTransitionCheckValue())
    {
      /*  パッチが必要  */
      ResultHandler(AuthenticationHelper::INTERNET_CONNECT_RESULT_Error_RequirePatch);
      return;
    }
  }
#endif

  switch(helperResult)
  {
    case AuthenticationHelper::INTERNET_CONNECT_RESULT_Processing:           /**<  処理中  */
      break;

    case AuthenticationHelper::INTERNET_CONNECT_RESULT_Success:              /**<  成功    */
    {
      if(GetMysteryGiftSave()->QueryTimeoutPenalty(m_pAuthHelper->GetServerDate()))
      {
        /*  ペナルティ中  */
        m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Error_TimeoutPenalty);
        m_State = STATE_Error;
      }
      else
      {
        m_State = STATE_StartFriendServerConnect;
      }
    }
      break;

    case AuthenticationHelper::INTERNET_CONNECT_RESULT_Error_RequirePatch:
      /*  パッチが必要  */
      m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Serial_UpdateFound);
      m_State = STATE_EshopTransitionConfirm;
      break;


    /*  error  */
    default:
      /*  appletによる報告時はMsgなしにFinishが通知されるため、先にStateを変える  */
      m_State = STATE_Error;
      m_pLowerView_Back->ShowAuthError(helperResult);
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_ReceiveSerial::ResultHandler(const AuthenticationHelper::FriendServerConnectResult helperResult)
{
  switch(helperResult)
  {
    case AuthenticationHelper::FRIENDSERVER_CONNECT_RESULT_Processing:              /**<  処理中  */
      break;
    case AuthenticationHelper::FRIENDSERVER_CONNECT_RESULT_Success:                 /**<  成功    */
      m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Serial_InputSerial);  /*  シリアルコードを入力してください  */
      m_State = STATE_SerialInputConfirm;
      break;

    /*  error  */
    default:
      /*  appletによる報告時はMsgなしにFinishが通知されるため、先にStateを変える  */
      m_State = STATE_Error;
      m_pLowerView_Back->ShowAuthError(helperResult);
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    シリアル入力周辺での通信エラーハンドリング
  *  @note     シリアル入力周辺では通信を行っていないので、Managerから直接取得する
  *  @retval   true:  異常なし
  *  @retval   false: error
  */
//------------------------------------------------------------------
bool  Frame_ReceiveSerial::ResultHandler_InputSerial(void)
{
  NetLib::Error::NijiNetworkErrorManager* pErrorManager = GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager);

  if( pErrorManager->IsError() )
  {
    m_State = STATE_Error;
    m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Error_ReceiveFailed);
  }


  return (m_State != STATE_Error);
}


//------------------------------------------------------------------
/**
  *  @brief    PGL Auth
  */
//------------------------------------------------------------------
void  Frame_ReceiveSerial::ResultHandler(const AuthenticationHelper::PGLSerialAuthResult helperResult)
{
  m_IsSerialAuthPassed      = false;
  m_IsSerialTimeoutPenalty  = false;

  switch(helperResult)
  {
    case AuthenticationHelper::PGL_SERIAL_AUTH_RESULT_Processing:
      break;
    case AuthenticationHelper::PGL_SERIAL_AUTH_RESULT_Success:
    {
      const u32 authNo  = m_pAuthHelper->GetDataNo();
      
      /*    */
      GFL_PRINT("PGL auth no: %u\n", authNo);

      m_DelivHelper.SetSerialCode(authNo);
      m_IsSerialAuthPassed      = true;
      m_IsSerialTimeoutPenalty  = GetMysteryGiftSave()->UpdateSerialAuthStatus(true, m_pAuthHelper->GetServerDate());
      m_State = STATE_StartSaveAfterPGLAuth;
    }
      break;


    case AuthenticationHelper::PGL_SERIAL_AUTH_RESULT_Error_ServerError:
      /*  nop (applet)  */
      m_State = STATE_ServerError;
      break;


    /*  error  */
    case AuthenticationHelper::PGL_SERIAL_AUTH_RESULT_Error_InvalidCode:
      m_IsSerialTimeoutPenalty  = GetMysteryGiftSave()->UpdateSerialAuthStatus(false, m_pAuthHelper->GetServerDate());
      m_State = STATE_StartSaveAfterPGLAuth;

      break;

    /*  その他のエラー  */
    default:
      /*  appletによる報告時はMsgなしにFinishが通知されるため、先にStateを変える  */
      m_State = STATE_Error;
      m_pLowerView_Back->ShowAuthError(helperResult);
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
/*  ここではCommitSaveしない  */
#if 0
void  Frame_ReceiveSerial::ResultHandler(const AuthenticationHelper::CommitSaveResult helperResult)
{
  switch(helperResult)
  {
    case AuthenticationHelper::COMMIT_SAVE_RESULT_Processing:
      break;
    case AuthenticationHelper::COMMIT_SAVE_RESULT_Success:
      m_State = STATE_StartHelperFinalize;
      break;

    /*  error  */
    default:
      m_pLowerView_Back->ShowAuthError(helperResult);
      m_State = STATE_Error;
      break;
  }
}
#endif



} /*  namespace Gift    */
} /*  namespace NetApp  */
