//==============================================================================
/**
 * @file   UpperView_Main.cpp
 * @date   2015/07/14 Tue. 17:58:35
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "GameSys/include/GameManager.h"
#include  "GameSys/include/GameData.h"

#include  "AppLib/include/Util/app_util_GetPaneHelper.h"

#include  "Battle/Regulation/include/Regulation.h"
#include  "Savedata/include/LiveMatchSave.h"

#include  "../../LiveResource.h"
#include  "../../ApplicationSystem/LiveWork.h"

#include  "./UpperView_Main.h"


namespace
{
  enum  {
    LAYOUTWORK_Message,

    LAYOUTWORK_MAX
  };
} /*  namespace  */
// =============================================================================
/**/
//==============================================================================


namespace NetApp  {
namespace Live    {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
UpperView_Main::UpperView_Main(LiveWork* pWork)
  : NetAppLib::System::NetApplicationViewBase(
      pWork
      , RESOURCE_Layout
      , LYTRES_LIVE_LIVE_MIN_UPP_000_BFLYT
      , LA_LIVE_LIVE_MIN_UPP_000___NUM
      , app::util::G2DUtil::SETUP_UPPER
      , &(pWork->GetMessageUtility())
      , LIVE_MESSAGE_GRAC_ID
    )
  , NetAppLib::UI::NetAppTrainerIconVisibilityControl(pWork->GetAppHeap()->GetDeviceHeap(), 3)
  , m_pWork(pWork)
  , m_PartyUtil(m_pWork)
{
  app::util::G2DUtil*         pG2DUtil        = GetG2DUtil();
  gfl2::lyt::LytWk*           pLayoutWork     = pG2DUtil->GetLayoutWork(LAYOUTWORK_Message);
  gfl2::lyt::LytMultiResID&   rLytMultiResID  = pG2DUtil->GetLayoutResourceID(LAYOUTWORK_Message);
  app::util::GetPaneHelper    helper(pLayoutWork, &rLytMultiResID);

  /*  pane  */
  {
    m_TextBoxes[TEXTBOX_Date]               = helper.GetTextBoxPane(PANENAME_LIVE_MIN_UPP_000_PANE_TEXT_DAY);
    m_TextBoxes[TEXTBOX_LiveName1of2]       = helper.GetTextBoxPane(PANENAME_LIVE_MIN_UPP_000_PANE_TEXT_LIVENAME_00);
    m_TextBoxes[TEXTBOX_LiveName2of2]       = helper.GetTextBoxPane(PANENAME_LIVE_MIN_UPP_000_PANE_TEXT_LIVENAME_01);
    m_TextBoxes[TEXTBOX_Description]        = helper.GetTextBoxPane(PANENAME_LIVE_MIN_UPP_000_PANE_TEXT_DESCRIPTION);


    /*  VIEWMODE_TournamentNameWithID */
    {
      helper.Clear().Push(PANENAME_LIVE_MIN_UPP_000_PANE_IDCARD);
      pG2DUtil->SetTextBoxPaneString(helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXTBOX_01), m_pWork->GetString(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_07));
      pG2DUtil->SetTextBoxPaneString(helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXTBOX_02), m_pWork->GetString(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_11));
      pG2DUtil->SetTextBoxPaneString(helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXTBOX_03), m_pWork->GetString(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_13));

      m_TextBoxes[TEXTBOX_CARD_REMAIN_MATCH]  = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXT_99_00);
      m_TextBoxes[TEXTBOX_CARD_TOTAL_MATCH]   = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXT_99_01);
      m_TextBoxes[TEXTBOX_CARD_WINS]          = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXT_99_02);
      m_TextBoxes[TEXTBOX_CARD_LOSES]         = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXT_99_03);
      m_TextBoxes[TEXTBOX_CARD_TEAMNAME]      = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXTBOX_TEAMNAME);
      helper.Clear();
    }

    
    /*  VIEWMODE_TournamentNameWithPlayerInfo  */
    helper.Clear();
    m_TextBoxes[TEXTBOX_PlayerInfo1of4]     = helper.GetTextBoxPane(PANENAME_LIVE_MIN_UPP_000_PANE_TEXT_PLAYER_00);
    m_TextBoxes[TEXTBOX_PlayerInfo2of4]     = helper.GetTextBoxPane(PANENAME_LIVE_MIN_UPP_000_PANE_TEXT_PLAYER_01);
    m_TextBoxes[TEXTBOX_PlayerInfo3of4]     = helper.GetTextBoxPane(PANENAME_LIVE_MIN_UPP_000_PANE_TEXT_PLAYER_02);
    m_TextBoxes[TEXTBOX_PlayerInfo4of4]     = helper.GetTextBoxPane(PANENAME_LIVE_MIN_UPP_000_PANE_TEXT_PLAYER_03);


    /*  たいせんあいて  */
    pG2DUtil->SetTextBoxPaneString(m_TextBoxes[TEXTBOX_PlayerInfo1of4], msg_lt_sys_19);

    /*  VIEWMODE_BattleReplay  */
    m_TextBoxes[TEXTBOX_Replay_PlayerName_Mine]     = helper.GetTextBoxPane(PANENAME_LIVE_MIN_UPP_000_PANE_TEXTPLAYERNAME_00);
    m_TextBoxes[TEXTBOX_Replay_PlayerName_Opponent] = helper.GetTextBoxPane(PANENAME_LIVE_MIN_UPP_000_PANE_TEXTPLAYERNAME_01);
    m_TextBoxes[TEXTBOX_Replay_RemainMons_Mine]     = helper.GetTextBoxPane(PANENAME_LIVE_MIN_UPP_000_PANE_TEXTPOKENUMB_00);
    m_TextBoxes[TEXTBOX_Replay_RemainMons_Opponent] = helper.GetTextBoxPane(PANENAME_LIVE_MIN_UPP_000_PANE_TEXTPOKENUMB_01);


    /*  otherPanes  */
    helper.Clear();
    m_pPanes[PANE_Mine_Win]   = helper.GetPane(PANENAME_LIVE_MIN_UPP_000_PANE_VS_WIN_TEXT_00);
    m_pPanes[PANE_Mine_Lose]  = helper.GetPane(PANENAME_LIVE_MIN_UPP_000_PANE_VS_LOSE_TEXT_00);
    m_pPanes[PANE_Mine_Draw]  = helper.GetPane(PANENAME_LIVE_MIN_UPP_000_PANE_VS_DRAW_TEXT_00);
    m_pPanes[PANE_Oppo_Win]   = helper.GetPane(PANENAME_LIVE_MIN_UPP_000_PANE_VS_WIN_TEXT_01);
    m_pPanes[PANE_Oppo_Lose]  = helper.GetPane(PANENAME_LIVE_MIN_UPP_000_PANE_VS_LOSE_TEXT_01);
    m_pPanes[PANE_Oppo_Draw]  = helper.GetPane(PANENAME_LIVE_MIN_UPP_000_PANE_VS_DRAW_TEXT_01);

    /*  picture  */
    m_pPicture[PICTURE_Mine_Card]                 = helper.Clear().Push(PANENAME_LIVE_MIN_UPP_000_PANE_IDCARD).GetPicturePane(PANENAME_LIVE_PLT_000_PANE_PLAYERPHOTO);
    m_pPicture[PICTURE_Mine_Replay_Oppo_Matching] = helper.Clear().GetPicturePane(PANENAME_LIVE_MIN_UPP_000_PANE_PLAYERPHOTO_00);
    m_pPicture[PICTURE_Oppo_Replay]               = helper.Clear().GetPicturePane(PANENAME_LIVE_MIN_UPP_000_PANE_PLAYERPHOTO_01);

    /*  trainerIcon  */
    {
      const Savedata::MyStatus& rMyStatus = *(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus());
      
      app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA iconObjectData;

      iconObjectData.ConvFromMyStatusIconData( rMyStatus.GetIcon() );

      iconObjectData.pReplacePane             = m_pPicture[PICTURE_Mine_Card];
      iconObjectData.pSyncVisiblePane         = helper.Clear().GetPane(PANENAME_LIVE_MIN_UPP_000_PANE_NULL_ICONCTRL_CARD);
      iconObjectData.pInLoadVisibleChangePane = helper.Clear().Push(PANENAME_LIVE_MIN_UPP_000_PANE_IDCARD).GetPicturePane(PANENAME_LIVE_PLT_000_PANE_WINDOW_PLAYER_FRAME);
      iconObjectData.pTimerIconRootPane       = NULL;

      m_pWork->m_pTrainerIconUtil->SetIconObjectData( 0, iconObjectData );
      
      RegisterTrainerIconVisibilityControlPane(0, helper.Clear().GetPane(PANENAME_LIVE_MIN_UPP_000_PANE_NULL_ICONCTRL_CARD));
      RegisterTrainerIconVisibilityControlPane(1, helper.Clear().GetPane(PANENAME_LIVE_MIN_UPP_000_PANE_NULL_ICONCTRL_00));
      RegisterTrainerIconVisibilityControlPane(2, helper.Clear().GetPane(PANENAME_LIVE_MIN_UPP_000_PANE_NULL_ICONCTRL_01));
      ApplyTrainerIconVisibility();
    }
  }

  /*  PokePartyUtility  */
  {
    NetAppLib::UI::NetAppPokeparaUtility::RegisterInfo  info;


    info.pNameStringSource  = NULL;
    info.pLevelStringSource = m_pWork->GetString(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_18);

    m_PartyUtil.Setup(1, this, 0);
    helper.Clear().Push(PANENAME_LIVE_MIN_UPP_000_PANE_IDCARD);

    info.pVisibilityControl     = helper.GetPane(PANENAME_LIVE_PLT_000_PANE_POKE_00);
    info.pIconPicture           = helper.Push(PANENAME_LIVE_PLT_000_PANE_POKEICON_00).GetPicturePane(PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON); helper.Pop();
    info.pTextBox_Level         = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_POKE_00);
    info.pGenderIconParts       = helper.GetPartsPane(PANENAME_LIVE_PLT_000_PANE_GENDER_00);
    info.genderIconAnim_Male    = LA_LIVE_LIVE_MIN_UPP_000__IDCARD__GENDER_00_ICON_MALE;
    info.genderIconAnim_Female  = LA_LIVE_LIVE_MIN_UPP_000__IDCARD__GENDER_00_ICON_FEMALE;
    m_PartyUtil.RegisterStatusResource(0, 0, info);

    info.pVisibilityControl     = helper.GetPane(PANENAME_LIVE_PLT_000_PANE_POKE_01);
    info.pIconPicture           = helper.Push(PANENAME_LIVE_PLT_000_PANE_POKEICON_01).GetPicturePane(PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON); helper.Pop();
    info.pTextBox_Level         = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_POKE_01);
    info.pGenderIconParts       = helper.GetPartsPane(PANENAME_LIVE_PLT_000_PANE_GENDER_01);
    info.genderIconAnim_Male    = LA_LIVE_LIVE_MIN_UPP_000__IDCARD__GENDER_01_ICON_MALE;
    info.genderIconAnim_Female  = LA_LIVE_LIVE_MIN_UPP_000__IDCARD__GENDER_01_ICON_FEMALE;
    m_PartyUtil.RegisterStatusResource(0, 1, info);

    info.pVisibilityControl     = helper.GetPane(PANENAME_LIVE_PLT_000_PANE_POKE_02);
    info.pIconPicture           = helper.Push(PANENAME_LIVE_PLT_000_PANE_POKEICON_02).GetPicturePane(PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON); helper.Pop();
    info.pTextBox_Level         = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_POKE_02);
    info.pGenderIconParts       = helper.GetPartsPane(PANENAME_LIVE_PLT_000_PANE_GENDER_02);
    info.genderIconAnim_Male    = LA_LIVE_LIVE_MIN_UPP_000__IDCARD__GENDER_02_ICON_MALE;
    info.genderIconAnim_Female  = LA_LIVE_LIVE_MIN_UPP_000__IDCARD__GENDER_02_ICON_FEMALE;
    m_PartyUtil.RegisterStatusResource(0, 2, info);

    info.pVisibilityControl     = helper.GetPane(PANENAME_LIVE_PLT_000_PANE_POKE_03);
    info.pIconPicture           = helper.Push(PANENAME_LIVE_PLT_000_PANE_POKEICON_03).GetPicturePane(PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON); helper.Pop();
    info.pTextBox_Level         = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_POKE_03);
    info.pGenderIconParts       = helper.GetPartsPane(PANENAME_LIVE_PLT_000_PANE_GENDER_03);
    info.genderIconAnim_Male    = LA_LIVE_LIVE_MIN_UPP_000__IDCARD__GENDER_03_ICON_MALE;
    info.genderIconAnim_Female  = LA_LIVE_LIVE_MIN_UPP_000__IDCARD__GENDER_03_ICON_FEMALE;
    m_PartyUtil.RegisterStatusResource(0, 3, info);

    info.pVisibilityControl     = helper.GetPane(PANENAME_LIVE_PLT_000_PANE_POKE_04);
    info.pIconPicture           = helper.Push(PANENAME_LIVE_PLT_000_PANE_POKEICON_04).GetPicturePane(PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON); helper.Pop();
    info.pTextBox_Level         = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_POKE_04);
    info.pGenderIconParts       = helper.GetPartsPane(PANENAME_LIVE_PLT_000_PANE_GENDER_04);
    info.genderIconAnim_Male    = LA_LIVE_LIVE_MIN_UPP_000__IDCARD__GENDER_04_ICON_MALE;
    info.genderIconAnim_Female  = LA_LIVE_LIVE_MIN_UPP_000__IDCARD__GENDER_04_ICON_FEMALE;
    m_PartyUtil.RegisterStatusResource(0, 4, info);

    info.pVisibilityControl     = helper.GetPane(PANENAME_LIVE_PLT_000_PANE_POKE_05);
    info.pIconPicture           = helper.Push(PANENAME_LIVE_PLT_000_PANE_POKEICON_05).GetPicturePane(PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON); helper.Pop();
    info.pTextBox_Level         = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_POKE_05);
    info.pGenderIconParts       = helper.GetPartsPane(PANENAME_LIVE_PLT_000_PANE_GENDER_05);
    info.genderIconAnim_Male    = LA_LIVE_LIVE_MIN_UPP_000__IDCARD__GENDER_05_ICON_MALE;
    info.genderIconAnim_Female  = LA_LIVE_LIVE_MIN_UPP_000__IDCARD__GENDER_05_ICON_FEMALE;
    m_PartyUtil.RegisterStatusResource(0, 5, info);
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
UpperView_Main::~UpperView_Main()
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
void  UpperView_Main::Update(void)
{
  NetAppLib::System::NetApplicationViewBase::Update();  /*  super  */
  m_PartyUtil.OnUpdate();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Main::Draw(gfl2::gfx::CtrDisplayNo displayNo)
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
bool  UpperView_Main::IsDrawing(void) const
{
  bool  isDrawing = false;

  isDrawing |= !m_PartyUtil.Cleanup();
  isDrawing |= NetAppLib::System::NetApplicationViewBase::IsDrawing();

  return isDrawing;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Main::SetViewMode(const ViewMode viewMode)
{
  u32 animID  = UINT_MAX;

  SetAllTrainerIconVisibilityInternal(false);
  UpdateTournamentInfo();
  switch(viewMode)
  {
    case  VIEWMODE_TournamentName:                animID  = LA_LIVE_LIVE_SLC_LOW_000_PATERN_00; break;  /**<  大会名のみ              */
    case  VIEWMODE_TournamentNameWith2Line:       animID  = LA_LIVE_LIVE_SLC_LOW_000_PATERN_02; break;  /**<  大会名 + 2行情報        */

    case  VIEWMODE_TournamentNameWithPlayerInfo:
      SetTrainerIconVisibilityInternal(2, true);
      animID  = LA_LIVE_LIVE_SLC_LOW_000_PATERN_03;
      UpdateOpponentInfo();
      break;  /**<  大会名 + プレイヤー情報（相手） */

    case  VIEWMODE_TournamentNameWithID:
      SetTrainerIconVisibilityInternal(0, true);
      animID  = LA_LIVE_LIVE_SLC_LOW_000_PATERN_01; UpdateIDCard();
      break;  /**<  大会名 + ID情報（自分）         */


    case  VIEWMODE_BattleReplay:
      SetTrainerIconVisibilityInternal(1, true);
      SetTrainerIconVisibilityInternal(2, true);
      animID  = LA_LIVE_LIVE_MIN_UPP_000_PATERN_04; UpdateBattleReplayInfo();
      break;  /**<  バトル再生情報          */



    default:
      /*  nop  */
      break;
  }

  if(animID  != UINT_MAX)
  {
    GetG2DUtil()->StartAnime(0, animID);
    GetG2DUtil()->Update2D();
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
void  UpperView_Main::UpdateTournamentInfo(void)
{
  app::util::G2DUtil*                   pG2DUtil    = GetG2DUtil();
  NetAppLib::Message::MessageUtility&   rMessage    = m_pWork->GetMessageUtility();
  print::WordSet&                       rWord       = *(rMessage.GetWordSet());
  const Regulation&                     rRegulation = *(m_pWork->m_pAppParam->m_pSavedRegulation);
  const u32                             digits      = 2;
  const u32                             digits_year = 4;
  const print::NumberDispType           dispType    = print::NUM_DISP_ZERO;
  const print::NumberCodeType           codeType    = print::NUM_CODE_HANKAKU;



  /*  tournamentname  */
  {
    gfl2::str::StrBuf&  rStr  = rMessage.GetStrBuf();

    rRegulation.GetTextParam(Regulation::CUPNAME1, &rStr);
    pG2DUtil->SetTextBoxPaneString(m_TextBoxes[TEXTBOX_LiveName1of2], &rStr);

    rRegulation.GetTextParam(Regulation::CUPNAME2, &rStr);
    pG2DUtil->SetTextBoxPaneString(m_TextBoxes[TEXTBOX_LiveName2of2], &rStr);
  }

  /*  date  */
  {
    rWord.RegisterNumber(0, rRegulation.GetValueParam(Regulation::START_YEAR),  digits_year,  dispType, codeType);
    rWord.RegisterNumber(1, rRegulation.GetValueParam(Regulation::START_MONTH), digits,       dispType, codeType);
    rWord.RegisterNumber(2, rRegulation.GetValueParam(Regulation::START_DAY),   digits,       dispType, codeType);

    pG2DUtil->SetTextBoxPaneString(m_TextBoxes[TEXTBOX_Date], &(rMessage.GetExpandedString(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_05)));
  }
}




//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Main::UpdateOpponentInfo(void)
{
  app::util::G2DUtil*                   pG2DUtil    = GetG2DUtil();
  gfl2::lyt::LytWk*                     pLayoutWork = pG2DUtil->GetLayoutWork(LAYOUTWORK_Message);
  gfl2::lyt::LytMultiResID&             rLytMultiResID  = pG2DUtil->GetLayoutResourceID(LAYOUTWORK_Message);
  NetAppLib::Message::MessageUtility&   rMessage    = m_pWork->GetMessageUtility();
  print::WordSet&                       rWord       = *(rMessage.GetWordSet());
  const Regulation&                     rRegulation = *(m_pWork->m_pAppParam->m_pSavedRegulation);
  const Savedata::MyStatus&             rOppoStatus = *(m_pWork->m_pAppParam->m_pLiveNet->GetMyStatus_Opponent());

  app::util::GetPaneHelper              helper(pLayoutWork, &rLytMultiResID);

  app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA iconObjectData;

  iconObjectData.ConvFromMyStatusIconData( rOppoStatus.GetIcon() );

  iconObjectData.pReplacePane             = m_pPicture[PICTURE_Mine_Replay_Oppo_Matching];
  iconObjectData.pSyncVisiblePane         = helper.Clear().GetPane(PANENAME_LIVE_MIN_UPP_000_PANE_NULL_ICONCTRL_01);
  iconObjectData.pInLoadVisibleChangePane = helper.Clear().GetPicturePane(PANENAME_LIVE_MIN_UPP_000_PANE_WINDOW_PLAYER_FRAME_00);
  iconObjectData.pTimerIconRootPane       = NULL;

  m_pWork->m_pTrainerIconUtil->SetIconObjectData( 2, iconObjectData );


  {
    pG2DUtil->SetRegisterPlayerName(0, &rOppoStatus);
    pG2DUtil->SetTextBoxPaneStringExpand(m_TextBoxes[TEXTBOX_PlayerInfo2of4], msg_lt_sys_20);    /*  name      */
  }

  {
    gfl2::str::StrBuf*  pDstStr = &(rMessage.GetStrBuf());

    pDstStr->SetStr(m_pWork->GetCountry(&rOppoStatus));
    pG2DUtil->SetRegisterWord(0, *pDstStr);
    pG2DUtil->SetTextBoxPaneStringExpand(m_TextBoxes[TEXTBOX_PlayerInfo3of4], msg_lt_sys_21);    /*  country   */
  }

  {
    gfl2::str::StrBuf*  pDstStr = &(rMessage.GetStrBuf());

    pDstStr->SetStr(m_pWork->GetRegion(&rOppoStatus));
    pG2DUtil->SetRegisterWord(0, *pDstStr);
    pG2DUtil->SetTextBoxPaneStringExpand(m_TextBoxes[TEXTBOX_PlayerInfo4of4], msg_lt_sys_22);    /*  region    */
  }
 
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Main::UpdateIDCard(void)
{
  app::util::G2DUtil*                           pG2DUtil    = GetG2DUtil();
  APP_PARAM*                                    pAppParam   = m_pWork->m_pAppParam;
  Regulation*                                   pRegulation = pAppParam->m_pSavedRegulation;
  const Savedata::LiveMatchData::LIVE_RESULTS*  pResults    = pAppParam->m_pLiveMatchData->GetLiveResults();
  const print::NumberDispType                   dispType    = print::NUM_DISP_SPACE;
  const u32                                     digits      = 2;
  NetAppLib::Message::MessageUtility&           rMessage    = m_pWork->GetMessageUtility();
  print::WordSet&                               rWord       = *(rMessage.GetWordSet());


  m_PartyUtil.SetPokeParty(0, pAppParam->m_pMyBattleParty->GetPokeParty());

  /*  teamname  */
  pG2DUtil->SetRegisterWord(0, *(pAppParam->m_pMyBattleParty->GetTeamName()));
  pG2DUtil->SetTextBoxPaneStringExpand(m_TextBoxes[TEXTBOX_CARD_TEAMNAME],  msg_lt_sys_15);

  /*  stats  */
  /*
    NMCat[1478]   レギュレーションデータの「対戦回数」がライブ大会メニュー画面に表示されない
      LiveCompetitionでは大会の規定対戦数を表示する
      ※(一日の最大対戦数 < 大会の対戦数)とはならない運用である
  */
  {
    const u32 battleNumMax  = pRegulation->GetValueParam(Regulation::BATTLE_NUM);

    pG2DUtil->SetTextBoxPaneNumber(m_TextBoxes[TEXTBOX_CARD_REMAIN_MATCH],    m_pWork->GetString(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_10), pResults->battle_num,  digits, 0, dispType, print::NUM_CODE_DEFAULT);
    pG2DUtil->SetTextBoxPaneNumber(m_TextBoxes[TEXTBOX_CARD_TOTAL_MATCH],     m_pWork->GetString(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_08), battleNumMax,          digits, 0, dispType, print::NUM_CODE_DEFAULT);
    pG2DUtil->SetTextBoxPaneNumber(m_TextBoxes[TEXTBOX_CARD_WINS],            m_pWork->GetString(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_12), pResults->win_num,     digits, 0, dispType, print::NUM_CODE_DEFAULT);
    pG2DUtil->SetTextBoxPaneNumber(m_TextBoxes[TEXTBOX_CARD_LOSES],           m_pWork->GetString(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_14), pResults->lose_num,    digits, 0, dispType, print::NUM_CODE_DEFAULT);
  }
}



//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Main::UpdateBattleReplayInfo(void)
{
  gfl2::heap::HeapBase*                       pHeapBase         = m_pWork->GetDeviceHeap()->GetLowerHandle();
  const APP_PARAM*                            pAppParam         = m_pWork->m_pAppParam;
  const Savedata::MyStatus*                   pMyStatus_Mine    = pAppParam->m_pBattleRecorderData->GetMyStatus(BTL_CLIENT_PLAYER, pHeapBase);   /*  要解放  */
  const Savedata::MyStatus*                   pMyStatus_Oppo    = pAppParam->m_pBattleRecorderData->GetMyStatus(BTL_CLIENT_ENEMY1, pHeapBase);   /*  要解放  */
  const Savedata::LiveMatchData::LIVE_RECORD* pLastRecord       = pAppParam->m_pLiveMatchData->GetLastLiveRecord();
  const u8                                    remainMons_Mine   = pLastRecord->reporter_poke;
  const u8                                    remainMons_Oppo   = pLastRecord->opponent_poke;
  const u32                                   remainMons_Digits = 1;
  app::util::G2DUtil*                         pG2DUtil          = GetG2DUtil();
  gfl2::lyt::LytWk*                           pLayoutWork       = pG2DUtil->GetLayoutWork(LAYOUTWORK_Message);
  gfl2::lyt::LytMultiResID&                   rLytMultiResID    = pG2DUtil->GetLayoutResourceID(LAYOUTWORK_Message);
  NetAppLib::Message::MessageUtility&         rMessage          = m_pWork->GetMessageUtility();

  app::util::GetPaneHelper                    helper(pLayoutWork, &rLytMultiResID);

  const ExtSavedata::BattleRecorderSaveData::BATTLE_REC_UPLOAD_DATA*  pRecUploadData  = pAppParam->m_pBattleRecorderData->GetBattleRecUploadData();


  /*  tournamentname  */
  {
    gfl2::str::StrBuf&  rStr  = rMessage.GetStrBuf();

    pG2DUtil->SetTextBoxPaneString(m_TextBoxes[TEXTBOX_LiveName1of2], pRecUploadData->header.cupname1);
    pG2DUtil->SetTextBoxPaneString(m_TextBoxes[TEXTBOX_LiveName2of2], pRecUploadData->header.cupname2);
  }

  /*  result  */
  {
    if(pLastRecord->battle_result == 2)
    {
      /*  引き分け  */
      pG2DUtil->SetTextBoxPaneString(m_TextBoxes[TEXTBOX_Description], msg_lt_sys_24);
    }
    else
    {
      if(pLastRecord->battle_result == 0)
      {
        /*  0:勝利  */
        pG2DUtil->SetRegisterPlayerName(0, pMyStatus_Mine);
        pG2DUtil->SetRegisterPlayerName(1, pMyStatus_Oppo);

      }
      else
      {
        /*  1:敗北  */
        pG2DUtil->SetRegisterPlayerName(0, pMyStatus_Oppo);
        pG2DUtil->SetRegisterPlayerName(1, pMyStatus_Mine);
      }

      /*  reason  */
      {
        u32 messageID = msg_lt_sys_23;

        /*
          2016/05/03  Yu Muto
          川内丸さん/寺地さんに確認し、勝敗が決しておりReasonが1だった場合は必ず判定勝利と表示するようにした
        */
        switch(pLastRecord->battle_result_reason)
        {
          case  1:  messageID = msg_lt_sys_25/*msg_lt_sys_26*/;  break;  /*  1)時間切れ  */
          case  2:  messageID = msg_lt_sys_27;                   break;  /*  3)降参      */

          /*  0)全てのポケモンが瀕死  */
          case  0:
          default:
            /*  nop  */
            break;
        }

        pG2DUtil->SetTextBoxPaneStringExpand(m_TextBoxes[TEXTBOX_Description], messageID);
      }
    }
  }








  /*  trainer  */
  {
    gfl2::str::StrBuf*  pStr  = m_pWork->GetString(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_28);
    pG2DUtil->SetTextBoxPaneNumber(m_TextBoxes[TEXTBOX_Replay_RemainMons_Mine],     pStr, remainMons_Mine, remainMons_Digits, 0, print::NUM_DISP_SPACE, print::NUM_CODE_DEFAULT);
    pG2DUtil->SetTextBoxPaneNumber(m_TextBoxes[TEXTBOX_Replay_RemainMons_Opponent], pStr, remainMons_Oppo, remainMons_Digits, 0, print::NUM_DISP_SPACE, print::NUM_CODE_DEFAULT);

    pG2DUtil->SetRegisterPlayerName(0, pMyStatus_Mine);
    pG2DUtil->SetTextBoxPaneStringExpand(m_TextBoxes[TEXTBOX_Replay_PlayerName_Mine],     msg_lt_sys_29);
    pG2DUtil->SetRegisterPlayerName(0, pMyStatus_Oppo);
    pG2DUtil->SetTextBoxPaneStringExpand(m_TextBoxes[TEXTBOX_Replay_PlayerName_Opponent], msg_lt_sys_29);

    app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA iconObjectData0;
    app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA iconObjectData1;

    iconObjectData0.ConvFromMyStatusIconData( pMyStatus_Mine->GetIcon() );
    iconObjectData1.ConvFromMyStatusIconData( pMyStatus_Oppo->GetIcon() );

    iconObjectData0.pReplacePane             = m_pPicture[PICTURE_Mine_Replay_Oppo_Matching];
    iconObjectData0.pSyncVisiblePane         = helper.Clear().GetPane(PANENAME_LIVE_MIN_UPP_000_PANE_NULL_ICONCTRL_00);
    iconObjectData0.pInLoadVisibleChangePane = helper.Clear().GetPicturePane(PANENAME_LIVE_MIN_UPP_000_PANE_WINDOW_PLAYER_FRAME_00);
    iconObjectData0.pTimerIconRootPane       = NULL;

    iconObjectData1.pReplacePane             = m_pPicture[PICTURE_Oppo_Replay];
    iconObjectData1.pSyncVisiblePane         = helper.Clear().GetPane(PANENAME_LIVE_MIN_UPP_000_PANE_NULL_ICONCTRL_01);
    iconObjectData1.pInLoadVisibleChangePane = helper.Clear().GetPicturePane(PANENAME_LIVE_MIN_UPP_000_PANE_WINDOW_PLAYER_FRAME_01);
    iconObjectData1.pTimerIconRootPane       = NULL;

    m_pWork->m_pTrainerIconUtil->SetIconObjectData( 1, iconObjectData0 );
    m_pWork->m_pTrainerIconUtil->SetIconObjectData( 2, iconObjectData1 );

  }

  /*  win/lose/draw  */
  {
    if(pLastRecord->battle_result == 0)
    {
      /*  勝利  */
      m_pPanes[PANE_Mine_Win]->SetVisible(true);
      m_pPanes[PANE_Mine_Lose]->SetVisible(false);
      m_pPanes[PANE_Mine_Draw]->SetVisible(false);
      m_pPanes[PANE_Oppo_Win]->SetVisible(false);
      m_pPanes[PANE_Oppo_Lose]->SetVisible(true);
      m_pPanes[PANE_Oppo_Draw]->SetVisible(false);
    }
    else if(pLastRecord->battle_result == 1)
    {
      /*  敗北  */
      m_pPanes[PANE_Mine_Win]->SetVisible(false);
      m_pPanes[PANE_Mine_Lose]->SetVisible(true);
      m_pPanes[PANE_Mine_Draw]->SetVisible(false);
      m_pPanes[PANE_Oppo_Win]->SetVisible(true);
      m_pPanes[PANE_Oppo_Lose]->SetVisible(false);
      m_pPanes[PANE_Oppo_Draw]->SetVisible(false);
    }
    else
    {
      /*  引き分け  */
      m_pPanes[PANE_Mine_Win]->SetVisible(false);
      m_pPanes[PANE_Mine_Lose]->SetVisible(false);
      m_pPanes[PANE_Mine_Draw]->SetVisible(true);
      m_pPanes[PANE_Oppo_Win]->SetVisible(false);
      m_pPanes[PANE_Oppo_Lose]->SetVisible(false);
      m_pPanes[PANE_Oppo_Draw]->SetVisible(true);
    }

  }



  GFL_SAFE_DELETE(pMyStatus_Mine);
  GFL_SAFE_DELETE(pMyStatus_Oppo);
}



} /*  namespace Live    */
} /*  namespace NetApp  */

