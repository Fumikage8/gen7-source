//======================================================================
/**
 * @file WazaOshieLowerStatus.cpp
 * @date 2015/07/01 18:58:06
 * @author fukushima_yuya
 * @brief ステータス画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "WazaOshieLowerStatus.h"
#include "WazaOshieDrawerBase.h"
#include "WazaOshieConst.h"

#include <AppLib/include/Tool/app_tool_Gauge.h>
#include <AppLib/include/Tool/AppToolGaugePartsLayout.h>
#include <AppLib/include/Tool/AppCommonGrpIconData.h>
#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Util/app_util_Common.h>

// pml
#include <pml/include/pmlib.h>

// arc
#include <niji_conv_header/app/common_status/common_status_pane.h>
#include <niji_conv_header/app/common_status/common_status_anim_list.h>
#include <niji_conv_header/message/msg_btl_app.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( WazaOshie )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
static const u32 c_DisplayAnim[] = {
  LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000_POS_01,
  LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000_POS_00,
};

static const u32 c_ParamName[] = {
  PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_TEXLIST_00,
  PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_TEXLIST_01,
  PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_TEXLIST_02,
  PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_TEXLIST_03,
  PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_TEXLIST_04,
};

static const u32 c_ParamNum[] = {
  PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_TEXPARAM_00,
  PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_TEXPARAM_01,
  PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_TEXPARAM_02,
  PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_TEXPARAM_03,
  PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_TEXPARAM_04,
};

static const u32 c_HPGaugeAnim[] = {
  LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTS_HP_GAUGE_HP,
  LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTS_HP_COLOR_NORMAL,
  LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTS_HP_COLOR_HALF,
  LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTS_HP_COLOR_FEW,
};

static const u32 c_SkillPlateParts[] = {
  PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSSKILLPLT_00,
  PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSSKILLPLT_01,
  PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSSKILLPLT_02,
  PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSSKILLPLT_03,
  PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSSKILLPLT_04,
};

static const u32 c_PokeParamName[] = {
  STR_ATK,
  STR_DEF,
  STR_SPATK,
  STR_SPDEF,
  STR_AGI,
};

static const u32 c_WazaTypeTopAnim[WazaOshieStatus::SKILL_PLATE_NUM] = {
  LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_00_COLOR_00,
  LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_01_COLOR_00,
  LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_02_COLOR_00,
  LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_03_COLOR_00,
  LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_04_COLOR_00,
};


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 * @param   pBase   描画共通処理
 * @param   mode    画面モード
 */
//------------------------------------------------------------------------------
WazaOshieStatus::WazaOshieStatus( DrawerBase* pBase, u8 mode )
  : m_pBase( pBase )
  , m_Mode( mode )
  , m_Tokusei( TOKUSEI_NULL )
  , m_ItemNo( ITEM_DUMMY_DATA )
  , m_pPokeName( NULL )
  , m_pGenderIcon( NULL )
  , m_pType1Icon( NULL )
  , m_pType2Icon( NULL )
  , m_pType2IconParts( NULL )
  , m_pSickIcon( NULL )
  , m_pLevelIcon( NULL )
  , m_pLevelNum( NULL )
  , m_pHPGauge( NULL )
  , m_pExpGauge( NULL )
  , m_pExpNum( NULL )
  , m_pExpSlash( NULL )
  , m_pExpMaxNum( NULL )
  , m_pTokuseiName( NULL )
  , m_pItemName( NULL )
  , m_pPokeIcon(NULL)
{
  for( u32 i=0; i<SKILL_PLATE_NUM-1; ++i )
  {
    m_WazaNo[i] = WAZANO_NULL;
  }

  Init();
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
WazaOshieStatus::~WazaOshieStatus( void )
{
  GFL_SAFE_DELETE( m_pExpGauge );
  GFL_SAFE_DELETE( m_pHPGauge );
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------------------
void WazaOshieStatus::Init( void )
{
  gfl2::heap::HeapBase* devHeap = m_pBase->GetDeviceHeap();

  m_pPokeName     = m_pBase->GetTextBox( PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_TEXTBOX_00 );
  m_pGenderIcon   = m_pBase->GetPane( PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTS_GENDER );
  m_pLevelIcon    = m_pBase->GetTextBox( PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_TEXTBOX_02 );
  m_pLevelNum     = m_pBase->GetTextBox( PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_TEXTBOX_01 );
  m_pExpNum       = m_pBase->GetTextBox( PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_TEXEXP_00 );
  m_pExpSlash     = m_pBase->GetTextBox( PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_TEXEXP_01 );
  m_pExpMaxNum    = m_pBase->GetTextBox( PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_TEXEXP_02 );
  m_pTokuseiName  = m_pBase->GetTextBox( PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_TEXT_TOKUSEI );
  m_pItemName     = m_pBase->GetTextBox( PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_TEXT_ITEM );

  gfl2::lyt::LytParts* pokeIcontParts = m_pBase->GetParts( PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_POKEICON_04 );
  m_pPokeIcon = m_pBase->GetPicture( PANENAME_COMMON_BTN_LOW_001_GROUP_POKE_ICON, pokeIcontParts );

  gfl2::lyt::LytParts* typeIconParts = m_pBase->GetParts( PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSTYPE_00 );
  m_pType1Icon = m_pBase->GetPicture( PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, typeIconParts );

  m_pType2IconParts = m_pBase->GetParts( PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSTYPE_01 );
  m_pType2Icon      = m_pBase->GetPicture( PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, m_pType2IconParts );

  gfl2::lyt::LytParts* sickIconParts = m_pBase->GetParts( PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTS_SICK );
  m_pSickIcon = m_pBase->GetPicture( PANENAME_COMMON_ICN_002_PANE_ICON_SICK, sickIconParts );

  for( u32 i=0; i<POKE_PARAM_NUM; ++i )
  {
    m_pParamName[i] = m_pBase->GetTextBox( c_ParamName[i] );
    m_pParamNum[i]  = m_pBase->GetTextBox( c_ParamNum[i] );
  }

  for( u32 i=0; i<SKILL_PLATE_NUM; ++i )
  {
    m_pWazaPlate[i]     = m_pBase->GetParts( c_SkillPlateParts[i] );
    m_pWazaName[i]      = m_pBase->GetTextBox( PANENAME_STATUS_PLT_LOW_000_PANE_TEXTBOX_00, m_pWazaPlate[i] );
    m_pWazaPP[i]        = m_pBase->GetTextBox( PANENAME_STATUS_PLT_LOW_000_PANE_TEXTBOX_01, m_pWazaPlate[i] );
    m_pWazaPPNum[i]     = m_pBase->GetTextBox( PANENAME_STATUS_PLT_LOW_000_PANE_TEXTBOX_02, m_pWazaPlate[i] );
    m_pWazaPPSlash[i]   = m_pBase->GetTextBox( PANENAME_STATUS_PLT_LOW_000_PANE_TEXTBOX_04, m_pWazaPlate[i] );
    m_pWazaMaxPPNum[i]  = m_pBase->GetTextBox( PANENAME_STATUS_PLT_LOW_000_PANE_TEXTBOX_03, m_pWazaPlate[i] );
    m_pNullPassive[i]   = m_pBase->GetPane( PANENAME_STATUS_PLT_LOW_000_PANE_NULL_PASSIVE, m_pWazaPlate[i] );
    m_pNullCursor[i]    = m_pBase->GetPane( PANENAME_STATUS_PLT_LOW_000_PANE_NULL_CUR, m_pWazaPlate[i] );

    gfl2::lyt::LytParts* typeParts = m_pBase->GetParts( PANENAME_STATUS_PLT_LOW_000_PANE_PARTS_TYPE, m_pWazaPlate[i] );
    m_pWazaType[i] = m_pBase->GetPicture( PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, typeParts );
  }

  // HPゲージ
  gfl2::lyt::LytParts* pHPGaugeParts = m_pBase->GetParts( PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTS_HP );
  app::tool::AppToolHPGaugePartsLayout::INIT_PARAM hpParam;
  {
    hpParam.p_g2d_util                    = m_pBase->GetG2D();
    hpParam.layout_id                     = 0;
    hpParam.number_message_id             = STR_VALUE_NUM_3;
    hpParam.number_keta                   = 3;
    hpParam.gauge_hp_anime_idx            = c_HPGaugeAnim[0];
    hpParam.color_normal_anime_idx        = c_HPGaugeAnim[1];
    hpParam.color_half_anime_idx          = c_HPGaugeAnim[2];
    hpParam.color_few_anime_idx           = c_HPGaugeAnim[3];
    hpParam.p_left_number_text_box_pane   = m_pBase->GetTextBox( PANENAME_COMMON_GAG_000_PANE_TEXTBOX_00, pHPGaugeParts );
    hpParam.p_center_number_text_box_pane = m_pBase->GetTextBox( PANENAME_COMMON_GAG_000_PANE_TEXTBOX_01, pHPGaugeParts );
    hpParam.p_right_number_text_box_pane  = m_pBase->GetTextBox( PANENAME_COMMON_GAG_000_PANE_TEXTBOX_02, pHPGaugeParts );

    m_pHPGauge = GFL_NEW(devHeap) app::tool::AppToolHPGaugePartsLayout( hpParam, devHeap );
    GFL_ASSERT( m_pHPGauge );
  }

  // EXPゲージ
  app::tool::AppToolGaugePartsLayout::INIT_PARAM expParam;
  {
    expParam.p_g2d_util         = m_pBase->GetG2D();
    expParam.layout_id          = 0;
    expParam.p_parts_pane       = m_pBase->GetParts( PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTS_EXP );
    expParam.gauge_hp_anime_idx = LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTS_EXP_GAUGE_00;

    m_pExpGauge = GFL_NEW(devHeap) app::tool::AppToolGaugePartsLayout( expParam, devHeap );
  }

  // 固定テキストのセット
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    view->SetTextboxPaneMessage( m_pLevelIcon, str_btl_poke_lv );
    view->SetTextboxPaneMessage( m_pExpSlash, str_btl_poke_hp_slash );

    for( u32 i=0; i<POKE_PARAM_NUM; ++i )
    {
      view->SetTextboxPaneMessage( m_pParamName[i], c_PokeParamName[i] );
    }

    for( u32 i=0; i<SKILL_PLATE_NUM; ++i )
    {
      view->SetTextboxPaneMessage( m_pWazaPP[i], STR_PP );
      view->SetTextboxPaneMessage( m_pWazaPPSlash[i], str_btl_poke_hp_slash );
    }
  }

  // モードごとのレイアウト表示の変更
  m_pBase->StartAnim( c_DisplayAnim[m_Mode] );
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void WazaOshieStatus::Update( void )
{
  if( m_pHPGauge )
  {
    m_pHPGauge->Update();
  }

  if( m_pExpGauge )
  {
    m_pExpGauge->Update();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   セットアップ
 * @param   pCmnIcon    アイコン
 * @param   moudoku     猛毒フラグ
 * @param   pp          ポケモンパラメータ
 * @param   newWazaNo   新しく覚える技ID
 */
//------------------------------------------------------------------------------
void WazaOshieStatus::Setup( app::tool::AppCommonGrpIconData* pCmnIcon, bool moudoku, const pml::pokepara::PokemonParam* pp, WazaNo newWazaNo )
{
  bool fastmode_flag = pp->StartFastMode();
  {
    // ポケアイコン
    SetPokeIcon( pp );
    // ポケモン名
    SetPokeName( pp );
    // タイプアイコン
    SetTypeIcon( pp, pCmnIcon );
    // 状態異常
    SetCommonIcon( pp, pCmnIcon, moudoku );
    // レベル
    SetLevel( pp );
    // パラメータ
    SetParameter( pp );
    // 経験値
    SetExpGauge( pp );
    // 技
    SetWaza( pp, pCmnIcon );
    // 特性
    SetTokusei( pp );
    // アイテム
    SetItem( pp );
    // 新技
    SetNewWaza( pp, pCmnIcon, newWazaNo );
  }
  pp->EndFastMode( fastmode_flag );

  // 性別
  SetGender( pp );
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケアイコンのセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void WazaOshieStatus::SetPokeIcon( const pml::pokepara::PokemonParam* pp )
{
  app::tool::PokeIconToolEx* pokeIcon = m_pBase->GetPokeIcon();

  if( pokeIcon )
  {
    pokeIcon->SetPokeIcon( 0, pp, m_pPokeIcon );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモン名のセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void WazaOshieStatus::SetPokeName( const pml::pokepara::PokemonParam* pp )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    print::WordSet* wordSet = m_pBase->GetWordSet();
    {
      wordSet->RegisterPokeNickName( 0, pp );
      view->SetTextboxPaneMessage( m_pPokeName, STR_POKE_NAME );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   性別のセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void WazaOshieStatus::SetGender( const pml::pokepara::PokemonParam* pp )
{
  const u32 c_GenderIconAnim[] = {
    LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTS_GENDER_ICON_MALE,
    LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTS_GENDER_ICON_FEMALE
  };

  // 性別の取得
  pml::Sex sex = PokeTool::GetDrawSex(pp);
  {
    if( sex != pml::SEX_UNKNOWN )
    {
      m_pBase->StartAnim( c_GenderIconAnim[sex] );
      m_pBase->SetVisiblePane( m_pGenderIcon, true );
    }
    else {
      // 性別なしの場合は非表示
      m_pBase->SetVisiblePane( m_pGenderIcon, false );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   タイプアイコンのセット
 *
 * @param   pp        ポケモンパラメータ
 * @param   cmnIcon   共通アイコン
 */
//------------------------------------------------------------------------------
void WazaOshieStatus::SetTypeIcon( const pml::pokepara::PokemonParam* pp, app::tool::AppCommonGrpIconData* cmnIcon )
{
  pml::PokeType type1 = pp->GetType1();   //!< タイプ１の取得
  pml::PokeType type2 = pp->GetType2();   //!< タイプ２の取得

  // タイプ１のアイコンをセット
  cmnIcon->ReplacePaneTextureByWazaTypeIcon( static_cast<PokeType_tag>(type1), m_pType1Icon );

  if( type1 != type2 )
  {
    cmnIcon->ReplacePaneTextureByWazaTypeIcon( static_cast<PokeType_tag>(type2), m_pType2Icon );
    m_pBase->SetVisiblePane( m_pType2IconParts, true );
  }
  else {
    // タイプ２はないので非表示
    m_pBase->SetVisiblePane( m_pType2IconParts, false );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   状態異常アイコンのセット
 *
 * @param   pp        ポケモンパラメータ
 * @param   cmnIcon   共通アイコン
 * @param   moudoku   猛毒フラグ
 */
//------------------------------------------------------------------------------
void WazaOshieStatus::SetCommonIcon( const pml::pokepara::PokemonParam* pp, app::tool::AppCommonGrpIconData* cmnIcon, bool moudoku )
{
  bool visible = false;

  if( pp->GetHp() == 0 )
  {
    cmnIcon->ReplacePaneTextureByPokeHinshiIcon( m_pSickIcon );
    visible = true;
  }
  else if( moudoku )
  {
    cmnIcon->ReplacePaneTextureByPokeDokudokuIcon( m_pSickIcon );
    visible = true;
  }
  else if( pp->HaveSick() != false )
  {
    cmnIcon->ReplacePaneTextureByPokeSickIcon( pp->GetSick(), m_pSickIcon );
    visible = true;
  }
  else if( pp->HavePokerusJustNow() )   //!< 今現在ポケルスに感染中かどうか
  {
    // ポケルスアイコンに変更
    cmnIcon->ReplacePaneTextureByPokerusIcon( m_pSickIcon );
    visible = true;
  }

  // アイコンの表示切替
  m_pBase->SetVisiblePane( m_pSickIcon, visible );
}

//------------------------------------------------------------------------------
/**
 * @brief   レベルのセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void WazaOshieStatus::SetLevel( const pml::pokepara::PokemonParam* pp )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    u32 level = pp->GetLevel();

    view->SetTextboxPaneNumber( m_pLevelNum, STR_VALUE_NUM_3, level, 3 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   パラメータのセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void WazaOshieStatus::SetParameter( const pml::pokepara::PokemonParam* pp )
{
  // HP
  if( m_pHPGauge )
  {
    u32 nowHP = pp->GetHp();
    u32 maxHP = pp->GetMaxHp();

    m_pHPGauge->SetNumber( nowHP, maxHP );
  }

  const pml::pokepara::PowerID c_PowerID[POKE_PARAM_NUM] = {
    pml::pokepara::POWER_ATK,     //!< こうげき
    pml::pokepara::POWER_DEF,     //!< ぼうぎょ
    pml::pokepara::POWER_SPATK,   //!< とくこう
    pml::pokepara::POWER_SPDEF,   //!< とくぼう
    pml::pokepara::POWER_AGI,     //!< すばやさ
  };

  // パラメータ
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    for( u32 i=0; i<POKE_PARAM_NUM; ++i )
    {
      view->SetTextboxPaneNumber(
        m_pParamNum[i], STR_VALUE_NUM_3, pp->GetPower( c_PowerID[i] ), 3 );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   経験値のセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void WazaOshieStatus::SetExpGauge( const pml::pokepara::PokemonParam* pp )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    u32 nowExp  = pp->GetExp() - pp->GetExpForCurrentLevel();
    u32 nextExp = pp->GetExpForNextLevel() - pp->GetExpForCurrentLevel();

    view->SetTextboxPaneNumber( m_pExpNum, str_btl_poke_exp_s, nowExp, 5 );
    view->SetTextboxPaneNumber( m_pExpMaxNum, str_btl_poke_exp_s, nextExp, 5 );

    if( m_pExpGauge )
    {
      m_pExpGauge->SetNumber( nowExp, nextExp );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   技のセット
 *
 * @param   pp        ポケモンパラメータ
 * @param   cmnIcon   共通アイコン
 */
//------------------------------------------------------------------------------
void WazaOshieStatus::SetWaza( const pml::pokepara::PokemonParam* pp, app::tool::AppCommonGrpIconData* cmnIcon )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    app::util::G2DUtil*       g2d     = m_pBase->GetG2D();
    print::WordSet*           wordSet = m_pBase->GetWordSet();
    app::tool::ButtonManager* btnMan  = m_pBase->GetButtonManager();

    for( u32 i=0; i<(SKILL_PLATE_NUM - 1); ++i )
    {
      if( i < pp->GetWazaCount() )
      {
        m_WazaNo[i]    = pp->GetWazaNo( i );
        m_WazaNowPP[i] = pp->GetWazaPP( i );
        m_WazaMaxPP[i] = pp->GetWazaMaxPP( i );

        // 技名
        wordSet->RegisterWazaName( 0, m_WazaNo[i] );
        view->SetTextboxPaneMessage( m_pWazaName[i], STR_WAZA_NAME );

        // PP
        view->SetTextboxPaneNumber( m_pWazaPPNum[i], STR_VALUE_2, m_WazaNowPP[i], 2 );
        view->SetTextboxPaneNumber( m_pWazaMaxPPNum[i], STR_VALUE_2, m_WazaMaxPP[i], 2 );

        gfl2::lyt::ColorU8 color = app::util::GetWazaTextColor( m_WazaNowPP[i], m_WazaMaxPP[i] );
        {
          g2d->SetTextBoxPaneColor( m_pWazaPP[i], color, color );         //!< 色替え
          g2d->SetTextBoxPaneColor( m_pWazaPPNum[i], color, color );      //!< 色替え
          g2d->SetTextBoxPaneColor( m_pWazaPPSlash[i], color, color );    //!< 色替え
          g2d->SetTextBoxPaneColor( m_pWazaMaxPPNum[i], color, color );   //!< 色替え
        }

        // 属性
        pml::PokeType type = pml::wazadata::GetType( m_WazaNo[i] );
        {
          m_pBase->StartAnim( c_WazaTypeTopAnim[i] + type );

          // タイプアイコン
          cmnIcon->ReplacePaneTextureByWazaTypeIcon( static_cast<PokeType_tag>(type), m_pWazaType[i] );
        }

        // ボタンをアクティブ
        btnMan->SetButtonActive( BTN_ID_WAZA_0 + i );
        m_pBase->SetVisiblePane( m_pNullPassive[i], true );
      }
      else {
        m_pBase->StartAnim( c_WazaTypeTopAnim[0] );

        // ボタンをパッシブ
        btnMan->SetButtonPassive( BTN_ID_WAZA_0 + i );
        m_pBase->SetVisiblePane( m_pNullPassive[i], false );
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   特性のセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void WazaOshieStatus::SetTokusei( const pml::pokepara::PokemonParam* pp )
{
  if( m_Mode == MODE_WAZA_WASURE )
  {
    // 特性の取得
    m_Tokusei = pp->GetTokuseiNo();

    app::ui::UIView* view = m_pBase->GetUIView();
    {
      print::WordSet* wordSet = m_pBase->GetWordSet();
      {
        wordSet->RegisterTokuseiName( 0, m_Tokusei );
        view->SetTextboxPaneMessage( m_pTokuseiName, STR_TOKUSEI_NAME );
      }
    }

    m_pBase->SetVisiblePane( m_pTokuseiName, true );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   アイテムのセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void WazaOshieStatus::SetItem( const pml::pokepara::PokemonParam* pp )
{
  if( m_Mode == MODE_WAZA_WASURE )
  {
    // アイテムの取得
    m_ItemNo = pp->GetItem();

    // アイテムを所持している
    if( m_ItemNo != ITEM_DUMMY_DATA )
    {
      app::ui::UIView* view = m_pBase->GetUIView();
      {
        print::WordSet* wordSet = m_pBase->GetWordSet();
        {
          wordSet->RegisterItemName( 0, m_ItemNo, 1 );
          view->SetTextboxPaneMessage( m_pItemName, STR_ITEM_NAME );
        }
      }

      m_pBase->SetVisiblePane( m_pItemName, true );

      // インフォメーションボタンの入力を有効に
      app::tool::ButtonManager* btnMan = m_pBase->GetButtonManager();
      {
        btnMan->SetButtonInputEnable( BTN_ID_INFO_1, true );
      }
    }
    // アイテムを未所持
    else {
      m_pBase->SetVisiblePane( m_pItemName, false );

      // インフォメーションボタンの入力を無効に
      app::tool::ButtonManager* btnMan = m_pBase->GetButtonManager();
      {
        btnMan->SetButtonInputEnable( BTN_ID_INFO_1, false );
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   新規技のセット
 *
 * @param   pp        ポケモンパラメータ
 * @param   cmnIcon   共通アイコン
 * @param   wazaNo    覚える技No
 */
//------------------------------------------------------------------------------
void WazaOshieStatus::SetNewWaza( const pml::pokepara::PokemonParam* pp, app::tool::AppCommonGrpIconData* cmnIcon, WazaNo wazaNo )
{
  if( m_Mode == MODE_WAZA_OBOE )
  {
    GFL_ASSERT( wazaNo != WAZANO_NULL );

    app::ui::UIView* view = m_pBase->GetUIView();
    {
      print::WordSet* wordSet = m_pBase->GetWordSet();

      // 技名
      wordSet->RegisterWazaName( 0, wazaNo );
      view->SetTextboxPaneMessage( m_pWazaName[NEW_SKILL_IDX], STR_WAZA_NAME );

      // PP
      u8 ppNum = pml::wazadata::GetMaxPP( wazaNo, 0 );
      {
        view->SetTextboxPaneNumber( m_pWazaPPNum[NEW_SKILL_IDX], STR_VALUE_2, ppNum, 2 );
        view->SetTextboxPaneNumber( m_pWazaMaxPPNum[NEW_SKILL_IDX], STR_VALUE_2, ppNum, 2 );
      }

      // タイプ
      pml::PokeType type = pml::wazadata::GetType( wazaNo );
      {
        // アニメの変更
        m_pBase->StartAnim( c_WazaTypeTopAnim[NEW_SKILL_IDX] + type );
        // アイコンの変更
        cmnIcon->ReplacePaneTextureByWazaTypeIcon( static_cast<PokeType_tag>(type), m_pWazaType[NEW_SKILL_IDX] );
      }

      m_pBase->SetVisiblePane( m_pTokuseiName, false );
      m_pBase->SetVisiblePane( m_pItemName, false );
    }
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   説明画面に必要なデータのセットアップ
 *
 * @param   index   習得技のインデックス[IN]
 * @param   param   セットアップパラメータ[OUT]
 */
//------------------------------------------------------------------------------
void WazaOshieStatus::SetExplainSetupParam( u32 index, WazaOshieExplain::SetupParam* param )
{
  param->wazaNo = m_WazaNo[index];
  param->nowPP  = m_WazaNowPP[index];
  param->maxPP  = m_WazaMaxPP[index];
}

//------------------------------------------------------------------------------
/**
 * @brief   特性IDの取得
 * @return  TokuseiNo
 */
//------------------------------------------------------------------------------
TokuseiNo WazaOshieStatus::GetActTokuseiNo( void ) const
{
  return m_Tokusei;
}

//------------------------------------------------------------------------------
/**
 * @brief   技IDの取得
 * @return  WazaNo
 */
//------------------------------------------------------------------------------
u16 WazaOshieStatus::GetActItemNo( void ) const
{
  return m_ItemNo;
}

GFL_NAMESPACE_END( WazaOshie )
GFL_NAMESPACE_END( App )
