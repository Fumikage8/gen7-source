//==============================================================================
/**
 * @file   JoinFestaPlayerListForBattleFesUpperView.cpp
 * @date   2017/01/26 Thu. 17:17:19
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <arc_index/message.gaix>
#include  <niji_conv_header/message/msg_jf_playerlist.h>
#include  <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist.h>
#include  <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_anim_list.h>
#include  <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_pane.h>

#include  <AppLib/include/Util/app_util_GetPaneHelper.h>
#include  <Field/FieldStatic/include/BattleFes/BattleFesTool.h>

#include  "../../include/JoinFesta/JoinFestaPacketUtil.h"
#include  "../../include/JoinFestaUI/JoinFestaPlayerData.h"
#include  "../../include/JoinFestaUI/JoinFestaPlayerListForBattleFesUpperView.h"


namespace NetAppLib {
namespace JoinFestaUI {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_JoinFestaPlayerListForBattleFesUpperView
namespace FILELOCAL {
  static const u32  IconCapacity        = 1;
  static const u32  WazaNameBufCapacity = 256;
}


// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
JoinFestaPlayerListForBattleFesUpperView::JoinFestaPlayerListForBattleFesUpperView
(
    NetAppLib::System::ApplicationWorkBase*         pWorkBase
    , NetAppLib::System::ResourceID                 layoutResourceID
    , NetAppLib::Util::NetAppTrainerIconUtility&    netAppTrainerIconUtility
)
  : NetAppLib::UI::NetAppEmptyView(pWorkBase->GetAppHeap())
  , m_pWorkBase( pWorkBase )
  , m_PokeparaUtil(pWorkBase)
  , m_WazaNameFormatString(FILELOCAL::WazaNameBufCapacity, pWorkBase->GetDeviceHeap())
  , m_bDrawFlag(true)
  , m_pParameterChartDelegate(NULL)
  , m_MessageManip()
  , m_pParamVisibilityControl(NULL)
  /*  trainericon  */
  , m_pAppCommonGrpIconData(NULL)
  , m_CurrentFriendKey()
  , m_rTrainerIconUtility(netAppTrainerIconUtility)
  , m_pTrainerIconPicture(NULL)
  , m_pTrainerIconVisibleSync(NULL)
  , m_pTrainerIconWindowFrame(NULL)
  /*  language icon  */
  , m_pLanguageIconPicture(NULL)
{
  Create2DOneLayout(
    pWorkBase,
    layoutResourceID,
    LYTRES_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000_BFLYT,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000___NUM,
    app::util::G2DUtil::SETUP_UPPER,
    pWorkBase->GetMessageUtility(),
    GARC_message_jf_playerlist_DAT
  );

  /*  pane  */
  {
    app::util::G2DUtil*      pG2DUtil  = GetG2DUtil();
    app::util::GetPaneHelper helper(pG2DUtil->GetLayoutWork(0), &(pG2DUtil->GetLayoutResourceID(0)));

    m_pParamVisibilityControl = helper.GetPane(PANENAME_JFPL_BFINVITE_UPP_000_PANE_NULL_00);

    m_pTextBoxes[TEXTBOX_TrainerName]   = helper.GetTextBoxPane(PANENAME_JFPL_BFINVITE_UPP_000_PANE_PLAYERNAME);
    m_pTextBoxes[TEXTBOX_TrainerGrade]  = helper.GetTextBoxPane(PANENAME_JFPL_BFINVITE_UPP_000_PANE_PLAYERRANK);

    m_pTrainerIconPicture     = helper.GetPicturePane(PANENAME_JFPL_BFINVITE_UPP_000_PANE_PLAYERPHOTO);
    m_pTrainerIconVisibleSync = helper.GetPane(PANENAME_JFPL_BFINVITE_UPP_000_PANE_NULL_TRAINERICON);
    m_pTrainerIconWindowFrame = helper.GetPane(PANENAME_JFPL_BFINVITE_UPP_000_PANE_WINDOW_PLAYER_FRAME);
    
    m_pLanguageIconPicture    = helper.Push(PANENAME_JFPL_BFINVITE_UPP_000_PANE_PARTS_LANG).GetPicturePane(PANENAME_COMMON_ICN_008_PANE_BG_00);
  }

  /*  manip  */
  {
    App::Tool::CommonMessageWindowManipulator::SSetupInfo info;
    
    info.pHeap        = m_pWorkBase->GetAppHeap();
    info.pTargetView  = this;
    info.layoutWorkID = 0;
    info.pHandler     = NULL;
    
    info.paneID_WindowParts = PANENAME_JFPL_BFINVITE_UPP_000_PANE_MESSAGE_L2;
    info.paneID_TextBox2    = PANENAME_COMMON_MSG_UPP_000_PANE_TEXTBOX_00;
    info.paneID_NullText1   = PANENAME_COMMON_MSG_UPP_000_PANE_NULL_TEXT1;
    info.paneID_TextBox1a   = PANENAME_COMMON_MSG_UPP_000_PANE_TEXTBOX_01;
    info.paneID_TextBox2a   = PANENAME_COMMON_MSG_UPP_000_PANE_TEXTBOX_02;
    
    m_MessageManip.Setup(info);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
JoinFestaPlayerListForBattleFesUpperView::~JoinFestaPlayerListForBattleFesUpperView()
{
  if(m_pParameterChartDelegate) m_pParameterChartDelegate->ParameterChart_OnRelease();

  m_PokeparaUtil.Cleanup();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  JoinFestaPlayerListForBattleFesUpperView::SetCommonGrpIcon(app::tool::AppCommonGrpIconData* pAppCommonGrpIconData)
{
  NetAppLib::UI::NetAppPokeparaUtility::RegisterInfo  info;
  app::util::G2DUtil*                                 pG2DUtil  = GetG2DUtil();
  app::util::GetPaneHelper                            helper(pG2DUtil->GetLayoutWork(0), &(pG2DUtil->GetLayoutResourceID(0)));
  NetAppLib::Message::MessageUtility*                 pMessage  = m_pWorkBase->GetMessageUtility();
  /*  strbuf  */
  gfl2::heap::HeapBase*                               pTempStrBufHeap = m_pWorkBase->GetDeviceHeap()->GetLowerHandle();
  gfl2::str::StrBuf                                   strBuf_NameFormat(    pMessage->GetString(GARC_message_jf_playerlist_DAT, jf_playerlist_cap_80), pTempStrBufHeap);
  gfl2::str::StrBuf                                   strBuf_LevelFormat(   pMessage->GetString(GARC_message_jf_playerlist_DAT, jf_playerlist_cap_82), pTempStrBufHeap);
  gfl2::str::StrBuf                                   strBuf_AbilityFormat( pMessage->GetString(GARC_message_jf_playerlist_DAT, jf_playerlist_cap_92), pTempStrBufHeap);
  gfl2::str::StrBuf                                   strBuf_ItemFormat(    pMessage->GetString(GARC_message_jf_playerlist_DAT, jf_playerlist_cap_93), pTempStrBufHeap);

  m_pAppCommonGrpIconData = pAppCommonGrpIconData;
  m_PokeparaUtil.Setup(FILELOCAL::IconCapacity, this, 0, pAppCommonGrpIconData);

//  info.pVisibilityControl = 
  info.pIconPicture       = helper.GetPicturePane(PANENAME_JFPL_BFINVITE_UPP_000_PANE_POKEMONICON);
  info.pTextBox_Name      = helper.GetTextBoxPane(PANENAME_JFPL_BFINVITE_UPP_000_PANE_TEXT_PNAME);
  info.pTextBox_Level     = helper.GetTextBoxPane(PANENAME_JFPL_BFINVITE_UPP_000_PANE_TEXT_LV100);
  info.pGenderIconParts   = helper.GetPartsPane(PANENAME_JFPL_BFINVITE_UPP_000_PANE_GENDERICON);
  info.pNameStringSource  = &strBuf_NameFormat;
  info.pLevelStringSource = &strBuf_LevelFormat;
  /*  性別  */
  info.genderIconAnim_Male    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__GENDERICON_ICON_MALE;
  info.genderIconAnim_Female  = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__GENDERICON_ICON_FEMALE;
  /*  タイプ  */
  info.pPokeparaTypePicturePane0  = helper.Clear().Push(PANENAME_JFPL_BFINVITE_UPP_000_PANE_TYPEICON_00).GetPicturePane(PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE);
  info.pPokeparaTypePicturePane1  = helper.Clear().Push(PANENAME_JFPL_BFINVITE_UPP_000_PANE_TYPEICON_01).GetPicturePane(PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE);
  /*  特性/アイテム  */
  info.pTextBox_Ability           = helper.Clear().GetTextBoxPane(PANENAME_JFPL_BFINVITE_UPP_000_PANE_TEXT_TOKUSEINAME);
  info.pAbilityStringSource       = &strBuf_AbilityFormat;
  info.pVisibilityControl_Ability = info.pTextBox_Ability;
  info.pTextBox_Item              = helper.Clear().GetTextBoxPane(PANENAME_JFPL_BFINVITE_UPP_000_PANE_TEXT_ITEMNAME);
  info.pItemStringSource          = &strBuf_ItemFormat;
  info.pVisibilityControl_Item    = info.pTextBox_Item;

  /*  技  */
  m_WazaNameFormatString.SetStr(pMessage->GetString(GARC_message_jf_playerlist_DAT, jf_playerlist_cap_83));
  info.wazaUtilLayoutID           = 0;
  helper.Clear();
  info.m_WazaUtilInfos[0].pControlPane  = helper.Push(PANENAME_JFPL_BFINVITE_UPP_000_PANE_WAZAPLATE_00).Peek();
  info.m_WazaUtilInfos[0].pTextBoxPane  = helper.GetTextBoxPane(PANENAME_FESTA_PLATE_UPP_000_PANE_TEXTBOX_00);
  info.m_WazaUtilInfos[0].pFormatString = &m_WazaNameFormatString;
  info.m_WazaUtilInfos[0].typeAnimID[POKETYPE_NORMAL]   = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_00_COLOR_00;
  info.m_WazaUtilInfos[0].typeAnimID[POKETYPE_KAKUTOU]  = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_00_COLOR_01;
  info.m_WazaUtilInfos[0].typeAnimID[POKETYPE_HIKOU]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_00_COLOR_02;
  info.m_WazaUtilInfos[0].typeAnimID[POKETYPE_DOKU]     = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_00_COLOR_03;
  info.m_WazaUtilInfos[0].typeAnimID[POKETYPE_JIMEN]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_00_COLOR_04;
  info.m_WazaUtilInfos[0].typeAnimID[POKETYPE_IWA]      = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_00_COLOR_05;
  info.m_WazaUtilInfos[0].typeAnimID[POKETYPE_MUSHI]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_00_COLOR_06;
  info.m_WazaUtilInfos[0].typeAnimID[POKETYPE_GHOST]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_00_COLOR_07;
  info.m_WazaUtilInfos[0].typeAnimID[POKETYPE_HAGANE]   = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_00_COLOR_08;
  info.m_WazaUtilInfos[0].typeAnimID[POKETYPE_HONOO]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_00_COLOR_09;
  info.m_WazaUtilInfos[0].typeAnimID[POKETYPE_MIZU]     = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_00_COLOR_10;
  info.m_WazaUtilInfos[0].typeAnimID[POKETYPE_KUSA]     = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_00_COLOR_11;
  info.m_WazaUtilInfos[0].typeAnimID[POKETYPE_DENKI]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_00_COLOR_12;
  info.m_WazaUtilInfos[0].typeAnimID[POKETYPE_ESPER]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_00_COLOR_13;
  info.m_WazaUtilInfos[0].typeAnimID[POKETYPE_KOORI]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_00_COLOR_14;
  info.m_WazaUtilInfos[0].typeAnimID[POKETYPE_DRAGON]   = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_00_COLOR_15;
  info.m_WazaUtilInfos[0].typeAnimID[POKETYPE_AKU]      = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_00_COLOR_16;
  info.m_WazaUtilInfos[0].typeAnimID[POKETYPE_FAIRY]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_00_COLOR_17;
  info.m_WazaUtilInfos[0].pPicturePane  = helper.Push(PANENAME_FESTA_PLATE_UPP_000_PANE_PARTS_TYPE).GetPicturePane(PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE);

  helper.Clear();
  info.m_WazaUtilInfos[1].pControlPane  = helper.Push(PANENAME_JFPL_BFINVITE_UPP_000_PANE_WAZAPLATE_01).Peek();
  info.m_WazaUtilInfos[1].pTextBoxPane  = helper.GetTextBoxPane(PANENAME_FESTA_PLATE_UPP_000_PANE_TEXTBOX_00);
  info.m_WazaUtilInfos[1].pFormatString = &m_WazaNameFormatString;
  info.m_WazaUtilInfos[1].typeAnimID[POKETYPE_NORMAL]   = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_01_COLOR_00;
  info.m_WazaUtilInfos[1].typeAnimID[POKETYPE_KAKUTOU]  = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_01_COLOR_01;
  info.m_WazaUtilInfos[1].typeAnimID[POKETYPE_HIKOU]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_01_COLOR_02;
  info.m_WazaUtilInfos[1].typeAnimID[POKETYPE_DOKU]     = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_01_COLOR_03;
  info.m_WazaUtilInfos[1].typeAnimID[POKETYPE_JIMEN]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_01_COLOR_04;
  info.m_WazaUtilInfos[1].typeAnimID[POKETYPE_IWA]      = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_01_COLOR_05;
  info.m_WazaUtilInfos[1].typeAnimID[POKETYPE_MUSHI]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_01_COLOR_06;
  info.m_WazaUtilInfos[1].typeAnimID[POKETYPE_GHOST]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_01_COLOR_07;
  info.m_WazaUtilInfos[1].typeAnimID[POKETYPE_HAGANE]   = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_01_COLOR_08;
  info.m_WazaUtilInfos[1].typeAnimID[POKETYPE_HONOO]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_01_COLOR_09;
  info.m_WazaUtilInfos[1].typeAnimID[POKETYPE_MIZU]     = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_01_COLOR_10;
  info.m_WazaUtilInfos[1].typeAnimID[POKETYPE_KUSA]     = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_01_COLOR_11;
  info.m_WazaUtilInfos[1].typeAnimID[POKETYPE_DENKI]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_01_COLOR_12;
  info.m_WazaUtilInfos[1].typeAnimID[POKETYPE_ESPER]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_01_COLOR_13;
  info.m_WazaUtilInfos[1].typeAnimID[POKETYPE_KOORI]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_01_COLOR_14;
  info.m_WazaUtilInfos[1].typeAnimID[POKETYPE_DRAGON]   = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_01_COLOR_15;
  info.m_WazaUtilInfos[1].typeAnimID[POKETYPE_AKU]      = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_01_COLOR_16;
  info.m_WazaUtilInfos[1].typeAnimID[POKETYPE_FAIRY]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_01_COLOR_17;
  info.m_WazaUtilInfos[1].pPicturePane  = helper.Push(PANENAME_FESTA_PLATE_UPP_000_PANE_PARTS_TYPE).GetPicturePane(PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE);

  helper.Clear();
  info.m_WazaUtilInfos[2].pControlPane  = helper.Push(PANENAME_JFPL_BFINVITE_UPP_000_PANE_WAZAPLATE_02).Peek();
  info.m_WazaUtilInfos[2].pTextBoxPane  = helper.GetTextBoxPane(PANENAME_FESTA_PLATE_UPP_000_PANE_TEXTBOX_00);
  info.m_WazaUtilInfos[2].pFormatString = &m_WazaNameFormatString;
  info.m_WazaUtilInfos[2].typeAnimID[POKETYPE_NORMAL]   = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_02_COLOR_00;
  info.m_WazaUtilInfos[2].typeAnimID[POKETYPE_KAKUTOU]  = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_02_COLOR_01;
  info.m_WazaUtilInfos[2].typeAnimID[POKETYPE_HIKOU]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_02_COLOR_02;
  info.m_WazaUtilInfos[2].typeAnimID[POKETYPE_DOKU]     = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_02_COLOR_03;
  info.m_WazaUtilInfos[2].typeAnimID[POKETYPE_JIMEN]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_02_COLOR_04;
  info.m_WazaUtilInfos[2].typeAnimID[POKETYPE_IWA]      = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_02_COLOR_05;
  info.m_WazaUtilInfos[2].typeAnimID[POKETYPE_MUSHI]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_02_COLOR_06;
  info.m_WazaUtilInfos[2].typeAnimID[POKETYPE_GHOST]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_02_COLOR_07;
  info.m_WazaUtilInfos[2].typeAnimID[POKETYPE_HAGANE]   = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_02_COLOR_08;
  info.m_WazaUtilInfos[2].typeAnimID[POKETYPE_HONOO]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_02_COLOR_09;
  info.m_WazaUtilInfos[2].typeAnimID[POKETYPE_MIZU]     = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_02_COLOR_10;
  info.m_WazaUtilInfos[2].typeAnimID[POKETYPE_KUSA]     = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_02_COLOR_11;
  info.m_WazaUtilInfos[2].typeAnimID[POKETYPE_DENKI]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_02_COLOR_12;
  info.m_WazaUtilInfos[2].typeAnimID[POKETYPE_ESPER]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_02_COLOR_13;
  info.m_WazaUtilInfos[2].typeAnimID[POKETYPE_KOORI]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_02_COLOR_14;
  info.m_WazaUtilInfos[2].typeAnimID[POKETYPE_DRAGON]   = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_02_COLOR_15;
  info.m_WazaUtilInfos[2].typeAnimID[POKETYPE_AKU]      = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_02_COLOR_16;
  info.m_WazaUtilInfos[2].typeAnimID[POKETYPE_FAIRY]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_02_COLOR_17;
  info.m_WazaUtilInfos[2].pPicturePane  = helper.Push(PANENAME_FESTA_PLATE_UPP_000_PANE_PARTS_TYPE).GetPicturePane(PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE);

  helper.Clear();
  info.m_WazaUtilInfos[3].pControlPane  = helper.Push(PANENAME_JFPL_BFINVITE_UPP_000_PANE_WAZAPLATE_03).Peek();
  info.m_WazaUtilInfos[3].pTextBoxPane  = helper.GetTextBoxPane(PANENAME_FESTA_PLATE_UPP_000_PANE_TEXTBOX_00);
  info.m_WazaUtilInfos[3].pFormatString = &m_WazaNameFormatString;
  info.m_WazaUtilInfos[3].typeAnimID[POKETYPE_NORMAL]   = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_03_COLOR_00;
  info.m_WazaUtilInfos[3].typeAnimID[POKETYPE_KAKUTOU]  = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_03_COLOR_01;
  info.m_WazaUtilInfos[3].typeAnimID[POKETYPE_HIKOU]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_03_COLOR_02;
  info.m_WazaUtilInfos[3].typeAnimID[POKETYPE_DOKU]     = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_03_COLOR_03;
  info.m_WazaUtilInfos[3].typeAnimID[POKETYPE_JIMEN]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_03_COLOR_04;
  info.m_WazaUtilInfos[3].typeAnimID[POKETYPE_IWA]      = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_03_COLOR_05;
  info.m_WazaUtilInfos[3].typeAnimID[POKETYPE_MUSHI]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_03_COLOR_06;
  info.m_WazaUtilInfos[3].typeAnimID[POKETYPE_GHOST]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_03_COLOR_07;
  info.m_WazaUtilInfos[3].typeAnimID[POKETYPE_HAGANE]   = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_03_COLOR_08;
  info.m_WazaUtilInfos[3].typeAnimID[POKETYPE_HONOO]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_03_COLOR_09;
  info.m_WazaUtilInfos[3].typeAnimID[POKETYPE_MIZU]     = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_03_COLOR_10;
  info.m_WazaUtilInfos[3].typeAnimID[POKETYPE_KUSA]     = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_03_COLOR_11;
  info.m_WazaUtilInfos[3].typeAnimID[POKETYPE_DENKI]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_03_COLOR_12;
  info.m_WazaUtilInfos[3].typeAnimID[POKETYPE_ESPER]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_03_COLOR_13;
  info.m_WazaUtilInfos[3].typeAnimID[POKETYPE_KOORI]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_03_COLOR_14;
  info.m_WazaUtilInfos[3].typeAnimID[POKETYPE_DRAGON]   = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_03_COLOR_15;
  info.m_WazaUtilInfos[3].typeAnimID[POKETYPE_AKU]      = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_03_COLOR_16;
  info.m_WazaUtilInfos[3].typeAnimID[POKETYPE_FAIRY]    = LA_JOIN_FESTA_PLAYERLIST_JFPL_BFINVITE_UPP_000__WAZAPLATE_03_COLOR_17;
  info.m_WazaUtilInfos[3].pPicturePane  = helper.Push(PANENAME_FESTA_PLATE_UPP_000_PANE_PARTS_TYPE).GetPicturePane(PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE);

  m_PokeparaUtil.RegisterStatusResource(0, info);

  /*  paramchart  */
  if(m_pParameterChartDelegate)
  {
    gfl2::heap::HeapBase*                       pHeapBase = m_pWorkBase->GetDeviceHeap();
    IParameterChartDelegateObject::CreateParam  param;

    param.view      = this;
    param.heap      = m_pWorkBase->GetAppHeap();
    param.renderMan = m_pWorkBase->GetAppRenderingManager();
    param.cmnIcon   = pAppCommonGrpIconData;
    param.lytwk     = pG2DUtil->GetLayoutWork(0);
    param.resID     = pG2DUtil->GetLayoutResourceID(0);
    param.lytID     = 0;
    
    m_pParameterChartDelegate->ParameterChart_OnCreate(param);
  }

}


//------------------------------------------------------------------
/**
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void JoinFestaPlayerListForBattleFesUpperView::Update(void)
{
  if(m_pParameterChartDelegate)  m_pParameterChartDelegate->ParameterChart_OnUpdate();

  m_MessageManip.UpdateWindow();
  
  NetAppLib::UI::NetAppEmptyView::Update();   /* super   */
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void JoinFestaPlayerListForBattleFesUpperView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  const u32 layoutWorkID = 0;

  app::util::G2DUtil*     pG2DUtil          = UIView::GetG2DUtil();
  gfl2::lyt::LytWk*       pUpper = pG2DUtil->GetLayoutWork(layoutWorkID);

  if( m_bDrawFlag )
  {
    app::util::AppRenderingManager* pAppRenderingManager = m_pWorkBase->GetAppRenderingManager();
    pG2DUtil->AddDrawableLytWkForOneFrame(pAppRenderingManager, displayNo, layoutWorkID);
  }
  if( m_pTrainerIconVisibleSync )
  {
    m_pTrainerIconVisibleSync->SetVisible( m_bDrawFlag );
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  JoinFestaPlayerListForBattleFesUpperView::SetJoinFestaPlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pData )
{
  m_bDrawFlag = (pData != NULL);
  if(m_pParameterChartDelegate)  m_pParameterChartDelegate->ParameterChart_OnSetVisibility(m_bDrawFlag);

  if(!pData)  return;
  
  
  gfl2::heap::HeapBase*               pHeapBase     = m_pWorkBase->GetDeviceHeap()->GetLowerHandle();
  const pml::pokepara::CoreParam*     pCoreParam    = pData->BFC_GetPokepara();
  app::util::G2DUtil*                 pG2DUtil      = GetG2DUtil();
  NetAppLib::Message::MessageUtility* pMessage      = m_pWorkBase->GetMessageUtility();
  gfl2::str::MsgData*                 pMessageData  = pMessage->GetData(GARC_message_jf_playerlist_DAT);
  print::WordSet*                     pWordSet      = pMessage->GetWordSet();


  /*  trainer  */
  {
    const JoinFestaFriendKey joinFestaFriendKey = pData->GetJoinFestaFriendKey();

    if( !NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey( joinFestaFriendKey, m_CurrentFriendKey ) )
    {
      /*  name  */
      pG2DUtil->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_TrainerName], pData->GetName());

      /*  grade  */
      {
        /*  pokeparaが設定済みということはPacketGameStatus1が有効ということであるが、明示的に両方確認しておく  */
        NetAppLib::JoinFesta::PacketGameStatus1 val;
        const bool                              bPokeparaAvailable  = pData->BFC_GetPokeparaAvailability();
        const bool                              bPacketAvailable    = pData->GetPacketGameStatus1(&val);

        /*  グレード表示  */
        if(bPokeparaAvailable & bPacketAvailable)
        {
          m_pTextBoxes[TEXTBOX_TrainerGrade]->SetVisible(true);
          pG2DUtil->SetRegisterNumber( 0, val.battleFesRank, 2);
          pG2DUtil->SetTextBoxPaneStringExpand(m_pTextBoxes[TEXTBOX_TrainerGrade], pMessage->GetString(GARC_message_jf_playerlist_DAT, jf_playerlist_cap_79));
        }
        else
        {
          m_pTextBoxes[TEXTBOX_TrainerGrade]->SetVisible(false);
        }
      }
      
      /*  trainer icon  */
      if( m_pTrainerIconPicture )
      {
        const NetAppLib::JoinFesta::PacketPersonalInfo& packetPersonalInfo = pData->GetPacketPersonalInfo();

        app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA iconObjectData;

        iconObjectData.ConvFromMyStatusIconData( packetPersonalInfo.icon );

        iconObjectData.pReplacePane             = m_pTrainerIconPicture;
        iconObjectData.pSyncVisiblePane         = m_pTrainerIconVisibleSync;
        iconObjectData.pInLoadVisibleChangePane = m_pTrainerIconWindowFrame;
        iconObjectData.pTimerIconRootPane       = NULL;

        m_rTrainerIconUtility.SetIconObjectData( 0, iconObjectData );

        m_CurrentFriendKey = joinFestaFriendKey;
      }

      /*  language icon  */
      if(m_pLanguageIconPicture && m_pAppCommonGrpIconData)
      {
        const NetAppLib::JoinFesta::PacketPersonalInfo& packetPersonalInfo = pData->GetPacketPersonalInfo();
      
        m_pAppCommonGrpIconData->ReplacePaneTextureByLangIcon(packetPersonalInfo.languageId, m_pLanguageIconPicture);
      }
    }
  }


  if(pCoreParam)
  {
    m_pParamVisibilityControl->SetVisible(true);
    m_MessageManip.HideMessage();

    m_PokeparaUtil.SetCoreParam(0, pCoreParam);
    if(m_pParameterChartDelegate)  m_pParameterChartDelegate->ParameterChart_OnSetPokepara(pCoreParam);
  }
  else
  {
    /*  まだ遊んでいない  */
    m_pParamVisibilityControl->SetVisible(false);
    m_MessageManip.ShowMessage(jf_playerlist_win_39, true);
    if(m_pParameterChartDelegate)  m_pParameterChartDelegate->ParameterChart_OnSetVisibility(false);
  }
}

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  JoinFestaPlayerListForBattleFesUpperView::ParameterChart_InitFunc(void)
{
  bool  isEnd = false;

  if(m_pParameterChartDelegate)
  {
    isEnd = m_pParameterChartDelegate->ParameterChart_OnInitFunc();
  }
  else
  {
    isEnd = true;
  }

  
  return isEnd;
}

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  JoinFestaPlayerListForBattleFesUpperView::ParameterChart_EndFunc(void)
{
  bool  isEnd = false;

  if(m_pParameterChartDelegate)
  {
    isEnd = m_pParameterChartDelegate->ParameterChart_OnEndFunc();
  }
  else
  {
    isEnd = true;
  }

  return isEnd;
}



}  /*  namespace JoinFestaUI  */
}  /*  namespace NetAppLib  */
