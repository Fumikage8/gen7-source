#if !defined(IDCARDPARTS_H_INCLUDED)
#define IDCARDPARTS_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   IDCardParts.h
 * @date   2015/07/16 Thu. 13:09:58
 * @author muto_yu
 * @brief  IDカード機能
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <pml/include/pml_PokeParty.h>
#include  <Layout/include/gfl2_LytSys.h>
#include  <str/include/gfl2_Str.h>


// =============================================================================
/**/
//==============================================================================

namespace Savedata  {
  class MyStatus;
}

namespace app {
namespace util {
  class G2DUtil;
}
}





namespace NetApp  {
namespace Live    {
  class LiveWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  
  *  @brief  ポケモン表示パーツの取り回し
  *  @date   2015/07/14 Tue. 18:35:52
  */
//==============================================================================
class PokemonPartsBase
{
  GFL_FORBID_COPY_AND_ASSIGN( PokemonPartsBase );

public:
  PokemonPartsBase(LiveWork* pWork);
  virtual ~PokemonPartsBase();

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual void  Setup(const u32 slotIndex, app::util::G2DUtil* pG2DUtil, u32 lytID = 0, const u32 layoutMultiResourceID = 0) = 0;


  void  ApplyPokemonParam(const pml::pokepara::CoreParam* pCoreParam);

protected:
  app::util::G2DUtil*       m_pG2DUtil;
  u32                       m_LayoutWorkID;
  gfl2::lyt::LytPane*       m_pControlPane;
  gfl2::lyt::LytTextBox*    m_pTextBoxPane_PokemonLevel;
  u32                       m_PokemonGenderIconAnimID_Male;
  u32                       m_PokemonGenderIconAnimID_Female;
};


//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/07/14 Tue. 18:31:51
  */
//==============================================================================
class IDCardPartsBase
{
  GFL_FORBID_COPY_AND_ASSIGN( IDCardPartsBase );

public:
  static const u32        POKEMON_INFO_NUM  = 6;

  IDCardPartsBase(LiveWork* pWork);
  virtual ~IDCardPartsBase();


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    *  @note     リソースに依存するPane取得処理をここで吸収
    *  @note     PokemonPartsBaseの実際の型が派生先で決定されるので、本関数内でNewする
    */
  //------------------------------------------------------------------
  virtual void  Setup(app::util::G2DUtil* pG2DUtil, u32 lytID = 0, const u32 layoutMultiResourceID = 0) = 0;


  void  ApplyLabelText(void);
  void  ApplyPlayerInfo(
          const Savedata::MyStatus* pPlayerStatus,
          const gfl2::str::STRCODE* pTeamNameStrCode,
          const u32                 matchesRemain,
          const u32                 matchesMax,
          const u32                 wins,
          const u32                 loses
        );
  void  ApplyPokemonParam(const u32 slotIndex, const pml::pokepara::CoreParam* pCoreParam);


protected:
  /*
    members
  */
  LiveWork*               m_pWork;
  app::util::G2DUtil*     m_pG2DUtil;
  u32                     m_LayoutWorkID;

  enum  TextBoxID
  {
    /*  固定ラベル  */
    TEXTBOX_LabelMatches,     /*  【msg_lt_sys_07】たいせんかいすう */
    TEXTBOX_LabelSlash,       /*  【msg_lt_sys_09】99/99のスラッシュ  */
    TEXTBOX_LabelWins,        /*  【msg_lt_sys_08】かち */
    TEXTBOX_LabelLoses,       /*  【msg_lt_sys_09】まけ */

    /*  変数  */
//    TEXTBOX_PlayersRecord,    /*  【msg_lt_sys_06】「プレイヤー名」の　せいせき */
    TEXTBOX_TeamName,         /*  【msg_lt_sys_15】チーム名 */
    TEXTBOX_MatchesRemain,    /*  【msg_lt_sys_08】99/99の分子  ※対戦可能回数残    */
    TEXTBOX_MatchesMax,       /*  【msg_lt_sys_08】:99/99の分母 ※対戦可能回数最大  */
    TEXTBOX_Wins,             /*  【msg_lt_sys_08】かちの99 */
    TEXTBOX_Loses,            /*  【msg_lt_sys_08】まけの99 */

    TEXTBOX_MAX
  };
  gfl2::lyt::LytTextBox*    m_TextBoxes[TEXTBOX_MAX];
  
  PokemonPartsBase*       m_pPokemonParts[POKEMON_INFO_NUM];

};



} /*  namespace Live    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(IDCARDPARTS_H_INCLUDED)  */
