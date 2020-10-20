//==============================================================================
/**
 * @file   IDCardParts.cpp
 * @date   2015/07/16 Thu. 15:15:30
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <AppLib/include/Util/app_util_2d.h>

#include  "../../ApplicationSystem/LiveWork.h"
#include  "../../LiveResource.h"
#include  "./IDCardparts.h"


// =============================================================================
/**/
//==============================================================================
/*  local  */
namespace
{
  static const u32    POKEMON_LEVEL_DIGITS  = 3;    /**<  ポケモンレベル表示の桁数  */
}






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
PokemonPartsBase::PokemonPartsBase(LiveWork* pWork)
  : m_pG2DUtil(NULL)
  , m_LayoutWorkID(0)
  , m_pControlPane(NULL)
  , m_pTextBoxPane_PokemonLevel(NULL)
  , m_PokemonGenderIconAnimID_Male(gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR)
  , m_PokemonGenderIconAnimID_Female(gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
PokemonPartsBase::~PokemonPartsBase()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  PokemonPartsBase::ApplyPokemonParam(const pml::pokepara::CoreParam* pCoreParam)
{

  /*  level  */
  m_pG2DUtil->SetTextBoxPaneNumber(
    m_pTextBoxPane_PokemonLevel,
    msg_lt_sys_18,
    pCoreParam->GetLevel(),
    POKEMON_LEVEL_DIGITS
  );

  m_pG2DUtil->StartAnime( m_LayoutWorkID, (pCoreParam->GetSex() == pml::SEX_MALE) ? m_PokemonGenderIconAnimID_Male : m_PokemonGenderIconAnimID_Female);

}




//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
IDCardPartsBase::IDCardPartsBase(LiveWork* pWork)
  : m_pWork(pWork)
  , m_pG2DUtil(NULL)
  , m_LayoutWorkID(0)
{
  gfl2::heap::HeapBase*   pHeapBase = pWork->GetAppHeap()->GetDeviceHeap();

  for(u32 num = 0; num < GFL_NELEMS(m_TextBoxes); ++num)
  {
    m_TextBoxes[num]  = NULL;
  }

  for(u32 num = 0; num < GFL_NELEMS(m_pPokemonParts); ++num)
  {
    m_pPokemonParts[num]  = NULL;
  }
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
IDCardPartsBase::~IDCardPartsBase()
{
  for(u32 num = 0; num < GFL_NELEMS(m_pPokemonParts); ++num)
  {
    GFL_SAFE_DELETE(m_pPokemonParts[num]);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  IDCardPartsBase::ApplyLabelText(void)
{
  m_pG2DUtil->SetTextBoxPaneString(m_TextBoxes[TEXTBOX_LabelMatches], msg_lt_sys_07);
  m_pG2DUtil->SetTextBoxPaneString(m_TextBoxes[TEXTBOX_LabelSlash],   msg_lt_sys_09);
  m_pG2DUtil->SetTextBoxPaneString(m_TextBoxes[TEXTBOX_LabelWins],    msg_lt_sys_11);
  m_pG2DUtil->SetTextBoxPaneString(m_TextBoxes[TEXTBOX_LabelWins],    msg_lt_sys_13);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  IDCardPartsBase::ApplyPlayerInfo(
  const Savedata::MyStatus* pPlayerStatus,
  const gfl2::str::STRCODE* pTeamNameStrCode,
  const u32                 matchesRemain,
  const u32                 matchesMax,
  const u32                 wins,
  const u32                 loses
)
{
  m_pG2DUtil->SetRegisterPlayerName( 0, pPlayerStatus );
//  m_pG2DUtil->SetTextBoxPaneStringExpand(m_TextBoxes[TEXTBOX_PlayersRecord], msg_lt_sys_06);                 /*  プレイヤーの成績  */

  m_pG2DUtil->SetTextBoxPaneString(m_TextBoxes[TEXTBOX_TeamName],       pTeamNameStrCode);                   /*  チーム名     */
  m_pG2DUtil->SetTextBoxPaneNumber(m_TextBoxes[TEXTBOX_MatchesRemain],  msg_lt_sys_14, matchesRemain, 2);    /*  残試合       */
  m_pG2DUtil->SetTextBoxPaneNumber(m_TextBoxes[TEXTBOX_MatchesMax],     msg_lt_sys_14, matchesMax,    2);    /*  試合数最大   */
  m_pG2DUtil->SetTextBoxPaneNumber(m_TextBoxes[TEXTBOX_Wins],           msg_lt_sys_14, wins,          2);    /*  勝利数       */
  m_pG2DUtil->SetTextBoxPaneNumber(m_TextBoxes[TEXTBOX_Loses],          msg_lt_sys_14, loses,         2);    /*  敗北数       */
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  IDCardPartsBase::ApplyPokemonParam(const u32 slotIndex, const pml::pokepara::CoreParam* pCoreParam)
{
  m_pPokemonParts[slotIndex]->ApplyPokemonParam(pCoreParam);
}


} /*  namespace Live    */
} /*  namespace NetApp  */
