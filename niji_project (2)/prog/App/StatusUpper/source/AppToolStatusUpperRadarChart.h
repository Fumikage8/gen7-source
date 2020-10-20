#if !defined( __APP_TOOL_STATUS_UPPER_RADARCHART_H_INCLUDED__ )
#define __APP_TOOL_STATUS_UPPER_RADARCHART_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    AppToolStatusUpperRadarChart.h
 * @date    2015/12/21 19:34:16
 * @author  fukushima_yuya
 * @brief   フィールドステータス：レーダーチャート(上)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/RaderChart/include/app_tool_StatusRaderChart.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Layout/include/gfl2_Layout.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN(tool)
class AppCommonGrpIconData;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_BEGIN( ui )
class UIView;
GFL_NAMESPACE_END( ui )
GFL_NAMESPACE_BEGIN( util )
class AppRenderingManager;
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN(pml)
GFL_NAMESPACE_BEGIN(pokepara)
class PokemonParam;
GFL_NAMESPACE_END(pokepara)
GFL_NAMESPACE_END(pml)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

//------------------------------------------------------------------------------
// @brief   レーダーチャート(上)
//------------------------------------------------------------------------------
class FieldStatusUpperRadarChart
{
  GFL_FORBID_COPY_AND_ASSIGN( FieldStatusUpperRadarChart );

public:
  // 生成パラメータ
  struct CreateParam
  {
    app::ui::UIView*                view;
    app::util::Heap*                heap;
    app::util::AppRenderingManager*   renderMan;
    app::tool::AppCommonGrpIconData*  cmnIcon;    //!< アイコン
    gfl2::lyt::LytWk*               lytwk;
    gfl2::lyt::LytMultiResID        resID;
    u32                             lytID;
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  FieldStatusUpperRadarChart( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~FieldStatusUpperRadarChart( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //------------------------------------------------------------------
  bool InitFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   破棄
   */
  //------------------------------------------------------------------
  bool EndFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   セットアップ
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void Setup( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   レーダーチャートの色の設定
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetRadarChartColor( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   レーダーチャートの頂点の設定
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetRadarChartVertex( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   性格による能力値の高低の設定
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetSeikakuParam( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   パラメータの設定
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetParameter( const pml::pokepara::PokemonParam* pp );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ペインのセットアップ
   */
  //------------------------------------------------------------------
  void SetupPane( void );

  //------------------------------------------------------------------
  /**
   * @brief   レーダーチャートの生成
   */
  //------------------------------------------------------------------
  void CreateRadarChart( void );

  //------------------------------------------------------------------
  /**
   * @brief   レーダーチャートの中心座標の設定
   */
  //------------------------------------------------------------------
  void SetCentralCoordinate( void );

  //------------------------------------------------------------------
  /**
   * @brief   レーダーチャートの半径の設定
   */
  //------------------------------------------------------------------
  void SetRadarChartRadius( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   努力値最大演出
   *
   * @param   index   パラメータインデックス
   * @param   param   努力値
   */
  //------------------------------------------------------------------
  void SetDoryokuParamMax( u32 index, u32 param );

public:
  //------------------------------------------------------------------
  /**
   * @brief   レーダーチャートの表示
   */
  //------------------------------------------------------------------
  void VisibleRadarChart( void );

  //------------------------------------------------------------------
  /**
   * @brief   レーダーチャートの非表示
   */
  //------------------------------------------------------------------
  void InvisibleRadarChart( void );


public:
  //------------------------------------------------------------------
  /**
   * @brief   レーダーチャートの表示モードの設定
   *
   * @param   mode    表示モード
   */
  //------------------------------------------------------------------
  void SetRadarChartMode( app::tool::StatusRaderChart::StatusMode mode );

  //------------------------------------------------------------------
  /**
   * @brief   レーダーチャートの切り替え
   */
  //------------------------------------------------------------------
  void ChangeRadarChartMode( void );


public:
  static const u32 STATUS_KIND = 6;   //!< ステータスパラメータの数

private:
  CreateParam     m_CreateParam;

  u8              m_InitSeq;

  u8              m_EndSeq;

  bool            m_isVisible;

private:
  gfl2::fs::AsyncFileManager*               m_pAsyncFileManager;

  app::tool::StatusRaderChart*              m_pRadarChart;

  const pml::pokepara::PokemonParam*        m_pPokeParam;

  app::tool::StatusRaderChart::StatusMode   m_RadarChartMode;

private:
  gfl2::lyt::LytTextBox*        m_pTextNowHP;

  gfl2::lyt::LytTextBox*        m_pTextMaxHP;

  gfl2::lyt::LytTextBox*        m_pTextATK;

  gfl2::lyt::LytTextBox*        m_pTextDEF;

  gfl2::lyt::LytTextBox*        m_pTextAGI;

  gfl2::lyt::LytTextBox*        m_pTextSPDEF;

  gfl2::lyt::LytTextBox*        m_pTextSPATK;

  gfl2::lyt::LytPane*           m_pStarPane[STATUS_KIND];

  gfl2::lyt::LytPane*           m_pPaneVisibleStar;

  gfl2::lyt::LytPane*           m_pPaneVisibleParam;
};

GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

#endif // __APP_TOOL_STATUS_UPPER_RADARCHART_H_INCLUDED__