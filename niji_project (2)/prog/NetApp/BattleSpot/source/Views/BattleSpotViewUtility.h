#if !defined(BATTLESPOTVIEWUTILITY_H_INCLUDED)
#define BATTLESPOTVIEWUTILITY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotViewUtility.h
 * @date   2015/10/14 Wed. 18:53:53
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
#include  <types/include/gfl2_Typedef.h>
#include  <Layout/include/gfl2_Layout.h>
#include  <qr/include/gfl2_IQREncoder.h>

#include  "NetStatic/NetAppLib/include/UI/NetAppQRTexture.h"


// =============================================================================
/**/
//==============================================================================
namespace app   {
  namespace tool  {class  PokeIconToolEx;}
  namespace util  {class  G2DUtil;}
}



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
  *  @date   2015/10/14 Wed. 11:17:02
  */
//==============================================================================
class ViewUtility_Card
{
  GFL_FORBID_COPY_AND_ASSIGN(ViewUtility_Card);
public:
  static const u32  QR_SOURCE_DATA_MAXBYTE  = 256;


  ViewUtility_Card(BattleSpotWork* pWork);
  virtual ~ViewUtility_Card();


  void  SetG2DUtil(app::util::G2DUtil* pG2DUtil)  {m_pG2DUtil = pG2DUtil;}

  /*  player  */
  enum  TextBoxID_PlayerStatus
  {
    TEXTBOX_Player_LabelRate,
    TEXTBOX_Player_LabelMatches,
    TEXTBOX_Player_LabelWins,
    TEXTBOX_Player_LabelLoses,

    TEXTBOX_Player_Rate,
    TEXTBOX_Player_Matches,
    TEXTBOX_Player_Wins,
    TEXTBOX_Player_Loses,

    TEXTBOX_Player_Information,
    TEXTBOX_Player_1LineMessage,
    
    TEXTBOX_Player_MAX
  };
  typedef struct  PlayerStatusPaneInfo
  {
    gfl2::lyt::LytTextBox*    pTextBoxes[TEXTBOX_Player_MAX];
    gfl2::lyt::LytPicture*    pIconPicture;
    gfl2::lyt::LytPane*       pIconSyncVisiblePane;
    gfl2::lyt::LytPane*       pIconControlPane;
        
    PlayerStatusPaneInfo(void)
      : pIconPicture(NULL)
      , pIconSyncVisiblePane(NULL)
      , pIconControlPane(NULL)
    {
      for(u32 index = 0; index < GFL_NELEMS(pTextBoxes); ++index) {pTextBoxes[index] = NULL;}
    }
  }PlayerStatusPaneInfo;

  void  SetPlayerStatus(PlayerStatusPaneInfo* pPaneInfo, const bool bSkipIconLoad = false);


  /*  opponent  */
  enum  TextBoxID_OpponentStatus
  {
    TEXTBOX_Opponent_LabelName0,
    TEXTBOX_Opponent_LabelName1,
    TEXTBOX_Opponent_LabelName2,
    TEXTBOX_Opponent_LabelRate,

    TEXTBOX_Opponent_Name0,
    TEXTBOX_Opponent_Name1,
    TEXTBOX_Opponent_Name2,
    TEXTBOX_Opponent_Rate,
    TEXTBOX_Opponent_Country,
    TEXTBOX_Opponent_Region,
    TEXTBOX_Opponent_PrevTournament,
    TEXTBOX_Opponent_Rank,
    
    TEXTBOX_Opponent_MAX
  };

  typedef struct  OpponentStatusPaneInfo
  {
    gfl2::lyt::LytTextBox*    pTextBoxes[TEXTBOX_Opponent_MAX];
    gfl2::lyt::LytPicture*    pIconPicture[3];
    gfl2::lyt::LytPane*       pIconControlPane[3];
    
    OpponentStatusPaneInfo(void)
    {
      for(u32 index = 0; index < GFL_NELEMS(pTextBoxes); ++index)       {pTextBoxes[index]        = NULL;}
      for(u32 index = 0; index < GFL_NELEMS(pIconPicture); ++index)     {pIconPicture[index]      = NULL;}
      for(u32 index = 0; index < GFL_NELEMS(pIconControlPane); ++index) {pIconControlPane[index]  = NULL;}
    }
  }OpponentStatusPaneInfo;

  void  SetOpponentStatus(OpponentStatusPaneInfo* pPaneInfo);


  /*  team  */
  typedef struct  TeamStatusPaneInfo
  {
    gfl2::lyt::LytTextBox*      pTextBox_TeamName;
    
    
    TeamStatusPaneInfo(void)
      : pTextBox_TeamName(NULL)
    {
    }
  }TeamStatusPaneInfo;

  void  SetTeamStatus(const TeamStatusPaneInfo& rPaneInfo);




  /*  qr  */
  typedef struct  QRPaneInfo
  {
    gfl2::lyt::LytPicture*  pQRPicture;
    
    QRPaneInfo(void)
      : pQRPicture(NULL)
    {}
  }QRPaneInfo;

  void  SetQR(QRPaneInfo* pQRPaneInfo);



private:
  /*    */
  void  SetTextBoxString(gfl2::lyt::LytTextBox* pTarget, const u32 messageID,               const bool isExpand = false);
  void  SetTextBoxString(gfl2::lyt::LytTextBox* pTarget, gfl2::str::StrBuf& rStr,           const bool isExpand = false);
  void  SetTextBoxString(gfl2::lyt::LytTextBox* pTarget, const gfl2::str::STRCODE& rStrCode);

  BattleSpotWork*                 m_pWork;  
  app::util::G2DUtil*             m_pG2DUtil;
  gfl2::qr::IQREncoder*           m_pQREncoder;
  gfl2::qr::QREncodeData          m_QRSourceData;
  NetAppLib::UI::NetAppQRTexture  m_QRTexture;
};



} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
#endif  /*  #if !defined(BATTLESPOTVIEWUTILITY_H_INCLUDED)  */
