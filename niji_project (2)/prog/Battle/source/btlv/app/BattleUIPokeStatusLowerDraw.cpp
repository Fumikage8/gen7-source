//=============================================================================
/**
 * @file BattleUIPokeStatusLowerDraw.cpp
 * @brief バトル下画面 >> 「ポケモンステータス」画面
 * @author yuto_uchida
 * @date 2015.03.19
 */
//=============================================================================
#include "BattleUIPokeStatusLowerDraw.h"
#include "BattleUIViewDrawOrder.h"

#include "niji_conv_header/app/battle/res2d/stalow.h"
#include "niji_conv_header/app/battle/res2d/stalow_pane.h"
#include "niji_conv_header/app/battle/res2d/stalow_anim_list.h"
#include "niji_conv_header/message/msg_btl_app.h"

#include "AppLib/include/Util/app_util_Common.h"
#include "AppLib/include/Tool/AppCommonGrpIconData.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  //  ペインリストデータテーブル
  const BattleUIPokeStatusLowerDrawBase::StatusItemObject::INIT_PARAM   BattleUIPokeStatusLowerDrawBase::StatusList::m_aListItemInitParamTable[ BattleUIPokeStatusLowerDrawBase::StatusList::STATUS_LIST_NUM ] =
  {
    //  1行目
    {
      PANENAME_BATTLE_STA_LOW_000_PANE_NULLLIST_00,
      PANENAME_BATTLE_STA_LOW_000_PANE_TEXDESC_00,
      PANENAME_BATTLE_STA_LOW_000_PANE_TEXNUM_00,
      PANENAME_BATTLE_STA_LOW_000_PANE_ICONINFO_00,
      PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,

      //  インフォボタンのアニメid一覧
      {
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_00_TOUCH,
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_00_RELEASE,
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_00_CANSEL,
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_00_TOUCH_RELEASE,
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_00_ACTIVE,
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_00_PASSIVE,
      }
    },

    //  2行目
    {
      PANENAME_BATTLE_STA_LOW_000_PANE_NULLLIST_01,
      PANENAME_BATTLE_STA_LOW_000_PANE_TEXDESC_01,
      PANENAME_BATTLE_STA_LOW_000_PANE_TEXNUM_01,
      PANENAME_BATTLE_STA_LOW_000_PANE_ICONINFO_01,
      PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,

      //  インフォボタンのアニメid一覧
      {
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_01_TOUCH,
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_01_RELEASE,
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_01_CANSEL,
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_01_TOUCH_RELEASE,
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_01_ACTIVE,
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_01_PASSIVE,
      }
    },

    //  3行目
    {
      PANENAME_BATTLE_STA_LOW_000_PANE_NULLLIST_02,
      PANENAME_BATTLE_STA_LOW_000_PANE_TEXDESC_02,
      PANENAME_BATTLE_STA_LOW_000_PANE_TEXNUM_02,
      PANENAME_BATTLE_STA_LOW_000_PANE_ICONINFO_02,
      PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,

      //  インフォボタンのアニメid一覧
      {
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_02_TOUCH,
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_02_RELEASE,
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_02_CANSEL,
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_02_TOUCH_RELEASE,
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_02_ACTIVE,
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_02_PASSIVE,
      }
    },

    //  4行目
    {
      PANENAME_BATTLE_STA_LOW_000_PANE_NULLLIST_03,
      PANENAME_BATTLE_STA_LOW_000_PANE_TEXDESC_03,
      PANENAME_BATTLE_STA_LOW_000_PANE_TEXNUM_03,
      PANENAME_BATTLE_STA_LOW_000_PANE_ICONINFO_03,
      PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,

      //  インフォボタンのアニメid一覧
      {
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_03_TOUCH,
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_03_RELEASE,
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_03_CANSEL,
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_03_TOUCH_RELEASE,
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_03_ACTIVE,
        LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_03_PASSIVE,
      }
    }
  };

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUIPokeStatusLowerDrawBase::BattleUIPokeStatusLowerDrawBase( const INIT_PARAM& param ) :
    UIView( param.pUtilHeap, param.pUtilHeap->GetDeviceAllocator(), param.pUtilHeap->GetDeviceAllocator() )
  {
    GFL_ASSERT( param.pUtilHeap );
    GFL_ASSERT( param.pRenderingManager );
    GFL_ASSERT( param.p_lyt_buff );

    _Clear();

    //  初期化
    {
      m_state     = STATE_TYPE_NONE;
      m_initParam = param;
    }
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUIPokeStatusLowerDrawBase::~BattleUIPokeStatusLowerDrawBase()
  {
    GFL_SAFE_DELETE( m_pPlateObject );
    GFL_SAFE_DELETE_ARRAY( m_pParamitemObjectArray );

    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //------------------------------------------------------------------
  /**
    * @brief 初期化処理
    * @note
    *  クラスを実行するまえに呼ばないと動作保障はしない
    */
  //------------------------------------------------------------------
  void BattleUIPokeStatusLowerDrawBase::Initialize()
  {
    _InitializeBase( true );
  }

  void BattleUIPokeStatusLowerDrawBase::_InitializeBase( const bool bSelfParam )
  {
    //  レイアウト初期化
    _Initialize2D( m_initParam.p_lyt_buff );

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    gfl2::lyt::LytWk* pLayoutWork = UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN );
    GFL_ASSERT( pLayoutWork );

    gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

    //  ステータスリスト設定
    {
      m_statusList.Initialize( this, LYT_WK_TYPE_SCREEN, 0, &m_initParam );
    }

    //  ボタン関連の設定
    {
      gfl2::lyt::LytParts*  pBtnBackPartsPane = pLayoutWork->GetPartsPane( PANENAME_BATTLE_STA_LOW_000_PANE_BTN_BACK );
      GFL_ASSERT( pBtnBackPartsPane );

      gfl2::lyt::LytParts*  pBtnInfoTokuseiPartsPane  = pLayoutWork->GetPartsPane( PANENAME_BATTLE_STA_LOW_000_PANE_ICONINFO_04 );
      GFL_ASSERT( pBtnInfoTokuseiPartsPane );

      gfl2::lyt::LytParts*  pBtnInfoItemPartsPane = pLayoutWork->GetPartsPane( PANENAME_BATTLE_STA_LOW_000_PANE_ICONINFO_05 );
      GFL_ASSERT( pBtnInfoItemPartsPane );

      const ::app::ui::ButtonInfoEx aButtonPaneIdxList[ BUTTON_ID_MAX ] =
      {
        /*
          app::tool::ButtonManager::ButtonId button_id;
          gfl2::lyt::LytPane* picture_pane;
          gfl2::lyt::LytPane* bound_pane;
          u32                 touch_anime_index;        //!< タッチ時のアニメ
          u32                 release_anime_index;      //!< タッチ決定時のアニメ
          u32                 cancel_anime_index;       //!< タッチ状態からのキャンセルアニメ
          u32                 key_select_anime_index;   //!< キー決定時のアニメ ( touch => release の連続アニメ )
          u32                 active_anime_index;       //!< アクティブアニメ
          u32                 passive_anime_index;      //!< パッシブアニメ
        */

        //  キャンセルボタン
        {
          BUTTON_ID_CANCEL,
          pBtnBackPartsPane,
          pLayoutWork->GetBoundingPane( pBtnBackPartsPane, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &rMultiResId ),
          LA_STALOW_BATTLE_STA_LOW_000__BTN_BACK_TOUCH,
          LA_STALOW_BATTLE_STA_LOW_000__BTN_BACK_RELEASE,
          LA_STALOW_BATTLE_STA_LOW_000__BTN_BACK_CANSEL,
          LA_STALOW_BATTLE_STA_LOW_000__BTN_BACK_TOUCH_RELEASE,
          ::app::tool::ButtonManager::ANIMATION_NULL,
          ::app::tool::ButtonManager::ANIMATION_NULL,
        },

        //  特性のインフォボタン
        {
          BUTTON_ID_INFO_TOKUSEI,
          pBtnInfoTokuseiPartsPane,
          pLayoutWork->GetBoundingPane( pBtnInfoTokuseiPartsPane, PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00, &rMultiResId ),
          LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_04_TOUCH,
          LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_04_RELEASE,
          LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_04_CANSEL,
          LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_04_TOUCH_RELEASE,
          LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_04_ACTIVE,
          LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_04_PASSIVE,
        },

        //  アイテムのインフォボタン
        {
          BUTTON_ID_INFO_ITEM,
          pBtnInfoItemPartsPane,
          pLayoutWork->GetBoundingPane( pBtnInfoItemPartsPane, PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00, &rMultiResId ),
          LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_05_TOUCH,
          LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_05_RELEASE,
          LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_05_CANSEL,
          LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_05_TOUCH_RELEASE,
          LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_05_ACTIVE,
          LA_STALOW_BATTLE_STA_LOW_000__ICONINFO_05_PASSIVE,
        },

        //  状態リストの左右矢印ボタン
        m_statusList.CreateButtonUpArrow( BUTTON_ID_STATUS_LIST_LEFT_ARROW ),
        m_statusList.CreateButtonDownArrow( BUTTON_ID_STATUS_LIST_RIGHT_ARROW ),

        //  ステータスリストの各項目のインフォボタン
        m_statusList.CreateButtonListItem( 0, BUTTON_ID_STATIS_LIST_ITEM_INFO_0 ),
        m_statusList.CreateButtonListItem( 1, BUTTON_ID_STATIS_LIST_ITEM_INFO_1 ),
        m_statusList.CreateButtonListItem( 2, BUTTON_ID_STATIS_LIST_ITEM_INFO_2 ),
        m_statusList.CreateButtonListItem( 3, BUTTON_ID_STATIS_LIST_ITEM_INFO_3 ),
      };

      UIResponder::CreateButtonManager( m_initParam.pUtilHeap, pG2DUtil->GetLayoutWork( LYT_WK_TYPE_SCREEN ), aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );

      //  特性とアイテムのインフォボタンは継承した個別クラス側で使うのであれば有効にする
      //  味方は表示して、敵では表示しないから
      {
        UIResponder::SetButtonEnable( BUTTON_ID_INFO_TOKUSEI, false, false );
        UIResponder::SetButtonEnable( BUTTON_ID_INFO_ITEM, false, false );
      }

      //  入力コールバック設定
      UIView::SetInputListener( this );
    }

    //  ボタンSE設定
    if( m_initParam.bUseInputSE )
    {
      const s32 aButtonSeDataTable[ BUTTON_ID_MAX ]  =
      {
        SEQ_SE_CANCEL1,
        ::app::tool::ButtonManager::SE_NULL,
        ::app::tool::ButtonManager::SE_NULL,
        SEQ_SE_DECIDE1,
        SEQ_SE_DECIDE1,
        ::app::tool::ButtonManager::SE_NULL,
        ::app::tool::ButtonManager::SE_NULL,
        ::app::tool::ButtonManager::SE_NULL,
        ::app::tool::ButtonManager::SE_NULL
      };

      ::app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();
      for( u32 i = 0; i < BUTTON_ID_MAX; ++i )
      {
        pButtonManager->SetButtonSelectSE( i, aButtonSeDataTable[ i ] );
      }
    }

    //  パラメータ項目設定
    {
      const ParamItemRankObject::INIT_PARAM aInitParam[]  =
      {
        //  こうげき
        { 
          PANENAME_BATTLE_STA_LOW_000_PANE_ITEMNAME_00,
          STR_ATK,

          PANENAME_BATTLE_STA_LOW_000_PANE_PALAM_00,
          m_initParam.pPokeParam->GetValue( BTL_POKEPARAM::BPP_ATTACK_RANK ),
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_00_ICON_UP,
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_00_ICON_DOWN,
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_00_LOOP_UP,
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_00_LOOP_DOWN
        },

        //  ぼうぎょ
        {
          PANENAME_BATTLE_STA_LOW_000_PANE_ITEMNAME_01,
          STR_DEF,
          
          PANENAME_BATTLE_STA_LOW_000_PANE_PALAM_01,
          m_initParam.pPokeParam->GetValue( BTL_POKEPARAM::BPP_DEFENCE_RANK ),
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_01_ICON_UP,
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_01_ICON_DOWN,
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_01_LOOP_UP,
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_01_LOOP_DOWN
        },

        //  とくこう
        {
          PANENAME_BATTLE_STA_LOW_000_PANE_ITEMNAME_02,
          STR_SPATK,
          
          PANENAME_BATTLE_STA_LOW_000_PANE_PALAM_02,
          m_initParam.pPokeParam->GetValue( BTL_POKEPARAM::BPP_SP_ATTACK_RANK ),
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_02_ICON_UP,
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_02_ICON_DOWN,
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_02_LOOP_UP,
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_02_LOOP_DOWN,
        },

        //  とくぼう
        {
          PANENAME_BATTLE_STA_LOW_000_PANE_ITEMNAME_03,
          STR_SPDEF,
          
          PANENAME_BATTLE_STA_LOW_000_PANE_PALAM_03,
          m_initParam.pPokeParam->GetValue( BTL_POKEPARAM::BPP_SP_DEFENCE_RANK ),
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_03_ICON_UP,
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_03_ICON_DOWN,
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_03_LOOP_UP,
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_03_LOOP_DOWN
        },

        //  すばやさ
        {
          PANENAME_BATTLE_STA_LOW_000_PANE_ITEMNAME_04,
          STR_AGI,
          
          PANENAME_BATTLE_STA_LOW_000_PANE_PALAM_04,
          m_initParam.pPokeParam->GetValue( BTL_POKEPARAM::BPP_AGILITY_RANK ),
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_04_ICON_UP,
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_04_ICON_DOWN,
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_04_LOOP_UP,
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_04_LOOP_DOWN
        },

        //  めいちゅう
        {
          PANENAME_BATTLE_STA_LOW_000_PANE_ITEMNAME_05,
          STR_WAZA_HIT,
          
          PANENAME_BATTLE_STA_LOW_000_PANE_PALAM_05,
          m_initParam.pPokeParam->GetValue( BTL_POKEPARAM::BPP_HIT_RATIO ),
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_05_ICON_UP,
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_05_ICON_DOWN,
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_05_LOOP_UP,
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_05_LOOP_DOWN
        },

        //  かいひ
        {
          PANENAME_BATTLE_STA_LOW_000_PANE_ITEMNAME_06,
          STR_WAZA_AVOID,
          
          PANENAME_BATTLE_STA_LOW_000_PANE_PALAM_06,
          m_initParam.pPokeParam->GetValue( BTL_POKEPARAM::BPP_AVOID_RATIO ),
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_06_ICON_UP,
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_06_ICON_DOWN,
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_06_LOOP_UP,
          LA_STALOW_BATTLE_STA_LOW_000__PALAM_06_LOOP_DOWN
        },
      };

      m_pParamitemObjectArray = GFL_NEW_ARRAY( m_initParam.pUtilHeap->GetDeviceHeap() ) ParamItemRankObject[ GFL_NELEMS( aInitParam ) ];
      for( s32 i = 0; i < GFL_NELEMS( aInitParam ); ++i )
      {
        m_pParamitemObjectArray[ i ].Initialize( this, LYT_WK_TYPE_SCREEN, 0, aInitParam[ i ] );
      }
    }

    //  ポケモンステータスプレート作成
    {
      gfl2::lyt::LytParts*  pPlatePartPane  = pLayoutWork->GetPartsPane( PANENAME_BATTLE_STA_LOW_000_PANE_HP_PLAYER );
      GFL_ASSERT( pPlatePartPane );

      m_pPlateObject  = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) StatusPlatePartsObject();
      GFL_ASSERT( m_pPlateObject );

      StatusPlatePartsObject::INIT_PARAM        initParam;
      {
        initParam.pUtilHeap           = m_initParam.pUtilHeap;
        initParam.pPokeParam          = m_initParam.pPokeParam;
        initParam.pUiView             = this;
        initParam.layoutWorkId        = LYT_WK_TYPE_SCREEN;
        initParam.bRegsterZukan       = m_initParam.bRegsterZukan;
        initParam.bShowTypeIcon       = m_initParam.bShowTypeIcon;
        initParam.bPokeIconFlip       = m_bPokeIconFlip;
        initParam.bSelfParam          = bSelfParam;

        GFL_ASSERT( sizeof( m_initParam.aPokeType ) == sizeof( initParam.aPokeType ) );
        gfl2::std::MemCopy( m_initParam.aPokeType, initParam.aPokeType, sizeof( m_initParam.aPokeType ) );
      }

      gfl2::lyt::LytParts*  pPartsPaneHpGauge = pLayoutWork->GetPartsPane( pPlatePartPane, PANENAME_BATTLE_PLT_UPP_098_PANE_GAUGE_00 , &rMultiResId );
      GFL_ASSERT( pPartsPaneHpGauge );
      
      gfl2::lyt::LytParts*  pPartsPaneExpGauge = pLayoutWork->GetPartsPane( pPlatePartPane, PANENAME_BATTLE_PLT_UPP_098_PANE_GAUGE_01 , &rMultiResId );
      GFL_ASSERT( pPartsPaneExpGauge );
      
      StatusPlatePartsObject::PANE_CONFIG_PARAM paneConfigParam =
      {
        //  Battle_PLT_UPP_098.flyt専用のデータ設定
        {
          LA_STALOW_BATTLE_STA_LOW_000__HP_PLAYER_POS_OWN,
          LA_STALOW_BATTLE_STA_LOW_000__HP_PLAYER_POS_OTHERS,
          LA_STALOW_BATTLE_STA_LOW_000__HP_PLAYER_KEEP_SICK,

          pLayoutWork->GetTextBoxPane( pPlatePartPane,PANENAME_BATTLE_PLT_UPP_098_PANE_TEXTBOX_00, &rMultiResId ),
          pLayoutWork->GetTextBoxPane( pPlatePartPane,PANENAME_BATTLE_PLT_UPP_098_PANE_TEXTBOX_01, &rMultiResId ),
          pLayoutWork->GetTextBoxPane( pPlatePartPane,PANENAME_BATTLE_PLT_UPP_098_PANE_TEXTBOX_02, &rMultiResId ),
          pLayoutWork->GetPane( pPlatePartPane,PANENAME_BATTLE_PLT_UPP_098_PANE_PARTS_BALL, &rMultiResId ),
          pLayoutWork->GetPartsPane( pPlatePartPane, PANENAME_BATTLE_PLT_UPP_098_PANE_PARTS_GENDER, &rMultiResId ),
          pLayoutWork->GetPicturePane( pLayoutWork->GetPartsPane( pPlatePartPane, PANENAME_BATTLE_PLT_UPP_098_PANE_PARTS_SICK, &rMultiResId ), PANENAME_COMMON_ICN_002_PANE_ICON_SICK, &rMultiResId ),
        },

        //  Common_ICN_001.flyt専用のデータ(性別アイコン)
        {
          LA_STALOW_BATTLE_STA_LOW_000__HP_PLAYER__PARTS_GENDER_ICON_MALE,
          LA_STALOW_BATTLE_STA_LOW_000__HP_PLAYER__PARTS_GENDER_ICON_FEMALE,
        },

        //  Common_ICN_002.flyt専用のデータ(状態異常アイコン)
        {
          LA_STALOW_BATTLE_STA_LOW_000__HP_PLAYER__PARTS_SICK_ACTIVE,
          LA_STALOW_BATTLE_STA_LOW_000__HP_PLAYER__PARTS_SICK_PASSIVE,
        },

        //  Common_ICN_003.flyt専用のデータ(メガ進化アイコン)
        {
          LA_STALOW_BATTLE_STA_LOW_000__HP_PLAYER__PARTS_MEGA_ICONPATERN_00,
          LA_STALOW_BATTLE_STA_LOW_000__HP_PLAYER__PARTS_MEGA_ICONPATERN_01,
          LA_STALOW_BATTLE_STA_LOW_000__HP_PLAYER__PARTS_MEGA_ICONPATERN_02,
          LA_STALOW_BATTLE_STA_LOW_000__HP_PLAYER__PARTS_MEGA_ACTIVE,
          LA_STALOW_BATTLE_STA_LOW_000__HP_PLAYER__PARTS_MEGA_PASSIVE,
        },

        //  Common_BTN_LOW_001.flyt専用のデータ（ポケモンアイコン）
        {
          LA_STALOW_BATTLE_STA_LOW_000__PARTS_POKEICON_DIE,

          PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON,
          PANENAME_BATTLE_STA_LOW_000_PANE_PARTS_POKEICON,
        },

        //  ::app::tool::AppToolHPGaugePartsLayout::INIT_PARAM(HPゲージ)
        {
          pG2DUtil,
          LYT_WK_TYPE_SCREEN,

          //  数字の表示用のメッセージID
          STR_VALUE_3,
          //  数字の桁数
          3,

          pLayoutWork->GetTextBoxPane( pPartsPaneHpGauge, PANENAME_BATTLE_GAG_003_PANE_TEXTBOX_00, &rMultiResId ),
          pLayoutWork->GetTextBoxPane( pPartsPaneHpGauge, PANENAME_BATTLE_GAG_003_PANE_TEXTBOX_01, &rMultiResId ),
          pLayoutWork->GetTextBoxPane( pPartsPaneHpGauge, PANENAME_BATTLE_GAG_003_PANE_TEXTBOX_02, &rMultiResId ),

          LA_STALOW_BATTLE_STA_LOW_000__HP_PLAYER__GAUGE_00__BATTLE_HPGAUGE_GAUGE_HP,
          LA_STALOW_BATTLE_STA_LOW_000__HP_PLAYER__GAUGE_00__BATTLE_HPGAUGE_COLOR_NORMAL,
          LA_STALOW_BATTLE_STA_LOW_000__HP_PLAYER__GAUGE_00__BATTLE_HPGAUGE_COLOR_HALF,
          LA_STALOW_BATTLE_STA_LOW_000__HP_PLAYER__GAUGE_00__BATTLE_HPGAUGE_COLOR_FEW,
        },

        //  ::app::tool::AppToolGaugePartsLayout::INIT_PARAM(EXPゲージ)
        {
          pG2DUtil,
          LYT_WK_TYPE_SCREEN,
          pPartsPaneExpGauge,
          LA_STALOW_BATTLE_STA_LOW_000__HP_PLAYER__GAUGE_01_GAUGE_00,
        },

        //  ポケモンタイプアイコンの画像ペイン
        pLayoutWork->GetPicturePane( pLayoutWork->GetPartsPane( PANENAME_BATTLE_STA_LOW_000_PANE_PARTSTYPE_00 ), PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, &rMultiResId ),
        pLayoutWork->GetPicturePane( pLayoutWork->GetPartsPane( PANENAME_BATTLE_STA_LOW_000_PANE_PARTSTYPE_01 ), PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, &rMultiResId )
      };

      m_pPlateObject->Initialize( initParam, paneConfigParam, m_initParam.pAppCommonGrpIconData );
    }

    //  ステータスリストの初回ページ指定
    m_statusList.SetPage( 0 );
    
    UIView::SetVisible( false );
  }

  //------------------------------------------------------------------
  /**
    * @brief   終了処理
    */
  //------------------------------------------------------------------
  void BattleUIPokeStatusLowerDrawBase::Terminate()
  {
    m_pPlateObject->Terminate();
  }

  //------------------------------------------------------------------
  /**
    * @brief   破棄可能か
    */
  //------------------------------------------------------------------
  bool BattleUIPokeStatusLowerDrawBase::IsDelete()
  {
    if( UIView::IsDrawing() == false )
    {
      return  m_pPlateObject->IsDelete();
    }

    return  false;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 更新処理
    */
  //-------------------------------------------------------------------------
  void BattleUIPokeStatusLowerDrawBase::Update( void )
  {
    _UpdateState();

    m_pPlateObject->Update();
    
    UIView::Update();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 描画
    */
  //-------------------------------------------------------------------------
  void BattleUIPokeStatusLowerDrawBase::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    for( s32 i = 0; i < LYT_WK_TYPE_MAX; ++i )
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderingManager, displayNo, i, LOW_VIEW_ORDER_OTHER );
    }

    m_statusList.Draw();
  }

  //------------------------------------------------------------------
  /**
    * @brief ボタンを押す
    */
  //------------------------------------------------------------------
  void BattleUIPokeStatusLowerDrawBase::PushBtn( const ButtonId id )
  {
    UIResponder::GetButtonManager()->StartSelectedAct( id );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインイベントの検知
  *       【ボタンマネージャー関係】
  *
  * @param  buttonId   通知を受けるペインＩＤ
  *
  * @return 同フレーム内での他入力イベントを許可するならtrue、
  *         他のイベントを排除するならfalseを返却すること。
  */
  //--------------------------------------------------------------------------------------------
  // overwride
  ::app::ui::UIInputListener::ListenerResult BattleUIPokeStatusLowerDrawBase::OnLayoutPaneEvent( const u32 buttonId )
  {
    GFL_PRINT( "OnLayoutPaneTouchTriggerEvent : buttonId = %d\n", buttonId );

    if( m_initParam.pDelegate == NULL )
    {
      return  ENABLE_ACCESS;
    }

    //  ボタンアニメ再生はすでにボタン管理側で行っている
    BattleUIPokeStatusLowerDrawDelegate::ENTER_DATA  enterData;
    enterData.buttonId  = buttonId;

    if( buttonId == BUTTON_ID_INFO_TOKUSEI )
    {
      //  インフォアイコンの座標を設定
      enterData.pushInfoIconPos = UIResponder::GetButtonManager()->GetButtonPosByGfVec3( buttonId );
    }
    else
    if( buttonId == BUTTON_ID_INFO_ITEM )
    {
      //  インフォアイコンの座標を設定
      enterData.pushInfoIconPos = UIResponder::GetButtonManager()->GetButtonPosByGfVec3( buttonId );;
    }
    else
    {
      const u32 aStatusListInfoButtonId[] =
      {
        BUTTON_ID_STATIS_LIST_ITEM_INFO_0,
        BUTTON_ID_STATIS_LIST_ITEM_INFO_1,
        BUTTON_ID_STATIS_LIST_ITEM_INFO_2,
        BUTTON_ID_STATIS_LIST_ITEM_INFO_3,
      };

      u32 loopNum = GFL_NELEMS( aStatusListInfoButtonId );
      for( u32 i = 0; i < loopNum; ++i )
      {
        if( aStatusListInfoButtonId[ i ] == buttonId )
        {
          enterData.pushStatusData  = m_statusList.GetCurrentStatusItemData( i );
          enterData.pushInfoIconPos = UIResponder::GetButtonManager()->GetButtonPosByGfVec3( buttonId );

          break;
        }
      }
    }

    if( m_initParam.pDelegate->OnEnterInput( this, enterData ) )
    {
      return  DISABLE_ACCESS;
    }

    return  ENABLE_ACCESS;
  }

  //------------------------------------------------------------------
  /**
    * @brief   キー入力の検知
    *
    * @param  pKey    十字キー
    * @param  pStick  アナログスティック
    *
    * @return 同フレーム内での他入力イベントを許可するならtrue、
    *         他のイベントを排除するならfalseを返却すること。
    */
  //------------------------------------------------------------------
  // overwride
  ::app::ui::UIInputListener::ListenerResult BattleUIPokeStatusLowerDrawBase::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    if( m_initParam.pDelegate == NULL )
    {
      return ENABLE_ACCESS;
    }

    if( m_initParam.pDelegate->OnKeyAction( this, pButton, pKey, pStick ) )
    {
      return DISABLE_ACCESS;
    }

    return ENABLE_ACCESS;
  }

  /**
    * @brief レイアウトワークを生成する
    */
  void BattleUIPokeStatusLowerDrawBase::_Initialize2D( void* pAppLytBuff )
  {
    //  2D画面作成
    {
      // アーカイブIDテーブル
      const ::app::util::G2DUtil::LytResData resTbl[] =
      {
        {  pAppLytBuff, 32, (u32)::app::util::G2DUtil::ATTACH_ROOT },
      };

      //  作成するレイアウトワーク
      const ::app::util::G2DUtil::LytwkData setupData[ LYT_WK_TYPE_MAX ] =
      {
          //  画面作成
          {
            0,
            LYTRES_STALOW_BATTLE_STA_LOW_000_BFLYT,
            LA_STALOW_BATTLE_STA_LOW_000___NUM,
            pAppLytBuff,
            true,
            ::app::util::G2DUtil::SETUP_LOWER,
            true
          },
      };

      UIView::Create2D( m_initParam.pUtilHeap, NULL, 32, resTbl, GFL_NELEMS(resTbl), setupData, GFL_NELEMS(setupData), m_initParam.pBtlAppMsgData, m_initParam.pBtlWordSet );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void BattleUIPokeStatusLowerDrawBase::_UpdateState()
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    if( m_state == STATE_TYPE_NONE )
    {
    }

    if( m_state == STATE_TYPE_IN )
    {
      //  アニメの終了を検知
      if( pG2DUtil->IsAnimeEnd( LYT_WK_TYPE_SCREEN, LA_STALOW_BATTLE_STA_LOW_000_IN ) )
      {
        pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_STALOW_BATTLE_STA_LOW_000_BEATANIME );
        _SetState( STATE_TYPE_INPUT_IDLE );
      }
    }

    if( m_state == STATE_TYPE_INPUT_IDLE )
    {
    }

    if( m_state == STATE_TYPE_OUT )
    {
      //  アニメの終了を検知
      _SetState( STATE_TYPE_END );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void BattleUIPokeStatusLowerDrawBase::_SetState( const BattleUIPokeStatusLowerDrawBase::StateType newStateType )
  {
    GFL_ASSERT( m_state != newStateType );

    m_state = newStateType;

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    if( m_state == STATE_TYPE_NONE )
    {
    }
    else
    if( m_state == STATE_TYPE_IN )
    {
      UIView::SetVisible( true );

      //  開始アニメをする
      pG2DUtil->StartAnimeFrame( LYT_WK_TYPE_SCREEN, LA_STALOW_BATTLE_STA_LOW_000_IN, 0, true, true ); 
    }
    else
    if( m_state == STATE_TYPE_INPUT_IDLE )
    {
    }
    else
    if( m_state == STATE_TYPE_OUT )
    {
    }
    else
    if( m_state == STATE_TYPE_END )
    {
      UIView::SetVisible( false );
    }
  }

//  ここから先は BattleUIPokeStatusLowerDrawBase::ParamItemRankObjectクラスの定義

  //------------------------------------------------------------------
  /**
    * @brief 初期化処理
    * @note
    *  クラスを実行するまえに呼ばないと動作保障はしない
    */
  //------------------------------------------------------------------
  void BattleUIPokeStatusLowerDrawBase::ParamItemRankObject::Initialize( UIView* pParentUIView, const u32 layoutId, const s32 resId,  const INIT_PARAM& rInitParam )
  {
    GFL_ASSERT( pParentUIView );
    m_pParentUIView   = pParentUIView;
    m_layoutId        = layoutId;
    m_initParam       = rInitParam;

    ::app::util::G2DUtil* pG2DUtil  = m_pParentUIView->GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    {
      m_pLayoutWork = pParentUIView->GetLayoutWork( layoutId );
      GFL_ASSERT( m_pLayoutWork );

      m_pStatusPartsPane  = m_pLayoutWork->GetPartsPane( m_initParam.status_parts_pane_id );
      GFL_ASSERT( m_pStatusPartsPane );

      m_pResId = &( pG2DUtil->GetLayoutResourceID( resId ) );
      GFL_ASSERT( m_pResId );
    }

    SetItemName( m_initParam.item_name_message_id );
    SetStatusRank( m_initParam.status_rank_num );
  }

  /** 項目名を指定 */
  void BattleUIPokeStatusLowerDrawBase::ParamItemRankObject::SetItemName( const s32 messageId )
  {
    m_initParam.item_name_message_id  = messageId;

    m_pParentUIView->SetTextboxPaneMessage( m_layoutId, m_initParam.item_name_pane_id, messageId );
  }

  //------------------------------------------------------------------
  /**
    * @brief ステータスランク値を指定
    * @note
    *   rankNum = RANK_STATUS_DEFAULT であればステータスランクなし
    */
  //------------------------------------------------------------------
  void BattleUIPokeStatusLowerDrawBase::ParamItemRankObject::SetStatusRank( const s32 rankNum )
  {
    GFL_ASSERT( m_pParentUIView );

    ::app::util::G2DUtil* pG2DUtil  = m_pParentUIView->GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    //  表示
    pG2DUtil->SetPaneVisible( m_pStatusPartsPane, true );

    s32 visibleIconRankNum = 0;
    
    if( rankNum == BTL_POKEPARAM::RANK_STATUS_DEFAULT )
    {
      //  ステータスが何も変わっていない
    }
    //  ステータスのランクが下がっている
    else
    if( rankNum < BTL_POKEPARAM::RANK_STATUS_DEFAULT )
    {
      pG2DUtil->StartAnime( m_layoutId, m_initParam.status_down_type_change_anime_idx );

      //  ステータスの演出ループアニメ
      {
        pG2DUtil->StartAnime( m_layoutId, m_initParam.status_down_type_effect_loop_anime );
      }

      visibleIconRankNum  = BTL_POKEPARAM::RANK_STATUS_DEFAULT - rankNum;
    }
    //  ステータスのランクが上がっている
    else
    if( BTL_POKEPARAM::RANK_STATUS_DEFAULT < rankNum )
    {
      pG2DUtil->StartAnime( m_layoutId, m_initParam.status_up_type_change_anime_idx );

      //  ステータスの演出ループアニメ
      {
        pG2DUtil->StartAnime( m_layoutId, m_initParam.status_up_type_effect_loop_anime );
      }

      visibleIconRankNum  = rankNum - BTL_POKEPARAM::RANK_STATUS_DEFAULT;
    }
    else
    {
      //  ここにきたら論理バグ
      GFL_ASSERT( 0 );
    }

    //  ランクのアイコンの表示・非表示
    {
      const s32 aIconPaneIdList[] =
      {
        PANENAME_BATTLE_PLT_LOW_006_PANE_ICON_00,
        PANENAME_BATTLE_PLT_LOW_006_PANE_ICON_01,
        PANENAME_BATTLE_PLT_LOW_006_PANE_ICON_02,
        PANENAME_BATTLE_PLT_LOW_006_PANE_ICON_03,
        PANENAME_BATTLE_PLT_LOW_006_PANE_ICON_04,
        PANENAME_BATTLE_PLT_LOW_006_PANE_ICON_05,
      };

      gfl2::lyt::LytPane* pIconPane = NULL;
      s32 loopNum = GFL_NELEMS( aIconPaneIdList );
      for( s32 i = 0; i < loopNum; ++i )
      {
        pIconPane = m_pLayoutWork->GetPane( m_pStatusPartsPane, aIconPaneIdList[ i ], m_pResId );
        GFL_ASSERT( pIconPane );

        if( visibleIconRankNum == 0 )
        {
          pG2DUtil->SetPaneVisible( pIconPane, false );
        }
        else
        {
          if( i < visibleIconRankNum )
          {
            pG2DUtil->SetPaneVisible( pIconPane, true );
          }
          else
          {
            pG2DUtil->SetPaneVisible( pIconPane, false );
          }
        }
      }
    }
  }

//  ここから先はクラス BattleUIPokeStatusLowerDrawBase::StatusPlatePartsObject の定義

  //------------------------------------------------------------------
  /**
    * @brief 初期化処理
    * @note
    *  クラスを実行するまえに呼ばないと動作保障はしない
    */
  //------------------------------------------------------------------
  void BattleUIPokeStatusLowerDrawBase::StatusPlatePartsObject::Initialize( const INIT_PARAM& rInitParam, const PANE_CONFIG_PARAM& rPaneConfigParam, ::app::tool::AppCommonGrpIconData* pAppCommonGrpIconData )
  {
    GFL_ASSERT( rInitParam.pUiView );
    GFL_ASSERT( rInitParam.pUtilHeap );
    GFL_ASSERT( 0 <= rInitParam.layoutWorkId );
    GFL_ASSERT( rInitParam.pPokeParam );
    GFL_ASSERT( pAppCommonGrpIconData );

    m_initParam   = rInitParam;
    m_animeIdData = rPaneConfigParam.plt_upp_data.anime_id_data;

    const pml::pokepara::PokemonParam*  pSrcPokemonParam  = m_initParam.pPokeParam->GetSrcData();
    GFL_ASSERT( pSrcPokemonParam );

    ::app::util::G2DUtil* pG2DUtil  = m_initParam.pUiView->GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    //  名前とポケモンアイコンではこちらのパラメータを使う
    const pml::pokepara::PokemonParam*  pViewSrcPokemonParam  = pSrcPokemonParam;
    if( rInitParam.bSelfParam == true )
    {
    }
    else
    {
      pViewSrcPokemonParam  = m_initParam.pPokeParam->GetViewSrcData();
    }

    //  ポケモンステータスオブジェクト作成
    {
      gfl2::lyt::LytWk* pLayoutWork = m_initParam.pUiView->GetLayoutWork( m_initParam.layoutWorkId );
      GFL_ASSERT( pLayoutWork );

      ::app::tool::PokeIconToolEx::PokeIconToolParam  initParam;
      initParam.layout_id = m_initParam.layoutWorkId;
      initParam.pHeap     = m_initParam.pUtilHeap;
      initParam.pUIView   = m_initParam.pUiView;
      initParam.res_id    = 0;
      initParam.iconMax   = 1;
      initParam.sync      = false;

      m_pPokeIcon = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) ::app::tool::PokeIconToolEx( initParam );
      GFL_ASSERT( m_pPokeIcon );

      m_pPokeIcon->SetPokeIcon( 
        0, 
        pViewSrcPokemonParam, 
        rPaneConfigParam.poke_icon_btn_low_001_data.root_parts_pane_id, 
        pLayoutWork->GetPartsPane( rPaneConfigParam.poke_icon_btn_low_001_data.icon_pane_id ),
        m_initParam.bPokeIconFlip );
    }

    //  名前設定
    {
      GFL_ASSERT( rPaneConfigParam.plt_upp_data.p_name_text_box_pane );

      pG2DUtil->SetRegisterPokeNickName( 0, pViewSrcPokemonParam );
      pG2DUtil->SetTextBoxPaneStringExpand( rPaneConfigParam.plt_upp_data.p_name_text_box_pane, STR_POKE_BTL_NAME );
    }

    //  レベル表記
    {
      pG2DUtil->SetTextBoxPaneString( rPaneConfigParam.plt_upp_data.p_lv_item_text_box_pane, STR_LV_S );
    }

    //  レベル設定
    {
      pG2DUtil->SetTextBoxPaneNumber( rPaneConfigParam.plt_upp_data.p_lv_number_text_box_pane, STR_VALUE_3, pSrcPokemonParam->GetLevel(), 3 );
    }

    //  性別アイコン設定
    {
      pG2DUtil->SetPaneVisible( rPaneConfigParam.plt_upp_data.p_icon_parts, true );

      pml::Sex  nowSex  = PokeTool::GetDrawSex( pViewSrcPokemonParam );
      if( nowSex == pml::SEX_MALE )
      {
        //  オス
        pG2DUtil->StartAnime( m_initParam.layoutWorkId, rPaneConfigParam.icon_001_data.icon_male_anime_idx );
      }
      else if( nowSex == pml::SEX_FEMALE )
      {
        //  メス
        pG2DUtil->StartAnime( m_initParam.layoutWorkId, rPaneConfigParam.icon_001_data.icon_female_anime_idx );
      }
      else
      {
        //  それ以外
        //  非表示にする
        pG2DUtil->SetPaneVisible( rPaneConfigParam.plt_upp_data.p_icon_parts, false );
      }
    }

    //  メガ進化アイコン設定
    {
      if( m_initParam.pPokeParam->IsMegaEvolved() )
      {
        //  メガ進化中
        pG2DUtil->StartAnime( m_initParam.layoutWorkId, rPaneConfigParam.icon_003_data.icon_active_anime_idx );
        pG2DUtil->StartAnime( m_initParam.layoutWorkId, rPaneConfigParam.icon_003_data.icon_mega_anime_idx );
      }   
      else
      // @fix NMCat[2379] ゲンシカイキアイコンを出す（仕様漏れ）
      if( m_initParam.pPokeParam->IsGensiKaiki() )   
      {
        pG2DUtil->StartAnime( m_initParam.layoutWorkId, rPaneConfigParam.icon_003_data.icon_active_anime_idx );

        if( m_initParam.pPokeParam->GetMonsNo() == MONSNO_GURAADON )
        {
          //  グラードン( べにいろのたま )
          pG2DUtil->StartAnime( m_initParam.layoutWorkId, rPaneConfigParam.icon_003_data.icon_omega_anime_idx );
        }
        else
        if( m_initParam.pPokeParam->GetMonsNo() == MONSNO_KAIOOGA )
        {
          //  カイオーガー( あいいろのたま )
          pG2DUtil->StartAnime( m_initParam.layoutWorkId, rPaneConfigParam.icon_003_data.icon_alpha_anime_idx );
        }
      }
      else
      {
        //  メガ進化でない
        pG2DUtil->StartAnime( m_initParam.layoutWorkId, rPaneConfigParam.icon_003_data.icon_passive_anime_idx );
      }
    }

    //  捕獲済みかのアイコン設定
    {
      // @fix GFNMCat[4871] 捕獲済みかの表示は非表示になりました。
      //pG2DUtil->SetPaneVisible( rPaneConfigParam.plt_upp_data.p_ball_icon_pane, m_initParam.bRegsterZukan );
      pG2DUtil->SetPaneVisible( rPaneConfigParam.plt_upp_data.p_ball_icon_pane, false );
    }

    //  状態異常設定
    {
      bool  bSickVisible  = true;

      pml::pokepara::Sick nowSick = m_initParam.pPokeParam->GetPokeSick();
      //  ひんし
      if( m_initParam.pPokeParam->IsDead() )
      {
        //  UVテクスチャーの差し替え
        pAppCommonGrpIconData->ReplacePaneTextureByPokeHinshiIcon( rPaneConfigParam.plt_upp_data.p_sick_icon_picture );
      }
      else
      //  状態異常：もうどく
      if( m_initParam.pPokeParam->CheckMoudoku() == true )
      {
        //  UVテクスチャーの差し替え
        pAppCommonGrpIconData->ReplacePaneTextureByPokeDokudokuIcon( rPaneConfigParam.plt_upp_data.p_sick_icon_picture );
      }
      else
      //  状態異常
      if( nowSick != pml::pokepara::SICK_NULL )
      {
        //  UVテクスチャーの差し替え
        pAppCommonGrpIconData->ReplacePaneTextureByPokeSickIcon( nowSick, rPaneConfigParam.plt_upp_data.p_sick_icon_picture );
      }
      else
      //  ポケルス感染
      if( m_initParam.pPokeParam->HavePokerus() == true )
      {
        //  UVテクスチャーの差し替え
        pAppCommonGrpIconData->ReplacePaneTextureByPokerusIcon( rPaneConfigParam.plt_upp_data.p_sick_icon_picture );
      }
      else
      {
        bSickVisible  = false;
      }

      if( bSickVisible == true )
      {
        pG2DUtil->StartAnime( m_initParam.layoutWorkId, rPaneConfigParam.plt_upp_data.anime_id_data.keep_sick_anime_idx );
        pG2DUtil->StartAnime( m_initParam.layoutWorkId, rPaneConfigParam.icon_002_data.icon_acitve_anime_idx );
      }
      else
      {
        pG2DUtil->StartAnime( m_initParam.layoutWorkId, rPaneConfigParam.icon_002_data.icon_passive_anime_idx );
      }
    }

    //  HPゲージ
    {
      m_pHpGaugePartsLayout = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) ::app::tool::AppToolHPGaugePartsLayout( rPaneConfigParam.hp_gauge_data, m_initParam.pUtilHeap->GetDeviceHeap() );
      GFL_ASSERT( m_pHpGaugePartsLayout );

      m_pHpGaugePartsLayout->SetNumber( m_initParam.pPokeParam->GetValue( BTL_POKEPARAM::BPP_HP ), m_initParam.pPokeParam->GetValue( BTL_POKEPARAM::BPP_MAX_HP ) );
    }

    //  EXPゲージ
    {
      m_pExpGaugePartsLayout  = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) ::app::tool::AppToolGaugePartsLayout( rPaneConfigParam.exp_gauge_data, m_initParam.pUtilHeap->GetDeviceHeap() );
      GFL_ASSERT( m_pExpGaugePartsLayout );
      m_pExpGaugePartsLayout->SetVisible( false );

      //  次のLVのExpを取得
      const u32 nextLvExp = ::app::util::GetPokeNextLvExpNum( pSrcPokemonParam );
      const u32 nowLvExp  = ::app::util::GetPokeNowLvExpNum( pSrcPokemonParam );

      m_pExpGaugePartsLayout->SetNumber( nowLvExp, nextLvExp );
    }

    //  ポケモン死亡時の設定
    if( m_initParam.pPokeParam->IsDead() )
    {
      //  死亡アニメに
      m_pPokeIcon->StartAnim( rPaneConfigParam.poke_icon_btn_low_001_data.anime_die_idx );

      //  死亡アイコンを出す
    }

    //  タイプアイコンの設定
    {
      if( m_initParam.bShowTypeIcon == true )
      {
        //  タイプ１
        if( m_initParam.aPokeType[ 0 ] == POKETYPE_NULL )
        {
          pG2DUtil->SetPaneVisible( rPaneConfigParam.p_poke_type1_icon_picture, false );
        }
        else
        {
          pAppCommonGrpIconData->ReplacePaneTextureByWazaTypeIcon( (PokeType_tag)m_initParam.aPokeType[ 0 ], rPaneConfigParam.p_poke_type1_icon_picture );
          pG2DUtil->SetPaneVisible( rPaneConfigParam.p_poke_type1_icon_picture, true );
        }

        //  タイプ2
        if( m_initParam.aPokeType[ 1 ] == POKETYPE_NULL )
        {
          pG2DUtil->SetPaneVisible( rPaneConfigParam.p_poke_type2_icon_picture, false );
        }
        else
        {
          pAppCommonGrpIconData->ReplacePaneTextureByWazaTypeIcon( (PokeType_tag)m_initParam.aPokeType[ 1 ], rPaneConfigParam.p_poke_type2_icon_picture );
          pG2DUtil->SetPaneVisible( rPaneConfigParam.p_poke_type2_icon_picture, true );
        }
      }
      else
      {
        pG2DUtil->SetPaneVisible( rPaneConfigParam.p_poke_type1_icon_picture, false );
        pG2DUtil->SetPaneVisible( rPaneConfigParam.p_poke_type2_icon_picture, false );
      }
    }

#if 1 // ヒープ残量チェック
    GFL_PRINT( "BattleUIPokeStatusPlatePartsObject: 0x%x/0x%x, 0x%x/0x%x\n", 
        m_initParam.pUtilHeap->GetSystemHeap()->GetTotalFreeSize(), m_initParam.pUtilHeap->GetSystemHeap()->GetTotalSize(),
        m_initParam.pUtilHeap->GetDeviceHeap()->GetTotalFreeSize(), m_initParam.pUtilHeap->GetDeviceHeap()->GetTotalSize() );
#endif
  }

  //  自身のパラメータ表示か
  void BattleUIPokeStatusLowerDrawBase::StatusPlatePartsObject::SetEnableSelfParamView( const bool bFlg )
  {
    ::app::util::G2DUtil* pG2DUtil  = m_initParam.pUiView->GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    if( bFlg == true )
    {
      pG2DUtil->StartAnime( m_initParam.layoutWorkId, m_animeIdData.pos_own_anime_idx );

      m_pHpGaugePartsLayout->SetTextVisible( true );
      //  Expゲージを表示
      m_pExpGaugePartsLayout->SetVisible( true );
    }
    else
    {
      pG2DUtil->StartAnime( m_initParam.layoutWorkId, m_animeIdData.pos_other_anime_idx );

      //  HPゲージのテキストとexpゲージを非表示
      m_pHpGaugePartsLayout->SetTextVisible( false );
      m_pExpGaugePartsLayout->SetVisible( true );
    }
  }

//  ここからはクラス BattleUIPokeStatusLowerDrawBase::StatusItemObject の定義

  //------------------------------------------------------------------
  /**
    * @brief 初期化処理
    * @note
    *  クラスを実行するまえに呼ばないと動作保障はしない
    */
  //------------------------------------------------------------------
  void BattleUIPokeStatusLowerDrawBase::StatusItemObject::Initialize( UIView* pParentUIView, const u32 layoutId, const u32 resId, gfl2::heap::HeapBase* pHeap, const INIT_PARAM& rInitParam )
  {
    GFL_ASSERT( pParentUIView );
    GFL_ASSERT( pHeap );

    m_pG2DUtil  = pParentUIView->GetG2DUtil();
    GFL_ASSERT( m_pG2DUtil );

    gfl2::lyt::LytWk*         pLayoutWork = pParentUIView->GetLayoutWork( layoutId );
    GFL_ASSERT( pLayoutWork );

    gfl2::lyt::LytMultiResID& rMultResId  = m_pG2DUtil->GetLayoutResourceID( resId );

    //  状態名の文字列バッファ
    {
      m_pStatusName = GFL_NEW( pHeap ) gfl2::str::StrBuf( LIST_ITEM_TITLE_NAME_LEN, pHeap );
    }

    //  必要ペイン取得
    {
      //  項目のルートペイン
      m_pRootPane       = pLayoutWork->GetPane( rInitParam.item_root_pane_id );
      GFL_ASSERT( m_pRootPane );

      //  項目名のテキストペイン
      m_pTitleTextPane  = pLayoutWork->GetTextBoxPane( rInitParam.item_name_pane_id );
      GFL_ASSERT( m_pTitleTextPane );

      //  数字のテキストペイン
      m_pNumTextPane  = pLayoutWork->GetTextBoxPane( rInitParam.item_num_pane_id );
      GFL_ASSERT( m_pNumTextPane );
    }

    //  インフォボタン作成
    {
      gfl2::lyt::LytParts*  pInfoPartsPane  = pLayoutWork->GetPartsPane( rInitParam.info_icon_parts_pane_id );
      GFL_ASSERT( pInfoPartsPane );

      m_buttonDataInfoIcon.picture_pane = pInfoPartsPane;
      m_buttonDataInfoIcon.bound_pane   = pLayoutWork->GetBoundingPane( pInfoPartsPane, rInitParam.info_icon_bound_pane_id, &rMultResId );
      m_buttonDataInfoIcon.touch_anime_index      = rInitParam.a_info_anime_id[ BUTTON_ANIME_ID_TOUCH  ];
      m_buttonDataInfoIcon.cancel_anime_index     = rInitParam.a_info_anime_id[ BUTTON_ANIME_ID_CANCEL ];
      m_buttonDataInfoIcon.active_anime_index     = rInitParam.a_info_anime_id[ BUTTON_ANIME_ID_ACTIVE ];
      m_buttonDataInfoIcon.key_select_anime_index = rInitParam.a_info_anime_id[ BUTTON_ANIME_ID_KEY_SELECT ];
      m_buttonDataInfoIcon.passive_anime_index    = rInitParam.a_info_anime_id[ BUTTON_ANIME_ID_PASSIVE ];
      m_buttonDataInfoIcon.release_anime_index    = rInitParam.a_info_anime_id[ BUTTON_ANIME_ID_RELEASE ];

      //  インフォボタンの座標保存
      const gfl2::math::VEC3 pos  = m_pG2DUtil->GetPanePos( pInfoPartsPane );
      m_infoIconPos.Set( pos.x, pos.y, pos.z );
    }
  }

  //  表示設定
  void BattleUIPokeStatusLowerDrawBase::StatusItemObject::SetVisible( const bool bFlg )
  {
    m_pG2DUtil->SetPaneVisible( m_pRootPane, bFlg );

    //  ボタン関連のペインも表示設定（ボタン管理内でペインの表示でタッチ判定をしているから）
    m_pG2DUtil->SetPaneVisible( m_buttonDataInfoIcon.picture_pane, bFlg );
  }

  //  テキスト設定
  void BattleUIPokeStatusLowerDrawBase::StatusItemObject::SetText( const BattleUIDataManager::STATUS_DATA& rStatusData, gfl2::str::MsgData* pStatusMsgData )
  {
    GFL_ASSERT( pStatusMsgData );

    m_pG2DUtil->SetPaneVisible( m_pNumTextPane, true );

    //  タイプがないのであれば数字表示はしない
    if( rStatusData.type == BattleUIDataManager::STATUS_TYPE_NONE )
    {
      m_pG2DUtil->SetPaneVisible( m_pNumTextPane, false );
    }
    else
    if( rStatusData.type == BattleUIDataManager::STATUS_TYPE_COUNT )
    {
      //  ただの数字表示
      if( 10 <= rStatusData.data.count.num )
      {
        // @fix NMCat[1452] 数字は一桁限定なのに２桁以上の場合は永続状態なので非表示にする
        m_pG2DUtil->SetPaneVisible( m_pNumTextPane, false );
      }
      else
      {
        m_pG2DUtil->SetTextBoxPaneNumber( m_pNumTextPane, STR_VALUE_2, rStatusData.data.count.num, 1, 0 );
      }
    }
    else
    if( rStatusData.type == BattleUIDataManager::STATUS_TYPE_COUNT_SIGN )
    {
      //  +数字表示
      if( 10 <= rStatusData.data.count.num )
      {
        // @fix NMCat[1452] 数字は一桁限定なのに２桁以上の場合は永続状態なので非表示にする
        m_pG2DUtil->SetPaneVisible( m_pNumTextPane, false );
      }
      else
      {
        m_pG2DUtil->SetTextBoxPaneNumber( m_pNumTextPane, STR_VALUE_TURN_1, rStatusData.data.count.num, 1, 0 );
      }
    }
    else
    if( rStatusData.type == BattleUIDataManager::STATUS_TYPE_TURN )
    {
      //  ターン数表示( 0 / 0 の表記なので、数値を任意で登録が必要 )
      if( ( 10 <= rStatusData.data.turn.now ) || ( 10 <= rStatusData.data.turn.max ) )
      {
        // @fix NMCat[1452] 数字は一桁限定なのに２桁以上の場合は永続状態なので非表示にする
        m_pG2DUtil->SetPaneVisible( m_pNumTextPane, false );
      }
      else
      if( rStatusData.data.turn.max <= 0 )
      {
        // @fix NMCat[1717] 天候などで永続する状態では最大ターン数が０のケースが生まれたので、この場合はターン数は非表示にする
        m_pG2DUtil->SetPaneVisible( m_pNumTextPane, false );
      }
      else
      {
        m_pG2DUtil->SetRegisterNumber( 0, rStatusData.data.turn.now, 1 );
        m_pG2DUtil->SetTextBoxPaneNumber( m_pNumTextPane, STR_VALUE_TURN_2, rStatusData.data.turn.max, 1, 1 );
      }
    }

    //  状態名を設定
    {
      pStatusMsgData->GetString( rStatusData.titleNameId, *m_pStatusName );
      m_pG2DUtil->SetRegisterWord( 0, *m_pStatusName );

      m_pG2DUtil->SetTextBoxPaneStringExpand( m_pTitleTextPane, STR_BTL_STATE_NAME );
    }
  }

//  ここから先はクラス BattleUIPokeStatusLowerDrawBase::StatusList の定義

  //  初期化
  void BattleUIPokeStatusLowerDrawBase::StatusList::Initialize( UIView* pRootView, const u32 layoutId, const u32 resId, const BattleUIPokeStatusLowerDrawBase::INIT_PARAM* pRootDrawInitParam )
  {
    GFL_ASSERT( pRootView );
    GFL_ASSERT( pRootDrawInitParam );

    m_pDrawInitParam  = pRootDrawInitParam;
    m_pRootView = pRootView;

    u32 listViewMax = GFL_NELEMS( m_aStatusItemList );

    //  ステータス項目オブジェクト初期化
    {
      GFL_ASSERT( listViewMax == GFL_NELEMS( m_aListItemInitParamTable ) );

      for( u32 i = 0; i < listViewMax; ++i )
      {
        m_aStatusItemList[ i ].Initialize( pRootView, layoutId, 0, pRootDrawInitParam->pUtilHeap->GetDeviceHeap(), m_aListItemInitParamTable[ i ] );
      }
    }

    //  リストデータ作成
    //  リストの並び順を変える、リストの後ほどデータが新しいのでデータ作成時にはリストの並びを逆順にする。
    //  これをすることで、リストの先頭が新しいデータからの並びになる
    {
      s32 loopNum = (s32)pRootDrawInitParam->pStatusDataList->statusNum;

      m_pStatusDataList = GFL_NEW_ARRAY( pRootDrawInitParam->pUtilHeap->GetDeviceHeap() ) STATUS_DATA[ loopNum ];
      for( s32 drawListIdx = 0, statusDataIdx = loopNum - 1; 0 <= statusDataIdx; --statusDataIdx, ++drawListIdx )
      {
        //  表示する時にはリストの並びを降順に変えた下記のリストを使う
        m_pStatusDataList[ drawListIdx ].pOrgData = &pRootDrawInitParam->pStatusDataList->aStatus[ statusDataIdx ];
      }
    }

    ::app::util::G2DUtil* pG2DUtil           = m_pRootView->GetG2DUtil();
    gfl2::lyt::LytWk* pLayoutWork            = m_pRootView->GetLayoutWork( layoutId );
    gfl2::lyt::LytMultiResID&  rMulitiResId  = pG2DUtil->GetLayoutResourceID( resId );

    //  左矢印ボタンデータ作成
    {
      gfl2::lyt::LytParts*  pArrowPartsPane = pLayoutWork->GetPartsPane( PANENAME_BATTLE_STA_LOW_000_PANE_PAGEARROW_L );
      GFL_ASSERT( pArrowPartsPane );

      m_buttonDataLeftArrow.picture_pane  = pArrowPartsPane;
      m_buttonDataLeftArrow.bound_pane    = pLayoutWork->GetBoundingPane( pArrowPartsPane, PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00, &rMulitiResId );
      GFL_ASSERT( m_buttonDataLeftArrow.bound_pane );

      m_buttonDataLeftArrow.active_anime_index      = LA_STALOW_BATTLE_STA_LOW_000__PAGEARROW_L_ACTIVE;
      m_buttonDataLeftArrow.cancel_anime_index      = LA_STALOW_BATTLE_STA_LOW_000__PAGEARROW_L_CANSEL;
      m_buttonDataLeftArrow.key_select_anime_index  = LA_STALOW_BATTLE_STA_LOW_000__PAGEARROW_L_TOUCH_RELEASE;
      m_buttonDataLeftArrow.passive_anime_index     = LA_STALOW_BATTLE_STA_LOW_000__PAGEARROW_L_PASSIVE;
      m_buttonDataLeftArrow.release_anime_index     = LA_STALOW_BATTLE_STA_LOW_000__PAGEARROW_L_RELEASE;
      m_buttonDataLeftArrow.touch_anime_index       = LA_STALOW_BATTLE_STA_LOW_000__PAGEARROW_L_TOUCH;
    }

    //  右矢印ボタンデータ作成
    {
      gfl2::lyt::LytParts*  pArrowPartsPane = pLayoutWork->GetPartsPane( PANENAME_BATTLE_STA_LOW_000_PANE_PAGEARROW_R );
      GFL_ASSERT( pArrowPartsPane );

      m_buttonDataRightArrow.picture_pane  = pArrowPartsPane;
      m_buttonDataRightArrow.bound_pane    = pLayoutWork->GetBoundingPane( pArrowPartsPane, PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00, &rMulitiResId );
      GFL_ASSERT( m_buttonDataRightArrow.bound_pane );

      m_buttonDataRightArrow.active_anime_index      = LA_STALOW_BATTLE_STA_LOW_000__PAGEARROW_R_ACTIVE;
      m_buttonDataRightArrow.cancel_anime_index      = LA_STALOW_BATTLE_STA_LOW_000__PAGEARROW_R_CANSEL;
      m_buttonDataRightArrow.key_select_anime_index  = LA_STALOW_BATTLE_STA_LOW_000__PAGEARROW_R_TOUCH_RELEASE;
      m_buttonDataRightArrow.passive_anime_index     = LA_STALOW_BATTLE_STA_LOW_000__PAGEARROW_R_PASSIVE;
      m_buttonDataRightArrow.release_anime_index     = LA_STALOW_BATTLE_STA_LOW_000__PAGEARROW_R_RELEASE;
      m_buttonDataRightArrow.touch_anime_index       = LA_STALOW_BATTLE_STA_LOW_000__PAGEARROW_R_TOUCH;
    }

    //  ページ計算
    {
      //  最大ページ数
      //  リストのページは０から数えるのでページ値は0値から開始
      m_pageListBottomNum = 0;

      if( 0 < pRootDrawInitParam->pStatusDataList->statusNum )
      {
        // @fix NMCat[3435] 計算の基準を１から０に変更してからページ数計算をした
        u32 indexNum = pRootDrawInitParam->pStatusDataList->statusNum - 1;
        if( 0 < indexNum )
        {
          m_pageListBottomNum = indexNum / listViewMax;
        }

        //  ドットを表示する数を設定( こちらは 1 から始まるので +1 にする)
        pG2DUtil->StartAnimeFrame( LYT_WK_TYPE_SCREEN, LA_STALOW_BATTLE_STA_LOW_000_PAGEDOT, m_pageListBottomNum + 1, false );
      }
      else
      {
        //  ドットは非表示
        pG2DUtil->StartAnimeFrame( LYT_WK_TYPE_SCREEN, LA_STALOW_BATTLE_STA_LOW_000_PAGEDOT, 0, false );
      }

      m_pageNowNum        = 0;
    }
  }

  //  描画
  void BattleUIPokeStatusLowerDrawBase::StatusList::Draw()
  {
  }

  //  現在表示しているページから指定行のステータスデータ取得
  const BattleUIDataManager::STATUS_DATA& BattleUIPokeStatusLowerDrawBase::StatusList::GetCurrentStatusItemData( const u32 viewListItemIdx )
  {
    // @fix NMCat[1429] データidxの取得にミスがあった
    //  ページ数 * 一ページの選択最大数 + 一ページ内の選択idx
    u32 statusListIdx = m_pageNowNum * GFL_NELEMS( m_aStatusItemList ) + viewListItemIdx;

    GFL_ASSERT( statusListIdx < m_pDrawInitParam->pStatusDataList->statusNum );
    if( statusListIdx < m_pDrawInitParam->pStatusDataList->statusNum )
    {
      return  *( m_pStatusDataList[ statusListIdx ].pOrgData );
    }

    //  ここにきたらハングする。
    GFL_ASSERT( 0 );
    return  *( m_pStatusDataList[ 0 ].pOrgData );
  }

  //  現在表示しているページから指定行のインフォアイコンの座標取得
  const gfl2::math::Vector3& BattleUIPokeStatusLowerDrawBase::StatusList::GetCurrentStatusInfoIconPos( const u32 viewListItemIdx )
  {
    GFL_ASSERT( viewListItemIdx < GFL_NELEMS( m_aStatusItemList ) );
    return  m_aStatusItemList[ viewListItemIdx ].GetInfoIconPos();
  }

  //  ページ設定
  void BattleUIPokeStatusLowerDrawBase::StatusList::SetPage( const u32 num )
  {
    ::app::tool::ButtonManager* pButtonManager  = m_pRootView->GetButtonManager();

    m_pageNowNum  = num;
    if( m_pageNowNum <= 0 )
    {
      //  現在のページは先頭なので上矢印はいらない
      pButtonManager->SetButtonInvisible( m_buttonDataLeftArrow.button_id );
    }
    else
    {
      pButtonManager->SetButtonVisible( m_buttonDataLeftArrow.button_id );
    }

    if( m_pageListBottomNum <= m_pageNowNum )
    {
      //  現在のページは一番下なので下矢印はいらない
      pButtonManager->SetButtonInvisible( m_buttonDataRightArrow.button_id );
    }
    else
    {
      pButtonManager->SetButtonVisible( m_buttonDataRightArrow.button_id );
    }

    //  ドット表示変更
    {
      ::app::util::G2DUtil* pG2DUtil           = m_pRootView->GetG2DUtil();
      pG2DUtil->StartAnimeFrame( LYT_WK_TYPE_SCREEN, LA_STALOW_BATTLE_STA_LOW_000_PAGEDOTCOLOR, num, false );
    }

    //  行の内容を更新
    {
      u32 listMax       = GFL_NELEMS( m_aStatusItemList );
      u32 firstListIdx  = m_pageNowNum * listMax;
      u32 refListIdx    = 0;

      BattleUIPokeStatusLowerDrawBase::StatusItemObject*  pListItemObj  = NULL;
      for( u32 list_i = 0; list_i < listMax; ++list_i )
      {
        pListItemObj  = &m_aStatusItemList[ list_i ];

        refListIdx  = list_i + firstListIdx;

        if( refListIdx < m_pDrawInitParam->pStatusDataList->statusNum )
        {
          //  表示
          pListItemObj->SetVisible( true );
          pListItemObj->SetText( *(m_pStatusDataList[ refListIdx ].pOrgData), m_pDrawInitParam->pBtlStatusMsgData );
        }
        else
        {
          //  非表示
          pListItemObj->SetVisible( false );
        }
      }
    }
  }

  //  ページ移動
  void BattleUIPokeStatusLowerDrawBase::StatusList::MoveNextPage()
  {
    u32 nowPage = m_pageNowNum + 1;
    
    if( m_pageListBottomNum < nowPage )
    {
      GFL_ASSERT( 0 );
      return;
    }

    SetPage( nowPage );
  }

  void BattleUIPokeStatusLowerDrawBase::StatusList::MovePrevPage()
  {
    u32 nowPage = m_pageNowNum - 1;
    
    if( (s32)nowPage < 0 )
    {
      GFL_ASSERT( 0 );
      return;
    }

    SetPage( nowPage );
  }

//  ここからはクラス BattleUISelfPokeStatusLowerDraw の実装

  //------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    * @note
    *  クラス動作に必要なパラメータは設定する。
    *  以後は設定したパラメータを参照して動作(途中でパラメータの変更はできない)
    */
  //------------------------------------------------------------------
  BattleUISelfPokeStatusLowerDraw::BattleUISelfPokeStatusLowerDraw( const INIT_PARAM& param ) : BattleUIPokeStatusLowerDrawBase( param )
  {
    _Clear();

    m_bPokeIconFlip = true;
  };

  //------------------------------------------------------------------
  /**
    * @brief 初期化処理
    * @note
    *  クラスを実行するまえに呼ばないと動作保障はしない
    */
  //------------------------------------------------------------------
  // overwride
  void BattleUISelfPokeStatusLowerDraw::Initialize()
  {
    BattleUIPokeStatusLowerDrawBase::_InitializeBase( true );

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    //  画面を敵用に切り替え
    {
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_STALOW_BATTLE_STA_LOW_000_STATUSOWN );
    }

    //  特性とアイテムのインフォボタンは味方側では表示するので有効にする
    {
      UIResponder::SetButtonEnable( BUTTON_ID_INFO_TOKUSEI, true, false );
      UIResponder::SetButtonEnable( BUTTON_ID_INFO_ITEM, true, false );
    }

    //  インフォ項目を作成
    {
      const InfoItemObject::INIT_PARAM  aInitParam[ INFO_TYPE_MAX ]  =
      {
        //  特性
        {
          PANENAME_BATTLE_STA_LOW_000_PANE_TEXT_TOKUSEI,
          BUTTON_ID_INFO_TOKUSEI,
        },

        //  アイテム
        {
          PANENAME_BATTLE_STA_LOW_000_PANE_TEXT_ITEM,
          BUTTON_ID_INFO_ITEM,
        },
      };

      for( s32 i = 0; i < INFO_TYPE_MAX; ++i )
      {
        m_pInfoItemObjectArray[ i ] = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) InfoItemObject();
        GFL_ASSERT( m_pInfoItemObjectArray[ i ] );

        m_pInfoItemObjectArray[ i ]->Initialize( this, m_initParam.pUtilHeap->GetDeviceHeap(), LYT_WK_TYPE_SCREEN, aInitParam[ i ] );
      }
    }

    //  特性表示設定
    {
      InfoItemObject* pInfoTokuseiObject  = m_pInfoItemObjectArray[ INFO_TYPE_TOKUSEI ];
      GFL_ASSERT( pInfoTokuseiObject );

      int tokuseiId = m_initParam.pPokeParam->GetValue( BTL_POKEPARAM::BPP_TOKUSEI );
      if( tokuseiId == TOKUSEI_NULL )
      {
        pInfoTokuseiObject->SetVisible( false );
      }
      else
      {
        //  特性名表示
        {
          pG2DUtil->SetRegisterTokuseiName( 0, tokuseiId );
          pG2DUtil->SetTextBoxPaneStringExpand( pInfoTokuseiObject->GetLayoutId(), pInfoTokuseiObject->GetNamePaneId(), STR_TOKUSEI_NAME );
        }

        pInfoTokuseiObject->SetVisible( true );
      }
    }

    //  所持アイテム表示設定
    {
      InfoItemObject* pInfoItemObject  = m_pInfoItemObjectArray[ INFO_TYPE_ITEM ];
      GFL_ASSERT( pInfoItemObject );

      u16 itemNo  = m_initParam.pPokeParam->GetItem();
      if( itemNo == ITEM_DUMMY_DATA )
      {
        pInfoItemObject->SetVisible( false );
      }
      else
      {
        //  アイテム名表示
        {
          pG2DUtil->SetRegisterItemName( 0, itemNo );
          pG2DUtil->SetTextBoxPaneStringExpand( pInfoItemObject->GetLayoutId(), pInfoItemObject->GetNamePaneId(), STR_ITEM_NAME );
        }

        pInfoItemObject->SetVisible( true );
      }
    }

    //  プレイヤー自身のパラメータ
    m_pPlateObject->SetEnableSelfParamView( true );
  }

//  ここからはクラス BattleUIEnemyPokeStatusLowerDraw の実装

  //------------------------------------------------------------------
  /**
    * @brief 初期化処理
    * @note
    *  クラスを実行するまえに呼ばないと動作保障はしない
    */
  //------------------------------------------------------------------
  // overwride
  void BattleUIEnemyPokeStatusLowerDraw::Initialize()
  {
    BattleUIPokeStatusLowerDrawBase::_InitializeBase( false );

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    //  画面を敵用に切り替え
    {
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_STALOW_BATTLE_STA_LOW_000_STATUSEM );
    }

    //  プレイヤー自身のパラメータでない
    m_pPlateObject->SetEnableSelfParamView( false );
  }

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )
