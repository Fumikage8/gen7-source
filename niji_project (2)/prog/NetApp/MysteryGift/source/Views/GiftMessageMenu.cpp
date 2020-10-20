
//==============================================================================
/**
 * @file   GiftMessageMenu.cpp
 * @date   2015/08/04 Tue. 11:19:06
 * @author muto_yu
 * @brief  ふしぎなおくりものに特化したMessageMenu
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <thread/include/gfl2_ThreadStatic.h>
#include  <AppLib/include/Tool/app_tool_MenuWindow.h>
#include  "../ApplicationSystem/GiftWork.h"
#include  "../GiftResource.h"
#include  "./GiftMessageMenu.h"



// =============================================================================
/**/
//==============================================================================
namespace {
  enum  FinishMode
  {
    FINISH_None,
    FINISH_UserInput,
    FINISH_Timeout,
    FINISH_UserInputOrTimeout
  };


  static const struct MessageDefinition
  {
    u32         messageID;
    bool        isImmediate;
    bool        isYesNo;
    bool        isShowTimerIcon;
    bool        isShowBG;
    FinishMode  finishMode;
    u8          timeoutFrame;
    bool        bAutoHide;
  }messageDefinitions[NetApp::Gift::GiftMessageMenu::MODE_MAX]  =  {
                               /*  messageID, immediate,  yesNo,  timericon,  BG,     finishMode,       timeoutFrame  bAutoHide    */
#define   MSGDEF_Immediate(_id)       {(_id), true,       false,  false,      false,  FINISH_None,      0,            false}
#define   MSGDEF_Immediate_List(_id)  {(_id), true,       false,  false,      true,   FINISH_None,      0,            false}
#define   MSGDEF_YN(_id)              {(_id), false,      true,   false,      true,   FINISH_None,      0,            false}
#define   MSGDEF_Timer(_id)           {(_id), true,       false,  true,       true,   FINISH_None,      0,            false}
#define   MSGDEF_TimerNoBG(_id)       {(_id), true,       false,  true,       false,  FINISH_None,      0,            false}
#define   MSGDEF_Stream(_id)          {(_id), false,      false,  false,      true,   FINISH_None,      0,            false}
#define   MSGDEF_StreamDialog(_id)    {(_id), false,      false,  false,      true,   FINISH_None,      0,            true}   /**<  EOMで消える  */
#define   MSGDEF_StreamNoBG(_id)      {(_id), false,      false,  false,      false,  FINISH_None,      0,            false}


    MSGDEF_Immediate(   UINT_MAX),              //    MODE_None

    /*  common  */
    MSGDEF_TimerNoBG(msg_mystery_win_05),       //    MODE_Searching,                         /**<  おくりものを探していますn%          */
    MSGDEF_Timer(msg_mystery_win_10),           //    MODE_Connecting,                        /**<  接続しています                      */
    MSGDEF_Timer(msg_mystery_win_19),           //    MODE_Saving,                            /**<  おくりものをうけとっています        */

    MSGDEF_StreamDialog(msg_mystery_win_07),    //    MODE_NotFound,                          /**<  みつかりませんでした                */
    MSGDEF_StreamDialog(msg_mystery_win_52),    //    MODE_FoundButCannotReceive,             /**<  見つけましたがうけとれません        */

    MSGDEF_StreamDialog(msg_mystery_win_06),    //    MODE_Failed,                            /**<  しっぱいしました                    */
    MSGDEF_StreamDialog(msg_mystery_win_42),    //    MODE_WirelessSwitchOff,                 /**<  むせんOFFです                       */
    MSGDEF_YN(msg_mystery_win_18),              //    MODE_ReceiveCancelConfirm,              /**<  うけとるのをやめますか Y/N          */
    MSGDEF_YN(msg_mystery_win_47),              //    MODE_RemainCancelConfirm,               /**<  残っているけどやめますか Y/N        */

    MSGDEF_YN(msg_mystery_win_09),              //    MODE_InternetConnectionConfirm,         /**<  インターネットに接続しますか Y/N    */
    MSGDEF_StreamDialog(msg_mystery_win_16),    //    MODE_GiftNotThere,                      /**<  おくりものがありませんでした        */
    MSGDEF_YN(msg_mystery_win_43),              //    MODE_UpdateFound,                       /**<  追加データが見つかりました eshopにいきますか？ Y/N  */
    MSGDEF_StreamDialog(msg_mystery_win_44),    //    MODE_InternetNotAvailable,              /**<  インターネットに接続できないため終了                */
    MSGDEF_StreamDialog(msg_mystery_win_26),    //    MODE_CanReceiveOnceAtDay,               /**<  一日一回だけ受け取れます            */
    MSGDEF_StreamDialog(msg_mystery_win_50),    //    MODE_EshopNotAvailable,                 /**<  eshopを使用できません 本体を更新してください  */
    MSGDEF_Timer(msg_mystery_win_49),           //  MODE_Finalizing,                          /**<  終了しています                      */


    /*  info  */
    MSGDEF_StreamDialog(msg_mystery_win_01),    //    MODE_AlbumFull,                         /**<  いっぱいでうけとれない              */
    MSGDEF_StreamDialog(msg_mystery_win_48),    //    MODE_AlbumEmpty,                        /**<  うけとっている贈り物がありません    */
    MSGDEF_StreamDialog(msg_mystery_win_02),    //    MODE_AmountSpace,                       /**<  あとn件うけとれます                 */
    MSGDEF_StreamDialog(msg_mystery_win_03),    //    MODE_SelectFunction,                    /**<  うけとりかたをえらんで              */

    MSGDEF_StreamDialog(msg_mystery_win_17),    //    MODE_NG_AlreadyReceived,                /**<  すでにうけとっているのでNG          */
    MSGDEF_StreamDialog(msg_mystery_win_20),    //    MODE_NG_OnceAtDay,                      /**<  一日一回なのでNG                    */
    MSGDEF_StreamDialog(msg_mystery_win_41),    //    MODE_NG_OnceAtDay_HistoryFull,          /**<  一日一回系履歴領域Full              */

    MSGDEF_YN(msg_mystery_win_34),              //    MODE_AlbumCleanupConfirm,               /**<  アルバムを整理しますか？            */
    MSGDEF_StreamDialog(msg_mystery_win_35),    //    MODE_AlbumCleanup,                      /**<  いらないカードを捨ててください      */
    MSGDEF_YN(msg_mystery_win_36),              //    MODE_AlbumCleanupExitConfirm,           /**<  捨てないと貰えません宜しいですか？  */

    /*  Ir  */
    MSGDEF_YN(msg_mystery_win_04),              //    MODE_StartIr,                           /**<  向かい合わせてください Y/N          */

    /*  Serial  */
    MSGDEF_YN(msg_mystery_win_08),              //    MODE_Serial_1stConfirm,                 /**<  ご注意！～シリアルコードで受け取りますか？ Y/N      */
    MSGDEF_StreamDialog(msg_mystery_win_12),    //    MODE_Serial_InputSerial,                /**<  お手持ちのシリアルコードを入力してください          */
    MSGDEF_Timer(msg_mystery_win_13),           //    MODE_Serial_Authentication,             /**<  シリアルコードを確認しています                      */
    MSGDEF_YN(msg_mystery_win_15),              //    MODE_Serial_RetryConfirm,               /**<  もう一度シリアルコードを入れますか？ Y/N            */

    MSGDEF_StreamDialog(msg_mys_serial_error_01), //    MODE_Serial_Error_SerialNG,             /**<  シリアルコードが間違っています                      */
    MSGDEF_StreamDialog(msg_mystery_win_11),      //    MODE_Serial_Error_TimeoutPenalty,       /**<  続けて失敗したためしばらくたってから                */

    MSGDEF_StreamDialog(msg_mys_serial_error_02), //    MODE_Serial_Error_MismatchROM,          /**<  お手持ちのタイトルでは受け取れません  */
    MSGDEF_StreamDialog(msg_mys_serial_error_03), //    MODE_Serial_Error_MismatchRegion,       /**<  お手持ちのソフトでは受け取れません  */
    MSGDEF_StreamDialog(msg_mys_serial_error_04), //    MODE_Serial_Error_AlreadyDownloaded,    /**<  シリアルコードは使用済みです        */
    MSGDEF_StreamDialog(msg_mys_serial_error_05), //    MODE_Serial_Error_CampaignDownloaded,   /**<  このコードは既に受け取っています  */
    MSGDEF_StreamDialog(msg_mys_serial_error_06), //    MODE_Serial_Error_LockedOther,          /**<  このコードは使用中です            */
    MSGDEF_StreamDialog(msg_mys_serial_error_07), //    MODE_Serial_Error_CampaignNotStarted,   /**<  このコードはまだ受け取れません  */
    MSGDEF_StreamDialog(msg_mys_serial_error_08), //    MODE_Serial_Error_CampaignEnd,          /**<  有効期限が過ぎています  */
    MSGDEF_StreamDialog(msg_mys_serial_error_09), //    MODE_Serial_Error_SerialUpdateNG,       /**<  シリアルコードの更新に失敗（Commit失敗）  */


    /*  album  */
    MSGDEF_Immediate_List(msg_mystery_win_28),  //    MODE_Album_ChooseOperation_FrontFace,   /**<  カードをどうしますか（表面）                */
    MSGDEF_Immediate_List(msg_mystery_win_28),  //    MODE_Album_ChooseOperation_BackFace,    /**<  カードをどうしますか（裏面）                */
    MSGDEF_YN(msg_mystery_win_29),              //    MODE_Album_DeleteCardYN,                /**<  捨ててもよろしいですか？                    */
    MSGDEF_StreamDialog(msg_mystery_win_30),    //    MODE_Album_CannotDeleteCard,            /**<  うけとっていないのですてられません          */

    MSGDEF_StreamDialog(msg_mystery_win_46),    //    MODE_Album_DeletedCard,                 /**<  カードを捨てました                          */
    MSGDEF_Stream(msg_mystery_win_46),          //    MODE_Album_DeletedCard_NoHide,          /**<  カードを捨てました                          */

    MSGDEF_Timer(msg_mystery_win_32),           //    MODE_Album_Saving,                      /**<  アルバムを保存しています                    */
    MSGDEF_StreamDialog(msg_mystery_win_51),    //    MODE_Album_NoCard,                      /**<  カードがなくなりました                      */


#undef    MSGDEF_Immediate
#undef    MSGDEF_Immediate_List
#undef    MSGDEF_YN
#undef    MSGDEF_Timer
#undef    MSGDEF_TimerNoBG
#undef    MSGDEF_Stream
  };

  static const u32  MESSAGE_ARCDAT = GARC_message_mystery_DAT;
  
  static const u32  autoHideDelayFrame  = 1;
}




namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
GiftMessageMenu::GiftMessageMenu(GiftWork* pWork, NetAppLib::UI::NetAppMessageMenu::IEventHandler* pHandler)
  : NetAppLib::UI::NetAppMessageMenu(pWork, pHandler)
  , m_pWork(pWork)
  , m_Mode(MODE_None)
  , m_pValueProvider(NULL)
  , m_IsAutoHide(false)
  , m_IsEnableCardSwap(false)
  , m_AutoHideDelay(0)
{
  Setup(RESOURCE_LayoutCommonWindow, &m_pWork->GetMessageUtility());
  SetInputListener(this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
GiftMessageMenu::~GiftMessageMenu()
{
}


//------------------------------------------------------------------
/**
  *  @brief    パラメータなし汎用
  */
//------------------------------------------------------------------
void GiftMessageMenu::Update(void)
{
  while(!IsSetup())
  {
    m_pCommonMenu->Create();
    gfl2::thread::ThreadStatic::CurrentSleep(1);
  }

  NetAppLib::UI::NetAppMessageMenu::Update(); /*  super  */
  if(m_AutoHideDelay)
  {
    if(!(--m_AutoHideDelay))
    {
      HideMessage();
    }
  }
}

//------------------------------------------------------------------
/**
  *  @brief    パラメータなし汎用
  */
//------------------------------------------------------------------


/*
../../../../Print/include/PrintSystem.h"

enum NumberDispType {
  NUM_DISP_LEFT,    ///< 左詰め
  NUM_DISP_SPACE,   ///< 右詰め（スペース埋め）
  NUM_DISP_ZERO,    ///< 右詰め（ゼロ埋め）
};

enum NumberCodeType {
  NUM_CODE_ZENKAKU,   ///< 全角
  NUM_CODE_HANKAKU,   ///< 半角
  // ローカライズ時
  NUM_CODE_DEFAULT = NUM_CODE_HANKAKU,
};

*/
void  GiftMessageMenu::Show(const Mode mode)
{
  const MessageDefinition&  rMessageDefinition  = messageDefinitions[mode];

//  if(m_Mode == mode)  return;


  m_Mode          = mode;
  m_IsAutoHide    = false;
  m_AutoHideDelay = 0;

  PreShow();
  if(!m_pWork->GetMessageUtility().IsValidMessageID(MESSAGE_ARCDAT, rMessageDefinition.messageID)) return;


  m_IsAutoHide  = rMessageDefinition.bAutoHide;

  switch(rMessageDefinition.finishMode)
  {
    case  FINISH_None:                SetFinishMode_None();                                             break;
    case  FINISH_UserInput:           SetFinishMode_UserInput();                                        break;
    case  FINISH_Timeout:             SetFinishMode_Timer(rMessageDefinition.timeoutFrame);             break;
    case  FINISH_UserInputOrTimeout:  SetFinishMode_UserInputOrTimer(rMessageDefinition.timeoutFrame);  break;
  }

  if(rMessageDefinition.isYesNo)
  {
    ShowYN(
      rMessageDefinition.messageID,
      rMessageDefinition.isImmediate
    );
  }
  else
  {
    /*  特殊処理  */
    switch(mode)
    {
      case  MODE_Searching:                 /**<  おくりものを探していますn%          */
      {
        print::WordSet*   pWordSet  = m_pWork->GetMessageUtility().GetWordSet();
        const u32         digits    = 3;
        u32               value     = 0;

        if(m_pValueProvider)  value  = m_pValueProvider->ValueProvider_ReceiveProgress(this);

        pWordSet->RegisterNumber(0, value, digits, print::NUM_DISP_ZERO, print::NUM_CODE_HANKAKU);
        App::Tool::MessageMenuView::ShowMessage(
          &m_pWork->GetMessageUtility().GetExpandedString(MESSAGE_ARCDAT, rMessageDefinition.messageID),
          rMessageDefinition.isImmediate,
          rMessageDefinition.isShowTimerIcon,
          rMessageDefinition.isShowBG
        );
      }
        break;
    
      case  MODE_AmountSpace:               /**<  あとn件うけとれます                 */
      {
        print::WordSet*   pWordSet  = m_pWork->GetMessageUtility().GetWordSet();
        const u32         digits    = 2;
        u32               value     = 0;

        if(m_pValueProvider)  value  = m_pValueProvider->ValueProvider_AmountSpace(this);

        pWordSet->RegisterNumber(0, value, digits, print::NUM_DISP_LEFT, print::NUM_CODE_HANKAKU);
        App::Tool::MessageMenuView::ShowMessage(
          &m_pWork->GetMessageUtility().GetExpandedString(MESSAGE_ARCDAT, rMessageDefinition.messageID),
          rMessageDefinition.isImmediate,
          rMessageDefinition.isShowTimerIcon,
          rMessageDefinition.isShowBG
        );
      }
        break;
      
      /*
        不思議なカードをどうしますか
      */
      case  MODE_Album_ChooseOperation_FrontFace:
      case  MODE_Album_ChooseOperation_BackFace:
      {
#if 0
          ShowMessageWithListMenu(
            GIFT_MESSAGE_GRAC_ID,
            rMessageDefinition.messageID,
            rMessageDefinition.isImmediate,
            /*  list  */
            GIFT_MESSAGE_GRAC_ID,
            msg_mystery_sel_07,    /*  うらをみる      */
            msg_mystery_sel_08,    /*  カードを捨てる  */
            msg_mystery_sel_09,    /*  入れ替える      */
            msg_mystery_sel_18     /*  戻る            */
            );
#else
          gfl2::str::MsgData*   pMessageData  = m_pWork->GetMessageData(GIFT_MESSAGE_GRAC_ID);

          if(mode == MODE_Album_ChooseOperation_FrontFace)  AddItem(pMessageData, msg_mystery_sel_07, false,    false,  SEID_DECIDE);  /*  うらをみる      */
          else                                              AddItem(pMessageData, msg_mystery_sel_06, false,    false,  SEID_DECIDE);  /*  おもてをみる    */
                                                /*  isCancel, isMark,    */
          AddItem(pMessageData, msg_mystery_sel_08, false,    false,  SEID_DECIDE);  /*  カードを捨てる  */

          if(m_IsEnableCardSwap)
          {
            AddItem(pMessageData, msg_mystery_sel_09, false,    false,  SEID_DECIDE);  /*  入れ替える      */
          }

          AddItem(pMessageData, msg_mystery_sel_18, true,     true,   SEID_CANCEL);  /*  戻る            */

          ShowMessageWithListMenu(
            GIFT_MESSAGE_GRAC_ID,
            rMessageDefinition.messageID,
            rMessageDefinition.isImmediate,
            /*  list  */
            GIFT_MESSAGE_GRAC_ID,
            UINT_MAX,   /*  dummy  */
            UINT_MAX    /*  dummy  */
          );
#endif
        }
        break;
      
      default:
        ShowMessage(
          MESSAGE_ARCDAT,
          rMessageDefinition.messageID,
          rMessageDefinition.isImmediate,
          rMessageDefinition.isShowTimerIcon,
          rMessageDefinition.isShowBG
        );
        break;
    }
  }
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    showの前に共通でやっておくこと
  */
//------------------------------------------------------------------
void  GiftMessageMenu::PreShow(void)
{
  SetYNListMode(false);
  SetFinishMode_None();
  m_IsAutoHide  = false;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  GiftMessageMenu::ShowYN(const u32 messageID, const bool isImmediate)
{
  SetYNListMode(true);

  if(messageID == msg_mystery_win_08)
  {
    /*  msg_mystery_win_08は長いため一時バッファからあふれる  */
    NetAppLib::Message::MessageUtility&  rMessageUtil   = m_pWork->GetMessageUtility();
    const gfl2::str::MsgData&            rMsgData       = *(rMessageUtil.GetData(MESSAGE_ARCDAT));

    App::Tool::MessageMenuView::ShowMessageWithListMenu(
      /*  ウインドウ設定  */
      &(rMessageUtil.GetString(MESSAGE_ARCDAT, messageID)),
      isImmediate,
      /*  リスト設定  */
      rMsgData,
      msg_mystery_sel_04,
      msg_mystery_sel_05
    );
  }
  else
  {
    ShowMessageWithListMenu(
      MESSAGE_ARCDAT,
      messageID,
      isImmediate,
      MESSAGE_ARCDAT,
      msg_mystery_sel_04,
      msg_mystery_sel_05
    );
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  GiftMessageMenu::CommonMessageWindowView_OnEvent(const App::Tool::CommonMessageWindowView::IEventHandler::EventCode event)
{
  switch(event)
  {
    case  App::Tool::CommonMessageWindowView::IEventHandler::EVENT_OnEOM:
      if(m_IsAutoHide)
      {
        m_AutoHideDelay = autoHideDelayFrame;
      }
      break;

    case  App::Tool::CommonMessageWindowView::IEventHandler::EVENT_OnFinish:
      /*  nop  */
      break;
  };
  /*  super  */
  NetAppLib::UI::NetAppMessageMenu::CommonMessageWindowView_OnEvent(event);
}



//------------------------------------------------------------------
/**
  *  @brief    internet接続
  */
//------------------------------------------------------------------
/*  autherror  */
void  GiftMessageMenu::ShowAuthError(const AuthenticationHelper::InternetConnectResult     authResult)
{
  switch(authResult)
  {
    case  AuthenticationHelper::INTERNET_CONNECT_RESULT_Processing:           /**<  処理中  */
    case  AuthenticationHelper::INTERNET_CONNECT_RESULT_Success:              /**<  成功    */
      /*  nop  */
      break;



    case  AuthenticationHelper::INTERNET_CONNECT_RESULT_Error_Network:        /**<  エラー：net接続       */
      Show(MODE_Failed);
      break;

    case  AuthenticationHelper::INTERNET_CONNECT_RESULT_Error_RequirePatch:   /**<  エラー：パッチが必要  */
      Show(MODE_UpdateFound);
      break;

    default:
      GFL_ASSERT(false);
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    friendServer接続
  */
//------------------------------------------------------------------
void  GiftMessageMenu::ShowAuthError(const AuthenticationHelper::FriendServerConnectResult authResult)
{
  switch(authResult)
  {
    case  AuthenticationHelper::FRIENDSERVER_CONNECT_RESULT_Processing:             /**<  処理中  */
    case  AuthenticationHelper::FRIENDSERVER_CONNECT_RESULT_Success:                /**<  成功    */
      /*  nop  */
      break;


    case  AuthenticationHelper::FRIENDSERVER_CONNECT_RESULT_Error_Network:          /**<  エラー：net接続  */
      Show(MODE_Failed);
      break;


    default:
      GFL_ASSERT(false);
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    PGL認証
  */
//------------------------------------------------------------------
void  GiftMessageMenu::ShowAuthError(const AuthenticationHelper::PGLSerialAuthResult       authResult)
{
  switch(authResult)
  {
    case  AuthenticationHelper::PGL_SERIAL_AUTH_RESULT_Processing:
    case  AuthenticationHelper::PGL_SERIAL_AUTH_RESULT_Success:
      /*  nop  */
      break;

    case  AuthenticationHelper::PGL_SERIAL_AUTH_RESULT_Error_InvalidCode:                 Show(MODE_Serial_Error_SerialNG);           break;
    case  AuthenticationHelper::PGL_SERIAL_AUTH_RESULT_Error_MismatchROM:                 Show(MODE_Serial_Error_MismatchROM);        break;
    case  AuthenticationHelper::PGL_SERIAL_AUTH_RESULT_Error_MismatchRegion:              Show(MODE_Serial_Error_MismatchRegion);     break;
    case  AuthenticationHelper::PGL_SERIAL_AUTH_RESULT_Error_AlreadyDownloaded:           Show(MODE_Serial_Error_AlreadyDownloaded);  break;
    case  AuthenticationHelper::PGL_SERIAL_AUTH_RESULT_Error_CampaignDownloaded:          Show(MODE_Serial_Error_CampaignDownloaded); break;
    case  AuthenticationHelper::PGL_SERIAL_AUTH_RESULT_Error_LockedOther:                 Show(MODE_Serial_Error_LockedOther);        break;
    case  AuthenticationHelper::PGL_SERIAL_AUTH_RESULT_Error_CampaignNotStarted:          Show(MODE_Serial_Error_CampaignNotStarted); break;
    case  AuthenticationHelper::PGL_SERIAL_AUTH_RESULT_Error_CampaignEnd:                 Show(MODE_Serial_Error_CampaignEnd);        break;


    case  AuthenticationHelper::PGL_SERIAL_AUTH_RESULT_Error_ServerError:
      /*  appletで通知するため、ゲーム側ではなにもしない  */
      NetAppLib::UI::NetAppMessageMenu::CommonMessageWindowView_OnEvent(App::Tool::CommonMessageWindowView::IEventHandler::EVENT_OnEOM);
      break;

    default:
      GFL_ASSERT(false);
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    CommitSave
  */
//------------------------------------------------------------------
void  GiftMessageMenu::ShowAuthError(const AuthenticationHelper::CommitSaveResult          authResult)
{
  switch(authResult)
  {
    case  AuthenticationHelper::COMMIT_SAVE_RESULT_Processing:
    case  AuthenticationHelper::COMMIT_SAVE_RESULT_Success:
      /*  nop  */
      break;

    /*  commitsaveではtimeoutpenaltyは確認しない  */
    case  AuthenticationHelper::COMMIT_SAVE_RESULT_Error_InvalidCode:         Show(MODE_Serial_Error_SerialUpdateNG);     break;
    case  AuthenticationHelper::COMMIT_SAVE_RESULT_Error_MismatchROM:         Show(MODE_Serial_Error_MismatchROM);        break;
    case  AuthenticationHelper::COMMIT_SAVE_RESULT_Error_MismatchRegion:      Show(MODE_Serial_Error_MismatchRegion);     break;
    case  AuthenticationHelper::COMMIT_SAVE_RESULT_Error_AlreadyDownloaded:   Show(MODE_Serial_Error_AlreadyDownloaded);  break;
    case  AuthenticationHelper::COMMIT_SAVE_RESULT_Error_CampaignDownloaded:  Show(MODE_Serial_Error_CampaignDownloaded); break;
    case  AuthenticationHelper::COMMIT_SAVE_RESULT_Error_LockedOther:         Show(MODE_Serial_Error_LockedOther);        break;
    case  AuthenticationHelper::COMMIT_SAVE_RESULT_Error_CampaignNotStarted:  Show(MODE_Serial_Error_CampaignNotStarted); break;
    case  AuthenticationHelper::COMMIT_SAVE_RESULT_Error_CampaignEnd:         Show(MODE_Serial_Error_CampaignEnd);        break;


    case  AuthenticationHelper::COMMIT_SAVE_RESULT_Error_ServerError:
      /*  appletで通知するため、ゲーム側ではなにもしない  */
      NetAppLib::UI::NetAppMessageMenu::CommonMessageWindowView_OnEvent(App::Tool::CommonMessageWindowView::IEventHandler::EVENT_OnEOM);
      break;


    default:
      GFL_ASSERT(false);
      break;
  }
}




} /*  namespace Gift    */
} /*  namespace NetApp  */
