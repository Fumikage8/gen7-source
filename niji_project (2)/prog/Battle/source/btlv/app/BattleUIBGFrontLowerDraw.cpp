//======================================================================
/**
 * @file BattleUIBGFrontLowerDraw.cpp
 * @date 2015/07/28 12:47:28
 * @author uchida_yuto
 * @brief バトル下画面のBG手前描画
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "BattleUIBGFrontLowerDraw.h"
#include "BattleUIViewDrawOrder.h"

//  nijiのインクルード
#include "AppLib/include/Util/AppRenderingManager.h"
#include "AppLib/include/Tool/AppToolPokeSimpleModel.h"
#include "AppLib/include/Tool/AppCommonGrpIconData.h"
#include "AppLib/include/Tool/AppToolItemIcon.h"
#include "AppLib/include/Util/app_util_Common.h"
#include "AppLib/include/Tool/CommonMessageWindowManipulator.h"

//  素材のインクルード
#include "niji_conv_header/app/box/res2d/staupp.h"
#include "niji_conv_header/app/box/res2d/staupp_anim_list.h"
#include "niji_conv_header/app/box/res2d/staupp_pane.h"

//  レイアウトデータのインクルード
#include "niji_conv_header/app/battle/res2d/bgrlow.h"
#include "niji_conv_header/app/battle/res2d/bgrlow_pane.h"
#include "niji_conv_header/app/battle/res2d/bgrlow_anim_list.h"

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  //  天候アニメid一覧
  const BattleUIBGFrontLowerDraw::WeatherAnimeData BattleUIBGFrontLowerDraw::ms_aWeatherAnimeDataTable[ BTL_WEATHER_MAX ]  =
  {
    //  BTL_WEATHER_NONE = 0,   ///< 天候なし
      { 
        -1,
        -1,
      },

    //  BTL_WEATHER_SHINE,    ///< はれ
      { 
        LA_BGRLOW_BATTLE_BGR_LOW_001_IN_SUNNY,
        LA_BGRLOW_BATTLE_BGR_LOW_001_OUT_SUNNY,
      },

    //  BTL_WEATHER_RAIN,     ///< あめ
      {
        LA_BGRLOW_BATTLE_BGR_LOW_001_IN_RAIN,
        LA_BGRLOW_BATTLE_BGR_LOW_001_OUT_RAIN,
      },

    //  BTL_WEATHER_SNOW,     ///< あられ
      {
        LA_BGRLOW_BATTLE_BGR_LOW_001_IN_HAILSTONE,
        LA_BGRLOW_BATTLE_BGR_LOW_001_OUT_HAILSTONE,
      },
      
    //  BTL_WEATHER_SAND,     ///< すなあらし
      {
        LA_BGRLOW_BATTLE_BGR_LOW_001_IN_SANDSTORM,
        LA_BGRLOW_BATTLE_BGR_LOW_001_OUT_SANDSTORM,
      },

    //  BTL_WEATHER_STORM,    ///< おおあめ
      {
        LA_BGRLOW_BATTLE_BGR_LOW_001_IN_RAIN_HEAVY,
        LA_BGRLOW_BATTLE_BGR_LOW_001_OUT_RAIN_HEAVY,
      },

    //  BTL_WEATHER_DAY,      ///< おおひでり
      {
        LA_BGRLOW_BATTLE_BGR_LOW_001_IN_SUNNY_HEAVY,
        LA_BGRLOW_BATTLE_BGR_LOW_001_OUT_SUNNY_HEAVY,
      },

    //  BTL_WEATHER_TURBULENCE,  ///< 乱気流
      {
        LA_BGRLOW_BATTLE_BGR_LOW_001_IN_TURBULENCE,
        LA_BGRLOW_BATTLE_BGR_LOW_001_OUT_TURBULENCE,
      },
  };

  BattleUIBGFrontLowerDraw::~BattleUIBGFrontLowerDraw()
  {
    GFL_SAFE_DELETE( m_pCommonMessageWindow );

    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  初期化処理
  * @return true 処理終了 / false 処理が終了していないので再度呼び出す
  * @note
  *         返り値が true になるまで呼び出す
  */
  //--------------------------------------------------------------------------------------------
  bool BattleUIBGFrontLowerDraw::InitFunc()
  {
    if( m_bInit == true )
    {
      return  true;
    }

    enum
    {
      SEQ_INIT  = 0,
      SEQ_WAIT_LOAD
    };

    if( m_initSeq == SEQ_INIT )
    {
      //  2D画面を生成
      {
        _Init2D();
      }

      ++m_initSeq;
    }

    if( m_initSeq == SEQ_WAIT_LOAD )
    {
      bool  bNextSeq  = true;

      if( bNextSeq == true )
      {
        m_initSeq   = 0;
        m_endSeq    = 0;
        m_bInit     = true;

        return  true;
      }
    }

    return  false;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  終了処理
  * @return true 処理終了 / false 処理が終了していないので再度呼び出す
  * @note
  *         返り値が true になるまで
  *         この関数を呼び出すとInitFunc()で初期化しないと使えないので注意
  *         使うケースとしては画面をもう使わないときの後処理に使う
  */
  //--------------------------------------------------------------------------------------------
  bool BattleUIBGFrontLowerDraw::EndFunc()
  {
    if( m_bInit == false )
    {
      return  true;
    }

    enum
    {
      SEQ_END = 0,
    };

    if( m_endSeq == SEQ_END )
    {
      if( UIView::IsDrawing() == false )
      {
        m_endSeq   = 0;
        m_initSeq  = 0;
        m_bInit    = false;

        return true;
      }
    }

    return false;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  画面を開く
  */
  //--------------------------------------------------------------------------------------------
  void BattleUIBGFrontLowerDraw::Open()
  {
    GFL_ASSERT( m_bInit == true );

    UIView::SetVisible( true );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  画面を閉じる
  */
  //--------------------------------------------------------------------------------------------
  void BattleUIBGFrontLowerDraw::Close()
  {
    GFL_ASSERT( m_bInit == true );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void BattleUIBGFrontLowerDraw::Update()
  {
    if( m_bInit == false )
    {
      return;
    }

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    if( m_weatherIconState == WEATHER_ICON_STAET_IDLE )
    {
      if( m_newIconWeather != m_nowIconWeather )
      {
        //  アイコンの天候と最新の天候との状態を同期
        if( m_nowIconWeather == BTL_WEATHER_NONE )
        {
          _InWeatherIcon( m_newIconWeather );
        }
        else
        {
          _OutWeatherIcon( m_nowIconWeather );
        }
      }
    }

    if( m_weatherIconState == WEATHER_ICON_STAET_IN )
    {
      if( pG2DUtil->IsAnimeEnd( LYT_WORK_NORMAL, m_iconWeatherInAnimeId ) == true )
      {
        m_weatherIconState = WEATHER_ICON_STAET_IDLE;
      }
    }

    if( m_weatherIconState == WEATHER_ICON_STAET_OUT )
    {
      if( pG2DUtil->IsAnimeEnd( LYT_WORK_NORMAL, m_iconWeatherOutAnimeId ) == true )
      {
        if( m_nowIconWeather != m_newIconWeather )
        {
          if( _InWeatherIcon( m_newIconWeather ) == false )
          {
            m_weatherIconState = WEATHER_ICON_STAET_IDLE;
          }
        }
        else
        {
          m_weatherIconState = WEATHER_ICON_STAET_IDLE;
        }
      }
    }

    UIView::Update();
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
  void BattleUIBGFrontLowerDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    if( m_bInit == false )
    {
      return;
    }

    if( displayNo == gfl2::gfx::CtrDisplayNo::DOWN )
    {
      ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
      for( s32 i = 0; i < LYT_WORK_MAX; ++i )
      {
        pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderinManager, displayNo, i, LOW_VIEW_ORDER_BG_FRONT );
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief 天候設定
    * @param weathre : 設定する天候
    * @note  天候アイコン挙動が変わる
    */
  //------------------------------------------------------------------
  void BattleUIBGFrontLowerDraw::SetWeather( const BtlWeather weather )
  {
    m_newIconWeather  = weather;
  }

  //------------------------------------------------------------------
  /**
    * @brief 暗転フィルターの有効設定
    * @param bEnable : 有効設定
    */
  //------------------------------------------------------------------
  void BattleUIBGFrontLowerDraw::SetEnableDimFilterLayout( const bool bEnable )
  {
    GFL_ASSERT( m_pMaskScreenPane );

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    pG2DUtil->SetPaneVisible( m_pMaskScreenPane, bEnable );
  }

  //------------------------------------------------------------------
  /**
    * @brief   メッセージウィンドウの表示開始
    */
  //------------------------------------------------------------------
  void BattleUIBGFrontLowerDraw::ShowMessageWindow( const u32 strId, const bool bShowTimerIcon )
  {
    GFL_ASSERT( m_pCommonMessageWindow );

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_BGRLOW_BATTLE_BGR_LOW_001__WINDOW_IN );

    m_pCommonMessageWindow->ShowMessage( strId, true, bShowTimerIcon );
  }

  //------------------------------------------------------------------
  /**
    * @brief   メッセージウィンドウの非表示開始
    */
  //------------------------------------------------------------------
  void BattleUIBGFrontLowerDraw::HideMessageWindow()
  {
    GFL_ASSERT( m_pCommonMessageWindow );

    m_pCommonMessageWindow->HideMessage();
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  2D初期化処理
  * @note
  *     レイアウトデータから描画に必要な生成をしている
  */
  //--------------------------------------------------------------------------------------------
  void BattleUIBGFrontLowerDraw::_Init2D()
  {
    // アーカイブIDテーブル
    const ::app::util::G2DUtil::LytResData aResTbl[] =
    {
      {  m_initParam.pLytBinData, 8, ::app::util::G2DUtil::ATTACH_NONE },
    };

    //  作成するレイアウトワーク
    const ::app::util::G2DUtil::LytwkData aSetupData[ LYT_WORK_MAX ] =
    {
      {
        0,
        LYTRES_BGRLOW_BATTLE_BGR_LOW_001_BFLYT,
        LA_BGRLOW_BATTLE_BGR_LOW_001___NUM,
        m_initParam.pLytBinData,
        true,
        ::app::util::G2DUtil::SETUP_LOWER,
        true
      },
    };

    UIView::Create2D( m_initParam.pHeap, NULL, 32, aResTbl, GFL_NELEMS(aResTbl), aSetupData, GFL_NELEMS(aSetupData), m_initParam.pBtlAppMsgData, m_initParam.pBtlWordSet );

    UIView::SetVisible( false );

    gfl2::lyt::LytWk* pLayoutWork = UIView::GetLayoutWork( LYT_WORK_NORMAL );

    //  暗転フィルターペイン取得
    {
      m_pMaskScreenPane = pLayoutWork->GetPane( PANENAME_BATTLE_BGR_LOW_001_PANE_BG_WHITE4X4 );
      GFL_ASSERT( m_pMaskScreenPane );
    }

    //  汎用ウィンドウ作成
    {
      m_pCommonMessageWindow  = GFL_NEW( m_initParam.pHeap->GetDeviceHeap() ) App::Tool::CommonMessageWindowManipulator();

      //  パラメータ設定
      App::Tool::CommonMessageWindowManipulator::SSetupInfo setupInfo;
      {
        setupInfo.layoutWorkID        = LYT_WORK_NORMAL;

        setupInfo.animID_MsgCursor    = LA_BGRLOW_BATTLE_BGR_LOW_001__WINDOW__CURSOR_00_KEEP;
        setupInfo.animID_TimerIcon    = LA_BGRLOW_BATTLE_BGR_LOW_001__WINDOW__TIMERICON_KEEP;
        setupInfo.animID_TimerIcon_In = LA_BGRLOW_BATTLE_BGR_LOW_001__WINDOW__TIMERICON_IN;
        setupInfo.animID_TimerIcon_Out= LA_BGRLOW_BATTLE_BGR_LOW_001__WINDOW__TIMERICON_OUT;
        setupInfo.paneID_MsgCursor    = gfl2::lyt::LYTSYS_PANEINDEX_ERROR;
        setupInfo.paneID_NullText1    = PANENAME_COMMON_MSG_LOW_000_PANE_NULL_TEXT1;
        setupInfo.paneID_TextBox1a    = PANENAME_COMMON_MSG_LOW_000_MATERIAL_TEXTBOX_1A_OF_TEXTBOX_1A;
        setupInfo.paneID_TextBox2     = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2;
        setupInfo.paneID_TextBox2a    = PANENAME_COMMON_MSG_LOW_000_MATERIAL_TEXTBOX_1B_OF_TEXTBOX_1B;
        setupInfo.paneID_TimerIcon    = PANENAME_COMMON_MSG_LOW_000_PANE_TIMERICON;
        setupInfo.paneID_WindowParts  = PANENAME_BATTLE_BGR_LOW_001_PANE_WINDOW;
        setupInfo.pHandler            = NULL;
        setupInfo.pHeap               = m_initParam.pHeap;
        setupInfo.pTargetView         = this;
      }

      m_pCommonMessageWindow->Setup( setupInfo );
    }
  }

  //  天候アイコン登場
  bool BattleUIBGFrontLowerDraw::_InWeatherIcon( const BtlWeather weather )
  {
    m_nowIconWeather = weather;

    if( m_nowIconWeather == BTL_WEATHER_NONE )
    {
      return  false;
    }

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    //  アニメ再生
    m_iconWeatherInAnimeId  = ms_aWeatherAnimeDataTable[ weather ].iconInId;
    pG2DUtil->StartAnime( LYT_WORK_NORMAL, m_iconWeatherInAnimeId );

    m_weatherIconState  = WEATHER_ICON_STAET_IN;

    return  true;
  }

  //  アイコン退場
  void BattleUIBGFrontLowerDraw::_OutWeatherIcon( const BtlWeather weather )
  {
    if( weather == BTL_WEATHER_NONE )
    {
      return;
    }

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    m_iconWeatherOutAnimeId  = ms_aWeatherAnimeDataTable[ weather ].iconOutId;
    pG2DUtil->StartAnime( LYT_WORK_NORMAL, m_iconWeatherOutAnimeId );

    m_weatherIconState  = WEATHER_ICON_STAET_OUT;
  }

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )
