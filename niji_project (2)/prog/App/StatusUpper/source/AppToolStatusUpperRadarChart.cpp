//======================================================================
/**
 * @file    AppToolStatusUpperRadarChart.cpp
 * @date    2015/12/21 19:39:11
 * @author  fukushima_yuya
 * @brief   フィールドステータス：レーダーチャート(上)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "AppToolStatusUpperRadarChart.h"

#include <AppLib/include/Tool/AppCommonGrpIconData.h>
#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Util/app_util_Common.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <GameSys/include/GameManager.h>

// arc
#include <niji_conv_header/app/field_status/field_status_pane.h>
#include <niji_conv_header/app/field_status/field_status_anim_list.h>

#include <niji_conv_header/message/msg_pokemon_status.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
#define DISPLAY_UPPER             (app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY)
#define RADARCHART_TYPE_AMA       (app::tool::StatusRaderChart::STATUS_MODE_AMA)
#define RADARCHART_TYPE_PRO       (app::tool::StatusRaderChart::STATUS_MODE_PRO)
#define RADARCHART_GRAPH_AMA      (app::tool::StatusRaderChart::STATUS_FIGURE_AMA)
#define RADARCHART_GRAPH_INSIDE   (app::tool::StatusRaderChart::STATUS_FIGURE_PRO_INSIDE)
#define RADARCHART_GRAPH_OUTSIDE  (app::tool::StatusRaderChart::STATUS_FIGURE_PRO_OUTSIDE)


// 基本能力値
static const pml::pokepara::PowerID c_PowerID[FieldStatusUpperRadarChart::STATUS_KIND] = {
  pml::pokepara::POWER_HP,        //!< HP
  pml::pokepara::POWER_ATK,       //!< こうげき
  pml::pokepara::POWER_DEF,       //!< ぼうぎょ
  pml::pokepara::POWER_AGI,       //!< すばやさ
  pml::pokepara::POWER_SPDEF,     //!< とくぼう
  pml::pokepara::POWER_SPATK,     //!< とくこう
};


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
FieldStatusUpperRadarChart::FieldStatusUpperRadarChart( CreateParam& param )
  : m_CreateParam(param)
  , m_InitSeq(0)
  , m_EndSeq(0)
  , m_isVisible(false)
  , m_pAsyncFileManager(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager())
  , m_pRadarChart(NULL)
  , m_pPokeParam(NULL)
  , m_RadarChartMode(RADARCHART_TYPE_AMA)
  , m_pTextNowHP(NULL)
  , m_pTextMaxHP(NULL)
  , m_pTextATK(NULL)
  , m_pTextDEF(NULL)
  , m_pTextAGI(NULL)
  , m_pTextSPDEF(NULL)
  , m_pTextSPATK(NULL)
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
FieldStatusUpperRadarChart::~FieldStatusUpperRadarChart( void )
{
  GFL_ASSERT( m_pRadarChart == NULL );
}


//------------------------------------------------------------------------------
/**
 * @brief   破棄
 */
//------------------------------------------------------------------------------
bool FieldStatusUpperRadarChart::EndFunc( void )
{
  enum {
    END_SEQ_INIT,
    END_SEQ_WAIT,
    END_SEQ_DESTROY,
    END_SEQ_END,
  };

  switch( m_EndSeq )
  {
  case END_SEQ_INIT:
    {
      // レーダーチャートを非表示
      InvisibleRadarChart();

      m_EndSeq++;
    } break;

  case END_SEQ_WAIT:
    {
      if( !m_pRadarChart->CanDestroy() ) break;

      m_EndSeq++;
    } // no break;

  case END_SEQ_DESTROY:
    {
      GFL_SAFE_DELETE( m_pRadarChart );

      m_EndSeq++;
    } // no break;

  case END_SEQ_END:
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------------------
bool FieldStatusUpperRadarChart::InitFunc( void )
{
  enum {
    INIT_SEQ_INIT,
    INIT_SEQ_READY,
    INIT_SEQ_SETUP,
    INIT_SEQ_END,
  };

  switch( m_InitSeq )
  {
  case INIT_SEQ_INIT:
    {
      // ペインのセットアップ
      SetupPane();
      // レーダーチャートの生成
      CreateRadarChart();

      m_InitSeq++;
    } // no break;

  case INIT_SEQ_READY:
    {
      if( !m_pRadarChart->IsCreated() ) break;

      m_InitSeq++;
    } // no break;

  case INIT_SEQ_SETUP:
    {
      // レーダーチャートの中心座標の設定
      SetCentralCoordinate();

      // レーダーチャートの半径の設定
      SetRadarChartRadius();

      m_InitSeq++;
    } // no break;

  case INIT_SEQ_END:
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   セットアップ
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void FieldStatusUpperRadarChart::Setup( const pml::pokepara::PokemonParam* pp )
{
  bool fastmode_flag = pp->StartFastMode();
  {
    // レーダーチャートの色の設定
    SetRadarChartColor( pp );
    // レーダーチャートの頂点の設定
    SetRadarChartVertex( pp );
    // 性格による能力値の高低の設定
    SetSeikakuParam( pp );
    // ポケモンのパラメータの設定
    SetParameter( pp );
  }
  pp->EndFastMode( fastmode_flag );

  // 初期表示は能力値のレーダーチャート
  SetRadarChartMode( m_RadarChartMode );

  // 表示するポケモンへとパラメータを変更
  m_pPokeParam = pp;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void FieldStatusUpperRadarChart::Update( void )
{
  if( m_pRadarChart )
  {
    m_pRadarChart->Update();
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   レーダーチャートの色の設定
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void FieldStatusUpperRadarChart::SetRadarChartColor( const pml::pokepara::PokemonParam* pp )
{
  const u32 doryokuMax = 510;   //!< 努力値のMAX
  u32 doryoku = 0;

  for( u32 i=0; i<STATUS_KIND; ++i )
  {
    doryoku += pp->GetEffortPower( c_PowerID[i] );
  }

  gfl2::math::Vector4 baseColor    = app::tool::StatusRaderChart::RADAR_CHART_NORMAL_PARAM_COLOR;
  gfl2::math::Vector4 insideColor  = app::tool::StatusRaderChart::RADAR_CHART_INSIDE_COLOR;
  gfl2::math::Vector4 outsideColor = app::tool::StatusRaderChart::RADAR_CHART_OUTSIDE_COLOR;
  
  // 努力値が最大の時
  if( doryoku == doryokuMax )
  {
    outsideColor = app::tool::StatusRaderChart::RADAR_CHART_OUTSIDE_COLOR_DORYOKU_MAX;
  }

  // レーダーチャートの色設定
  m_pRadarChart->SetStatusColor( insideColor, outsideColor, baseColor );
}

//------------------------------------------------------------------------------
/**
 * @brief   レーダーチャートの頂点の設定
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void FieldStatusUpperRadarChart::SetRadarChartVertex( const pml::pokepara::PokemonParam* pp )
{
  const pml::personal::ParamID c_ParamID[STATUS_KIND] = {
    pml::personal::PARAM_ID_basic_hp,
    pml::personal::PARAM_ID_basic_atk,
    pml::personal::PARAM_ID_basic_def,
    pml::personal::PARAM_ID_basic_agi,
    pml::personal::PARAM_ID_basic_spdef,
    pml::personal::PARAM_ID_basic_spatk,
  };

  // パーソナルデータの読み込み
  pml::personal::LoadPersonalData( pp->GetMonsNo(), pp->GetFormNo() );
  
  for( u32 i=0; i<STATUS_KIND; ++i )
  {
    // 種族値の取得
    u32 syuzoku = pml::personal::GetPersonalParam( c_ParamID[i] );
    // 努力値の取得
    u32 doryoku = pp->GetEffortPower( c_PowerID[i] );

    m_pRadarChart->SetStatusVal( RADARCHART_GRAPH_INSIDE, i, syuzoku );
    m_pRadarChart->SetStatusVal( RADARCHART_GRAPH_OUTSIDE, i, doryoku );

    // 努力値最大演出
    SetDoryokuParamMax( i, doryoku );
  }

  // 通常の能力値
  for( u32 i=0; i<STATUS_KIND; ++i )
  {
    u32 power = pp->GetPower( c_PowerID[i] );

    m_pRadarChart->SetStatusVal( RADARCHART_GRAPH_AMA, i, power );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   性格による能力値の高低の設定
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void FieldStatusUpperRadarChart::SetSeikakuParam( const pml::pokepara::PokemonParam* pp )
{
  const u32 c_ParamNameAnim[] = {
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000_PARAM_NAME_00,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000_PARAM_NAME_01,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000_PARAM_NAME_02,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000_PARAM_NAME_05,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000_PARAM_NAME_04,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000_PARAM_NAME_03,
  };

  app::util::G2DUtil* g2d = m_CreateParam.view->GetG2DUtil();
  u32 lytID = m_CreateParam.lytID;

  // アニメを停止
  for( u32 i=0; i<STATUS_KIND; ++i )
  {
    g2d->StopAnime( lytID, c_ParamNameAnim[i], false );
  }

  pml::pokepara::PowerID goodPower, badPower;
  app::util::OutputSpecialPointPokemSeikaku( pp->GetSeikaku(), &goodPower, &badPower );

  for( u32 i=0; i<STATUS_KIND; ++i )
  {
    u32 anmID = c_ParamNameAnim[i];

    // アニメの再生
    g2d->StartAnime( lytID, anmID, false, true );

    // 高い
    if( c_PowerID[i] == goodPower )
    {
      g2d->SetAnimeFrame( lytID, anmID, 1.0f );
    }
    // 低い
    else if( c_PowerID[i] == badPower )
    {
      g2d->SetAnimeFrame( lytID, anmID, 2.0f );
    }
    // 普通
    else {
      g2d->SetAnimeFrame( lytID, anmID, 0.0f );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   パラメータの設定
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void FieldStatusUpperRadarChart::SetParameter( const pml::pokepara::PokemonParam* pp )
{
  app::ui::UIView* view = m_CreateParam.view;

  // HP(現在値)
  view->SetTextboxPaneNumber( m_pTextNowHP, PARAM_VALUE_3, pp->GetHp(), 3 );

  // HP(最大値)
  view->SetTextboxPaneNumber( m_pTextMaxHP, PARAM_VALUE_3, pp->GetMaxHp(), 3 );

  // 攻撃
  view->SetTextboxPaneNumber( m_pTextATK, PARAM_VALUE_3, pp->GetPower(pml::pokepara::POWER_ATK), 3 );

  // 防御
  view->SetTextboxPaneNumber( m_pTextDEF, PARAM_VALUE_3, pp->GetPower(pml::pokepara::POWER_DEF), 3 );

  // 特攻
  view->SetTextboxPaneNumber( m_pTextSPATK, PARAM_VALUE_3, pp->GetPower(pml::pokepara::POWER_SPATK), 3 );

  // 特防
  view->SetTextboxPaneNumber( m_pTextSPDEF, PARAM_VALUE_3, pp->GetPower(pml::pokepara::POWER_SPDEF), 3 );

  // すばやさ
  view->SetTextboxPaneNumber( m_pTextAGI, PARAM_VALUE_3, pp->GetPower(pml::pokepara::POWER_AGI), 3 );
}


//------------------------------------------------------------------------------
/**
 * @brief   ペインのセットアップ
 */
//------------------------------------------------------------------------------
void FieldStatusUpperRadarChart::SetupPane( void )
{
  gfl2::lyt::LytWk* lytwk = m_CreateParam.lytwk;

  // 能力値名
  {
    static const u32 c_ParamNamePane[] = {
      PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_PARAMNAME_00,
      PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_PARAMNAME_01,
      PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_PARAMNAME_02,
      PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_PARAMNAME_05,
      PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_PARAMNAME_04,
      PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_PARAMNAME_03,
    };

    for( u32 i=0; i<STATUS_KIND; ++i )
    {
      gfl2::lyt::LytPicture* pic = lytwk->GetPicturePane( c_ParamNamePane[i] );
      m_CreateParam.cmnIcon->ReplacePaneTextureByPokeParamIcon( c_PowerID[i], pic );
    }
  }

  // テキストペイン
  m_pTextNowHP = lytwk->GetTextBoxPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTVALUE_07 );
  m_pTextMaxHP = lytwk->GetTextBoxPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTVALUE_09 );
  m_pTextATK   = lytwk->GetTextBoxPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTVALUE_10 );
  m_pTextDEF   = lytwk->GetTextBoxPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTVALUE_11 );
  m_pTextAGI   = lytwk->GetTextBoxPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTVALUE_14 );
  m_pTextSPDEF = lytwk->GetTextBoxPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTVALUE_13 );
  m_pTextSPATK = lytwk->GetTextBoxPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTVALUE_12 );

  // キラキラ演出部品ペイン
  static const u32 c_StarParts[] = {
    PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_STAR_00,
    PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_STAR_01,
    PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_STAR_02,
    PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_STAR_05,
    PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_STAR_04,
    PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_STAR_03,
  };

  for( u32 i=0; i<STATUS_KIND; ++i )
  {
    m_pStarPane[i] = lytwk->GetPane( c_StarParts[i] );

    m_pStarPane[i]->SetVisible( false );
  }

  // 全キラキラ演出の表示切替ペイン
  m_pPaneVisibleStar  = lytwk->GetPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_STAR_EFF );
  // 能力値の表示切替ペイン
  m_pPaneVisibleParam =  lytwk->GetPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_PARAMNAME_NUM );

  app::ui::UIView* view = m_CreateParam.view;
  //「/」
  gfl2::lyt::LytTextBox* slashText = lytwk->GetTextBoxPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTVALUE_08 );
  view->SetTextboxPaneMessage( slashText, PARAM_HP_SLASH );
}

//------------------------------------------------------------------------------
/**
 * @brief   レーダーチャートの生成
 */
//------------------------------------------------------------------------------
void FieldStatusUpperRadarChart::CreateRadarChart( void )
{
  gfl2::heap::HeapBase* devHeap = m_CreateParam.heap->GetDeviceHeap();

  m_pRadarChart = GFL_NEW(devHeap) app::tool::StatusRaderChart();
  {
    GFL_ASSERT( m_pRadarChart );

    m_pRadarChart->StartAsyncCreate( m_CreateParam.heap, m_pAsyncFileManager );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   レーダーチャートの中心座標の設定
 */
//------------------------------------------------------------------------------
void FieldStatusUpperRadarChart::SetCentralCoordinate( void )
{
  gfl2::lyt::LytWk* lytwk = m_CreateParam.lytwk;

  gfl2::lyt::LytPane * pane =
    lytwk->GetPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_PIVOT_CENTER );

  if( pane )
  {
    gfl2::math::VEC3 pos = pane->GetTranslate();
    gfl2::math::Vector2 vec( pos.x, pos.y );

    // 中心座標の設定
    m_pRadarChart->SetPos( vec );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   レーダーチャートの半径の設定
 */
//------------------------------------------------------------------------------
void FieldStatusUpperRadarChart::SetRadarChartRadius( void )
{
  gfl2::lyt::LytWk* lytwk = m_CreateParam.lytwk;

  gfl2::lyt::LytPane* centralPane = lytwk->GetPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_PIVOT_CENTER );
  gfl2::lyt::LytPane* vertexPane  = lytwk->GetPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_PIVOT_00 );

  if( centralPane && vertexPane )
  {
    gfl2::math::VEC3 center = centralPane->GetTranslate();
    gfl2::math::VEC3 vertex = vertexPane->GetTranslate();

    f32 radius = gfl2::math::FAbs( vertex.y - center.y ) + 1.0f;  // @fix NMCat[2019] ステータス画面の「種族値+努力値」のグラフが、隣接する能力によって見え方が異なってしまう  →横に-0.5ずらすというのは行わないが、半径は1.0増やす。

    // 半径の設定
    m_pRadarChart->SetStatusRadius( radius );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   努力値最大演出
 *
 * @param   index   パラメータインデックス
 * @param   param   努力値
 */
//------------------------------------------------------------------------------
void FieldStatusUpperRadarChart::SetDoryokuParamMax( u32 index, u32 param )
{
  static const u32 c_StarAnim[] = {
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__STAR_00_STAR_00,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__STAR_01_STAR_00,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__STAR_02_STAR_00,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__STAR_05_STAR_00,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__STAR_04_STAR_00,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__STAR_03_STAR_00,
  };

  const u32 doryokuParamMax = 252;    //!< 努力値の項目最大値

  // 努力値が最大ではない
  if( param >= doryokuParamMax )
  {
    app::util::G2DUtil* g2d = m_CreateParam.view->GetG2DUtil();
    {
      u32 lytID = m_CreateParam.lytID;

      // アニメの再生
      g2d->StartAnime( lytID, c_StarAnim[index] );

      // 表示
      g2d->SetPaneVisible( m_pStarPane[index], true );
    }
  }
  else {
    app::util::G2DUtil* g2d = m_CreateParam.view->GetG2DUtil();
    {
      u32 lytID = m_CreateParam.lytID;

      // アニメの再生
      g2d->StopAnime( lytID, c_StarAnim[index] );

      // 表示
      g2d->SetPaneVisible( m_pStarPane[index], false );
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   レーダーチャートの表示
 */
//------------------------------------------------------------------------------
void FieldStatusUpperRadarChart::VisibleRadarChart( void )
{
  if( !m_isVisible )
  {
    app::util::AppRenderingManager* renderMan = m_CreateParam.renderMan;

    // レンダリングマネージャへ登録
    renderMan->SetRaderChart( DISPLAY_UPPER, m_pRadarChart );

    m_isVisible = true;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   レーダーチャートの非表示
 */
//------------------------------------------------------------------------------
void FieldStatusUpperRadarChart::InvisibleRadarChart( void )
{
  if( m_isVisible )
  {
    app::util::AppRenderingManager* renderMan = m_CreateParam.renderMan;

    // レンダリングマネージャへの登録を解除
    renderMan->UnsetRaderChart( DISPLAY_UPPER );

    m_isVisible = false;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   レーダーチャートの表示モードの設定
 *
 * @param   mode    表示モード
 */
//------------------------------------------------------------------------------
void FieldStatusUpperRadarChart::SetRadarChartMode( app::tool::StatusRaderChart::StatusMode mode )
{
  if( m_pRadarChart )
  {
    // 表示するレーダーチャートの切替
    m_pRadarChart->SetStatusMode( mode );

    // 種族値・努力値の表示の時、キラキラ演出を表示
    m_pPaneVisibleStar->SetVisible( (mode == RADARCHART_TYPE_PRO) );

    // 能力値の表示切替
    m_pPaneVisibleParam->SetVisible( (mode == RADARCHART_TYPE_AMA) );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   レーダーチャートの切り替え
 */
//------------------------------------------------------------------------------
void FieldStatusUpperRadarChart::ChangeRadarChartMode( void )
{
  if( m_RadarChartMode == RADARCHART_TYPE_AMA )
  {
    m_RadarChartMode = RADARCHART_TYPE_PRO;
  }
  else {
    m_RadarChartMode = RADARCHART_TYPE_AMA;
  }

  // レーダーチャートの表示モードを設定
  SetRadarChartMode( m_RadarChartMode );
}

GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)
