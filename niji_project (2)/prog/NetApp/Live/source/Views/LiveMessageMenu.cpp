
//==============================================================================
/**
 * @file   LiveMessageMenu.cpp
 * @date   2015/07/13 Mon. 19:02:54
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../ApplicationSystem/LiveWork.h"

#include  "../LiveResource.h"
#include  "./LiveMessageMenu.h"


// =============================================================================
/**/
//==============================================================================
namespace {
  static const u32  MESSAGE_GARC_ID = GARC_message_live_tournament_DAT;
}




namespace NetApp  {
namespace Live {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  */
//==============================================================================
class LiveMessageMenu::LiveCommonMessageWindowView
  : public  App::Tool::CommonMessageWindowView
{
  GFL_FORBID_COPY_AND_ASSIGN(LiveCommonMessageWindowView);
public:
  LiveCommonMessageWindowView(const App::Tool::CommonMessageWindowView::PARAM& rParam, App::Tool::CommonMessageWindowView::IEventHandler* pHandler)
    : App::Tool::CommonMessageWindowView(rParam, pHandler)
    , m_pTargetView(NULL)
  {
  }
  virtual ~LiveCommonMessageWindowView(){}

  /*  本クラスでは描画しない(Create2Dしていないのでできない)  */
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo)
  {}

  //------------------------------------------------------------------
  /**
    *  @brief    Liveではウインドウが部品埋め込みなので、Create2Dは行わず描画は親に任せる
    */
  //------------------------------------------------------------------
  virtual void  Setup(void* pLayoutBuffer)
  {
    App::Tool::CommonMessageWindowManipulator::SSetupInfo setupInfo;

    setupInfo.pHeap               = m_Param.pHeap;
    setupInfo.pTargetView         = m_pTargetView;
    setupInfo.layoutWorkID        = 0;
    setupInfo.pHandler            = this;
    /*
      
    */
    setupInfo.animID_TimerIcon      = LA_LIVE_LIVE_SLC_LOW_000__MSG_L2__TIMERICON_KEEP;
    setupInfo.animID_TimerIcon_In   = LA_LIVE_LIVE_SLC_LOW_000__MSG_L2__TIMERICON_IN;
    setupInfo.animID_TimerIcon_Out  = LA_LIVE_LIVE_SLC_LOW_000__MSG_L2__TIMERICON_OUT;
    setupInfo.animID_MsgCursor      = LA_LIVE_LIVE_SLC_LOW_000__MSG_L2__CURSOR_00_KEEP;
    setupInfo.paneID_WindowParts    = PANENAME_LIVE_SLC_LOW_000_PANE_MSG_L2;
    setupInfo.paneID_TextBox2       = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2;
    setupInfo.paneID_NullText1      = PANENAME_COMMON_MSG_LOW_000_PANE_NULL_TEXT1;
    setupInfo.paneID_TextBox1a      = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1A;
    setupInfo.paneID_TextBox2a      = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1B;
    setupInfo.paneID_TimerIcon      = PANENAME_COMMON_MSG_LOW_000_PANE_TIMERICON;
    setupInfo.paneID_MsgCursor      = PANENAME_COMMON_MSG_LOW_000_PANE_CURSOR_00;

    m_MessageWindowManipulator.Setup(setupInfo);
  }


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  void  SetTargetView(app::ui::UIView* pTargetView)
  {
    m_pTargetView = pTargetView;
  }

protected:
  app::ui::UIView*    m_pTargetView;    /**<  Create2Dが行われた、各種Paneのインスタンスを保持するView  */
};


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
LiveMessageMenu::LiveMessageMenu(LiveWork* pWork, NetAppLib::UI::NetAppMessageMenu::IEventHandler* pHandler)
  : NetAppLib::UI::NetAppMessageMenu(pWork, pHandler)
  , m_pWork(pWork)
  , m_pLiveCommonMessageWindowView(NULL)
  , m_pCommonMessageWindowTargetView(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
LiveMessageMenu::~LiveMessageMenu()
{
  Delete2D();
  DeleteGraphicsSystem();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    LiveMessageMenu専用のSetup
  */
//------------------------------------------------------------------
void  LiveMessageMenu::LiveMessageMenuSetup(app::ui::UIView* pTargetView)
{
  m_pCommonMessageWindowTargetView  = pTargetView;
  Setup(RESOURCE_Layout, &m_pWork->GetMessageUtility());
}


//------------------------------------------------------------------
/**
  *  @brief    パラメータなし汎用
  */
//------------------------------------------------------------------
void  LiveMessageMenu::Show(const Mode mode)
{

/*
  void  ShowMessage(const gfl2::str::MsgData& rWindowMessageData, const u32 messageID, const bool isImmediateMessage = false, const bool iynimerIcon = false, const bool isShowBG = false);
*/
  PreShow();
  switch(mode)
  {
    case  MODE_Common_Saving:     /**<  セーブしています  */
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_ymadd_00, true, true);
      break;

    case  MODE_Welcome:           /**<  ライブ大会へようこそ  */
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_01, true);
      break;


    case  MODE_NeedPlayerPass:    /**<  選手証が必要です      */
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_02, false);
      break;


    case  MODE_ReadyIrPortForReceiving:       /**<  IRを向かい合わせて  */
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_03, true);
      break;

