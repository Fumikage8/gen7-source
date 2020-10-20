#if !defined(UPPERVIEW_INFORMATION_H_INCLUDED)
#define UPPERVIEW_INFORMATION_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   UpperView_Information.h
 * @date   2015/10/06 Tue. 15:04:03
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_Macros.h>
#include  "./BattleSpotViewUtility.h"
#include  "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"


namespace NetApp      {
namespace BattleSpot  {
  class BattleSpotWork;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/10/06 Tue. 15:06:37
  */
//==============================================================================
class UpperView_Information
  : public    NetAppLib::UI::NetAppEmptyView
  , protected ViewUtility_Card
{
  GFL_FORBID_COPY_AND_ASSIGN(UpperView_Information);

public:
  UpperView_Information(BattleSpotWork* pWork);
  virtual ~UpperView_Information();


  enum  ViewMode
  {
    VIEWMODE_None,

    /*  welcome  */
    VIEWMODE_Welcome,

    /*  freebattle  */
    VIEWMODE_FreeBattle,
    VIEWMODE_FreeBattle_Matching_Single,    /**<  マッチング      */
    VIEWMODE_FreeBattle_Matching_Double,
    VIEWMODE_FreeBattle_Matching_BattleRoyal,
    VIEWMODE_FreeBattle_Matched_Single,     /**<  マッチング完了  */
    VIEWMODE_FreeBattle_Matched_Double,
    VIEWMODE_FreeBattle_Matched_BattleRoyal,
    VIEWMODE_FreeBattle_Rematch_Single,     /**<  再戦確認  */
    VIEWMODE_FreeBattle_Rematch_Double,
    VIEWMODE_FreeBattle_Rematch_BattleRoyal,

    /*  ratingBattle  */
    VIEWMODE_RatingBattle,
    VIEWMODE_RatingBattle_Matching_Single,    /**<  マッチング      */
    VIEWMODE_RatingBattle_Matching_Double,  
    VIEWMODE_RatingBattle_Matching_Special,
    VIEWMODE_RatingBattle_Matching_WCS,
    VIEWMODE_RatingBattle_Matched_Single,     /**<  マッチング完了  */
    VIEWMODE_RatingBattle_Matched_Double,
    VIEWMODE_RatingBattle_Matched_Special,
    VIEWMODE_RatingBattle_Matched_WCS,
    VIEWMODE_RatingBattle_Rematch_Single,     /**<  再戦確認  */
    VIEWMODE_RatingBattle_Rematch_Double,
    VIEWMODE_RatingBattle_Rematch_Special,
    VIEWMODE_RatingBattle_Rematch_WCS,

    VIEWMODE_RatingBattle_BattleVideoConfirm, /**<  バトルビデオ確認  */

    /*  internetMatch  */
    VIEWMODE_InternetMatch,
    VIEWMODE_InternetMatch_Card,              /**<  選手証          */
    VIEWMODE_InternetMatch_Information,       /**<  大会情報        */
    VIEWMODE_InternetMatch_Matching,          /**<  マッチング中    */
    VIEWMODE_InternetMatch_Matched,           /**<  マッチング完了  */
    VIEWMODE_InternetMatch_Message,           /**<  （通信エラーなどの）メッセージ表示  */

    /*  friendmatch  */
    VIEWMODE_FriendMatch,
    VIEWMODE_FriendMatch_Card,                /**<  選手証          */
    VIEWMODE_FriendMatch_Information,         /**<  大会情報        */
    VIEWMODE_FriendMatch_Matching,            /**<  マッチング中    */
    VIEWMODE_FriendMatch_Matched,             /**<  マッチング完了  */
    VIEWMODE_FriendMatch_Message,             /**<  （通信エラーなどの）メッセージ表示  */

    /*  others  */


    VIEWMODE_MAX
  };
  void  SetViewMode(const ViewMode viewMode);

  void StartIconSetup(const ViewMode viewMode);
  bool IsIconSetupComplete(void);

  gfl2::lyt::LytPicture* GetIconPane(void);

  void SetupOpponentText( void );
  void SetupOpponentTextRoyal( void );
  void SetupCupTitle( const wchar_t* pTitle1 , const wchar_t* pTitle2 );

  /*  implement UIView I/F  */
//  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
//  virtual bool  IsDrawing(void) const;
   /*  endimplement UIView I/F  */



protected:
  enum  TextBoxID
  {
    TEXTBOX_Title_1Line,
    TEXTBOX_Title_2Lines_0,
    TEXTBOX_Title_2Lines_1,
    TEXTBOX_Description,

    /*  typeB */
    TEXTBOX_TypeB_PlayerName,
    TEXTBOX_TypeB_Rate_PlayerName,
    TEXTBOX_TypeB_Rate_RateLabel,
    TEXTBOX_TypeB_Rate_RateValue,

    /*  typeC */
    TEXTBOX_TypeC_PlayerName,

    /*  typeD */
    TEXTBOX_TypeD_Player0_NameLabel,
    TEXTBOX_TypeD_Player1_NameLabel,
    TEXTBOX_TypeD_Player2_NameLabel,
    TEXTBOX_TypeD_Player0_Name,
    TEXTBOX_TypeD_Player1_Name,
    TEXTBOX_TypeD_Player2_Name,

    TEXTBOX_MAX
  };
  enum  ControlPaneID
  {
    CONTROLPANE_TitleWindow,
    CONTROLPANE_TitleWindow_1Line,
    CONTROLPANE_TitleWindow_2Lines,
    CONTROLPANE_TypeA,
    CONTROLPANE_TypeB,
    CONTROLPANE_TypeB_NameOnly,
    CONTROLPANE_TypeB_NameRate,
    CONTROLPANE_TypeC,
    CONTROLPANE_TypeD,
    
    CONTROLPANE_MAX
  };


  enum  LayoutMode
  {
    LAYOUTMODE_None,

    LAYOUTMODE_TypeA,       /**<  Desctiption                */
    LAYOUTMODE_TypeB,       /**<  顔,名前      +4行          */
    LAYOUTMODE_TypeB_Rate,  /**<  顔,名前,Rate +4行          */
    LAYOUTMODE_TypeC,       /**<  顔+名前+4行+messageWin     */
    LAYOUTMODE_TypeD,       /**<  (顔+2行)*3                 */
    
    LAYOUTMODE_MAX
  };
  void  SetLayoutMode(const LayoutMode layoutMode);



  /*  title  */
  void  ResetTitle(void);
  void  Set1LineTitle(const u32 messageID);
  void  Set2LinesTitle(const u32 messageID_0, const u32 messageID_1);

  /*  description  */
  void  SetDescription(const u32 messageID);


  /*
    members
  */
  BattleSpotWork*         m_pWork;
  ViewMode                m_ViewMode;

  /*  panes  */
  gfl2::lyt::LytTextBox*  m_pTextBoxes[TEXTBOX_MAX];
  gfl2::lyt::LytPane*     m_pControlPanes[CONTROLPANE_MAX];

  u32 m_iconIndex;
};



} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
#endif  /*  #if !defined(UPPERVIEW_INFORMATION_H_INCLUDED)  */
