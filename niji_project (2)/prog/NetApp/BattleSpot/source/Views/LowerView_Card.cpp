//==============================================================================
/**
 * @file   LowerView_Card.cpp
 * @date   2015/10/09 Fri. 19:25:00
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================



// =============================================================================
/**/
//==============================================================================
#include  "AppLib/include/Util/app_util_GetPaneHelper.h"

#include  "../BattleSpotResource.h"
#include  "../System/BattleSpotWork.h"

#include  "./LowerView_Card.h"
#include "NetStatic/NetAppLib/include/BgmSelect/NetworkBattleBgmSelect.h"

#include "Sound/include/Sound.h"

// =============================================================================
/**/
//==============================================================================


namespace
{
  enum
  {
    BUTTON_Info,
    BUTTON_BGMSelect_Prev,
    BUTTON_BGMSelect_Next,
    BUTTON_Back,
    
    BUTTON_MAX
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
LowerView_Card::LowerView_Card(BattleSpotWork* pWork)
  : NetAppLib::UI::NetAppCursorView(pWork)
  , NetAppLib::UI::NetAppTrainerIconVisibilityControl(pWork->GetAppHeap()->GetDeviceHeap(), 1)
  , ViewUtility_Card(pWork)
  , m_pWork(pWork)
  , m_pEventHandler(NULL)
  , m_ViewMode(VIEWMODE_INVALID)
  , m_LayoutMode(LAYOUTMODE_INVALID)
  , m_pMessageMenu(NULL)
  , m_pQRView(NULL)
  , m_PokePartyUtility(pWork)
  , m_LevelStringSource(pWork->GetMessageUtility()->GetString(BATTLESPOT_MESSAGE_ARCDAT, msg_bs_sys_33), pWork->GetDeviceHeap())
  , m_BGMIndex(0)

{
  gfl2::heap::HeapBase* pHeapBase = m_pWork->GetDeviceHeap();


  Create2DOneLayout(
    m_pWork,
    RESOURCE_Layout,
    LYTRES_BATTLESPOT_BATTLESPOT_CRD_LOW_000_BFLYT,
    LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    m_pWork->GetMessageUtility(),
    BATTLESPOT_MESSAGE_ARCDAT
  );

  ViewUtility_Card::SetG2DUtil(GetG2DUtil());


  app::util::GetPaneHelper  helper(GetG2DUtil()->GetLayoutWork(0), &GetG2DUtil()->GetLayoutResourceID(0));


  /*  textbox  */
  {
    /*  typeB  */
    m_pTextBoxes[TEXTBOX_TypeB_LabelRate]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_10);
    m_pTextBoxes[TEXTBOX_TypeB_LabelMatches]    = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_11);
    m_pTextBoxes[TEXTBOX_TypeB_LabelWins]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_12);
    m_pTextBoxes[TEXTBOX_TypeB_LabelLoses]      = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_17);
    m_pTextBoxes[TEXTBOX_TypeB_Rate]            = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_15);
    m_pTextBoxes[TEXTBOX_TypeB_Matches]         = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_16);
    m_pTextBoxes[TEXTBOX_TypeB_Wins]            = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_13);
    m_pTextBoxes[TEXTBOX_TypeB_Loses]           = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_14);
    m_pTextBoxes[TEXTBOX_TypeB_TeamName]        = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEAMNAME_01);
    m_pTextBoxes[TEXTBOX_TypeB_PMLevel00]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_06);
    m_pTextBoxes[TEXTBOX_TypeB_PMLevel01]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_07);
    m_pTextBoxes[TEXTBOX_TypeB_PMLevel02]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_08);
    m_pTextBoxes[TEXTBOX_TypeB_PMLevel03]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_09);
    m_pTextBoxes[TEXTBOX_TypeB_PMLevel04]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_10);
    m_pTextBoxes[TEXTBOX_TypeB_PMLevel05]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_11);

    /*  typeC  */
    m_pTextBoxes[TEXTBOX_TypeC_TeamName]        = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEAMNAME_02);
    m_pTextBoxes[TEXTBOX_TypeC_PMLevel00]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_12);
    m_pTextBoxes[TEXTBOX_TypeC_PMLevel01]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_13);
    m_pTextBoxes[TEXTBOX_TypeC_PMLevel02]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_14);
    m_pTextBoxes[TEXTBOX_TypeC_PMLevel03]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_15);
    m_pTextBoxes[TEXTBOX_TypeC_PMLevel04]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_16);
    m_pTextBoxes[TEXTBOX_TypeC_PMLevel05]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_17);

    /*  typeD  */
    m_pTextBoxes[TEXTBOX_TypeD_LabelRate]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_20);
    m_pTextBoxes[TEXTBOX_TypeD_LabelMatches]    = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_21);
    m_pTextBoxes[TEXTBOX_TypeD_Rate]            = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_22);
    m_pTextBoxes[TEXTBOX_TypeD_Matches]         = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_23);
    m_pTextBoxes[TEXTBOX_TypeD_TeamName]        = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEAMNAME_03);
    m_pTextBoxes[TEXTBOX_TypeD_PMLevel00]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_18);
    m_pTextBoxes[TEXTBOX_TypeD_PMLevel01]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_19);
    m_pTextBoxes[TEXTBOX_TypeD_PMLevel02]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_20);
    m_pTextBoxes[TEXTBOX_TypeD_PMLevel03]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_21);
    m_pTextBoxes[TEXTBOX_TypeD_PMLevel04]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_22);
    m_pTextBoxes[TEXTBOX_TypeD_PMLevel05]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_23);
    
    /*  typeE  */
    m_pTextBoxes[TEXTBOX_TypeE_1LineMessage]    = helper.Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_MESSAGE_E).GetTextBoxPane(BattleSpotPane::PANENAME_COMMON_MSG_LOW_003_PANE_TEXTBOX_00);
                                                  helper.Clear();
    m_pTextBoxes[TEXTBOX_TypeE_1LineMessage]->SetVisible(true);

    m_pTextBoxes[TEXTBOX_TypeE_LabelRate]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_25);
    m_pTextBoxes[TEXTBOX_TypeE_LabelMatches]    = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_26);
    m_pTextBoxes[TEXTBOX_TypeE_LabelWins]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_27);
    m_pTextBoxes[TEXTBOX_TypeE_LabelLoses]      = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_28);
    m_pTextBoxes[TEXTBOX_TypeE_Rate]            = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_29);
    m_pTextBoxes[TEXTBOX_TypeE_Matches]         = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_30);
    m_pTextBoxes[TEXTBOX_TypeE_Wins]            = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_31);
    m_pTextBoxes[TEXTBOX_TypeE_Loses]           = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_32);
    m_pTextBoxes[TEXTBOX_TypeE_TeamName]        = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEAMNAME_04);
    m_pTextBoxes[TEXTBOX_TypeE_PMLevel00]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_24);
    m_pTextBoxes[TEXTBOX_TypeE_PMLevel01]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_25);
    m_pTextBoxes[TEXTBOX_TypeE_PMLevel02]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_26);
    m_pTextBoxes[TEXTBOX_TypeE_PMLevel03]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_27);
    m_pTextBoxes[TEXTBOX_TypeE_PMLevel04]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_28);
    m_pTextBoxes[TEXTBOX_TypeE_PMLevel05]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_29);

    /*  typeF  */
    m_pTextBoxes[TEXTBOX_TypeF_1LineMessage]    = helper.Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_MESSAGE_E_00).GetTextBoxPane(BattleSpotPane::PANENAME_COMMON_MSG_LOW_003_PANE_TEXTBOX_00);
                                                  helper.Clear();
    m_pTextBoxes[TEXTBOX_TypeF_1LineMessage]->SetVisible(true);

    m_pTextBoxes[TEXTBOX_TypeF_LabelMatches]    = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_34);
    m_pTextBoxes[TEXTBOX_TypeF_Matches]         = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_35);
    m_pTextBoxes[TEXTBOX_TypeF_TeamName]        = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEAMNAME_05);
    m_pTextBoxes[TEXTBOX_TypeF_Information]     = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_36);
    m_pTextBoxes[TEXTBOX_TypeF_PMLevel00]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_30);
    m_pTextBoxes[TEXTBOX_TypeF_PMLevel01]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_31);
    m_pTextBoxes[TEXTBOX_TypeF_PMLevel02]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_32);
    m_pTextBoxes[TEXTBOX_TypeF_PMLevel03]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_33);
    m_pTextBoxes[TEXTBOX_TypeF_PMLevel04]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_34);
    m_pTextBoxes[TEXTBOX_TypeF_PMLevel05]       = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_35);

    /*  others  */
    m_pTextBoxes[TEXTBOX_TournamentInfoButton]  = helper.Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBUTTON).GetTextBoxPane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_016_PANE_TEXTBOX_00);
                                                  helper.Clear();

    m_pTextBoxes[TEXTBOX_BGM]                   = helper.GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBOX_MUSIC);

    /*  fixed string  */
    GetG2DUtil()->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_TournamentInfoButton], msg_bs_sel_17);
  }


  /*  pane  */
  {
    helper.Clear();
    m_pPanes[PANE_TypeB]                  = helper.GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TYPE_B);
    m_pPanes[PANE_TypeC]                  = helper.GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TYPE_C);
    m_pPanes[PANE_TypeD]                  = helper.GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TYPE_D);
    m_pPanes[PANE_TypeE]                  = helper.GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TYPE_E);
    m_pPanes[PANE_TypeF]                  = helper.GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TYPE_F);
    m_pPanes[PANE_TypeF_Team]             = helper.GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEAMSET_F);
    m_pPanes[PANE_TypeF_Information]      = helper.GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_NULL_TYPEF_INFO);
    m_pPanes[PANE_Info]                   = helper.GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_NULL_INFO);
    m_pPanes[PANE_BGM]                    = helper.GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_NULL_BGM);
    m_pPanes[PANE_Back]                   = helper.GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_NULL_BACK);
    m_pPanes[PANE_TrainerIconSyncVisible] = helper.GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_NULL_ICONCTRL_00);
    m_pPanes[PANE_Back]->SetVisible(true);
  }

  /*  buttons  */
  {
    const app::ui::ButtonInfoEx   buttonInfoEx[BUTTON_MAX]  =
    {
      {
        BUTTON_Info,                                                                                    //  app::tool::ButtonManager::ButtonId button_id;
        helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_TEXTBUTTON).Peek(),    //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_016_PANE_BOUND_00),              //  gfl2::lyt::LytPane* bound_pane;
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__TEXTBUTTON_TOUCH,
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__TEXTBUTTON_RELEASE,
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__TEXTBUTTON_CANSEL,
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__TEXTBUTTON_TOUCH_RELEASE,
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__TEXTBUTTON_ACTIVE,
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__TEXTBUTTON_PASSIVE,
      },
      {
        BUTTON_BGMSelect_Prev,                                                                          //  app::tool::ButtonManager::ButtonId button_id;
        helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_CURSOR_L_A).Peek(),    //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(BattleSpotPane::PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00),              //  gfl2::lyt::LytPane* bound_pane;
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__CURSOR_L_A_TOUCH,
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__CURSOR_L_A_RELEASE,
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__CURSOR_L_A_CANSEL,
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__CURSOR_L_A_TOUCH_RELEASE,
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__CURSOR_L_A_ACTIVE,
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__CURSOR_L_A_PASSIVE,
      },
      {
        BUTTON_BGMSelect_Next,                                                                          //  app::tool::ButtonManager::ButtonId button_id;
        helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_CURSOR_R_A).Peek(),    //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(BattleSpotPane::PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00),              //  gfl2::lyt::LytPane* bound_pane;
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__CURSOR_R_A_TOUCH,
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__CURSOR_R_A_RELEASE,
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__CURSOR_R_A_CANSEL,
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__CURSOR_R_A_TOUCH_RELEASE,
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__CURSOR_R_A_ACTIVE,
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__CURSOR_R_A_PASSIVE,
      },
      {
        BUTTON_Back,                                                                                    //  app::tool::ButtonManager::ButtonId button_id;
        helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_BACKBUTTON).Peek(),    //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00),              //  gfl2::lyt::LytPane* bound_pane;
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__BACKBUTTON_TOUCH,
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__BACKBUTTON_RELEASE,
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__BACKBUTTON_CANSEL,
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__BACKBUTTON_TOUCH_RELEASE,
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__BACKBUTTON_ACTIVE,
        LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__BACKBUTTON_PASSIVE,
      },
    };

    CreateButtonManager( m_pWork->GetAppHeap(), GetG2DUtil()->GetLayoutWork(0), buttonInfoEx, GFL_NELEMS(buttonInfoEx) );
    RegisterShortcut_B(BUTTON_Back);

    /*  SE  */
    {
      app::tool::ButtonManager* pButtonManager  = GetButtonManager();

      pButtonManager->SetButtonSelectSE(BUTTON_Info,            SEQ_SE_DECIDE1);
      pButtonManager->SetButtonSelectSE(BUTTON_BGMSelect_Prev,  SEQ_SE_DECIDE1);
      pButtonManager->SetButtonSelectSE(BUTTON_BGMSelect_Next,  SEQ_SE_DECIDE1);
      pButtonManager->SetButtonSelectSE(BUTTON_Back,            SEQ_SE_CANCEL1);
    }
  }


  /*  qrview  */
  {
    m_pQRView = GFL_NEW(pHeapBase)  LowerView_QR(m_pWork);

    m_pQRView->SetVisible(false);
    m_pQRView->SetModal(false);
    AddSubView(m_pQRView);
  }

  /*  messageMenu  */
  {
    m_pMessageMenu  = GFL_NEW(pHeapBase)  MessageMenu(pWork);
    m_pMessageMenu->Setup(RESOURCE_LayoutCommonWindow, RESOURCE_LayoutCommonMenuCursor, m_pWork->GetMessageUtility());
    m_pMessageMenu->SetEventHandler(this);

    AddSubView(m_pMessageMenu);
  }

  /*  partyUtil  */
  {
    const u32 partyCapacity = 5;  /*  B,C,D,E,F  */
    NetAppLib::UI::NetAppPokeparaUtility::RegisterInfo  infos[pml::PokeParty::MAX_MEMBERS];

    m_PokePartyUtility.Setup(partyCapacity, this, 0);

    /*  共通部分  */
    for(u32 index = 0; index < GFL_NELEMS(infos); ++index)
    {
      infos[index].pLevelStringSource = &m_LevelStringSource;
    }

    /*  TypeB  */
    infos[0].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_06);
    infos[1].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_07);
    infos[2].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_08);
    infos[3].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_09);
    infos[4].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_10);
    infos[5].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_11);

    infos[0].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_06).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[1].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_07).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[2].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_08).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[3].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_09).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[4].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_10).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[5].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_11).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);

    infos[0].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeB_PMLevel00];
    infos[1].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeB_PMLevel01];
    infos[2].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeB_PMLevel02];
    infos[3].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeB_PMLevel03];
    infos[4].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeB_PMLevel04];
    infos[5].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeB_PMLevel05];

    infos[0].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_06);
    infos[1].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_07);
    infos[2].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_08);
    infos[3].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_09);
    infos[4].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_10);
    infos[5].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_11);

    infos[0].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_06_ICON_MALE;
    infos[1].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_07_ICON_MALE;
    infos[2].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_08_ICON_MALE;
    infos[3].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_09_ICON_MALE;
    infos[4].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_10_ICON_MALE;
    infos[5].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_11_ICON_MALE;

    infos[0].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_06_ICON_FEMALE;
    infos[1].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_07_ICON_FEMALE;
    infos[2].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_08_ICON_FEMALE;
    infos[3].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_09_ICON_FEMALE;
    infos[4].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_10_ICON_FEMALE;
    infos[5].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_11_ICON_FEMALE;

    m_PokePartyUtility.RegisterStatusResource(0, 0, infos[0]);
    m_PokePartyUtility.RegisterStatusResource(0, 1, infos[1]);
    m_PokePartyUtility.RegisterStatusResource(0, 2, infos[2]);
    m_PokePartyUtility.RegisterStatusResource(0, 3, infos[3]);
    m_PokePartyUtility.RegisterStatusResource(0, 4, infos[4]);
    m_PokePartyUtility.RegisterStatusResource(0, 5, infos[5]);

    m_PokePartyUtility.SetPokeParty( 0 , m_pWork->m_pAppParam->m_pPersonalData[0]->pPokeParty );


    /*  TypeC  */
    infos[0].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_12);
    infos[1].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_13);
    infos[2].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_14);
    infos[3].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_15);
    infos[4].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_16);
    infos[5].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_17);

    infos[0].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_12).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[1].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_13).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[2].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_14).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[3].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_15).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[4].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_16).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[5].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_17).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);

    infos[0].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeC_PMLevel00];
    infos[1].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeC_PMLevel01];
    infos[2].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeC_PMLevel02];
    infos[3].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeC_PMLevel03];
    infos[4].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeC_PMLevel04];
    infos[5].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeC_PMLevel05];

    infos[0].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_12);
    infos[1].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_13);
    infos[2].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_14);
    infos[3].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_15);
    infos[4].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_16);
    infos[5].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_17);

    infos[0].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_12_ICON_MALE;
    infos[1].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_13_ICON_MALE;
    infos[2].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_14_ICON_MALE;
    infos[3].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_15_ICON_MALE;
    infos[4].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_16_ICON_MALE;
    infos[5].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_17_ICON_MALE;

    infos[0].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_12_ICON_FEMALE;
    infos[1].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_13_ICON_FEMALE;
    infos[2].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_14_ICON_FEMALE;
    infos[3].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_15_ICON_FEMALE;
    infos[4].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_16_ICON_FEMALE;
    infos[5].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_17_ICON_FEMALE;

    m_PokePartyUtility.RegisterStatusResource(1, 0, infos[0]);
    m_PokePartyUtility.RegisterStatusResource(1, 1, infos[1]);
    m_PokePartyUtility.RegisterStatusResource(1, 2, infos[2]);
    m_PokePartyUtility.RegisterStatusResource(1, 3, infos[3]);
    m_PokePartyUtility.RegisterStatusResource(1, 4, infos[4]);
    m_PokePartyUtility.RegisterStatusResource(1, 5, infos[5]);

    m_PokePartyUtility.SetPokeParty( 1 , m_pWork->m_pAppParam->m_pPersonalData[0]->pPokeParty );


    /*  TypeD  */
    infos[0].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_18);
    infos[1].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_19);
    infos[2].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_20);
    infos[3].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_21);
    infos[4].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_22);
    infos[5].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_23);

    infos[0].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_18).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[1].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_19).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[2].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_20).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[3].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_21).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[4].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_22).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[5].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_23).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);

    infos[0].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeD_PMLevel00];
    infos[1].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeD_PMLevel01];
    infos[2].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeD_PMLevel02];
    infos[3].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeD_PMLevel03];
    infos[4].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeD_PMLevel04];
    infos[5].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeD_PMLevel05];

    infos[0].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_18);
    infos[1].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_19);
    infos[2].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_20);
    infos[3].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_21);
    infos[4].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_22);
    infos[5].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_23);

    infos[0].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_18_ICON_MALE;
    infos[1].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_19_ICON_MALE;
    infos[2].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_20_ICON_MALE;
    infos[3].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_21_ICON_MALE;
    infos[4].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_22_ICON_MALE;
    infos[5].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_23_ICON_MALE;

    infos[0].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_18_ICON_FEMALE;
    infos[1].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_19_ICON_FEMALE;
    infos[2].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_20_ICON_FEMALE;
    infos[3].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_21_ICON_FEMALE;
    infos[4].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_22_ICON_FEMALE;
    infos[5].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_23_ICON_FEMALE;

    m_PokePartyUtility.RegisterStatusResource(2, 0, infos[0]);
    m_PokePartyUtility.RegisterStatusResource(2, 1, infos[1]);
    m_PokePartyUtility.RegisterStatusResource(2, 2, infos[2]);
    m_PokePartyUtility.RegisterStatusResource(2, 3, infos[3]);
    m_PokePartyUtility.RegisterStatusResource(2, 4, infos[4]);
    m_PokePartyUtility.RegisterStatusResource(2, 5, infos[5]);

    m_PokePartyUtility.SetPokeParty( 2 , m_pWork->m_pAppParam->m_pPersonalData[0]->pPokeParty );

    /*  TypeE  */
    infos[0].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_24);
    infos[1].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_25);
    infos[2].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_26);
    infos[3].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_27);
    infos[4].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_28);
    infos[5].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_29);
                                                                                                                       
    infos[0].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_24).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[1].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_25).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[2].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_26).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[3].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_27).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[4].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_28).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[5].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_29).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);

    infos[0].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeE_PMLevel00];
    infos[1].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeE_PMLevel01];
    infos[2].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeE_PMLevel02];
    infos[3].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeE_PMLevel03];
    infos[4].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeE_PMLevel04];
    infos[5].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeE_PMLevel05];

    infos[0].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_24);
    infos[1].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_25);
    infos[2].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_26);
    infos[3].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_27);
    infos[4].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_28);
    infos[5].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_29);

    infos[0].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_24_ICON_MALE;
    infos[1].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_25_ICON_MALE;
    infos[2].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_26_ICON_MALE;
    infos[3].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_27_ICON_MALE;
    infos[4].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_28_ICON_MALE;
    infos[5].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_29_ICON_MALE;

    infos[0].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_24_ICON_FEMALE;
    infos[1].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_25_ICON_FEMALE;
    infos[2].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_26_ICON_FEMALE;
    infos[3].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_27_ICON_FEMALE;
    infos[4].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_28_ICON_FEMALE;
    infos[5].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_29_ICON_FEMALE;

    m_PokePartyUtility.RegisterStatusResource(3, 0, infos[0]);
    m_PokePartyUtility.RegisterStatusResource(3, 1, infos[1]);
    m_PokePartyUtility.RegisterStatusResource(3, 2, infos[2]);
    m_PokePartyUtility.RegisterStatusResource(3, 3, infos[3]);
    m_PokePartyUtility.RegisterStatusResource(3, 4, infos[4]);
    m_PokePartyUtility.RegisterStatusResource(3, 5, infos[5]);

    m_PokePartyUtility.SetPokeParty( 3 , m_pWork->m_pAppParam->m_pPersonalData[0]->pPokeParty );

    /*  TypeF  */
    infos[0].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_30);
    infos[1].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_31);
    infos[2].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_32);
    infos[3].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_33);
    infos[4].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_34);
    infos[5].pVisibilityControl     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKE_35);
                                                                                                                       
    infos[0].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_30).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[1].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_31).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[2].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_32).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[3].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_33).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[4].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_34).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);
    infos[5].pIconPicture           = helper.Clear().Push(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_POKEICON_35).GetPicturePane(BattleSpotPane::PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON);

    infos[0].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeF_PMLevel00];
    infos[1].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeF_PMLevel01];
    infos[2].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeF_PMLevel02];
    infos[3].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeF_PMLevel03];
    infos[4].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeF_PMLevel04];
    infos[5].pTextBox_Level         = m_pTextBoxes[TEXTBOX_TypeF_PMLevel05];

    infos[0].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_30);
    infos[1].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_31);
    infos[2].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_32);
    infos[3].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_33);
    infos[4].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_34);
    infos[5].pGenderIconParts       = helper.Clear().GetPartsPane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_GENDER_35);

    infos[0].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_30_ICON_MALE;
    infos[1].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_31_ICON_MALE;
    infos[2].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_32_ICON_MALE;
    infos[3].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_33_ICON_MALE;
    infos[4].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_34_ICON_MALE;
    infos[5].genderIconAnim_Male    = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_35_ICON_MALE;

    infos[0].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_30_ICON_FEMALE;
    infos[1].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_31_ICON_FEMALE;
    infos[2].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_32_ICON_FEMALE;
    infos[3].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_33_ICON_FEMALE;
    infos[4].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_34_ICON_FEMALE;
    infos[5].genderIconAnim_Female  = LA_BATTLESPOT_BATTLESPOT_CRD_LOW_000__GENDER_35_ICON_FEMALE;

    m_PokePartyUtility.RegisterStatusResource(4, 0, infos[0]);
    m_PokePartyUtility.RegisterStatusResource(4, 1, infos[1]);
    m_PokePartyUtility.RegisterStatusResource(4, 2, infos[2]);
    m_PokePartyUtility.RegisterStatusResource(4, 3, infos[3]);
    m_PokePartyUtility.RegisterStatusResource(4, 4, infos[4]);
    m_PokePartyUtility.RegisterStatusResource(4, 5, infos[5]);

    m_PokePartyUtility.SetPokeParty( 4 , m_pWork->m_pAppParam->m_pPersonalData[0]->pPokeParty );
  }

  RegisterTrainerIconVisibilityControlPane(0, m_pPanes[PANE_TrainerIconSyncVisible]);

  SetViewMode(VIEWMODE_None);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
LowerView_Card::~LowerView_Card()
{
  UnregisterAllTrainerIconVisibilityControlPane();

  if(m_pQRView)       m_pQRView->RemoveFromSuperView();
  if(m_pMessageMenu)  m_pMessageMenu->RemoveFromSuperView();

  GFL_SAFE_DELETE(m_pQRView);
  GFL_SAFE_DELETE(m_pMessageMenu);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool LowerView_Card::IsDrawing(void) const
{
  return  (
    NetAppLib::UI::NetAppCursorView::IsDrawing()
    ||  m_pMessageMenu->IsDrawing()
    ||  (!m_PokePartyUtility.Cleanup())
  );
}


//------------------------------------------------------------------
/**
  *  @func     OnKeyAction
  */
//------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  LowerView_Card::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  ListenerResult  result  = ENABLE_ACCESS;

  if(NetAppCursorView::OnKeyAction(pButton, pKey, pStick) == DISABLE_ACCESS)  return DISABLE_ACCESS;


  app::tool::ButtonManager*   pUIButtonManager  = GetButtonManager();

  if( pKey->IsTrigger( gfl2::ui::DIR_LEFT ) || pButton->IsTrigger( gfl2::ui::BUTTON_L ) )
  {
    pUIButtonManager->StartSelectedAct(BUTTON_BGMSelect_Prev);
    result  = DISABLE_ACCESS;
  }
  else if( pKey->IsTrigger(gfl2::ui::DIR_RIGHT) || pButton->IsTrigger( gfl2::ui::BUTTON_R ) )
  {
    pUIButtonManager->StartSelectedAct(BUTTON_BGMSelect_Next);
    result  = DISABLE_ACCESS;
  }

  return result;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult LowerView_Card::OnLayoutPaneEvent(const u32 buttonID)
{
  ListenerResult result  =  NetAppCursorView::OnLayoutPaneEvent(buttonID);

  if(result == DISABLE_ACCESS)  return DISABLE_ACCESS;


  switch(buttonID)
  {
    case  BUTTON_Info:
      Event(IEventHandler::EVENT_Info);
      result  = DISABLE_ACCESS;
      break;

    case  BUTTON_BGMSelect_Prev:
      SelectBGM(-1);
      result  = DISABLE_ACCESS;
      break;

    case  BUTTON_BGMSelect_Next:
      SelectBGM(1);
      result  = DISABLE_ACCESS;
      break;

    case  BUTTON_Back:
      result  = DISABLE_ACCESS;
      Event(IEventHandler::EVENT_Back);
      break;
  }

  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Card::SelectBGM(const s32 indexOffset)
{
  u32 bgmCount = NetApp::BgmSelect::NetworkBattleBgmSelect::GetMaxBgmIndex() + 1;

  m_BGMIndex  = (m_BGMIndex + (bgmCount + indexOffset)) % bgmCount;

  u32 bgmTextLabel = NetApp::BgmSelect::NetworkBattleBgmSelect::GetBgmTextLabel( m_BGMIndex );

  GetG2DUtil()->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_BGM], &(m_pWork->GetMessageUtility()->GetString(BATTLESPOT_MESSAGE_BGMLIST_ARCDAT, bgmTextLabel )));
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Card::Update(void)
{
  /*  super  */
  NetAppLib::UI::NetAppCursorView::Update();

  m_PokePartyUtility.OnUpdate();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Card::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pWork->GetAppRenderingManager(), displayNo, 0);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Card::SetViewMode(const ViewMode viewMode)
{
  LayoutMode        layoutMode              = LAYOUTMODE_None;
  bool              isBGMSelectEnable       = false;
  bool              isTournamentInfoEnable  = false;
  bool              isCardViewMode          = false;
  bool              isEnableQRView          = false;
  bool              isEnableBack            = true;
  bool              isSkipIconLoad          = false;
  MessageMenu::Mode messageMode             = MessageMenu::MODE_None;

  m_ViewMode  = viewMode;
  SetAllTrainerIconVisibilityInternal(false);
  switch(viewMode)
  {
    case  VIEWMODE_None:
      break;

    /*  ratingBattle  */
    case  VIEWMODE_RatingBattle_Matching:           /**<  対戦相手を探しています          */
      layoutMode              = LAYOUTMODE_TypeB;
      isBGMSelectEnable       = true;
      isTournamentInfoEnable  = false;
      isCardViewMode          = false;
      isEnableQRView          = false;
      isEnableBack            = false;
      messageMode             = MessageMenu::MODE_Message_SearchingOpponent;
      break;

    case  VIEWMODE_RatingBattle_Matched:            /**<  対戦相手がみつかりました        */
      layoutMode              = LAYOUTMODE_TypeB;
      isBGMSelectEnable       = true;
      isTournamentInfoEnable  = false;
      isCardViewMode          = false;
      isEnableQRView          = false;
      isEnableBack            = false;
      messageMode             = MessageMenu::MODE_Message_OpponentFound;
      break;

    case  VIEWMODE_RatingBattle_MatchingFailed:     /**<  対戦相手がみつかりません        */
      layoutMode              = LAYOUTMODE_TypeB;
      isBGMSelectEnable       = false;
      isTournamentInfoEnable  = false;
      isCardViewMode          = false;
      isEnableQRView          = false;
      isEnableBack            = false;
      messageMode             = MessageMenu::MODE_Message_OpponentNotFound;
      break;

    /*  freebattle  */
    case  VIEWMODE_FreeBattle_Matching:             /**<  対戦相手を探しています          */
      layoutMode              = LAYOUTMODE_TypeC;
      isBGMSelectEnable       = true;
      isTournamentInfoEnable  = false;
      isCardViewMode          = false;
      isEnableQRView          = false;
      isEnableBack            = false;
      messageMode             = MessageMenu::MODE_Message_SearchingOpponent;
      break;

    case  VIEWMODE_FreeBattle_Matched:              /**<  対戦相手がみつかりました        */
      layoutMode              = LAYOUTMODE_TypeC;
      isBGMSelectEnable       = true;
      isTournamentInfoEnable  = false;
      isCardViewMode          = false;
      isEnableQRView          = false;
      isEnableBack            = false;
      messageMode             = MessageMenu::MODE_Message_OpponentFound;
      break;

    case  VIEWMODE_FreeBattle_MatchingFailed:       /**<  対戦相手がみつかりません        */
      layoutMode              = LAYOUTMODE_TypeC;
      isBGMSelectEnable       = false;
      isTournamentInfoEnable  = false;
      isCardViewMode          = false;
      isEnableQRView          = false;
      isEnableBack            = false;
      messageMode             = MessageMenu::MODE_Message_OpponentNotFound;
      break;

    /*  internetMatch  */
    case  VIEWMODE_InternetMatch_Card:              /**<  参加中の大会です：チーム登録前  */
      layoutMode              = LAYOUTMODE_TypeF;
      isBGMSelectEnable       = false;
      isTournamentInfoEnable  = true;
      isCardViewMode          = false;
      isEnableQRView          = false;
      isEnableBack            = true;
      messageMode             = MessageMenu::MODE_None;
      break;

    case  VIEWMODE_InternetMatch_TeamCard:          /**<  参加中の大会です：チーム登録後  */
      layoutMode              = LAYOUTMODE_TypeF;
      isBGMSelectEnable       = false;
      isTournamentInfoEnable  = true;
      isCardViewMode          = true;
      isEnableQRView          = false;
      isEnableBack            = true;
      messageMode             = MessageMenu::MODE_None;
      break;

    case  VIEWMODE_InternetMatch_TournamentInfo:    /**<  大会情報（QR）                  */
      layoutMode              = LAYOUTMODE_None;
      isBGMSelectEnable       = false;
      isTournamentInfoEnable  = false;
      isCardViewMode          = false;
      isEnableQRView          = true;
      isEnableBack            = true;
      messageMode             = MessageMenu::MODE_None;
      SetQRView(isEnableQRView);
      break;

    case  VIEWMODE_InternetMatch_Matching:          /**<  対戦相手を探しています          */
      layoutMode              = LAYOUTMODE_TypeD;
      isBGMSelectEnable       = true;
      isTournamentInfoEnable  = false;
      isCardViewMode          = false;
      isEnableQRView          = false;
      isEnableBack            = false;
      messageMode             = MessageMenu::MODE_Message_SearchingOpponent;
      break;

    case  VIEWMODE_InternetMatch_Matched:           /**<  対戦相手がみつかりました        */
      layoutMode              = LAYOUTMODE_TypeD;
      isBGMSelectEnable       = true;
      isTournamentInfoEnable  = false;
      isCardViewMode          = false;
      isEnableQRView          = false;
      isEnableBack            = false;
      messageMode             = MessageMenu::MODE_Message_OpponentFound;
      break;

    case  VIEWMODE_InternetMatch_MatchingFailed:    /**<  対戦相手がみつかりません        */
      layoutMode              = LAYOUTMODE_TypeD;
      isBGMSelectEnable       = false;
      isEnableQRView          = false;
      isTournamentInfoEnable  = false;
      messageMode             = MessageMenu::MODE_Message_OpponentNotFound;
      isEnableBack            = false;
      isCardViewMode          = false;
      break;

    /*  friendmatch  */
    case  VIEWMODE_FriendMatch_Card:                /**<  参加中の大会です：チーム登録前  */
      layoutMode              = LAYOUTMODE_TypeF;
      isBGMSelectEnable       = false;
      isTournamentInfoEnable  = true;
      isCardViewMode          = false;
      isEnableQRView          = false;
      isEnableBack            = true;
      messageMode             = MessageMenu::MODE_None;
      break;

    case  VIEWMODE_FriendMatch_TeamCard:            /**<  参加中の大会です：チーム登録後  */
      layoutMode              = LAYOUTMODE_TypeF;
      isBGMSelectEnable       = false;
      isTournamentInfoEnable  = true;
      isCardViewMode          = true;
      isEnableQRView          = false;
      isEnableBack            = true;
      messageMode             = MessageMenu::MODE_None;
      break;

    case  VIEWMODE_FriendMatch_TournamentInfo:      /**<  大会情報（QR）                  */
      layoutMode              = LAYOUTMODE_None;
      isBGMSelectEnable       = false;
      isTournamentInfoEnable  = false;
      isCardViewMode          = false;
      isEnableQRView          = true;
      isEnableBack            = true;
      messageMode             = MessageMenu::MODE_None;
      break;

    case  VIEWMODE_FriendMatch_Matching:            /**<  対戦相手を探しています          */
      layoutMode              = LAYOUTMODE_TypeD;
      isBGMSelectEnable       = true;
      isTournamentInfoEnable  = false;
      isCardViewMode          = false;
      isEnableQRView          = false;
      isEnableBack            = false;
      messageMode             = MessageMenu::MODE_Message_SearchingOpponent;
      break;

    case  VIEWMODE_FriendMatch_Matched:             /**<  対戦相手がみつかりました        */
      layoutMode              = LAYOUTMODE_TypeD;
      isBGMSelectEnable       = true;
      isTournamentInfoEnable  = false;
      isCardViewMode          = false;
      isEnableQRView          = false;
      isEnableBack            = false;
      messageMode             = MessageMenu::MODE_Message_OpponentFound;
      break;

    case  VIEWMODE_FriendMatch_MatchingFailed:      /**<  対戦相手がみつかりません        */
      layoutMode              = LAYOUTMODE_TypeD;
      isBGMSelectEnable       = false;
      isTournamentInfoEnable  = false;
      isCardViewMode          = false;
      isEnableQRView          = false;
      isEnableBack            = false;
      messageMode             = MessageMenu::MODE_Message_OpponentNotFound;
      break;
  }

  isSkipIconLoad  = (m_LayoutMode == layoutMode);
  SetLayoutMode(layoutMode);
  SetCardViewMode(isCardViewMode);
  SetTournamentInfoEnable(isTournamentInfoEnable);
  SetBGMSelectEnable(isBGMSelectEnable);
  SetBackEnable(isEnableBack);
  if(messageMode == MessageMenu::MODE_None)  {
    m_pMessageMenu->Hide();
  }
  else
  {
    m_pMessageMenu->Show(messageMode);
  }
  
  m_pQRView->SetVisible(isEnableQRView);



  app::util::GetPaneHelper  helper(GetG2DUtil()->GetLayoutWork(0), &GetG2DUtil()->GetLayoutResourceID(0));
  PlayerStatusPaneInfo      playerStatusInfo;
  OpponentStatusPaneInfo    opponentStatusInfo;
  TeamStatusPaneInfo        teamInfo;
  
  
  switch(layoutMode)
  {
    case  LAYOUTMODE_None:
      break;
      
    case  LAYOUTMODE_TypeB:   /**<  マッチング：レーティング  */
      playerStatusInfo.pIconPicture         = helper.GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_PLAYERPHOTO_00);
      playerStatusInfo.pIconSyncVisiblePane = m_pPanes[PANE_TrainerIconSyncVisible];
      playerStatusInfo.pIconControlPane     = helper.GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_WINDOW_PLAYER_FRAME_00);

      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_LabelRate]     = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeB_LabelRate];
      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_LabelMatches]  = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeB_LabelMatches];
      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_LabelWins]     = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeB_LabelWins];
      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_LabelLoses]    = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeB_LabelLoses];
      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_Rate]          = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeB_Rate];
      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_Matches]       = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeB_Matches];
      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_Wins]          = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeB_Wins];
      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_Loses]         = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeB_Loses];
      /*  trainericon  */
      SetTrainerIconVisibilityInternal(0, true);
      SetPlayerStatus(&playerStatusInfo, isSkipIconLoad);

      teamInfo.pTextBox_TeamName  = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeB_TeamName];
      SetTeamStatus(teamInfo);
      break;

    case  LAYOUTMODE_TypeC:   /**<  マッチング：フリー        */
      playerStatusInfo.pIconPicture         = helper.GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_PLAYERPHOTO_01);
      playerStatusInfo.pIconSyncVisiblePane = m_pPanes[PANE_TrainerIconSyncVisible];
      playerStatusInfo.pIconControlPane     = helper.GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_WINDOW_PLAYER_FRAME_01);
      /*  trainericon  */
      SetTrainerIconVisibilityInternal(0, true);
      SetPlayerStatus(&playerStatusInfo, isSkipIconLoad);

      teamInfo.pTextBox_TeamName  = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeC_TeamName];
      SetTeamStatus(teamInfo);
      break;

    case  LAYOUTMODE_TypeD:   /**<  マッチング：仲間大会      */
      playerStatusInfo.pIconPicture         = helper.GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_PLAYERPHOTO_02);
      playerStatusInfo.pIconSyncVisiblePane = m_pPanes[PANE_TrainerIconSyncVisible];
      playerStatusInfo.pIconControlPane     = helper.GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_WINDOW_PLAYER_FRAME_02);

      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_LabelRate]     = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeD_LabelRate];
      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_LabelMatches]  = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeD_LabelMatches];
      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_Rate]          = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeD_Rate];
      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_Matches]       = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeD_Matches];
      /*  trainericon  */
      SetTrainerIconVisibilityInternal(0, true);
      SetPlayerStatus(&playerStatusInfo, isSkipIconLoad);

      teamInfo.pTextBox_TeamName  = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeD_TeamName];
      SetTeamStatus(teamInfo);
      break;

    case  LAYOUTMODE_TypeE:   /**<  ?        */
      playerStatusInfo.pIconPicture         = helper.GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_PLAYERPHOTO_03);
      playerStatusInfo.pIconSyncVisiblePane = m_pPanes[PANE_TrainerIconSyncVisible];
      playerStatusInfo.pIconControlPane     = helper.GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_WINDOW_PLAYER_FRAME_03);

