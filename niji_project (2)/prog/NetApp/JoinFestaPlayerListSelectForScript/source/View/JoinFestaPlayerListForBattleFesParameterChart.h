
#if !defined(JOINFESTAPLAYERLISTFORBATTLEFESPARAMETERCHART_H_INCLUDED)
#define JOINFESTAPLAYERLISTFORBATTLEFESPARAMETERCHART_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   JoinFestaPlayerListForBattleFesParameterChart.h
 * @date   2017/01/30 Mon. 16:36:34
 * @author muto_yu
 * @brief  BattleFesトレーナー選択用パラメータチャート
 * @note   AppToolStatusUpperRaderChartと同等の機能であるが、リソースが異なるため別途実装する必要があった
 *         後方バージョンの開発であるため、協議の結果（既存の実装に影響の発生する）汎化ではなく複製で対処することとなった
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
// niji
#include <App/RaderChart/include/app_tool_StatusRaderChart.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Layout/include/gfl2_Layout.h>

#include <NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListForBattleFesUpperView.h>


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
class CoreParam;
GFL_NAMESPACE_END(pokepara)
GFL_NAMESPACE_END(pml)


namespace NetApp {
namespace JoinFestaPlayerListSelectForScript {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------------------
// @brief   レーダーチャート(上)
//------------------------------------------------------------------------------
class ParameterChart
{
  GFL_FORBID_COPY_AND_ASSIGN( ParameterChart );
public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  ParameterChart( const NetAppLib::JoinFestaUI::JoinFestaPlayerListForBattleFesUpperView::IParameterChartDelegateObject::CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~ParameterChart( void );

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
  void Setup( const pml::pokepara::CoreParam* cp );

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
  void SetRadarChartColor( const pml::pokepara::CoreParam* cp );

  //------------------------------------------------------------------
  /**
   * @brief   レーダーチャートの頂点の設定
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetRadarChartVertex( const pml::pokepara::CoreParam* cp );

  //------------------------------------------------------------------
  /**
   * @brief   性格による能力値の高低の設定
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetSeikakuParam( const pml::pokepara::CoreParam* cp );

  //------------------------------------------------------------------
  /**
   * @brief   パラメータの設定
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetParameter( const pml::pokepara::CoreParam* cp );

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
  NetAppLib::JoinFestaUI::JoinFestaPlayerListForBattleFesUpperView::IParameterChartDelegateObject::CreateParam     m_CreateParam;

  u8              m_InitSeq;

  u8              m_EndSeq;

  bool            m_isVisible;

private:
  gfl2::fs::AsyncFileManager*               m_pAsyncFileManager;

  app::tool::StatusRaderChart*              m_pRadarChart;

  const pml::pokepara::CoreParam*        m_pCoreParam;

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

}  /*  namespace JoinFestaPlayerListSelectForScript  */
}  /*  namespace NetApp  */
#endif  /*  #if !defined(JOINFESTAPLAYERLISTFORBATTLEFESPARAMETERCHART_H_INCLUDED)  */
