
//==============================================================================
/**
 * @file   GiftFrame_SelectData.cpp
 * @date   2015/08/05 Wed. 16:40:42
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
#include  "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include  "../ApplicationSystem/GiftWork.h"
#include  "../GiftResource.h"

#include  "./GiftFrame_SelectData.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,

    STATE_Main,

    STATE_Info_ReceiveOnceADay,

    STATE_StartSave,
    STATE_Saving,


    STATE_StartCommitSave,
    STATE_CommitSaving,
    STATE_StartAuthHelperFinalize,
    STATE_AuthHelperFinalize,
    STATE_CommitSaveError,

    /*  error  */
    STATE_ShowError,

    /*  exitflow  */
    STATE_ExitConfirm,            /**<  うけとるのをやめますか  */
    STATE_ExitConfirm_Remaining,  /**<  まだあるけどやめますか  */


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
  *  @func     Frame_SelectData
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_SelectData::Frame_SelectData(GiftWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_IsCommitSaveStarted(false)
  , m_CommitSaveResult(AuthenticationHelper::COMMIT_SAVE_RESULT_Processing)
  , m_pUpperView_Message(NULL)
  , m_pLowerView_SelectData(NULL)
{

  m_pUpperView_Message    = m_pViewContainer->m_pUpperView_Message;
  m_pLowerView_SelectData = m_pViewContainer->m_pLowerView_SelectData;
}


//------------------------------------------------------------------
/**
  *  @func     Frame_SelectData
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_SelectData::~Frame_SelectData()
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
bool  Frame_SelectData::startup()
{
  m_pViewContainer->SetScene(GiftViewContainer::SCENE_SelectData);

  m_pUpperView_Message->SetViewMode(UpperView_Message::VIEWMODE_SelectData_ReceivedData);
//    m_pLowerView_SelectData->SetViewMode(LowerView_Selection::VIEWMODE_MainMenu_ChooseFunction);

  /*  listのrefreshにEventHandlerが必要  */
  m_pLowerView_SelectData->SetEventHandler(this);
  m_pLowerView_SelectData->HideMessage();
  RefreshList();
  m_pLowerView_SelectData->Update();    /*  listの更新  */

  /*    */
  m_pWork->m_pAppParam->procResult    = APP_PARAM::PROCRESULT_Exit;
  m_pWork->m_bNoHandlingNetworkError  = false;

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
void  Frame_SelectData::setListener()
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
void  Frame_SelectData::removeListener()
{
  m_pLowerView_SelectData->SetEventHandler(NULL);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_SelectData::updateSequence()
{
  switch(m_State)
  {
    case  STATE_Entry:
      m_State = STATE_Main;
      /*  fallthrough  */

    case  STATE_Main:
      m_pViewContainer->SetAllInputEnable(true);
      /*  シリアル受信時はここでエラーを監視する必要がある   */
      if(m_pWork->m_IsReceiveViaSerialAuth)
      {
        if(GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsError())
        {
          m_pViewContainer->SetAllInputEnable(false);
          OnExit();
        }
      }
      break;

    case  STATE_Info_ReceiveOnceADay:
      break;



    /*
      commitsave
    */
    case  STATE_StartCommitSave:
      m_pLowerView_SelectData->ShowMessage_Saving();
      m_pWork->m_pAuthHelper->BeginCommitSave();
      m_IsCommitSaveStarted = true;
      m_State = STATE_CommitSaving;
      /*  fall through  */

    case  STATE_CommitSaving:
      AuthHelperResultHandling(m_pWork->m_pAuthHelper->ObserveCommitSave());
      break;

    case  STATE_StartAuthHelperFinalize:
      m_pWork->m_pAuthHelper->BeginFinalize();
      m_State = STATE_AuthHelperFinalize;
      /*  fall through  */

    case  STATE_AuthHelperFinalize:
      if(m_pWork->m_pAuthHelper->ObserveFinalize())
      {
        /*  commitSaveが開始済みであれば結果を参照、そうでなければキャンセルなのでそのままExit  */
        if(m_IsCommitSaveStarted)
        {
          if(m_CommitSaveResult == AuthenticationHelper::COMMIT_SAVE_RESULT_Success)
          {
            /*  complete  */
            m_pLowerView_SelectData->HideMessage();
            ExitFrame(RESULT_SELECTDATA_SelectedItem);
          }
          else
          {
            m_State = STATE_CommitSaveError;    /*  appletによる報告時はMsgなしにFinishが通知されるため、先にStateを変える  */
            m_pLowerView_SelectData->ShowMessage_CommitSaveError(m_CommitSaveResult);
          }
        }
        else
        {
          ExitFrame();
        }
      }
      break;

    case  STATE_CommitSaveError:
      break;


    case STATE_StartSave:
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SaveStart(m_pWork->GetDeviceHeap());
      m_pLowerView_SelectData->ShowMessage_Saving();
      m_State = STATE_Saving;
      /*  fall through  */
    
    case STATE_Saving:
      {
        const gfl2::fs::FsCtrBackupSystem::Result   result  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SaveMain();
        
        if(result == gfl2::fs::FsCtrBackupSystem::RESULT_OK)
        {
          /*  complete  */
          m_pLowerView_SelectData->HideMessage();
          ExitFrame(RESULT_SELECTDATA_SelectedItem);
        }
        else if(result == gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE)
        {
          /*  nop  */
        }
        else
        {
          /**   fatal    */
          GFL_ASSERT(false);
        }
      }
      break;

    case  STATE_ShowError:
      break;

    case  STATE_Exit:
      /*
        workにFrameResultを設定することでSwitcherが呼ばれる
      */
//      m_pLowerView_SelectData->SetInputEnabled(true); /*  次のFrameの先頭で解除される  */
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
bool  Frame_SelectData::cleanup()
{
  return true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    リスト内容の更新
  */
//------------------------------------------------------------------
void  Frame_SelectData::RefreshList(void)
{
//  m_pUpperView_Card->UpdateCardInfo(0);
  m_pLowerView_SelectData->SetupList(m_pWork->m_pAppParam->m_ReceiveDataInfo.dataNum);
  m_pUpperView_Message->UpdateReceivedDataInfo(m_pWork->m_pAppParam->GetReceivedDeliveryData(0));
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_SelectData::OnExit(void)
{
  /*  シリアル認証経由であればAuthHelperを終了してから  */
  if(m_pWork->m_IsReceiveViaSerialAuth)
  {
    m_pLowerView_SelectData->HideMessage();
    m_pLowerView_SelectData->ShowMessage_Finalizing();
    m_State = STATE_StartAuthHelperFinalize;
  }
  else
  {
    ExitFrame();
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
/*  implement LowerView_SelectData::IEventHandler  */
void  Frame_SelectData::LowerView_SelectData_OnEvent(const LowerView_SelectData::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_SelectData::IEventHandler::EVENT_Exit:
      OnExit();
      break;

    case  LowerView_SelectData::IEventHandler::EVENT_MessageFinish:
      if(m_State  == STATE_Info_ReceiveOnceADay)
      {
        OnStartSave();
      }
      else if(m_State  == STATE_CommitSaveError)
      {
        ExitFrame();
      }
      break;


    case  LowerView_SelectData::IEventHandler::EVENT_FocusItemChange:    /**<  選択中の項目が変更された  */
    {
      const u32 selectedIndex = m_pLowerView_SelectData->SelectedIndex();

      m_pUpperView_Message->UpdateReceivedDataInfo(m_pWork->m_pAppParam->GetReceivedDeliveryData(selectedIndex));
    }
      break;

    case  LowerView_SelectData::IEventHandler::EVENT_SelectedItem:       /**<  項目が決定された          */
    {
      const u32 selectedIndex = m_pLowerView_SelectData->SelectedIndex();

      m_pUpperView_Message->UpdateReceivedDataInfo(m_pWork->m_pAppParam->GetReceivedDeliveryData(selectedIndex));
      /*  Save可能チェックによる分岐  */
      ReceiveCheck();
    }
      break;

    case  LowerView_SelectData::IEventHandler::EVENT_OnCheckRemains:     /**<  受信可能なデータがまだあるか  */
    {
      m_pLowerView_SelectData->SetRemains(m_pWork->m_pAppParam->GetRemainingDataNum());
    }
      break;

    /*  アルバム整理要求  */
    case  LowerView_SelectData::IEventHandler::EVENT_AlbumCleanup:
      m_pWork->m_bCleanupMode = true;
      ExitFrame(RESULT_SELECTDATA_AlbumCleanup);
      break;


    default:
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
void  Frame_SelectData::ExitFrame(const FrameResult frameResult)
{
  if(frameResult == RESULT_SELECTDATA_SelectedItem)
  {
    m_pWork->m_pAppParam->procResult  = APP_PARAM::PROCRESULT_ReceiveDemo;
  }

  m_Result  = frameResult;
  m_State   = STATE_Exit;
  m_pViewContainer->SetAllInputEnable(false);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_SelectData::OnStartSave(void)
{
  const Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA*  pReceivedData = m_pWork->m_pAppParam->GetSelectedDeliveryData();


   /*  serialCode経由の受信の場合はCommitSave準備  */
  if(m_pWork->m_IsReceiveViaSerialAuth)
  {
    m_pWork->m_pAuthHelper->ReadyCommitSave();
  }

  /*  savedataへ反映  */
  GetMysteryGiftSave()->PushBackGiftData(pReceivedData);
  m_pWork->m_pAppParam->RemoveSelectedDeliveryData();      /*  ここで受け取りデータをリストから削除  */
  m_pLowerView_SelectData->SetInputEnabled(false);

  
  if(m_pWork->m_IsReceiveViaSerialAuth)
  {
    /*  serialCode経由の受信の場合はCommitSave  */
    m_State = STATE_StartCommitSave;
  }
  else
  {
    m_State = STATE_StartSave;
  }
}




//------------------------------------------------------------------
/**
  *  @brief    受け取りチェック
  */
//------------------------------------------------------------------
void  Frame_SelectData::ReceiveCheck(void)
{
  const u32 selectedIndex = m_pLowerView_SelectData->SelectedIndex();

  m_pWork->m_pAppParam->m_ReceiveDataInfo.selectedIndex = selectedIndex;

  const Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA*  pReceivedData = m_pWork->m_pAppParam->GetSelectedDeliveryData();
  const Savedata::MysteryGiftSave::ReceiveCheckResult       checkResult  = Savedata::MysteryGiftSave::ReceiveCheck(pReceivedData);

  switch(checkResult)
  {
    case  Savedata::MysteryGiftSave::RECEIVE_CHECK_RESULT_OK:
    {
      if(pReceivedData->GetGiftData().IsReceiveOnceADay())
      {
        /*  一日一度しか受け取れません表示をしてから受信処理  */
        m_pLowerView_SelectData->ShowMessage_OnceAtDayInfo();
        m_State = STATE_Info_ReceiveOnceADay;
      }
      else
      {
        /*  それ以外はセーブ開始  */
        OnStartSave();
      }
    }
      break;

    case  Savedata::MysteryGiftSave::RECEIVE_CHECK_RESULT_NG_TODAY:   /**<  NG   一日1回          */
      m_pLowerView_SelectData->ShowMessage_OnceAtDay();
      break;

    case  Savedata::MysteryGiftSave::RECEIVE_CHECK_RESULT_NG_FULL:    /**<  NG カードアルバムいっぱい */
    {
      Savedata::MysteryGiftSave*   pGiftSave  = GetMysteryGiftSave();
      
      
      if(pGiftSave->GetGiftDataVacanciesWithDeletables())
      {
        /*  削除可能なものがある  */
        m_pLowerView_SelectData->ShowMessage_AlbumCleanupConfirm();
      }
      else
      {
        /*  削除不可  */
        m_pLowerView_SelectData->ShowMessage_AlbumFull();
      }
    }
      break;


    case  Savedata::MysteryGiftSave::REDEIVE_CHECK_RESULT_NG_TODAY_FULL:  /**<  NG   一日1回の履歴が一杯*/ 
      m_pLowerView_SelectData->ShowMessage_OnceAtDay_HistoryFull();
      break;


    case  Savedata::MysteryGiftSave::RECEIVE_CHECK_RESULT_NG:         /**<  NG  受け取り済み      */
    default:
      m_pLowerView_SelectData->ShowMessage_AlreadyReceived();
      break;
  }
}



//------------------------------------------------------------------
/**
  *  @brief    認証ヘルパ結果
  */
//------------------------------------------------------------------
void  Frame_SelectData::AuthHelperResultHandling(AuthenticationHelper::CommitSaveResult helperResult)
{
  m_CommitSaveResult  = helperResult;
  
  switch(helperResult)
  {
    case  AuthenticationHelper::COMMIT_SAVE_RESULT_Processing:
      /*  nop */
      break;

    case  AuthenticationHelper::COMMIT_SAVE_RESULT_Success:
    case  AuthenticationHelper::COMMIT_SAVE_RESULT_Error_InvalidCode:         /**<  エラー：認証に失敗  */
    case  AuthenticationHelper::COMMIT_SAVE_RESULT_Error_AlreadyDownloaded:
    default:
      m_State = STATE_StartAuthHelperFinalize;
      break;
  }
}


} /*  namespace Gift    */
} /*  namespace NetApp  */
