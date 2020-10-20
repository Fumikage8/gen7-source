//==============================================================================
/**
 * @file   LiveFrame_ReceivePlayerPass.cpp
 * @date   2015/07/24 Fri. 11:49:12
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <fs/include/gfl2_fs.h>

#include  "GameSys/include/GameData.h"

#include  "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include  "../ApplicationSystem/LiveWork.h"
#include  "../LiveResource.h"

#include  "./LiveFrame_ReceivePlayerPass.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,

    STATE_ConnectStart,
    STATE_Connecting,
    STATE_Receiving,
    STATE_DisconnectStart,
    STATE_Disconnecting,

    STATE_ReceiveResult,
    STATE_CheckRegulation,
    STATE_CheckResult,

    STATE_SaveStart,
    STATE_Saving,

    STATE_Exit,


    STATE_     /*  terminator  */
  };
} /*  namespace  */


namespace NetApp  {
namespace Live {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     Frame_ReceivePlayerPass
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_ReceivePlayerPass::Frame_ReceivePlayerPass(LiveWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_pUpperView_Message(NULL)
  , m_pLowerView_Selection(NULL)
  , m_ReceiveResult(RECVRESULT_Initial)

{
  m_pUpperView_Message    = m_pViewContainer->m_pUpperView_Message;
  m_pLowerView_Selection  = m_pViewContainer->m_pLowerView_Selection;
}


//------------------------------------------------------------------
/**
  *  @func     Frame_ReceivePlayerPass
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_ReceivePlayerPass::~Frame_ReceivePlayerPass()
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
bool  Frame_ReceivePlayerPass::startup()
{
  m_pViewContainer->SetScene(LiveViewContainer::SCENE_ReceivePlayerPass);
  m_pViewContainer->SetAllInputEnable(true);

  if(m_pWork->m_pAppParam->IsReturnedFromQRRead())
  {
    m_pLowerView_Selection->SetVisible(false);
  }

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
void  Frame_ReceivePlayerPass::setListener()
{
  m_pLowerView_Selection->SetEventHandler(this);
  m_pWork->m_pAppParam->m_pLiveNet->SetEventHandler(this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_ReceivePlayerPass::removeListener()
{
  m_pLowerView_Selection->SetEventHandler(NULL);
  m_pWork->m_pAppParam->m_pLiveNet->SetEventHandler(NULL);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_ReceivePlayerPass::updateSequence()
{
  APP_PARAM*  pAppParam = m_pWork->m_pAppParam;

  switch(m_State)
  {
    case  STATE_Entry:
      if(pAppParam->IsReturnedFromQRRead())
      {
        OnReceive_QRRegulation();
        pAppParam->ResetBootMode(); /*  リセットする  */
        break;
      }
      else
      {
        /*  通常は赤外線受信  */
        m_State = STATE_ConnectStart;
      }
      /*  fallthrough  */

    case  STATE_ConnectStart:
      m_ReceiveResult = RECVRESULT_Initial;
      m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_ReceivePlayerPass_ReadyIrPort);   /*  向かい合わせて  */
      pAppParam->m_pLiveNet->Connect();
      m_State = STATE_Connecting;
      /*  fallthrough  */

    case  STATE_Connecting:
    {
      if(pAppParam->m_pLiveNet->IsConnected())
      {
        m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_ReceivePlayerPass_Receiving);   /*  通信しています  */
        pAppParam->m_pLiveNet->ExchangeRegulation();  /*  送信データはダミー  */
        m_State = STATE_Receiving;
      }
    }
      break;

    case  STATE_Receiving:
      break;


    case  STATE_DisconnectStart:
      pAppParam->m_pLiveNet->Disconnect();
      m_State = STATE_Disconnecting;
      /*  fallthrough  */

    case  STATE_Disconnecting:
      if(pAppParam->m_pLiveNet->IsDisconnected())
      {
        if(m_ReceiveResult == RECVRESULT_Received)
        {
          m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_ReceivePlayerPass_Received);  /*  受信しました  */
          m_State = STATE_ReceiveResult;
        }
        else
        {
          m_State = STATE_CheckRegulation;
        }
      }
      break;

    case  STATE_ReceiveResult:
      break;

    /*  受信Regulation確認  */
    case  STATE_CheckRegulation:
    {
      if(m_ReceiveResult == RECVRESULT_Failed_Cancel)
      {
        /*  キャンセル  */
        m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_ReceivePlayerPass_ReceiveCancel);
        m_State = STATE_CheckResult;
      }
      else
      {
        /*  キャンセルせず受信完了  */
        RegulationCheckResultHandling();
      }
    }
      break;

    /*  Regulationチェック結果（失敗時）  */
    case  STATE_CheckResult:
      break;

    case  STATE_SaveStart:
      m_pLowerView_Selection->SetVisible(true);           /*  QRからの戻り用  */
      pAppParam->SetLiveEntryWork_RegulationReceived();   /*  受信済み状態に  */
      m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_ReceivePlayerPass_Saving);
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SaveStart(m_pWork->GetDeviceHeap());
      m_State = STATE_Saving;
      /*  fallthrough  */

    case  STATE_Saving:
      {
        const gfl2::fs::FsCtrBackupSystem::Result   result  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SaveMain();
        
        if(result == gfl2::fs::FsCtrBackupSystem::RESULT_OK)
        {
          ExitFrame(RESULT_RECEIVE_PLAYER_PASS_RegulationCheckPassed);
        }
        else if(result == gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE)
        {
          /*  nop  */
        }
        else
        {
          /*  nop  */
        }
      }
      break;


    case  STATE_Exit:
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
bool  Frame_ReceivePlayerPass::cleanup()
{
  return true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
/*  implement LowerView_Selection::IEventHandler  */
void  Frame_ReceivePlayerPass::LowerView_Selection_OnEvent(const EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Selection::IEventHandler::EVENT_MessageFinish:
      if(m_State == STATE_ReceiveResult)
      {
        if(m_ReceiveResult == RECVRESULT_Received)  m_State = STATE_CheckRegulation;
        else                                        ExitFrame();
      }
      else if(m_State == STATE_CheckResult)
      {
        ExitFrame();
      }
      break;

    case  LowerView_Selection::IEventHandler::EVENT_Exit:
      if(m_State == STATE_Connecting)
      {
        m_pViewContainer->SetAllInputEnable(false);
        m_ReceiveResult = RECVRESULT_Failed_Cancel;
        m_State         = STATE_DisconnectStart;
      }
      else
      {
        ExitFrame();
      }
      break;

    default:
      break;
  }

}
/*  endimplement LowerView_Selection::IEventHandler  */


