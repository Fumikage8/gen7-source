#if !defined(LOWERVIEW_SELECTION_H_INCLUDED)
#define LOWERVIEW_SELECTION_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LowerView_Selection.h
 * @date   2015/07/16 Thu. 12:36:12
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/UI/NetAppCursorView.h"
#include  "NetStatic/NetAppLib/include/UI/NetAppPokePartyUtility.h"
#include  "NetStatic/NetAppLib/include/UI/NetAppTrainerIconVisibilityControl.h"

#include  "../LiveMessageMenu.h"
#include  "./IDCardParts_LowerSelection.h"

namespace NetApp  {
namespace Live    {

class LiveWork;

// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/07/16 Thu. 12:58:37
  */
//==============================================================================
class LowerView_Selection
  : public NetAppLib::UI::NetAppCursorView
  , public NetAppLib::UI::NetAppTrainerIconVisibilityControl
  , public LiveMessageMenu::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( LowerView_Selection );
public:

  /**
  */
  class IEventHandler
  {
  public:
    virtual ~IEventHandler(){}

    enum  EventCode

    {
      /*
        common
      */
      EVENT_Exit,
      EVENT_MessageFinish,
      EVENT_PushA,          /*  試合開始用  */
      
      /*
        mainmenu
      */
      EVENT_SelectedStart,
      EVENT_SelectedReplay,
      EVENT_SelectedUnregister,
      EVENT_ConfirmReceivePlayerPass_Ir,
      EVENT_ConfirmReceivePlayerPass_QR,
      
      /*
        receivePlayerPass
      */

      /*
        registration
      */
      EVENT_Registration_ConfirmRegistration_Start,   /**<  チーム登録：｛登録する｝                        */
      EVENT_Registration_ConfirmCancel_Yes,           /**<  チーム登録：登録を中止する？                    */
      EVENT_Registration_ConfirmCancel_No,            /**<  チーム登録：登録を中止する？                    */
      EVENT_Registration_ConfirmDeregistration_Yes,   /**<  チーム登録：登録解除する？                      */
      EVENT_Registration_ConfirmDeregistration_No,    /**<  チーム登録：登録解除する？                      */
      EVENT_Registration_ConfirmDeregistration2_Yes,  /**<  チーム登録：復帰できなくなるが登録解除する？    */
      EVENT_Registration_ConfirmDeregistration2_No,   /**<  チーム登録：復帰できなくなるが登録解除する？    */
      
      /*
        checkRemainingMatches
      */
      
      /*
        matchMenu
      */
      EVENT_MatchMenu_SelectedStart,          /**<  マッチメニュー：挑戦する  */
      EVENT_MatchMenu_SelectedReplay,         /**<  マッチメニュー：振り返る  */
      EVENT_MatchMenu_SelectedDeregistration, /**<  マッチメニュー：解除する  */
      
      /*
        replayMatch
      */
      EVENT_ReplayMatch_Replay_Yes,           /**<  対戦リプレイ：振り返る？→はい    */
      EVENT_ReplayMatch_Replay_No,            /**<  対戦リプレイ：振り返る？→いいえ  */
      
      /*
        registration
      */
      EVENT_Deregistration_ConfirmUnregister_Yes,           /**<  登録解除：復帰できませんが解除しますか？  */
      EVENT_Deregistration_ConfirmUnregister_No,            /**<  登録解除：復帰できませんが解除しますか？  */
      EVENT_Deregistration_ConfirmUnregister2_Yes,          /**<  登録解除：本当に解除しますか？  */
      EVENT_Deregistration_ConfirmUnregister2_No,           /**<  登録解除：本当に解除しますか？  */
      
      /*
        matching
      */
      EVENT_Matching_ConfirmCancel_Yes,                     /**<  マッチング：通信をキャンセルしますか？         */
      EVENT_Matching_ConfirmCancel_No,
      
      /*
        matchResult
      */
      
      EVENT_NONE
    };

