//==============================================================================
/**
 * @file   BattleSpotMessageMenu.cpp
 * @date   2015/10/05 Mon. 18:24:01
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../System/BattleSpotWork.h"


#include  "../BattleSpotResource.h"
#include  "./BattleSpotMessageMenu.h"

// =============================================================================
/**/
//==============================================================================
namespace
{
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
  }messageDefinitions[NetApp::BattleSpot::MessageMenu::MODE_MAX]  =  {
    /*  messageID,  immediate,  yesNo,  timericon,  BG,     finishMode,       timeoutFrame    */
    {UINT_MAX,      true,       false,  false,      false,  FINISH_None,      0},  //    MODE_None
    {msg_bs_win_01, true,       false,  false,      false,  FINISH_None,      0},  //    MODE_Welcome_SelectMode,              /**<  モードを選んでください        */
    {msg_bs_win_02, true,       false,  false,      false,  FINISH_None,      0},  //    MODE_FreeBattle_SelectRule,           /**<  どのルールで戦いますか？      */
    {msg_bs_win_02, true,       false,  false,      false,  FINISH_None,      0},  //    MODE_FreeBattle_PokemonRule,          /**<  どのルールで戦いますか？(特別なポケモン)      */
    {msg_bs_win_03, true,       false,  false,      false,  FINISH_None,      0},  //    MODE_RatingBattle_SelectRule,         /**<  どのルールで戦いますか？      */
    {msg_bs_win_50, true,       false,  false,      false,  FINISH_None,      0},  //    MODE_InternetMatch_SelectMode,        /**<  なにをしますか？  */
    {msg_bs_win_50, true,       false,  false,      false,  FINISH_None,      0},  //    MODE_FriendMatch_SelectMode,          /**<  なにをしますか？  */

    {msg_bs_win_06, true,       false,  true,       false,  FINISH_None,      0},  //    MODE_Information_Connecting,          /**<  通信しています                */
    {msg_bs_win_07, true,       false,  true,       false,  FINISH_None,      0},  //    MODE_Information_SDCard,              /**<  SDカードを確認しています                              */
    {msg_bs_win_08, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Information_RegulationUpdated,   /**<  レギュレーションが更新されました                      */
    {msg_bs_win_09, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Information_RegulationRepaired,  /**<  レギュレーションを修復しました                        */
    {msg_bs_win_11, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Information_Regulation_SDFull,   /**<  SDカードがいっぱいでレギュレーションを保存できません  */
    {msg_bs_win_23, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Information_ConnectionError,     /**<  通信エラーが発生しました                              */

    {msg_bs_win_20, true,       false,  true,       false,  FINISH_None,      0},  //    MODE_Message_SearchingOpponent,       /**<  対戦相手を探しています                        */
    {msg_bs_win_21, true,       false,  true,      false,  FINISH_None,      0},  //    MODE_Message_OpponentFound,           /**<  対戦相手が見つかりました                      */
    {msg_bs_win_22, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Message_OpponentNotFound,        /**<  対戦相手が見つかりませんでした                */

    {msg_bs_win_26, true,       false,  true,       false,  FINISH_None,      0},  //    MODE_Message_Checking,                /**<  大会の状態を確認しています          */
    {msg_bs_win_27, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Message_HowToEntry,              /**<  大会に参加するには                  */
    {msg_bs_win_28, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Message_NoTournament,            /**<  参加できる大会はありません          */
    {msg_bs_win_55, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Message_InvalidCupID,            /**<  ことなるたいかいです          */
    
    {msg_bs_win_13, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Message_SeasonIsOver,        /**<  シーズンが終了しました                  */
    {msg_bs_win_29, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Message_TournamentIsOver,        /**<  大会は終了しました                  */
    {msg_bs_win_30, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Message_TournamentIsOverUnlock,        /**<  大会は終了しましたUnlock                 */
    {msg_bs_win_31, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Message_CannotEntry,             /**<  この大会には参加できません          */
    {msg_bs_win_34, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Message_Unregistered,            /**<  大会の参加を解除しました            */
    {msg_bs_win_35, true,       false,  true,       false,  FINISH_None,      0},  //    MODE_Message_DownloadingCard,         /**<  選手証をダウンロードしています      */
    {msg_bs_win_36, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Message_DownloadedCard,          /**<  選手証のダウンロードが完了しました  */
    {msg_bs_win_38, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Message_TournamentNotStarted,    /**<  大会はまだ始まっていません          */
    {msg_bs_win_39, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Message_Retired,                 /**<  途中で参加を解除しました            */
    {msg_bs_win_40, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Message_GameSync,                /**<  バトルチームをロックしてゲームシンクを行いました  */
    {msg_bs_win_41, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Message_DifferentConsole,        /**<  登録したときの本体と異なります      */
    {msg_bs_win_42, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Message_NotAvailableTime,        /**<  対戦できない時間です                      */
    {msg_bs_win_43, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Message_PrescribedMatches,       /**<  対戦回数が決められた回数に達しました      */
    {msg_bs_win_48, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Message_QRRead,                  /**<  QRコードを撮影するためカメラを起動します  */
    {msg_bs_win_49, false,      false,  false,      false,  FINISH_UserInput, 0},  //    MODE_Message_GetCard,                 /**<  デジタル選手証を取得しました              */

    {msg_bs_win_32, false,      false,  false,      false,  FINISH_None,      0},  //    MODE_Message_RetireConfirm,           /**<  リタイアとなります 参加を解除しますか？   */
    {msg_bs_win_33, false,      false,  false,      false,  FINISH_None,      0},  //    MODE_Message_RetireConfirm2,          /**<  本当によろしいですか？                    */

    {msg_bs_win_53, false,      false,  false,      false,  FINISH_None,      0},  //    MODE_Message_RetireConfirm_friend,           
    {msg_bs_win_52, false,      false,  false,      false,  FINISH_None,      0},  //    MODE_Message_RetireConfirm2_friend,          
    {msg_bs_win_51, false,      false,  false,      false,  FINISH_None,      0},  //    MODE_Message_RetireConfirm_no_reg,           
    {msg_bs_win_54, false,      false,  false,      false,  FINISH_UserInput,      0},  //    MODE_Message_Retire_NewCup,           
    

    {msg_bs_win_04, true,       false,  false,      false,  FINISH_None,      0},  //    MODE_BattleVideoAutoUploadConfirm,    /**<  バトルビデオの自動送信を許可しますか？  */
    {msg_bs_win_25, true,       false,  false,      false,  FINISH_None,      0},  //    MODE_Message_ContinueConfirm,    /**<  つづけてあそびますか？  */
    {msg_bs_win_24, false,       false,  false,      false,  FINISH_None,      0},  //    MODE_Message_ContinueConfirmRate,    /**<  つづけてあそびますか？レート  */
    
    {msg_bs_win_25, true,       false,  false,      false,  FINISH_None,      0},  //    MODE_Message_ContinueConfirmCup,    /**<  つづけてあそびますか？  */
    {msg_bs_win_05, false,       false,  false,      false,  FINISH_UserInput,      0},  //    MODE_Message_PGLNoRegisted,    /**<  PGLとうろくしてください（レート）  */
    
    {msg_bs_win_14, false,       false,  false,      false,  FINISH_None,      0},  //    MODE_Message_RegistPIDConfirm,    /**<  本体登録しますか？  */
    {msg_bs_win_15, true,       false,  true,       false,  FINISH_None,      0},  //    MODE_Message_RegistPIDWait,          /**<  本体登録しています 
    {msg_bs_win_16, false,       false,  false,      false,  FINISH_UserInput,      0},  //    MODE_Message_PGLNoRegisted,    /**<  本体登録しました  */

    {msg_bs_win_17, false,       false,  false,      false,  FINISH_None,      0},  //    MODE_Message_ResetRateConfirm,    /**<  本体登録しますか？  */
    {msg_bs_win_18, true,       false,  true,       false,  FINISH_None,      0},  //    MODE_Message_ResetRateWait,          /**<  本体登録しています 
    {msg_bs_win_19, false,       false,  false,      false,  FINISH_UserInput,      0},  //    MODE_Message_ResetRate,    /**<  本体登録しました  */
  };
}




namespace NetApp      {
namespace BattleSpot  {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
MessageMenu::MessageMenu(BattleSpotWork* pWork)
  : NetAppLib::UI::NetAppMessageMenu(pWork)
  , m_pWork(pWork)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
MessageMenu::~MessageMenu()
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
void  MessageMenu::Show(const Mode mode)
{
  const MessageDefinition&  rMessageDefinition  = messageDefinitions[mode];

  PreShow();
  if(!m_pWork->GetMessageUtility()->IsValidMessageID(BATTLESPOT_MESSAGE_ARCDAT, rMessageDefinition.messageID)) return;

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
    if( rMessageDefinition.messageID == msg_bs_win_32 ||
        rMessageDefinition.messageID == msg_bs_win_33 )
    {//大会名を設定
      gfl2::str::StrBuf* pStr = m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetTextParam(Regulation::CUPNAME1);
      m_pWork->GetMessageUtility()->GetWordSet()->RegisterWord( 0 , *pStr );

      GFL_SAFE_DELETE( pStr );

      App::Tool::MessageMenuView::ShowMessage(
        &( m_pWork->GetMessageUtility()->GetExpandedString( BATTLESPOT_MESSAGE_ARCDAT , rMessageDefinition.messageID ) ),
        rMessageDefinition.isImmediate,
        rMessageDefinition.isShowTimerIcon,
        rMessageDefinition.isShowBG
        );
    }
    else
    {
      ShowMessage(
        BATTLESPOT_MESSAGE_ARCDAT,
        rMessageDefinition.messageID,
        rMessageDefinition.isImmediate,
        rMessageDefinition.isShowTimerIcon,
        rMessageDefinition.isShowBG
        );
    }
  }
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    showの前に共通でやっておくこと
  */
//------------------------------------------------------------------
void  MessageMenu::PreShow(void)
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
void  MessageMenu::ShowYN(const u32 messageID, const bool isImmediate)
{
  SetYNListMode(true);
  ShowMessageWithListMenu(
    BATTLESPOT_MESSAGE_ARCDAT,
    messageID,
    isImmediate,
    BATTLESPOT_MESSAGE_ARCDAT,
    msg_bs_sel_18,
    msg_bs_sel_19
  );
}



} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
