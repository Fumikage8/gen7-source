//=============================================================================
/**
 * @file BattleUIWatchLowerDraw.cpp
 * @brief バトル下画面 >> 「観戦画面」画面
 * @author yuto_uchida
 * @date 2015.05.07
 */
//=============================================================================
#include "BattleUIWatchLowerDraw.h"
#include "BattleUIViewDrawOrder.h"

#include "niji_conv_header/app/battle/res2d/obslow.h"
#include "niji_conv_header/app/battle/res2d/obslow_pane.h"
#include "niji_conv_header/app/battle/res2d/obslow_anim_list.h"
#include "niji_conv_header/message/msg_btl_app.h"
#include "niji_conv_header/sound/SoundMiddleID.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  //  ボタン作成データテーブル
  const BattleUIWatchLowerDraw::BUTTON_CREATE_DATA BattleUIWatchLowerDraw::m_aCreateButtonDataTable[ BUTTON_ID_MAX ]  =
  {
    //  戻る
    //  BUTTON_ID_BACK  = 0,
    {
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTN_BACK_TOUCH,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTN_BACK_RELEASE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTN_BACK_CANSEL,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTN_BACK_TOUCH_RELEASE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTN_BACK_ACTIVE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTN_BACK_PASSIVE,

      PANENAME_BATTLE_OBS_LOW_000_PANE_BTN_BACK,
      PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00
    },

    //  左下のポケモン、わざインフォ、プレイヤー名 -> ID: 00
    //  BUTTON_ID_POKE_ICON_LEFT_DOWN,
    {
      LA_OBSLOW_BATTLE_OBS_LOW_000__ICONPOKE_00_TOUCH,
      LA_OBSLOW_BATTLE_OBS_LOW_000__ICONPOKE_00_RELEASE,
      ::app::tool::ButtonManager::ANIMATION_NULL,
      LA_OBSLOW_BATTLE_OBS_LOW_000__ICONPOKE_00_TOUCH_RELEASE,
      ::app::tool::ButtonManager::ANIMATION_NULL,
      ::app::tool::ButtonManager::ANIMATION_NULL,

      PANENAME_BATTLE_OBS_LOW_000_PANE_ICONPOKE_00,
      PANENAME_COMMON_BTN_LOW_001_PANE_BOUND_00
    },

    //  BUTTON_ID_USER_NAME_LEFT_DOWN,
    {
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_00_TOUCH,
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_00_RELEASE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_00_CANSEL,
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_00_TOUCH_RELEASE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_00_ACTIVE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_00_PASSIVE,

      PANENAME_BATTLE_OBS_LOW_000_PANE_PLTUSER_00,
      PANENAME_BATTLE_BTN_LOW_003_PANE_BOUND_00
    },

    //  BUTTON_ID_SKILL_INFO_LEFT_DOWN,
    {
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_00_TOUCH,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_00_RELEASE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_00_CANSEL,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_00_TOUCH_RELEASE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_00_ACTIVE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_00_PASSIVE,

      PANENAME_BATTLE_OBS_LOW_000_PANE_BTNINFO_00,
      PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
    },

    //  右下のポケモン、わざインフォ、プレイヤー名 -> ID: 03
    //  BUTTON_ID_POKE_ICON_RIGHT_DOWN,
    {
      LA_OBSLOW_BATTLE_OBS_LOW_000__ICONPOKE_03_TOUCH,
      LA_OBSLOW_BATTLE_OBS_LOW_000__ICONPOKE_03_RELEASE,
      ::app::tool::ButtonManager::ANIMATION_NULL,
      LA_OBSLOW_BATTLE_OBS_LOW_000__ICONPOKE_03_TOUCH_RELEASE,
      ::app::tool::ButtonManager::ANIMATION_NULL,
      ::app::tool::ButtonManager::ANIMATION_NULL,

      PANENAME_BATTLE_OBS_LOW_000_PANE_ICONPOKE_03,
      PANENAME_COMMON_BTN_LOW_001_PANE_BOUND_00
    },

    //  BUTTON_ID_USER_NAME_RIGHT_DOWN,
    {
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_03_TOUCH,
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_03_RELEASE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_03_CANSEL,
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_03_TOUCH_RELEASE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_03_ACTIVE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_03_PASSIVE,

      PANENAME_BATTLE_OBS_LOW_000_PANE_PLTUSER_03,
      PANENAME_BATTLE_BTN_LOW_003_PANE_BOUND_00
    },

    //  BUTTON_ID_SKILL_INFO_RIGHT_DOWN,
    {
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_03_TOUCH,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_03_RELEASE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_03_CANSEL,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_03_TOUCH_RELEASE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_03_ACTIVE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_03_PASSIVE,

      PANENAME_BATTLE_OBS_LOW_000_PANE_BTNINFO_03,
      PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00
    },

    //  左上のポケモン、わざインフォ、プレイヤー名 -> ID: 02
    //  BUTTON_ID_POKE_ICON_LEFT_UP,
    {
      LA_OBSLOW_BATTLE_OBS_LOW_000__ICONPOKE_02_TOUCH,
      LA_OBSLOW_BATTLE_OBS_LOW_000__ICONPOKE_02_RELEASE,
      ::app::tool::ButtonManager::ANIMATION_NULL,
      LA_OBSLOW_BATTLE_OBS_LOW_000__ICONPOKE_02_TOUCH_RELEASE,
      ::app::tool::ButtonManager::ANIMATION_NULL,
      ::app::tool::ButtonManager::ANIMATION_NULL,

      PANENAME_BATTLE_OBS_LOW_000_PANE_ICONPOKE_02,
      PANENAME_COMMON_BTN_LOW_001_PANE_BOUND_00
    },

    //  BUTTON_ID_USER_NAME_LEFT_UP,
    {
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_02_TOUCH,
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_02_RELEASE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_02_CANSEL,
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_02_TOUCH_RELEASE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_02_ACTIVE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_02_PASSIVE,

      PANENAME_BATTLE_OBS_LOW_000_PANE_PLTUSER_02,
      PANENAME_BATTLE_BTN_LOW_003_PANE_BOUND_00
    },

    //  BUTTON_ID_SKILL_INFO_LEFT_UP,
    {
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_02_TOUCH,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_02_RELEASE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_02_CANSEL,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_02_TOUCH_RELEASE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_02_ACTIVE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_02_PASSIVE,

      PANENAME_BATTLE_OBS_LOW_000_PANE_BTNINFO_02,
      PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
    },

    //  右上のポケモン、わざインフォ、プレイヤー名 -> ID: 01
    //  BUTTON_ID_POKE_ICON_RIGHT_UP,
    {
      LA_OBSLOW_BATTLE_OBS_LOW_000__ICONPOKE_01_TOUCH,
      LA_OBSLOW_BATTLE_OBS_LOW_000__ICONPOKE_01_RELEASE,
      ::app::tool::ButtonManager::ANIMATION_NULL,
      LA_OBSLOW_BATTLE_OBS_LOW_000__ICONPOKE_01_TOUCH_RELEASE,
      ::app::tool::ButtonManager::ANIMATION_NULL,
      ::app::tool::ButtonManager::ANIMATION_NULL,

      PANENAME_BATTLE_OBS_LOW_000_PANE_ICONPOKE_01,
      PANENAME_COMMON_BTN_LOW_001_PANE_BOUND_00
    },

    //  BUTTON_ID_USER_NAME_RIGHT_UP,
    {
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_01_TOUCH,
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_01_RELEASE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_01_CANSEL,
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_01_TOUCH_RELEASE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_01_ACTIVE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__PLTUSER_01_PASSIVE,

      PANENAME_BATTLE_OBS_LOW_000_PANE_PLTUSER_01,
      PANENAME_BATTLE_BTN_LOW_003_PANE_BOUND_00
    },

    //  BUTTON_ID_SKILL_INFO_RIGHT_UP,
    {
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_01_TOUCH,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_01_RELEASE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_01_CANSEL,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_01_TOUCH_RELEASE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_01_ACTIVE,
      LA_OBSLOW_BATTLE_OBS_LOW_000__BTNINFO_01_PASSIVE,

      PANENAME_BATTLE_OBS_LOW_000_PANE_BTNINFO_01,
      PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
    }
  };

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUIWatchLowerDraw::BattleUIWatchLowerDraw( const INIT_PARAM& param ) :
    UIView( param.pUtilHeap, param.pUtilHeap->GetDeviceAllocator(), param.pUtilHeap->GetDeviceAllocator() )
  {
    GFL_ASSERT( param.pUtilHeap );
    GFL_ASSERT( param.pRenderingManager );
    GFL_ASSERT( param.pLytBuff );

    _Clear();

    m_initParam = param;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUIWatchLowerDraw::~BattleUIWatchLowerDraw()
  {
    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 初期化
    */
  //-------------------------------------------------------------------------
  void BattleUIWatchLowerDraw::Initialize()
  {
    _Initialize2D( m_initParam.pLytBuff );

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    gfl2::lyt::LytWk* pLayoutWork = UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN );

    //  画面の各ボタンペインポイントリスト取得
    {
      u32 loopNum = GFL_NELEMS( m_aCreateButtonDataTable );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_pBtnPartRootPaneList[ i ] = pLayoutWork->GetPartsPane( m_aCreateButtonDataTable[ i ].buttonPartPaneId );
        GFL_ASSERT( m_pBtnPartRootPaneList[ i ] );
      }
    }

    //  ボタン入力関連の設定
    {
      //  ボタン設定データ定義一覧
      const ::app::ui::ButtonInfoEx    aButtonPaneIdxList[] =
      {
        //  戻るボタン
        _CreateButtonData( BUTTON_ID_BACK ),

        //  左下のポケモン、わざインフォ、プレイヤー名
        _CreateButtonData( BUTTON_ID_POKE_ICON_LEFT_DOWN ),
        _CreateButtonData( BUTTON_ID_USER_NAME_LEFT_DOWN ),
        _CreateButtonData( BUTTON_ID_SKILL_INFO_LEFT_DOWN ),


        //  右下のポケモン、わざインフォ、プレイヤー名
        _CreateButtonData( BUTTON_ID_POKE_ICON_RIGHT_DOWN ),
        _CreateButtonData( BUTTON_ID_USER_NAME_RIGHT_DOWN ),
        _CreateButtonData( BUTTON_ID_SKILL_INFO_RIGHT_DOWN ),


        //  左上のポケモン、わざインフォ、プレイヤー名
        _CreateButtonData( BUTTON_ID_POKE_ICON_LEFT_UP ),
        _CreateButtonData( BUTTON_ID_USER_NAME_LEFT_UP ),
        _CreateButtonData( BUTTON_ID_SKILL_INFO_LEFT_UP ),


        //  右上のポケモン、わざインフォ、プレイヤー名
        _CreateButtonData( BUTTON_ID_POKE_ICON_RIGHT_UP ),
        _CreateButtonData( BUTTON_ID_USER_NAME_RIGHT_UP ),
        _CreateButtonData( BUTTON_ID_SKILL_INFO_RIGHT_UP ),
      };

      //  ボタン登録
      UIResponder::CreateButtonManager( m_initParam.pUtilHeap, pG2DUtil->GetLayoutWork( LYT_WK_TYPE_SCREEN ), aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );
    }

    //  ボタンのSE設定
    if( m_initParam.bUseInputSE )
    {
      ::app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();

      const s32 aSetupSeTable[ BUTTON_ID_MAX ][ 2 ] =
      {
        { BUTTON_ID_BACK,                  SEQ_SE_CANCEL1 },

        { BUTTON_ID_POKE_ICON_LEFT_DOWN,   SEQ_SE_DECIDE1 },
        { BUTTON_ID_USER_NAME_LEFT_DOWN,   SEQ_SE_DECIDE1 },
        { BUTTON_ID_SKILL_INFO_LEFT_DOWN,  SEQ_SE_DECIDE1 },

        { BUTTON_ID_POKE_ICON_RIGHT_DOWN,  SEQ_SE_DECIDE1 },
        { BUTTON_ID_USER_NAME_RIGHT_DOWN,  SEQ_SE_DECIDE1 },
        { BUTTON_ID_SKILL_INFO_RIGHT_DOWN, SEQ_SE_DECIDE1 },

        { BUTTON_ID_POKE_ICON_LEFT_UP,     SEQ_SE_DECIDE1 },
        { BUTTON_ID_USER_NAME_LEFT_UP,     SEQ_SE_DECIDE1 },
        { BUTTON_ID_SKILL_INFO_LEFT_UP,    SEQ_SE_DECIDE1 },

        { BUTTON_ID_POKE_ICON_RIGHT_UP,    SEQ_SE_DECIDE1 },
        { BUTTON_ID_USER_NAME_RIGHT_UP,    SEQ_SE_DECIDE1 },
        { BUTTON_ID_SKILL_INFO_RIGHT_UP,   SEQ_SE_DECIDE1 },
      };

      for( s32 i = 0; i < BUTTON_ID_MAX; ++i )
      {
        pButtonManager->SetButtonSelectSE( aSetupSeTable[ i ][ 0 ], aSetupSeTable[ i ][ 1 ] );
      }
    }

    //  ポケモンデータ作成
    {
      POKE_DATA*  pPokeData         = NULL;

      GFL_ASSERT( GFL_NELEMS( m_initParam.aPokeData ) == POKE_ICON_POS_TYPE_MAX );

      for( s32 i = 0; i < POKE_ICON_POS_TYPE_MAX; ++i )
      {
        pPokeData = &m_initParam.aPokeData[ i ];

        //  ポケモンデータ作成
        _CreatePokeData( i, pPokeData );
      }
    }

    //  入力コールバック設定
    UIView::SetInputListener( this );

    //  Start関数が呼ばれるまで非表示
    UIView::SetVisible( false );

    _SetScreenType( m_initParam.screenType );
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 更新処理
    */
  //-------------------------------------------------------------------------
  void BattleUIWatchLowerDraw::Update( void )
  {
    _UpdateState();

    {
      u32 loopNum = GFL_NELEMS( m_aPokeObj );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_aPokeObj[ i ].Update();
      }
    }

    UIView::Update();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 描画
    */
  //-------------------------------------------------------------------------
  void BattleUIWatchLowerDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    for( s32 i = 0; i < LYT_WK_TYPE_MAX; ++i )
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderingManager, displayNo, i, LOW_VIEW_ORDER_OTHER );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   ボタン項目表示設定
    */
  //------------------------------------------------------------------
  void BattleUIWatchLowerDraw::SetVisibleButton( ButtonId buttonId, bool bVisible )
  {
    if( bVisible )
    {
      UIResponder::GetButtonManager()->SetButtonVisible( buttonId );
    }
    else
    {
      UIResponder::GetButtonManager()->SetButtonInvisible( (s32)buttonId );
    }

    UIResponder::SetButtonEnable( (s32)buttonId, bVisible );
  }

  //------------------------------------------------------------------
  /**
    * @brief   画面タイプ設定
    */
  //------------------------------------------------------------------
  void BattleUIWatchLowerDraw::_SetScreenType( const ScreenType newType )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    const s32 aSetAnimeIdx[ SCREEN_TYPE_MAX ] =
    {
      LA_OBSLOW_BATTLE_OBS_LOW_000_BATTLE_NORMAL,
      LA_OBSLOW_BATTLE_OBS_LOW_000_BATTLE_DOUBLE,
      LA_OBSLOW_BATTLE_OBS_LOW_000_BATTLE_ROYAL
    };

    pG2DUtil->StartAnimeFrame( LYT_WK_TYPE_SCREEN, aSetAnimeIdx[ newType ], 0, true, true );
  }

  //------------------------------------------------------------------
  /**
    * @brief   プレイヤー名のテキスト設定
    */
  //------------------------------------------------------------------
  void BattleUIWatchLowerDraw::SetPlayerNameText( gfl2::str::StrBuf* pPlayerNameArray, const s32 arrayNum )
  {
    GFL_ASSERT( arrayNum < GFL_NELEMS( m_pPlayerNameTextBoxPaneArray ) );

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    const gfl2::str::STRCODE* pBlankText  = L"";

    if( pPlayerNameArray )
    {
      GFL_ASSERT( 0 < arrayNum );

      for( s32 i = 0; i < GFL_NELEMS( m_pPlayerNameTextBoxPaneArray ); ++i )
      {
        if( m_pPlayerNameTextBoxPaneArray[ i ] )
        {
          if( i < arrayNum )
          {
            pG2DUtil->SetTextBoxPaneString( m_pPlayerNameTextBoxPaneArray[ i ], &( pPlayerNameArray[ i ] ) );
          }
          else
          {
            pG2DUtil->SetTextBoxPaneString( m_pPlayerNameTextBoxPaneArray[ i ], pBlankText );
          }
        }
      }
    }
    else
    {
      for( s32 i = 0; i < GFL_NELEMS( m_pPlayerNameTextBoxPaneArray ); ++i )
      {
        if( m_pPlayerNameTextBoxPaneArray[ i ] )
        {
          pG2DUtil->SetTextBoxPaneString( m_pPlayerNameTextBoxPaneArray[ i ], pBlankText );
        }
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   各ボタンの登録データ作成
    * @param   createButtonType  : 作成するボタンデータのタイプ
    * @param   buttonId          : ボタンID
    */
  //------------------------------------------------------------------
  ::app::ui::ButtonInfoEx BattleUIWatchLowerDraw::_CreateButtonData( ButtonId createButtonType )
  {
    ::app::ui::ButtonInfoEx data;

    data.button_id     = createButtonType;

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    gfl2::lyt::LytWk* pLayoutWork   = UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN );

    //  使用しているResID
    gfl2::lyt::LytMultiResID& rLytMultiResId  = pG2DUtil->GetLayoutResourceID( 0 );

    const BUTTON_CREATE_DATA* pCreateData = &m_aCreateButtonDataTable[ createButtonType ];

    data.touch_anime_index       = pCreateData->touch_anime_index;
    data.release_anime_index     = pCreateData->release_anime_index;
    data.cancel_anime_index      = pCreateData->cancel_anime_index;
    data.key_select_anime_index  = pCreateData->key_select_anime_index;
    data.active_anime_index      = pCreateData->active_anime_index;
    data.passive_anime_index     = pCreateData->passive_anime_index;

    data.picture_pane = m_pBtnPartRootPaneList[ createButtonType ];
    data.bound_pane   = pLayoutWork->GetBoundingPane( m_pBtnPartRootPaneList[ createButtonType ], pCreateData->partsBoundPaneId, &rLytMultiResId );

    data.picture_pane = m_pBtnPartRootPaneList[ createButtonType ];

    GFL_ASSERT( data.picture_pane );
    GFL_ASSERT( data.bound_pane );

    return  data;
  }

  //------------------------------------------------------------------
  /**
    * @brief   ポケモンデータ作成
    */
  //------------------------------------------------------------------
  BattleUIWatchLowerDraw::PokeObj* BattleUIWatchLowerDraw::_CreatePokeData( const s32 pokeDataIdx, const POKE_DATA* pPokeData )
  {
    GFL_ASSERT( pokeDataIdx < GFL_NELEMS( m_aPokeObj ) );
    GFL_ASSERT( pPokeData );

    //  ポケモンアイコンパーツペイン一覧
    const PokeObj::INIT_PARAM  aIniteData[ POKE_ICON_POS_TYPE_MAX ]  =
    {
      //  左下のアイコン
      {
        POKE_ICON_POS_TYPE_LEFT_DOWN,
        m_pBtnPartRootPaneList[ BUTTON_ID_POKE_ICON_LEFT_DOWN ],

        BUTTON_ID_POKE_ICON_LEFT_DOWN,
        BUTTON_ID_SKILL_INFO_LEFT_DOWN,

        PANENAME_BATTLE_OBS_LOW_000_PANE_BTNINFO_04,
        PANENAME_BATTLE_OBS_LOW_000_PANE_BG_03,
        PANENAME_BATTLE_OBS_LOW_000_PANE_TEXTBOX_00,

        PANENAME_BATTLE_OBS_LOW_000_PANE_PLTUSER_00,

        {
          LA_OBSLOW_BATTLE_OBS_LOW_000__ICONPOKE_00_DIE,
          LA_OBSLOW_BATTLE_OBS_LOW_000__ICONPOKE_00_REVIVE
        }
      },

      //  右下のアイコン
      {
        POKE_ICON_POS_TYPE_RIGHT_DOWN,
        m_pBtnPartRootPaneList[ BUTTON_ID_POKE_ICON_RIGHT_DOWN ],

        BUTTON_ID_POKE_ICON_RIGHT_DOWN,
        BUTTON_ID_SKILL_INFO_RIGHT_DOWN,

        PANENAME_BATTLE_OBS_LOW_000_PANE_BTNINFO_07,
        PANENAME_BATTLE_OBS_LOW_000_PANE_BG_06,
        PANENAME_BATTLE_OBS_LOW_000_PANE_TEXTBOX_03,

        PANENAME_BATTLE_OBS_LOW_000_PANE_PLTUSER_03,

        {
          LA_OBSLOW_BATTLE_OBS_LOW_000__ICONPOKE_03_DIE,
          LA_OBSLOW_BATTLE_OBS_LOW_000__ICONPOKE_03_REVIVE
        }
      },

      //  左上のアイコン
      {
        POKE_ICON_POS_TYPE_LEFT_UP,
        m_pBtnPartRootPaneList[ BUTTON_ID_POKE_ICON_LEFT_UP ],

        BUTTON_ID_POKE_ICON_LEFT_UP,
        BUTTON_ID_SKILL_INFO_LEFT_UP,

        PANENAME_BATTLE_OBS_LOW_000_PANE_BTNINFO_06,
        PANENAME_BATTLE_OBS_LOW_000_PANE_BG_04,
        PANENAME_BATTLE_OBS_LOW_000_PANE_TEXTBOX_02,

        PANENAME_BATTLE_OBS_LOW_000_PANE_PLTUSER_02,

        {
          LA_OBSLOW_BATTLE_OBS_LOW_000__ICONPOKE_02_DIE,
          LA_OBSLOW_BATTLE_OBS_LOW_000__ICONPOKE_02_REVIVE
        }
      },

      //  右上のアイコン
      {
        POKE_ICON_POS_TYPE_RIGHT_UP,
        m_pBtnPartRootPaneList[ BUTTON_ID_POKE_ICON_RIGHT_UP ],

        BUTTON_ID_POKE_ICON_RIGHT_UP,
        BUTTON_ID_SKILL_INFO_RIGHT_UP,

        PANENAME_BATTLE_OBS_LOW_000_PANE_BTNINFO_05,
        PANENAME_BATTLE_OBS_LOW_000_PANE_BG_05,
        PANENAME_BATTLE_OBS_LOW_000_PANE_TEXTBOX_01,

        PANENAME_BATTLE_OBS_LOW_000_PANE_PLTUSER_01,

        {
          LA_OBSLOW_BATTLE_OBS_LOW_000__ICONPOKE_01_DIE,
          LA_OBSLOW_BATTLE_OBS_LOW_000__ICONPOKE_01_REVIVE
        }
      }
    };

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

    PokeObj*  pPokeObj  = &m_aPokeObj[ pokeDataIdx ];
    pPokeObj->Initialize( this, m_initParam.pUtilHeap, LYT_WK_TYPE_SCREEN, &rMultiResId, aIniteData[ pokeDataIdx ], pPokeData );

    return  pPokeObj;
  }

  /**
    * @brief レイアウトワークを生成する
    */
  void BattleUIWatchLowerDraw::_Initialize2D( void* pAppLytBuff )
  {
    //  2D画面作成
    {
      // アーカイブIDテーブル
      const ::app::util::G2DUtil::LytResData resTbl[] =
      {
        {  pAppLytBuff, 32, ::app::util::G2DUtil::ATTACH_NONE, },
      };

      //  作成するレイアウトワーク
      const ::app::util::G2DUtil::LytwkData setupData[ LYT_WK_TYPE_MAX ] =
      {
          //  選択画面
          {
            0,
            LYTRES_OBSLOW_BATTLE_OBS_LOW_000_BFLYT,
            LA_OBSLOW_BATTLE_OBS_LOW_000___NUM,
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
  void BattleUIWatchLowerDraw::_UpdateState()
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    if( m_state == STATE_TYPE_NONE )
    {
    }
    else if( m_state == STATE_TYPE_IN )
    {
      //  アニメの終了を検知
      if( pG2DUtil->IsAnimeEnd( LYT_WK_TYPE_SCREEN, LA_OBSLOW_BATTLE_OBS_LOW_000_IN ) )
      {
        _SetState( STATE_TYPE_IDLE );
      }
    }
    else if( m_state == STATE_TYPE_IDLE )
    {
    }
    else if( m_state == STATE_TYPE_OUT )
    {
      _SetState( STATE_TYPE_END );
    }
  }


  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void BattleUIWatchLowerDraw::_SetState( const BattleUIWatchLowerDraw::StateType newStateType )
  {
    GFL_ASSERT( m_state != newStateType );

    m_state = newStateType;

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    if( m_state == STATE_TYPE_NONE )
    {
    }
    else if( m_state == STATE_TYPE_IN )
    {
      UIView::SetVisible( true );

      //  画面が開くSE
      Sound::PlaySE( SEQ_SE_OPEN2 );

      //  開始アニメをする
      pG2DUtil->StartAnimeFrame( LYT_WK_TYPE_SCREEN, LA_OBSLOW_BATTLE_OBS_LOW_000_IN, 0, true, true ); 
    }
    else if( m_state == STATE_TYPE_IDLE )
    {
    }
    else if( m_state == STATE_TYPE_ENTER_ANIM_WAIT )
    {
      
    }
    else if( m_state == STATE_TYPE_OUT )
    {
    }
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
  ::app::ui::UIInputListener::ListenerResult BattleUIWatchLowerDraw::OnLayoutPaneEvent( const u32 buttonId )
  {
    GFL_PRINT( "OnLayoutPaneEvent : painId = %d\n", buttonId );

    if( m_initParam.pDelegate == NULL )
    {
      return  ENABLE_ACCESS;
    }

    if( m_initParam.pDelegate->OnEnterInput( this, buttonId ) )
    {
      return DISABLE_ACCESS;
    }

    return ENABLE_ACCESS;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインタッチ時を検知
  * @note 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
  */
  //--------------------------------------------------------------------------------------------
  // overwride
  void BattleUIWatchLowerDraw::OnLayoutPaneTouchTriggerEvent( const u32 buttonId )
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief   キー入力の検知
    *
    * @param  pButton ボタン
    * @param  pKey    十字キー
    * @param  pStick  アナログスティック
    *
    * @return 同フレーム内での他入力イベントを許可するならtrue、
    *         他のイベントを排除するならfalseを返却すること。
    */
  //------------------------------------------------------------------
  // overwride
  ::app::ui::UIInputListener::ListenerResult BattleUIWatchLowerDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    if( m_initParam.pDelegate == NULL ){
      return ENABLE_ACCESS;
    }

    if( m_initParam.pDelegate->OnKeyAction( this, pButton, pKey, pStick ) )
    {
      return  DISABLE_ACCESS;
    }

    return ENABLE_ACCESS;
  }

//  ここからは PokeObjクラス定義

  void BattleUIWatchLowerDraw::PokeObj::Initialize( UIView* pView, ::app::util::Heap* pUtilHeap, const u32 lytId, gfl2::lyt::LytMultiResID* pMultiResId, const INIT_PARAM& rInitParam, const POKE_DATA* pPokeData )
  {
    GFL_ASSERT( pView );
    GFL_ASSERT( pPokeData );
    GFL_ASSERT( pUtilHeap );

    bool  bPokeUse  = ( pPokeData->p_btl_poke_param != NULL );

    ::app::util::G2DUtil* pG2DUtil  = pView->GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    gfl2::lyt::LytWk* pLayoutWork = pView->GetLayoutWork( lytId );

    //  ポケモンアイコン作成
    ::app::tool::PokeIconToolEx*  pPokeIconTool = NULL;
    if( bPokeUse == true )
    {
      ::app::tool::PokeIconToolEx::PokeIconToolParam  initParam;
      initParam.layout_id = LYT_WK_TYPE_SCREEN;
      initParam.pHeap     = pUtilHeap;
      initParam.pUIView   = pView;
      initParam.res_id    = 0;
      initParam.iconMax   = 1;
      initParam.sync      = false;

      pPokeIconTool = GFL_NEW( pUtilHeap->GetDeviceHeap() ) ::app::tool::PokeIconToolEx( initParam );
      GFL_ASSERT( pPokeIconTool );

      pPokeIconTool->SetPokeIcon( 0, pPokeData->p_btl_poke_param->GetSrcData(), PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON, rInitParam.p_poke_icon_pane );
    }

    //  アニメidの設定
    {
      for( s32 i = 0; i < POKE_ICON_ANIME_MAX; ++i )
      {
        m_aAnimeIdx[ i ]  = rInitParam.a_anime_id[ i ];
      }
    }

    //  わざ項目
    {
      m_pWazaNamePane = pLayoutWork->GetTextBoxPane( rInitParam.waza_name_pane_id );
      GFL_ASSERT( m_pWazaNamePane );

      m_pWazaNameRootPane = pLayoutWork->GetPane( rInitParam.waza_name_root_pane_id );
      GFL_ASSERT( m_pWazaNameRootPane );
    }

    m_pIcon           = pPokeIconTool;
    m_pInfoIconPane   = pLayoutWork->GetPartsPane( rInitParam.info_icon_parts_pane_id );
    m_baseData        = *pPokeData;

    //  ポケモンデータがないのであれば、オブジェクトを使えないようにする
    if( bPokeUse == true )
    {
      //  インフォアイコンはパラメータを参照して出す
      {
        bool bInfoIconVisible  = false;
        if( 0 < pPokeData->p_btl_poke_status->statusNum )
        {
          bInfoIconVisible  = true;
        }

        if( m_pInfoIconPane != NULL )
        {
          pG2DUtil->SetPaneVisible( m_pInfoIconPane, bInfoIconVisible );
        }
      }

      //  最後に使用したわざがあれば技表記する
      {
        bool bWazaVisible  = false;
        if( pPokeData->last_waza_data.normal_waza_id != WAZANO_NULL )
        {
          bWazaVisible  = true;

          pG2DUtil->SetRegisterWazaName( 0, pPokeData->last_waza_data.normal_waza_id );
          pG2DUtil->SetTextBoxPaneStringExpand( m_pWazaNamePane, STR_WAZA_NAME );
        }
        else
        {
          //  アイコン表示していないのはタッチさせない
          pView->SetButtonEnable( rInitParam.waza_info_icon_buffon_id, false );
        }

        pG2DUtil->SetPaneVisible( m_pWazaNameRootPane, bWazaVisible );
      }

      //  プレイヤー名設定
      {
        gfl2::lyt::LytParts*  pPlayerPartsPane  = pLayoutWork->GetPartsPane( rInitParam.player_name_parts_pane_id );
        GFL_ASSERT( pPlayerPartsPane );

        gfl2::lyt::LytPane* pShadowLPane  = pLayoutWork->GetPane( pPlayerPartsPane, PANENAME_BATTLE_BTN_LOW_003_PANE_WINSHADOW_L, pMultiResId );
        GFL_ASSERT( pShadowLPane );

        gfl2::lyt::LytPane* pShadowRPane  = pLayoutWork->GetPane( pPlayerPartsPane, PANENAME_BATTLE_BTN_LOW_003_PANE_WINSHADOW_R, pMultiResId );
        GFL_ASSERT( pShadowRPane );

        gfl2::lyt::LytPane* pWindowMainLPane  = pLayoutWork->GetPane( pPlayerPartsPane, PANENAME_BATTLE_BTN_LOW_003_PANE_WINMAIN_L, pMultiResId );
        GFL_ASSERT( pWindowMainLPane );

        gfl2::lyt::LytPane* pWindowMainRPane  = pLayoutWork->GetPane( pPlayerPartsPane, PANENAME_BATTLE_BTN_LOW_003_PANE_WINMAIN_R, pMultiResId );
        GFL_ASSERT( pWindowMainRPane );

        gfl2::lyt::LytTextBox* pTextNameLPane  = pLayoutWork->GetTextBoxPane( pPlayerPartsPane, PANENAME_BATTLE_BTN_LOW_003_PANE_TEXTBOX_L, pMultiResId );
        GFL_ASSERT( pTextNameLPane );

        gfl2::lyt::LytTextBox* pTextNameRPane  = pLayoutWork->GetTextBoxPane( pPlayerPartsPane, PANENAME_BATTLE_BTN_LOW_003_PANE_TEXTBOX_R, pMultiResId );
        GFL_ASSERT( pTextNameRPane );

        gfl2::lyt::LytPane*     pActiveShadowPane     = NULL;
        gfl2::lyt::LytPane*     pActiveWindowMainPane = NULL;
        gfl2::lyt::LytTextBox*  pActiveTextBoxPane    = NULL;

        if( ( rInitParam.pos_type == POKE_ICON_POS_TYPE_LEFT_DOWN ) ||
            ( rInitParam.pos_type == POKE_ICON_POS_TYPE_LEFT_UP ) )
        {
          pG2DUtil->SetPaneVisible( pShadowRPane, false );
          pG2DUtil->SetPaneVisible( pWindowMainRPane, false );
          pG2DUtil->SetPaneVisible( pTextNameRPane, false );

          pActiveShadowPane     = pShadowLPane;
          pActiveWindowMainPane = pWindowMainLPane;
          pActiveTextBoxPane    = pTextNameLPane;
        }
        else if( ( rInitParam.pos_type == POKE_ICON_POS_TYPE_RIGHT_DOWN ) ||
                 ( rInitParam.pos_type == POKE_ICON_POS_TYPE_RIGHT_UP ) )
        {
          pG2DUtil->SetPaneVisible( pShadowLPane, false );
          pG2DUtil->SetPaneVisible( pWindowMainLPane, false );
          pG2DUtil->SetPaneVisible( pTextNameLPane, false );

          pActiveShadowPane     = pShadowRPane;
          pActiveWindowMainPane = pWindowMainRPane;
          pActiveTextBoxPane    = pTextNameRPane;
        }
        else
        {
          GFL_ASSERT( 0 );
        }

         GFL_ASSERT( pActiveShadowPane );
         GFL_ASSERT( pActiveWindowMainPane );
         GFL_ASSERT( pActiveTextBoxPane );
         GFL_ASSERT( pPokeData->p_player_name );

         // 表示設定
         pG2DUtil->SetPaneVisible( pActiveShadowPane, true );
         pG2DUtil->SetPaneVisible( pActiveWindowMainPane, true );
         pG2DUtil->SetPaneVisible( pActiveTextBoxPane, true );

         // プレイヤー名の長さでフレームの横サイズを可変に
         u32 playerNameTextLength  = pPokeData->p_player_name->GetLength();
         if( 0 < playerNameTextLength )
         {
            u32 defaultTextLenght     = (u32)pActiveTextBoxPane->GetStringLength();

            const f32 inp = (f32)playerNameTextLength / (f32)defaultTextLenght;

            //  フレーム影のサイズ
            {
              nw::lyt::Size size  = pActiveShadowPane->GetSize();
              size.width  = size.width * inp;

              pActiveShadowPane->SetSize( size );
            }

            //  フレームのサイズ
            {
              nw::lyt::Size size  = pActiveWindowMainPane->GetSize();
              size.width  = size.width * inp;

              pActiveWindowMainPane->SetSize( size );
            }
         }

         // プレイヤー名設定
         {
            pG2DUtil->SetRegisterWord( 0, *pPokeData->p_player_name );
            pG2DUtil->SetTextBoxPaneStringExpand( pActiveTextBoxPane, STR_TRAINER_NAME );
         }
      }
    }
    else
    {
      //  アイコン表示していないのは関連するボタンはタッチさせない
      pView->SetButtonEnable( rInitParam.waza_info_icon_buffon_id, false );
      pView->SetButtonEnable( rInitParam.poke_icon_button_id, false );
    }
  }

  //  破棄完了か
  bool BattleUIWatchLowerDraw::PokeObj::IsDelete()
  {
    bool  bRet  = true;
    if( m_pIcon != NULL )
    {
      if( m_pIcon->EndFunc() == false )
      {
        bRet  = false;
      }
    }

    return  bRet;
  }

  //  更新
  void BattleUIWatchLowerDraw::PokeObj::Update()
  {
    if( m_pIcon != NULL )
    {
      m_pIcon->Update();
    }
  }

  //  ポケモンアイコンアニメ再生
  void BattleUIWatchLowerDraw::PokeObj::StartAnimePokeIcon( const PokeIconAnime animeIdx, const bool bForcePlayAnime )
  {
    if( m_pIcon == NULL )
    {
      return;
    }

    if( ( m_playAnimeIdx != m_aAnimeIdx[ animeIdx ] ) || bForcePlayAnime )
    {
      m_playAnimeIdx = m_aAnimeIdx[ animeIdx ];
      m_pIcon->StartAnim( m_playAnimeIdx );
    }
  }

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )
