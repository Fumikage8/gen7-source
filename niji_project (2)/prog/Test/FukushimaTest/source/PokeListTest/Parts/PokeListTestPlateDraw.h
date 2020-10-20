#if PM_DEBUG

#ifndef __TEST_FUKUSHIMA_TEST_PARTS_POKELIST_PLATE_DRAW_H__
#define __TEST_FUKUSHIMA_TEST_PARTS_POKELIST_PLATE_DRAW_H__
#pragma once
//==============================================================================
/**
 * @file    PokeListTestPlateDraw.h.h
 * @author  fukushima_yuya
 * @date    2015.06.25
 * @brief   ポケモンリストのプレート
 */
//==============================================================================

// niji
#include <App/PokeList/source/PokeListConst.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class DrawerBase;
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
// @brief   プレートのステータス表示クラス
//------------------------------------------------------------------------------
class TestPlateStatus
{
  GFL_FORBID_COPY_AND_ASSIGN( TestPlateStatus );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   pBase   描画共通処理
   * @param   cpos    プレート位置
   */
  //------------------------------------------------------------------
  TestPlateStatus( App::PokeList::DrawerBase* pBase, App::PokeList::CursorPos cpos );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~TestPlateStatus( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //------------------------------------------------------------------
  void Init( void );

protected:
  //------------------------------------------------------------------
  /**
   * @brief   ニックネームのセット
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetParamNickName( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   性別のセット
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetParamSex( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   レベルのセット
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetParamLevel( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   HPのセット
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetParamHp( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   アイテムのセット
   * @param   pp  ポケモンパラメータ
   * @param   egg タマゴかどうか
   */
  //------------------------------------------------------------------
  void SetItemIconByPoke( const pml::pokepara::PokemonParam* pp, bool egg );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンアイコンのセット
   */
  //------------------------------------------------------------------
  void SetPokeIcon( const pml::pokepara::PokemonParam* pp );

protected:
  //------------------------------------------------------------------
  /**
   * @brief   各モードごとの表示に切り替える
   * @param   pp  ポケモンパラメータ
   * @param   egg タマゴかどうか
   */
  //------------------------------------------------------------------
  void SetModePlate_PARAM( const pml::pokepara::PokemonParam*, bool );
  void SetModePlate_ITEM( const pml::pokepara::PokemonParam*, bool );
  void SetModePlate_JOIN( const pml::pokepara::PokemonParam*, bool );
  void SetModePlate_WAZAOSHIE( const pml::pokepara::PokemonParam*, bool );


protected:
  App::PokeList::DrawerBase*   m_pBase;

  app::tool::AppToolHPGaugePartsLayout*  m_pHPGauge;

  const App::PokeList::CursorPos   m_cCursorPos;

private:
  gfl2::lyt::LytParts*    m_pPlateParts;

  gfl2::lyt::LytParts*    m_pHPGaugeParts;

  gfl2::lyt::LytTextBox*  m_pNickNamePane;

  gfl2::lyt::LytParts*    m_pGenderParts;
  gfl2::lyt::LytPane*     m_pGenderIcon;

  gfl2::lyt::LytTextBox*  m_pLVText;
  gfl2::lyt::LytTextBox*  m_pLVValueText;

  gfl2::lyt::LytParts*    m_pSickParts;

  gfl2::lyt::LytParts*    m_pPokeIconParts;
  gfl2::lyt::LytPane*     m_pItemIconPane;
};

//------------------------------------------------------------------------------
// @brief   プレートクラス
//------------------------------------------------------------------------------
class TestPokePlate
  : public TestPlateStatus
{
  GFL_FORBID_COPY_AND_ASSIGN( TestPokePlate );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  TestPokePlate( App::PokeList::DrawerBase* pBase, App::PokeList::CursorPos cpos );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~TestPokePlate( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   セットアップ
   */
  //------------------------------------------------------------------
  void Setup( App::PokeList::PlateMode plateMode );

  //------------------------------------------------------------------
  /**
   * @brief   更新処理
   */
  //------------------------------------------------------------------
  void Update( void );
};


GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif  // __TEST_FUKUSHIMA_TEST_PARTS_POKELIST_PLATE_DRAW_H__

#endif // PM_DEBUG