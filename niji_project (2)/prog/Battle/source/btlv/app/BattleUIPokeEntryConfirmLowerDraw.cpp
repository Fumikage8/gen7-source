//=============================================================================
/**
 * @file BattleUIPokeEntryConfirmLowerDraw.h
 * @brief バトル下画面 >> 「バトル参加確認画面」画面
 * @author yuto_uchida
 * @date 2015.04.23
 */
//=============================================================================
#include "BattleUIPokeEntryConfirmLowerDraw.h"
#include "BattleUIViewDrawOrder.h"

#include "niji_conv_header/app/battle/res2d/opplow.h"
#include "niji_conv_header/app/battle/res2d/opplow_pane.h"
#include "niji_conv_header/app/battle/res2d/opplow_anim_list.h"
#include "niji_conv_header/message/msg_btl_app.h"

//  nijiのインクルード
#include "AppLib/include/Tool/AppCommonGrpIconData.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUIPokeEntryConfirmLowerDraw::BattleUIPokeEntryConfirmLowerDraw( const INIT_PARAM& param ) :
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
  BattleUIPokeEntryConfirmLowerDraw::~BattleUIPokeEntryConfirmLowerDraw()
  {
    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  void BattleUIPokeEntryConfirmLowerDraw::Initialize()
  {
    _Initialize2D( m_initParam.pLytBuff );

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    gfl2::lyt::LytMultiResID& rMultiResID = pG2DUtil->GetLayoutResourceID( 0 );

    gfl2::lyt::LytWk*  pLayoutWork = pG2DUtil->GetLayoutWork( LYT_WK_TYPE_SCREEN );
    GFL_ASSERT( pLayoutWork );

    //  ボタン制御クラス作成
    {
      m_initParam.pButtonCtrl->_Setup( this, LYT_WK_TYPE_SCREEN, &rMultiResID, m_initParam.pUtilHeap, m_initParam.bUseInputSE );

      //  通知設定
      UIView::SetInputListener( m_initParam.pButtonCtrl );
    }

    //  内容表示するペインパーツを取得
    gfl2::lyt::LytParts*  pPlayerUnitPartsPane  = pLayoutWork->GetPartsPane( PANENAME_BATTLE_OPP_LOW_000_PANE_PARTSUNIT_00 );
    GFL_ASSERT( pPlayerUnitPartsPane );

    //  プレイヤー名とレートのテキストペインは更新たびに必要になるのであらかじめ確保
    {
      //  プレイヤー名のペイン取得
      m_pPlayerNameTextPane = pLayoutWork->GetTextBoxPane( pPlayerUnitPartsPane, PANENAME_BATTLE_PLT_LOW_004_PANE_TEXTBOX_00, &rMultiResID );
      GFL_ASSERT( m_pPlayerNameTextPane );

      //  レート値のペイン取得
      m_pRateNumberTextPane = pLayoutWork->GetTextBoxPane( pPlayerUnitPartsPane, PANENAME_BATTLE_PLT_LOW_004_PANE_TEXTBOX_02, &rMultiResID );
      GFL_ASSERT( m_pRateNumberTextPane );
    }

    //  レートタイトル名を設定
    {
      m_pRateTitleTextPane  = pLayoutWork->GetTextBoxPane( pPlayerUnitPartsPane, PANENAME_BATTLE_PLT_LOW_004_PANE_TEXTBOX_01, &rMultiResID );
      GFL_ASSERT( m_pRateTitleTextPane );
      pG2DUtil->SetTextBoxPaneString( m_pRateTitleTextPane, str_btl_rate );
    }

    //  各ポケモンオブジェクトを作成
    {
      u32 loopNum = GFL_NELEMS( m_aPokeObj );
      for( u32 i = 0; i < loopNum; ++i )
      {
        _CreatePokeData( i );
      }
    }

    UIView::SetVisible( false );
  }

  //------------------------------------------------------------------
  /**
    * @brief   終了処理
    */
  //------------------------------------------------------------------
  void BattleUIPokeEntryConfirmLowerDraw::Terminate()
  {
    u32 loopNum = GFL_NELEMS( m_aPokeObj );
    for( u32 i = 0; i < loopNum; ++i )
    {
      m_aPokeObj[ i ].Terminate();
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   破棄可能か
    */
  //------------------------------------------------------------------
  bool BattleUIPokeEntryConfirmLowerDraw::IsDelete()
  {
    bool  bRet  = true;

    u32 loopNum = GFL_NELEMS( m_aPokeObj );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( m_aPokeObj[ i ].IsDelete() == false )
      {
        bRet  = false;
        break;
      }
    }

    if( UIView::IsDrawing() == true )
    {
      bRet  = false;
    }

    return  bRet;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 更新処理
    */
  //-------------------------------------------------------------------------
  void BattleUIPokeEntryConfirmLowerDraw::Update( void )
  {
    _UpdateState();

    UIView::Update();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 描画
    */
  //-------------------------------------------------------------------------
  void BattleUIPokeEntryConfirmLowerDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    for( s32 i = 0; i < LYT_WK_TYPE_MAX; ++i )
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderingManager, displayNo, i, LOW_VIEW_ORDER_OTHER );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   描画パラメータ設定
    * @note
    *    描画するパラメータを設定、矢印表示をするかどうかもここで決める
    */
  //------------------------------------------------------------------
  void BattleUIPokeEntryConfirmLowerDraw::SetParam( const PARTY_DATA& rPartyData, const bool bLeftArrowButtonVisible, const bool bRightArrowButtonVisible )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    //  各ポケモンオブジェクトを描画設定
    {
      u32 loopNum = GFL_NELEMS( m_aPokeObj );
      GFL_ASSERT( loopNum == GFL_NELEMS( rPartyData.poke_data ) );

      bool  bSickVisible  = false;
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_aPokeObj[ i ].SetDrawParam( &rPartyData.poke_data[ i ] );
        if( m_aPokeObj[ i ].IsVisibleSick() == true )
        {
          bSickVisible  = true;
        }
      }

      //  状態異常のペインが表示しているのでれあれば状態異常のアニメ演出再生
      pG2DUtil->StopAnime( LYT_WK_TYPE_SCREEN, LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00_KEEP_SICK );
      if( bSickVisible == true )
      {
        pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00_KEEP_SICK );
      }
    }

    if( rPartyData.b_enemy_party == true )
    {
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00_COLOR01 );
    }
    else
    {
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00_COLOR00 );
    }

    //  プレイヤー名設定
    {
      if( rPartyData.p_player_name != NULL )
      {
        pG2DUtil->GetWordSet()->RegisterWord( 0, *rPartyData.p_player_name, rPartyData.player_grammer );
        pG2DUtil->SetTextBoxPaneStringExpand( m_pPlayerNameTextPane, STR_TRAINER_NAME );
      }
      else
      {
        pG2DUtil->SetTextBoxPaneStringExpand( m_pPlayerNameTextPane, STR_VS_PLAYER_NAME );
      }
    }

    //  レート値設定( レート値は非表示のケースもある )
    if( rPartyData.b_rating == true )
    {
      pG2DUtil->SetPaneVisible( m_pRateTitleTextPane, true );
      pG2DUtil->SetPaneVisible( m_pRateNumberTextPane, true );

      //  レート値は0埋めにする
      pG2DUtil->SetTextBoxPaneNumber( m_pRateNumberTextPane, STR_VALUE_4, rPartyData.rate, 4, 0, print::NUM_DISP_ZERO );
    }
    else
    {
      pG2DUtil->SetPaneVisible( m_pRateTitleTextPane, false );
      pG2DUtil->SetPaneVisible( m_pRateNumberTextPane, false );
    }

    //  矢印ボタン描画設定
    {
      m_initParam.pButtonCtrl->SetButtonVisible( BattleUIPokeEntryConfirmtLowerButtonCtrl::BUTTON_ID_LEFT_ARROW, bLeftArrowButtonVisible );
      m_initParam.pButtonCtrl->SetButtonVisible( BattleUIPokeEntryConfirmtLowerButtonCtrl::BUTTON_ID_RIGHT_ARROW, bRightArrowButtonVisible );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   描画切り替えイベント
    * @note
    *    描画するパラメータを設定、矢印表示をするかどうかもここで決める
    */
  //------------------------------------------------------------------
  void BattleUIPokeEntryConfirmLowerDraw::ChangeParamEvent( const PARTY_DATA& rPartyData, const bool bLeftArrowButtonVisible, const bool bRightArrowButtonVisible, const ParamChangeEventType eventType )
  {
    gfl2::std::MemCopy( & rPartyData, &m_changeParamEventData.partyData, sizeof( rPartyData ) );
    m_changeParamEventData.bLeftArrowButton   = bLeftArrowButtonVisible;
    m_changeParamEventData.bRightArrowButton  = bRightArrowButtonVisible;

    if( eventType == PARAM_CHANGE_EVENT_TYPE_LEFT_MOVE )
    {
      m_changeParamEventData.inAnimeId  = LA_OPPLOW_BATTLE_OPP_LOW_000_RIGHT_IN;
      // @fix GFNMCat[1412] 挙動に違和感があるので左にぬけて左から来るように
      m_changeParamEventData.outAnimeId = LA_OPPLOW_BATTLE_OPP_LOW_000_LEFT_OUT;
    }
    else
    if( eventType == PARAM_CHANGE_EVENT_TYPE_RIGHT_MOVE )
    {
      m_changeParamEventData.inAnimeId  = LA_OPPLOW_BATTLE_OPP_LOW_000_LEFT_IN;
      // @fix GFNMCat[1412] 挙動に違和感があるので右にぬけて右から来るように
      m_changeParamEventData.outAnimeId = LA_OPPLOW_BATTLE_OPP_LOW_000_RIGHT_OUT;
    }
    else
    {
      GFL_ASSERT( 0 );
    }

    _SetState( STATE_TYPE_DATA_CHANGE );
  }

  /**
    * @brief レイアウトワークを生成する
    */
  void BattleUIPokeEntryConfirmLowerDraw::_Initialize2D( void* pAppLytBuff )
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
            LYTRES_OPPLOW_BATTLE_OPP_LOW_000_BFLYT,
            LA_OPPLOW_BATTLE_OPP_LOW_000___NUM,
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
  void BattleUIPokeEntryConfirmLowerDraw::_UpdateState()
  {
    if( m_state == STATE_TYPE_NONE )
    {
    }

    if( m_state == STATE_TYPE_IN )
    {
      ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
      if( pG2DUtil->IsAnimeEnd( LYT_WK_TYPE_SCREEN, LA_OPPLOW_BATTLE_OPP_LOW_000_IN ) == true )
      {
        //  アニメの終了を検知
        _SetState( STATE_TYPE_INPUT_IDLE );
      }
    }

    if( m_state == STATE_TYPE_INPUT_IDLE )
    {
    }

    //  データ切り替え
    if( m_state == STATE_TYPE_DATA_CHANGE )
    {
      enum
      {
        SUB_STATE_TYPE_DATA_CHANGE_OUT_ANIME = 0,
        SUB_STATE_TYPE_DATA_CHANGE_OUT_ANIME_WAIT,

        SUB_STATE_TYPE_DATA_CHANGE_IN_ANIME,
        SUB_STATE_TYPE_DATA_CHANGE_IN_ANIME_WAIT,

        SUB_STATE_TYPE_DATA_CHANGE_END,
      };

      if( m_subState == SUB_STATE_TYPE_DATA_CHANGE_OUT_ANIME )
      {
        ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
        pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, m_changeParamEventData.outAnimeId );

        ++m_subState;
      }

      if( m_subState == SUB_STATE_TYPE_DATA_CHANGE_OUT_ANIME_WAIT )
      {
        ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
        if( pG2DUtil->IsAnimeEnd( LYT_WK_TYPE_SCREEN, m_changeParamEventData.outAnimeId ) == true )
        {
          SetParam( m_changeParamEventData.partyData, m_changeParamEventData.bLeftArrowButton, m_changeParamEventData.bRightArrowButton );
          ++m_subState;
        }
      }

      if( m_subState == SUB_STATE_TYPE_DATA_CHANGE_IN_ANIME )
      {
        ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
        pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, m_changeParamEventData.inAnimeId );

        ++m_subState;
      }

      if( m_subState == SUB_STATE_TYPE_DATA_CHANGE_IN_ANIME_WAIT )
      {
        ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
        if( pG2DUtil->IsAnimeEnd( LYT_WK_TYPE_SCREEN, m_changeParamEventData.inAnimeId ) == true )
        {
          ++m_subState;
        }
      }

      if( m_subState == SUB_STATE_TYPE_DATA_CHANGE_END )
      {
        _SetState( STATE_TYPE_INPUT_IDLE );
      }
    }

    if( m_state == STATE_TYPE_OUT )
    {
      if( UIView::IsVisible() == true )
      {
        ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
        if( pG2DUtil->IsAnimeEnd( LYT_WK_TYPE_SCREEN, LA_OPPLOW_BATTLE_OPP_LOW_000_OUT ) == true )
        {
          //  アニメの終了を検知
          _SetState( STATE_TYPE_END );
        }
      }
      else
      {
        //  アニメの終了を検知
        _SetState( STATE_TYPE_END );
      }
    }

    if( m_state != STATE_TYPE_END ) 
    {
      u32 loopNum = GFL_NELEMS( m_aPokeObj );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_aPokeObj[ i ].Update();
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void BattleUIPokeEntryConfirmLowerDraw::_SetState( const BattleUIPokeEntryConfirmLowerDraw::StateType newStateType )
  {
    GFL_ASSERT( m_state != newStateType );

    m_state = newStateType;
    m_subState  = 0;

    if( m_state == STATE_TYPE_NONE )
    {
    }
    else
    if( m_state == STATE_TYPE_IN )
    {
      //  開始なので表示
      UIView::SetVisible( true );
      UIView::SetInputEnabled( false );
      UIView::SetInputExclusiveKeyByButtonAnimation( true );

      //  inアニメ
      {
        ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
        pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_OPPLOW_BATTLE_OPP_LOW_000_IN );
      }
    }
    else
    if( m_state == STATE_TYPE_INPUT_IDLE )
    {
      UIView::SetInputEnabled( true );
    }
    else
    if( m_state == STATE_TYPE_DATA_CHANGE )
    {
      UIView::SetInputEnabled( false );
    }
    else
    if( m_state == STATE_TYPE_OUT )
    {
      //  outアニメ
      {
        ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
        pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_OPPLOW_BATTLE_OPP_LOW_000_OUT );
      }

      UIView::SetInputEnabled( false );
    }
    else
    if( m_state == STATE_TYPE_END )
    {
      UIView::SetInputEnabled( false );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   ポケモンオブジェクト作成
    */
  //------------------------------------------------------------------
  BattleUIPokeEntryConfirmLowerDraw::PokeObj* BattleUIPokeEntryConfirmLowerDraw::_CreatePokeData( const u32 pokeDataIdx )
  {
    GFL_ASSERT( pokeDataIdx < GFL_NELEMS( m_aPokeObj ) );

    PokeObj*  pPokeObj  = &m_aPokeObj[ pokeDataIdx ];

    gfl2::lyt::LytWk* pLayoutWork = UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN );

    gfl2::lyt::LytParts*  pUnitPartsPane  = pLayoutWork->GetPartsPane( PANENAME_BATTLE_OPP_LOW_000_PANE_PARTSUNIT_00 );
    GFL_ASSERT( pUnitPartsPane );

    gfl2::lyt::LytMultiResID& rMultiResId = UIView::GetG2DUtil()->GetLayoutResourceID( 0 );

    //  ローカルで使用する、作成データ構造体
    typedef struct
    {
      u32 draw_root_pane_id;
      u32 poke_icon_parts_pane_id;
      u32 ly_title_text_pane_id;
      u32 lv_num_text_pane_id;
      u32 gender_parts_pane_id;
      u32 poke_not_join_pane_id;
      u32 sick_root_pane_id;
      u32 sick_icon_pane_id;

      u32 a_anime_id[ PokeObj::POKE_ICON_ANIME_MAX ];
    } LOCAL_CREATE_DATA;

    const LOCAL_CREATE_DATA aCreateData[] = 
    {
      //  1番目
      {
        PANENAME_BATTLE_PLT_LOW_004_PANE_POKESET_00,
        PANENAME_BATTLE_PLT_LOW_004_PANE_POKEICON_00,
        PANENAME_BATTLE_PLT_LOW_004_PANE_TEXTLV_00,
        PANENAME_BATTLE_PLT_LOW_004_PANE_POKELV_00,
        PANENAME_BATTLE_PLT_LOW_004_PANE_GENDER_00,
        PANENAME_BATTLE_PLT_LOW_004_PANE_INACTIVE_00,
        PANENAME_BATTLE_PLT_LOW_004_PANE_PARTSSICK_00,
        PANENAME_COMMON_ICN_002_PANE_ICON_SICK,

        {
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__POKEICON_00_DIE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__POKEICON_00_REVIVE,

          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__GENDER_00_ICON_MALE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__GENDER_00_ICON_FEMALE,
        }
      },

      //  2番目
      {
        PANENAME_BATTLE_PLT_LOW_004_PANE_POKESET_01,
        PANENAME_BATTLE_PLT_LOW_004_PANE_POKEICON_01,
        PANENAME_BATTLE_PLT_LOW_004_PANE_TEXTLV_01,
        PANENAME_BATTLE_PLT_LOW_004_PANE_POKELV_01,
        PANENAME_BATTLE_PLT_LOW_004_PANE_GENDER_01,
        PANENAME_BATTLE_PLT_LOW_004_PANE_INACTIVE_01,
        PANENAME_BATTLE_PLT_LOW_004_PANE_PARTSSICK_01,
        PANENAME_COMMON_ICN_002_PANE_ICON_SICK,

        {
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__POKEICON_01_DIE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__POKEICON_01_REVIVE,

          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__GENDER_01_ICON_MALE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__GENDER_01_ICON_FEMALE,
        }
      },

      //  3番目
      {
        PANENAME_BATTLE_PLT_LOW_004_PANE_POKESET_02,
        PANENAME_BATTLE_PLT_LOW_004_PANE_POKEICON_02,
        PANENAME_BATTLE_PLT_LOW_004_PANE_TEXTLV_02,
        PANENAME_BATTLE_PLT_LOW_004_PANE_POKELV_02,
        PANENAME_BATTLE_PLT_LOW_004_PANE_GENDER_02,
        PANENAME_BATTLE_PLT_LOW_004_PANE_INACTIVE_02,
        PANENAME_BATTLE_PLT_LOW_004_PANE_PARTSSICK_02,
        PANENAME_COMMON_ICN_002_PANE_ICON_SICK,

        {
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__POKEICON_02_DIE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__POKEICON_02_REVIVE,

          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__GENDER_02_ICON_MALE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__GENDER_02_ICON_FEMALE,
        }
      },

      //  4番目
      {
        PANENAME_BATTLE_PLT_LOW_004_PANE_POKESET_03,
        PANENAME_BATTLE_PLT_LOW_004_PANE_POKEICON_03,
        PANENAME_BATTLE_PLT_LOW_004_PANE_TEXTLV_03,
        PANENAME_BATTLE_PLT_LOW_004_PANE_POKELV_03,
        PANENAME_BATTLE_PLT_LOW_004_PANE_GENDER_03,
        PANENAME_BATTLE_PLT_LOW_004_PANE_INACTIVE_03,
        PANENAME_BATTLE_PLT_LOW_004_PANE_PARTSSICK_03,
        PANENAME_COMMON_ICN_002_PANE_ICON_SICK,

        {
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__POKEICON_03_DIE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__POKEICON_03_REVIVE,

          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__GENDER_03_ICON_MALE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__GENDER_03_ICON_FEMALE,
        }
      },

      //  5番目
      {
        PANENAME_BATTLE_PLT_LOW_004_PANE_POKESET_04,
        PANENAME_BATTLE_PLT_LOW_004_PANE_POKEICON_04,
        PANENAME_BATTLE_PLT_LOW_004_PANE_TEXTLV_04,
        PANENAME_BATTLE_PLT_LOW_004_PANE_POKELV_04,
        PANENAME_BATTLE_PLT_LOW_004_PANE_GENDER_04,
        PANENAME_BATTLE_PLT_LOW_004_PANE_INACTIVE_04,
        PANENAME_BATTLE_PLT_LOW_004_PANE_PARTSSICK_04,
        PANENAME_COMMON_ICN_002_PANE_ICON_SICK,

        {
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__POKEICON_04_DIE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__POKEICON_04_REVIVE,

          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__GENDER_04_ICON_MALE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__GENDER_04_ICON_FEMALE,
        }
      },

      //  6番目
      {
        PANENAME_BATTLE_PLT_LOW_004_PANE_POKESET_05,
        PANENAME_BATTLE_PLT_LOW_004_PANE_POKEICON_05,
        PANENAME_BATTLE_PLT_LOW_004_PANE_TEXTLV_05,
        PANENAME_BATTLE_PLT_LOW_004_PANE_POKELV_05,
        PANENAME_BATTLE_PLT_LOW_004_PANE_GENDER_05,
        PANENAME_BATTLE_PLT_LOW_004_PANE_INACTIVE_05,
        PANENAME_BATTLE_PLT_LOW_004_PANE_PARTSSICK_05,
        PANENAME_COMMON_ICN_002_PANE_ICON_SICK,

        {
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__POKEICON_05_DIE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__POKEICON_05_REVIVE,

          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__GENDER_05_ICON_MALE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTSUNIT_00__GENDER_05_ICON_FEMALE,
        }
      }
    };

    GFL_ASSERT( pokeDataIdx < GFL_NELEMS( aCreateData ) );
    const LOCAL_CREATE_DATA&  rActiveCreateData = aCreateData[ pokeDataIdx ];

    //  初期化パラメータ
    PokeObj::INIT_PARAM initParam;
    {
      initParam.p_root_pane           = pLayoutWork->GetPane( pUnitPartsPane, rActiveCreateData.draw_root_pane_id, &rMultiResId );
      GFL_ASSERT( initParam.p_root_pane );

      initParam.p_poke_icon_pane      = pLayoutWork->GetPartsPane( pUnitPartsPane, rActiveCreateData.poke_icon_parts_pane_id, &rMultiResId );
      GFL_ASSERT( initParam.p_poke_icon_pane );

      initParam.p_lv_num_pane         = pLayoutWork->GetTextBoxPane( pUnitPartsPane, rActiveCreateData.lv_num_text_pane_id, &rMultiResId );
      GFL_ASSERT( initParam.p_lv_num_pane );

      initParam.p_lv_title_text_pane  = pLayoutWork->GetTextBoxPane( pUnitPartsPane, rActiveCreateData.ly_title_text_pane_id, &rMultiResId );
      GFL_ASSERT( initParam.p_lv_title_text_pane );

      initParam.p_gender_parts_pane   = pLayoutWork->GetPartsPane( pUnitPartsPane, rActiveCreateData.gender_parts_pane_id, &rMultiResId );
      GFL_ASSERT( initParam.p_gender_parts_pane );

      initParam.p_not_join_pane       = pLayoutWork->GetPane( pUnitPartsPane, rActiveCreateData.poke_not_join_pane_id, &rMultiResId );
      GFL_ASSERT( initParam.p_not_join_pane );

      initParam.p_sick_root_pane       = pLayoutWork->GetPartsPane( pUnitPartsPane, rActiveCreateData.sick_root_pane_id, &rMultiResId );
      GFL_ASSERT( initParam.p_sick_root_pane );

      initParam.p_sick_pic_pane       = pLayoutWork->GetPicturePane( initParam.p_sick_root_pane, rActiveCreateData.sick_icon_pane_id, &rMultiResId );
      GFL_ASSERT( initParam.p_sick_pic_pane );

      ::std::memcpy( initParam.a_anime_id, aCreateData[ pokeDataIdx ].a_anime_id, sizeof( initParam.a_anime_id ) );

      initParam.p_common_grp_icon_data  = m_initParam.pAppCommonGrpIconData;
      GFL_ASSERT( initParam.p_common_grp_icon_data );
    }

    pPokeObj->Initialize( this, m_initParam.pUtilHeap, LYT_WK_TYPE_SCREEN, &rMultiResId, initParam );

    return  pPokeObj;
  }

//  ここからは PokeObjクラス定義

  void BattleUIPokeEntryConfirmLowerDraw::PokeObj::Initialize( UIView* pView, ::app::util::Heap* pUtilHeap, const u32 lytId, gfl2::lyt::LytMultiResID* pMultiResId, const INIT_PARAM& rInitParam )
  {
    GFL_ASSERT( pView );
    GFL_ASSERT( pUtilHeap );

    ::app::util::G2DUtil* pG2DUtil  = pView->GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    //  ポケモンアイコン作成
    ::app::tool::PokeIconToolEx*  pPokeIconTool = NULL;
    {
      ::app::tool::PokeIconToolEx::PokeIconToolParam  initParam;
      initParam.layout_id = lytId;
      initParam.pHeap     = pUtilHeap;
      initParam.pUIView   = pView;
      initParam.res_id    = 0;
      initParam.iconMax   = 1;
      initParam.sync      = false;

      pPokeIconTool = GFL_NEW( pUtilHeap->GetDeviceHeap() ) ::app::tool::PokeIconToolEx( initParam );
      GFL_ASSERT( pPokeIconTool );
    }

    //  アニメidの設定
    {
      for( s32 i = 0; i < POKE_ICON_ANIME_MAX; ++i )
      {
        m_aAnimeIdx[ i ]  = rInitParam.a_anime_id[ i ];
      }
    }

    m_initParam       = rInitParam;
    m_pIcon           = pPokeIconTool;
    m_pRootView       = pView;
    m_lytId           = lytId;

    //  アイテム所持しているかどうかのアイコンペイン取得
    {
      gfl2::lyt::LytWk* pLayoutWork = pView->GetLayoutWork( m_lytId );
      GFL_ASSERT( pLayoutWork );

      m_pHaveItemIcon   = pLayoutWork->GetPane( m_initParam.p_poke_icon_pane, PANENAME_COMMON_BTN_LOW_001_PANE_ICON_ITEM, pMultiResId );
      GFL_ASSERT( m_pHaveItemIcon );
    }
  }

  //  更新
  void BattleUIPokeEntryConfirmLowerDraw::PokeObj::Update()
  {
    if( m_pIcon != NULL )
    {
      if( m_pIcon->IsReplaced( 0 ) == true )
      {
       ::app::util::G2DUtil* pG2DUtil  = m_pRootView->GetG2DUtil();

        pG2DUtil->SetPaneVisible( m_initParam.p_poke_icon_pane, true );
      }

      m_pIcon->Update();
    }
  }

  //  表示パラメータ設定
  void BattleUIPokeEntryConfirmLowerDraw::PokeObj::SetDrawParam( const POKE_DATA* pPokeData )
  {
    GFL_ASSERT( m_pIcon );

    ::app::util::G2DUtil* pG2DUtil  = m_pRootView->GetG2DUtil();

    if( ( pPokeData == NULL ) || ( pPokeData->p_poke_param == NULL ) )
    {
      //  非表示して終了
      pG2DUtil->SetPaneVisible( m_initParam.p_root_pane, false );

      if( m_initParam.p_not_join_pane != NULL )
      {
        pG2DUtil->SetPaneVisible( m_initParam.p_not_join_pane, false );
      }

      return;
    }

    pG2DUtil->SetPaneVisible( m_initParam.p_root_pane, true );

    const pml::pokepara::PokemonParam*  pSrcPokemonParam  = pPokeData->p_poke_param;

    //  高速ロードモードに
    bool  bFastMode = pSrcPokemonParam->StartFastMode();

    if( m_initParam.p_not_join_pane != NULL )
    {
      pG2DUtil->StopAnime( m_lytId, m_initParam.a_anime_id[ POKE_ICON_ANIME_DIE ] );
      pG2DUtil->StopAnime( m_lytId, m_initParam.a_anime_id[ POKE_ICON_ANIME_REVIVE ] );

      if( pPokeData->b_on_battle_field == false )
      {
        //  非参加している時の専用表示
        pG2DUtil->StartAnime( m_lytId, m_initParam.a_anime_id[ POKE_ICON_ANIME_DIE ] );
        pG2DUtil->SetPaneVisible( m_initParam.p_not_join_pane, true );
      }
      else
      {
        pG2DUtil->StartAnime( m_lytId, m_initParam.a_anime_id[ POKE_ICON_ANIME_REVIVE ] );
        pG2DUtil->SetPaneVisible( m_initParam.p_not_join_pane, false );
      }
    }

    //  アイコン設定
    {
      m_pIcon->SetPokeIcon( 0, pPokeData->p_poke_param, PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON, m_initParam.p_poke_icon_pane );
      pG2DUtil->SetPaneVisible( m_initParam.p_poke_icon_pane, false );
    }

    //  レベル設定
    {
      //  レベル値設定
      // @fix GFNMCat[2245] レベル値が３桁なのを忘れていた。
      pG2DUtil->SetTextBoxPaneNumber( m_initParam.p_lv_num_pane, STR_VALUE_3, pSrcPokemonParam->GetLevel(), 3, 0 );
    }

    //  性別設定
    {
      pml::Sex  nowSex  = pSrcPokemonParam->GetSex();

      pG2DUtil->SetPaneVisible( m_initParam.p_gender_parts_pane, true );

      pG2DUtil->StopAnime( m_lytId, m_initParam.a_anime_id[ POKE_ICON_GENDER_SEX_MALE ] );
      pG2DUtil->StopAnime( m_lytId, m_initParam.a_anime_id[ POKE_ICON_GENDER_SEX_FEMALE ] );

      if( nowSex == pml::SEX_MALE )
      {
        //  オス
        pG2DUtil->StartAnime( m_lytId, m_initParam.a_anime_id[ POKE_ICON_GENDER_SEX_MALE ] );
      }
      else if( nowSex == pml::SEX_FEMALE )
      {
        //  メス
        pG2DUtil->StartAnime( m_lytId, m_initParam.a_anime_id[ POKE_ICON_GENDER_SEX_FEMALE ] );
      }
      else
      {
        //  それ以外
        //  非表示にする
        pG2DUtil->SetPaneVisible( m_initParam.p_gender_parts_pane, false );
      }
    }

    //  アイテムを所持している場合は所持アイコンを表示する
    {
      if( pSrcPokemonParam->GetItem() != ITEM_DUMMY_DATA )
      {
        pG2DUtil->SetPaneVisible( m_pHaveItemIcon, true );
      }
      else
      {
        pG2DUtil->SetPaneVisible( m_pHaveItemIcon, false );
      }
    }

    //  状態異常であれば表示
    if( pSrcPokemonParam->GetSick() != pml::pokepara::SICK_NULL )
    {
      //  表示
      pG2DUtil->SetPaneVisible( m_initParam.p_sick_root_pane, true );
      m_initParam.p_common_grp_icon_data->ReplacePaneTextureByPokeSickIcon( pSrcPokemonParam->GetSick(), m_initParam.p_sick_pic_pane );
    }
    else
    {
      pG2DUtil->SetPaneVisible( m_initParam.p_sick_root_pane, false );
    }

    //  高速ロードモードを終了
    pSrcPokemonParam->EndFastMode( bFastMode );
  }
  
  //  状態異常表示しているかどうか
  bool BattleUIPokeEntryConfirmLowerDraw::PokeObj::IsVisibleSick()
  {
    ::app::util::G2DUtil* pG2DUtil  = m_pRootView->GetG2DUtil();

    return  pG2DUtil->GetPaneVisible( m_initParam.p_sick_root_pane );
  }

  //  ポケモンアイコンアニメ再生v
  void BattleUIPokeEntryConfirmLowerDraw::PokeObj::StartAnimePokeIcon( const PokeIconAnime animeIdx, const bool bForcePlayAnime )
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

