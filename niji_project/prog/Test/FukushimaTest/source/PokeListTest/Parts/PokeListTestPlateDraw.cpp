#if PM_DEBUG

//==============================================================================
/**
 * @file    PokeListTestPlateDraw.h.cpp
 * @author  fukushima_yuya
 * @date    2015.06.25
 * @brief   ポケモンリスト用プレート内のステータス表示
 */
//==============================================================================

// niji
#include "Test/FukushimaTest/source/PokeListTest/Parts/PokeListTestPlateDraw.h"

#include <App/PokeList/source/parts/PokeListDrawerBase.h>
#include <App/PokeList/source/work/PokeListStaticWork.h>
#include <AppLib/include/Tool/AppToolGaugePartsLayout.h>
#include <AppLib/include/Tool/app_tool_Gauge.h>
#include <AppLib/include/ui/UIView.h>
#include <PokeTool/include/PokeTool.h>

// pml
#include <pml/include/pmlib.h>

// arc
#include <niji_conv_header/app/common_pokeList/common_pokeList_anim_list.h>
#include <niji_conv_header/app/common_pokeList/common_pokeList_pane.h>
#include <niji_conv_header/message/msg_pokelist.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//==============================================================================
// @brief   プレートのステータス表示クラス
//==============================================================================

  //----------------------------------------------------------------------------
  // @brief   アニメインデックス配列
  //----------------------------------------------------------------------------
  //! @brief  HPゲージアニメ
  const u32 c_HpGaugeAnimTbl[App::PokeList::PLATE_NUM][4] = {
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

  //! @brief  性別アニメ
  const u32 c_SexIconAnimTbl[App::PokeList::PLATE_NUM][2] = {
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


  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //----------------------------------------------------------------------------
  TestPlateStatus::TestPlateStatus( App::PokeList::DrawerBase* pBase, App::PokeList::CursorPos cpos )
    : m_pBase( pBase )
    , m_pHPGauge( NULL )
    , m_cCursorPos( cpos )
    , m_pPlateParts( NULL )
    , m_pHPGaugeParts( NULL )
    , m_pNickNamePane( NULL )
    , m_pGenderParts( NULL )
    , m_pGenderIcon( NULL )
    , m_pLVText( NULL )
    , m_pLVValueText( NULL )
    , m_pSickParts( NULL )
    , m_pPokeIconParts( NULL )
    , m_pItemIconPane( NULL )
  {
    Init();
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //----------------------------------------------------------------------------
  TestPlateStatus::~TestPlateStatus( void )
  {
    GFL_SAFE_DELETE( m_pHPGauge );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //----------------------------------------------------------------------------
  void TestPlateStatus::Init( void )
  {
    app::ui::UIView* pUIView = m_pBase->GetUIView();

    // ペインのセット
    m_pPlateParts = GetPlateParts( m_pBase, m_cCursorPos );
    m_pHPGaugeParts = m_pBase->GetParts( PANENAME_POKELIST_PLT_LOW_000_PANE_PARTS_HP, m_pPlateParts );
    m_pNickNamePane = m_pBase->GetTextBox( PANENAME_POKELIST_PLT_LOW_000_PANE_TEXTBOX_00, m_pPlateParts );
    m_pGenderParts = m_pBase->GetParts( PANENAME_POKELIST_PLT_LOW_000_PANE_PARTS_GENDER, m_pPlateParts );
    m_pGenderIcon = m_pBase->GetPane( PANENAME_COMMON_ICN_001_PANE_ICON_GENDER, m_pGenderParts );
    m_pLVText = m_pBase->GetTextBox( PANENAME_POKELIST_PLT_LOW_000_PANE_TEXTBOX_01, m_pPlateParts );
    m_pLVValueText = m_pBase->GetTextBox( PANENAME_POKELIST_PLT_LOW_000_PANE_TEXTBOX_02, m_pPlateParts );
    m_pSickParts = m_pBase->GetParts( PANENAME_POKELIST_PLT_LOW_000_PANE_PARTS_SICK, m_pPlateParts );
    m_pPokeIconParts = m_pBase->GetParts( PANENAME_POKELIST_PLT_LOW_000_PANE_PARTS_POKEICON, m_pPlateParts );
    m_pItemIconPane = m_pBase->GetPane( PANENAME_COMMON_BTN_LOW_001_PANE_ICON_ITEM, m_pPokeIconParts );

    // 初期テキストの設定
    pUIView->SetTextboxPaneMessage( m_pLVText, mes_pokelist_01_02 );    //!< [Lv.]

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

  //----------------------------------------------------------------------------
  /**
   * @brief   ニックネームのセット
   * @param   pp    ポケモンパラメータ
   */
  //----------------------------------------------------------------------------
  void TestPlateStatus::SetParamNickName( const pml::pokepara::PokemonParam* pp )
  {
    app::util::G2DUtil* g2d = m_pBase->GetG2D();
    {
      g2d->SetRegisterPokeNickName( 0, pp );
      g2d->SetTextBoxPaneStringExpand( m_pNickNamePane, mes_pokelist_poke_name );
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   性別のセット
   * @param   pp    ポケモンパラメータ
   */
  //----------------------------------------------------------------------------
  void TestPlateStatus::SetParamSex( const pml::pokepara::PokemonParam* pp )
  {
    const u32* pAnimTable = c_SexIconAnimTbl[m_cCursorPos];

    // アニメーションを全て停止させる
    u8 anmCount = GFL_NELEMS( c_SexIconAnimTbl[m_cCursorPos] );
    for( u8 i=0; i<anmCount; ++i )
    {
      m_pBase->StopAnim( pAnimTable[i] );
    }

    pml::Sex sex = PokeTool::GetDrawSex( pp );

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

  //----------------------------------------------------------------------------
  /**
   * @brief   レベルのセット
   * @param   pp    ポケモンパラメータ
   */
  //----------------------------------------------------------------------------
  void TestPlateStatus::SetParamLevel( const pml::pokepara::PokemonParam* pp )
  {
    app::ui::UIView* pUIView = m_pBase->GetUIView();
    bool lv_pane = true;
    //u32 anm_frame = 0;

    bool isEgg = pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
    bool die = (pp->GetHp() == 0);
    pml::pokepara::Sick sick = pp->GetSick();

    if( isEgg )
    {
      m_pBase->SetVisiblePane( m_pLVText, false );        //!< 「Lv.」
      m_pBase->SetVisiblePane( m_pLVValueText, false );   //!< レベル数値
      m_pBase->SetVisiblePane( m_pSickParts, false );     //!< 状態異常アイコン
    }
    else if( die )
    {
      //anm_frame = pml::pokepara::SICK_MAX;    //!< 瀕死は最後にアニメが追加されている
      lv_pane = false;
    }
    else {
      // 状態異常時はレベルは表示せず状態異常を表示
      if( sick != pml::pokepara::SICK_NULL )
      {
        //anm_frame = sick;
        lv_pane = false;
      }
      // 瀕死でも状態異常でもなければレベルを表示
      else {
        // レベル数値
        pUIView->SetTextboxPaneNumber( m_pLVValueText, mes_pokelist_01_03, pp->GetLevel(), 3 );
      }
    }

    // 非表示時は状態異常or瀕死の表示を行う
    if( lv_pane == false )
    {
  #if 0 // 状態異常はアニメーションでなくテクスチャの差し替えに変更
      pG2DUtil->StartAnime( m_cLayoutID, c_SickAnimTbl[m_cCursorPos] );
      pG2DUtil->PauseAnime( m_cLayoutID, c_SickAnimTbl[m_cCursorPos], false );
      pG2DUtil->SetAnimeFrame( m_cLayoutID, c_SickAnimTbl[m_cCursorPos], anm_frame );
  #endif
    }

    // ペインの表示切替
    m_pBase->SetVisiblePane( m_pLVText, lv_pane );        //!< 「Lv.」
    m_pBase->SetVisiblePane( m_pLVValueText, lv_pane );   //!< レベル数値
    m_pBase->SetVisiblePane( m_pSickParts, !lv_pane );     //!< 状態異常アイコン
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   HPのセット
   * @param   pp    ポケモンパラメータ
   */
  //----------------------------------------------------------------------------
  void TestPlateStatus::SetParamHp( const pml::pokepara::PokemonParam* pp )
  {
    // HPの取得
    u16 now_hp = pp->GetHp();
    u16 max_hp = pp->GetMaxHp();

    if( m_pHPGauge )
    {
      m_pHPGauge->SetNumber( now_hp, max_hp );
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   アイテムのセット
   * @param   pp    ポケモンパラメータ
   * @param   isEgg タマゴフラグ
   */
  //----------------------------------------------------------------------------
  void TestPlateStatus::SetItemIconByPoke( const pml::pokepara::PokemonParam* pp, bool isEgg )
  {
    App::PokeList::StaticWork* pStaticWork = m_pBase->GetStaticWork();

    // 道具Noの取得
    u16 item_no = pStaticWork->GetPokeItem( m_cCursorPos );

    // 表示を切り替える
    bool visible = !isEgg && (item_no != ITEM_DUMMY_DATA);
    m_pBase->SetVisiblePane( m_pItemIconPane, visible );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ポケモンアイコンのセット
   */
  //----------------------------------------------------------------------------
  void TestPlateStatus::SetPokeIcon( const pml::pokepara::PokemonParam* pp )
  {
    app::tool::PokeIconToolEx* pPokeIcon = m_pBase->GetPokeIcon();

    if( pPokeIcon )
    {
      // ポケモンアイコンの変更
      pPokeIcon->SetPokeIcon( m_cCursorPos, pp, PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON, m_pPokeIconParts );
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   通常表示への切り替え
   * @param   pp  ポケモンパラメータ
   * @param   egg タマゴかどうか
   */
  //----------------------------------------------------------------------------
  void TestPlateStatus::SetModePlate_PARAM( const pml::pokepara::PokemonParam* pp, bool egg )
  {
    if( egg )
    {
      m_pBase->SetVisiblePane( m_pGenderParts, false );
      m_pBase->SetVisiblePane( m_pHPGaugeParts, false );
      m_pBase->SetVisiblePane( m_pLVText, false );
      m_pBase->SetVisiblePane( m_pLVValueText, false );
    }
    else {
      m_pBase->SetVisiblePane( m_pGenderParts, true );
      m_pBase->SetVisiblePane( m_pHPGaugeParts, true );
      m_pBase->SetVisiblePane( m_pLVText, true );
      m_pBase->SetVisiblePane( m_pLVValueText, true );
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   道具表示への切り替え
   * @param   pp  ポケモンパラメータ
   * @param   egg タマゴかどうか
   */
  //----------------------------------------------------------------------------
  void TestPlateStatus::SetModePlate_ITEM( const pml::pokepara::PokemonParam* pp, bool egg )
  {
    if( egg )
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

  //----------------------------------------------------------------------------
  /**
   * @brief   参加選択表示への切り替え
   * @param   pp  ポケモンパラメータ
   * @param   egg タマゴかどうか
   */
  //----------------------------------------------------------------------------
  void TestPlateStatus::SetModePlate_JOIN( const pml::pokepara::PokemonParam* pp, bool egg )
  {
    if( egg )
    {
      m_pBase->SetVisiblePane( m_pGenderParts, false );
      m_pBase->SetVisiblePane( m_pHPGaugeParts, false );
      m_pBase->SetVisiblePane( m_pLVText, false );
      m_pBase->SetVisiblePane( m_pLVValueText, false );
    }
    else {
      m_pBase->SetVisiblePane( m_pGenderParts, true );
      m_pBase->SetVisiblePane( m_pHPGaugeParts, true );
      m_pBase->SetVisiblePane( m_pLVText, true );
      m_pBase->SetVisiblePane( m_pLVValueText, true );
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   技教え表示への切り替え
   * @param   pp  ポケモンパラメータ
   * @param   egg タマゴかどうか
   */
  //----------------------------------------------------------------------------
  void TestPlateStatus::SetModePlate_WAZAOSHIE( const pml::pokepara::PokemonParam* pp, bool egg )
  {
    if( egg )
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



//==============================================================================
// @brief   プレートクラス
//==============================================================================

  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //----------------------------------------------------------------------------
  TestPokePlate::TestPokePlate( App::PokeList::DrawerBase* pBase, App::PokeList::CursorPos cpos )
    : TestPlateStatus( pBase, cpos )
  {
    ;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //----------------------------------------------------------------------------
  TestPokePlate::~TestPokePlate( void )
  {

  }



  //----------------------------------------------------------------------------
  /**
   * @brief   セットアップ
   */
  //----------------------------------------------------------------------------
  void TestPokePlate::Setup( App::PokeList::PlateMode plateMode )
  {

  }

  //----------------------------------------------------------------------------
  /**
   * @brief   更新処理
   */
  //----------------------------------------------------------------------------
  void TestPokePlate::Update( void )
  {
    if( m_pHPGauge ) m_pHPGauge->Update();
  }


GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG