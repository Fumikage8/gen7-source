//=============================================================================
/**
 * @file	 TitleFrame.h
 * @brief	 タイトル制御フレーム
 * @author yuto_uchida
 * @date	 2015.05.08
 */
//=============================================================================
#pragma once

#include "App/Title/include/TitleFrame.h"
#include "App/Title/source/parts/TitleUpperUIDraw.h"
#include "App/Title/source/parts/TitleLowerUIDraw.h"
#include "App/Title/source/parts/TitlePokeModelObj.h"

#include "arc_def_index/arc_def.h"
#include <arc_index/NijiRenderPath.gaix>

//  gfl2のインクルード
#include <Fade/include/gfl2_FadeManager.h>

//  nijiのインクルード
#include "System/include/ArcIdGetter.h"
#include "System/include/HeapDefine.h"
#include "System/include/PokemonVersion.h"

#include "PokeTool/include/PokeModelSystem.h"

#include "Applib/include/Frame/AppFrameManager.h"
#include "Applib/include/Util/AppUtilFileControlState.h"

#include "model/include/gfl2_BaseCamera.h"

//  sun/moonのレイアウト関連ヘッダー
#include "niji_conv_header/app/title/res2d/moon/titleMoon.h"
#include "niji_conv_header/app/title/res2d/moon/titleMoon_anim_list.h"

#include "niji_conv_header/app/title/res2d/sun/titleSun.h"
#include "niji_conv_header/app/title/res2d/sun/titleSun_anim_list.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( title )

  static const gfl2::math::Vector4 s_fadeUpperColor(255.0f,255.0f,255.0f,255.0f);
  static const gfl2::math::Vector4 s_fadeLowerColor(0.0f,0.0f,0.0f,255.0f);

  typedef struct
  {
    u32 lytId;
    u32 lytAnimeMax;
    u32 animeInId;
    u32 animeKeepId;

  } _LYT_SETUP_DATA;

  //  バージョン違いによるデータ
  typedef struct
  { 
    _LYT_SETUP_DATA upperLytSetupData;
    _LYT_SETUP_DATA lowerLytSetupData;

    //  泣き声SEid
    u32 crySeSoundId;

    //  カメラ座標
    gfl2::math::Vector3 camPos;
    //  カメラ回転
    gfl2::math::Vector3 camRotAngle;
    //  カメラの視野角
    f32                 camFov;


    MonsNo  modelMonsNo;

  } _VERSION_DATA;

  static const _VERSION_DATA s_aLocalVersionData[] =
  {
    //  sun
    {
      //  upper lyt
      {
        LYTRES_TITLESUN_TITLE_MIN_UPP_000_BFLYT,
        LA_TITLESUN_TITLE_MIN_LOW_000___NUM,
        LA_TITLESUN_TITLE_MIN_UPP_000_IN,
        LA_TITLESUN_TITLE_MIN_UPP_000_KEEP
      },

      //  lower lyt
      {
        LYTRES_TITLESUN_TITLE_MIN_LOW_000_BFLYT,
        LA_TITLESUN_TITLE_MIN_UPP_000___NUM,
        LA_TITLESUN_TITLE_MIN_LOW_000_IN,
        LA_TITLESUN_TITLE_MIN_LOW_000_KEEP
      },

      //  crySeSoundId
      SEQ_SE_TITLESTART_SUN,

      //  camPos
      gfl2::math::Vector3( 740, -150, 1220 ),

      //  camRotAngle
      gfl2::math::Vector3( GFL_MATH_DEG_TO_RAD( 16 ), GFL_MATH_DEG_TO_RAD( 33 ), GFL_MATH_DEG_TO_RAD( 0 ) ),

      //  camFov
      0.314873f,

      //  poke model
      MONSNO_NIKKOU,
    },

    //  moon
    {
      //  upper lyt
      {
        LYTRES_TITLEMOON_TITLE_MIN_UPP_000_BFLYT,
        LA_TITLEMOON_TITLE_MIN_LOW_000___NUM,
        LA_TITLEMOON_TITLE_MIN_UPP_000_IN,
        LA_TITLEMOON_TITLE_MIN_UPP_000_KEEP
      },

      //  lower lyt
      {
        LYTRES_TITLEMOON_TITLE_MIN_LOW_000_BFLYT,
        LA_TITLEMOON_TITLE_MIN_UPP_000___NUM,
        LA_TITLEMOON_TITLE_MIN_LOW_000_IN,
        LA_TITLEMOON_TITLE_MIN_LOW_000_KEEP
      },

      //  crySeSoundId
      SEQ_SE_TITLESTART_MOON,

      //  camPos
      gfl2::math::Vector3( 0, -285, 1475 ),

      //  camRotAngle
      gfl2::math::Vector3( GFL_MATH_DEG_TO_RAD( 19 ), GFL_MATH_DEG_TO_RAD( 0 ), GFL_MATH_DEG_TO_RAD( 0 ) ),

      //  camFov
      0.31487f,

      //  poke model
      MONSNO_GEKKOU,
    },
  };

  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  TitleFrame::TitleFrame(void)
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  TitleFrame::~TitleFrame()
  {
  }

  void TitleFrame::Initialize( APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );
    GFL_ASSERT( pAppParam->in_param.p_rendering_manager );
    GFL_ASSERT( pAppParam->in_param.p_parent_app_heap );
    GFL_ASSERT( pAppParam->in_param.p_parent_dev_heap );

    m_pAppParam = pAppParam;

    //  出力パラメータを初期化
    {
      m_pAppParam->out_param.b_ret_debug_seq  = false;
    }

    //  version違いで使用するデータに違いをつける
    {
      m_arcId = NIJI_ARCID_NUM( ARCID_TITLE );
      if( GET_VERSION() == VERSION_SUN2 )
      {
        m_versionType = VERSION_TYPE_SUN;
      }
      else
      {
        m_versionType = VERSION_TYPE_MOON;
      }
    }

    // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
    m_pManagerHeapBase = GFL_CREATE_LOCAL_HEAP_NAME( m_pAppParam->in_param.p_parent_app_heap, SIZE_FRAME_MANAGER, gfl2::heap::HEAP_TYPE_EXP, false, "TitleFrame" );

    // このアプリ全体に必要なメモリを作成
    m_pAppHeap = GFL_NEW( m_pManagerHeapBase ) app::util::Heap();

    // ローカルヒープを作る
    m_pAppHeap->LocalHeapCreate( m_pAppParam->in_param.p_parent_app_heap, m_pAppParam->in_param.p_parent_dev_heap, SIZE_MY_APP, SIZE_MY_APPDEV );

    //  ファイル制御ステータス
    {
      //  レイアウトデータをファイル制御
      {
        m_pFileCtrlStateList[ FILE_DATA_UI_LYT ]  = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) app::util::AppUtilFileControlState( m_arcId, 0, m_pAppHeap->GetDeviceHeap(), true, 128 );
      }
    }

    //  ポケモンモデルシステム作成
    m_pPokeModelSys = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) PokeTool::PokeModelSystem();
    GFL_ASSERT( m_pPokeModelSys );

    //  ポケモンモデルの3D作成
    {
      //  カメラ生成
      app::util::AppRenderingManager::CreateDefaultCamera( m_pAppHeap, &m_pDrawEnvNode, &m_pCamera );

      //  カメラ設定
      {
        const _VERSION_DATA* pVersionData = &s_aLocalVersionData[ m_versionType ];

        m_pCamera->SetPosition( pVersionData->camPos );
        m_pCamera->SetRotation( pVersionData->camRotAngle.x, pVersionData->camRotAngle.y, pVersionData->camRotAngle.z );
        //  下画面の縦横にアスペクト比
        m_pCamera->SetAspectRatio( 320.0f / 240.0f );
        m_pCamera->SetFovy( pVersionData->camFov );
      }

      //  カメラ設定
      pAppParam->in_param.p_rendering_manager->SetCamera( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pCamera );
    }

    //  強制フェードアウト状態に
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      pFadeManager->RequestDefaultOut( gfl2::Fade::DISP_DOUBLE );
    }

    _SetState( STATE_TYPE_LOAD );
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result TitleFrame::InitFunc(void)
  {
    GFL_ASSERT( m_pAppHeap );
    GFL_ASSERT( m_pAppParam );
    GFL_ASSERT( m_pAppParam->in_param.p_rendering_manager );

    //  ロードステートでないとarcファイル処理はしない
    if( m_state == STATE_TYPE_LOAD )
    {
      bool  bEnd  = true;

      //  他のファイル処理
      {
        u32 loopNum = GFL_NELEMS( m_pFileCtrlStateList );
        for( u32 i = 0; i < loopNum; ++i )
        {
          m_pFileCtrlStateList[ i ]->Update();

          if( m_pFileCtrlStateList[ i ]->IsEnd() == false )
          {
            bEnd  = false;
          }
        }
      }

      //  ポケモンモデルシステム読み込みまち
      {
        if( m_pPokeModelSys->IsFinishInitSystemAsync() == false )
        {
          bEnd  = false;
        }
      }

      if( bEnd )
      {
        _SetState( STATE_TYPE_LOAD_MODEL );
      }
    }

    if( m_state == STATE_TYPE_LOAD_MODEL )
    {
      m_pPokeModelObj->Update();
      if( m_pPokeModelObj->IsReady() )
      {
        m_pPokeModelObj->SetPos( gfl2::math::Vector3( 0.0f, 0.0f, 0.0f ) );

        //  ロードを終了して更新処理へ
        return applib::frame::RES_FINISH;
      }
    }

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result TitleFrame::UpdateFunc(void)
  {
    if( m_state < STATE_TYPE_OPEN )
    {
      _SetState( STATE_TYPE_OPEN );
    }

    if( m_state == STATE_TYPE_OPEN )
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      //  上下画面同じフェード設定しているので、片方見るだけでよい
      if( pFadeManager->IsEnd( gfl2::Fade::DISP_LOWER ) == true )
      {
        _SetState( STATE_TYPE_UPDATE );
      }
    }

    if( m_state == STATE_TYPE_UPDATE )
    {
    }

    if( m_state == STATE_TYPE_CLOSE )
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      //  上下画面同じフェード設定しているので、片方見るだけでよい
      if( pFadeManager->IsEnd( gfl2::Fade::DISP_LOWER ) == true )
      {
        _SetState( STATE_TYPE_END );
      }
    }

    if( m_state == STATE_TYPE_END )
    {
      if( m_pPokeModelObj->IsDelete() )
      {
        return applib::frame::RES_FINISH;
      }
    }

    if( m_pUpperUIDraw != NULL )
    {
        m_pUpperUIDraw->Update();
    }

    if( m_pLowerUIDraw != NULL )
    {
      m_pLowerUIDraw->Update();
    }

    if( m_pPokeModelObj != NULL )
    {
      m_pPokeModelObj->Update();
    }

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  void TitleFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
  {
    if( ( no == gfl2::gfx::CtrDisplayNo::LEFT ) || ( no == gfl2::gfx::CtrDisplayNo::RIGHT ) )
    {
      if( m_pUpperUIDraw != NULL )
      {
        m_pUpperUIDraw->DrawTree( no );
      }
    }

    if( m_pLowerUIDraw != NULL )
    {
      m_pLowerUIDraw->DrawTree( no );
    }

    if( m_pPokeModelObj != NULL )
    {
      m_pPokeModelObj->Draw( no );
    }
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの終了を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  applib::frame::Result TitleFrame::EndFunc(void)
  {
    enum
    {
      SEQ_SYS_POKE_DELETE = 0,
      SEQ_SYS_POKE_DELETE_WAIT,
      SEQ_END
    };

    s32 seq = applib::frame::CellSuper::GetSubSeq();

    if( seq == SEQ_SYS_POKE_DELETE )
    {
      m_pPokeModelSys->TermSystemAsync();

      ++seq;
    }
    else
    if( seq == SEQ_SYS_POKE_DELETE_WAIT )
    {
      if( m_pPokeModelSys->IsFinishTermSystemAsync() == true )
      {
        ++seq;
      }
    }
    else
    if( seq == SEQ_END )
    {
      m_pAppParam->in_param.p_rendering_manager->UnsetCamera( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY );

      GFL_SAFE_DELETE( m_pPokeModelObj );
      GFL_SAFE_DELETE( m_pPokeModelSys );
      GFL_SAFE_DELETE( m_pUpperUIDraw );
      GFL_SAFE_DELETE( m_pLowerUIDraw );

      {
        u32 loopNum = GFL_NELEMS( m_pFileCtrlStateList );
        for( u32 i = 0; i < loopNum; ++i )
        {
          GFL_SAFE_DELETE( m_pFileCtrlStateList[ i ] );
        }
      }

      GFL_SAFE_DELETE( m_pDrawEnvNode );
      GFL_SAFE_DELETE( m_pCamera );

      GFL_SAFE_DELETE( m_pAppHeap );

      gfl2::heap::Manager::DeleteHeap( m_pManagerHeapBase );

      return applib::frame::RES_FINISH;
    }

    applib::frame::CellSuper::SetSubSeq( seq );

    return applib::frame::RES_CONTINUE;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  タッチパネルイベントの検知
  *
  * @param  pTouchPanel タッチパネル
  * @param  isTouch     タッチ状態であるならtrue、タッチされていないならfalse
  *                     (タッチトリガを調べたい場合はTouchPanel::IsTouchTrigger()にて判断可能）
  *
  * @return 同フレーム内での他入力イベントを許可するならtrue、
  *         他のイベントを排除するならfalseを返却すること。
  */
  //--------------------------------------------------------------------------------------------
  // overwride
  ::app::ui::UIInputListener::ListenerResult TitleFrame::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
  {
    return ENABLE_ACCESS;
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
  ::app::ui::UIInputListener::ListenerResult TitleFrame::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      _SetState( STATE_TYPE_CLOSE );

      return DISABLE_ACCESS;
    }
    else
    if( pButton->IsTrigger( gfl2::ui::BUTTON_START ) )
    {
      _SetState( STATE_TYPE_CLOSE );

      return DISABLE_ACCESS;
    }
    else
    if( ( pButton->IsHold( gfl2::ui::BUTTON_UP ) == true ) &&
        ( pButton->IsHold( gfl2::ui::BUTTON_B  ) == true ) &&
        ( pButton->IsHold( gfl2::ui::BUTTON_X ) == true ) )
    {
      m_pAppParam->out_param.b_ret_del_save_seq  = true;
      _SetState( STATE_TYPE_CLOSE );
      return DISABLE_ACCESS;
    }
#if PM_DEBUG
    //  デバッグフローに戻るかどうか
    else
    if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
    {
      m_pAppParam->out_param.b_ret_debug_seq  = true;
      _SetState( STATE_TYPE_CLOSE );

      return DISABLE_ACCESS;
    }
#endif

    return ENABLE_ACCESS;
  }

  //------------------------------------------------------------------
  /**
    * @brief 状態ステート設定
  */
  //------------------------------------------------------------------
  void TitleFrame::_SetState( const StateType newStateType )
  {
    m_state = newStateType;

    if( m_state == STATE_TYPE_LOAD )
    {
      u32 loopNum = GFL_NELEMS( m_pFileCtrlStateList );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_pFileCtrlStateList[ i ]->Start();
      }

      m_pPokeModelSys->InitSystemAsync( m_pAppHeap->GetDeviceHeap(), m_pAppHeap->GetDeviceHeap(), GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager(), 1 );
    }
    else
    if( m_state == STATE_TYPE_LOAD_MODEL )
    {
      //システムの生成(colorShaderが無いならAllocatorはいらない)
      m_pPokeModelSys->CreateSystem(false,NULL);

      //  モデル作成
      {
        PokeModelObj::INIT_PARAM  initParam;

        //  必要なパラメータを設定
        {
          PokeTool::PokeModel::SetupOption* pOption = &initParam.setupOpt;
          pOption->dataHeap = m_pAppHeap->GetDeviceHeap();
          pOption->workHeap = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
        }

        {
          PokeTool::SimpleParam* pParam  = &initParam.setupParam;
          pParam->monsNo = s_aLocalVersionData[ m_versionType ].modelMonsNo;
        }

        initParam.pPokeModelSys       = m_pPokeModelSys;
        initParam.pRenderingManager   = m_pAppParam->in_param.p_rendering_manager;

        m_pPokeModelObj = GFL_NEW( m_pManagerHeapBase ) PokeModelObj( initParam );
        m_pPokeModelObj->Initialize();
      }
    }
    else
    if( m_state == STATE_TYPE_OPEN )
    {
      //  フェードイン
      {
        gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
        pFadeManager->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );
      }

      //  BGM再生
      {
        Sound::StartBGMReq( STRM_BGM_POKEMON_THEME, Sound::BGM_FADE_VERY_FAST );
      }

      //  上画面クラスを作成
      {
        TitleUpperUIDraw::INIT_PARAM  drawInitParam;
        drawInitParam.pLytBuff            = m_pFileCtrlStateList[ FILE_DATA_UI_LYT ]->GetLoadDataBuffer();
        GFL_ASSERT( drawInitParam.pLytBuff );

        drawInitParam.pRenderingManager   = m_pAppParam->in_param.p_rendering_manager;
        drawInitParam.pUtilHeap           = m_pAppHeap;
        //  レイアウト作成に必要なパラメータを設定
        {
          const _LYT_SETUP_DATA*  pLytSetupData = &s_aLocalVersionData[ m_versionType ].upperLytSetupData;

          drawInitParam.animeInId   = pLytSetupData->animeInId;
          drawInitParam.animeKeepId = pLytSetupData->animeKeepId;
          drawInitParam.lytAnimeMax = pLytSetupData->lytAnimeMax;
          drawInitParam.lytId       = pLytSetupData->lytId;
        }

        m_pUpperUIDraw  = GFL_NEW( m_pManagerHeapBase ) TitleUpperUIDraw( drawInitParam );

        m_pUpperUIDraw->Initialize();

        //  入力コールバック設定
        m_pUpperUIDraw->SetInputListener( this );

        //  開始
        m_pUpperUIDraw->Start();
      }

      //  下画面クラスを作成
      {
        TitleLowerUIDraw::INIT_PARAM  drawInitParam;
        drawInitParam.pLytBuff            = m_pFileCtrlStateList[ FILE_DATA_UI_LYT ]->GetLoadDataBuffer();
        GFL_ASSERT( drawInitParam.pLytBuff );

        drawInitParam.pRenderingManager   = m_pAppParam->in_param.p_rendering_manager;
        drawInitParam.pUtilHeap           = m_pAppHeap;

        //  レイアウト作成に必要なパラメータを設定
        {
          const _LYT_SETUP_DATA*  pLytSetupData = &s_aLocalVersionData[ m_versionType ].lowerLytSetupData;

          drawInitParam.animeInId   = pLytSetupData->animeInId;
          drawInitParam.animeKeepId = pLytSetupData->animeKeepId;
          drawInitParam.lytAnimeMax = pLytSetupData->lytAnimeMax;
          drawInitParam.lytId       = pLytSetupData->lytId;
        }

        m_pLowerUIDraw  = GFL_NEW( m_pManagerHeapBase ) TitleLowerUIDraw( drawInitParam );

        m_pLowerUIDraw->Initialize();

        //  開始
        m_pLowerUIDraw->Start();
      }

      //  ポケモンアニメ再生
      {
        m_pPokeModelObj->SetAnime( PokeTool::MODEL_ANIME_BT_WAIT_A, true );
      }

#if GFL_DEBUG_PRINT
      // ヒープ残量チェック
      GFL_PRINT( "TitleFrame: 0x%x/0x%x, 0x%x/0x%x\n\n", 
        m_pAppParam->in_param.p_parent_app_heap->GetTotalFreeSize(), m_pAppParam->in_param.p_parent_app_heap->GetTotalSize(),
        m_pAppParam->in_param.p_parent_dev_heap->GetTotalFreeSize(), m_pAppParam->in_param.p_parent_dev_heap->GetTotalSize() );
#endif
      m_pUpperUIDraw->SetInputEnabled( false );
    }
    else
    if( m_state == STATE_TYPE_UPDATE )
    {
      m_pUpperUIDraw->SetInputEnabled( true );
    }
    else
    if( m_state == STATE_TYPE_CLOSE )
    {
      //  なきごえSE
      {
        Sound::PlaySE( s_aLocalVersionData[ m_versionType ].crySeSoundId );
      }

      m_pUpperUIDraw->SetInputEnabled( false );

      m_pUpperUIDraw->End();
      m_pLowerUIDraw->End();

      //  フェードアウト
      {
        gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );

        gfl2::math::Vector4 sCouUpper  = s_fadeUpperColor;
        sCouUpper.w = 0.0f;

        gfl2::math::Vector4 sCouLower  = s_fadeLowerColor;
        sCouLower.w = 0.0f;

        pFadeManager->RequestOut( gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, &sCouUpper, &s_fadeUpperColor );
        pFadeManager->RequestOut( gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA, &sCouLower, &s_fadeLowerColor );
      }

      //  BGM停止
      {
        Sound::EndBGMReq( Sound::BGM_FADE_VERY_FAST );
      }
    }
    else
    if( m_state == STATE_TYPE_END )
    {
      m_pUpperUIDraw->SetInputEnabled( false );

      m_pUpperUIDraw->Terminate();
      m_pLowerUIDraw->Terminate();
      m_pPokeModelObj->Terminate();
    }
  }

GFL_NAMESPACE_END( title )
GFL_NAMESPACE_END( app )
