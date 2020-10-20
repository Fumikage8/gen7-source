//==============================================================================
/**
 * @file    PokeListPokeInfoDraw.cpp
 * @author  fukushima_yuya
 * @date    2015.03.09
 * @brief   プレート内のポケモンの情報部分
 *          sango_project app_pokelist_Drawer
 *          sango_project app_pokelist_ListDrawer
 */
//==============================================================================


//niji
#include "App/PokeList/source/parts/PokeListDrawerBase.h"
#include "App/PokeList/source/parts/PokeListPokeInfoDraw.h"
#include "App/PokeList/source/work/PokeListLocalWork.h"
#include "App/PokeList/source/work/PokeListStaticWork.h"

#include <AppLib/include/Tool/AppCommonGrpIconData.h>
#include <AppLib/include/Tool/AppToolGaugePartsLayout.h>
#include <AppLib/include/Tool/app_tool_Gauge.h>
#include <AppLib/include/Tool/app_tool_PokeIcon.h>
#include <AppLib/include/ui/UIView.h>

// pml
#include <pml/include/pmlib.h>

// arc
#include <niji_conv_header/app/common_pokeList/common_pokeList_anim_list.h>
#include <niji_conv_header/app/common_pokeList/common_pokeList_pane.h>
#include <niji_conv_header/message/msg_pokelist.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )


//------------------------------------------------------------------------------
// @brief   変数
//------------------------------------------------------------------------------
//! @brief  HPゲージアニメ(CursorPos順)
const u32 c_HpGaugeAnimTbl[PLATE_NUM][4] = {
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_HP_GAUGE_HP,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_HP_COLOR_NORMAL,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_HP_COLOR_HALF,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_HP_COLOR_FEW,
  },
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03__PARTS_HP_GAUGE_HP,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03__PARTS_HP_COLOR_NORMAL,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03__PARTS_HP_COLOR_HALF,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03__PARTS_HP_COLOR_FEW,
  },
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01__PARTS_HP_GAUGE_HP,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01__PARTS_HP_COLOR_NORMAL,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01__PARTS_HP_COLOR_HALF,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01__PARTS_HP_COLOR_FEW,
  },
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04__PARTS_HP_GAUGE_HP,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04__PARTS_HP_COLOR_NORMAL,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04__PARTS_HP_COLOR_HALF,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04__PARTS_HP_COLOR_FEW,
  },
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02__PARTS_HP_GAUGE_HP,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02__PARTS_HP_COLOR_NORMAL,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02__PARTS_HP_COLOR_HALF,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02__PARTS_HP_COLOR_FEW,
  },
  {
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05__PARTS_HP_GAUGE_HP,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05__PARTS_HP_COLOR_NORMAL,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05__PARTS_HP_COLOR_HALF,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05__PARTS_HP_COLOR_FEW,
  },
};

//! @brief  性別アニメ(CursorPos順)
const u32 c_SexIconAnimTbl[PLATE_NUM][2] = {
  { //! 左上
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_GENDER_ICON_MALE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00__PARTS_GENDER_ICON_FEMALE,
  },
  { //! 右上
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03__PARTS_GENDER_ICON_MALE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03__PARTS_GENDER_ICON_FEMALE,
  },
  { //! 左中
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01__PARTS_GENDER_ICON_MALE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01__PARTS_GENDER_ICON_FEMALE,
  },
  { //! 右中
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04__PARTS_GENDER_ICON_MALE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04__PARTS_GENDER_ICON_FEMALE,
  },
  { //! 左下
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02__PARTS_GENDER_ICON_MALE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02__PARTS_GENDER_ICON_FEMALE,
  },
  { //! 右下
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05__PARTS_GENDER_ICON_MALE,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05__PARTS_GENDER_ICON_FEMALE,
  },
};

enum IconSeq
{
  ICON_SEQ_IDLE,
  ICON_SEQ_START,
  ICON_SEQ_REPLACE,
  ICON_SEQ_END,
};



//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
PokeInfoDraw::PokeInfoDraw( DrawerBase* pBase, CursorPos cpos )
  : m_pBase( pBase )
  , m_pHPGauge( NULL )
  , m_cCursorPos( cpos )
  , m_bLvVisible( true )
  , m_SimpleParam()
  , m_IconSeq(ICON_SEQ_IDLE)
  , m_pPlateParts( NULL )
  , m_pHPGaugeParts( NULL )
  , m_pNickNamePane( NULL )
  , m_pGenderParts( NULL )
  , m_pGenderIcon( NULL )
  , m_pLVText( NULL )
  , m_pLVValueText( NULL )
  , m_pSickParts( NULL )
  , m_pSickIcon( NULL )
  , m_pPokeIconParts( NULL )
  , m_pItemIconPane( NULL )
{
  Init();
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
PokeInfoDraw::~PokeInfoDraw( void )
{
  GFL_DELETE m_pHPGauge;
}



//------------------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------------------
void PokeInfoDraw::Init( void )
{
  app::ui::UIView* pUIView = m_pBase->GetUIView();

  // ペインのセット
  {
    m_pPlateParts = GetPlateParts( m_pBase, m_cCursorPos );
    m_pHPGaugeParts = m_pBase->GetParts( PANENAME_POKELIST_PLT_LOW_000_PANE_PARTS_HP, m_pPlateParts );
    m_pNickNamePane = m_pBase->GetTextBox( PANENAME_POKELIST_PLT_LOW_000_PANE_TEXTBOX_00, m_pPlateParts );
    m_pGenderParts = m_pBase->GetParts( PANENAME_POKELIST_PLT_LOW_000_PANE_PARTS_GENDER, m_pPlateParts );
    m_pGenderIcon = m_pBase->GetPane( PANENAME_COMMON_ICN_001_PANE_ICON_GENDER, m_pGenderParts );
    m_pLVText = m_pBase->GetTextBox( PANENAME_POKELIST_PLT_LOW_000_PANE_TEXTBOX_01, m_pPlateParts );
    m_pLVValueText = m_pBase->GetTextBox( PANENAME_POKELIST_PLT_LOW_000_PANE_TEXTBOX_02, m_pPlateParts );
    m_pSickParts = m_pBase->GetParts( PANENAME_POKELIST_PLT_LOW_000_PANE_PARTS_SICK, m_pPlateParts );
    m_pSickIcon = m_pBase->GetPicture( PANENAME_COMMON_ICN_002_PANE_ICON_SICK, m_pSickParts );
    m_pPokeIconParts = m_pBase->GetParts( PANENAME_POKELIST_PLT_LOW_000_PANE_PARTS_POKEICON, m_pPlateParts );
    m_pItemIconPane = m_pBase->GetPane( PANENAME_COMMON_BTN_LOW_001_PANE_ICON_ITEM, m_pPokeIconParts );
  }

  // 初期テキストの設定
  {
    // [Lv.]
    pUIView->SetTextboxPaneMessage( m_pLVText, mes_pokelist_01_02 );
  }

  // HPゲージの生成
  {
    gfl2::heap::HeapBase* pHeap = m_pBase->GetDeviceHeap();
    {
      app::tool::AppToolHPGaugePartsLayout::INIT_PARAM param;
      param.p_g2d_util              = m_pBase->GetG2D();
      param.layout_id               = m_pBase->GetLytID();
      param.number_message_id       = mes_pokelist_01_03;
      param.number_keta             = 3;
      param.gauge_hp_anime_idx      = c_HpGaugeAnimTbl[m_cCursorPos][0];
      param.color_normal_anime_idx  = c_HpGaugeAnimTbl[m_cCursorPos][1];
      param.color_half_anime_idx    = c_HpGaugeAnimTbl[m_cCursorPos][2];
      param.color_few_anime_idx     = c_HpGaugeAnimTbl[m_cCursorPos][3];
      param.p_left_number_text_box_pane   = m_pBase->GetTextBox( PANENAME_COMMON_GAG_000_PANE_TEXTBOX_00, m_pHPGaugeParts );
      param.p_center_number_text_box_pane = m_pBase->GetTextBox( PANENAME_COMMON_GAG_000_PANE_TEXTBOX_01, m_pHPGaugeParts );
      param.p_right_number_text_box_pane  = m_pBase->GetTextBox( PANENAME_COMMON_GAG_000_PANE_TEXTBOX_02, m_pHPGaugeParts );

      m_pHPGauge = GFL_NEW(pHeap)
        app::tool::AppToolHPGaugePartsLayout( param, pHeap );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void PokeInfoDraw::Update( void )
{
  // HPゲージ
  if( m_pHPGauge ) m_pHPGauge->Update();

  switch( m_IconSeq )
  {
  case ICON_SEQ_START:
    {
      m_IconSeq++;
    } break;

  case ICON_SEQ_REPLACE:
    {
      app::tool::PokeIconToolEx* pPokeIcon = m_pBase->GetPokeIcon();

      if( pPokeIcon )
      {
        // ポケモンアイコンの変更
        pPokeIcon->SetPokeIcon(
          m_cCursorPos,
          m_SimpleParam,
          PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON,
          m_pPokeIconParts );
      }

      m_IconSeq++;
    } break;

  case ICON_SEQ_END:
    {
      m_pBase->SetVisiblePane( m_pPokeIconParts, true );

      m_IconSeq = ICON_SEQ_IDLE;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   表示を更新
 * @param   pPokeParam    ポケモンデータ
 */
//------------------------------------------------------------------------------
void PokeInfoDraw::SetupPokeDisplay( const pml::pokepara::PokemonParam* pPokeParam )
{
  bool fastmode_flag = pPokeParam->StartFastMode();
  {
    if( pPokeParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
    {
      // パラメータ
      SetParamNickName( pPokeParam );
      SetParamLevel( pPokeParam );

      // アイテム
      SetItemIconByPoke( pPokeParam, true );

      // ポケモンアイコン
      SetPokeIcon( pPokeParam );
    }
    else {
      // パラメータ
      SetParamNickName( pPokeParam );
      SetParamSex( pPokeParam );
      SetParamLevel( pPokeParam );
      SetParamHp( pPokeParam );

      // アイテム
      SetItemIconByPoke( pPokeParam, false );

      // ポケモンアイコン
      SetPokeIcon( pPokeParam );
    }
  }
  pPokeParam->EndFastMode( fastmode_flag );
}

//------------------------------------------------------------------------------
/*
 * @brief   プレートモードごとの表示
 * @param   plate_mode    プレートモード
 */
//------------------------------------------------------------------------------
void PokeInfoDraw::SetupPlateMode( PlateMode plate_mode )
{
  void ( PokeInfoDraw::*POKEINFO_MODE_CHANGE[ PLATE_MODE_NUM ] )( const pml::pokepara::PokemonParam* ) = {
    &PokeInfoDraw::SetModePlate_PARAM,
    &PokeInfoDraw::SetModePlate_ITEM,
    &PokeInfoDraw::SetModePlate_JOIN,
    &PokeInfoDraw::SetModePlate_WAZAOSHIE,
    &PokeInfoDraw::SetModePlate_PARAM,
  };

  const StaticWork* pStaticWork = m_pBase->GetStaticWork();
  const pml::pokepara::PokemonParam* pPokeParam = pStaticWork->GetPokeConst( m_cCursorPos );

  ( this->*POKEINFO_MODE_CHANGE[ plate_mode ] )( pPokeParam );
}



//------------------------------------------------------------------------------
/**
 * @brief   ニックネーム設定
 * @param   pPokeParam    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void PokeInfoDraw::SetParamNickName( const pml::pokepara::PokemonParam* pPokeParam )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    print::WordSet* wordSet = m_pBase->GetWordSet();
    {
      wordSet->RegisterPokeNickName( 0, pPokeParam );
    }
    view->SetTextboxPaneMessage( m_pNickNamePane, mes_pokelist_poke_name );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   性別設定
 * @param   pPokeParam    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void PokeInfoDraw::SetParamSex( const pml::pokepara::PokemonParam* pPokeParam )
{
  const u32* pAnimTable = c_SexIconAnimTbl[m_cCursorPos];

  // アニメーションを全て停止させる
  u8 anmCount = GFL_NELEMS( c_SexIconAnimTbl[m_cCursorPos] );
  for( u8 i=0; i<anmCount; ++i )
  {
    m_pBase->StopAnim( pAnimTable[i] );
  }

  pml::Sex sex = PokeTool::GetDrawSex( pPokeParam );

  // 性別不明の場合は表示しない
  if( sex == pml::SEX_UNKNOWN )
  {
    m_pBase->SetVisiblePane( m_pGenderIcon, false );
  }
  else {
    m_pBase->SetVisiblePane( m_pGenderIcon, true );

    u32 anmID = pAnimTable[sex];
    m_pBase->StartAnim( anmID );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   レベル設定
 * @param   pPokeParam    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void PokeInfoDraw::SetParamLevel( const pml::pokepara::PokemonParam* pPokeParam )
{
  app::ui::UIView* pUIView = m_pBase->GetUIView();
  StaticWork* sw = m_pBase->GetStaticWork();
  app::tool::AppCommonGrpIconData* cmnIcon = sw->GetCommonIcon();

  m_bLvVisible = true;
  bool moudoku = sw->IsMoudoku( m_cCursorPos ); //!< 猛毒チェック

  bool isEgg = pPokeParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
  bool die = (pPokeParam->GetHp() == 0);
  pml::pokepara::Sick sick = pPokeParam->GetSick();

  if( isEgg )
  {
    m_pBase->SetVisiblePane( m_pLVText, false );        //!< 「Lv.」
    m_pBase->SetVisiblePane( m_pLVValueText, false );   //!< レベル数値
    m_pBase->SetVisiblePane( m_pSickParts, false );     //!< 状態異常アイコン

    return;
  }
  else if( die )
  {
    if( cmnIcon )
    {
      cmnIcon->ReplacePaneTextureByPokeHinshiIcon( m_pSickIcon );
    }

    m_bLvVisible = false;
  }
  else {
    // 状態異常時はレベルは表示せず状態異常を表示
    if( sick != pml::pokepara::SICK_NULL )
    {
      if( cmnIcon )
      {
        if( !moudoku )
        {
          cmnIcon->ReplacePaneTextureByPokeSickIcon( sick, m_pSickIcon );
        }
        else {
          cmnIcon->ReplacePaneTextureByPokeDokudokuIcon( m_pSickIcon );
        }
      }
      m_bLvVisible = false;
    }
    // 瀕死でも状態異常でもなければレベルを表示
    else {
      // レベル数値
      pUIView->SetTextboxPaneNumber( m_pLVValueText, mes_pokelist_01_03, pPokeParam->GetLevel(), 3 );
    }
  }

  // ペインの表示切替
  m_pBase->SetVisiblePane( m_pLVText, m_bLvVisible );         //!< 「Lv.」
  m_pBase->SetVisiblePane( m_pLVValueText, m_bLvVisible );    //!< レベル数値
  m_pBase->SetVisiblePane( m_pSickParts, !m_bLvVisible );     //!< 状態異常アイコン
}

//------------------------------------------------------------------------------
/**
 * @brief   HP設定
 * @param   pPokeParam    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void PokeInfoDraw::SetParamHp( const pml::pokepara::PokemonParam* pPokeParam )
{
  // HPの取得
  u16 now_hp = pPokeParam->GetHp();
  u16 max_hp = pPokeParam->GetMaxHp();

  if( m_pHPGauge )
  {
    m_pHPGauge->SetNumber( now_hp, max_hp );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   道具アイコンの表示をポケモンが道具を持っているかどうかで設定する
 * @param   pPokeParam  ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void PokeInfoDraw::SetItemIconByPoke( const pml::pokepara::PokemonParam* pPokeParam, bool isEgg )
{
  StaticWork* pStaticWork = m_pBase->GetStaticWork();

  // 道具Noの取得
  u16 item_no = pStaticWork->GetPokeItem( m_cCursorPos );

  // 表示を切り替える
  bool visible = !isEgg && (item_no != ITEM_DUMMY_DATA);
  m_pBase->SetVisiblePane( m_pItemIconPane, visible );
}




//------------------------------------------------------------------------------
/**
 * @brief   ポケモンアイコンのセット
 */
//------------------------------------------------------------------------------
void PokeInfoDraw::SetPokeIcon( const pml::pokepara::PokemonParam * pp )
{
  if( m_pPokeIconParts )
  {
    PokeTool::SimpleParam sp;
    {
      PokeTool::GetSimpleParam( &sp, pp );
    }

    // パラメータに差異がある場合はアイコンを生成しなおす
    if( !PokeTool::CompareSimpleParam( m_SimpleParam, sp ) )
    {
      m_SimpleParam = sp;

      m_pBase->SetVisiblePane( m_pPokeIconParts, false );

      m_IconSeq = ICON_SEQ_START;
    }
  }
}




//==============================================================================
/**
 *    ↓ 以下はモードごとの表示切替関数 ↓
 */
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   各モードごとの表示に切り替える(パラメータ表示)
 * @param   pPokeParam    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void PokeInfoDraw::SetModePlate_PARAM( const pml::pokepara::PokemonParam* pPokeParam )
{
  if( pPokeParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
  {
    m_pBase->SetVisiblePane( m_pGenderParts, false );
    m_pBase->SetVisiblePane( m_pHPGaugeParts, false );
    m_pBase->SetVisiblePane( m_pLVText, false );
    m_pBase->SetVisiblePane( m_pLVValueText, false );
  }
  else {
    m_pBase->SetVisiblePane( m_pGenderParts, true );
    m_pBase->SetVisiblePane( m_pHPGaugeParts, true );
    m_pBase->SetVisiblePane( m_pLVText, m_bLvVisible );
    m_pBase->SetVisiblePane( m_pLVValueText, m_bLvVisible );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   各モードごとの表示に切り替える(道具表示)
 * @param   pPokeParam    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void PokeInfoDraw::SetModePlate_ITEM( const pml::pokepara::PokemonParam* pPokeParam )
{
  if( pPokeParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
  {
    m_pBase->SetVisiblePane( m_pGenderParts, false );
    m_pBase->SetVisiblePane( m_pHPGaugeParts, false );
    m_pBase->SetVisiblePane( m_pLVText, false );
    m_pBase->SetVisiblePane( m_pLVValueText, false );
  }
  else {
    m_pBase->SetVisiblePane( m_pGenderParts, true );
    m_pBase->SetVisiblePane( m_pHPGaugeParts, false );
    m_pBase->SetVisiblePane( m_pLVText, false );
    m_pBase->SetVisiblePane( m_pLVValueText, false );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   各モードごとの表示に切り替える(参加選択表示)
 * @param   pPokeParam    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void PokeInfoDraw::SetModePlate_JOIN( const pml::pokepara::PokemonParam* pPokeParam )
{
  if( pPokeParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
  {
    m_pBase->SetVisiblePane( m_pGenderParts, false );
    m_pBase->SetVisiblePane( m_pHPGaugeParts, false );
    m_pBase->SetVisiblePane( m_pLVText, false );
    m_pBase->SetVisiblePane( m_pLVValueText, false );
  }
  else {
    m_pBase->SetVisiblePane( m_pGenderParts, true );
    m_pBase->SetVisiblePane( m_pHPGaugeParts, true );
    m_pBase->SetVisiblePane( m_pLVText, m_bLvVisible );
    m_pBase->SetVisiblePane( m_pLVValueText, m_bLvVisible );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   各モードごとの表示に切り替える(技教え表示)
 * @param   pPokeParam    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void PokeInfoDraw::SetModePlate_WAZAOSHIE( const pml::pokepara::PokemonParam* pPokeParam )
{
  if( pPokeParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
  {
    m_pBase->SetVisiblePane( m_pGenderParts, false );
    m_pBase->SetVisiblePane( m_pHPGaugeParts, false );
    m_pBase->SetVisiblePane( m_pLVText, false );
    m_pBase->SetVisiblePane( m_pLVValueText, false );
  }
  else {
    m_pBase->SetVisiblePane( m_pGenderParts, true );
    m_pBase->SetVisiblePane( m_pHPGaugeParts, false );
    m_pBase->SetVisiblePane( m_pLVText, false );
    m_pBase->SetVisiblePane( m_pLVValueText, false );
  }
}



GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )
