//=============================================================================
/**
 * @file BattleUIActSelectLowerDraw.cpp
 * @brief バトル下画面 >> 「行動選択画面」画面
 * @author yuto_uchida
 * @date 2015.03.10
 */
//=============================================================================
#include "BattleUIActSelectLowerDraw.h"
#include "BattleUIViewDrawOrder.h"

#include "niji_conv_header/app/battle/res2d/minlow.h"
#include "niji_conv_header/app/battle/res2d/minlow_pane.h"
#include "niji_conv_header/app/battle/res2d/minlow_anim_list.h"
#include "niji_conv_header/sound/SoundMiddleID.h"

//  nijiのインクルード
#include "AppLib/include/Tool/CommonMessageWindowManipulator.h"

//  バトル用のappテキストのインクルード
#include "niji_conv_header/message/msg_btl_app.h"

//  ポケモンが味方か敵かの判別定義がある
#include "Battle/source/btlv/BtlvUIPokeStatusFrame.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  //  カーソル設定データテーブル
  const BattleUIActSelectLowerDraw::CURSOR_CONFIG_DATA BattleUIActSelectLowerDraw::m_saCursorConfigDataTable[ CURSOR_POS_TYPE_MAX ]  =
  {
    { CURSOR_POS_TYPE_FIGHT,      BUTTON_ID_FIGHT },
    { CURSOR_POS_TYPE_POKE,       BUTTON_ID_POKE  },
    { CURSOR_POS_TYPE_BAG,        BUTTON_ID_BAG   },
    { CURSOR_POS_TYPE_RUN,        BUTTON_ID_RUN   }
  };

  //  ボタン作成データテーブル
  const BattleUIActSelectLowerDraw::BUTTON_CREATE_DATA BattleUIActSelectLowerDraw::m_saCreateButtonOrgDataTable[ BUTTON_ID_MAX ]  =
  {
    //  BUTTON_ID_FIGHT,
    {
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_FIGHT_TOUCH,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_FIGHT_RELEASE,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_FIGHT_CANSEL,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_FIGHT_TOUCH_RELEASE,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_FIGHT_ACTIVE,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_FIGHT_PASSIVE,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_FIGHT_SELECT_LOOP,

      PANENAME_BATTLE_MIN_LOW_000_PANE_BTN_FIGHT,
      PANENAME_BATTLE_BTN_LOW_000_PANE_BOUND_00
    },

    //  BUTTON_ID_POKE( m_initParam.drawMode == DRAW_MODE_NET では別途データを指定 ),
    {
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_POKEMON_00_TOUCH,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_POKEMON_00_RELEASE,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_POKEMON_00_CANSEL,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_POKEMON_00_TOUCH_RELEASE,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_POKEMON_00_ACTIVE,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_POKEMON_00_PASSIVE,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_POKEMON_00_SELECT_LOOP,

      PANENAME_BATTLE_MIN_LOW_000_PANE_BTN_POKEMON_00,
      PANENAME_BATTLE_BTN_LOW_000_PANE_BOUND_00
    },

    //  BUTTON_ID_BAG,
    {
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_BAG_TOUCH,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_BAG_RELEASE,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_BAG_CANSEL,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_BAG_TOUCH_RELEASE,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_BAG_ACTIVE,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_BAG_PASSIVE,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_BAG_SELECT_LOOP,

      PANENAME_BATTLE_MIN_LOW_000_PANE_BTN_BAG,
      PANENAME_BATTLE_BTN_LOW_000_PANE_BOUND_00
    },

    //  BUTTON_ID_RUN,
    {
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_RUN_TOUCH,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_RUN_RELEASE,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_RUN_CANSEL,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_RUN_TOUCH_RELEASE,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_RUN_ACTIVE,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_RUN_PASSIVE,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_RUN_SELECT_LOOP,

      PANENAME_BATTLE_MIN_LOW_000_PANE_BTN_RUN,
      PANENAME_BATTLE_BTN_LOW_000_PANE_BOUND_00
    },

    //  BUTTON_ID_BACK,
    {
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_BACK_TOUCH,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_BACK_RELEASE,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_BACK_CANSEL,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_BACK_TOUCH_RELEASE,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_BACK_ACTIVE,
      LA_MINLOW_BATTLE_MIN_LOW_000__BTN_BACK_PASSIVE,
      ::app::tool::ButtonManager::ANIMATION_NULL,

      PANENAME_BATTLE_MIN_LOW_000_PANE_BTN_BACK,
      PANENAME_BATTLE_BTN_LOW_000_PANE_BOUND_00
    },

    //  BUTTON_ID_BALL,
    {
      LA_MINLOW_BATTLE_MIN_LOW_000__PARTS_BALL_TOUCH,
      LA_MINLOW_BATTLE_MIN_LOW_000__PARTS_BALL_RELEASE,
      LA_MINLOW_BATTLE_MIN_LOW_000__PARTS_BALL_CANSEL,
      LA_MINLOW_BATTLE_MIN_LOW_000__PARTS_BALL_TOUCH_RELEASE,
      ::app::tool::ButtonManager::ANIMATION_NULL,
      ::app::tool::ButtonManager::ANIMATION_NULL,
      ::app::tool::ButtonManager::ANIMATION_NULL,

      PANENAME_BATTLE_MIN_LOW_000_PANE_PARTS_BALL,
      PANENAME_BATTLE_BTN_LOW_004_PANE_BOUND_00
    },

    //  BUTTON_ID_POKE_ICON_LEFT_DOWN,
    {
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_LB_TOUCH,
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_LB_RELEASE,
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_LB_CANSEL,
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_LB_TOUCH_RELEASE,
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_LB_ACTIVE,
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_LB_PASSIVE,
      ::app::tool::ButtonManager::ANIMATION_NULL,

      PANENAME_BATTLE_MIN_LOW_000_PANE_POKE_LB,
      PANENAME_BATTLE_BTN_LOW_007_PANE_BOUND_00
    },

    //  BUTTON_ID_POKE_ICON_RIGHT_DOWN,
    {
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_RB_TOUCH,
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_RB_RELEASE,
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_RB_CANSEL,
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_RB_TOUCH_RELEASE,
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_RB_ACTIVE,
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_RB_PASSIVE,
      ::app::tool::ButtonManager::ANIMATION_NULL,

      PANENAME_BATTLE_MIN_LOW_000_PANE_POKE_RB,
      PANENAME_BATTLE_BTN_LOW_007_PANE_BOUND_00
    },

    //  BUTTON_ID_POKE_ICON_LEFT_UP,
    {
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_LT_TOUCH,
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_LT_RELEASE,
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_LT_CANSEL,
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_LT_TOUCH_RELEASE,
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_LT_ACTIVE,
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_LT_PASSIVE,
      ::app::tool::ButtonManager::ANIMATION_NULL,

      PANENAME_BATTLE_MIN_LOW_000_PANE_POKE_LT,
      PANENAME_BATTLE_BTN_LOW_007_PANE_BOUND_00
    },

    //  BUTTON_ID_POKE_ICON_RIGHT_UP,
    {
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_RT_TOUCH,
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_RT_RELEASE,
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_RT_CANSEL,
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_RT_TOUCH_RELEASE,
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_RT_ACTIVE,
      LA_MINLOW_BATTLE_MIN_LOW_000__POKE_RT_PASSIVE,
      ::app::tool::ButtonManager::ANIMATION_NULL,

      PANENAME_BATTLE_MIN_LOW_000_PANE_POKE_RT,
      PANENAME_BATTLE_BTN_LOW_007_PANE_BOUND_00
    },

    //  BUTTON_ID_USER_NAME,
    {
      LA_MINLOW_BATTLE_MIN_LOW_000__PARTSUSER_00_TOUCH,
      LA_MINLOW_BATTLE_MIN_LOW_000__PARTSUSER_00_RELEASE,
      LA_MINLOW_BATTLE_MIN_LOW_000__PARTSUSER_00_CANSEL,
      LA_MINLOW_BATTLE_MIN_LOW_000__PARTSUSER_00_TOUCH_RELEASE,
      ::app::tool::ButtonManager::ANIMATION_NULL,
      ::app::tool::ButtonManager::ANIMATION_NULL,
      ::app::tool::ButtonManager::ANIMATION_NULL,

      PANENAME_BATTLE_MIN_LOW_000_PANE_PARTSUSER_00,
      PANENAME_BATTLE_BTN_LOW_000_PANE_BOUND_00
    },
  };

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUIActSelectLowerDraw::BattleUIActSelectLowerDraw( const INIT_PARAM& param ) :
    UIView( param.pUtilHeap, param.pUtilHeap->GetDeviceAllocator(), param.pUtilHeap->GetDeviceAllocator() )
  {
    GFL_ASSERT( param.pUtilHeap  != NULL );
    GFL_ASSERT( param.pRenderingManager  != NULL );
    GFL_ASSERT( param.pLytBuff  != NULL );
    GFL_ASSERT( param.pMenuCursorLytBuff != NULL );
    GFL_ASSERT( param.pBtlAppMsgData  != NULL );
    GFL_ASSERT( param.pBtlWordSet != NULL );

    _Clear();

    m_initParam = param;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUIActSelectLowerDraw::~BattleUIActSelectLowerDraw()
  {
    GFL_SAFE_DELETE( m_pBallObj );
    GFL_SAFE_DELETE( m_pMenuCursor );

    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 初期化
    */
  //-------------------------------------------------------------------------
  void BattleUIActSelectLowerDraw::Initialize()
  {
    _Initialize2D( m_initParam.pLytBuff );

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    gfl2::lyt::LytWk* pLayoutWork = UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN );

    gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

    //  カーソルオブジェクト作成
    {
      //  すでにリソースバッファを作成して持っているのでロードは不要
      m_pMenuCursor = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) ::app::tool::MenuCursor( m_initParam.pUtilHeap, &m_initParam.pMenuCursorLytBuff );
      GFL_ASSERT( m_pMenuCursor );

      m_pMenuCursor->CreateLayoutWork( UIView::GetLayoutSystem(), UIView::GetLytSetup(gfl2::lyt::DISPLAY_LOWER), pG2DUtil->GetAppLytAccessor(), ::app::tool::MenuCursor::SIZE_BTL );

      /*  momiji対応  */
      m_pMenuCursor->SetVisible(false);
      
      pG2DUtil->StartAnime(0,  LA_MINLOW_BATTLE_MIN_LOW_000__BTN_POKEMON_00_SELECT_LOOP);
      pG2DUtil->StartAnime(0,  LA_MINLOW_BATTLE_MIN_LOW_000__BTN_BAG_SELECT_LOOP);
      pG2DUtil->StartAnime(0,  LA_MINLOW_BATTLE_MIN_LOW_000__BTN_FIGHT_SELECT_LOOP);
      pG2DUtil->StartAnime(0,  LA_MINLOW_BATTLE_MIN_LOW_000__BTN_RUN_SELECT_LOOP);
    }

    //  元のボタン作成データをコピーする
    {
      ::std::memcpy( m_aCreateButtonDataTable, m_saCreateButtonOrgDataTable, sizeof( m_saCreateButtonOrgDataTable ) );
    }

    if (m_initParam.drawMode == DRAW_MODE_NET || m_initParam.drawMode == DRAW_MODE_BTL_FACLIITY)
    {
      m_aCreateButtonDataTable[BUTTON_ID_POKE].touch_anime_index = LA_MINLOW_BATTLE_MIN_LOW_000__BTN_POKEMON_01_TOUCH;
      m_aCreateButtonDataTable[BUTTON_ID_POKE].release_anime_index = LA_MINLOW_BATTLE_MIN_LOW_000__BTN_POKEMON_01_RELEASE;
      m_aCreateButtonDataTable[BUTTON_ID_POKE].cancel_anime_index = LA_MINLOW_BATTLE_MIN_LOW_000__BTN_POKEMON_01_CANSEL;
      m_aCreateButtonDataTable[BUTTON_ID_POKE].key_select_anime_index = LA_MINLOW_BATTLE_MIN_LOW_000__BTN_POKEMON_01_TOUCH_RELEASE;
      m_aCreateButtonDataTable[BUTTON_ID_POKE].active_anime_index = LA_MINLOW_BATTLE_MIN_LOW_000__BTN_POKEMON_01_ACTIVE;
      m_aCreateButtonDataTable[BUTTON_ID_POKE].passive_anime_index = LA_MINLOW_BATTLE_MIN_LOW_000__BTN_POKEMON_01_PASSIVE;

      m_aCreateButtonDataTable[BUTTON_ID_POKE].selectLoopAnimeId = LA_MINLOW_BATTLE_MIN_LOW_000__BTN_POKEMON_01_SELECT_LOOP;

      m_aCreateButtonDataTable[BUTTON_ID_POKE].buttonPartPaneId = PANENAME_BATTLE_MIN_LOW_000_PANE_BTN_POKEMON_01;
      m_aCreateButtonDataTable[BUTTON_ID_POKE].partsBoundPaneId = PANENAME_BATTLE_BTN_LOW_000_PANE_BOUND_00;
    }

    //  カーソルオブジェクトの座標位置ペイン作成
    {
      gfl2::lyt::LytParts*  pRootPartsPane  = NULL;

      u32 loopNum = GFL_NELEMS( m_saCursorConfigDataTable );
      for( u32 i = 0; i < loopNum; ++i )
      {
        //  ボタンデータテーブルからボタンのペインidを使い、ボタンのペインを取得して、カーソルペインを取得
        pRootPartsPane = pLayoutWork->GetPartsPane( m_aCreateButtonDataTable[ m_saCursorConfigDataTable[ i ].button_id ].buttonPartPaneId );
        GFL_ASSERT( pRootPartsPane );

        m_aMenuCursorItemData[ i ].pNullCursorPane  = pLayoutWork->GetPane( pRootPartsPane, PANENAME_BATTLE_BTN_LOW_000_PANE_NULL_CUR, &rMultiResId );
        m_aMenuCursorItemData[ i ].pNullGlarePane   = pLayoutWork->GetPane( pRootPartsPane, PANENAME_BATTLE_BTN_LOW_000_PANE_NULL_GLOW, &rMultiResId );

        pLayoutWork->GetPane( pRootPartsPane, PANENAME_BATTLE_BTN_LOW_000_PANE_EFFECT_GLOW, &rMultiResId )->SetVisible(true);
        m_aMenuCursorItemData[ i ].pNullGlarePane->SetVisible(false);

        GFL_ASSERT( m_aMenuCursorItemData[ i ].pNullCursorPane );

        m_aMenuCursorItemData[ i ].selectLoopAnimeId  = m_aCreateButtonDataTable[ m_saCursorConfigDataTable[ i ].button_id ].selectLoopAnimeId;
      }
    }

    //  画面の各ボタンペインポイントリスト取得
    {
      u32 loopNum = GFL_NELEMS( m_aCreateButtonDataTable );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_pBtnPartRootPaneList[ i ] = pLayoutWork->GetPartsPane( m_aCreateButtonDataTable[ i ].buttonPartPaneId );
        GFL_ASSERT( m_pBtnPartRootPaneList[ i ] );
      }
    }

    //  ポケモンオブジェクト作成
    {
      POKE_DATA*  pPokeData         = NULL;
      bool        bInfoIconVisible  = false;

      for( s32 i = 0; i < POKE_ICON_POS_TYPE_MAX; ++i )
      {
        pPokeData = &m_initParam.aPokeData[ i ];
        if( pPokeData->p_btl_poke_param != NULL )
        {
          //  ポケモンデータ作成
          _CreatePokeData( i, *pPokeData, &rMultiResId, m_initParam.screenType );

          bInfoIconVisible  = false;

          //  インフォアイコンはパラメータを参照して出す
          if( m_aPokeObjData[ i ].IsChangeStatus() == true )
          {
            bInfoIconVisible  = true;
          }

          //  初回のインフォアイコン表示設定
          m_aPokeObjData[ i ].SetIconVisible( bInfoIconVisible );
        }
        else
        {
          //  アイコン表示していないのはタッチさせない
        }
      }
    }

    //  ボタン入力関連の設定
    {
      //  ボタン設定データ定義一覧
      ::app::ui::ButtonInfoEx    aSetButtonPaneIdxList[ BUTTON_ID_MAX ];
      gfl2::std::MemClear( aSetButtonPaneIdxList, sizeof( aSetButtonPaneIdxList ) );

      const ::app::ui::ButtonInfoEx    aButtonPaneIdxList[] =
      {
        //  たたかうボタン
        _CreateButtonData( BUTTON_ID_FIGHT ),
        //  バッグボタン
        _CreateButtonData( BUTTON_ID_BAG ),
        //  にげるボタン
        _CreateButtonData( BUTTON_ID_RUN ),
        //  ポケモンボタン
        _CreateButtonData( BUTTON_ID_POKE ),
        //  戻るボタン
        _CreateButtonData( BUTTON_ID_BACK ),

        //  ボールボタン
        _CreateButtonData( BUTTON_ID_BALL ),
        /*
        //  ポケモンアイコンボタン
        _CreateButtonData( BUTTON_ID_POKE_ICON_RIGHT_UP ),
        _CreateButtonData( BUTTON_ID_POKE_ICON_LEFT_DOWN ),
        _CreateButtonData( BUTTON_ID_POKE_ICON_LEFT_UP ),
        _CreateButtonData( BUTTON_ID_POKE_ICON_RIGHT_DOWN ),
        */

        //  ユーザー名のボタン(1vs1用)
        _CreateButtonData( BUTTON_ID_USER_NAME ),
      };

      u32 setButtonInfoNum  = 0;
      //  まず絶対押せるボタンを設定
      {
        u32 loopNum = GFL_NELEMS( aButtonPaneIdxList );
        for( u32 i = 0; i < loopNum; ++i )
        {
          aSetButtonPaneIdxList[ i ]  = aButtonPaneIdxList[ i ];
        }

        setButtonInfoNum  = loopNum;
      }

      //  ポケモンオブジェクトをボタンに追加するか
      {
        for( s32 i = 0; i < POKE_ICON_POS_TYPE_MAX; ++i )
        {
          if( m_aPokeObjData[ i ].IsPut() == true )
          {
            aSetButtonPaneIdxList[ setButtonInfoNum ] = m_aPokeObjData[ i ].GetButtonInfoData();
            ++setButtonInfoNum;
          }
        }
      }

      //  ボタン登録
      UIResponder::CreateButtonManager( m_initParam.pUtilHeap, pG2DUtil->GetLayoutWork( LYT_WK_TYPE_SCREEN ), aSetButtonPaneIdxList, setButtonInfoNum );
    }

    //  ボタンのSE設定
    if( m_initParam.bUseInputSE )
    {
      ::app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();

      const s32 aSetupSeTable[ BUTTON_ID_MAX ][ 2 ] =
      {
        { BUTTON_ID_FIGHT,                SEQ_SE_DECIDE1 },
        { BUTTON_ID_POKE,                 SEQ_SE_DECIDE1 },
        { BUTTON_ID_BAG,                  SEQ_SE_DECIDE1 },
        { BUTTON_ID_RUN,                  SEQ_SE_DECIDE1 },
        { BUTTON_ID_BACK,                 SEQ_SE_CANCEL1 },
        { BUTTON_ID_BALL,                 SEQ_SE_DECIDE1 },
        { BUTTON_ID_POKE_ICON_LEFT_DOWN,  SEQ_SE_DECIDE1 },
        { BUTTON_ID_POKE_ICON_RIGHT_DOWN, SEQ_SE_DECIDE1 },
        { BUTTON_ID_POKE_ICON_LEFT_UP,    SEQ_SE_DECIDE1 },
        { BUTTON_ID_POKE_ICON_RIGHT_UP,   SEQ_SE_DECIDE1 },
        { BUTTON_ID_USER_NAME,            SEQ_SE_DECIDE1 },
      };

      for( s32 i = 0; i < BUTTON_ID_MAX; ++i )
      {
        pButtonManager->SetButtonSelectSE( aSetupSeTable[ i ][ 0 ], aSetupSeTable[ i ][ 1 ] );
      }
    }

    //  ボールアイコンオブジェクト作成
    {
      BallObj::INIT_DATA  ballObjInitData;
      ballObjInitData.pSysHeap      = m_initParam.pUtilHeap->GetSystemHeap();
      ballObjInitData.pDevHeap      = m_initParam.pUtilHeap->GetDeviceHeap();
      ballObjInitData.itemId        = 0;
      ballObjInitData.pPanePicture  = pLayoutWork->GetPicturePane( m_pBtnPartRootPaneList[ BUTTON_ID_BALL ], PANENAME_BATTLE_BTN_LOW_004_PANE_ICON_BALL, &rMultiResId );;
      ballObjInitData.pG2DUtil      = pG2DUtil;

      m_pBallObj = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) BallObj( ballObjInitData );
    }

    //  入力コールバック設定
    UIView::SetInputListener( this );

    //  Start関数が呼ばれるまで非表示
    UIView::SetVisible( false );

    _SetScreenType( m_initParam.screenType );

    _SetDrawMode( m_initParam.drawMode );
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 更新処理
    */
  //-------------------------------------------------------------------------
  void BattleUIActSelectLowerDraw::Update( void )
  {
    _UpdateState();

    {
      u32 loopNum = GFL_NELEMS( m_aPokeObjData );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_aPokeObjData[ i ].Update();
      }
    }

    m_pMenuCursor->Update();
    UIView::Update();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 描画
    */
  //-------------------------------------------------------------------------
  void BattleUIActSelectLowerDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    for( s32 i = 0; i < LYT_WK_TYPE_MAX; ++i )
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderingManager, displayNo, i, LOW_VIEW_ORDER_TOP );
    }

    //  入力できない時は描画はしない
    if( UIView::IsInputEnabled() == true )
    {
      m_pMenuCursor->Draw( m_initParam.pRenderingManager, displayNo, gfl2::lyt::DISPLAY_LOWER, LOW_VIEW_ORDER_TOP );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   破棄可能か
    */
  //------------------------------------------------------------------
  bool BattleUIActSelectLowerDraw::IsDelete()
  {
    bool  bRet  = true;

    u32 loopNum = GFL_NELEMS( m_aPokeObjData );
    for( u32 i = 0; i < loopNum; ++i )
    {
      //  アイコン処理が終っているかチェック
      if( m_aPokeObjData[ i ].IsDelete() == false )
      {
        bRet  = false;
        break;
      }
    }

    //  メニューカーソルの破棄が終っているか
    if( m_pMenuCursor->DeleteLayoutWork() == false )
    {
      bRet  = false;
    }

    if( UIView::IsDrawing() == true )
    {
      bRet  = false;
    }

    return bRet;
  }

  //------------------------------------------------------------------
  /**
    * @brief   ボタン項目表示設定
    */
  //------------------------------------------------------------------
  void BattleUIActSelectLowerDraw::SetVisibleButton( ButtonId buttonId, bool bVisible )
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
    * @brief   ボールアイテムid設定
    * @param   itemId : ボールのアイテムidを指定 / Savedata::MyItem::ITEM_NONE ならボール項目は非表示に
    * 
    */
  //------------------------------------------------------------------
  void BattleUIActSelectLowerDraw::SetBallItemId( const u32 itemId, const bool bPassive )
  {
    if( itemId == Savedata::MyItem::ITEM_NONE )
    {
      SetVisibleButton( BUTTON_ID_BALL ,false );
    }
    else
    {
      m_pBallObj->SetItemId( itemId );
      m_pBallObj->SetPassive( bPassive );
      SetVisibleButton( BUTTON_ID_BALL ,true );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   モード設定
    */
  //------------------------------------------------------------------
  void BattleUIActSelectLowerDraw::SetCommandMode( const CommandMode newMode, const bool bEventAnime )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    if( m_nowCommandMode == newMode )
    {
      return;
    }

    m_nowCommandMode  = newMode;

    s32 startAnimeFrame = 2;
    if( bEventAnime )
    {
      startAnimeFrame = 0;
    }

    if( newMode == COMMAND_MODE_RUN )
    {
      pG2DUtil->StartAnimeFrame( LYT_WK_TYPE_SCREEN, LA_MINLOW_BATTLE_MIN_LOW_000_RUN_IN, startAnimeFrame, true, true );

      //  戻るボタンを使用不可能にしてにげるボタンを使用可能に
      SetVisibleButton( BUTTON_ID_BACK, false );
      SetVisibleButton( BUTTON_ID_RUN, true );
    }
    else if( newMode == COMMAND_MODE_BACK )
    {
      pG2DUtil->StartAnimeFrame( LYT_WK_TYPE_SCREEN, LA_MINLOW_BATTLE_MIN_LOW_000_BACK_IN, startAnimeFrame, true, true );

      //  にげるボタンを使用不可能にして戻るボタンを使用可能に
      SetVisibleButton( BUTTON_ID_BACK, true );
      SetVisibleButton( BUTTON_ID_RUN, false );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   表示モード設定
    */
  //------------------------------------------------------------------
  void BattleUIActSelectLowerDraw::_SetDrawMode( const DrawMode newMode )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    gfl2::std::MemFill( m_pPlayerNameTextBoxPaneArray, NULL, sizeof( m_pPlayerNameTextBoxPaneArray ) );

    gfl2::lyt::LytWk* pLayoutWork = UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN );

    gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

    if( newMode == DRAW_MODE_NORMAL )
    {
      //  通常表示

      //  プレイヤー名ボタンを使用不可能に
      SetVisibleButton( BUTTON_ID_USER_NAME , false );

      //  バックボタンを使用可能に
      SetVisibleButton( BUTTON_ID_BAG, true );

      //  ボールボタンを表示
      SetVisibleButton( BUTTON_ID_BALL, true );
      m_bBtnBallShortCut  = true;

      //  アニメで表示を切り替え
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_MINLOW_BATTLE_MIN_LOW_000_VISIBLE_NORMAL );
    }
    else 
    if( newMode == DRAW_MODE_NET )
    {
      //  通信時の表示

      //  プレイヤー名ボタンを表示
      {
        SetVisibleButton( BUTTON_ID_USER_NAME , false );

        //  テキスト表示のペイン取得
        {
          m_pPlayerNameTextBoxPaneArray[ 0 ]  = pLayoutWork->GetTextBoxPane( m_pBtnPartRootPaneList[ BUTTON_ID_USER_NAME ], PANENAME_BATTLE_BTN_LOW_004_PANE_TEXTBOX_00, &rMultiResId );
          GFL_ASSERT( m_pPlayerNameTextBoxPaneArray[ 0 ] );
        }

        //  通信対戦で味方がいるマルチの場合はレート表示は出さない
        if( m_initParam.bFriendMuliti == false )
        {
          SetVisibleButton( BUTTON_ID_USER_NAME, true );
          _SetPlayerNameText( m_initParam.pEnemyPlayerName, m_initParam.enemyPlayerGrammer );
        }
        else
        {
          //  空名を設定
          _SetPlayerNameText( NULL, m_initParam.enemyPlayerGrammer );
        }
      }

      //  バックボタンを使用不可能に
      SetVisibleButton( BUTTON_ID_BAG, false );

      //  ボールボタンを非表示
      SetVisibleButton( BUTTON_ID_BALL, false );
      m_bBtnBallShortCut  = false;

      //  アニメで表示を切り替え
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_MINLOW_BATTLE_MIN_LOW_000_VISIBLE_NET );
    }
    else
    if( newMode == DRAW_MODE_BTL_FACLIITY )
    {
      //  バトル施設トレーナー時の表示

      SetVisibleButton( BUTTON_ID_USER_NAME , false );

      //  バックボタンを使用不可能に
      SetVisibleButton( BUTTON_ID_BAG, false );

      //  ボールボタンを非表示
      SetVisibleButton( BUTTON_ID_BALL, false );
      m_bBtnBallShortCut  = false;

      //  アニメで表示を切り替え
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_MINLOW_BATTLE_MIN_LOW_000_VISIBLE_NET );
    }
  }

  /**
    * @brief 選択項目のカーソルactive設定( どれかひとつの項目のみactiveにする )
    */
  bool BattleUIActSelectLowerDraw::SetActiveSelectItemCursor( const bool bActive, const s32 selectItemType )
  {
    GFL_ASSERT( selectItemType < CURSOR_POS_TYPE_MAX );

    //  カーソル表示設定
//    m_pMenuCursor->SetVisible( bActive );
    
    gfl2::lyt::LytWk* pLayoutWork = UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN );
    s32 buttonId  = _ConvCursorTypeToButtonId( selectItemType );
    if( UIResponder::GetButtonManager()->IsButtonVisible( buttonId ) == false )
    {
      return  false;
    }

    if( bActive == true )
    {
      ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

      //  カーソル配置
      m_pMenuCursor->Put( pLayoutWork, m_aMenuCursorItemData[ selectItemType ].pNullCursorPane );
      /*  momiji対応  */
      m_pMenuCursor->SetVisible(false);

      u32 loopNum = GFL_NELEMS( m_saCursorConfigDataTable );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_aMenuCursorItemData[ i ].pNullGlarePane->SetVisible(false);
      }
      m_aMenuCursorItemData[ selectItemType ].pNullGlarePane->SetVisible(true);

      //  カーソルアニメ開始
      m_pMenuCursor->StartAnime();

      //  カーソルがヒットしている項目をアニメ再生
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, m_aMenuCursorItemData[ selectItemType ].selectLoopAnimeId );

      //  現在のカーソル先と移動先とが異なる場合のに更新
      if( m_initParam.selectItemIdx != selectItemType )
      {
        m_oldCursor               = m_initParam.selectItemIdx;

        //  変更前のカーソル項目のアニメ再生を停止
        pG2DUtil->StopAnime( LYT_WK_TYPE_SCREEN, m_aMenuCursorItemData[ m_oldCursor ].selectLoopAnimeId );

        m_initParam.selectItemIdx = selectItemType;

        return  true;
      }
    }
    else
    {
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief   画面タイプ設定
    */
  //------------------------------------------------------------------
  void BattleUIActSelectLowerDraw::_SetScreenType( const ScreenType newType )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    //  画面全体の配置をタイプごとに変える
    {
      const s32 aSetAnimeIdx[ SCREEN_TYPE_MAX ] =
      {
        LA_MINLOW_BATTLE_MIN_LOW_000_BATTLE_NORMAL,
        LA_MINLOW_BATTLE_MIN_LOW_000_BATTLE_2VS2,
        LA_MINLOW_BATTLE_MIN_LOW_000_BATTLE_1VS3,
        LA_MINLOW_BATTLE_MIN_LOW_000_BATTLE_RANNYU
      };

      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, aSetAnimeIdx[ newType ] );
    }
  }

  /**
  * @brief  カーソル選択している項目を押す
  */
  void BattleUIActSelectLowerDraw::PushBtnCurrentCursorItem()
  {
    //  コールバックで入力結果を渡す
    if( m_initParam.pDelegate )
    {
      s32 buttonId  = _ConvCursorTypeToButtonId( m_initParam.selectItemIdx );
      if( buttonId != -1 )
      {
        //  コールバックの OnLayoutPaneEvent 関数が呼ばれる
        UIResponder::GetButtonManager()->StartSelectedAct( buttonId );

        //  入力が無効の場合はOnLayoutPaneEvent()関数が呼ばれないので手動で呼び出す
        if( UIView::IsInputEnabled() == false )
        {
          OnLayoutPaneEvent( buttonId );
        }
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   指定したカーソルを選択して項目を押す
    */
  //------------------------------------------------------------------
  void BattleUIActSelectLowerDraw::PushBtnSelectCursorItem( const CursorType newCursorType )
  {
    m_initParam.selectItemIdx = newCursorType;

    PushBtnCurrentCursorItem();
  }

  //  note  通信対戦相手名の複数設定の仕様がなくなった
  //        しかし仕様が復活した時のためにいったん残す
#if 0
  //------------------------------------------------------------------
  /**
    * @brief   プレイヤー名のテキスト設定
    */
  //------------------------------------------------------------------
  void BattleUIActSelectLowerDraw::SetPlayerNameText( gfl2::str::StrBuf* pPlayerNameArray, const s32 arrayNum )
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
            pG2DUtil->GetWordSet()->RegisterWord( 0, pPlayerNameArray[ i ], 0 );
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
          pG2DUtil->SetTextBoxPaneString( m_pPlayerNameTextBoxPaneArray[ i ], STR_VS_PLAYER_NAME );
        }
      }
    }
  }
#endif

  //------------------------------------------------------------------
  /**
    * @brief   各ボタンの登録データ作成
    * @param   createButtonType  : 作成するボタンデータのタイプ
    * @param   buttonId          : ボタンID
    */
  //------------------------------------------------------------------
  ::app::ui::ButtonInfoEx BattleUIActSelectLowerDraw::_CreateButtonData( ButtonId createButtonType )
  {
    ::app::ui::ButtonInfoEx data;

    data.button_id     = createButtonType;

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    gfl2::lyt::LytWk* pLayoutWork   = UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN );

    //  使用しているResID
    gfl2::lyt::LytMultiResID& rLytMultiResId  = pG2DUtil->GetLayoutResourceID( 0 );

    BUTTON_CREATE_DATA* pCreateData = &m_aCreateButtonDataTable[ createButtonType ];

    data.touch_anime_index       = pCreateData->touch_anime_index;
    data.release_anime_index     = pCreateData->release_anime_index;
    data.cancel_anime_index      = pCreateData->cancel_anime_index;
    data.key_select_anime_index  = pCreateData->key_select_anime_index;
    data.active_anime_index      = pCreateData->active_anime_index;
    data.passive_anime_index     = pCreateData->passive_anime_index;

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
  void BattleUIActSelectLowerDraw::_CreatePokeData( const s32 pokeDataIdx, const POKE_DATA& rPokeData, gfl2::lyt::LytMultiResID* pLytMultiResID, const ScreenType screenType )
  {
    GFL_ASSERT( pokeDataIdx < GFL_NELEMS( m_aPokeObjData ) );
    GFL_ASSERT( pLytMultiResID );

    gfl2::lyt::LytWk* pLayoutWork = UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN );

    //  ポケモンアイコンと関連づける、ボタンId一覧
    const ButtonId aPokeDataIdxButtonIdMap[ POKE_ICON_POS_TYPE_MAX ] =
    {
      BUTTON_ID_POKE_ICON_LEFT_DOWN,
      BUTTON_ID_POKE_ICON_RIGHT_DOWN,
      BUTTON_ID_POKE_ICON_LEFT_UP,
      BUTTON_ID_POKE_ICON_RIGHT_UP
    };

    PokeObj::INIT_DATA  aInitData[ POKE_ICON_POS_TYPE_MAX ] =
    {
      //  左下のポケモンオブジェクト
      {
        PokeObj::ICON_DIR_TYPE_LEFT,
        pLayoutWork->GetPartsPane( m_pBtnPartRootPaneList[ BUTTON_ID_POKE_ICON_LEFT_DOWN ], PANENAME_BATTLE_BTN_LOW_007_PANE_BTNPOKEICON_00, pLytMultiResID ),
        pLayoutWork->GetPane( m_pBtnPartRootPaneList[ BUTTON_ID_POKE_ICON_LEFT_DOWN ], PANENAME_BATTLE_BTN_LOW_007_PANE_NULL_02, pLytMultiResID ),
        pLayoutWork->GetBoundingPane( m_pBtnPartRootPaneList[ BUTTON_ID_POKE_ICON_LEFT_DOWN ], PANENAME_BATTLE_BTN_LOW_007_PANE_BOUND_00, pLytMultiResID ),

        {
          LA_MINLOW_BATTLE_MIN_LOW_000__POKE_LB__BTNPOKEICON_00_DIE,
          LA_MINLOW_BATTLE_MIN_LOW_000__POKE_LB_IN,
          LA_MINLOW_BATTLE_MIN_LOW_000__POKE_LB_ENEMY,
          LA_MINLOW_BATTLE_MIN_LOW_000__POKE_LB_ALLY,
        },
      },

      //  右下のポケモンオブジェクト
      {
        PokeObj::ICON_DIR_TYPE_LEFT,
        pLayoutWork->GetPartsPane( m_pBtnPartRootPaneList[ BUTTON_ID_POKE_ICON_RIGHT_DOWN ], PANENAME_BATTLE_BTN_LOW_007_PANE_BTNPOKEICON_00, pLytMultiResID ),
        pLayoutWork->GetPane( m_pBtnPartRootPaneList[ BUTTON_ID_POKE_ICON_RIGHT_DOWN ], PANENAME_BATTLE_BTN_LOW_007_PANE_NULL_02, pLytMultiResID ),
        pLayoutWork->GetBoundingPane( m_pBtnPartRootPaneList[ BUTTON_ID_POKE_ICON_RIGHT_DOWN ], PANENAME_BATTLE_BTN_LOW_007_PANE_BOUND_00, pLytMultiResID ),

        {
          LA_MINLOW_BATTLE_MIN_LOW_000__POKE_RB__BTNPOKEICON_00_DIE,
          LA_MINLOW_BATTLE_MIN_LOW_000__POKE_RB_IN,
          LA_MINLOW_BATTLE_MIN_LOW_000__POKE_RB_ENEMY,
          LA_MINLOW_BATTLE_MIN_LOW_000__POKE_RB_ALLY,
        },
      },

      //  左上のポケモンオブジェクト
      {
        PokeObj::ICON_DIR_TYPE_LEFT,
        pLayoutWork->GetPartsPane( m_pBtnPartRootPaneList[ BUTTON_ID_POKE_ICON_LEFT_UP ], PANENAME_BATTLE_BTN_LOW_007_PANE_BTNPOKEICON_00, pLytMultiResID ),
        pLayoutWork->GetPane( m_pBtnPartRootPaneList[ BUTTON_ID_POKE_ICON_LEFT_UP ], PANENAME_BATTLE_BTN_LOW_007_PANE_NULL_02, pLytMultiResID ),
        pLayoutWork->GetBoundingPane( m_pBtnPartRootPaneList[ BUTTON_ID_POKE_ICON_LEFT_UP ], PANENAME_BATTLE_BTN_LOW_007_PANE_BOUND_00, pLytMultiResID ),

        {
          LA_MINLOW_BATTLE_MIN_LOW_000__POKE_LT__BTNPOKEICON_00_DIE,
          LA_MINLOW_BATTLE_MIN_LOW_000__POKE_LT_IN,
          LA_MINLOW_BATTLE_MIN_LOW_000__POKE_LT_ENEMY,
          LA_MINLOW_BATTLE_MIN_LOW_000__POKE_LT_ALLY,
        },
      },

      //  右上のポケモンオブジェクト
      {
        PokeObj::ICON_DIR_TYPE_LEFT,
        pLayoutWork->GetPartsPane( m_pBtnPartRootPaneList[ BUTTON_ID_POKE_ICON_RIGHT_UP ], PANENAME_BATTLE_BTN_LOW_007_PANE_BTNPOKEICON_00, pLytMultiResID ),
        pLayoutWork->GetPane( m_pBtnPartRootPaneList[ BUTTON_ID_POKE_ICON_RIGHT_UP ], PANENAME_BATTLE_BTN_LOW_007_PANE_NULL_02, pLytMultiResID ),
        pLayoutWork->GetBoundingPane( m_pBtnPartRootPaneList[ BUTTON_ID_POKE_ICON_RIGHT_UP ], PANENAME_BATTLE_BTN_LOW_007_PANE_BOUND_00, pLytMultiResID ),

        {
          LA_MINLOW_BATTLE_MIN_LOW_000__POKE_RT__BTNPOKEICON_00_DIE,
          LA_MINLOW_BATTLE_MIN_LOW_000__POKE_RT_IN,
          LA_MINLOW_BATTLE_MIN_LOW_000__POKE_RT_ENEMY,
          LA_MINLOW_BATTLE_MIN_LOW_000__POKE_RT_ALLY,
        },
      }
    };

    //  画面タイプごとにポケモンの向きを変える
    {
      static const PokeObj::IconDirType  aIconDirType[ SCREEN_TYPE_MAX ][ POKE_ICON_POS_TYPE_MAX ]  =
      {
        //  シングル
        {
          //  左下
          PokeObj::ICON_DIR_TYPE_RIGHT,

          //  右下
          PokeObj::ICON_DIR_TYPE_RIGHT,

          //  左上
          PokeObj::ICON_DIR_TYPE_LEFT,

          //  右上
          PokeObj::ICON_DIR_TYPE_LEFT,

        },

        //  ダブル
        {
          //  左下
          PokeObj::ICON_DIR_TYPE_RIGHT,

          //  右下
          PokeObj::ICON_DIR_TYPE_RIGHT,

          //  左上
          PokeObj::ICON_DIR_TYPE_LEFT,

          //  右上
          PokeObj::ICON_DIR_TYPE_LEFT,
        },

        //  ロイヤル
        {
          //  左下
          PokeObj::ICON_DIR_TYPE_RIGHT,

          //  右下
          PokeObj::ICON_DIR_TYPE_LEFT,

          //  左上
          PokeObj::ICON_DIR_TYPE_RIGHT,

          //  右上
          PokeObj::ICON_DIR_TYPE_LEFT,
        },

        //  乱入
        {
          //  左下
          PokeObj::ICON_DIR_TYPE_RIGHT,

          //  右下
          PokeObj::ICON_DIR_TYPE_RIGHT,

          //  左上
          PokeObj::ICON_DIR_TYPE_LEFT,

          //  右上
          PokeObj::ICON_DIR_TYPE_LEFT,
        },
      };

      aInitData[ pokeDataIdx ].dir_type = aIconDirType[ screenType ][ pokeDataIdx ];
    }

    m_aPokeObjData[ pokeDataIdx ].Initialize( aInitData[ pokeDataIdx ], _CreateButtonData( aPokeDataIdxButtonIdMap[ pokeDataIdx ] ), this, LYT_WK_TYPE_SCREEN, rPokeData, m_initParam.pUtilHeap );
  }

  /**
    * @brief レイアウトワークを生成する
    */
  void BattleUIActSelectLowerDraw::_Initialize2D( void* pAppLytBuff )
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
            LYTRES_MINLOW_BATTLE_MIN_LOW_000_BFLYT,
            LA_MINLOW_BATTLE_MIN_LOW_000___NUM,
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
  void BattleUIActSelectLowerDraw::_UpdateState()
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    if( m_state == STATE_TYPE_NONE )
    {
    }

    if( m_state == STATE_TYPE_IN )
    {
      //  アニメの終了を検知
      if( pG2DUtil->IsAnimeEnd( LYT_WK_TYPE_SCREEN, LA_MINLOW_BATTLE_MIN_LOW_000_IN ) == true )
      {
        _SetState( STATE_TYPE_IDLE );
      }
    }

    if( m_state == STATE_TYPE_IDLE )
    {
    }

    if( m_state == STATE_TYPE_OUT )
    {
      if( ( UIView::IsVisible() == false ) || ( pG2DUtil->IsAnimeEnd( LYT_WK_TYPE_SCREEN, LA_MINLOW_BATTLE_MIN_LOW_000_OUT ) == true ) )
      {
        //  アニメの終了を検知
        _SetState( STATE_TYPE_END );
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   プレイヤー名のテキスト設定
    */
  //------------------------------------------------------------------
  void BattleUIActSelectLowerDraw::_SetPlayerNameText( const gfl2::str::StrBuf* pPlayerNameStrBuf, const print::GrammaticalAttribute grammer )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    if( pPlayerNameStrBuf == NULL )
    {
      pG2DUtil->SetTextBoxPaneString( m_pPlayerNameTextBoxPaneArray[ 0 ], STR_VS_PLAYER_NAME );
    }
    else
    {
      pG2DUtil->GetWordSet()->RegisterWord( 0, *pPlayerNameStrBuf, grammer );
      pG2DUtil->SetTextBoxPaneStringExpand( m_pPlayerNameTextBoxPaneArray[ 0 ], STR_TRAINER_NAME );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   カーソルタイプ値からボタンid値を変換
    */
  //------------------------------------------------------------------
  s32 BattleUIActSelectLowerDraw::_ConvCursorTypeToButtonId( const s32 cursorType )
  {
    u32 loopNum = GFL_NELEMS( m_saCursorConfigDataTable );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( m_saCursorConfigDataTable[ i ].pos_id == cursorType )
      {
        return  m_saCursorConfigDataTable[ i ].button_id;
      }
    }

    return  -1;
  }

  //------------------------------------------------------------------
  /**
    * @brief   ボタンid値からカーソルタイプ値を変換して取得
    */
  //------------------------------------------------------------------
  s32 BattleUIActSelectLowerDraw::_ConvButtonIdToCursorType( const s32 buttonId )
  {
    u32 loopNum = GFL_NELEMS( m_saCursorConfigDataTable );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( m_saCursorConfigDataTable[ i ].button_id == buttonId )
      {
        return  m_saCursorConfigDataTable[ i ].pos_id;
      }
    }

    return  -1;
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void BattleUIActSelectLowerDraw::_SetState( const BattleUIActSelectLowerDraw::StateType newStateType )
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

      //  画面が開くSE
      Sound::PlaySE( SEQ_SE_OPEN2 );

      //  開始アニメをする
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_MINLOW_BATTLE_MIN_LOW_000_IN );

      //  ポケモンの開始アニメをする
      {
        for( s32 i = 0; i < POKE_ICON_POS_TYPE_MAX; ++i )
        {
          m_aPokeObjData[ i ].StartIconAnime( PokeObj::POKE_ICON_ANIME_IN, true );
        }
      }

      //  カーソル表示
//      m_pMenuCursor->SetVisible( true );
    }
    else
    if( m_state == STATE_TYPE_IDLE )
    {
      //  初期カーソル設定
      SetActiveSelectItemCursor( true, m_initParam.selectItemIdx );
    }
    else
    if( m_state == STATE_TYPE_ENTER_ANIM_WAIT )
    {
      
    }
    else
    if( m_state == STATE_TYPE_OUT )
    {
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_MINLOW_BATTLE_MIN_LOW_000_OUT );

      //  カーソル非表示
//      m_pMenuCursor->SetVisible( false );
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
  ::app::ui::UIInputListener::ListenerResult BattleUIActSelectLowerDraw::OnLayoutPaneEvent( const u32 buttonId )
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
  void BattleUIActSelectLowerDraw::OnLayoutPaneTouchTriggerEvent( const u32 buttonId )
  {
    //  カーソル移動
    const s32 cursorSelectitem  = _ConvButtonIdToCursorType( buttonId );
    if( cursorSelectitem != -1 )
    {
      m_initParam.selectItemIdx = cursorSelectitem;
      SetActiveSelectItemCursor( true, m_initParam.selectItemIdx );
    }
    else
    {
      //  ポケモンアイコンにタッチしたらタッチアニメ再生
      u32 loopNum = GFL_NELEMS( m_aPokeObjData );
      for( u32 i = 0; i < loopNum; ++i )
      {
        if( m_aPokeObjData[ i ].GetButtonInfoData().button_id == buttonId )
        {
//          m_aPokeObjData[ i ].StartObjAnime( PokeObj::OBJ_ANIME_TOUCH );
          break;
        }
      }
    }
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
  ::app::ui::UIInputListener::ListenerResult BattleUIActSelectLowerDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    if( m_initParam.pDelegate == NULL )
    {
      return ENABLE_ACCESS;
    }

    if( m_initParam.pDelegate->OnKeyAction( this, pButton, pKey, pStick ) )
    {
      return  DISABLE_ACCESS;
    }

    return ENABLE_ACCESS;
  }

//  ここから先は BattleUIActSelectLowerDraw::PokeObj クラスの定義

  //  初期化
  void BattleUIActSelectLowerDraw::PokeObj::Initialize( const INIT_DATA& rInitData, const ::app::ui::ButtonInfoEx& rButtonInfo, ::app::ui::UIView* pRootView, u32 lytId, const POKE_DATA& rPokeData, ::app::util::Heap* pUtilHeap )
  {
    m_initData    = rInitData;
    m_pokeData    = rPokeData;
    m_pRootView   = pRootView;
    m_buttonInfo  = rButtonInfo;

    GFL_ASSERT( m_initData.p_icon_parts_pane );
    GFL_ASSERT( m_initData.p_info_icon_pane );
    GFL_ASSERT( m_initData.p_touch_hit_pane );

    //  ポケモンアイコン生成
    {
      ::app::tool::PokeIconToolEx::PokeIconToolParam  initPokeIconParam;
      initPokeIconParam.layout_id = lytId;
      initPokeIconParam.pHeap     = pUtilHeap;
      initPokeIconParam.pUIView   = pRootView;
      initPokeIconParam.res_id    = 0;
      initPokeIconParam.sync      = false;
      initPokeIconParam.iconMax   = 1;

      m_pIcon = GFL_NEW( pUtilHeap->GetDeviceHeap() ) ::app::tool::PokeIconToolEx( initPokeIconParam );
      GFL_ASSERT( m_pIcon );

      //  アイコンの向きが右向きだと反転させる
      bool  bIconFlip = false;
      {
        if( rInitData.dir_type == ICON_DIR_TYPE_LEFT )
        {
          bIconFlip = false;
        }
        else
        {
          bIconFlip = true;
        }
      }

      const pml::pokepara::PokemonParam*  pViewPokemonParam = NULL;
      if( rPokeData.type == btl::BattleUIPokeStatusFrame::STATUS_TYPE_SELF )
      {
        pViewPokemonParam = rPokeData.p_btl_poke_param->GetSrcData();
      }
      else
      {
        pViewPokemonParam = rPokeData.p_btl_poke_param->GetViewSrcData();
      }

      m_pIcon->SetPokeIcon( 0, pViewPokemonParam, PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON, m_initData.p_icon_parts_pane, bIconFlip );

      SetDirType( rInitData.dir_type );
    }

    //  ボタンのタッチ範囲を設定
    {
      m_buttonInfo.bound_pane = m_initData.p_touch_hit_pane;
    }

    if( rPokeData.p_btl_poke_param->IsDead() == true )
    {
      m_pIcon->StartAnim( m_initData.a_icon_anime_id[ POKE_ICON_ANIME_DIE ] );
    }

    //ポケモン状態変化しているか
    {
      m_bChangeStatus = false;
      if( 0 < rPokeData.p_btl_poke_status->statusNum )
      {
        m_bChangeStatus = true;
      }
      else
      {
        //  ステータスが変化しているか
        const BTL_POKEPARAM::ValueID  caPokeParamId[] =
        {
          //  こうげき
            BTL_POKEPARAM::BPP_ATTACK_RANK,
          //  ぼうぎょ
            BTL_POKEPARAM::BPP_DEFENCE_RANK,
          //  とくこう
            BTL_POKEPARAM::BPP_SP_ATTACK_RANK,
          //  とくぼう
            BTL_POKEPARAM::BPP_SP_DEFENCE_RANK,
          //  すばやさ
            BTL_POKEPARAM::BPP_AGILITY_RANK,
          //  めいちゅう
            BTL_POKEPARAM::BPP_HIT_RATIO,
          //  かいひ
            BTL_POKEPARAM::BPP_AVOID_RATIO
        };

        u32 loopNum = GFL_NELEMS( caPokeParamId );
        for( u32 i = 0; i < loopNum; ++i )
        {
          //  ステータスが変化しているかどうか
          if( rPokeData.p_btl_poke_param->GetValue( caPokeParamId[ i ] ) != BTL_POKEPARAM::RANK_STATUS_DEFAULT )
          {
            m_bChangeStatus = true;
            break;
          }
        }
      }
    }
  }

  //  終了処理
  void BattleUIActSelectLowerDraw::PokeObj::Terminate()
  {
  }

  //  破棄可能か
  bool BattleUIActSelectLowerDraw::PokeObj::IsDelete()
  {
    if( m_pIcon == NULL )
    {
      return  true;
    }

    if( m_pIcon->EndFunc() == false )
    {
      return  false;
    }

    return true;
  }

  //  インフォアイコン表示設定
  void BattleUIActSelectLowerDraw::PokeObj::SetIconVisible( const bool bFlg )
  {
    if( m_pIcon == NULL )
    {
      return;
    }

    ::app::util::G2DUtil* pG2DUtil  = m_pRootView->GetG2DUtil();

    GFL_ASSERT( m_initData.p_info_icon_pane );
    if( m_initData.p_info_icon_pane != NULL )
    {
      pG2DUtil->SetPaneVisible( m_initData.p_info_icon_pane, bFlg );
    }
  }

  //  アイコンアニメ再生
  void BattleUIActSelectLowerDraw::PokeObj::StartIconAnime( const PokeIconAnime animeIdx, const bool bForcePlayAnime )
  {
    if( m_pIcon == NULL )
    {
      return;
    }

    if( ( m_playIconAnimeIdx != m_initData.a_icon_anime_id[ animeIdx ] ) || bForcePlayAnime )
    {
      m_playIconAnimeIdx = m_initData.a_icon_anime_id[ animeIdx ];
      m_pIcon->StartAnim( m_playIconAnimeIdx );
    }
  }

  //  汎用アニメ再生
  void BattleUIActSelectLowerDraw::PokeObj:: StartAnime( const PokeIconAnime animeIdx )
  {
    if( m_pIcon == NULL )
    {
      return;
    }

    m_playIconAnimeIdx = m_initData.a_icon_anime_id[ animeIdx ];
    m_pIcon->StartAnim( m_playIconAnimeIdx );
  }

  //  アルファ設定
  void BattleUIActSelectLowerDraw::PokeObj::SetAlpha( const u32 alpha )
  {
    ::app::util::G2DUtil* pG2DUtil  = m_pRootView->GetG2DUtil();

    pG2DUtil->SetPaneAlpha( m_pIcon->GetPicturePane(), alpha );
  }

  //  アイコン向き指定
  void BattleUIActSelectLowerDraw::PokeObj::SetDirType( const IconDirType dir )
  {
    if( m_pIcon == NULL )
    {
      return;
    }

    PokeIconAnime animeId = POKE_ICON_ANIME_DIR_RIGHT;
    if( dir == ICON_DIR_TYPE_LEFT )
    {
      animeId = POKE_ICON_ANIME_DIR_LEFT;
    }

    StartAnime( animeId );
  }

  void BattleUIActSelectLowerDraw::PokeObj::Update()
  {
    if( m_pIcon != NULL )
    {
      m_pIcon->Update();
    }
  }

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )
