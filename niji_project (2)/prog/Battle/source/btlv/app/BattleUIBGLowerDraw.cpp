//=============================================================================
/**
 * @file BattleUIBGLowerDraw.h
 * @brief バトル下画面 >> 「背景」画面
 * @author yuto_uchida
 * @date 2015.03.23
 */
//=============================================================================
#include "BattleUIBGLowerDraw.h"
#include "BattleUIViewDrawOrder.h"

#include "niji_conv_header/app/battle/res2d/bgrlow.h"
#include "niji_conv_header/app/battle/res2d/bgrlow_pane.h"
#include "niji_conv_header/app/battle/res2d/bgrlow_anim_list.h"

#include "AppLib/include/Util/app_util_GetPaneHelper.h"

#include <Sound/include/Sound.h>

//  バトル用のアプリテキスト
#include "niji_conv_header/message/msg_btl_app.h"

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  //  天候アニメid一覧
  const BattleUIBGLowerDraw::WeatherAnimeData BattleUIBGLowerDraw::ms_aWeatherAnimeDataTable[ BTL_WEATHER_MAX ]  =
  {
    //  BTL_WEATHER_NONE = 0,   ///< 天候なし
      { 
        LA_BGRLOW_BATTLE_BGR_LOW_000__BG_WEATHER_KEEP_NORMAL,
      },

    //  BTL_WEATHER_SHINE,    ///< はれ
      { 
        LA_BGRLOW_BATTLE_BGR_LOW_000__BG_WEATHER_KEEP_SUNNY,
      },

    //  BTL_WEATHER_RAIN,     ///< あめ
      {
        LA_BGRLOW_BATTLE_BGR_LOW_000__BG_WEATHER_KEEP_RAIN,
      },

    //  BTL_WEATHER_SNOW,     ///< あられ
      {
        LA_BGRLOW_BATTLE_BGR_LOW_000__BG_WEATHER_KEEP_HAILSTONE,
      },
      
    //  BTL_WEATHER_SAND,     ///< すなあらし
      {
        LA_BGRLOW_BATTLE_BGR_LOW_000__BG_WEATHER_KEEP_SANDSTORM,
      },

    //  BTL_WEATHER_STORM,    ///< おおあめ
      {
        LA_BGRLOW_BATTLE_BGR_LOW_000__BG_WEATHER_KEEP_RAIN_HEAVY,
      },

    //  BTL_WEATHER_DAY,      ///< おおひでり
      {
        LA_BGRLOW_BATTLE_BGR_LOW_000__BG_WEATHER_KEEP_SUNNY_HEAVY,
      },

    //  BTL_WEATHER_TURBULENCE,  ///< 乱気流
      {
        LA_BGRLOW_BATTLE_BGR_LOW_000__BG_WEATHER_KEEP_TURBULENCE,
      },
  };

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUIBGLowerDraw::BattleUIBGLowerDraw( const INIT_PARAM& param ) :
    UIView( param.pUtilHeap, param.pUtilHeap->GetDeviceAllocator(), param.pUtilHeap->GetDeviceAllocator() )
  {
    GFL_ASSERT( param.pUtilHeap );
    GFL_ASSERT( param.pRenderingManager );
    GFL_ASSERT( param.pLytBuff );

    _Clear();

    //  初期化
    {
      m_state     = STATE_TYPE_NONE;
      m_initParam = param;
    }

    _Initialize2D( m_initParam.pLytBuff );

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    gfl2::lyt::LytWk* pLayout             = pG2DUtil->GetLayoutWork( LYT_WK_TYPE_SCREEN );
    gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

    //  天候アイコンのペイン取得
    {
      m_pWeatherObject  = GFL_NEW( param.pUtilHeap->GetDeviceHeap() ) WeatherObject();
      GFL_ASSERT( m_pWeatherObject );

      WeatherObject::INIT_PARAM initParam;
      {
        initParam.lytWorkId           = LYT_WK_TYPE_SCREEN;
        initParam.pG2DUtil            = pG2DUtil;
        initParam.pPartsWeatherBG     = pLayout->GetPartsPane( PANENAME_BATTLE_BGR_LOW_000_PANE_BG_WEATHER );
      }

      m_pWeatherObject->Initialize( initParam );
    }

    //  カバー背景
    {
      m_pCoverBGPane  = pLayout->GetPane( PANENAME_BATTLE_BGR_LOW_000_PANE_BATTLE_BG_01 );
      GFL_ASSERT( m_pCoverBGPane );

      SetVisibleCurverBG( true );
    }

    //  ボタン配置
    {
      ::app::util::GetPaneHelper oteireBtnPaneHelper( pLayout, &rMultiResId );
      oteireBtnPaneHelper.Push( PANENAME_BATTLE_BGR_LOW_000_PANE_BTN_CARE );

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
        //  おていれボタン
        {
          BUTTON_ID_OTEIRE,
          oteireBtnPaneHelper.Peek(),
          oteireBtnPaneHelper.GetBoundingPane( PANENAME_BATTLE_BTN_LOW_006_PANE_BOUND_00 ),
          LA_BGRLOW_BATTLE_BGR_LOW_000__BTN_CARE_TOUCH,
          LA_BGRLOW_BATTLE_BGR_LOW_000__BTN_CARE_RELEASE,
          LA_BGRLOW_BATTLE_BGR_LOW_000__BTN_CARE_CANSEL,
          LA_BGRLOW_BATTLE_BGR_LOW_000__BTN_CARE_TOUCH_RELEASE,
          LA_BGRLOW_BATTLE_BGR_LOW_000__BTN_CARE_ACTIVE,
          LA_BGRLOW_BATTLE_BGR_LOW_000__BTN_CARE_PASSIVE
        },
      };

      UIResponder::CreateButtonManager( m_initParam.pUtilHeap, pG2DUtil->GetLayoutWork( LYT_WK_TYPE_SCREEN ), aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );
 
      //  入力コールバック設定
      UIView::SetInputListener( this );
      UIView::SetInputEnabled( false );

      //  ボタンSE設定
      {
        ::app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();
        pButtonManager->SetButtonSelectSE( BUTTON_ID_OTEIRE, SEQ_SE_DECIDE1 );
      }

      //  ボタン名設定
      {
        //  お手入れ
        pG2DUtil->SetTextBoxPaneString( oteireBtnPaneHelper.GetTextBoxPane( PANENAME_BATTLE_BTN_LOW_006_PANE_TEXTBOX_00 ), str_pokeparure_01_01 );
      }
    }

    //  天候設定
    SetWeather( param.weatherType );

    //  モード切り替える
    SetMode( param.modeType );
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUIBGLowerDraw::~BattleUIBGLowerDraw()
  {
    m_pWeatherObject->Terminate();
    GFL_SAFE_DELETE( m_pWeatherObject );

    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 更新処理
    */
  //-------------------------------------------------------------------------
  void BattleUIBGLowerDraw::Update( void )
  {
    _UpdateState();

    m_pWeatherObject->Update();

    UIView::Update();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 描画
    */
  //-------------------------------------------------------------------------
  void BattleUIBGLowerDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    for( s32 i = 0; i < LYT_WK_TYPE_MAX; ++i )
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderingManager, displayNo, i, LOW_VIEW_ORDER_BG );
    }
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 天候設定
    */
  //-------------------------------------------------------------------------
  void BattleUIBGLowerDraw::SetWeather( const BtlWeather weather )
  {
    m_pWeatherObject->Start( weather );
  }

  //------------------------------------------------------------------
  /**
    * @brief   モード設定
    */
  //------------------------------------------------------------------
  void BattleUIBGLowerDraw::SetMode( const ModeType modeType )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    const s32 aModeAnimeIdxList[ MODE_TYPE_MAX ]  =
    {
      LA_BGRLOW_BATTLE_BGR_LOW_000_BATTLE_NORMAL,
      LA_BGRLOW_BATTLE_BGR_LOW_000_BATTLE_NORMAL,
    };

    pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, aModeAnimeIdxList[ modeType ] );

    m_initParam.modeType = modeType;
  }

  /**
    * @brief BGカバー表示設定
    */
  void BattleUIBGLowerDraw::SetVisibleCurverBG( const bool bFlg )
  {
    GFL_ASSERT( m_pCoverBGPane );

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    pG2DUtil->SetPaneVisible( m_pCoverBGPane, bFlg );
  }

  //------------------------------------------------------------------
  /**
    * @brief   おていれモード開始
    */
  //------------------------------------------------------------------
  void BattleUIBGLowerDraw::StartOteireMode()
  {
    UIView::SetInputEnabled( true );

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_BGRLOW_BATTLE_BGR_LOW_000_IN_CARE );

    m_bStartOteire  = true;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 指定したidのボタンを押す
    */
  //-------------------------------------------------------------------------
  void BattleUIBGLowerDraw::PushBtn( ButtonId buttonId )
  {
    ::app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();

    if( pButtonManager->IsButtonVisible( buttonId ) == false )
    {
      return;
    }

    //  コールバックの OnLayoutPaneEvent 関数が呼ばれる
    pButtonManager->StartSelectedAct( buttonId );

    //  入力が無効の場合はOnLayoutPaneEvent()関数が呼ばれないので手動で呼び出す
    if( UIView::IsInputEnabled() == false )
    {
      OnLayoutPaneEvent( buttonId );
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペイン選択アニメーションの開始を検知
  *       【ボタンマネージャー関係】
  *
  * @param  painId   通知を受けるペインＩＤ
  *
  * @return none
  */
  //--------------------------------------------------------------------------------------------
  void BattleUIBGLowerDraw::OnLayoutPaneStartingAnimeEvent( const u32 buttonId )
  {
    if( m_initParam.pDelegate == NULL )
    {
      return;
    }

    if( m_bStartOteire == false )
    {
      return;
    }

    // @fix NMCat[3350] ボタンアニメが開始された段階でボタン通知
    if( buttonId == BUTTON_ID_OTEIRE )
    {
      m_initParam.pDelegate->OnEnterInput( this, buttonId );
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインイベントの検知
  *       【ボタンマネージャー関係】
  *
  * @param  painId   通知を受けるペインＩＤ
  *
  * @return 同フレーム内での他入力イベントを許可するならtrue、
  *         他のイベントを排除するならfalseを返却すること。
  */
  //--------------------------------------------------------------------------------------------
  // overwride
  ::app::ui::UIInputListener::ListenerResult BattleUIBGLowerDraw::OnLayoutPaneEvent( const u32 buttonId )
  {
    GFL_PRINT( "OnLayoutPaneEvent : buttonId = %d\n", buttonId );
    if( m_initParam.pDelegate == NULL )
    {
      return  ENABLE_ACCESS;
    }

    if( m_bStartOteire == false )
    {
      return  ENABLE_ACCESS;
    }

    if( m_initParam.pDelegate->OnEnterInput( this, buttonId ) == true )
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
  ::app::ui::UIInputListener::ListenerResult BattleUIBGLowerDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    if( m_initParam.pDelegate == NULL )
    {
      return  ENABLE_ACCESS;
    }

    if( m_bStartOteire == false )
    {
      return  ENABLE_ACCESS;
    }

    m_initParam.pDelegate->OnKeyAction( this, pButton, pKey, pStick );

    return ENABLE_ACCESS;
  }

  /**
    * @brief レイアウトワークを生成する
    */
  void BattleUIBGLowerDraw::_Initialize2D( void* pAppLytBuff )
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
            LYTRES_BGRLOW_BATTLE_BGR_LOW_000_BFLYT,
            LA_BGRLOW_BATTLE_BGR_LOW_000___NUM,
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
  void BattleUIBGLowerDraw::_UpdateState()
  {
    if( m_state == STATE_TYPE_NONE )
    {
    }

    if( m_state == STATE_TYPE_IN )
    {
      //  アニメの終了を検知
      _SetState( STATE_TYPE_INPUT_IDLE );
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
  void BattleUIBGLowerDraw::_SetState( const BattleUIBGLowerDraw::StateType newStateType )
  {
    GFL_ASSERT( m_state != newStateType );

    m_state = newStateType;

    if( m_state == STATE_TYPE_NONE )
    {
    }
    else
    if( m_state == STATE_TYPE_IN )
    {
      UIView::SetInputEnabled( false );
    }
    else
    if( m_state == STATE_TYPE_INPUT_IDLE )
    {
    }
    else
    if( m_state == STATE_TYPE_OUT )
    {
      UIView::SetInputEnabled( false );
    }
    else
    if( m_state == STATE_TYPE_END )
    {
      ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
      GFL_ASSERT( pG2DUtil );

      UIView::SetInputEnabled( false );

      //  おていれボタンが表示している場合は退場アニメをする
      if( m_bStartOteire == true )
      {
        pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_BGRLOW_BATTLE_BGR_LOW_000_OUT_CARE );
      }
    }
  }

//  ここから先は WeatherObject の定義

  //-------------------------------------------------------------------------
  /**
    * @brief 初期化
    */
  //-------------------------------------------------------------------------
  void BattleUIBGLowerDraw::WeatherObject::Initialize( const INIT_PARAM& rInitParam )
  {
    GFL_ASSERT( rInitParam.pG2DUtil );

    m_initParam = rInitParam;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 開始
    * @note
    *   天候アイコン表示、非表示をして指定した天候アニメを再生
    */
  //-------------------------------------------------------------------------
  void BattleUIBGLowerDraw::WeatherObject::Start( const BtlWeather weather )
  {
    if( m_weather != weather )
    {
      m_initParam.pG2DUtil->SetPaneVisible( m_initParam.pPartsWeatherBG, true );

      //  ループアニメなのでアニメをストップしないといけない
      gfl2::lyt::LytWk* pLyoutWork  = m_initParam.pG2DUtil->GetLayoutWork( m_initParam.lytWorkId );
      {
        u32 oldAnimeId  = ms_aWeatherAnimeDataTable[ m_weather ].bgKeepId;
        if( pLyoutWork->IsBindAnime( oldAnimeId ) == true )
        {
          pLyoutWork->UnBindAnime( oldAnimeId );
        }
      }

      //  天候切り替えでキープアニメをする
      {
        u32 newAnimeId  = ms_aWeatherAnimeDataTable[ weather ].bgKeepId;
        if( pLyoutWork->IsBindAnime( newAnimeId ) == true )
        {
          pLyoutWork->UnBindAnime( newAnimeId );
        }
        m_initParam.pG2DUtil->StartAnime( m_initParam.lytWorkId, ms_aWeatherAnimeDataTable[ weather ].bgKeepId, true, false );
      }

      m_weather = weather;
    }
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 更新
    */
  //-------------------------------------------------------------------------
  void BattleUIBGLowerDraw::WeatherObject::Update()
  {
  }

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )
