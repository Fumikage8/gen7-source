//=============================================================================
/**
 * @file NumInput.cpp
 * @brief デバッグ数値入力
 * @author obata_toshihiro
 * @date 2011.07.11
 */
//=============================================================================
#if PM_DEBUG

#if 0
#include <gflib.h>
#include <sound/sound.h>
#include <app/util/app_util.h>
#include <app/util/app_util_LayoutContainer.h>
#include <app/util/app_util_Controller.h>
#include "DebugNumInput.h"

#include <arc/arc_def.h>
#include <arc/debug_num_input.gaix>
#include <assets/app/debug_num_input/num_input.h>
#include <assets/app/debug_num_input/num_input_pane.h>
#endif

#include <System/include/GflUse.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_AppLytAccessor.h>
#include <Debug/DebugNumInput/include/DebugNumInput.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/debug_num_input.gaix>
#include <niji_conv_header/app/debug/debug_num_input/res2d/debug_num_input.h>
#include <niji_conv_header/app/debug/debug_num_input/res2d/debug_num_input_pane.h>


namespace debug {


  const gfl2::lyt::LytWkSetUp NumInput::LAYOUT_WORK_SETUP_DATA = 
  {
    gfl2::math::Vector3( 0, -290, 0 ),  // pos
    gfl2::math::Vector2( 1, 1 ),        // scale
    gfl2::math::Vector3( 0, 0, 0 ),     // rotate
    0,                              // priority
    0,
  };

#if 0

  const gfl::grp::ProjectionParam NumInput::PROJECTION_PARAM[2] = 
  {
    // 上画面
    {
      gfl::grp::PROJECTION_ORTHO,
      { -200.0f, 200.0f, -120.0f, 120.0f },
      0.10f,
      300.0f,
      gfl2::math::PIVOT_UPSIDE_TO_TOP,
    },
    // 下画面
    {
      gfl::grp::PROJECTION_ORTHO,
      { -160.0f, 160.0f, -120.0f, 120.0f },
      0.10f,
      300.0f,
      gfl2::math::PIVOT_UPSIDE_TO_TOP,
    },
  };


  const gfl::grp::ViewParam NumInput::VIEW_PARAM[2] = 
  {
    // 上画面
    {
      gfl2::math::VEC3( 0, 0, 100 ),  // カメラ座標
      gfl2::math::VEC3( 0, 1, 0 ),    // 上方向
      gfl2::math::VEC3( 0, 0, 0 ),    // ターゲット座標
    },
    // 下画面
    {
      gfl2::math::VEC3( 0, -290, 100 ),  // カメラ座標
      gfl2::math::VEC3( 0, 1, 0 ),       // 上方向
      gfl2::math::VEC3( 0, -290, 0 ),    // ターゲット座標
    },
  }; 


  const gfl::grp::FragmentOperationParam NumInput::OPERATION_PARAM = 
  {
    gfl::grp::CULL_NONE,  // カリングモード（ENV_CULL〜）
    false,                // シザーテスト（true/false）
    {0},                  //
    false,                // アーリーデプステスト(true/false)
    false,                // ステンシルテスト(true/false)
    {0},                  // 
    0.0f,                 // Wバッファ設定（0.0fで無効）
    0.0f,                 // デプスレンジ Near
    1.0f,                 // デプスレンジ Far
    0,                    // デプス値 分解最小値に対するスケール値
    false,                // デプステスト(true/false)
    0,                    // デプステスト比較関数(ENV_TEST_FUNC_〜)
    true,                 // デプスバッファへの書き込みが可能か(true/false)
    true,                 // カラーバッファへ赤を書き込むか（true/false）
    true,                 // カラーバッファへ赤を書き込むか（true/false）
    true,                 // カラーバッファへ赤を書き込むか（true/false）
    true,                 // カラーバッファへ赤を書き込むか（true/false）
    true,                 // カラーバッファの読み込みが可能か（true/false）
    true,                 // カラーバッファの書き込みが可能か（true/false）
    true,                 // デプスバッファの読み込みが可能か（true/false）
    true,                 // デプスバッファの書き込みが可能か（true/false）
    false,                // ステンシルバッファの読み込みが可能か（true/false）
    false,                // ステンシルバッファの書き込みが可能か（true/false）
  };

#endif

