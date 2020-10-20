#ifndef NIJI_PROJECT_APP_WAZAOSHIE_LOWER_STATUS_H_INCLUDED
#define NIJI_PROJECT_APP_WAZAOSHIE_LOWER_STATUS_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file WazaOshieLowerStatus.h
 * @date 2015/07/01 18:57:49
 * @author fukushima_yuya
 * @brief ステータス画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "WazaOshieLowerExplain.h"

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Layout/include/gfl2_Layout.h>

#include <pml/include/pmlib.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class AppToolGaugePartsLayout;
class AppToolHPGaugePartsLayout;
class AppCommonGrpIconData;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( WazaOshie )
class DrawerBase;
GFL_NAMESPACE_END( WazaOshie )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( pml )
GFL_NAMESPACE_BEGIN( pokepara )
class PokemonParam;
GFL_NAMESPACE_END( pokepara )
GFL_NAMESPACE_END( pml )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( WazaOshie )

//------------------------------------------------------------------------------
// @brief   ステータス画面
//------------------------------------------------------------------------------
class WazaOshieStatus
{
  GFL_FORBID_COPY_AND_ASSIGN( WazaOshieStatus );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   * @param   pBase   描画共通処理
   * @param   mode    画面モード
   */
  //------------------------------------------------------------------
  WazaOshieStatus( DrawerBase* pBase, u8 mode );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~WazaOshieStatus( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //------------------------------------------------------------------
  void Init( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   セットアップ
   * @param   pCmnIcon    アイコン
   * @param   moudoku     猛毒フラグ
   * @param   pp          ポケモンパラメータ
   * @param   newWazaNo   新しく覚える技ID
   */
  //------------------------------------------------------------------
  void Setup( app::tool::AppCommonGrpIconData* pCmnIcon, bool moudoku, const pml::pokepara::PokemonParam* pp, WazaNo newWazaNo = WAZANO_NULL );

  //------------------------------------------------------------------
  /**
   * @brief   説明画面に必要なデータのセットアップ
   *
   * @param   index   習得技のインデックス[IN]
   * @param   param   セットアップパラメータ[OUT]
   */
  //------------------------------------------------------------------
  void SetExplainSetupParam( u32 index, WazaOshieExplain::SetupParam* param );

  //------------------------------------------------------------------
  /**
   * @brief   特性IDの取得
   * @return  TokuseiNo
   */
  //------------------------------------------------------------------
  TokuseiNo GetActTokuseiNo( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   道具IDの取得
   * @return  道具ID
   */
  //------------------------------------------------------------------
  u16 GetActItemNo( void ) const;

private:
  //------------------------------------------------------------------
  /**
   * @brief   ポケアイコンのセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetPokeIcon( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモン名のセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetPokeName( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   性別のセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetGender( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   タイプアイコンのセット
   *
   * @param   pp        ポケモンパラメータ
   * @param   cmnIcon   共通アイコン
   */
  //------------------------------------------------------------------
  void SetTypeIcon( const pml::pokepara::PokemonParam* pp, app::tool::AppCommonGrpIconData* cmnIcon );

  //------------------------------------------------------------------
  /**
   * @brief   状態異常アイコンのセット
   *
   * @param   pp        ポケモンパラメータ
   * @param   cmnIcon   共通アイコン
   * @param   moudoku   猛毒フラグ
   */
  //------------------------------------------------------------------
  void SetCommonIcon( const pml::pokepara::PokemonParam* pp, app::tool::AppCommonGrpIconData* cmnIcon, bool moudoku );

  //------------------------------------------------------------------
  /**
   * @brief   レベルのセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetLevel( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   パラメータのセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetParameter( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   経験値のセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetExpGauge( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   技のセット
   *
   * @param   pp        ポケモンパラメータ
   * @param   cmnIcon   共通アイコン
   */
  //------------------------------------------------------------------
  void SetWaza( const pml::pokepara::PokemonParam* pp, app::tool::AppCommonGrpIconData* cmnIcon );

  //------------------------------------------------------------------
  /**
   * @brief   特性のセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetTokusei( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   アイテムのセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetItem( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   新規技のセット
   *
   * @param   pp        ポケモンパラメータ
   * @param   cmnIcon   共通アイコン
   * @param   wazaNo    覚える技No
   */
  //------------------------------------------------------------------
  void SetNewWaza( const pml::pokepara::PokemonParam* pp, app::tool::AppCommonGrpIconData* cmnIcon, WazaNo wazaNo );


public:
  static const u32    POKE_PARAM_NUM = 5;
  static const u32    SKILL_PLATE_NUM = 5;
  static const u32    NEW_SKILL_IDX = 4;

private:
  DrawerBase* m_pBase;

  u8 m_Mode;

  WazaNo  m_WazaNo[SKILL_PLATE_NUM-1];
  u32     m_WazaNowPP[SKILL_PLATE_NUM-1];
  u32     m_WazaMaxPP[SKILL_PLATE_NUM-1];

  TokuseiNo   m_Tokusei;
  u16         m_ItemNo;

private:
  gfl2::lyt::LytTextBox*    m_pPokeName;

  gfl2::lyt::LytPane*       m_pGenderIcon;

  gfl2::lyt::LytPicture*    m_pType1Icon;
  gfl2::lyt::LytPicture*    m_pType2Icon;
  gfl2::lyt::LytParts*      m_pType2IconParts;

  gfl2::lyt::LytPicture*    m_pSickIcon;

  gfl2::lyt::LytTextBox*    m_pLevelIcon;
  gfl2::lyt::LytTextBox*    m_pLevelNum;

  app::tool::AppToolHPGaugePartsLayout*   m_pHPGauge;

  gfl2::lyt::LytTextBox*    m_pParamName[POKE_PARAM_NUM];
  gfl2::lyt::LytTextBox*    m_pParamNum[POKE_PARAM_NUM];

  app::tool::AppToolGaugePartsLayout*     m_pExpGauge;
  gfl2::lyt::LytTextBox*    m_pExpNum;
  gfl2::lyt::LytTextBox*    m_pExpSlash;
  gfl2::lyt::LytTextBox*    m_pExpMaxNum;

  gfl2::lyt::LytParts*      m_pWazaPlate[SKILL_PLATE_NUM];
  gfl2::lyt::LytTextBox*    m_pWazaName[SKILL_PLATE_NUM];
  gfl2::lyt::LytTextBox*    m_pWazaPP[SKILL_PLATE_NUM];
  gfl2::lyt::LytTextBox*    m_pWazaPPNum[SKILL_PLATE_NUM];
  gfl2::lyt::LytTextBox*    m_pWazaPPSlash[SKILL_PLATE_NUM];
  gfl2::lyt::LytTextBox*    m_pWazaMaxPPNum[SKILL_PLATE_NUM];
  gfl2::lyt::LytPicture*    m_pWazaType[SKILL_PLATE_NUM];
  gfl2::lyt::LytPane*       m_pNullPassive[SKILL_PLATE_NUM];
  gfl2::lyt::LytPane*       m_pNullCursor[SKILL_PLATE_NUM];

  gfl2::lyt::LytTextBox*    m_pTokuseiName;
  gfl2::lyt::LytTextBox*    m_pItemName;

  gfl2::lyt::LytPicture*    m_pPokeIcon;
};

GFL_NAMESPACE_END( WazaOshie )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_WAZAOSHIE_LOWER_STATUS_H_INCLUDED