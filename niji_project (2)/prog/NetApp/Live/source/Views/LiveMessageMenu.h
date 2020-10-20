
#if !defined(LIVEMESSAGEMENU_H_INCLUDED)
#define LIVEMESSAGEMENU_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LiveMessageMenu.h
 * @date   2015/07/13 Mon. 19:01:15
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h>


namespace NetApp  {
namespace Live {
  class LiveWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  LiveMessageMenu
  *  @brief  ライブ大会に特化したMessageMenu
 * @date   2015/07/13 Mon. 19:01:15
  */
//==============================================================================
class LiveMessageMenu
  : public NetAppLib::UI::NetAppMessageMenu
{
  GFL_FORBID_COPY_AND_ASSIGN(LiveMessageMenu);
public:
  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  LiveMessageMenu(LiveWork* pWork, NetAppLib::UI::NetAppMessageMenu::IEventHandler* pHandler = NULL);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual ~LiveMessageMenu();



  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    LiveMessageMenu専用のSetup
    *  @note     ResourceIDはわかっているので省略
    */
  //------------------------------------------------------------------
  void  LiveMessageMenuSetup(app::ui::UIView* pTargetView);



  enum  Mode
  {
    /*  common  */
    MODE_Common_Saving,                  /**<  セーブしています  */


    /*  main  */
    MODE_Welcome,           /**<  ライブ大会へようこそ  */
    MODE_NeedPlayerPass,    /**<  選手証が必要です      */

    /*  receivePlayerPass  */
    MODE_ReadyIrPortForReceiving,                   /**<  あわせてください       */
    MODE_ReceivingPlayerPass,                       /**<  受信しています         */
    MODE_ReceivedPlayerPass,                        /**<  受信しました           */
    MODE_ReceivingPlayerPassCancel,                 /**<  受信キャンセルしました */
    MODE_ReceivingPlayerPassFailed,                 /**<  受信失敗しました       */
    MODE_ReceivingPlayerPasshaveParticipated,       /**<  すでに参加済み         */
    MODE_ReceivingPlayerPassRejected,               /**<  参加できません         */

    /*  registration team  */
    MODE_Registration_ConfirmRegistration,          /**<  チーム登録：｛登録する｝                        */
    MODE_Registration_ConfirmCancel,                /**<  チーム登録：登録を中止する？                    */
    MODE_Registration_ConfirmDeregistration,        /**<  チーム登録：登録解除する？                      */
    MODE_Registration_ConfirmDeregistration2,       /**<  チーム登録：復帰できなくなるが登録解除する？    */
    MODE_Registration_CancelRegistration,           /**<  チーム登録：登録キャンセル                      */
    MODE_Registration_DeregistrationDone,           /**<  チーム登録：登録解除した                        */
    MODE_Registration_Saving,                       /**<  チーム登録：セーブ中                            */

    /*  check remain match  */
    MODE_CheckRemainingMatches_Completed,           /**<  残試合確認：達成  */

    /*  matchmenu */
    MODE_MatchMenu_WhatWantToDo,                    /**<  なにをしますか？  */
    MODE_MatchMenu_NoReplayData,                    /**<  対戦の　記録がみつかりませんでした  */

    /*  replayMatch */
    MODE_ReplayMatch_CheckingSDCard,                /**<  対戦リプレイ：SDカードを確認中      */
    MODE_ReplayMatch_ConfirmReplay,                 /**<  対戦リプレイ：この対戦を振り返る？  */
    MODE_ReplayMatch_NoData,                        /**<  対戦リプレイ：データがなかった      */
    MODE_ReplayMatch_BrokenData,                    /**<  対戦リプレイ：データが壊れている    */

    /*  deregistration  */
    MODE_Deregistration_ConfirmUnregister,          /**<  登録解除：復帰できませんが解除しますか？  */
    MODE_Deregistration_ConfirmUnregister2,         /**<  登録解除：本当に解除しますか？  */
    MODE_Deregistration_Saving,                     /**<  登録解除：セーブしています    */
    MODE_Deregistration_DeregistrationInformation,  /**<  登録解除：解除し、アンロック  */

    /*  matching  */
    MODE_Matching_Searching,                        /**<  マッチング：探しています                      */
    MODE_Matching_Connecting,                       /**<  マッチング：通信中  */
    MODE_Matching_SendDataToCheatCheckROM,          /**<  マッチング：不正チェックROMへのデータ送信完了  */
    MODE_Matching_ConfirmCancel,                    /**<  マッチング：通信をキャンセルしますか？         */
    MODE_Matching_Cancel,                           /**<  マッチング：通信をキャンセルしています         */
    MODE_Matching_RegulationNotMatched,             /**<  マッチング：レギュレーション不一致             */
    MODE_Matching_WaitingInstruction,               /**<  マッチング：スタッフ指示待ち                   */
    MODE_Matching_Matching,                         /**<  マッチング：通信中（マッチング）               */

    /*  matchResult  */
    MODE_MatchResult_Win,                           /**<  対戦結果：勝利            */
    MODE_MatchResult_Win_Decision,                  /**<  対戦結果：判定勝ち        */
    MODE_MatchResult_Win_Timeout,                   /**<  対戦結果：時間勝利        */
    MODE_MatchResult_Win_Surrender,                 /**<  対戦結果：降参による勝利  */
    MODE_MatchResult_Draw,                          /**<  対戦結果：ひきわけ        */
    MODE_MatchResult_Saving,                        /**<  対戦結果：セーブ中        */
    MODE_MatchResult_SaveFailed_SDFull,             /**<  対戦結果：セーブ失敗：SDFull  */

    MODE_
  };


  void  Show(const Mode mode);                                /**<  汎用（パラメータなし）   */

protected:
  class LiveCommonMessageWindowView;

  //------------------------------------------------------------------
  /**
    *  @brief     CommonMessageWindowViewを生成する
    */
  //------------------------------------------------------------------
  virtual App::Tool::CommonMessageWindowView*  CreateCommonMessageWindowView(const App::Tool::CommonMessageWindowView::PARAM& rParam, App::Tool::CommonMessageWindowView::IEventHandler* pHandler);


  void  PreShow(void);                                                             /**<  showの前に共通でやっておくこと  */
  void  ShowYN(const u32 messageID, const bool isImmediate, const bool isExpand);  /**<  Y/N  */
  void  RegisterCupName(void);

  LiveWork*                     m_pWork;
  LiveCommonMessageWindowView*  m_pLiveCommonMessageWindowView;
  app::ui::UIView*              m_pCommonMessageWindowTargetView;
};



} /*  namespace Live */
} /*  namespace NetApp      */
#endif  /*  #if !defined(LIVEMESSAGEMENU_H_INCLUDED)  */