    virtual void  LowerView_Selection_OnEvent(const EventCode eventCode) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pHandler = pHandler;}


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  LowerView_Selection(LiveWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual ~LowerView_Selection();

  /*  implement UIView I/F  */
  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
  virtual bool  IsDrawing(void) const;
   /*  endimplement UIView I/F  */


  enum  ViewMode
  {
    /*  mainmenu  */
    VIEWMODE_MainMenu_NoReplay,                         /**<  メインメニュー：｛はじめる｝                */
    VIEWMODE_MainMenu_Full,                             /**<  メインメニュー：｛はじめる|前回の成績｝     */
    VIEWMODE_MainMenu_ReceivePlayerPass,                /**<  メインメニュー：｛選手証をうけとる｝        */

    /*  receivePlayerPass  */
    VIEWMODE_ReceivePlayerPass_ReadyIrPort,             /**<  選手証受信：向かい合わせて */
    VIEWMODE_ReceivePlayerPass_Receiving,               /**<  選手証受信：受信しています  */
    VIEWMODE_ReceivePlayerPass_Received,                /**<  選手証受信：受信完了        */
    VIEWMODE_ReceivePlayerPass_ReceiveCancel,           /**<  選手証受信：受信キャンセル  */
    VIEWMODE_ReceivePlayerPass_ReceiveFailed,           /**<  選手証受信：受信失敗          */
    VIEWMODE_ReceivePlayerPass_HaveParticipated,        /**<  選手証受信：すでに参加済み    */
    VIEWMODE_ReceivePlayerPass_Rejected,                /**<  選手証受信：参加できない      */
    VIEWMODE_ReceivePlayerPass_Saving,                  /**<  選手証受信：セーブしています    */

    /*  registration  */
    VIEWMODE_Registration_ConfirmRegistration,          /**<  チーム登録：｛登録する｝                        */
    VIEWMODE_Registration_ConfirmCancel,                /**<  チーム登録：登録を中止する？                    */
    VIEWMODE_Registration_ConfirmDeregistration,        /**<  チーム登録：登録解除する？                      */
    VIEWMODE_Registration_ConfirmDeregistration2,       /**<  チーム登録：復帰できなくなるが登録解除する？    */
    VIEWMODE_Registration_CancelRegistration,           /**<  チーム登録：登録解除キャンセル                  */
    VIEWMODE_Registration_DeregistrationDone,           /**<  チーム登録：登録解除した                        */
    VIEWMODE_Registration_Saving,

    /*  checkRemainingMatches  */
    VIEWMODE_CheckRemainingMatches_Completed,           /**<  残試合数：達成  */
    VIEWMODE_CheckRemainingMatches_NotComplete,         /**<  残試合数：未達  */
    VIEWMODE_CheckRemainingMatches_Saving,              /**<  残試合数：セーブ中        */

    /*  matchMenu  */
    VIEWMODE_MatchMenu_Full,                            /**<  大会メニュー：｛挑戦する|振り返る|解除する｝  */
    VIEWMODE_MatchMenu_ReplayOnly,                      /**<  大会メニュー：｛振り返る｝           */
    VIEWMODE_MatchMenu_NoReplayData,                    /**<  大会メニュー： 対戦の　記録がみつかりませんでした▼          */

    /*  replaymatch  */
    VIEWMODE_ReplayMatch_CheckingSDCard,                /**<  対戦リプレイ：SDカードを確認中      */
    VIEWMODE_ReplayMatch_ConfirmReplay,                 /**<  対戦リプレイ：この対戦を振り返る？  */
    VIEWMODE_ReplayMatch_NoData,                        /**<  対戦リプレイ：データがなかった      */
    VIEWMODE_ReplayMatch_BrokenData,                    /**<  対戦リプレイ：データが壊れている    */

    /*  deregistration  */
    VIEWMODE_Deregistration_ConfirmUnregister,          /**<  登録解除：復帰できませんが解除しますか？  */
    VIEWMODE_Deregistration_ConfirmUnregister2,         /**<  登録解除：本当に解除しますか？  */
    VIEWMODE_Deregistration_Saving,                     /**<  登録解除：セーブしています  */
    VIEWMODE_Deregistration_DeregistrationInformation,  /**<  登録解除：解除し、アンロック  */

    /*  matching  */
    VIEWMODE_Matching_Searching,                        /**<  マッチング：対戦相手を探しています              */
    VIEWMODE_Matching_Connecting,                       /**<  マッチング：通信中  */
    VIEWMODE_Matching_SendDataToCheatCheckROM,          /**<  マッチング：不正チェックROMへのデータ送信完了  */
    VIEWMODE_Matching_ConfirmCancel,                    /**<  マッチング：通信をキャンセルしますか？         */
    VIEWMODE_Matching_Cancel,                           /**<  マッチング：通信をキャンセルしています         */
    VIEWMODE_Matching_RegulationNotMatched,             /**<  マッチング：レギュレーション不一致             */
    VIEWMODE_Matching_WaitingInstruction,               /**<  マッチング：スタッフ指示待ち                   */
    VIEWMODE_Matching_Matching,                         /**<  マッチング：通信中（マッチング）               */

    /*  matchResult  */
    VIEWMODE_MatchResult_Win,                           /**<  対戦結果：勝利            */
    VIEWMODE_MatchResult_Win_Decision,                  /**<  対戦結果：判定勝ち        */
    VIEWMODE_MatchResult_Win_Timeout,                   /**<  対戦結果：時間勝利        */
    VIEWMODE_MatchResult_Win_Surrender,                 /**<  対戦結果：降参による勝利  */
    VIEWMODE_MatchResult_Draw,                          /**<  対戦結果：ひきわけ        */
    VIEWMODE_MatchResult_Saving,                        /**<  対戦結果：セーブ中        */
    VIEWMODE_MatchResult_SaveFailed_SDFull,             /**<  対戦結果：セーブ失敗：SDがいっぱい  */

    VIEWMODE_NONE
  };
  void  SetViewMode(const ViewMode viewMode);


  void  UpdateIDCard(void);


  void  SetAButtonEnable(const bool isEnable)           {m_IsAButtonEnable  = isEnable;}
  void  SetBButtonEnable(const bool isEnable)           {m_IsBButtonEnable  = isEnable;}

protected:
  /*  implement MessageMenuView I/F  */
  virtual void  MessageMenuView_OnEvent(const LiveMessageMenu::IEventHandler::EventCode event);
  /*  endimplement MessageMenuView I/F  */
  /*
    UIInputListener
  */
  virtual app::ui::UIInputListener::ListenerResult  OnLayoutPaneEvent( const u32 buttonID );
  virtual app::ui::UIInputListener::ListenerResult  OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );
  /*
    end UIInputListener
  */

  virtual void  OnSetInputEnabled( bool isEnable );


  virtual void  OnSelected()  {};



  LiveWork*                     m_pWork;
  IEventHandler*                m_pHandler;
  ViewMode                      m_ViewMode;

private:
  enum  LayoutMode
  {
    LAYOUT_Option_1,
    LAYOUT_Option_2,
    LAYOUT_Option_3,
    LAYOUT_Search_1,
    LAYOUT_Search_2,
    LAYOUT_IDCard,
    LAYOUT_Message,
    LAYOUT_MessageWithBack,

    LAYOUT_NONE,
    LAYOUT_
  };

  void  SetLayoutMode(const LayoutMode layoutMode);
  void  SetLayoutModeAfterOnMessageFinish(const LayoutMode layoutModeImmediate, const LayoutMode reserveLayoutMode, const bool isActivateOnEOM = false);
  

  enum  TextBoxID
  {
    TEXTBOX_OptionButton_1,
    TEXTBOX_OptionButton_2,
    TEXTBOX_OptionButton_3,

    TEXTBOX_SearchButton_1,
    TEXTBOX_SearchButton_2,
    
    /*  LAYOUT_IDCard */
    TEXTBOX_CARD_REMAIN_MATCH,
    TEXTBOX_CARD_TOTAL_MATCH,
    TEXTBOX_CARD_WINS,
    TEXTBOX_CARD_LOSES,
    TEXTBOX_CARD_TEAMNAME,

    TEXTBOX_MAX
  };
  void  SetTextBoxMessage(const TextBoxID textBoxID, const u32 messageID);
  void  ApplyButtonAndCursor(const u32 buttonIDs[], const u32 idNum);
  void  Event(const IEventHandler::EventCode event);
  void  ClearButtonEvent(void);


  /*
  */
  gfl2::lyt::LytTextBox*                          m_pTextBoxes[TEXTBOX_MAX];
  LiveMessageMenu*                                m_pLiveMessageMenu;
  LayoutMode                                      m_ReservedLayoutModeOnMessageFinished;
  bool                                            m_bReservedLayoutModeAvtivateOnEOM;
  IEventHandler::EventCode*                       m_pButtonEventArray;
  mutable NetAppLib::UI::NetAppPokePartyUtility   m_PartyUtil;
  bool                                            m_IsAButtonEnable;
  bool                                            m_IsBButtonEnable;


  enum  PictureID
  {
    PICTURE_Mine_Card,                  /**<  カード内自分                      */
    
    PICTURE_MAX
  };
  gfl2::lyt::LytPicture*    m_pPicture[PICTURE_MAX];
};


} /*  namespace Live    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(LOWERVIEW_SELECTION_H_INCLUDED)  */