  const gfl2::lyt::LytArcIndex NumInput::ANIME_DATA_TABLE[ ANIME_DATA_NUM ] = 
  {
    /* ANIME_DATA_DUMMY_BUTTON_HOLD */ LYTRES_DEBUG_NUM_INPUT_NUM_INPUT_DUMMY_BUTTON_HOLD_BFLAN,
    /* ANIME_DATA_DUMMY_BUTTON_RELEASE */ LYTRES_DEBUG_NUM_INPUT_NUM_INPUT_DUMMY_BUTTON_RELEASE_BFLAN,
    /* ANIME_DATA_DUMMY_BUTTON_SELECT */ LYTRES_DEBUG_NUM_INPUT_NUM_INPUT_DUMMY_BUTTON_SELECT_BFLAN,
    /* ANIME_DATA_FOCUS_PLACE */ LYTRES_DEBUG_NUM_INPUT_NUM_INPUT_FOCUS_PLACE_BFLAN,
    /* ANIME_DATA_TITLE_SCROLL */ LYTRES_DEBUG_NUM_INPUT_NUM_INPUT_TITLE_SCROLL_BFLAN,
  };



#if 0
  const app::util::LayoutContainer::ShareAnimeDesc NumInput::SHARE_ANIME_TABLE[ SHARE_ANIME_NUM ] = 
  {
    // SHARE_ANIME_BUTTON_HOLD,
    {
      ANIME_BUTTON_HOLD,
      BUTTON_NUM,
      LYTRES_NUM_INPUT_NUM_INPUT_DUMMY_BUTTON_HOLD_BCLAN, 
      PANENAME_NUM_INPUT_GROUP_BUTTON, 
      gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
    },
    // SHARE_ANIME_BUTTON_RELEASE,
    {
      ANIME_BUTTON_RELEASE,
      BUTTON_NUM,
      LYTRES_NUM_INPUT_NUM_INPUT_DUMMY_BUTTON_RELEASE_BCLAN,
      PANENAME_NUM_INPUT_GROUP_BUTTON, 
      gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
    },
    // SHARE_ANIME_BUTTON_SELECT,
    {
      ANIME_BUTTON_SELECT,
      BUTTON_NUM,
      LYTRES_NUM_INPUT_NUM_INPUT_DUMMY_BUTTON_SELECT_BCLAN,
      PANENAME_NUM_INPUT_GROUP_BUTTON, 
      gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
    },
  };
#endif




  const NumInput::PlaceDesc NumInput::PLACE_DESC[ PLACE_NUM ] = 
  {
    {
      PANENAME_NUM_INPUT_PANE_NUM1_VAL,
      PANENAME_NUM_INPUT_PANE_ARROW_UP_00,
      PANENAME_NUM_INPUT_PANE_ARROW_DOWN_00,
      BUTTON_PLACE_00_UP,
      BUTTON_PLACE_00_DOWN,
    },
    {
      PANENAME_NUM_INPUT_PANE_NUM2_VAL,
      PANENAME_NUM_INPUT_PANE_ARROW_UP_01,
      PANENAME_NUM_INPUT_PANE_ARROW_DOWN_01,
      BUTTON_PLACE_01_UP,
      BUTTON_PLACE_01_DOWN,
    },
    {
      PANENAME_NUM_INPUT_PANE_NUM3_VAL,
      PANENAME_NUM_INPUT_PANE_ARROW_UP_02,
      PANENAME_NUM_INPUT_PANE_ARROW_DOWN_02,
      BUTTON_PLACE_02_UP,
      BUTTON_PLACE_02_DOWN,
    },
    {
      PANENAME_NUM_INPUT_PANE_NUM4_VAL,
      PANENAME_NUM_INPUT_PANE_ARROW_UP_03,
      PANENAME_NUM_INPUT_PANE_ARROW_DOWN_03,
      BUTTON_PLACE_03_UP,
      BUTTON_PLACE_03_DOWN,
    },
    {
      PANENAME_NUM_INPUT_PANE_NUM5_VAL,
      PANENAME_NUM_INPUT_PANE_ARROW_UP_04,
      PANENAME_NUM_INPUT_PANE_ARROW_DOWN_04,
      BUTTON_PLACE_04_UP,
      BUTTON_PLACE_04_DOWN,
    },
    {
      PANENAME_NUM_INPUT_PANE_NUM6_VAL,
      PANENAME_NUM_INPUT_PANE_ARROW_UP_05,
      PANENAME_NUM_INPUT_PANE_ARROW_DOWN_05,
      BUTTON_PLACE_05_UP,
      BUTTON_PLACE_05_DOWN,
    },
    {
      PANENAME_NUM_INPUT_PANE_NUM7_VAL,
      PANENAME_NUM_INPUT_PANE_ARROW_UP_06,
      PANENAME_NUM_INPUT_PANE_ARROW_DOWN_06,
      BUTTON_PLACE_06_UP,
      BUTTON_PLACE_06_DOWN,
    },
  };



  const NumInput::ButtonDesc NumInput::BUTTON_DESC[ BUTTON_NUM ] = 
  {
    /* BUTTON_SIGN */
    {
      PANENAME_NUM_INPUT_PANE_SIGN_BACK,
      static_cast<AnimeIndex>( ANIME_BUTTON_HOLD + BUTTON_SIGN ),
      static_cast<AnimeIndex>( ANIME_BUTTON_RELEASE + BUTTON_SIGN ),
      static_cast<AnimeIndex>( ANIME_BUTTON_SELECT + BUTTON_SIGN ),
      0,//SEQ_SE_KW_KNOCK,
    },
    /* BUTTON_PLACE_00_UP */
    {
      PANENAME_NUM_INPUT_PANE_ARROW_UP_00,
      static_cast<AnimeIndex>( ANIME_BUTTON_HOLD + BUTTON_PLACE_00_UP ),
      static_cast<AnimeIndex>( ANIME_BUTTON_RELEASE + BUTTON_PLACE_00_UP ),
      static_cast<AnimeIndex>( ANIME_BUTTON_SELECT + BUTTON_PLACE_00_UP ),
      0,//SEQ_SE_KW_KNOCK,
    },
    /* BUTTON_PLACE_01_UP */
    {
      PANENAME_NUM_INPUT_PANE_ARROW_UP_01,
      static_cast<AnimeIndex>( ANIME_BUTTON_HOLD + BUTTON_PLACE_01_UP ),
      static_cast<AnimeIndex>( ANIME_BUTTON_RELEASE + BUTTON_PLACE_01_UP ),
      static_cast<AnimeIndex>( ANIME_BUTTON_SELECT + BUTTON_PLACE_01_UP ),
      0,//SEQ_SE_KW_KNOCK,
    },
    /* BUTTON_PLACE_02_UP */
    {
      PANENAME_NUM_INPUT_PANE_ARROW_UP_02,
      static_cast<AnimeIndex>( ANIME_BUTTON_HOLD + BUTTON_PLACE_02_UP ),
      static_cast<AnimeIndex>( ANIME_BUTTON_RELEASE + BUTTON_PLACE_02_UP ),
      static_cast<AnimeIndex>( ANIME_BUTTON_SELECT + BUTTON_PLACE_02_UP ),
      0,//SEQ_SE_KW_KNOCK,
    },
    /* BUTTON_PLACE_03_UP */
    {
      PANENAME_NUM_INPUT_PANE_ARROW_UP_03,
      static_cast<AnimeIndex>( ANIME_BUTTON_HOLD + BUTTON_PLACE_03_UP ),
      static_cast<AnimeIndex>( ANIME_BUTTON_RELEASE + BUTTON_PLACE_03_UP ),
      static_cast<AnimeIndex>( ANIME_BUTTON_SELECT + BUTTON_PLACE_03_UP ),
      0,//SEQ_SE_KW_KNOCK,
    },
    /* BUTTON_PLACE_04_UP */
    {
      PANENAME_NUM_INPUT_PANE_ARROW_UP_04,
      static_cast<AnimeIndex>( ANIME_BUTTON_HOLD + BUTTON_PLACE_04_UP ),
      static_cast<AnimeIndex>( ANIME_BUTTON_RELEASE + BUTTON_PLACE_04_UP ),
      static_cast<AnimeIndex>( ANIME_BUTTON_SELECT + BUTTON_PLACE_04_UP ),
      0,//SEQ_SE_KW_KNOCK,
    },
    /* BUTTON_PLACE_05_UP */
    {
      PANENAME_NUM_INPUT_PANE_ARROW_UP_05,
      static_cast<AnimeIndex>( ANIME_BUTTON_HOLD + BUTTON_PLACE_05_UP ),
      static_cast<AnimeIndex>( ANIME_BUTTON_RELEASE + BUTTON_PLACE_05_UP ),
      static_cast<AnimeIndex>( ANIME_BUTTON_SELECT + BUTTON_PLACE_05_UP ),
      0,//SEQ_SE_KW_KNOCK,
    },
    /* BUTTON_PLACE_06_UP */
    {
      PANENAME_NUM_INPUT_PANE_ARROW_UP_06,
      static_cast<AnimeIndex>( ANIME_BUTTON_HOLD + BUTTON_PLACE_06_UP ),
      static_cast<AnimeIndex>( ANIME_BUTTON_RELEASE + BUTTON_PLACE_06_UP ),
      static_cast<AnimeIndex>( ANIME_BUTTON_SELECT + BUTTON_PLACE_06_UP ),
      0,//SEQ_SE_KW_KNOCK,
    },
    /* BUTTON_PLACE_00_DOWN */
    {
      PANENAME_NUM_INPUT_PANE_ARROW_DOWN_00,
      static_cast<AnimeIndex>( ANIME_BUTTON_HOLD + BUTTON_PLACE_00_DOWN ),
      static_cast<AnimeIndex>( ANIME_BUTTON_RELEASE + BUTTON_PLACE_00_DOWN ),
      static_cast<AnimeIndex>( ANIME_BUTTON_SELECT + BUTTON_PLACE_00_DOWN ),
      0,//SEQ_SE_KW_KNOCK,
    },
    /* BUTTON_PLACE_01_DOWN */
    {
      PANENAME_NUM_INPUT_PANE_ARROW_DOWN_01,
      static_cast<AnimeIndex>( ANIME_BUTTON_HOLD + BUTTON_PLACE_01_DOWN ),
      static_cast<AnimeIndex>( ANIME_BUTTON_RELEASE + BUTTON_PLACE_01_DOWN ),
      static_cast<AnimeIndex>( ANIME_BUTTON_SELECT + BUTTON_PLACE_01_DOWN ),
      0,//SEQ_SE_KW_KNOCK,
    },
    /* BUTTON_PLACE_02_DOWN */
    {
      PANENAME_NUM_INPUT_PANE_ARROW_DOWN_02,
      static_cast<AnimeIndex>( ANIME_BUTTON_HOLD + BUTTON_PLACE_02_DOWN ),
      static_cast<AnimeIndex>( ANIME_BUTTON_RELEASE + BUTTON_PLACE_02_DOWN ),
      static_cast<AnimeIndex>( ANIME_BUTTON_SELECT + BUTTON_PLACE_02_DOWN ),
      0,//SEQ_SE_KW_KNOCK,
    },
    /* BUTTON_PLACE_03_DOWN */
    {
      PANENAME_NUM_INPUT_PANE_ARROW_DOWN_03,
      static_cast<AnimeIndex>( ANIME_BUTTON_HOLD + BUTTON_PLACE_03_DOWN ),
      static_cast<AnimeIndex>( ANIME_BUTTON_RELEASE + BUTTON_PLACE_03_DOWN ),
      static_cast<AnimeIndex>( ANIME_BUTTON_SELECT + BUTTON_PLACE_03_DOWN ),
      0,//SEQ_SE_KW_KNOCK,
    },
    /* BUTTON_PLACE_04_DOWN */
    {
      PANENAME_NUM_INPUT_PANE_ARROW_DOWN_04,
      static_cast<AnimeIndex>( ANIME_BUTTON_HOLD + BUTTON_PLACE_04_DOWN ),
      static_cast<AnimeIndex>( ANIME_BUTTON_RELEASE + BUTTON_PLACE_04_DOWN ),
      static_cast<AnimeIndex>( ANIME_BUTTON_SELECT + BUTTON_PLACE_04_DOWN ),
      0,//SEQ_SE_KW_KNOCK,
    },
    /* BUTTON_PLACE_05_DOWN */
    {
      PANENAME_NUM_INPUT_PANE_ARROW_DOWN_05,
      static_cast<AnimeIndex>( ANIME_BUTTON_HOLD + BUTTON_PLACE_05_DOWN ),
      static_cast<AnimeIndex>( ANIME_BUTTON_RELEASE + BUTTON_PLACE_05_DOWN ),
      static_cast<AnimeIndex>( ANIME_BUTTON_SELECT + BUTTON_PLACE_05_DOWN ),
      0,//SEQ_SE_KW_KNOCK,
    },
    /* BUTTON_PLACE_06_DOWN */
    {
      PANENAME_NUM_INPUT_PANE_ARROW_DOWN_06,
      static_cast<AnimeIndex>( ANIME_BUTTON_HOLD + BUTTON_PLACE_06_DOWN ),
      static_cast<AnimeIndex>( ANIME_BUTTON_RELEASE + BUTTON_PLACE_06_DOWN ),
      static_cast<AnimeIndex>( ANIME_BUTTON_SELECT + BUTTON_PLACE_06_DOWN ),
      0,//SEQ_SE_KW_KNOCK,
    },
    /* BUTTON_OK */
    {
      PANENAME_NUM_INPUT_PANE_OK_BUTTON,
      static_cast<AnimeIndex>( ANIME_BUTTON_HOLD + BUTTON_OK ),
      static_cast<AnimeIndex>( ANIME_BUTTON_RELEASE + BUTTON_OK ),
      static_cast<AnimeIndex>( ANIME_BUTTON_SELECT + BUTTON_OK ),
      0,
    },
    /* BUTTON_CANCEL */
    {
      PANENAME_NUM_INPUT_PANE_NG_BUTTON,
      static_cast<AnimeIndex>( ANIME_BUTTON_HOLD + BUTTON_CANCEL ),
      static_cast<AnimeIndex>( ANIME_BUTTON_RELEASE + BUTTON_CANCEL ),
      static_cast<AnimeIndex>( ANIME_BUTTON_SELECT + BUTTON_CANCEL ),
      0,//SEQ_SE_CANCEL1,
    },
  };



  //---------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param heap             使用するヒープ
   * @param device_heap      使用するデバイスヒープ
   * @param graphics_system  描画に使用するグラフィックスシステム
   * @param layout_system    描画に使用するレイアウトシステム
   */
  //---------------------------------------------------------------------------
  NumInput::NumInput(
      gfl2::heap::HeapBase* heap,
      gfl2::heap::HeapBase* device_heap,
#if 0
      gfl::grp::GraphicsSystem* graphisc_system,
#endif
      gfl2::lyt::LytSys* layout_system,
      bool play_se ) :
    app::util::Controller(),
    NumInputTemp::LytUtil( heap ),
#if 0
    app::util::LayoutContainer( 
        heap, device_heap, layout_system, 
        ARCID_DEBUG_NUM_INPUT, GARC_debug_num_input_num_input_LYTDAT,
        LYTRES_NUM_INPUT_NUM_INPUT_BCLYT,
        ANIME_DATA_TABLE, ANIME_DATA_NUM, ANIME_NUM,
        SHARE_ANIME_TABLE, SHARE_ANIME_NUM, 
        LAYOUT_WORK_SETUP_DATA,
        PROJECTION_PARAM[0], PROJECTION_PARAM[1],
        VIEW_PARAM[0], VIEW_PARAM[0], VIEW_PARAM[1],
        OPERATION_PARAM ),
    m_graphics_system( graphisc_system ),
#endif
    m_button_manager( NULL ),
    m_min_value( 0 ),
    m_max_value( 0 ),
    m_value( 0 ),
    m_max_place_index( 0 ),
    m_current_place_index( 0 ),
    m_result( RES_CONT ),
    m_mute( !play_se )
  { 
    m_sysAllocator   = GFL_NEW(heap)  gfl2::heap::NwAllocator(heap);
    m_devAllocator   = GFL_NEW(device_heap)  gfl2::heap::NwAllocator(device_heap);

    this->CreateRenderingPipeLine( heap, device_heap );

    {
      // リソース読み込み
      app::util::FileAccessor::FileOpenSync( ARCID_DEBUG_NUM_INPUT, heap );
      app::util::FileAccessor::FileLoadSync( ARCID_DEBUG_NUM_INPUT,   GARC_debug_num_input_debug_num_input_APPLYT, &m_lytDataBuffer, device_heap, false, 128 );
      app::util::FileAccessor::FileCloseSync( ARCID_DEBUG_NUM_INPUT );

      app::util::AppLytAccessor* accessor = GFL_NEW_LOW( heap ) app::util::AppLytAccessor();
      accessor->SetAppLytBuf( m_lytDataBuffer );

      m_lytResID = layout_system->LoadLytRes( NULL, m_devAllocator, accessor->GetLytDat() );
      m_lytWk  = layout_system->CreateLytWk( &m_lytResID, LYTRES_DEBUG_NUM_INPUT_NUM_INPUT_BFLYT, ANIME_DATA_TABLE, ANIME_DATA_NUM, ANIME_NUM, &LAYOUT_WORK_SETUP_DATA );
      m_lytWk->SetAutoDrawFlag( false );

      m_lytSys = layout_system;

      GFL_SAFE_DELETE( accessor );
    }

    this->CreateButtonManager( heap );
  }

