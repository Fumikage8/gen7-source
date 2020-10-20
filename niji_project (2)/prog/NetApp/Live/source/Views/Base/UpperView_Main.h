#if !defined(UPPERVIEW_MAIN_H_INCLUDED)
#define UPPERVIEW_MAIN_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   UpperView_Main.h
 * @date   2015/07/14 Tue. 17:58:33
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include  "NetStatic/NetAppLib/include/UI/NetAppPokePartyUtility.h"
#include  "NetStatic/NetAppLib/include/UI/NetAppTrainerIconVisibilityControl.h"

#include  "./IDCardParts_UpperMain.h"


namespace Savedata  {
  class LiveMatchData;
}

class Regulation;

namespace NetApp  {
namespace Live    {
  class LiveWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  UpperView_Main
  *  @brief  上画面 メイン
  *  @date   2015/07/13 Mon. 19:54:16
  */
//==============================================================================
class UpperView_Main
  : public NetAppLib::System::NetApplicationViewBase
  , public NetAppLib::UI::NetAppTrainerIconVisibilityControl
{
  GFL_FORBID_COPY_AND_ASSIGN( UpperView_Main );
public:
  UpperView_Main(LiveWork* pWork);
  virtual ~UpperView_Main();

  /*  implement UIView I/F  */
  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
  virtual bool  IsDrawing(void) const;
   /*  endimplement UIView I/F  */

  enum  ViewMode
  {
    VIEWMODE_TournamentName,                /**<  大会名のみ              */
    VIEWMODE_TournamentNameWithID,          /**<  大会名 + ID情報         */
    VIEWMODE_TournamentNameWith2Line,       /**<  大会名 + 2行情報        */
    VIEWMODE_TournamentNameWithPlayerInfo,  /**<  大会名 + プレイヤー情報 */
    VIEWMODE_BattleReplay,                  /**<  バトル再生情報          */

    VIEWMODE_
  };
  void  SetViewMode(const ViewMode viewMode);


  void  UpdateTournamentInfo(void);
  void  UpdateOpponentInfo(void);
  void  UpdateIDCard(void);
  void  UpdateBattleReplayInfo(void);

protected:


  /*
    members
  */
  LiveWork*     m_pWork;

  enum  TextBoxID
  {
    TEXTBOX_Date,           /**<  大会日付            */
    TEXTBOX_LiveName1of2,   /**<  大会名1/2           */
    TEXTBOX_LiveName2of2,   /**<  大会名2/2           */
    TEXTBOX_Description,    /**<  説明文（2行）       */
    
    /*  VIEWMODE_TournamentNameWithID */
    TEXTBOX_CARD_REMAIN_MATCH,
    TEXTBOX_CARD_TOTAL_MATCH,
    TEXTBOX_CARD_WINS,
    TEXTBOX_CARD_LOSES,
    TEXTBOX_CARD_TEAMNAME,
    
    /*  VIEWMODE_TournamentNameWithPlayerInfo  */
    TEXTBOX_PlayerInfo1of4, /**<  プレイヤー詳細1/4   */
    TEXTBOX_PlayerInfo2of4, /**<  プレイヤー詳細2/4   */
    TEXTBOX_PlayerInfo3of4, /**<  プレイヤー詳細3/4   */
    TEXTBOX_PlayerInfo4of4, /**<  プレイヤー詳細4/4   */

    /*  VIEWMODE_BattleReplay */
    TEXTBOX_Replay_PlayerName_Mine,
    TEXTBOX_Replay_PlayerName_Opponent,
    TEXTBOX_Replay_RemainMons_Mine,
    TEXTBOX_Replay_RemainMons_Opponent,

    TEXTBOX_MAX
  };
  gfl2::lyt::LytTextBox*    m_TextBoxes[TEXTBOX_MAX];

  enum  PaneID
  {
    PANE_Mine_Win,
    PANE_Mine_Lose,
    PANE_Mine_Draw,
    PANE_Oppo_Win,
    PANE_Oppo_Lose,
    PANE_Oppo_Draw,

    PANE_MAX
  };

  gfl2::lyt::LytPane*       m_pPanes[PANE_MAX];



  enum  PictureID
  {
    PICTURE_Mine_Card,                  /**<  カード内自分                      */
    PICTURE_Mine_Replay_Oppo_Matching,  /**<  自分：replay, 相手：マッチング    */
    PICTURE_Oppo_Replay,                /**<  相手：replay                      */
    
    PICTURE_MAX
  };
  gfl2::lyt::LytPicture*    m_pPicture[PICTURE_MAX];




  mutable NetAppLib::UI::NetAppPokePartyUtility   m_PartyUtil;
};


} /*  namespace Live    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(UPPERVIEW_MAIN_H_INCLUDED)  */