//    TEXTBOX_TypeE_1LineMessage,   /**<  一行メッセージ      */
      {
        const u32 messageID = Get1LiveMessageIDFromEntryWork((m_ViewMode == VIEWMODE_InternetMatch_Card) || (m_ViewMode == VIEWMODE_InternetMatch_TeamCard));

        if(messageID != UINT_MAX)
        {
          GetG2DUtil()->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_TypeE_1LineMessage], messageID);
        }
      }

      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_LabelRate]     = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeE_LabelRate];
      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_LabelMatches]  = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeE_LabelMatches];
      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_LabelWins]     = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeE_LabelWins];
      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_LabelLoses]    = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeE_LabelLoses];
      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_Rate]          = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeE_Rate];
      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_Matches]       = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeE_Matches];
      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_Wins]          = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeE_Wins];
      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_Loses]         = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeE_Loses];

      /*  trainericon  */
      SetTrainerIconVisibilityInternal(0, true);
      SetPlayerStatus(&playerStatusInfo, isSkipIconLoad);

      teamInfo.pTextBox_TeamName  = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeE_TeamName];
      SetTeamStatus(teamInfo);
      break;

    case  LAYOUTMODE_TypeF:   /**<  大会情報：仲間大会        */
      /*  一行メッセージ  */
      {
        const u32 messageID = Get1LiveMessageIDFromEntryWork((m_ViewMode == VIEWMODE_InternetMatch_Card) || (m_ViewMode == VIEWMODE_InternetMatch_TeamCard));

        if(messageID != UINT_MAX)
        {
          GetG2DUtil()->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_TypeF_1LineMessage], messageID);
        }
      }

      playerStatusInfo.pIconPicture         = helper.GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_PLAYERPHOTO_04);
      playerStatusInfo.pIconSyncVisiblePane = m_pPanes[PANE_TrainerIconSyncVisible];
      playerStatusInfo.pIconControlPane     = helper.GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_CRD_LOW_000_PANE_WINDOW_PLAYER_FRAME_04);

      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_LabelMatches]  = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeF_LabelMatches];
      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_Matches]       = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeF_Matches];
      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_Information]   = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeF_Information];
      playerStatusInfo.pTextBoxes[ViewUtility_Card::TEXTBOX_Player_1LineMessage]  = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeF_1LineMessage];

      /*  trainericon  */
      SetTrainerIconVisibilityInternal(0, true);
      SetPlayerStatus(&playerStatusInfo, isSkipIconLoad);

      teamInfo.pTextBox_TeamName  = m_pTextBoxes[LowerView_Card::TEXTBOX_TypeF_TeamName];
      SetTeamStatus(teamInfo);
      break;
  }

//  m_pMessageMenu->Show(rModeDef.messageMode);

  SelectBGM(0);
  GetG2DUtil()->Update2D();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
u32 LowerView_Card::Get1LiveMessageIDFromEntryWork(const bool isInternetMatch)
{
  u32       messageID = UINT_MAX;
  const u8  entryWork = isInternetMatch ? m_pWork->m_pBattleSpotSave->GetInternetEntryWork() : m_pWork->m_pBattleSpotSave->GetFriendEntryWork();

  switch(entryWork)
  {
    case  ENTRY_REGULATION:   ///< 2:レギュレーション取得済み
    case  ENTRY_BATTLEBOX:    ///< 3:バトルボックス登録完了
    case  ENTRY_UPLOAD:       ///< 4:全セーブデータアップ完了
      /*  さんかちゅうの　たいかい　です        */
      messageID = msg_bs_win_44;
      break;

    case  ENTRY_END:          ///< 5:大会終了
      /*  ぜんかい　さんかした　たいかい　です  */
      messageID = msg_bs_win_45;
      break;

    case  ENTRY_RETIRE:       ///< 6:リタイア
      /*  この　たいかいは　リタイア　しました  */
      messageID = msg_bs_win_46;
      break;

    case  ENTRY_CANCEL:       ///< 7:参加解除（失格など）
        /*  この　たいかいには　さんかできません  */
      messageID = msg_bs_win_47;
      break;

    case  ENTRY_NOT:          ///< 0:未エントリー
    case  ENTRY_FINISH:       ///< 1:エントリー完了
    default:
      GFL_ASSERT(false);    /*  想定外  */
      break;
  }
  
  return messageID;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
u32 LowerView_Card::GetSelectedBGM(void) const
{
  return NetApp::BgmSelect::NetworkBattleBgmSelect::GetBgmID( m_BGMIndex );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Card::SetLayoutMode(const LayoutMode layoutMode)
{
  m_pPanes[PANE_TypeB]->SetVisible(false);
  m_pPanes[PANE_TypeC]->SetVisible(false);
  m_pPanes[PANE_TypeD]->SetVisible(false);
  m_pPanes[PANE_TypeE]->SetVisible(false);
  m_pPanes[PANE_TypeF]->SetVisible(false);

  m_LayoutMode  = layoutMode;
  switch(m_LayoutMode)
  {
    case  LAYOUTMODE_TypeB:   m_pPanes[PANE_TypeB]->SetVisible(true);   break;   /**<  マッチング：レーティング  */
    case  LAYOUTMODE_TypeC:   m_pPanes[PANE_TypeC]->SetVisible(true);   break;   /**<  マッチング：フリー        */
    case  LAYOUTMODE_TypeD:   m_pPanes[PANE_TypeD]->SetVisible(true);   break;   /**<  マッチング：仲間大会      */
    case  LAYOUTMODE_TypeE:   m_pPanes[PANE_TypeE]->SetVisible(true);   break;   /**<  ?        */
    case  LAYOUTMODE_TypeF:   m_pPanes[PANE_TypeF]->SetVisible(true);   break;   /**<  大会情報：仲間大会        */

    case  LAYOUTMODE_None:    /*  fallthrough  */
    default:
      break;
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
void  LowerView_Card::SetCardViewMode(const bool bTeamRegistered)
{
  m_pPanes[PANE_TypeF_Team        ]->SetVisible( bTeamRegistered);
  m_pPanes[PANE_TypeF_Information ]->SetVisible(!bTeamRegistered);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Card::SetTournamentInfoEnable(const bool isEnable)
{
  m_pPanes[PANE_Info]->SetVisible(isEnable);
  SetButtonEnable(BUTTON_Info, isEnable);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Card::SetBGMSelectEnable(const bool isEnable)
{
  m_pPanes[PANE_BGM]->SetVisible(isEnable);
  SetButtonEnable(BUTTON_BGMSelect_Prev, isEnable);
  SetButtonEnable(BUTTON_BGMSelect_Next, isEnable);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Card::SetBackEnable(const bool isEnable)
{
  m_pPanes[PANE_Back]->SetVisible(isEnable);
  SetButtonEnable(BUTTON_Back, isEnable);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Card::SetQRView(const bool isQRVIsible)
{
  ViewUtility_Card::QRPaneInfo  info;

  info.pQRPicture = m_pQRView->GetQRPicture();

  SetQR(&info);
  m_pQRView->SetVisible(isQRVIsible);
}


void LowerView_Card::ResetPokeParty( void )
{
  m_PokePartyUtility.SetPokeParty( 0 , m_pWork->m_pAppParam->m_pPersonalData[0]->pPokeParty );
  m_PokePartyUtility.SetPokeParty( 1 , m_pWork->m_pAppParam->m_pPersonalData[0]->pPokeParty );
  m_PokePartyUtility.SetPokeParty( 2 , m_pWork->m_pAppParam->m_pPersonalData[0]->pPokeParty );
  m_PokePartyUtility.SetPokeParty( 3 , m_pWork->m_pAppParam->m_pPersonalData[0]->pPokeParty );
  m_PokePartyUtility.SetPokeParty( 4 , m_pWork->m_pAppParam->m_pPersonalData[0]->pPokeParty );
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
void    LowerView_Card::MessageMenuView_OnEvent(const MessageMenu::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  MessageMenu::IEventHandler::EVENT_OnEOM:     Event(LowerView_Card::IEventHandler::EVENT_Message_EOM);     break;   /**<  メッセージ表示が終端に到達                                  */
    case  MessageMenu::IEventHandler::EVENT_OnFinish:  Event(LowerView_Card::IEventHandler::EVENT_Message_Finish);  break;   /**<  EOM後、ユーザの入力まちなどまでが完了した                   */
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Card::Event(const IEventHandler::EventCode eventCode)
{
  if(m_pEventHandler)
  {
    m_pEventHandler->LowerView_Card_OnEvent(this, eventCode);
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
  void LowerView_Card::OnSetInputEnabled( bool isEnable )
  {
    if( m_pQRView )
    {
      m_pQRView->SetInputEnabled( isEnable );
    }
    if( m_pMessageMenu )
    {
      m_pMessageMenu->SetInputEnabled( isEnable );
    }
  }

} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