    case  MODE_ReceivingPlayerPass:           /**<  受信しています      */
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_21, true, true);
      break;

    case  MODE_ReceivedPlayerPass:            /**<  受信しました        */
//      SetFinishMode_UserInput();
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_05);
      break;

    case  MODE_ReceivingPlayerPassCancel:     /**<  キャンセルしました  */
//      SetFinishMode_UserInput();
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_04);
      break;

    case  MODE_ReceivingPlayerPassFailed:     /**<  失敗しました        */
//      SetFinishMode_UserInput();
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_06);
      break;

    case  MODE_ReceivingPlayerPasshaveParticipated:     /**<  すでに参加済み         */
//      SetFinishMode_UserInput();
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_ymadd_01);
      break;

    case  MODE_ReceivingPlayerPassRejected:                /**<  参加できません         */
//      SetFinishMode_UserInput();
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_29);
      break;

    case  MODE_Registration_ConfirmRegistration:          /**<  チーム登録：｛登録する｝                        */
////      SetFinishMode_UserInput();
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_13, true);
      break;

    case  MODE_Registration_ConfirmCancel:                /**<  チーム登録：登録を中止する？                    */
      ShowYN(msg_lt_win_27, false, false);
      break;

    case  MODE_Registration_ConfirmDeregistration:        /**<  チーム登録：%sへの登録解除する？                      */
    {
      RegisterCupName();
      ShowYN(msg_lt_win_14, false, true);
    }
      break;

    case  MODE_Registration_ConfirmDeregistration2:       /**<  チーム登録：%sへの復帰できなくなるが登録解除する？    */
    {
      RegisterCupName();
      ShowYN(msg_lt_win_15, false, true);
    }
      break;

    case  MODE_Registration_CancelRegistration:           /**<  チーム登録：登録キャンセル                  */
//      SetFinishMode_UserInput();
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_17);
      break;


    case  MODE_Registration_DeregistrationDone:           /**<  チーム登録：%sへの登録解除した              */
    {
      RegisterCupName();
//      SetFinishMode_UserInput();
      App::Tool::MessageMenuView::ShowMessage(&(m_pWork->GetMessageUtility().GetExpandedString(LIVE_MESSAGE_GRAC_ID, msg_lt_win_28)));
    }
      break;


    case  MODE_Registration_Saving:                       /**<  チーム登録：セーブ中                            */
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_ymadd_00, true, true);
      break;


    case  MODE_CheckRemainingMatches_Completed:           /**<  残試合確認：達成  */
//      SetFinishMode_UserInput();
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_07);
      break;

    case  MODE_MatchMenu_WhatWantToDo:                    /**<  なにをしますか？  */
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_18, true);
      break;
      
    case  MODE_MatchMenu_NoReplayData:                    /**<  対戦の　記録がみつかりませんでした  */
//      SetFinishMode_UserInput();
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_25);
      break;
     

    case  MODE_ReplayMatch_CheckingSDCard:                /**<  対戦リプレイ：SDカードを確認中      */
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_08, true, true);
      break;

    case  MODE_ReplayMatch_ConfirmReplay:                 /**<  対戦リプレイ：この対戦を振り返る？  */
      ShowYN(msg_lt_win_24, false, false);
      break;

    case  MODE_ReplayMatch_NoData:                        /**<  対戦リプレイ：データがなかった      */
//      SetFinishMode_UserInput();
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_25);
      break;

    case  MODE_ReplayMatch_BrokenData:                    /**<  対戦リプレイ：データが壊れている    */
//      SetFinishMode_UserInput();
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_10);
      break;


    case  MODE_Deregistration_ConfirmUnregister:          /**<  登録解除：復帰できませんが解除しますか？  */
      RegisterCupName();
      ShowYN(msg_lt_win_14, false, true);
      break;

    case  MODE_Deregistration_ConfirmUnregister2:         /**<  登録解除：本当に解除しますか？  */
      RegisterCupName();
      ShowYN(msg_lt_win_15, false, true);
      break;

    case  MODE_Deregistration_Saving:                     /**<  登録解除：セーブしています  */
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_ymadd_00, true, true);
      break;

    case  MODE_Deregistration_DeregistrationInformation:  /**<  登録解除：解除し、アンロック  */
    {
      RegisterCupName();
//      SetFinishMode_UserInput();
      App::Tool::MessageMenuView::ShowMessage(&(m_pWork->GetMessageUtility().GetExpandedString(LIVE_MESSAGE_GRAC_ID, msg_lt_win_16)));
    }
      break;

    case  MODE_Matching_Searching:
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_20, true, true);
      break;

    case  MODE_Matching_Connecting:                       /**<  マッチング：通信中  */
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_21, true, true);
      break;

    case  MODE_Matching_SendDataToCheatCheckROM:          /**<  マッチング：不正チェックROMへのデータ送信完了  */
