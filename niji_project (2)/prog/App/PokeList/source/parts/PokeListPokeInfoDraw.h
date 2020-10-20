#ifndef NIJI_PROJECT_APP_POKELIST_POKEINFO_DRAW_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_POKEINFO_DRAW_H_INCLUDED
//==============================================================================
/**
 * @file    PokeListPokeInfoDraw.h
 * @author  fukushima_yuya
 * @date    2015.03.09
 * @brief   プレート内のポケモンのステータス部分の描画
 *          sango_project app_pokelist_Drawer
 *          sango_project app_pokelist_ListDrawer
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListConst.h"

#include <PokeTool/include/PokeTool.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class DrawerBase;
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   ポケモンのステータスの描画
// @note    ニックネーム・レベル・性別・HP・ポケモンアイコンなど
//------------------------------------------------------------------------------
class PokeInfoDraw
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeInfoDraw );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  PokeInfoDraw( DrawerBase* pBase, CursorPos cpos );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~PokeInfoDraw();


public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //------------------------------------------------------------------
  void Init( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   表示を更新
   * @param   pPokeParam    ポケモンデータ
   */
  //------------------------------------------------------------------
  void SetupPokeDisplay( const pml::pokepara::PokemonParam* pPokeParam );

  //------------------------------------------------------------------
  /**
   * @brief   プレートモードごとの表示
   * @param   plate_mode    プレートモード
   */
  //------------------------------------------------------------------
  void SetupPlateMode( PlateMode plate_mode );


private:   //! ステータス
  //------------------------------------------------------------------
  /**
   * @brief   ニックネームのセット
   * @param   pPokeParam    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetParamNickName( const pml::pokepara::PokemonParam* pPokeParam );

  //------------------------------------------------------------------
  /**
   * @brief   性別のセット
   * @param   pPokeParam    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetParamSex( const pml::pokepara::PokemonParam* pPokeParam );

  //------------------------------------------------------------------
  /**
   * @brief   レベルのセット
   * @param   pPokeParam    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetParamLevel( const pml::pokepara::PokemonParam* pPokeParam );
public:
  //------------------------------------------------------------------
  /**
   * @brief   HPのセット
   * @param   pPokeParam    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetParamHp( const pml::pokepara::PokemonParam* pPokeParam );
private:
  //------------------------------------------------------------------
  /**
   * @brief   アイテムのセット
   * @param   pPokeParam    ポケモンパラメータ
   * @param   isEgg         タマゴフラグ
   */
  //------------------------------------------------------------------
  void SetItemIconByPoke( const pml::pokepara::PokemonParam* pPokeParam, bool isEgg );

  //------------------------------------------------------------------
  /**
   * @brief   技アイコンのセット
   * @param   pPokeParam    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  //void SetParamWazaIcon( const pml::pokepara::PokemonParam* pPokeParam );


public: //! ポケモンアイコン
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンアイコンのセット
   */
  //------------------------------------------------------------------
  void SetPokeIcon( const pml::pokepara::PokemonParam * pp );


private:

  //------------------------------------------------------------------
  /**
   * @brief   各モードごとの表示に切り替える
   * @param   pPokeParam    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetModePlate_PARAM( const pml::pokepara::PokemonParam* );
  void SetModePlate_ITEM( const pml::pokepara::PokemonParam* );
  void SetModePlate_JOIN( const pml::pokepara::PokemonParam* );
  void SetModePlate_WAZAOSHIE( const pml::pokepara::PokemonParam* );


private:
  DrawerBase*   m_pBase;

  app::tool::AppToolHPGaugePartsLayout*  m_pHPGauge;

  const CursorPos   m_cCursorPos;

  bool    m_bLvVisible;

  PokeTool::SimpleParam   m_SimpleParam;

  u8    m_IconSeq;

private:
  gfl2::lyt::LytParts*    m_pPlateParts;

  gfl2::lyt::LytParts*    m_pHPGaugeParts;

  gfl2::lyt::LytTextBox*  m_pNickNamePane;

  gfl2::lyt::LytParts*    m_pGenderParts;
  gfl2::lyt::LytPane*     m_pGenderIcon;

  gfl2::lyt::LytTextBox*  m_pLVText;
  gfl2::lyt::LytTextBox*  m_pLVValueText;

  gfl2::lyt::LytParts*    m_pSickParts;
  gfl2::lyt::LytPicture*  m_pSickIcon;

  gfl2::lyt::LytParts*    m_pPokeIconParts;
  gfl2::lyt::LytPane*     m_pItemIconPane;
};

GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif  // #ifndef NIJI_PROJECT_APP_POKELIST_POKEINFO_DRAW_H_INCLUDED
