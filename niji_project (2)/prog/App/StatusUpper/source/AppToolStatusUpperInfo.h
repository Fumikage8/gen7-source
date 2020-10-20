#if !defined( __APP_TOOL_STATUS_UPPER_INFO_H_INCLUDED__ )
#define __APP_TOOL_STATUS_UPPER_INFO_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    AppToolStatusUpperInfo.h
 * @date    2015/12/21 17:08:25
 * @author  fukushima_yuya
 * @brief   フィールドステータス：ポケモン情報(上)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Layout/include/gfl2_Layout.h>

#include <Field/FieldStatic/include/BattleFes/BattleFesTool.h>
#include <AppLib/include/Tool/AppToolTrainerIconRendering.h>

//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class AppCommonGrpIconData;
class AppToolGaugePartsLayout;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_BEGIN(ui)
class UIView;
GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_BEGIN(util)
class Heap;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(print)
class WordSet;
GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(pml)
GFL_NAMESPACE_BEGIN(pokepara)
class PokemonParam;
GFL_NAMESPACE_END(pokepara)
GFL_NAMESPACE_END(pml)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

//------------------------------------------------------------------------------
// @brief   ポケモン情報(上)
//------------------------------------------------------------------------------
class FieldStatusUpperInfo
{
  GFL_FORBID_COPY_AND_ASSIGN( FieldStatusUpperInfo );

public:
  // 生成パラメータ
  struct CreateParam
  {
    app::ui::UIView*                  view;         //!< ビュー
    app::util::Heap*                  heap;         //!< ヒープ
    app::tool::AppCommonGrpIconData*  cmnIcon;      //!< アイコン
    app::tool::AppToolTrainerIconRendering*
                                      trainerIconPtr;
    gfl2::lyt::LytWk*                 lytwk;        //!< レイアウトワーク
    gfl2::lyt::LytMultiResID          resID;        //!< レイアウトリソースID
    print::WordSet*                   wordSet;      //!< ワードセット
    bool                              isBFCLayout;  //!< BattleFesCircle向けのLayoutにする
    
    /* ctor */
    CreateParam(void)
      : view(NULL)
      , heap(NULL)
      , cmnIcon(NULL)
      , trainerIconPtr(NULL)
      , lytwk(NULL)
      , resID()
      , wordSet(NULL)
      , isBFCLayout(false)
    {}
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  FieldStatusUpperInfo( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~FieldStatusUpperInfo( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //------------------------------------------------------------------
  void Initialize( void );

  //------------------------------------------------------------------
  /**
   * @brief   セットアップ
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void Setup( const pml::pokepara::PokemonParam* pp, const BattleFesTool::BFCTrainerInfo* pBFCTrainerInfo = NULL );

private:
  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト
   */
  //------------------------------------------------------------------
  void InitializeText( void );

  //------------------------------------------------------------------
  /**
   * @brief   EXPゲージの生成
   */
  //------------------------------------------------------------------
  void CreateExpGauge( void );

  //------------------------------------------------------------------
  /**
   * @brief   タイプアイコンのセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetTypeIcon( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   次の経験値のセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetNextExp( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   言語アイコンのセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetLangIcon( const pml::pokepara::PokemonParam* pp );

private:
  static const u8 POKE_TYPE_ICON_MAX = 2;   //!< タイプアイコン数

private:
  CreateParam   m_CreateParam;

private:
  const pml::pokepara::PokemonParam*    m_pPokeParam;

  app::tool::AppCommonGrpIconData*      m_pCommonIcon;

  app::tool::AppToolTrainerIconRendering*
                                        m_pTrainerIcon;

  app::tool::AppToolGaugePartsLayout*   m_pExpGauge;

private:
  gfl2::lyt::LytTextBox*    m_pTextZukanNo;

  gfl2::lyt::LytTextBox*    m_pTextPokeName;

  gfl2::lyt::LytTextBox*    m_pTextPlayerName;

  gfl2::lyt::LytTextBox*    m_pTextIDNo;

  gfl2::lyt::LytTextBox*    m_pTextExpTotal;

  gfl2::lyt::LytTextBox*    m_pTextExpNext;

  gfl2::lyt::LytPicture*    m_pLangIcon;

  gfl2::lyt::LytParts*      m_pTypeIconParts[POKE_TYPE_ICON_MAX];

  gfl2::lyt::LytPicture*    m_pTypeIconPane[POKE_TYPE_ICON_MAX];

  /*  for BFC  */
  gfl2::lyt::LytPicture*    m_pTrainerIconPicture;
  gfl2::lyt::LytPane*       m_pTrainerIconVisibilityControl;
  gfl2::lyt::LytPane*       m_pTrainerIconFrame;
  gfl2::lyt::LytTextBox*    m_pTextTrainerName;
  gfl2::lyt::LytTextBox*    m_pTextBFCGrade;
};

GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

#endif // __APP_TOOL_STATUS_UPPER_INFO_H_INCLUDED__
