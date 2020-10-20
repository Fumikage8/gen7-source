
//==============================================================================
/**
 * @file   LowerView_Selection.cpp
 * @date   2015/10/02 Fri. 15:00:44
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../BattleSpotResource.h"
#include  "../System/BattleSpotWork.h"

#include  "./LowerView_Selection.h"




// =============================================================================
/**/
//==============================================================================

#if 0
  case VIEWMODE_Message_RetireConfirm://デフォルトNoにする選択肢 
  case VIEWMODE_Message_RetireConfirm2: 
  case VIEWMODE_Message_RetireConfirm_friend: 
  case VIEWMODE_Message_RetireConfirm2_friend: 
  case VIEWMODE_Message_RetireConfirm_no_reg: 
  case VIEWMODE_Message_ResetRateConfirm:
#endif
  
namespace
{
  static const u32  CURINDEX_NOSET  = UINT_MAX;

#define BUTTONDEF(_messageID, _eventCode) {(_messageID), NetApp::BattleSpot::LowerView_Selection::IEventHandler::_eventCode}
#define BUTTONDEF_NOUSE                   BUTTONDEF(UINT_MAX, EVENT_NONE)
  static const struct ViewModeDefinition
  {
    NetApp::BattleSpot::MessageMenu::Mode                               messageMode;
    struct
    {
      u32                                                               messageID;
      NetApp::BattleSpot::LowerView_Selection::IEventHandler::EventCode eventCode;
    }buttonDef[4];
    bool                                                                bEnableBackButton;
    bool                                                                bShowBG;
    bool                                                                bAppearButtonOnEOM;
    u32                                                                 defaultCursorIndex;   /*  選択肢表示時、最初にカーソルがある位置  */
  }viewModeDefinitions[NetApp::BattleSpot::LowerView_Selection::VIEWMODE_MAX] = {

    /*
      welcome
    */
    //    VIEWMODE_Welcome_SelectMode,              /**<  モードを選んでください      */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Welcome_SelectMode,
      {
        BUTTONDEF(msg_bs_sel_01,  EVENT_WelCome_FreeBattle),
        BUTTONDEF(msg_bs_sel_02,  EVENT_WelCome_RatingBattle),
        BUTTONDEF(msg_bs_sel_03,  EVENT_WelCome_InternetMatch),
        BUTTONDEF(msg_bs_sel_04,  EVENT_WelCome_FriendMatch),
      },
      true,               /*  bool  bEnableBackButton;  */
      false,              /*  bool  bShowBG;            */
      false,              /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    /*
      freebattle
    */
    //    VIEWMODE_FreeBattle_SelectRule,           /**<  どのルールで戦いますか？    */
    {
      NetApp::BattleSpot::MessageMenu::MODE_FreeBattle_SelectRule,
      {
        BUTTONDEF(msg_bs_sel_05,  EVENT_FreeBattle_Rule_SingleBattle),
        BUTTONDEF(msg_bs_sel_06,  EVENT_FreeBattle_Rule_DoubleBattle),
        BUTTONDEF(msg_bs_sel_07,  EVENT_FreeBattle_Rule_BattleRoyal),
        BUTTONDEF_NOUSE,
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_FreeBattle_PokemonRule,          /**<  特別なポケモンあり/なし     */
    {
      NetApp::BattleSpot::MessageMenu::MODE_FreeBattle_PokemonRule,
      {
        BUTTONDEF(msg_bs_sel_20,  EVENT_FreeBattle_Pokemon_SpecialPokemonEnable),
        BUTTONDEF(msg_bs_sel_21,  EVENT_FreeBattle_Pokemon_SpecialPokemonDisable),
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },


    /*
      ratingBattle
    */
    //    VIEWMODE_RatingBattle_SelectRule,         /**<  どのルールで戦いますか？    */
    {
      NetApp::BattleSpot::MessageMenu::MODE_RatingBattle_SelectRule,
      {
        BUTTONDEF(msg_bs_sel_05,  EVENT_RatingBattle_Rule_SingleBattle),
        BUTTONDEF(msg_bs_sel_06,  EVENT_RatingBattle_Rule_DoubleBattle),
        BUTTONDEF(msg_bs_sel_08,  EVENT_RatingBattle_Rule_SpecialBattle),
        BUTTONDEF(msg_bs_sel_23,  EVENT_RatingBattle_Rule_WCSBattle),
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },



    //------------------------------------------------------------------
    /*
      internetMatch
      */
    //------------------------------------------------------------------
    //    VIEWMODE_InternetMatch_Work0,               /**<  参加ワークの値に応じたトップメニュー  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_InternetMatch_SelectMode,
      {
        BUTTONDEF(msg_bs_sel_12,  EVENT_InternetMatch_Entry),   /*  大会に参加する  */
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_InternetMatch_Work1,               /**<  参加ワークの値に応じたトップメニュー  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_InternetMatch_SelectMode,
      {
        BUTTONDEF(msg_bs_sel_12,  EVENT_InternetMatch_Entry),   /*  大会に参加する  */
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_InternetMatch_Work2,               /**<  参加ワークの値に応じたトップメニュー  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_InternetMatch_SelectMode,
      {
        BUTTONDEF(msg_bs_sel_13,  EVENT_InternetMatch_Match),       /*  対戦する        */
        BUTTONDEF(msg_bs_sel_14,  EVENT_InternetMatch_WatchCard),   /*  選手証を見る    */
        BUTTONDEF(msg_bs_sel_15,  EVENT_InternetMatch_Retire),      /*  参加を解除する    */
        BUTTONDEF_NOUSE,
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_InternetMatch_Work3_TeamLocked,    /**<  参加ワークの値に応じたトップメニュー  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_InternetMatch_SelectMode,
      {
        BUTTONDEF(msg_bs_sel_13,  EVENT_InternetMatch_Match),       /*  対戦する        */
        BUTTONDEF(msg_bs_sel_14,  EVENT_InternetMatch_WatchCard),   /*  選手証を見る    */
        BUTTONDEF(msg_bs_sel_15,  EVENT_InternetMatch_Retire),      /*  参加を解除する    */
        BUTTONDEF_NOUSE,
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_InternetMatch_Work3_TeamUnlocked,  /**<  参加ワークの値に応じたトップメニュー  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_InternetMatch_SelectMode,
      {
        BUTTONDEF(msg_bs_sel_12,  EVENT_InternetMatch_Entry),                 /*  大会に参加する  */
        BUTTONDEF_NOUSE,    /*  前回の大会      */
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_InternetMatch_Work4_TeamLocked,    /**<  参加ワークの値に応じたトップメニュー  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_InternetMatch_SelectMode,
      {
        BUTTONDEF(msg_bs_sel_13,  EVENT_InternetMatch_Match),       /*  対戦する        */
        BUTTONDEF(msg_bs_sel_14,  EVENT_InternetMatch_WatchCard),   /*  選手証を見る    */
        BUTTONDEF(msg_bs_sel_15,  EVENT_InternetMatch_Retire),      /*  参加を解除する    */
        BUTTONDEF_NOUSE,
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_InternetMatch_Work4_TeamUnlocked,  /**<  参加ワークの値に応じたトップメニュー  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_InternetMatch_SelectMode,
      {
        BUTTONDEF(msg_bs_sel_12,  EVENT_InternetMatch_Entry),                 /*  大会に参加する  */
        BUTTONDEF_NOUSE,    /*  前回の大会      */
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_InternetMatch_Work5,               /**<  参加ワークの値に応じたトップメニュー  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_InternetMatch_SelectMode,
      {
        BUTTONDEF(msg_bs_sel_12,  EVENT_InternetMatch_Entry),                 /*  大会に参加する  */
        BUTTONDEF_NOUSE,    /*  前回の大会      */
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_InternetMatch_Work6,               /**<  参加ワークの値に応じたトップメニュー  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_InternetMatch_SelectMode,
      {
        BUTTONDEF(msg_bs_sel_12,  EVENT_InternetMatch_Entry),                 /*  大会に参加する  */
        BUTTONDEF_NOUSE,    /*  前回の大会      */
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_InternetMatch_Work7,               /**<  参加ワークの値に応じたトップメニュー  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_InternetMatch_SelectMode,
      {
        BUTTONDEF(msg_bs_sel_12,  EVENT_InternetMatch_Entry),                 /*  大会に参加する  */
        BUTTONDEF_NOUSE,    /*  前回の大会      */
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //------------------------------------------------------------------
    /*
      friendMatch
      */
    //------------------------------------------------------------------
    //    VIEWMODE_FriendMatch_Work0,               /**<  参加ワークの値に応じたトップメニュー  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_InternetMatch_SelectMode,
      {
        BUTTONDEF(msg_bs_sel_22,  EVENT_Friendmatch_QRRead),   /*  QRを読み込む  */
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_FriendMatch_Work0_1,               /**<  参加ワークの値に応じたトップメニュー  */
    {
    NetApp::BattleSpot::MessageMenu::MODE_InternetMatch_SelectMode,
    {
      BUTTONDEF(msg_bs_sel_22,  EVENT_Friendmatch_QRRead),   /*  QRを読み込む  */
        BUTTONDEF(msg_bs_sel_15,  EVENT_FriendMatch_Retire),   /*  リタイアする  */
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
    },
    true,   /*  bool  bEnableBackButton;  */
    false,  /*  bool  bShowBG;            */
    false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_FriendMatch_Work1,               /**<  参加ワークの値に応じたトップメニュー  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_InternetMatch_SelectMode,
      {
        BUTTONDEF(msg_bs_sel_22,  EVENT_Friendmatch_QRRead),   /*  QRを読み込む  */
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_FriendMatch_Work2,               /**<  参加ワークの値に応じたトップメニュー  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_InternetMatch_SelectMode,
      {
        BUTTONDEF(msg_bs_sel_13,  EVENT_FriendMatch_Match),       /*  対戦する        */
        BUTTONDEF(msg_bs_sel_14,  EVENT_FriendMatch_WatchCard),   /*  選手証を見る    */
        BUTTONDEF(msg_bs_sel_15,  EVENT_FriendMatch_Retire),      /*  参加を解除する    */
        BUTTONDEF_NOUSE,
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_FriendMatch_Work3_TeamLocked,    /**<  参加ワークの値に応じたトップメニュー  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_InternetMatch_SelectMode,
      {
        BUTTONDEF(msg_bs_sel_13,  EVENT_FriendMatch_Match),       /*  対戦する        */
        BUTTONDEF(msg_bs_sel_14,  EVENT_FriendMatch_WatchCard),   /*  選手証を見る    */
        BUTTONDEF(msg_bs_sel_15,  EVENT_FriendMatch_Retire),      /*  参加を解除する    */
        BUTTONDEF_NOUSE,
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_FriendMatch_Work3_TeamUnlocked,  /**<  参加ワークの値に応じたトップメニュー  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_InternetMatch_SelectMode,
      {
        BUTTONDEF(msg_bs_sel_22,  EVENT_Friendmatch_QRRead),                /*  QRを読み込む  */
        BUTTONDEF_NOUSE,    /*  前回の大会      */
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_FriendMatch_Work4_TeamLocked,    /**<  参加ワークの値に応じたトップメニュー  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_InternetMatch_SelectMode,
      {
        BUTTONDEF(msg_bs_sel_13,  EVENT_FriendMatch_Match),           /*  対戦する        */
        BUTTONDEF(msg_bs_sel_14,  EVENT_FriendMatch_WatchCard),       /*  選手証を見る    */
        BUTTONDEF(msg_bs_sel_15,  EVENT_FriendMatch_Retire),          /*  参加を解除する    */
        BUTTONDEF_NOUSE,
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_FriendMatch_Work4_TeamUnlocked,  /**<  参加ワークの値に応じたトップメニュー  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_InternetMatch_SelectMode,
      {
        BUTTONDEF(msg_bs_sel_22,  EVENT_Friendmatch_QRRead),                /*  QRを読み込む  */
        BUTTONDEF_NOUSE,    /*  前回の大会      */
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_FriendMatch_Work5,               /**<  参加ワークの値に応じたトップメニュー  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_InternetMatch_SelectMode,
      {
        BUTTONDEF(msg_bs_sel_22,  EVENT_Friendmatch_QRRead),                /*  QRを読み込む  */
        BUTTONDEF_NOUSE,    /*  前回の大会      */
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_FriendMatch_Work6,               /**<  参加ワークの値に応じたトップメニュー  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_InternetMatch_SelectMode,
      {
        BUTTONDEF(msg_bs_sel_22,  EVENT_Friendmatch_QRRead),                /*  QRを読み込む  */
        BUTTONDEF_NOUSE,    /*  前回の大会      */
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      true,   /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },



    //------------------------------------------------------------------
    /**
        message
      */
    //------------------------------------------------------------------
    //    VIEWMODE_Message_Connecting,          /**<  通信しています                */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_Connecting,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_Message_SDCard,              /**<  SDカードを確認しています                              */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_SDCard,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_Message_RegulationUpdated,   /**<  レギュレーションが更新されました                      */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_RegulationUpdated,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_Message_RegulationRepaired,  /**<  レギュレーションを修復しました                        */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_RegulationRepaired,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_Message_Regulation_SDFull,   /**<  SDカードがいっぱいでレギュレーションを保存できません  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_Regulation_SDFull,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_Message_ConnectionError,     /**<  通信エラーが発生しました                              */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_ConnectionError,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_Checking,              /**<  大会の状態を確認しています          */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_Checking,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_HowToEntry,            /**<  大会に参加するには                  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_HowToEntry,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_NoTournament,          /**<  参加できる大会はありません          */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_NoTournament,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_InvalidCupID,          /**<  ことなるたいかいです          */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_InvalidCupID,
      {
        BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_SeasonIsOver,          /**<  シーズンは終了しました　*/
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_SeasonIsOver,
      {
        BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_TournamentIsOver,      /**<  大会は終了しました                  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_TournamentIsOver,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_TournamentIsOverUnlock,      /**<  大会は終了しました Unlock                 */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_TournamentIsOverUnlock,
      {
        BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_CannotEntry,           /**<  この大会には参加できません          */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_CannotEntry,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_Unregistered,          /**<  大会の参加を解除しました            */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_Unregistered,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_DownloadingCard,       /**<  選手証をダウンロードしています      */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_DownloadingCard,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_DownloadedCard,        /**<  選手証のダウンロードが完了しました  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_DownloadedCard,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_TournamentNotStarted,  /**<  大会はまだ始まっていません          */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_TournamentNotStarted,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_Retired,               /**<  途中で参加を解除しました            */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_Retired,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_GameSync,              /**<  バトルチームをロックしてゲームシンクを行いました  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_GameSync,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_DifferentConsole,      /**<  登録したときの本体と異なります      */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_DifferentConsole,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_NotAvailableTime,      /**<  対戦できない時間です                      */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_NotAvailableTime,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_PrescribedMatches,     /**<  対戦回数が決められた回数に達しました      */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_PrescribedMatches,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_QRRead,                /**<  QRコードを撮影するためカメラを起動します  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_QRRead,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_GetCard,               /**<  デジタル選手証を取得しました              */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_GetCard,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },
    //    VIEWMODE_Message_RetireConfirm,         /**<  リタイアとなります 参加を解除しますか？   */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_RetireConfirm,
      {
        BUTTONDEF(msg_bs_sel_18,  EVENT_Retire1_Yes),   /*  はい  */
        BUTTONDEF(msg_bs_sel_19,  EVENT_Retire1_No),    /*  いいえ */
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      true,   /*  bool  bAppearButtonOnEOM; */
      1,      /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_RetireConfirm2,        /**<  本当によろしいですか？                    */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_RetireConfirm2,
      {
        BUTTONDEF(msg_bs_sel_18,  EVENT_Retire2_Yes),   /*  はい  */
        BUTTONDEF(msg_bs_sel_19,  EVENT_Retire2_No),    /*  いいえ */
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      true,   /*  bool  bAppearButtonOnEOM; */
      1,      /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_RetireConfirm_friend,        /**<  リタイアとなります 参加を解除しますか？ （仲間大会）           */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_RetireConfirm_friend,
      {
        BUTTONDEF(msg_bs_sel_18,  EVENT_Retire1_Yes),   /*  はい  */
          BUTTONDEF(msg_bs_sel_19,  EVENT_Retire1_No),    /*  いいえ */
          BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      true,   /*  bool  bAppearButtonOnEOM; */
      1,      /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_RetireConfirm2_friend,        /**<  本当によろしいですか？（大会名なし）                   */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_RetireConfirm2_friend,
      {
        BUTTONDEF(msg_bs_sel_18,  EVENT_Retire2_Yes),   /*  はい  */
          BUTTONDEF(msg_bs_sel_19,  EVENT_Retire2_No),    /*  いいえ */
          BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      true,   /*  bool  bAppearButtonOnEOM; */
      1,      /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_RetireConfirm_no_reg,        /**<  レギュレーションがないのでリタイアとなります 参加を解除しますか？         */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_RetireConfirm_no_reg,
      {
        BUTTONDEF(msg_bs_sel_18,  EVENT_Retire4_Yes),   /*  はい  */
          BUTTONDEF(msg_bs_sel_19,  EVENT_Retire4_No),    /*  いいえ */
          BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      true,   /*  bool  bAppearButtonOnEOM; */
      1,      /*  u32   defaultCursorIndex; */
    },

    

    //    VIEWMODE_Message_RetireConfirm_friend_NewCup,        
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_Retire_NewCup,
      {
        BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,   /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    /*
      others
    */
    //    VIEWMODE_BattleVideoAutoUploadConfirm,    /**<  バトルビデオの自動送信を許可しますか？  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_BattleVideoAutoUploadConfirm,
      {
        BUTTONDEF(msg_bs_sel_18,  EVENT_BattleVideoAutoUpload_Yes),   /*  はい  */
        BUTTONDEF(msg_bs_sel_19,  EVENT_BattleVideoAutoUpload_No),    /*  いいえ */
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_ContinueConfirm,    /**<  たいせんを　つづけますか？  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_ContinueConfirm,
      {
        BUTTONDEF(msg_bs_sel_09,  EVENT_Continue_Next),   /*  つづける  */
        BUTTONDEF(msg_bs_sel_10,  EVENT_Continue_Change),    /* チームをかえる */
        BUTTONDEF(msg_bs_sel_11,  EVENT_Continue_Exit),    /*  やめる */
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_ContinueConfirmRate,    /**<  たいせんを　つづけますか？レート  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_ContinueConfirmRate,
      {
        BUTTONDEF(msg_bs_sel_09,  EVENT_Continue_Next),   /*  つづける  */
          BUTTONDEF(msg_bs_sel_10,  EVENT_Continue_Change),    /* チームをかえる */
          BUTTONDEF(msg_bs_sel_11,  EVENT_Continue_Exit),    /*  やめる */
          BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      true,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //VIEWMODE_Message_ContinueConfirmCup
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_ContinueConfirm,
      {
        BUTTONDEF(msg_bs_sel_09,  EVENT_Continue_Next),   /*  つづける  */
          BUTTONDEF(msg_bs_sel_11,  EVENT_Continue_Exit),    /*  やめる */
          BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_PGLNoRegist,    /**<  PGLとうろくしてください  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_PGLNoRegisted,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_RegistPIDConfirm,    /**<  ほんたい登録しますか？  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_RegistPIDConfirm,
      {
        BUTTONDEF(msg_bs_sel_18,  EVENT_RegistPID_Yes),   /*  はい  */
        BUTTONDEF(msg_bs_sel_19,  EVENT_RegistPID_No),    /* いいえ */
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      true,   /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_RegistPIDWait,    /**<  ほんたい登録しています  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_RegistPIDWait,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_RegistedPID,    /**<  ほんたい登録しました  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_RegistedPID,
      {
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
        BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_ResetRateConfirm,    /**<  リセットしますか？  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_ResetRateConfirm,
      {
        BUTTONDEF(msg_bs_sel_18,  EVENT_ResetRate_Yes),   /*  はい  */
          BUTTONDEF(msg_bs_sel_19,  EVENT_ResetRate_No),    /* いいえ */
          BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      true,   /*  bool  bAppearButtonOnEOM; */
      1,      /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_ResetRateWait,    /**<  リセットしています  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_ResetRateWait,
      {
        BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

    //    VIEWMODE_Message_ResetRate,    /**<  リセットしました  */
    {
      NetApp::BattleSpot::MessageMenu::MODE_Message_ResetRate,
      {
        BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
          BUTTONDEF_NOUSE,
      },
      false,  /*  bool  bEnableBackButton;  */
      false,  /*  bool  bShowBG;            */
      false,  /*  bool  bAppearButtonOnEOM; */
      CURINDEX_NOSET,     /*  u32   defaultCursorIndex; */
    },

  };
#undef BUTTONDEF_NOUSE
#undef BUTTONDEF
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
LowerView_Selection::LowerView_Selection(BattleSpotWork* pWork)
  : NetAppLib::UI::NetAppEmptyView(pWork)
  , m_pWork(pWork)
  , m_pEventHandler(NULL)
  , m_ViewMode(VIEWMODE_INVALID)
  , m_pGeneralLowerView(NULL)
  , m_pMessageMenu(NULL)

{
  gfl2::heap::HeapBase* pHeapBase = m_pWork->GetDeviceHeap();

  /*  generalLowerView  */
  {
    m_pGeneralLowerView = GFL_NEW(pHeapBase)  app::tool::GeneralLowerView(m_pWork->GetAppHeap(), m_pWork->GetAppRenderingManager());
    m_pGeneralLowerView->SetResourceProvider(this);
    m_pGeneralLowerView->ObserveSetup();
    m_pGeneralLowerView->SetEventHandler(this);
    AddSubView(m_pGeneralLowerView);
  }

  /*  messageMenu  */
  {
    m_pMessageMenu  = GFL_NEW(pHeapBase)  MessageMenu(pWork);
    m_pMessageMenu->Setup(RESOURCE_LayoutCommonWindow, RESOURCE_LayoutCommonMenuCursor, m_pWork->GetMessageUtility());
    m_pMessageMenu->SetEventHandler(this);

    AddSubView(m_pMessageMenu);
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
LowerView_Selection::~LowerView_Selection()
{
  if(m_pGeneralLowerView) m_pGeneralLowerView->RemoveFromSuperView();
  if(m_pMessageMenu)      m_pMessageMenu->RemoveFromSuperView();

  GFL_SAFE_DELETE(m_pMessageMenu);
  GFL_SAFE_DELETE(m_pGeneralLowerView);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool LowerView_Selection::IsDrawing(void) const
{
  return  (
    NetAppLib::UI::NetAppEmptyView::IsDrawing()
    ||  m_pGeneralLowerView->IsDrawing()
    ||  m_pMessageMenu->IsDrawing()
  );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
/*
void  LowerView_Selection::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pWork->GetAppRenderingManager(), displayNo, 0);
}
*/


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Selection::SetViewMode(const ViewMode viewMode)
{
  const ViewModeDefinition&  rModeDef = viewModeDefinitions[viewMode];

  m_pMessageMenu->HideMessage();

  m_ViewMode  = viewMode;

  if(rModeDef.bAppearButtonOnEOM)
  {
    SetupButtons(
      false,
      rModeDef.bShowBG,
      MESSAGEID_INVALID,
      MESSAGEID_INVALID,
      MESSAGEID_INVALID,
      MESSAGEID_INVALID
    );
  }
  else
  {
    SetupButtons(
      rModeDef.bEnableBackButton,
      rModeDef.bShowBG,
      rModeDef.buttonDef[0].messageID,
      rModeDef.buttonDef[1].messageID,
      rModeDef.buttonDef[2].messageID,
      rModeDef.buttonDef[3].messageID
    );
  }

  m_pMessageMenu->Show(rModeDef.messageMode);
  //カーソル位置設定
  switch( viewMode )
  {
  case VIEWMODE_Welcome_SelectMode://記憶する選択肢
    m_pGeneralLowerView->CursorMoveTo( m_pWork->m_pAppParam->m_topMenuCurorIndex );
    break;
  case VIEWMODE_FreeBattle_SelectRule:
    m_pGeneralLowerView->CursorMoveTo( m_pWork->m_pAppParam->m_freeRuleCurorIndex );
    break;
  case VIEWMODE_FreeBattle_PokemonRule:
    m_pGeneralLowerView->CursorMoveTo( m_pWork->m_pAppParam->m_freeLegendCurorIndex );
    break;
  case VIEWMODE_RatingBattle_SelectRule:
    m_pGeneralLowerView->CursorMoveTo( m_pWork->m_pAppParam->m_rateRuleCurorIndex );
    break;
  case VIEWMODE_Message_RetireConfirm://デフォルトNoにする選択肢
  case VIEWMODE_Message_RetireConfirm2:
  case VIEWMODE_Message_RetireConfirm_friend:
  case VIEWMODE_Message_RetireConfirm2_friend:
  case VIEWMODE_Message_RetireConfirm_no_reg:
  case VIEWMODE_Message_ResetRateConfirm:
    m_pGeneralLowerView->CursorMoveTo(1);
    break;
  default:
    m_pGeneralLowerView->CursorMoveTo(0);
    break;
  }
}



//------------------------------------------------------------------
/**
  *  @brief    GeneralLowerViewからの通知ハンドラ
  */
//------------------------------------------------------------------
void  LowerView_Selection::GeneralLowerView_OnEvent(app::tool::GeneralLowerView* pSender, const app::tool::GeneralLowerView::IEventHandler::EventCode eventCode)
{
  LowerView_Selection::IEventHandler::EventCode mappedEventCode = LowerView_Selection::IEventHandler::EVENT_NONE;

  switch(eventCode)
  {
    case  app::tool::GeneralLowerView::IEventHandler::EVENT_OnButton0:    mappedEventCode = viewModeDefinitions[m_ViewMode].buttonDef[0].eventCode; break;
    case  app::tool::GeneralLowerView::IEventHandler::EVENT_OnButton1:    mappedEventCode = viewModeDefinitions[m_ViewMode].buttonDef[1].eventCode; break;
    case  app::tool::GeneralLowerView::IEventHandler::EVENT_OnButton2:    mappedEventCode = viewModeDefinitions[m_ViewMode].buttonDef[2].eventCode; break;
    case  app::tool::GeneralLowerView::IEventHandler::EVENT_OnButton3:    mappedEventCode = viewModeDefinitions[m_ViewMode].buttonDef[3].eventCode; break;
    case  app::tool::GeneralLowerView::IEventHandler::EVENT_OnButtonBack: mappedEventCode = LowerView_Selection::IEventHandler::EVENT_Back;         break;
  }

  if(mappedEventCode != LowerView_Selection::IEventHandler::EVENT_NONE) Event(mappedEventCode);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
/*  implement MessageMenu::IEventHandler  */
void    LowerView_Selection::MessageMenuView_OnEvent(const MessageMenu::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  MessageMenu::IEventHandler::EVENT_OnEOM:
    {
      const ViewModeDefinition&  rModeDef = viewModeDefinitions[m_ViewMode];

      if(rModeDef.bAppearButtonOnEOM)
      {
        SetupButtons(
          rModeDef.bEnableBackButton,
          rModeDef.bShowBG,
          rModeDef.buttonDef[0].messageID,
          rModeDef.buttonDef[1].messageID,
          rModeDef.buttonDef[2].messageID,
          rModeDef.buttonDef[3].messageID
        );
        /*  カーソル位置  */
        if(rModeDef.defaultCursorIndex != CURINDEX_NOSET)
        {
          m_pGeneralLowerView->CursorMoveTo(rModeDef.defaultCursorIndex);
        }
      }
      Event(LowerView_Selection::IEventHandler::EVENT_Message_EOM);
    }
      break;   /**<  メッセージ表示が終端に到達                                  */
    case  MessageMenu::IEventHandler::EVENT_OnFinish:  Event(LowerView_Selection::IEventHandler::EVENT_Message_Finish);   break;   /**<  EOM後、ユーザの入力まちなどまでが完了した                   */
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
/*  implement app::tool::IResourceProvider  */
void*   LowerView_Selection::IResourceProvider_OnResourceRequest(const gfl2::fs::ArcFile::ARCID arcID, const gfl2::fs::ArcFile::ARCDATID arcDatID)
{
  return m_pWork->GetResourceManager()->GetResourceBuffer(arcID, arcDatID);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Selection::SetupButtons(const bool bEnableBackButton, const bool bShowBG, const u32 messageID_0, const u32 messageID_1, const u32 messageID_2, const u32 messageID_3)
{
  u32                                   numOfButtons  = 0;
  NetAppLib::Message::MessageUtility*   pMessageUtil  = m_pWork->GetMessageUtility();


  {
    const struct
    {
      app::tool::GeneralLowerView::TextBoxID  textboxID;
      u32                                     messageID;
    }textTable[]  = {
      {app::tool::GeneralLowerView::TEXTBOX_Button0,  messageID_0},
      {app::tool::GeneralLowerView::TEXTBOX_Button1,  messageID_1},
      {app::tool::GeneralLowerView::TEXTBOX_Button2,  messageID_2},
      {app::tool::GeneralLowerView::TEXTBOX_Button3,  messageID_3},
    };
  
    for(u32 index = 0; index < GFL_NELEMS(textTable); ++index)
    {
      if(textTable[index].messageID == MESSAGEID_INVALID)
      {
        break;
      }
      else
      {
        m_pGeneralLowerView->SetTextBoxString(textTable[index].textboxID, &pMessageUtil->GetString(BATTLESPOT_MESSAGE_ARCDAT, textTable[index].messageID));    
        ++numOfButtons;
      }
    }
  }

  {
    app::tool::GeneralLowerView::LayoutMode   layoutMode;

    switch(numOfButtons)
    {
      case  1:  layoutMode  = app::tool::GeneralLowerView::LAYOUTMODE_1Button_WithMessage;  break;
      case  2:  layoutMode  = app::tool::GeneralLowerView::LAYOUTMODE_2Buttons_WithMessage; break;
      case  3:  layoutMode  = app::tool::GeneralLowerView::LAYOUTMODE_3Buttons_WithMessage; break;
      case  4:  layoutMode  = app::tool::GeneralLowerView::LAYOUTMODE_4Buttons_WithMessage; break;
      default:  layoutMode  = app::tool::GeneralLowerView::LAYOUTMODE_NoButton_WithMessage; break;
    }
    m_pGeneralLowerView->SetLayoutMode(layoutMode, bEnableBackButton, bShowBG);
  }

}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Selection::Event(const IEventHandler::EventCode eventCode)
{
  if(m_pEventHandler)
  {
    m_pEventHandler->LowerView_Selection_OnEvent(this, eventCode);
  }
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   SetInputEnabled を呼び出した後にコールされます
  * @note    UIView側で、自身の子Viewのみの入力制御を行いたい場合等に使用します
  *
  * @param   isEnable 入力検出を有効化するならtrue、無効化するならfalseを指定
  */
//--------------------------------------------------------------------------------------------
void LowerView_Selection::OnSetInputEnabled( bool isEnable )
{
  if( m_pGeneralLowerView )
  {
    m_pGeneralLowerView->SetInputEnabled( isEnable );
  }
  if( m_pMessageMenu )
  {
    m_pGeneralLowerView->SetInputEnabled( isEnable );
  }
}


} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