//      SetFinishMode_UserInput();
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_26);   /**<  データを送信しました  */
      break;

    case  MODE_Matching_ConfirmCancel:                    /**<  マッチング：通信をキャンセルしますか？         */
      ShowYN(msg_lt_win_11, false, false);
      break;

    case  MODE_Matching_Cancel:                           /**<  マッチング：通信をキャンセルしています         */
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_12, true, true);
      break;

    case  MODE_Matching_RegulationNotMatched:             /**<  マッチング：レギュレーション不一致             */
//      SetFinishMode_UserInput();
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_23);
      break;

    case  MODE_Matching_WaitingInstruction:               /**<  マッチング：スタッフ指示待ち                   */
//      SetFinishMode_UserInput();
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_22);
      break;

    case  MODE_Matching_Matching:                         /**<  マッチング：通信中（マッチング）               */
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_21, true, true);
      break;


    case  MODE_MatchResult_Win:                           /**<  対戦結果：勝利            */
//      SetFinishMode_UserInput();
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_23);
      break;

    case  MODE_MatchResult_Win_Decision:                  /**<  対戦結果：判定勝ち        */
//      SetFinishMode_UserInput();
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_25);
      break;

    case  MODE_MatchResult_Win_Timeout:                   /**<  対戦結果：時間勝利        */
//      SetFinishMode_UserInput();
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_26);
      break;

    case  MODE_MatchResult_Win_Surrender:                 /**<  対戦結果：降参による勝利  */
//      SetFinishMode_UserInput();
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_27);
      break;

    case  MODE_MatchResult_Draw:                          /**<  対戦結果：ひきわけ        */
//      SetFinishMode_UserInput();
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_24);
      break;

    case  MODE_MatchResult_Saving:                        /**<  対戦結果：セーブ中        */
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_08, true, true);
      break;

    case  MODE_MatchResult_SaveFailed_SDFull:
//      SetFinishMode_UserInput();
      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_09);   /*  SDいっぱい  */
      break;

    default:
      /*  nop  */
      break;
  }

}


//------------------------------------------------------------------
/**
  *  @brief     CommonMessageWindowViewを生成する
  */
//------------------------------------------------------------------
App::Tool::CommonMessageWindowView*  LiveMessageMenu::CreateCommonMessageWindowView(const App::Tool::CommonMessageWindowView::PARAM& rParam, App::Tool::CommonMessageWindowView::IEventHandler* pHandler)
{
  m_pLiveCommonMessageWindowView  = GFL_NEW(rParam.pHeap->GetDeviceHeap()) LiveCommonMessageWindowView(rParam, pHandler);
  m_pLiveCommonMessageWindowView->SetTargetView(m_pCommonMessageWindowTargetView);
  return m_pLiveCommonMessageWindowView;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    showの前に共通でやっておくこと
  */
//------------------------------------------------------------------
void  LiveMessageMenu::PreShow(void)
{
  SetYNListMode(false);
  SetFinishMode_None();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LiveMessageMenu::ShowYN(const u32 messageID, const bool isImmediate, const bool isExpand)
{
  gfl2::heap::HeapBase*                 pHeapBaseLower = m_pWork->GetDeviceHeap()->GetLowerHandle();
  NetAppLib::Message::MessageUtility&   rMessageUtil  = m_pWork->GetMessageUtility();
  gfl2::str::StrBuf                     strYes(rMessageUtil.GetString(MESSAGE_GARC_ID, msg_lt_sel_02), pHeapBaseLower);
  gfl2::str::StrBuf                     strNo(rMessageUtil.GetString(MESSAGE_GARC_ID, msg_lt_sel_03), pHeapBaseLower);
  const gfl2::str::StrBuf*              pMessageStr   = NULL;


  if(isExpand)
  {
    pMessageStr =  &(rMessageUtil.GetExpandedString(MESSAGE_GARC_ID, messageID));
  }
  else
  {
    pMessageStr = &(rMessageUtil.GetString(MESSAGE_GARC_ID, messageID));
  }

  SetYNListMode(true);
  ShowMessageWithListMenu(
    /*  ウインドウ設定  */
    pMessageStr,
    isImmediate,
    /*  リスト設定  */
    &strYes,
    &strNo
  );
}


/*
*/
void  LiveMessageMenu::RegisterCupName(void)
{
  const gfl2::str::StrBuf*  pCupName  = m_pWork->m_pAppParam->m_pSavedRegulation->GetTextParam(Regulation::CUPNAME1);

  m_pWork->GetMessageUtility().GetWordSet()->RegisterWord(0, *pCupName);

  GFL_SAFE_DELETE(pCupName);
}



} /*  namespace Live */
} /*  namespace NetApp      */