  /**
   * @brief レンダリングパイプラインを生成する
   */
  void NumInput::CreateRenderingPipeLine( gfl2::heap::HeapBase* systemHeap, gfl2::heap::HeapBase* deviceHeap )
  {
    m_renderingPipeLine = GFL_NEW( systemHeap ) AppRenderingPipeLine( deviceHeap, 512, 256 );
  }

  /**
   * @brief ボタンマネージャを生成する
   * @param heap  使用するヒープ
   */
  void NumInput::CreateButtonManager( gfl2::heap::HeapBase* heap )
  {
    m_button_manager = 
      GFL_NEW( heap ) app::tool::ButtonManager(
          heap, this->GetDeviceManager(), BUTTON_NUM, ANIME_NULL );

    for( u32 i=0; i<BUTTON_NUM; i++ ) {
      m_button_manager->AddButton( 
          heap, 
          i,
          m_lytWk, 
          BUTTON_DESC[i].pane_index,
          BUTTON_DESC[i].hold_anime,
          BUTTON_DESC[i].release_anime,
          app::tool::ButtonManager::ANIMATION_NULL,
          BUTTON_DESC[i].select_anime,
          app::tool::ButtonManager::ANIMATION_NULL,
          app::tool::ButtonManager::ANIMATION_NULL,
          this );

      if( !m_mute ) m_button_manager->SetButtonSelectSE( i, BUTTON_DESC[i].select_SE );
    }

    m_button_manager->SetSelectType( app::tool::ButtonManager::SELECT_TYPE_TRIGGER ); 
    m_button_manager->SetNotifyTimming( app::tool::ButtonManager::NOTIFY_AFTER_ANIME );
  }



