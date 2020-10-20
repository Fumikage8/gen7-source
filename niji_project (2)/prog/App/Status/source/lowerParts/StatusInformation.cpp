//======================================================================
/**
 * @file    StatusInformation.cpp
 * @date    2015/12/03 12:03:10
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：情報(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusInformation.h"

#include <App/Status/source/StatusDrawerBase.h>

#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Tool/AppCommonGrpIconData.h>
#include <PokeTool/include/PokeToolPokePlace.h>

// pml
#include <pml/include/pmlib.h>

// arc
#include <niji_conv_header/app/field_status/field_status_pane.h>
#include <niji_conv_header/app/field_status/field_status_anim_list.h>

#include <niji_conv_header/message/msg_pokemon_status.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  //! @brief    ボックスマークの色変更アニメ
  static const u32 c_BoxMarkAnim[] = {
    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_MARKPLT_ICON_00,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_MARKPLT_ICON_01,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_MARKPLT_ICON_02,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_MARKPLT_ICON_03,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_MARKPLT_ICON_04,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_MARKPLT_ICON_05,
  };

  //! @brief    アイテムプレート
  const u32 c_ItemPlateTable[] = {
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_FONT_BAR02,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_FONT_BAR04,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_TEXT_ITEM_00,
  };
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
Information::Information( DrawerBase* base, app::tool::AppCommonGrpIconData* cmnIcon )
  : m_pBase(base)
  , m_pListener(NULL)
  , m_pPokeParam(NULL)
  , m_pCommonIcon(cmnIcon)
  , m_HaveItem(false)
  , m_pTextNickName(NULL)
  , m_pTextLevel(NULL)
  , m_pGenderIcon(NULL)
  , m_pSickIcon(NULL)
  , m_pRareIcon(NULL)
  , m_pPokerusuIcon(NULL)
  , m_pSantiIcon(NULL)
  , m_pTextTokusei(NULL)
  , m_pTextItem(NULL)
  , m_pEggVisible(NULL)
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化
 *
 * @param   listener          リスナー
 * @param   ribbonListener    リボン用リスナー
 */
//------------------------------------------------------------------------------
void Information::Initialize( InformationListener* listener, InformationRibbonListener* ribbonListener )
{
  // リスナーのセット
  m_pListener       = listener;
  m_pRibbonListener = ribbonListener;

  // ペインのセット
  {
    // ポケモンのニックネーム
    m_pTextNickName = m_pBase->GetTextBox( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_TEXTBOX_00 );
    // ポケモンのレベル
    m_pTextLevel    = m_pBase->GetTextBox( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_TEXTBOX_02 );
    // ポケモンの性別
    m_pGenderIcon   = m_pBase->GetPane( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTS_GENDER );
    // 状態異常アイコン
    gfl2::lyt::LytParts* parts = m_pBase->GetParts( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTS_SICK );
    m_pSickIcon     = m_pBase->GetPicture( PANENAME_COMMON_ICN_002_PANE_ICON_SICK, parts );
    // レアアイコン
    m_pRareIcon     = m_pBase->GetPane( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_ICON_RARE );
    // ポケルスアイコン
    m_pPokerusuIcon = m_pBase->GetPane( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_ICON_POKERUS );
    // 産地アイコン
    m_pSantiIcon    = m_pBase->GetPane( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTS_SANCHI );
    // ポケモンの特性
    m_pTextTokusei  = m_pBase->GetTextBox( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_TEXT_TOKUSEI );
    // ポケモンの所持アイテム
    m_pTextItem     = m_pBase->GetTextBox( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_TEXT_ITEM );
    // タマゴの表示切替ペイン
    m_pEggVisible   = m_pBase->GetPane( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_ALL_EGG );
    // ロックアイコン
    m_pLockIcon     = m_pBase->GetPane( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_ICONPARTS_LOCK );
    // アイテムのプレート
    for( u32 i=0; i<GFL_NELEMS(c_ItemPlateTable); ++i )
    {
      m_pPaneItemPlate[i] = m_pBase->GetPane( c_ItemPlateTable[i] );
    }
  }

  // 初期テキスト
  InitializeText();

  // アニメの再生
  {
    for( u32 i=0; i<pml::pokepara::BOX_MARK_NUM; ++i )
    {
      m_pBase->StartAnim( c_BoxMarkAnim[i], false );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト
 */
//------------------------------------------------------------------------------
void Information::InitializeText( void )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    gfl2::lyt::LytTextBox* textbox = m_pBase->GetTextBox( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_TEXTBOX_01 );
    {
      view->SetTextboxPaneMessage( textbox, PARAM_LEVEL );
    }

    gfl2::lyt::LytTextBox* textInfoTokusei = m_pBase->GetTextBox( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_TEXT_TOKUSEI_00 );
    {
      view->SetTextboxPaneMessage( textInfoTokusei, PARAM_TAB_01 );
    }

    gfl2::lyt::LytTextBox* textInfoItem = m_pBase->GetTextBox( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_TEXT_ITEM_00 );
    {
      view->SetTextboxPaneMessage( textInfoItem, PARAM_TAB_02 );
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   セットアップ
 *
 * @param   pp    ポケモンパラメータ
 * @param   lock  ロックフラグ
 */
//------------------------------------------------------------------------------
void Information::Setup( const pml::pokepara::PokemonParam* pp, bool lock )
{
  //if( m_pPokeParam != pp )
  {
    bool fastmode_flag = pp->StartFastMode();
    {
      bool isEgg = pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG );

      // ニックネームのセット
      SetNickName( pp );
      // レベルのセット
      SetLevel( pp );
      // 状態異常アイコンのセット
      SetStateIcon( pp );
      // レアアイコンのセット
      SetRareIcon( pp, isEgg );
      // ポケルスアイコンのセット
      SetPokerusuIcon( pp, isEgg );
      // 産地アイコンのセット
      SetSanchiIcon( pp, isEgg );
      // 特性のセット
      SetTokusei( pp );
      // アイテムのセット
      SetItem( pp );
      // ボックスマークのセット
      SetBoxMark( pp );
      // ロックアイコンのセット
      SetLockIcon( lock );
    }
    pp->EndFastMode( fastmode_flag );

    // 性別アイコンのセット
    SetGenderIcon( pp );

    m_pPokeParam = pp;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ニックネームのセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void Information::SetNickName( const pml::pokepara::PokemonParam* pp )
{
  app::ui::UIView* view   = m_pBase->GetUIView();
  print::WordSet* wordSet = m_pBase->GetWordSet();

  // ニックネームの登録
  wordSet->RegisterPokeNickName( 0, pp );
  // ニックネームのセット
  view->SetTextboxPaneMessage( m_pTextNickName, PARAM_POKE_NAME );
}

//------------------------------------------------------------------------------
/**
 * @brief   性別アイコンのセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void Information::SetGenderIcon( const pml::pokepara::PokemonParam* pp )
{
  const u32 c_GenderAnim[] = {
    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_GENDER_ICON_MALE,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_GENDER_ICON_FEMALE,
  };

  pml::Sex sex = PokeTool::GetDrawSex( pp );
  bool visible = (sex != pml::SEX_UNKNOWN);

  if( visible )
  {
    // 男女の変更アニメの再生
    m_pBase->StartAnim( c_GenderAnim[sex] );
  }

  // 性別アイコンの表示切替
  m_pBase->SetVisiblePane( m_pGenderIcon, visible );
}

//------------------------------------------------------------------------------
/**
 * @brief   レベルのセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void Information::SetLevel( const pml::pokepara::PokemonParam* pp )
{
  app::ui::UIView* view = m_pBase->GetUIView();

  // レベルの取得
  u32 level = pp->GetLevel();

  // レベルのセット
  view->SetTextboxPaneNumber( m_pTextLevel, PARAM_LEVEL__VALUE, level, 3 );
}

//------------------------------------------------------------------------------
/**
 * @brief   状態異常アイコンのセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void Information::SetStateIcon( const pml::pokepara::PokemonParam* pp )
{
  bool visible = false;

  if( pp->GetHp() == 0 )
  {
    // ひんしアイコンに変更
    m_pCommonIcon->ReplacePaneTextureByPokeHinshiIcon( m_pSickIcon );
    visible = true;
  }
  //else if( false )
  //{
  //  // もうどくアイコンに変更
  //  m_pCommonIcon->ReplacePaneTextureByPokeDokudokuIcon( m_pSickIcon );
  //  visible = true;
  //}
  else if( pp->HaveSick() != false )
  {
    // 状態異常ごとのアイコンに変更
    m_pCommonIcon->ReplacePaneTextureByPokeSickIcon( pp->GetSick(), m_pSickIcon );
    visible = true;
  }
  else if( pp->HavePokerusJustNow() )   //!< 今現在ポケルスに感染中かどうか
  {
    // ポケルスアイコンに変更
    m_pCommonIcon->ReplacePaneTextureByPokerusIcon( m_pSickIcon );
    visible = true;
  }

  // 状態異常アイコンの表示切替
  m_pBase->SetVisiblePane( m_pSickIcon, visible );
}

//------------------------------------------------------------------------------
/**
 * @brief   レアアイコンのセット
 *
 * @param   pp      ポケモンパラメータ
 * @param   isEgg   タマゴフラグ
 */
//------------------------------------------------------------------------------
void Information::SetRareIcon( const pml::pokepara::PokemonParam* pp, bool isEgg )
{
  // レアアイコンの表示切替
  m_pBase->SetVisiblePane( m_pRareIcon, (!isEgg && pp->IsRare()) );
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケルスアイコンのセット
 *
 * @param   pp      ポケモンパラメータ
 * @param   isEgg   タマゴフラグ
 */
//------------------------------------------------------------------------------
void Information::SetPokerusuIcon( const pml::pokepara::PokemonParam* pp, bool isEgg )
{
  // ポケルスアイコンの表示切替
  m_pBase->SetVisiblePane( m_pPokerusuIcon, (!isEgg && pp->HavePokerusPast()) );
}

//------------------------------------------------------------------------------
/**
 * @brief   産地アイコンのセット
 *
 * @param   pp      ポケモンパラメータ
 * @param   isEgg   タマゴフラグ
 */
//------------------------------------------------------------------------------
void Information::SetSanchiIcon( const pml::pokepara::PokemonParam* pp, bool isEgg )
{
  const u32 c_SanchiIconAnim[] = {
    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_SANCHI_ICON_6TH,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_SANCHI_ICON_ALORA,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_SANCHI_ICON_NONE,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_SANCHI_ICON_VC,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_SANCHI_ICON_HOROHORO,
  };

  if( !isEgg )
  {
    // 産地情報の取得
    u8 version = pp->GetCassetteVersion();
    PokeTool::PokePlace::PlaceMarkID markID = PokeTool::PokePlace::GetPlaceMarkID( version );

    // 産地アイコンを表示
    m_pBase->SetVisiblePane( m_pSantiIcon, true );

    // 産地アイコンのセット
    m_pBase->StartAnim( c_SanchiIconAnim[markID] );
  }
  else {
    // 産地アイコンを非表示
    m_pBase->SetVisiblePane( m_pSantiIcon, false );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   特性のセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void Information::SetTokusei( const pml::pokepara::PokemonParam* pp )
{
  app::ui::UIView* view   = m_pBase->GetUIView();
  print::WordSet* wordSet = m_pBase->GetWordSet();

  // ポケモンの特性を取得
  TokuseiNo tokusei = pp->GetTokuseiNo();

  // 特性名の登録
  wordSet->RegisterTokuseiName( 0, tokusei );
  // 特性名のセット
  view->SetTextboxPaneMessage( m_pTextTokusei, PARAM_TOKUSEI_VALUE );
}

//------------------------------------------------------------------------------
/**
 * @brief   アイテムのセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void Information::SetItem( const pml::pokepara::PokemonParam* pp )
{
  app::ui::UIView* view   = m_pBase->GetUIView();
  print::WordSet* wordSet = m_pBase->GetWordSet();

  // ポケモンの所持アイテムを取得
  u16 itemNo = pp->GetItem();

  m_HaveItem = (itemNo != ITEM_DUMMY_DATA);

  if( m_HaveItem )
  {
    // 特性名の登録
    wordSet->RegisterItemName( 0, itemNo, 1 );
    // 特性名のセット
    view->SetTextboxPaneMessage( m_pTextItem, PARAM_ITEM_VALUE );
  }

  // アイテムの表示切替
  m_pBase->SetVisiblePane( m_pTextItem, m_HaveItem );
  // アイテムプレートの表示切替
  for( u32 i=0; i<GFL_NELEMS(c_ItemPlateTable); ++i )
  {
    m_pBase->SetVisiblePane( m_pPaneItemPlate[i], m_HaveItem );
  }

  // ボタンの入力フラグを設定
  m_pListener->SetInputEnableItemInfoBtn( m_HaveItem );
}

//------------------------------------------------------------------------------
/**
 * @brief   ロックアイコンのセット
 *
 * @param   lock    ロックフラグ
 */
//------------------------------------------------------------------------------
void Information::SetLockIcon( bool lock )
{
  m_pBase->SetVisiblePane( m_pLockIcon, lock );
}

//------------------------------------------------------------------------------
/**
 * @brief   ボックスマークのセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void Information::SetBoxMark( const pml::pokepara::PokemonParam* pp )
{
  FUKUSHIMA_PRINT( "%s : color[", __FUNCTION__ );

#ifdef DEBUG_ONLY_FOR_fukushima_yuya
  static const char* c_ColorName[] = {
    "NONE", "BLUE", "RED",
  };
#endif

  for( u32 i=0; i<pml::pokepara::BOX_MARK_NUM; ++i )
  {
    pml::pokepara::BoxMarkColor color =
      pp->GetBoxMark( static_cast<pml::pokepara::BoxMark>(i) );

    m_pBase->SetAnimFrame( c_BoxMarkAnim[i], static_cast<f32>(color) );

#ifdef DEBUG_ONLY_FOR_fukushima_yuya
    FUKUSHIMA_PRINT( " %d=%s", i+1, c_ColorName[color] );
#endif
  }

  FUKUSHIMA_PRINT( " ]\n" );
}


//------------------------------------------------------------------------------
/**
 * @brief   タマゴ専用モードの設定
 *
 * @param   flag    タマゴ専用モードフラグ(true = タマゴ専用、false = 通常モード)
 */
//------------------------------------------------------------------------------
void Information::SetEggMode( bool flag )
{
  // 表示切替
  m_pBase->SetVisiblePane( m_pEggVisible, !flag );
}

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)