//------------------------------------------------------------------
/**
  *  @brief    受信した
  */
//------------------------------------------------------------------
  /*  implement NetApp::Live::LiveNet::IEventHandler  */
void  Frame_ReceivePlayerPass::OnReceive_Regulation(LiveNet* pSender, const SerializedRegulation& rSerializedRegulation)
{
  if(m_State == STATE_Receiving)
  {
    m_pWork->m_pAppParam->SetReceivedRegulation(rSerializedRegulation);
    m_ReceiveResult = RECVRESULT_Received;
    m_State         = STATE_DisconnectStart;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_ReceivePlayerPass::OnReceive_QRRegulation(void)
{
  GFL_ASSERT(m_State == STATE_Entry);
  
  /*  ※QR時はm_pReceivedRegulationに直接入るのでDeserialize不要  */
  m_ReceiveResult = RECVRESULT_Received;
  m_State         = STATE_CheckRegulation;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_ReceivePlayerPass::ExitFrame(const FrameResult frameResult)
{
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
bool  Frame_ReceivePlayerPass::CheckForcedOverwriteRegulation(void)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  return (true
    && pButton->IsHold(gfl2::ui::BUTTON_DOWN)
    && pButton->IsHold(gfl2::ui::BUTTON_A)
  );
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_ReceivePlayerPass::RegulationCheckResultHandling(void)
{
  switch(m_pWork->m_pAppParam->CheckRegulation())
  {
    case  APP_PARAM::REGULATION_CHECK_RESULT_OK:
      /*  OK  */
      m_ReceiveResult = RECVRESULT_RegulationPassed;
      m_State = STATE_SaveStart;        /*  セーブ開始  */
      break;

    case  APP_PARAM::REGULATION_CHECK_RESULT_Error_SameID:
    {
      /*  強制受信  */
      /*
        GFNMcat[5481]   【ライブ大会】選手証を強制上書きする際、データの妥当性チェックが適切に行われていない
        破損データ等の取り込みを回避するため、SameID時のみ強制上書きを有効とする
      */
      if(CheckForcedOverwriteRegulation())
      {
        /*  OK  */
        GFL_PRINT("regulation forced overwrite\n");
        m_ReceiveResult = RECVRESULT_RegulationPassed;
        m_State = STATE_SaveStart;        /*  セーブ開始  */
        return;
      }
      else
      {
        /*  既に参加しています  */
        m_pLowerView_Selection->SetVisible(true);           /*  QRからの戻り用  */
        m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_ReceivePlayerPass_HaveParticipated);
        m_State = STATE_CheckResult;
      }
    }
      break;

    case  APP_PARAM::REGULATION_CHECK_RESULT_Error_CRCMismatch:
    case  APP_PARAM::REGULATION_CHECK_RESULT_Error_ROMVersion:
    case  APP_PARAM::REGULATION_CHECK_RESULT_Error_LanguageID:
    default:
      /*  この大会には参加できません  */
      m_pLowerView_Selection->SetVisible(true);           /*  QRからの戻り用  */
      m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_ReceivePlayerPass_Rejected);
      m_State = STATE_CheckResult;
      break;
  }
}


} /*  namespace Live    */
} /*  namespace NetApp  */
