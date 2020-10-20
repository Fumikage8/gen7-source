#if !defined(LOWERVIEW_SELECTION_H_INCLUDED)
#define LOWERVIEW_SELECTION_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LowerView_Selection.h
 * @date   2015/10/02 Fri. 13:34:08
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================

// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"
#include  "AppLib/include/Tool/GeneralView/GeneralLowerView.h"

#include  "./BattleSpotMessageMenu.h"


namespace NetApp      {
namespace BattleSpot  {
  class BattleSpotWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  */
//==============================================================================
class LowerView_Selection
  : public    NetAppLib::UI::NetAppEmptyView
  , protected app::tool::GeneralLowerView::IEventHandler
  , protected MessageMenu::IEventHandler
  , protected app::tool::IResourceProvider
{
  GFL_FORBID_COPY_AND_ASSIGN( LowerView_Selection );
public:

  class IEventHandler
  {
  public:
    enum  EventCode
    {
      /*  general */
      EVENT_Back,

      /*  message  */
      EVENT_Message_EOM,
      EVENT_Message_Finish,

      /*  welcome  */
      EVENT_WelCome_FreeBattle,
      EVENT_WelCome_RatingBattle,
      EVENT_WelCome_InternetMatch,
      EVENT_WelCome_FriendMatch,
      
      /*  freebattle  */
      EVENT_FreeBattle_Rule_SingleBattle,
      EVENT_FreeBattle_Rule_DoubleBattle,
      EVENT_FreeBattle_Rule_BattleRoyal,
      EVENT_FreeBattle_Pokemon_SpecialPokemonEnable,
      EVENT_FreeBattle_Pokemon_SpecialPokemonDisable,

      /*  ratingBattle  */
      EVENT_RatingBattle_Rule_SingleBattle,
      EVENT_RatingBattle_Rule_DoubleBattle,
      EVENT_RatingBattle_Rule_SpecialBattle,
      EVENT_RatingBattle_Rule_WCSBattle,

      /*  internetMatch  */
      EVENT_InternetMatch_Entry,                /**<  モード選択：大会に参加する  */
      EVENT_InternetMatch_Match,                /**<  モード選択：対戦する        */
      EVENT_InternetMatch_WatchCard,            /**<  モード選択：選手証を見る    */
      EVENT_InternetMatch_PreviousTournament,   /**<  モード選択：前回大会        */
      EVENT_InternetMatch_Retire,               /**<  モード選択：参加解除        */
      /*  friendMatch  */
      EVENT_Friendmatch_QRRead,                 /**<  モード選択：QRコードを読み込む  */
      EVENT_FriendMatch_Match,                  /**<  モード選択：対戦する            */
      EVENT_FriendMatch_WatchCard,              /**<  モード選択：選手証を見る        */
      EVENT_FriendMatch_PreviousTournament,     /**<  モード選択：前回大会            */
      EVENT_FriendMatch_Retire,                 /**<  モード選択：参加解除            */
      
      /*  battleVideoAutoUpload  */
      EVENT_BattleVideoAutoUpload_Yes,          /**<  バトルビデオ許可する？：はい    */
      EVENT_BattleVideoAutoUpload_No,           /**<  バトルビデオ許可する？：いいえ  */

      /*  retire  */
      EVENT_Retire1_Yes,                        /**<  参加解除する？：はい    */
      EVENT_Retire1_No,                         /**<  参加解除する？：いいえ  */
      EVENT_Retire2_Yes,                        /**<  ほんとうに参加解除する？：はい    */
      EVENT_Retire2_No,                         /**<  ほんとうに参加解除する？：いいえ  */

      EVENT_Retire3_Yes,                        /**<  参加解除する？：はい    *///仲間大会新大会用
      EVENT_Retire3_No,                         /**<  参加解除する？：いいえ  *///仲間大会新大会用

      EVENT_Retire4_Yes,                        /**<  参加解除する？：はい    *///レギュレーションないとき
      EVENT_Retire4_No,                         /**<  参加解除する？：いいえ  *///レギュレーションないとき

      /* continue */
      EVENT_Continue_Next,
      EVENT_Continue_Exit,
      EVENT_Continue_Change,
      

      /* regist pid */
      EVENT_RegistPID_Yes,
      EVENT_RegistPID_No,

      /* Reset Rate */
      EVENT_ResetRate_Yes,
      EVENT_ResetRate_No,

      EVENT_NONE    /**<  内部制御用  */
    };

    virtual ~IEventHandler(){}
    virtual void  LowerView_Selection_OnEvent(LowerView_Selection* pSender, const EventCode eventCode) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pEventHandler = pHandler;}


  LowerView_Selection(BattleSpotWork* pWork);
  virtual ~LowerView_Selection();

  /*  implement UIView I/F  */
//  virtual void  Update(void);
//  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
  virtual bool IsDrawing(void) const;
   /*  endimplement UIView I/F  */


  enum  ViewMode
  {
    /*  welcome  */
    VIEWMODE_Welcome_SelectMode,                /**<  モードを選んでください      */

    /*  freebattle  */
    VIEWMODE_FreeBattle_SelectRule,             /**<  どのルールで戦いますか？    */
    VIEWMODE_FreeBattle_PokemonRule,            /**<  特別なポケモンあり/なし     */

    /*  ratingBattle  */
    VIEWMODE_RatingBattle_SelectRule,           /**<  どのルールで戦いますか？    */

    /*  internetMatch  */
    VIEWMODE_InternetMatch_Work0,               /**<  参加ワークの値に応じたトップメニュー  */
    VIEWMODE_InternetMatch_Work1,               /**<  参加ワークの値に応じたトップメニュー  */
    VIEWMODE_InternetMatch_Work2,               /**<  参加ワークの値に応じたトップメニュー  */
    VIEWMODE_InternetMatch_Work3_TeamLocked,    /**<  参加ワークの値に応じたトップメニュー  */
    VIEWMODE_InternetMatch_Work3_TeamUnlocked,  /**<  参加ワークの値に応じたトップメニュー  */
    VIEWMODE_InternetMatch_Work4_TeamLocked,    /**<  参加ワークの値に応じたトップメニュー  */
    VIEWMODE_InternetMatch_Work4_TeamUnlocked,  /**<  参加ワークの値に応じたトップメニュー  */
    VIEWMODE_InternetMatch_Work5,               /**<  参加ワークの値に応じたトップメニュー  */
    VIEWMODE_InternetMatch_Work6,               /**<  参加ワークの値に応じたトップメニュー  */
    VIEWMODE_InternetMatch_Work7,               /**<  参加ワークの値に応じたトップメニュー  */

    /*  friendMatch  */
    VIEWMODE_FriendMatch_Work0,               /**<  参加ワークの値に応じたトップメニュー  */
    VIEWMODE_FriendMatch_Work0_1,
    VIEWMODE_FriendMatch_Work1,               /**<  参加ワークの値に応じたトップメニュー  */
    VIEWMODE_FriendMatch_Work2,               /**<  参加ワークの値に応じたトップメニュー  */
    VIEWMODE_FriendMatch_Work3_TeamLocked,    /**<  参加ワークの値に応じたトップメニュー  */
    VIEWMODE_FriendMatch_Work3_TeamUnlocked,  /**<  参加ワークの値に応じたトップメニュー  */
    VIEWMODE_FriendMatch_Work4_TeamLocked,    /**<  参加ワークの値に応じたトップメニュー  */
    VIEWMODE_FriendMatch_Work4_TeamUnlocked,  /**<  参加ワークの値に応じたトップメニュー  */
    VIEWMODE_FriendMatch_Work5,               /**<  参加ワークの値に応じたトップメニュー  */
    VIEWMODE_FriendMatch_Work6,               /**<  参加ワークの値に応じたトップメニュー  */

    /*  message  */
    VIEWMODE_Message_Connecting,            /**<  通信しています                */
    VIEWMODE_Message_SDCard,                /**<  SDカードを確認しています                              */
    VIEWMODE_Message_RegulationUpdated,     /**<  レギュレーションが更新されました                      */
    VIEWMODE_Message_RegulationRepaired,    /**<  レギュレーションを修復しました                        */
    VIEWMODE_Message_Regulation_SDFull,     /**<  SDカードがいっぱいでレギュレーションを保存できません  */
    VIEWMODE_Message_ConnectionError,       /**<  通信エラーが発生しました                              */

    VIEWMODE_Message_Checking,              /**<  大会の状態を確認しています          */
    VIEWMODE_Message_HowToEntry,            /**<  大会に参加するには                  */
    VIEWMODE_Message_NoTournament,          /**<  参加できる大会はありません          */
    VIEWMODE_Message_InvalidCupID,//異なる大会ですSD確認してください

    VIEWMODE_Message_SeasonIsOver,//シーズン終了しました
    VIEWMODE_Message_TournamentIsOver,      /**<  大会は終了しました                  */
    VIEWMODE_Message_TournamentIsOverUnlock,//大会は終了しましたUnlock
    VIEWMODE_Message_CannotEntry,           /**<  この大会には参加できません          */
    VIEWMODE_Message_Unregistered,          /**<  大会の参加を解除しました            */
    VIEWMODE_Message_DownloadingCard,       /**<  選手証をダウンロードしています      */
    VIEWMODE_Message_DownloadedCard,        /**<  選手証のダウンロードが完了しました  */
    VIEWMODE_Message_TournamentNotStarted,  /**<  大会はまだ始まっていません          */
    VIEWMODE_Message_Retired,               /**<  途中で参加を解除しました            */
    VIEWMODE_Message_GameSync,              /**<  バトルチームをロックしてゲームシンクを行いました  */
    VIEWMODE_Message_DifferentConsole,      /**<  登録したときの本体と異なります      */
    VIEWMODE_Message_NotAvailableTime,      /**<  対戦できない時間です                      */
    VIEWMODE_Message_PrescribedMatches,     /**<  対戦回数が決められた回数に達しました      */
    VIEWMODE_Message_QRRead,                /**<  QRコードを撮影するためカメラを起動します  */
    VIEWMODE_Message_GetCard,               /**<  デジタル選手証を取得しました              */

    VIEWMODE_Message_RetireConfirm,         /**<  リタイアとなります 参加を解除しますか？   */
    VIEWMODE_Message_RetireConfirm2,        /**<  本当によろしいですか？                    */

    VIEWMODE_Message_RetireConfirm_friend,         /**<  リタイアとなります 参加を解除しますか？ （仲間大会）  */
    VIEWMODE_Message_RetireConfirm2_friend,        /**<  本当によろしいですか？（大会名なし）                    */
    VIEWMODE_Message_RetireConfirm_no_reg,      /**<  レギュレーションがないのでリタイアとなります 参加を解除しますか？   */

    VIEWMODE_Message_Retire_friend_NewCup,//新しい大会のためにリタイアしました

    /*  others  */
    VIEWMODE_BattleVideoAutoUploadConfirm,    /**<  バトルビデオの自動送信を許可しますか？  */

    VIEWMODE_Message_ContinueConfirm,        /**< たいせんを　つづけますか？ */
    VIEWMODE_Message_ContinueConfirmRate,   //つづけますか？レート
    VIEWMODE_Message_ContinueConfirmCup,//続けますか
    VIEWMODE_Message_PGLNoRegist,            //PGLとうろくしてください 
    VIEWMODE_Message_RegistPIDConfirm,       //ほんたい登録しますか
    VIEWMODE_Message_RegistPIDWait,          //ほんたい登録しています
    VIEWMODE_Message_RegistedPID,            //ほんたい登録しました

    VIEWMODE_Message_ResetRateConfirm,       //本体変わってるのでリセットされますがよろしいですか？
    VIEWMODE_Message_ResetRateWait,          //リセットしています
    VIEWMODE_Message_ResetRate,              //リセットしました

    VIEWMODE_MAX,
    VIEWMODE_INVALID
  };
  void  SetViewMode(const ViewMode viewMode);


protected:
  /*  implement app::tool::GeneralLowerView::IEventHandler  */
  virtual void    GeneralLowerView_OnEvent(app::tool::GeneralLowerView* pSender, const app::tool::GeneralLowerView::IEventHandler::EventCode eventCode);

  /*  implement MessageMenu::IEventHandler  */
  virtual void    MessageMenuView_OnEvent(const MessageMenu::IEventHandler::EventCode eventCode);

  /*  implement app::tool::IResourceProvider  */
  virtual void*   IResourceProvider_OnResourceRequest(const gfl2::fs::ArcFile::ARCID arcID, const gfl2::fs::ArcFile::ARCDATID arcDatID);


  static const u32  MESSAGEID_INVALID = UINT_MAX;
  void  SetupButtons(const bool bEnableBackButton, const bool bShowBG, const u32 messageID_0 = MESSAGEID_INVALID, const u32 messageID_1 = MESSAGEID_INVALID, const u32 messageID_2 = MESSAGEID_INVALID, const u32 messageID_3 = MESSAGEID_INVALID);
  void  Event(const IEventHandler::EventCode eventCode);
  
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   SetInputEnabled を呼び出した後にコールされます
   * @note    UIView側で、自身の子Viewのみの入力制御を行いたい場合等に使用します
   *
   * @param   isEnable 入力検出を有効化するならtrue、無効化するならfalseを指定
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnSetInputEnabled( bool isEnable );

  /*
    members
  */
  BattleSpotWork*               m_pWork;
  IEventHandler*                m_pEventHandler;
  ViewMode                      m_ViewMode;

  /*  subviews  */
  app::tool::GeneralLowerView*  m_pGeneralLowerView;
  MessageMenu*                  m_pMessageMenu;
};





} /*  namespace BattleSpot */
} /*  namespace NetApp    */
#endif  /*  #if !defined(LOWERVIEW_SELECTION_H_INCLUDED)  */
