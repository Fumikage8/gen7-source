#if !defined(LOWERVIEW_CARD_H_INCLUDED)
#define LOWERVIEW_CARD_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LowerView_Card.h
 * @date   2015/10/09 Fri. 19:15:11
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "AppLib/include/Tool/GeneralView/GeneralLowerView.h"
#include  "NetStatic/NetAppLib/include/UI/NetAppCursorView.h"
#include  "NetStatic/NetAppLib/include/UI/NetAppPokePartyUtility.h"
#include  "NetStatic/NetAppLib/include/UI/NetAppTrainerIconVisibilityControl.h"

#include  "./BattleSpotViewUtility.h"
#include  "./LowerView_QR.h"
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
class LowerView_Card
  : public    NetAppLib::UI::NetAppCursorView
  , public NetAppLib::UI::NetAppTrainerIconVisibilityControl
  , protected ViewUtility_Card
  , protected MessageMenu::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( LowerView_Card );
public:

  class IEventHandler
  {
  public:
    enum  EventCode
    {
      /*  general */
      EVENT_Back,
      EVENT_Info,
      /*  message  */
      EVENT_Message_EOM,
      EVENT_Message_Finish,
      
      
      EVENT_NONE    /**<  内部制御用  */
    };

    virtual ~IEventHandler(){}
    virtual void  LowerView_Card_OnEvent(LowerView_Card* pSender, const EventCode eventCode) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pEventHandler = pHandler;}


  LowerView_Card(BattleSpotWork* pWork);
  virtual ~LowerView_Card();

  /*  implement UIView I/F  */
  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
  virtual bool  IsDrawing(void) const;
   /*  endimplement UIView I/F  */


  enum  ViewMode
  {
    VIEWMODE_None,

    /*  ratingBattle  */
    VIEWMODE_RatingBattle_Matching,           /**<  対戦相手を探しています          */
    VIEWMODE_RatingBattle_Matched,            /**<  対戦相手がみつかりました        */
    VIEWMODE_RatingBattle_MatchingFailed,     /**<  対戦相手がみつかりません        */

    /*  freebattle  */
    VIEWMODE_FreeBattle_Matching,             /**<  対戦相手を探しています          */
    VIEWMODE_FreeBattle_Matched,              /**<  対戦相手がみつかりました        */
    VIEWMODE_FreeBattle_MatchingFailed,       /**<  対戦相手がみつかりません        */

    /*  internetMatch  */
    VIEWMODE_InternetMatch_Card,              /**<  参加中の大会です：チーム登録前  */
    VIEWMODE_InternetMatch_TeamCard,          /**<  参加中の大会です：チーム登録後  */
    VIEWMODE_InternetMatch_TournamentInfo,    /**<  大会情報（QR）                  */
    VIEWMODE_InternetMatch_Matching,          /**<  対戦相手を探しています          */
    VIEWMODE_InternetMatch_Matched,           /**<  対戦相手がみつかりました        */
    VIEWMODE_InternetMatch_MatchingFailed,    /**<  対戦相手がみつかりません        */

    /*  friendmatch  */
    VIEWMODE_FriendMatch_Card,                /**<  参加中の大会です：チーム登録前  */
    VIEWMODE_FriendMatch_TeamCard,            /**<  参加中の大会です：チーム登録後  */
    VIEWMODE_FriendMatch_TournamentInfo,      /**<  大会情報（QR）                  */
    VIEWMODE_FriendMatch_Matching,            /**<  対戦相手を探しています          */
    VIEWMODE_FriendMatch_Matched,             /**<  対戦相手がみつかりました        */
    VIEWMODE_FriendMatch_MatchingFailed,      /**<  対戦相手がみつかりません        */

    VIEWMODE_MAX,
    VIEWMODE_INVALID
  };
  void  SetViewMode(const ViewMode viewMode);
  ViewMode GetViewMode( void ){ return m_ViewMode; };


  u32 GetSelectedBGM(void) const;

  void  ResetPokeParty( void );

protected:
  /*  implement MessageMenu::IEventHandler  */
  virtual void    MessageMenuView_OnEvent(const MessageMenu::IEventHandler::EventCode eventCode);

  virtual UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );
  virtual UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 buttonID );

  enum  LayoutMode
  {
    LAYOUTMODE_None,
    LAYOUTMODE_TypeB,   /**<  マッチング：レーティング  */
    LAYOUTMODE_TypeC,   /**<  マッチング：フリー        */
    LAYOUTMODE_TypeD,   /**<  マッチング：仲間大会      */
    LAYOUTMODE_TypeE,   /**<  ?        */
    LAYOUTMODE_TypeF,   /**<  大会情報：仲間大会        */

    LAYOUTMODE_MAX,
    LAYOUTMODE_INVALID
  };
  void  SetLayoutMode(const LayoutMode layoutMode);
  void  SetCardViewMode(const bool bTeamRegistered);
  void  SetTournamentInfoEnable(const bool isEnable);
  void  SetBGMSelectEnable(const bool isEnable);
  void  SetBackEnable(const bool isEnable);


  /*  pane  */
  enum  TextBoxID
  {
    /*  typeB  */
    TEXTBOX_TypeB_LabelRate,      /**<  レート    */
    TEXTBOX_TypeB_LabelMatches,   /**<  対戦回数  */
    TEXTBOX_TypeB_LabelWins,      /**<  勝ち      */
    TEXTBOX_TypeB_LabelLoses,     /**<  負け      */
    TEXTBOX_TypeB_Rate,           /**<  レート    */
    TEXTBOX_TypeB_Matches,        /**<  対戦回数  */
    TEXTBOX_TypeB_Wins,           /**<  勝ち      */
    TEXTBOX_TypeB_Loses,          /**<  負け      */
    TEXTBOX_TypeB_TeamName,       /**<  チーム名  */
    TEXTBOX_TypeB_PMLevel00,      /**<  ポケモン00レベル    */
    TEXTBOX_TypeB_PMLevel01,      /**<  ポケモン01レベル    */
    TEXTBOX_TypeB_PMLevel02,      /**<  ポケモン02レベル    */
    TEXTBOX_TypeB_PMLevel03,      /**<  ポケモン03レベル    */
    TEXTBOX_TypeB_PMLevel04,      /**<  ポケモン04レベル    */
    TEXTBOX_TypeB_PMLevel05,      /**<  ポケモン05レベル    */

    /*  typeC  */
    TEXTBOX_TypeC_TeamName,       /**<  チーム名  */
    TEXTBOX_TypeC_PMLevel00,      /**<  ポケモン00レベル    */
    TEXTBOX_TypeC_PMLevel01,      /**<  ポケモン01レベル    */
    TEXTBOX_TypeC_PMLevel02,      /**<  ポケモン02レベル    */
    TEXTBOX_TypeC_PMLevel03,      /**<  ポケモン03レベル    */
    TEXTBOX_TypeC_PMLevel04,      /**<  ポケモン04レベル    */
    TEXTBOX_TypeC_PMLevel05,      /**<  ポケモン05レベル    */

    /*  typeD  */
    TEXTBOX_TypeD_LabelRate,      /**<  レート    */
    TEXTBOX_TypeD_LabelMatches,   /**<  対戦回数  */
    TEXTBOX_TypeD_Rate,           /**<  レート    */
    TEXTBOX_TypeD_Matches,        /**<  対戦回数  */
    TEXTBOX_TypeD_TeamName,       /**<  チーム名  */
    TEXTBOX_TypeD_PMLevel00,      /**<  ポケモン00レベル    */
    TEXTBOX_TypeD_PMLevel01,      /**<  ポケモン01レベル    */
    TEXTBOX_TypeD_PMLevel02,      /**<  ポケモン02レベル    */
    TEXTBOX_TypeD_PMLevel03,      /**<  ポケモン03レベル    */
    TEXTBOX_TypeD_PMLevel04,      /**<  ポケモン04レベル    */
    TEXTBOX_TypeD_PMLevel05,      /**<  ポケモン05レベル    */
    
    /*  typeE  */
    TEXTBOX_TypeE_1LineMessage,   /**<  一行メッセージ      */
    TEXTBOX_TypeE_LabelRate,      /**<  レート    */
    TEXTBOX_TypeE_LabelMatches,   /**<  対戦回数  */
    TEXTBOX_TypeE_LabelWins,      /**<  勝ち      */
    TEXTBOX_TypeE_LabelLoses,     /**<  負け      */
    TEXTBOX_TypeE_Rate,           /**<  レート    */
    TEXTBOX_TypeE_Matches,        /**<  対戦回数  */
    TEXTBOX_TypeE_Wins,           /**<  勝ち      */
    TEXTBOX_TypeE_Loses,          /**<  負け      */
    TEXTBOX_TypeE_TeamName,       /**<  チーム名  */
    TEXTBOX_TypeE_PMLevel00,      /**<  ポケモン00レベル    */
    TEXTBOX_TypeE_PMLevel01,      /**<  ポケモン01レベル    */
    TEXTBOX_TypeE_PMLevel02,      /**<  ポケモン02レベル    */
    TEXTBOX_TypeE_PMLevel03,      /**<  ポケモン03レベル    */
    TEXTBOX_TypeE_PMLevel04,      /**<  ポケモン04レベル    */
    TEXTBOX_TypeE_PMLevel05,      /**<  ポケモン05レベル    */

    /*  typeF  */
    TEXTBOX_TypeF_1LineMessage,   /**<  一行メッセージ      */
    TEXTBOX_TypeF_LabelMatches,   /**<  対戦回数  */
    TEXTBOX_TypeF_Matches,        /**<  対戦回数  */

    TEXTBOX_TypeF_TeamName,       /**<  チーム名  */
    TEXTBOX_TypeF_Information,    /**<  パーティ未登録時のInfo  */
    TEXTBOX_TypeF_PMLevel00,      /**<  ポケモン00レベル    */
    TEXTBOX_TypeF_PMLevel01,      /**<  ポケモン01レベル    */
    TEXTBOX_TypeF_PMLevel02,      /**<  ポケモン02レベル    */
    TEXTBOX_TypeF_PMLevel03,      /**<  ポケモン03レベル    */
    TEXTBOX_TypeF_PMLevel04,      /**<  ポケモン04レベル    */
    TEXTBOX_TypeF_PMLevel05,      /**<  ポケモン05レベル    */

    /*  others  */
    TEXTBOX_TournamentInfoButton, /**<  大会の情報          */
    TEXTBOX_BGM,                  /**<  BGM                 */


    TEXTBOX_MAX
  };

  enum  PaneID
  {
    PANE_TypeB,
    PANE_TypeC,
    PANE_TypeD,
    PANE_TypeE,
    PANE_TypeF,
    PANE_TypeF_Team,
    PANE_TypeF_Information,
    PANE_Info,
    PANE_BGM,
    PANE_Back,
    PANE_TrainerIconSyncVisible,

    PANE_MAX
  };

  

  void  SetQRView(const bool isQRVIsible);


  static const u32  MESSAGEID_INVALID = UINT_MAX;
  void  Event(const IEventHandler::EventCode eventCode);

  void  SelectBGM(const s32 indexOffset);
  u32   Get1LiveMessageIDFromEntryWork(const bool isInternetMatch);

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
  BattleSpotWork*                               m_pWork;
  IEventHandler*                                m_pEventHandler;
  ViewMode                                      m_ViewMode;
  LayoutMode                                    m_LayoutMode;

  /*  panes  */
  gfl2::lyt::LytTextBox*                        m_pTextBoxes[TEXTBOX_MAX];
  gfl2::lyt::LytPane*                           m_pPanes[PANE_MAX];


  /*  subviews  */
  MessageMenu*                                  m_pMessageMenu;
  LowerView_QR*                                 m_pQRView;

  /*  others  */
  mutable NetAppLib::UI::NetAppPokePartyUtility m_PokePartyUtility;
  gfl2::str::StrBuf                             m_LevelStringSource;
  u32                                           m_BGMIndex;
};





} /*  namespace BattleSpot */
} /*  namespace NetApp    */
#endif  /*  #if !defined(LOWERVIEW_CARD_H_INCLUDED)  */
