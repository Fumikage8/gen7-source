//======================================================================
/**
 * @file    StatusWaza.cpp
 * @date    2015/12/04 17:56:15
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：技(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusWaza.h"

#include <App/Status/source/StatusDrawerBase.h>

#include <AppLib/include/Tool/AppCommonGrpIconData.h>
#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Util/app_util_Common.h>

// pml
#include <pml/include/pmlib.h>

// arc
#include <niji_conv_header/app/field_status/field_status.h>
#include <niji_conv_header/app/field_status/field_status_pane.h>
#include <niji_conv_header/app/field_status/field_status_anim_list.h>

#include <niji_conv_header/message/msg_pokemon_status.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  //! @brief  技プレート
  static const u32 c_PlateParentIndex[] = {
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSSKILLPLT_00,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSSKILLPLT_01,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSSKILLPLT_02,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSSKILLPLT_03,
  };
  //! @brief  技の属性アニメの先頭インデックス
  const u32 c_WazaTypeAnim[] = {
    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_00_COLOR_00,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_01_COLOR_00,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_02_COLOR_00,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_03_COLOR_00,
  };
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
Waza::Waza( DrawerBase* base, app::tool::AppCommonGrpIconData* cmnIcon )
  : m_pBase(base)
  , m_pCommonIcon(cmnIcon)
  , m_pPokeParam(NULL)
  , m_WazaCount(0)
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------------------
void Waza::Initialize( void )
{
  // ペインのセット
  {
    for( u32 i=0; i<PLATE_NUM; ++i )
    {
      gfl2::lyt::LytParts* parts = m_pBase->GetParts( c_PlateParentIndex[i] );

      m_PlatePaneData[i].parent       = parts;
      m_PlatePaneData[i].wazaName     = m_pBase->GetTextBox( PANENAME_STATUS_PLT_LOW_000_PANE_TEXTBOX_00, parts );
      m_PlatePaneData[i].wazaPP       = m_pBase->GetTextBox( PANENAME_STATUS_PLT_LOW_000_PANE_TEXTBOX_01, parts );
      m_PlatePaneData[i].wazaPPNow    = m_pBase->GetTextBox( PANENAME_STATUS_PLT_LOW_000_PANE_TEXTBOX_02, parts );
      m_PlatePaneData[i].wazaPPSlash  = m_pBase->GetTextBox( PANENAME_STATUS_PLT_LOW_000_PANE_TEXTBOX_04, parts );
      m_PlatePaneData[i].wazaPPMax    = m_pBase->GetTextBox( PANENAME_STATUS_PLT_LOW_000_PANE_TEXTBOX_03, parts );
      m_PlatePaneData[i].iconParts    = m_pBase->GetParts( PANENAME_STATUS_PLT_LOW_000_PANE_PARTS_TYPE, parts );
      m_PlatePaneData[i].iconPane     = m_pBase->GetPicture( PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, m_PlatePaneData[i].iconParts );
    }
  }

  // 初期テキスト
  InitializeText();
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト
 */
//------------------------------------------------------------------------------
void Waza::InitializeText( void )
{
  app::ui::UIView* view = m_pBase->GetUIView();

  for( u32 i=0; i<PLATE_NUM; ++i )
  {
    //「PP」
    view->SetTextboxPaneMessage( m_PlatePaneData[i].wazaPP, PARAM_PP );

    //「/」
    view->SetTextboxPaneMessage( m_PlatePaneData[i].wazaPPSlash, PARAM_SLASH );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   セットアップ
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void Waza::Setup( const pml::pokepara::PokemonParam* pp )
{
  FUKUSHIMA_PRINT( "%s\n", __FUNCTION__ );
  FUKUSHIMA_PRINT( "{\n" );

  //if( m_pPokeParam != pp )
  {
    bool fastmode_flag = pp->StartFastMode();
    {
      // 所持技数の取得
      m_WazaCount = pp->GetWazaCount();

      for( u32 i=0; i<PLATE_NUM; ++i )
      {
        if( i < m_WazaCount )
        {
          u32 wazaNo = pp->GetWazaNo( i );

          // 技名の設定
          SetWazaName( i, wazaNo );
          // 技PPの設定
          SetWazaPP( i, pp );
          // 技タイプの設定
          SetWazaType( i, wazaNo );

          FUKUSHIMA_PRINT( " Waza_%d[%d]\n", i, wazaNo );
        }

        // 部品ペインの表示切替
        m_pBase->SetVisiblePane( m_PlatePaneData[i].parent, (i < m_WazaCount) );
      }
    }
    pp->EndFastMode( fastmode_flag );

    m_pPokeParam = pp;
  }
  FUKUSHIMA_PRINT( "}\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief   技名の設定
 *
 * @param   index     プレートインデックス
 * @param   wazaNo    技No
 */
//------------------------------------------------------------------------------
void Waza::SetWazaName( u32 index, u32 wazaNo )
{
  app::ui::UIView* view    = m_pBase->GetUIView();
  print::WordSet*  wordSet = m_pBase->GetWordSet();

  // 技名の登録
  wordSet->RegisterWazaName( 0, wazaNo );

  // 技名のセット
  view->SetTextboxPaneMessage( m_PlatePaneData[index].wazaName, PARAM_WAZA_NAME );
}

//------------------------------------------------------------------------------
/**
 * @brief   技PPの設定
 *
 * @param   index プレートインデックス
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void Waza::SetWazaPP( u32 index, const pml::pokepara::PokemonParam* pp )
{
  app::util::G2DUtil* g2d = m_pBase->GetG2D();

  u32 nowPP = pp->GetWazaPP( index );
  u32 maxPP = pp->GetWazaMaxPP( index );

  // 現在のPPをセット
  g2d->SetTextBoxPaneNumber( m_PlatePaneData[index].wazaPPNow, PARAM_PP_VALUE, nowPP, 2, 0, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  // 最大のPPをセット
  g2d->SetTextBoxPaneNumber( m_PlatePaneData[index].wazaPPMax, PARAM_PP_VALUE, maxPP, 2, 0, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );

  // PPの消費に合わせて色を変更
  gfl2::lyt::ColorU8 color = app::util::GetWazaTextColor( nowPP, maxPP );
  {
    g2d->SetTextBoxPaneColor( m_PlatePaneData[index].wazaPP, color, color );        //!< 「PP」
    g2d->SetTextBoxPaneColor( m_PlatePaneData[index].wazaPPNow, color, color );     //!< 現在のPP
    g2d->SetTextBoxPaneColor( m_PlatePaneData[index].wazaPPSlash, color, color );   //!< 「/」
    g2d->SetTextBoxPaneColor( m_PlatePaneData[index].wazaPPMax, color, color );     //!< 最大のPP
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   技タイプの設定
 *
 * @param   index     プレートインデックス
 * @param   wazaNo    技No
 */
//------------------------------------------------------------------------------
void Waza::SetWazaType( u32 index, u32 wazaNo )
{
  // 技のタイプを取得
  pml::PokeType type = pml::wazadata::GetType( static_cast<WazaNo>(wazaNo) );
  
  // タイプ変更アニメの取得
  const u32 anmID = static_cast<u32>(c_WazaTypeAnim[index] + type);
  // タイプ変更アニメの再生
  m_pBase->StartAnim( anmID );

  // タイプアイコンの変更
  PokeType_tag typeTag = static_cast<PokeType_tag>(type);
  m_pCommonIcon->ReplacePaneTextureByWazaTypeIcon( typeTag, m_PlatePaneData[index].iconPane );
}

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

