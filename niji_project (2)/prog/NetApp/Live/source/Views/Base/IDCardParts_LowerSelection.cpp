//==============================================================================
/**
 * @file   IDCardParts_LowerSelection.cpp
 * @date   2015/07/16 Thu. 16:41:57
 * @author muto_yu
 * @brief  Live_SLC_LOW_000.flytに特化したIDCardPartsBase::Setupの実装
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <AppLib/include/util/app_util_2d.h>

#include  "AppLib/include/Util/app_util_GetPaneHelper.h"

#include  "../../LiveResource.h"
#include  "../../ApplicationSystem/LiveWork.h"
#include  "./IDCardParts_LowerSelection.h"


namespace NetApp  {
namespace Live    {
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
PokemonParts_LowerSelection::PokemonParts_LowerSelection(LiveWork* pWork)
  : PokemonPartsBase(pWork)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
PokemonParts_LowerSelection::~PokemonParts_LowerSelection()
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
void  PokemonParts_LowerSelection::Setup(const u32 slotIndex, app::util::G2DUtil* pG2DUtil, u32 lytID, const u32 layoutMultiResourceID)
{
  const struct PaneIDTableInfo
  {
    gfl2::lyt::LytPaneIndex   controlPaneID;
    gfl2::lyt::LytPaneIndex   pokemonLevelPaneID;
    u32                       genderIconAnimID_Male;
    u32                       genderIconAnimID_Female;
  }paneIDTable[IDCardPartsBase::POKEMON_INFO_NUM]  = {
    {PANENAME_LIVE_PLT_000_PANE_POKE_00, PANENAME_LIVE_PLT_000_PANE_POKE_00, LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_00_ICON_MALE, LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_00_ICON_FEMALE},
    {PANENAME_LIVE_PLT_000_PANE_POKE_01, PANENAME_LIVE_PLT_000_PANE_POKE_01, LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_01_ICON_MALE, LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_01_ICON_FEMALE},
    {PANENAME_LIVE_PLT_000_PANE_POKE_02, PANENAME_LIVE_PLT_000_PANE_POKE_02, LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_02_ICON_MALE, LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_02_ICON_FEMALE},
    {PANENAME_LIVE_PLT_000_PANE_POKE_03, PANENAME_LIVE_PLT_000_PANE_POKE_03, LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_03_ICON_MALE, LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_03_ICON_FEMALE},
    {PANENAME_LIVE_PLT_000_PANE_POKE_04, PANENAME_LIVE_PLT_000_PANE_POKE_04, LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_04_ICON_MALE, LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_04_ICON_FEMALE},
    {PANENAME_LIVE_PLT_000_PANE_POKE_05, PANENAME_LIVE_PLT_000_PANE_POKE_05, LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_05_ICON_MALE, LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_05_ICON_FEMALE},
  };

  GFL_ASSERT_MSG(slotIndex < GFL_NELEMS(paneIDTable), "slotIndex out of range: max:%d], in:%d\n", GFL_NELEMS(paneIDTable), slotIndex);


  app::util::GetPaneHelper        helper(pG2DUtil->GetLayoutWork(lytID), &(pG2DUtil->GetLayoutResourceID( layoutMultiResourceID )));
  const struct PaneIDTableInfo&   rTableItem  = paneIDTable[slotIndex];

  m_pG2DUtil      = pG2DUtil;
  m_LayoutWorkID  = lytID;

  helper.Push(PANENAME_LIVE_SLC_LOW_000_PANE_IDCARD);

  m_pControlPane                    = helper.GetPane(rTableItem.controlPaneID);
  m_pTextBoxPane_PokemonLevel       = helper.GetTextBoxPane(rTableItem.pokemonLevelPaneID);
  m_PokemonGenderIconAnimID_Male    = rTableItem.genderIconAnimID_Male;
  m_PokemonGenderIconAnimID_Female  = rTableItem.genderIconAnimID_Female;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
IDCardParts_LowerSelection::IDCardParts_LowerSelection(LiveWork* pWork)
  : IDCardPartsBase(pWork)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
IDCardParts_LowerSelection::~IDCardParts_LowerSelection()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  IDCardParts_LowerSelection::Setup(app::util::G2DUtil* pG2DUtil, u32 lytID, const u32 layoutMultiResourceID)
{
  gfl2::heap::HeapBase*     pHeapBase = m_pWork->GetAppHeap()->GetDeviceHeap();
  app::util::GetPaneHelper  helper(pG2DUtil->GetLayoutWork(lytID), &(pG2DUtil->GetLayoutResourceID( layoutMultiResourceID )));


  m_pG2DUtil      = pG2DUtil;
  m_LayoutWorkID  = lytID;

  helper.Push(PANENAME_LIVE_SLC_LOW_000_PANE_IDCARD);
  {
    m_TextBoxes[TEXTBOX_LabelMatches]  = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXTBOX_01);         /*  【msg_lt_sys_07】たいせんかいすう */
    m_TextBoxes[TEXTBOX_LabelSlash]    = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXTBOX_SLASH);      /*  【msg_lt_sys_09】99/99のスラッシュ  */
    m_TextBoxes[TEXTBOX_LabelWins]     = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXTBOX_02);         /*  【msg_lt_sys_08】かち */
    m_TextBoxes[TEXTBOX_LabelLoses]    = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXTBOX_03);         /*  【msg_lt_sys_09】まけ */

//    m_TextBoxes[TEXTBOX_PlayersRecord] = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXTBOX_00);         /*  【msg_lt_sys_06】「プレイヤー名」の　せいせき */
    m_TextBoxes[TEXTBOX_TeamName]      = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXTBOX_TEAMNAME);   /*  【msg_lt_sys_15】チーム名 */
    m_TextBoxes[TEXTBOX_MatchesRemain] = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXT_99_00);         /*  【msg_lt_sys_08】99/99の分子  ※対戦可能回数残    */
    m_TextBoxes[TEXTBOX_MatchesMax]    = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXT_99_01);         /*  【msg_lt_sys_08】:99/99の分母 ※対戦可能回数最大  */
    m_TextBoxes[TEXTBOX_Wins]          = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXT_99_02);         /*  【msg_lt_sys_08】かちの99 */
    m_TextBoxes[TEXTBOX_Loses]         = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXT_99_03);         /*  【msg_lt_sys_08】まけの99 */
  }

  /*    */
  for(u32 num = 0; num < GFL_NELEMS(m_pPokemonParts); ++num)
  {
    m_pPokemonParts[num]  = GFL_NEW(pHeapBase) PokemonParts_LowerSelection(m_pWork);  /*  解体は基底のdtor  */
    m_pPokemonParts[num]->Setup(num, pG2DUtil, lytID, layoutMultiResourceID);
  }

  /*  固定文字列  */
  ApplyLabelText();
}



} /*  namespace Live    */
} /*  namespace NetApp  */

