//======================================================================
/**
 * @file    AppToolStatusUpperDraw.cpp
 * @date    2015/12/16 17:56:23
 * @author  fukushima_yuya
 * @brief   フィールドステータス：メイン画面(上)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/StatusUpper/include/AppToolStatusUpperDraw.h>

#include "AppToolStatusUpperInfo.h"
#include "AppToolStatusUpperRadarChart.h"

// arc
#include <niji_conv_header/app/field_status/field_status.h>
#include <niji_conv_header/app/field_status/field_status_pane.h>
#include <niji_conv_header/app/field_status/field_status_anim_list.h>
#include <niji_conv_header/message/msg_pokemon_status.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   param   生成パラメータ
 */
//------------------------------------------------------------------------------
FieldStatusUpperDraw::FieldStatusUpperDraw( FieldStatusUpperDraw::CreateParam& param )
  : app::ui::UIView(param.heap, param.heap->GetDeviceAllocator(), param.heap->GetDeviceAllocator())
  , m_pHeap(param.heap)
  , m_pAppRenderingManager(param.renderMan)
  , m_pCommonIcon(param.cmnIcon)
  , m_pTrainerIcon(param.trainerIconPtr)
  , m_pMsgData(param.msgData)
  , m_pWordSet(param.wordSet)
  , m_pLytBuff(param.lytBuff)
  , m_pUpperInfo(NULL)
  , m_HelpVisible(param.helpFlag)
  , m_IsBattleFesCircleLayout(param.battleFesCircleLayout)
{
  for( u32 i=0; i<SEQ_KIND_NUM; ++i )
  {
    m_Seq[i] = 0;
  }

  _Create2D();
}


//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void FieldStatusUpperDraw::Update( void )
{
  app::ui::UIView::Update();

  if( m_pUpperRadarChart )
  {
    m_pUpperRadarChart->Update();
  }
  if(m_pTrainerIcon)
  {
    m_pTrainerIcon->Update();
  }

}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void FieldStatusUpperDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2D = this->GetG2DUtil();

  pG2D->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );

  if(m_pTrainerIcon)
  {
    m_pTrainerIcon->Draw(no);
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 *
 * @return  "true  = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool FieldStatusUpperDraw::InitFunc( void )
{
  enum {
    //INIT_SEQ_CREATE_2D,
    INIT_SEQ_INFO,
    INIT_SEQ_RADARCHART,
    INIT_SEQ_END,
  };

  switch( m_Seq[SEQ_KIND_INIT] )
  {
  //case INIT_SEQ_CREATE_2D:
  //  {
  //    // 2Dの生成
  //    if( _Create2D() )
  //    {
  //      m_Seq[SEQ_KIND_INIT]++;
  //    }
  //  } break;

  case INIT_SEQ_INFO:
    {
      // ポケモン情報の生成
      if( _CreatePokeInfo() )
      {
        m_Seq[SEQ_KIND_INIT]++;
      }
    } break;

  case INIT_SEQ_RADARCHART:
    {
      // レーダーチャートの生成
      if( _CreateRadarChart() )
      {
        m_Seq[SEQ_KIND_INIT]++;
      }
    } break;

  case INIT_SEQ_END:
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   2Dの生成
 *
 * @return  "true  = 完了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool FieldStatusUpperDraw::_Create2D( void )
{
  const app::util::G2DUtil::LytResData resTbl[] = {
    { m_pLytBuff, 1, static_cast<u32>(app::util::G2DUtil::ATTACH_ROOT) },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    { // MIN_UPP
      0,
      LYTRES_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000_BFLYT,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000___NUM,
      m_pLytBuff,
      true,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
  };

  // 2Dの生成
  Create2D(
    m_pHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    m_pMsgData,
    m_pWordSet );

  app::util::G2DUtil* g2d = this->GetG2DUtil();
  {
    gfl2::lyt::LytWk* lytwk        = this->GetLayoutWork(0);
    gfl2::lyt::LytMultiResID resID = g2d->GetLayoutResourceID(0);

    gfl2::lyt::LytParts*   helpParts = lytwk->GetPartsPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_PARTS_BTNHELP );
    gfl2::lyt::LytTextBox* helpText  = lytwk->GetTextBoxPane( helpParts, PANENAME_COMMON_PLT_000_PANE_TEXTBOX_00, &resID );

    // メッセージの設定
    this->SetTextboxPaneMessage( helpText, PARAM_S_TEXT_01 );
    // 表示切替
    g2d->SetPaneVisible( helpParts, m_HelpVisible );

    // BGループアニメの再生
    g2d->StartAnime( 0, LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__BG_WAVE_GREEN_UPP_00_BG_LOOP_00 );
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモン情報の生成
 *
 * @return  "true  = 完了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool FieldStatusUpperDraw::_CreatePokeInfo( void )
{
  enum {
    SEQ_INFO_INIT,
    SEQ_INFO_END,
  };
    
  switch( m_Seq[SEQ_KIND_INFO] )
  {
  case SEQ_INFO_INIT:
    {
      app::util::G2DUtil* g2d = this->GetG2DUtil();

      // 生成パラメータ
      FieldStatusUpperInfo::CreateParam param;
      {
        param.view            = this;
        param.heap            = m_pHeap;
        param.cmnIcon         = m_pCommonIcon;
        param.trainerIconPtr  = m_pTrainerIcon;
        param.lytwk           = this->GetLayoutWork(0);
        param.resID           = g2d->GetLayoutResourceID(0);
        param.wordSet         = m_pWordSet;
        param.isBFCLayout     = m_IsBattleFesCircleLayout;
      }

      // ポケモン情報
      m_pUpperInfo = GFL_NEW(m_pHeap->GetDeviceHeap()) FieldStatusUpperInfo( param );
      {
        GFL_ASSERT( m_pUpperInfo );

        // 初期化
        m_pUpperInfo->Initialize();
      }

      m_Seq[SEQ_KIND_INFO]++;
    } break;
    
  case SEQ_INFO_END:
    {
      return true;
    }
  }
  
  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   レーダーチャートの生成
 *
 * @return  "true  = 完了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool FieldStatusUpperDraw::_CreateRadarChart( void )
{
  enum {
    SEQ_CHART_INIT,
    SEQ_CHART_READY,
    SEQ_CHART_END,
  };

  switch( m_Seq[SEQ_KIND_CHART] )
  {
  case SEQ_CHART_INIT:
    {
      app::util::G2DUtil* g2d = this->GetG2DUtil();

      // 生成パラメータ
      FieldStatusUpperRadarChart::CreateParam param;
      {
        param.view      = this;
        param.heap      = m_pHeap;
        param.renderMan = m_pAppRenderingManager;
        param.cmnIcon   = m_pCommonIcon;
        param.lytwk     = this->GetLayoutWork(0);
        param.resID     = g2d->GetLayoutResourceID(0);
        param.lytID     = 0;
      }

      // ポケモン情報
      m_pUpperRadarChart = GFL_NEW(m_pHeap->GetDeviceHeap()) FieldStatusUpperRadarChart( param );
      {
        GFL_ASSERT( m_pUpperRadarChart );
      }

      m_Seq[SEQ_KIND_CHART]++;
    } break;

  case SEQ_CHART_READY:
    {
      m_pUpperRadarChart->Update();

      if( m_pUpperRadarChart->InitFunc() )
      {
        m_Seq[SEQ_KIND_CHART]++;
      }
    } break;

  case SEQ_CHART_END:
    {
      return true;
    }
  }

  return false;
}



//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 *
 * @return  "true  = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool FieldStatusUpperDraw::EndFunc( void )
{
  if( m_pUpperRadarChart )
  {
    m_pUpperRadarChart->Update();

    if( !m_pUpperRadarChart->EndFunc() ) return false;

    GFL_SAFE_DELETE( m_pUpperRadarChart );
  }

  GFL_SAFE_DELETE( m_pUpperInfo );

  Delete2D();

  return true;
}



//------------------------------------------------------------------------------
/**
 * @brief   ポケモンパラメータの設定
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void FieldStatusUpperDraw::SetPokemonParam(const pml::pokepara::PokemonParam* pp, const BattleFesTool::BFCTrainerInfo* pBFCTrainerInfo)
{
  if( m_pUpperInfo )
  {
    m_pUpperInfo->Setup(pp, pBFCTrainerInfo);
  }
  if( m_pUpperRadarChart )
  {
    m_pUpperRadarChart->Setup( pp );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   レーダーチャートの表示切替
 *
 * @param   visible   表示フラグ
 */
//------------------------------------------------------------------------------
void FieldStatusUpperDraw::SetVisibleRadarChart( bool visible )
{
  if( m_pUpperRadarChart )
  {
    if( visible )
    {
      m_pUpperRadarChart->VisibleRadarChart();
    }
    else {
      m_pUpperRadarChart->InvisibleRadarChart();
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   レーダーチャートの表示モード切替
 */
//------------------------------------------------------------------------------
void FieldStatusUpperDraw::ChangeRadarChartMode( void )
{
  if( m_pUpperRadarChart )
  {
    m_pUpperRadarChart->ChangeRadarChartMode();
  }
}

GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)