  //---------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //---------------------------------------------------------------------------
  NumInput::~NumInput()
  {
    this->DeleteButtonManager();

    this->DeleteRenderingPipeLine();

    if( m_lytWk )
    {
      m_lytSys->RemoveLytWk( m_lytWk );
      m_lytWk = NULL;
    }

    m_lytSys->ReleaseLytRes( &m_lytResID );

    if( m_devAllocator ){
      GFL_DELETE  m_devAllocator;
      m_devAllocator = NULL;
    }
    if( m_sysAllocator ){
      GFL_DELETE  m_sysAllocator;
      m_sysAllocator = NULL;
    }
    GflHeapSafeFreeMemory( m_lytDataBuffer );
  }

  /**
   * @brief レンダリングパイプラインを削除する
   */
  void NumInput::DeleteRenderingPipeLine( void )
  {
    GFL_SAFE_DELETE( m_renderingPipeLine );
  }

  /**
   * @brief ボタンマネージャを破棄する
   */
  void NumInput::DeleteButtonManager( void )
  {
    GFL_DELETE m_button_manager;
  }








  //---------------------------------------------------------------------------
  /**
   * @brief 入力された値を取得する
   * @note 入力中なら, 表示されている値が返ります。
   */
  //---------------------------------------------------------------------------
  s32 NumInput::GetValue( void ) const
  {
    return m_value;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 入力を開始する
   * @param title  タイトル
   * @param init   初期値
   * @param min    最小値
   * @param max    最大値
   */
  //---------------------------------------------------------------------------
  void NumInput::Start( const wchar_t* title, s32 init, s32 min, s32 max )
  {
    m_seq = SEQ_WAIT_INPUT;
    m_result = RES_CONT;
    m_min_value = min;
    m_max_value = max;
    m_value = init;
    m_max_place_index = this->CalcPlaceCount( max ) - 1;
    this->UpdateDisplayValue();
    this->ChangePlaceIndex( 0 );

    gfl2::lyt::LytWk* layout_work = m_lytWk;
    this->SetTextboxPaneString( layout_work, PANENAME_NUM_INPUT_PANE_TITLE, title );
    this->StartAnimation( layout_work, ANIME_TITLE_SCROLL );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 更新処理
   * @return 結果
   */
  //---------------------------------------------------------------------------
  NumInput::Result NumInput::Update( void )
  {
    if( m_seq == SEQ_FINISH ) {
      return m_result;
    }

    m_lytWk->Update();
    m_button_manager->Update();

    void ( NumInput::*UPDATE_SEQ[ SEQ_NUM ] )( void ) = {
      &NumInput::UpdateSeq_WAIT_INPUT,
      &NumInput::UpdateSeq_WAIT_ANIME,
      NULL,
    };
    ( this->*UPDATE_SEQ[ m_seq ] )(); 


    if( m_renderingPipeLine ) {
      m_renderingPipeLine->Update();
      m_renderingPipeLine->GetLayoutRenderPath()->ClearDrawableLytWk();
      m_renderingPipeLine->GetLayoutRenderPath()->AddDrawableLytWk( m_lytWk, gfl2::lyt::DISPLAY_LOWER );
    }

    return RES_CONT;
  }

  //=======================================================
  /**
   * @brief シーケンス更新処理( 入力待ち )
   */
  //=======================================================
  void NumInput::UpdateSeq_WAIT_INPUT( void )
  { 
    const gfl2::ui::Button* ui_button = this->GetButton(); 
    if( ui_button->IsRepeat( gfl2::ui::BUTTON_RIGHT ) ) {
      this->ChangePlaceIndex( m_current_place_index - 1 );
#if 0
      if( !m_mute ) xy_snd::PlaySE( SEQ_SE_KW_KNOCK );
#endif
    }
    else if( ui_button->IsRepeat( gfl2::ui::BUTTON_LEFT ) ) {
      this->ChangePlaceIndex( m_current_place_index + 1 );
#if 0
      if( !m_mute ) xy_snd::PlaySE( SEQ_SE_KW_KNOCK );
#endif
    }

    ButtonIndex selected_button = BUTTON_NULL;
    if( ui_button->IsRepeat( gfl2::ui::BUTTON_UP ) ) {
      selected_button = PLACE_DESC[ m_current_place_index ].inc_button;
    }
    else if( ui_button->IsRepeat( gfl2::ui::BUTTON_DOWN ) ) {
      selected_button = PLACE_DESC[ m_current_place_index ].dec_button;
    }
    else if( ui_button->IsTrigger( gfl2::ui::BUTTON_B ) ) {
      selected_button = BUTTON_CANCEL;
    }
    else if( ui_button->IsTrigger( gfl2::ui::BUTTON_A ) ) {
      selected_button = BUTTON_OK;
    }

    if( selected_button != BUTTON_NULL ) {
      m_button_manager->StartSelectedAct( selected_button );  // コールバックが呼ばれる！
    }
  } 

  /**
   * @brief ボタンが選択された際に呼ばれるコールバック関数
   * @param buttonID  選択されたボタンのID
   */
  void NumInput::CallbackFuncOnButtonSelected( u32 buttonID )
  {
    switch( buttonID )
    {
    case BUTTON_SIGN:
      this->SwitchNumSign();
      break;

    case BUTTON_PLACE_00_UP:
      this->IncPlaceNumber( 0 );
      break;

    case BUTTON_PLACE_01_UP:
      this->IncPlaceNumber( 1 );
      break;

    case BUTTON_PLACE_02_UP:
      this->IncPlaceNumber( 2 );
      break;

    case BUTTON_PLACE_03_UP:
      this->IncPlaceNumber( 3 );
      break;

    case BUTTON_PLACE_04_UP:
      this->IncPlaceNumber( 4 );
      break;

    case BUTTON_PLACE_05_UP:
      this->IncPlaceNumber( 5 );
      break;

    case BUTTON_PLACE_06_UP:
      this->IncPlaceNumber( 6 );
      break;

    case BUTTON_PLACE_00_DOWN:
      this->DecPlaceNumber( 0 );
      break;

    case BUTTON_PLACE_01_DOWN:
      this->DecPlaceNumber( 1 );
      break;

    case BUTTON_PLACE_02_DOWN:
      this->DecPlaceNumber( 2 );
      break;

    case BUTTON_PLACE_03_DOWN:
      this->DecPlaceNumber( 3 );
      break;

    case BUTTON_PLACE_04_DOWN:
      this->DecPlaceNumber( 4 );
      break; 

    case BUTTON_PLACE_05_DOWN:
      this->DecPlaceNumber( 5 );
      break;

    case BUTTON_PLACE_06_DOWN:
      this->DecPlaceNumber( 6 );
      break;

    case BUTTON_OK:
      m_seq = SEQ_WAIT_ANIME;
      m_wait_anime = BUTTON_DESC[ BUTTON_OK ].select_anime;
      m_result = RES_DECIDE;
      break;

    case BUTTON_CANCEL:
      m_seq = SEQ_WAIT_ANIME;
      m_wait_anime = BUTTON_DESC[ BUTTON_CANCEL ].select_anime;
      m_result = RES_CANCEL;
      break;
    }
  }

  /**
   * @brief 数値の符号を反転する
   */
  void NumInput::SwitchNumSign( void )
  {
    if( ( ( 0 < m_value ) && ( m_min_value < 0 ) ) ||
        ( ( m_value < 0 ) && ( 0 < m_max_value ) ) ) {
      m_value = -m_value;
      m_value = gfl2::math::Max( m_value, m_min_value );
      m_value = gfl2::math::Min( m_value, m_max_value );
      this->UpdateDisplayValue();
    }
  }

  //=======================================================
  /**
   * @brief シーケンス更新処理( アニメ終了待ち )
   */
  //=======================================================
  void NumInput::UpdateSeq_WAIT_ANIME( void )
  {
    if( this->IsAnimationEnd( m_lytWk, m_wait_anime ) ) {
      m_seq = SEQ_FINISH;
    }
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 描画処理
   */
  //---------------------------------------------------------------------------
  void NumInput::Draw( void )
  {
    if( m_renderingPipeLine == NULL ) {
      return;
    }
    gfl2::gfx::DisplayDeviceHandle handle = System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::DOWN ];
    gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( handle );
    gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( handle );
    u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( handle );
    u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( handle );
    m_renderingPipeLine->SetRenderTarget( pBackBuffer );
    m_renderingPipeLine->SetDepthStencil( pDepthBuffer );
    m_renderingPipeLine->SetDisplaySize( displayWidth, displayHeight );
    m_renderingPipeLine->Execute();
  } 

  /**
   * @brief 指定した桁の数字を取得する
   * @param value        数値
   * @param place_index  桁
   */
  u32 NumInput::CalcPlaceNumber( s32 value, u8 place_index )
  {
    u32 val = gfl2::math::Abs( value );
    for( u32 i=0; i<place_index; i++ ) {
      val /= 10;
    }
    return val % 10;
  }

  u32 NumInput::CalcPlaceCount( s32 value )
  {
    u32 place_count = 1;
    for( u32 val=value; 10<=val; val/=10 ) {
      place_count++;
    }
    return place_count;
  }

  /**
   * @brief 指定した桁の数値を増やす
   * @param place_index  桁
   */
  void NumInput::IncPlaceNumber( u8 place_index )
  {
    s32 add_value = 1;
    for( u32 i=0; i<place_index; i++ ) {
      add_value *= 10;
    }
    s32 next_value = m_value + add_value;
    m_value = gfl2::math::Min( next_value, m_max_value );
    this->UpdateDisplayValue();
  }

  /**
   * @brief 指定した桁の数値を減らす
   * @param place_index  桁
   */
  void NumInput::DecPlaceNumber( u8 place_index )
  {
    s32 sub_value = 1;
    for( u32 i=0; i<place_index; i++ ) {
      sub_value *= 10;
    }
    s32 next_value = m_value - sub_value;
    m_value = gfl2::math::Max( next_value, m_min_value );
    this->UpdateDisplayValue();
  } 

  /**
   * @brief 数値の表示を更新する
   */
  void NumInput::UpdateDisplayValue( void )
  {
    for( u32 i=0; i<PLACE_NUM; i++ ) {
      u32 number = this->CalcPlaceNumber( m_value, i );
      this->UpdatePlaceNumber( i, number );
    }

    // 符号を調整
    gfl2::lyt::LytWk* layout_work = m_lytWk;
    this->SetPaneInvisible( layout_work, PANENAME_NUM_INPUT_PANE_PLUS );
    this->SetPaneInvisible( layout_work, PANENAME_NUM_INPUT_PANE_MINUS );
    if( 0 < m_value ) {
      this->SetPaneVisible( layout_work, PANENAME_NUM_INPUT_PANE_PLUS );
    }
    else if( m_value < 0 ) {
      this->SetPaneVisible( layout_work, PANENAME_NUM_INPUT_PANE_MINUS );
    }
  }

  /**
   * @brief 指定した桁の表示を更新する
   * @param place_index  桁
   * @param number       表示する数字
   */
  void NumInput::UpdatePlaceNumber( u32 place_index, u32 number )
  {
    gfl2::lyt::LytWk* layout_work = m_lytWk;
    gfl2::str::StrBuf* strbuf = this->GetTempStrBuf();
    print::StrNumber( strbuf, number, 1, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU ); 
    this->SetTextboxPaneString( layout_work, PLACE_DESC[ place_index ].num_pane, strbuf );
  }

  /**
   * @brief 操作対象の桁を変更する
   * @param place_index  新しい操作対象の桁
   */
  void NumInput::ChangePlaceIndex( s8 place_index )
  {
    if( ( 0 <= place_index ) && ( place_index <= m_max_place_index ) )
    {
      m_current_place_index = place_index; 
      gfl2::lyt::LytWk* layout_work = m_lytWk;
      this->StartAnimation( layout_work, ANIME_FOCUS_PLACE );
      this->PauseAnimation( layout_work, ANIME_FOCUS_PLACE, false );
      this->SetAnimationFrame( layout_work, ANIME_FOCUS_PLACE, place_index );
    }
  }


  /**
   * @brief   レイアウトワークを取得
   * @return  レウアウトワーク
   */
  gfl2::lyt::LytWk * NumInput::GetLayoutWork(void)
  {
    return m_lytWk;
  }

} // namespace debug 

#endif //PM_DEBUG