//  ここから先は BattleUIPokeEntryConfirmtLowerButtonCtrl の定義

  //  ボタン関連のアニメが終わっているか
  bool BattleUIPokeEntryConfirmtLowerButtonCtrl::IsAnimEnd()
  {
    GFL_ASSERT( m_pView );
    return  ( m_pView->IsButtonAnimation() == false );
  }

  //  ボタンを押す
  void BattleUIPokeEntryConfirmtLowerButtonCtrl::Push( const ButtonId id )
  {
    GFL_ASSERT( m_pDelegate );
    GFL_ASSERT( m_pView );

    m_pView->GetButtonManager()->StartSelectedAct( id );
  }

  //  ボタン表示設定
  void BattleUIPokeEntryConfirmtLowerButtonCtrl::SetButtonVisible( const ButtonId id, const bool bFlg )
  {
    if( bFlg )
    {
      m_pView->GetButtonManager()->SetButtonVisible( id, true );
    }
    else
    {
      m_pView->GetButtonManager()->SetButtonInvisible( id );
    }
  }

  // ペインタッチ時を検知
  ::app::ui::UIInputListener::ListenerResult BattleUIPokeEntryConfirmtLowerButtonCtrl::OnLayoutPaneEvent( const u32 buttonId )
  {
    if( m_pDelegate != NULL )
    {
      if( m_pDelegate->OnEnterInput( m_pView, this, buttonId ) )
      {
        ::app::util::G2DUtil* pG2DUtil  = m_pView->GetG2DUtil();
        if( buttonId == BUTTON_ID_LEFT_ARROW )
        {
          pG2DUtil->StartAnime( m_lytWkId, LA_OPPLOW_BATTLE_OPP_LOW_000__CUR_L_SELECT );
        }
        else
        if( buttonId == BUTTON_ID_RIGHT_ARROW )
        {
          pG2DUtil->StartAnime( m_lytWkId, LA_OPPLOW_BATTLE_OPP_LOW_000__CUR_R_SELECT );
        }
        else
        if( buttonId == BUTTON_ID_CANCEL_02 )
        {
          m_pView->GetButtonManager()->StartSelectedAct( buttonId );
        }
        else
        if( buttonId == BUTTON_ID_CANCEL )
        {
          m_pView->GetButtonManager()->StartSelectedAct( buttonId );
        }

        return DISABLE_ACCESS;
      }
    }

    return ENABLE_ACCESS;
  }

  /// キー入力通知用リスナ
  ::app::ui::UIInputListener::ListenerResult BattleUIPokeEntryConfirmtLowerButtonCtrl::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    GFL_ASSERT( m_pDelegate );

    if( m_pDelegate->OnKeyAction( m_pView, this, pButton, pKey, pStick ) )
    {
      return DISABLE_ACCESS;
    }

    return ENABLE_ACCESS;
  }

  //------------------------------------------------------------------
  /**
    * @brief   処理前に必ず呼ぶが、これはViewクラス内で呼ぶ前提で作成している
    */
  //------------------------------------------------------------------
  void BattleUIPokeEntryConfirmtLowerButtonCtrl::_Setup( BattleUIPokeEntryConfirmLowerDraw* pView, const s32 useLytWkId, gfl2::lyt::LytMultiResID* pMultiResId, ::app::util::Heap* pUtilHeap, const bool bUseInputSE )
  {
    GFL_ASSERT( pView );
    GFL_ASSERT( pMultiResId );
    GFL_ASSERT( pUtilHeap );

    m_pView   = pView;
    m_lytWkId = useLytWkId;

    ::app::util::G2DUtil* pG2DUtil  = m_pView->GetG2DUtil();

    gfl2::lyt::LytWk*  pLayoutWork = pG2DUtil->GetLayoutWork( useLytWkId );
    GFL_ASSERT( pLayoutWork );

    //  ボタン作成
    {
      gfl2::lyt::LytParts*  pCurLeftPartsPane       = pLayoutWork->GetPartsPane( PANENAME_BATTLE_OPP_LOW_000_PANE_CUR_L );
      gfl2::lyt::LytParts*  pCurRightPartsPane      = pLayoutWork->GetPartsPane( PANENAME_BATTLE_OPP_LOW_000_PANE_CUR_R );
      gfl2::lyt::LytParts*  pCancelButtonPartsPane  = pLayoutWork->GetPartsPane( PANENAME_BATTLE_OPP_LOW_000_PANE_PARTS_00 );

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
        //  左矢印ボタン
        {
          BUTTON_ID_LEFT_ARROW,
          pCurLeftPartsPane,
          pLayoutWork->GetBoundingPane( pCurLeftPartsPane, PANENAME_COMMON_CUR_LOW_002_PANE_BOUND_00, pMultiResId ),
          LA_OPPLOW_BATTLE_OPP_LOW_000__CUR_L_TOUCH,
          LA_OPPLOW_BATTLE_OPP_LOW_000__CUR_L_RELEASE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__CUR_L_CANSEL,
          LA_OPPLOW_BATTLE_OPP_LOW_000__CUR_L_TOUCH_RELEASE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__CUR_L_ACTIVE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__CUR_L_PASSIVE,
        },

        //  右矢印ボタン
        {
          BUTTON_ID_RIGHT_ARROW,
          pCurRightPartsPane,
          pLayoutWork->GetBoundingPane( pCurRightPartsPane, PANENAME_COMMON_CUR_LOW_002_PANE_BOUND_00, pMultiResId ),
          LA_OPPLOW_BATTLE_OPP_LOW_000__CUR_R_TOUCH,
          LA_OPPLOW_BATTLE_OPP_LOW_000__CUR_R_RELEASE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__CUR_R_CANSEL,
          LA_OPPLOW_BATTLE_OPP_LOW_000__CUR_R_TOUCH_RELEASE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__CUR_R_ACTIVE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__CUR_R_PASSIVE,
        },

        //  キャンセルボタン
        {
          BUTTON_ID_CANCEL_02,
          pCancelButtonPartsPane,
          pLayoutWork->GetBoundingPane( pCancelButtonPartsPane, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, pMultiResId ),
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTS_00_TOUCH,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTS_00_RELEASE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTS_00_CANSEL,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTS_00_TOUCH_RELEASE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTS_00_ACTIVE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTS_00_PASSIVE,
        },

        //  背景（背景タッチ後に画面終了させるのでボタンとして扱う）
        {
          BUTTON_ID_CANCEL,
          pLayoutWork->GetPane( PANENAME_BATTLE_OPP_LOW_000_PANE_BG_00 ),
          pLayoutWork->GetPane( PANENAME_BATTLE_OPP_LOW_000_PANE_BG_00 ),
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTS_00_TOUCH,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTS_00_RELEASE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTS_00_CANSEL,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTS_00_TOUCH_RELEASE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTS_00_ACTIVE,
          LA_OPPLOW_BATTLE_OPP_LOW_000__PARTS_00_PASSIVE,
        }
      };

      m_pView->CreateButtonManager( pUtilHeap, pLayoutWork, aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );
    }

        //  ボタンSE設定
    if( bUseInputSE )
    {
      ::app::tool::ButtonManager* pButtonManager  = m_pView->GetButtonManager();

      const u32 aButtonSE[ BUTTON_ID_MAX ][ 2 ] =
      {
        { BUTTON_ID_CANCEL,       SEQ_SE_CANCEL1 },
        { BUTTON_ID_LEFT_ARROW,   SEQ_SE_DECIDE1 },
        { BUTTON_ID_RIGHT_ARROW,  SEQ_SE_DECIDE1 },
      };

      for( u32 i = 0; i < BUTTON_ID_MAX; ++i )
      {
        pButtonManager->SetButtonSelectSE( aButtonSE[ i ][ 0 ], aButtonSE[ i ][ 1 ] );
      }
    }
  }

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )
