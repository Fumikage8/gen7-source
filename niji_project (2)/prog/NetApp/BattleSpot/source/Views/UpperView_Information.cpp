//==============================================================================
/**
 * @file   UpperView_Information.cpp
 * @date   2015/10/06 Tue. 15:18:17
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
#include  "../System/BattleSpotWork.h"

#include  "../BattleSpotResource.h"
#include  "./UpperView_Information.h"

#include  "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"

// =============================================================================
/**/
//==============================================================================
namespace
{
}


namespace NetApp      {
namespace BattleSpot  {
// =============================================================================
/**/
//==============================================================================
UpperView_Information::UpperView_Information(BattleSpotWork* pWork)
  : NetAppLib::UI::NetAppEmptyView(pWork)
  , ViewUtility_Card(pWork)
  , m_pWork(pWork)
  , m_ViewMode(VIEWMODE_None)
  , m_iconIndex(0)
{
  Create2DOneLayout(
    m_pWork,
    RESOURCE_Layout,
    LYTRES_BATTLESPOT_BATTLESPOT_MSG_UPP_000_BFLYT,
    LA_BATTLESPOT_BATTLESPOT_MSG_UPP_000___NUM,
    app::util::G2DUtil::SETUP_UPPER,
    m_pWork->GetMessageUtility(),
    BATTLESPOT_MESSAGE_ARCDAT
  );

  UpperView_Information::SetG2DUtil(GetG2DUtil());
  
  {
    app::util::G2DUtil*       pG2DUtil  = GetG2DUtil();
    app::util::GetPaneHelper  helper(pG2DUtil->GetLayoutWork(0), pG2DUtil->GetLayoutResourceID());

    /*  textbox  */
    m_pTextBoxes[TEXTBOX_Title_1Line]               = helper.Clear().GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_TEXTBOX_L1);
    m_pTextBoxes[TEXTBOX_Title_2Lines_0]            = helper.Clear().GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_TEXTBOX_L2_00);
    m_pTextBoxes[TEXTBOX_Title_2Lines_1]            = helper.Clear().GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_TEXTBOX_L2_01);
    m_pTextBoxes[TEXTBOX_Description]               = helper.Clear().GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_TEXTBOX_A_00);


    /*  typeB */
    m_pTextBoxes[TEXTBOX_TypeB_PlayerName]          = helper.Clear().GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERNAME_B_00);
    m_pTextBoxes[TEXTBOX_TypeB_Rate_PlayerName]     = helper.Clear().GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERNAME_B_01);
    m_pTextBoxes[TEXTBOX_TypeB_Rate_RateLabel]      = helper.Clear().GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERRATE_B_00);
    m_pTextBoxes[TEXTBOX_TypeB_Rate_RateValue]      = helper.Clear().GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERRATE_B_01);

    /*  typeC */
    m_pTextBoxes[TEXTBOX_TypeC_PlayerName]          = helper.Clear().GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERNAME_C_00);

    /*  typeD */
    m_pTextBoxes[TEXTBOX_TypeD_Player0_NameLabel]   = helper.Clear().GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERNAME_D_00);
    m_pTextBoxes[TEXTBOX_TypeD_Player1_NameLabel]   = helper.Clear().GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERNAME_D_01);
    m_pTextBoxes[TEXTBOX_TypeD_Player2_NameLabel]   = helper.Clear().GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERNAME_D_02);
    m_pTextBoxes[TEXTBOX_TypeD_Player0_Name]        = helper.Clear().GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERRATE_D_00);
    m_pTextBoxes[TEXTBOX_TypeD_Player1_Name]        = helper.Clear().GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERRATE_D_01);
    m_pTextBoxes[TEXTBOX_TypeD_Player2_Name]        = helper.Clear().GetTextBoxPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERRATE_D_02);

    m_pControlPanes[CONTROLPANE_TitleWindow]        = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_NULL_TITLEWINDOW);
    m_pControlPanes[CONTROLPANE_TitleWindow_1Line]  = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_NULL_TITLEWINDOW_1LINE);
    m_pControlPanes[CONTROLPANE_TitleWindow_2Lines] = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_NULL_TITLEWINDOW_2LINES);
    m_pControlPanes[CONTROLPANE_TypeA]              = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_TYPE_A);
    m_pControlPanes[CONTROLPANE_TypeB]              = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_TYPE_B);
    m_pControlPanes[CONTROLPANE_TypeB_NameOnly]     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_NULL_TYPEB_NAMEONLY);
    m_pControlPanes[CONTROLPANE_TypeB_NameRate]     = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_NULL_TYPEB_NAMERATE);
    m_pControlPanes[CONTROLPANE_TypeC]              = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_TYPE_C);
    m_pControlPanes[CONTROLPANE_TypeD]              = helper.Clear().GetPane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_TYPE_D);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
UpperView_Information::~UpperView_Information()
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
void  UpperView_Information::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  app::util::G2DUtil* pG2DUtil  = GetG2DUtil();

  if(pG2DUtil)
    pG2DUtil->AddDrawableLytWkForOneFrame(m_pWork->GetAppRenderingManager(), displayNo, 0);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  UpperView_Information::SetViewMode(const ViewMode viewMode)
{
  //NetAppLib::Message::MessageUtility* pMessageUtil  = m_pWork->GetMessageUtility();


  switch(viewMode)
  {
    case  VIEWMODE_None:
      ResetTitle();
      SetLayoutMode(LAYOUTMODE_None);
      break;

    /*  welcome  */
    case  VIEWMODE_Welcome:
      Set1LineTitle(msg_bs_sys_01);
      SetDescription(msg_bs_sys_12);
      break;

    /*
      freebattle
    */
    case  VIEWMODE_FreeBattle:
      Set1LineTitle(msg_bs_sys_03);
      SetDescription(msg_bs_sys_13);
      break;

    /*  マッチング      */
    case  VIEWMODE_FreeBattle_Matching_Single:
      Set2LinesTitle(msg_bs_sys_03, msg_bs_sys_04);
      SetLayoutMode(LAYOUTMODE_None);
      break;

    case  VIEWMODE_FreeBattle_Matching_Double:
      Set2LinesTitle(msg_bs_sys_03, msg_bs_sys_05);
      SetLayoutMode(LAYOUTMODE_None);
      break;

    case  VIEWMODE_FreeBattle_Matching_BattleRoyal:
      Set2LinesTitle(msg_bs_sys_03, msg_bs_sys_07);
      SetLayoutMode(LAYOUTMODE_None);
      break;

    /*  マッチング完了  */
    case  VIEWMODE_FreeBattle_Matched_Single:
      Set2LinesTitle(msg_bs_sys_03, msg_bs_sys_04);
      SetLayoutMode(LAYOUTMODE_TypeB);
      break;

    case  VIEWMODE_FreeBattle_Matched_Double:
      Set2LinesTitle(msg_bs_sys_03, msg_bs_sys_05);
      SetLayoutMode(LAYOUTMODE_TypeB);
      break;

    case  VIEWMODE_FreeBattle_Matched_BattleRoyal:
      Set2LinesTitle(msg_bs_sys_03, msg_bs_sys_07);
      SetLayoutMode(LAYOUTMODE_TypeD);
      break;

    /*  再戦確認  */
    case  VIEWMODE_FreeBattle_Rematch_Single:
      Set2LinesTitle(msg_bs_sys_03, msg_bs_sys_04);
      SetLayoutMode(LAYOUTMODE_None);
      break;

    case  VIEWMODE_FreeBattle_Rematch_Double:
      Set2LinesTitle(msg_bs_sys_03, msg_bs_sys_05);
      SetLayoutMode(LAYOUTMODE_None);
      break;

    case  VIEWMODE_FreeBattle_Rematch_BattleRoyal:
      Set2LinesTitle(msg_bs_sys_03, msg_bs_sys_07);
      SetLayoutMode(LAYOUTMODE_None);
      break;




    /*
      ratingBattle
    */
    case  VIEWMODE_RatingBattle:
      Set1LineTitle(msg_bs_sys_02);
      SetDescription(msg_bs_sys_14);
      break;


    /*  マッチング      */
    case  VIEWMODE_RatingBattle_Matching_Single:
      Set2LinesTitle(msg_bs_sys_02, msg_bs_sys_04);
      SetLayoutMode(LAYOUTMODE_None);
      break;

    case  VIEWMODE_RatingBattle_Matching_Double:
      Set2LinesTitle(msg_bs_sys_02, msg_bs_sys_05);
      SetLayoutMode(LAYOUTMODE_None);
      break;

    case  VIEWMODE_RatingBattle_Matching_Special:
      Set2LinesTitle(msg_bs_sys_02, msg_bs_sys_06);
      SetLayoutMode(LAYOUTMODE_None);
      break;
    case  VIEWMODE_RatingBattle_Matching_WCS:
      Set2LinesTitle(msg_bs_sys_02, msg_bs_sel_23);
      SetLayoutMode(LAYOUTMODE_None);
      break;

    /*  マッチング完了  */
    case  VIEWMODE_RatingBattle_Matched_Single:
      Set2LinesTitle(msg_bs_sys_02, msg_bs_sys_04);
      SetLayoutMode(LAYOUTMODE_TypeB_Rate);
      break;

    case  VIEWMODE_RatingBattle_Matched_Double:
      Set2LinesTitle(msg_bs_sys_02, msg_bs_sys_05);
      SetLayoutMode(LAYOUTMODE_TypeB_Rate);
      break;

    case  VIEWMODE_RatingBattle_Matched_Special:
      Set2LinesTitle(msg_bs_sys_02, msg_bs_sys_06);
      SetLayoutMode(LAYOUTMODE_TypeB_Rate);
      break;

    case  VIEWMODE_RatingBattle_Matched_WCS:
      Set2LinesTitle(msg_bs_sys_02, msg_bs_sel_23);
      SetLayoutMode(LAYOUTMODE_TypeB_Rate);
      break;

    /*  再戦確認  */
    case  VIEWMODE_RatingBattle_Rematch_Single:
      Set2LinesTitle(msg_bs_sys_02, msg_bs_sys_04);
      SetLayoutMode(LAYOUTMODE_None);
      break;

    case  VIEWMODE_RatingBattle_Rematch_Double:
      Set2LinesTitle(msg_bs_sys_02, msg_bs_sys_05);
      SetLayoutMode(LAYOUTMODE_None);
      break;

    case  VIEWMODE_RatingBattle_Rematch_Special:
      Set2LinesTitle(msg_bs_sys_02, msg_bs_sys_06);
      SetLayoutMode(LAYOUTMODE_None);
      break;
    case  VIEWMODE_RatingBattle_Rematch_WCS:
      Set2LinesTitle(msg_bs_sys_02, msg_bs_sel_23);
      SetLayoutMode(LAYOUTMODE_None);
      break;
    
    case  VIEWMODE_RatingBattle_BattleVideoConfirm:
      Set1LineTitle(msg_bs_sys_02);
      SetDescription(msg_bs_sys_18);
      break;


    /*
      internetmatch
    */
    case  VIEWMODE_InternetMatch:
      Set1LineTitle(msg_bs_sys_08);
      SetDescription(msg_bs_sys_15);
      break;

    case  VIEWMODE_InternetMatch_Card:              /**<  選手証          */
      Set2LinesTitle(msg_bs_sys_09, msg_bs_sys_10);
      SetLayoutMode(LAYOUTMODE_None);
      break;

    case  VIEWMODE_InternetMatch_Information:       /**<  大会情報        */
      Set2LinesTitle(msg_bs_sys_09, msg_bs_sys_10);
      SetDescription(msg_bs_sys_17);
      break;


    case  VIEWMODE_InternetMatch_Matching:          /**<  マッチング中    */
      Set2LinesTitle(msg_bs_sys_09, msg_bs_sys_10);
      SetLayoutMode(LAYOUTMODE_None);
      break;

    case  VIEWMODE_InternetMatch_Matched:           /**<  マッチング完了  */
      Set2LinesTitle(msg_bs_sys_09, msg_bs_sys_10);
      SetLayoutMode(LAYOUTMODE_TypeB_Rate);
      break;

    case  VIEWMODE_InternetMatch_Message:           /**<  （通信エラーなどの）メッセージ表示  */
      ResetTitle();
      SetLayoutMode(LAYOUTMODE_None);
      break;


    /*
      friendmatch
    */
    case  VIEWMODE_FriendMatch:
      Set1LineTitle(msg_bs_sys_11);
      SetDescription(msg_bs_sys_16);
      break;

    case  VIEWMODE_FriendMatch_Card:                /**<  選手証          */
      Set2LinesTitle(msg_bs_sys_09, msg_bs_sys_10);
      SetLayoutMode(LAYOUTMODE_None);
      break;

    case  VIEWMODE_FriendMatch_Information:         /**<  大会情報        */
      Set2LinesTitle(msg_bs_sys_09, msg_bs_sys_10);
      SetDescription(msg_bs_sys_17);
      break;

    case  VIEWMODE_FriendMatch_Matching:            /**<  マッチング中    */
      Set2LinesTitle(msg_bs_sys_09, msg_bs_sys_10);
      SetLayoutMode(LAYOUTMODE_None);
      break;

    case  VIEWMODE_FriendMatch_Matched:             /**<  マッチング完了  */
      Set2LinesTitle(msg_bs_sys_09, msg_bs_sys_10);
      SetLayoutMode(LAYOUTMODE_TypeB_Rate);
      break;

    case  VIEWMODE_FriendMatch_Message:             /**<  （通信エラーなどの）メッセージ表示  */
      ResetTitle();
      SetLayoutMode(LAYOUTMODE_None);
      break;






  }
}


void UpperView_Information::StartIconSetup(const ViewMode viewMode)
{
  app::util::GetPaneHelper  helper(GetG2DUtil()->GetLayoutWork(0), &GetG2DUtil()->GetLayoutResourceID(0));
  const Savedata::MyStatus*   pMyStatus_Oppo0 = m_pWork->GetTargetStatus(BattleSpotWork::REGIONTARGET_Opponent0);
  const Savedata::MyStatus*   pMyStatus_Oppo1 = m_pWork->GetTargetStatus(BattleSpotWork::REGIONTARGET_Opponent1);
  const Savedata::MyStatus*   pMyStatus_Oppo2 = m_pWork->GetTargetStatus(BattleSpotWork::REGIONTARGET_Opponent2);

  switch(viewMode)
  {
  case  VIEWMODE_FreeBattle_Matched_BattleRoyal:
    {
      app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA iconObjectData0;
      app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA iconObjectData1;
      app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA iconObjectData2;

      iconObjectData0.ConvFromMyStatusIconData( pMyStatus_Oppo0->GetIcon() );
      iconObjectData1.ConvFromMyStatusIconData( pMyStatus_Oppo1->GetIcon() );
      iconObjectData2.ConvFromMyStatusIconData( pMyStatus_Oppo2->GetIcon() );

      iconObjectData0.pReplacePane             = helper.Clear().GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERPHOTO_02);
      iconObjectData0.pSyncVisiblePane         = NULL;
      iconObjectData0.pInLoadVisibleChangePane = helper.Clear().GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_WINDOW_PLAYER_FRAME_02);
      iconObjectData0.pTimerIconRootPane       = NULL;

      iconObjectData1.pReplacePane             = helper.Clear().GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERPHOTO_03);
      iconObjectData1.pSyncVisiblePane         = NULL;
      iconObjectData1.pInLoadVisibleChangePane = helper.Clear().GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_WINDOW_PLAYER_FRAME_03);
      iconObjectData1.pTimerIconRootPane       = NULL;

      iconObjectData2.pReplacePane             = helper.Clear().GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERPHOTO_04);
      iconObjectData2.pSyncVisiblePane         = NULL;
      iconObjectData2.pInLoadVisibleChangePane = helper.Clear().GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_WINDOW_PLAYER_FRAME_04);
      iconObjectData2.pTimerIconRootPane       = NULL;

      m_pWork->m_pTrainerIconUtility->SetIconObjectData(TRAINER_ICON_Opponent_0, iconObjectData0, false );
      m_pWork->m_pTrainerIconUtility->SetIconObjectData(TRAINER_ICON_Opponent_1, iconObjectData1, false );
      m_pWork->m_pTrainerIconUtility->SetIconObjectData(TRAINER_ICON_Opponent_2, iconObjectData2, false );

      m_iconIndex = TRAINER_ICON_MULTI;
    }
    break;
  case  VIEWMODE_FreeBattle_Matched_Single:
  case  VIEWMODE_FreeBattle_Matched_Double:
  case  VIEWMODE_RatingBattle_Matched_Double:
  case  VIEWMODE_RatingBattle_Matched_Special:
  case  VIEWMODE_RatingBattle_Matched_Single:
  case  VIEWMODE_RatingBattle_Matched_WCS:
  default:
    {
      app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA iconObjectData;

      iconObjectData.ConvFromMyStatusIconData( pMyStatus_Oppo0->GetIcon() );

      iconObjectData.pReplacePane             = helper.Clear().GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERPHOTO_00);
      iconObjectData.pSyncVisiblePane         = NULL;
      iconObjectData.pInLoadVisibleChangePane = helper.Clear().GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_WINDOW_PLAYER_FRAME_00);
      iconObjectData.pTimerIconRootPane       = NULL;

      m_pWork->m_pTrainerIconUtility->SetIconObjectData(TRAINER_ICON_Opponent_0, iconObjectData, false );

      m_iconIndex = TRAINER_ICON_Opponent_0;
    }
  break;
  }
}


void UpperView_Information::SetupCupTitle( const wchar_t* pTitle1 , const wchar_t* pTitle2 )
{
  ResetTitle();
  m_pControlPanes[CONTROLPANE_TitleWindow]->SetVisible(true);
  m_pControlPanes[CONTROLPANE_TitleWindow_2Lines]->SetVisible(true);

  gfl2::str::StrBuf temp( 256 , m_pWork->GetDeviceHeap() );
  temp.SetStr( pTitle1 );
  GetG2DUtil()->SetRegisterWord( 0 , temp );
  GetG2DUtil()->SetTextBoxPaneStringExpand(m_pTextBoxes[TEXTBOX_Title_2Lines_0], msg_bs_sys_09);
  temp.SetStr( pTitle2 );
  GetG2DUtil()->SetRegisterWord( 0 , temp );
  GetG2DUtil()->SetTextBoxPaneStringExpand(m_pTextBoxes[TEXTBOX_Title_2Lines_1], msg_bs_sys_10);
}


bool UpperView_Information::IsIconSetupComplete(void)
{
  if( m_iconIndex == TRAINER_ICON_MULTI )
  {
    bool isAllEnd = true;
    for( u32 index = TRAINER_ICON_Opponent_0 ; index < TRAINER_ICON_MAX ; ++index )
    {
      if( m_pWork->m_pTrainerIconUtility->IsEndIconObjectData(index) == false )
      {
        isAllEnd = false;
      }
    }

    return isAllEnd;
  }
  else
  {
    return m_pWork->m_pTrainerIconUtility->IsEndIconObjectData(m_iconIndex);
  }
}


gfl2::lyt::LytPicture* UpperView_Information::GetIconPane(void)
{
  app::util::GetPaneHelper  helper(GetG2DUtil()->GetLayoutWork(0), &GetG2DUtil()->GetLayoutResourceID(0));
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( 0 );

  return helper.Clear().GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERPHOTO_00);
}


void UpperView_Information::SetupOpponentText( void )
{
  {//個別でテキストを設定する
    app::util::GetPaneHelper  helper(GetG2DUtil()->GetLayoutWork(0), GetG2DUtil()->GetLayoutResourceID());
    Savedata::MyStatus* pStatus = NULL;
    pStatus = m_pWork->GetTargetStatus(BattleSpotWork::REGIONTARGET_Opponent0);

    m_g2dUtil->SetRegisterPlayerName( 0 , pStatus );
    m_g2dUtil->SetTextBoxPaneStringExpand( helper.Clear().GetTextBoxPane( BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERNAME_B_00 ) , msg_bs_sys_34 );//相手プレイヤー名
    m_g2dUtil->SetRegisterPlayerName( 0 , pStatus );
    m_g2dUtil->SetTextBoxPaneStringExpand( helper.Clear().GetTextBoxPane( BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERNAME_B_01 ) , msg_bs_sys_34 );//相手プレイヤー名

    m_g2dUtil->SetTextBoxPaneStringExpand( helper.Clear().GetTextBoxPane( BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERRATE_B_00 ) , msg_bs_sys_35 );//レートラベル
    u32 rate = m_pWork->GetOpponentRate();
    m_g2dUtil->SetRegisterNumber( 0 , rate , 4 , print::NUM_DISP_SPACE,  print::NUM_CODE_HANKAKU );//4桁
    m_g2dUtil->SetTextBoxPaneStringExpand( helper.Clear().GetTextBoxPane( BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERRATE_B_01 ) , msg_bs_sys_36 );//レート

    gfl2::str::StrBuf tempStr(  m_pWork->GetCountry(BattleSpotWork::REGIONTARGET_Opponent0) ,m_pWork->GetDeviceHeap()->GetLowerHandle() );
    m_g2dUtil->SetRegisterWord( 0 , tempStr );
    m_g2dUtil->SetTextBoxPaneStringExpand( helper.Clear().GetTextBoxPane( BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERPARAM_B_00 ) , msg_bs_sys_37 );//国
    gfl2::str::StrBuf tempStr2(  m_pWork->GetRegion(BattleSpotWork::REGIONTARGET_Opponent0) ,m_pWork->GetDeviceHeap()->GetLowerHandle() );
    m_g2dUtil->SetRegisterWord( 0 , tempStr2 );
    m_g2dUtil->SetTextBoxPaneStringExpand( helper.Clear().GetTextBoxPane( BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERPARAM_B_01 ) , msg_bs_sys_38 );//地域

  }
}

void UpperView_Information::SetupOpponentTextRoyal( void )
{
  {//個別でテキストを設定する
    app::util::GetPaneHelper  helper(GetG2DUtil()->GetLayoutWork(0), GetG2DUtil()->GetLayoutResourceID());
    Savedata::MyStatus* pStatus0 = m_pWork->GetTargetStatus(BattleSpotWork::REGIONTARGET_Opponent0);
    Savedata::MyStatus* pStatus1 = m_pWork->GetTargetStatus(BattleSpotWork::REGIONTARGET_Opponent1);
    Savedata::MyStatus* pStatus2 = m_pWork->GetTargetStatus(BattleSpotWork::REGIONTARGET_Opponent2);

    m_g2dUtil->SetRegisterPlayerName( 0 , pStatus0 );
    m_g2dUtil->SetTextBoxPaneStringExpand( helper.Clear().GetTextBoxPane( BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERRATE_D_00 ) , msg_bs_sys_34 );//相手プレイヤー名
    m_g2dUtil->SetRegisterPlayerName( 0 , pStatus1 );
    m_g2dUtil->SetTextBoxPaneStringExpand( helper.Clear().GetTextBoxPane( BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERRATE_D_01 ) , msg_bs_sys_34 );//相手プレイヤー名
    m_g2dUtil->SetRegisterPlayerName( 0 , pStatus2 );
    m_g2dUtil->SetTextBoxPaneStringExpand( helper.Clear().GetTextBoxPane( BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERRATE_D_02 ) , msg_bs_sys_34 );//相手プレイヤー名

    m_g2dUtil->SetTextBoxPaneString( helper.Clear().GetTextBoxPane( BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERNAME_D_00 ) , msg_bs_sys_41 );//相手プレイヤー名
    m_g2dUtil->SetTextBoxPaneString( helper.Clear().GetTextBoxPane( BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERNAME_D_01 ) , msg_bs_sys_42 );//相手プレイヤー名
    m_g2dUtil->SetTextBoxPaneString( helper.Clear().GetTextBoxPane( BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERNAME_D_02 ) , msg_bs_sys_43 );//相手プレイヤー名
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
void  UpperView_Information::SetLayoutMode(const LayoutMode layoutMode)
{
  app::util::GetPaneHelper  helper(GetG2DUtil()->GetLayoutWork(0), &GetG2DUtil()->GetLayoutResourceID(0));

  OpponentStatusPaneInfo    opponentStatusInfo;

  /*  reset */
  m_pControlPanes[CONTROLPANE_TypeA]->SetVisible(false);
  m_pControlPanes[CONTROLPANE_TypeB]->SetVisible(false);
  m_pControlPanes[CONTROLPANE_TypeC]->SetVisible(false);
  m_pControlPanes[CONTROLPANE_TypeD]->SetVisible(false);

  switch(layoutMode)
  {
    case  LAYOUTMODE_TypeA:
      m_pControlPanes[CONTROLPANE_TypeA]->SetVisible(true);
      break;


    case  LAYOUTMODE_TypeB:       /**<  顔,名前      +4行          */
      m_pControlPanes[CONTROLPANE_TypeB]->SetVisible(true);
      m_pControlPanes[CONTROLPANE_TypeB_NameOnly]->SetVisible(true);
      m_pControlPanes[CONTROLPANE_TypeB_NameRate]->SetVisible(false);




      /*  oppoInfo  */
      opponentStatusInfo.pTextBoxes[TEXTBOX_Opponent_Name0] = m_pTextBoxes[TEXTBOX_TypeB_PlayerName];
      //opponentStatusInfo.pIconPicture[0]      = helper.Clear().GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERPHOTO_00);
      //opponentStatusInfo.pIconControlPane[0]  = helper.Clear().GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_WINDOW_PLAYER_FRAME_00);

      SetOpponentStatus(&opponentStatusInfo);
      break;

    case  LAYOUTMODE_TypeB_Rate:  /**<  顔,名前,Rate +4行          */
      m_pControlPanes[CONTROLPANE_TypeB]->SetVisible(true);
      m_pControlPanes[CONTROLPANE_TypeB_NameOnly]->SetVisible(false);
      m_pControlPanes[CONTROLPANE_TypeB_NameRate]->SetVisible(true);

      opponentStatusInfo.pTextBoxes[TEXTBOX_Opponent_Name0]     = m_pTextBoxes[TEXTBOX_TypeB_Rate_PlayerName];
      opponentStatusInfo.pTextBoxes[TEXTBOX_Opponent_LabelRate] = m_pTextBoxes[TEXTBOX_TypeB_Rate_RateLabel];
      opponentStatusInfo.pTextBoxes[TEXTBOX_Opponent_Rate]      = m_pTextBoxes[TEXTBOX_TypeB_Rate_RateValue];
      //opponentStatusInfo.pIconPicture[0]      = helper.Clear().GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERPHOTO_00);
      //opponentStatusInfo.pIconControlPane[0]  = helper.Clear().GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_WINDOW_PLAYER_FRAME_00);
      SetOpponentStatus(&opponentStatusInfo);
      break;

    case  LAYOUTMODE_TypeC:       /**<  顔+名前+4行+messageWin     */ /*  ?  */
      m_pControlPanes[CONTROLPANE_TypeC]->SetVisible(true);

      opponentStatusInfo.pTextBoxes[TEXTBOX_Opponent_Name0]     = m_pTextBoxes[TEXTBOX_TypeC_PlayerName];
      opponentStatusInfo.pIconPicture[0]      = helper.Clear().GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERPHOTO_01);
      opponentStatusInfo.pIconControlPane[0]  = helper.Clear().GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_WINDOW_PLAYER_FRAME_01);
      SetOpponentStatus(&opponentStatusInfo);
      break;

    case  LAYOUTMODE_TypeD:       /**<  (顔+2行)*3                 */
      m_pControlPanes[CONTROLPANE_TypeD]->SetVisible(true);


      opponentStatusInfo.pTextBoxes[TEXTBOX_Opponent_LabelName0]  = m_pTextBoxes[TEXTBOX_TypeD_Player0_NameLabel];
      opponentStatusInfo.pTextBoxes[TEXTBOX_Opponent_LabelName1]  = m_pTextBoxes[TEXTBOX_TypeD_Player1_NameLabel];
      opponentStatusInfo.pTextBoxes[TEXTBOX_Opponent_LabelName2]  = m_pTextBoxes[TEXTBOX_TypeD_Player2_NameLabel];
      opponentStatusInfo.pTextBoxes[TEXTBOX_Opponent_Name0]       = m_pTextBoxes[TEXTBOX_Opponent_Name0];
      opponentStatusInfo.pTextBoxes[TEXTBOX_Opponent_Name1]       = m_pTextBoxes[TEXTBOX_Opponent_Name1];
      opponentStatusInfo.pTextBoxes[TEXTBOX_Opponent_Name2]       = m_pTextBoxes[TEXTBOX_Opponent_Name2];
      //opponentStatusInfo.pIconPicture[0]      = helper.Clear().GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERPHOTO_02);
      //opponentStatusInfo.pIconControlPane[0]  = helper.Clear().GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_WINDOW_PLAYER_FRAME_02);
      //opponentStatusInfo.pIconPicture[1]      = helper.Clear().GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERPHOTO_03);
      //opponentStatusInfo.pIconControlPane[1]  = helper.Clear().GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_WINDOW_PLAYER_FRAME_03);
      //opponentStatusInfo.pIconPicture[2]      = helper.Clear().GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_PLAYERPHOTO_04);
      //opponentStatusInfo.pIconControlPane[2]  = helper.Clear().GetPicturePane(BattleSpotPane::PANENAME_BATTLESPOT_MSG_UPP_000_PANE_WINDOW_PLAYER_FRAME_04);

      SetOpponentStatus(&opponentStatusInfo);
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
void  UpperView_Information::ResetTitle(void)
{
  m_pControlPanes[CONTROLPANE_TitleWindow]->SetVisible(false);
  m_pControlPanes[CONTROLPANE_TitleWindow_1Line]->SetVisible(false);
  m_pControlPanes[CONTROLPANE_TitleWindow_2Lines]->SetVisible(false);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  UpperView_Information::Set1LineTitle(const u32 messageID)
{
  ResetTitle();
  m_pControlPanes[CONTROLPANE_TitleWindow]->SetVisible(true);
  m_pControlPanes[CONTROLPANE_TitleWindow_1Line]->SetVisible(true);
  GetG2DUtil()->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_Title_1Line], messageID);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  UpperView_Information::Set2LinesTitle(const u32 messageID_0, const u32 messageID_1)
{
  ResetTitle();
  m_pControlPanes[CONTROLPANE_TitleWindow]->SetVisible(true);
  m_pControlPanes[CONTROLPANE_TitleWindow_2Lines]->SetVisible(true);

  switch( messageID_0 )
  {
  case msg_bs_sys_09://たいかい１
    gfl2::str::StrBuf* pStr = m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetTextParam(Regulation::CUPNAME1);
    GetG2DUtil()->SetRegisterWord( 0 , *pStr );
    GFL_SAFE_DELETE( pStr );
  break;
  }

  GetG2DUtil()->SetTextBoxPaneStringExpand(m_pTextBoxes[TEXTBOX_Title_2Lines_0], messageID_0);

  switch( messageID_1 )
  {
  case msg_bs_sys_10://たいかい２
    gfl2::str::StrBuf* pStr = m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetTextParam(Regulation::CUPNAME2);
    GetG2DUtil()->SetRegisterWord( 0 , *pStr );
    GFL_SAFE_DELETE( pStr );
  break;
  }
  GetG2DUtil()->SetTextBoxPaneStringExpand(m_pTextBoxes[TEXTBOX_Title_2Lines_1], messageID_1);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  UpperView_Information::SetDescription(const u32 messageID)
{
  SetLayoutMode(LAYOUTMODE_TypeA);
  GetG2DUtil()->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_Description], messageID);

}


} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
