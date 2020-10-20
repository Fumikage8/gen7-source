#if PM_DEBUG
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldDebugInfo.cpp
 *	@brief  フィールドデバッグインフォ―
 *	@author	tomoya takahashi
 *	@date		2012.09.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <util/include/gfl2_DrawUtilText.h>


#include "Field/FieldRo/include/Debug/FieldDebugInfo.h"

#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"

#include "Field/FieldRo/include/Debug/FieldDebugMenu.h"
#include "Field/FieldRo/include/Debug/FieldDebugMapJump.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

#include "../EventCheck/FieldEventCheck.h"
#include "Debug/DebugWin/include/DebugWinSystem.h"
#include "Field/FieldRo/include/Debug/FieldDebugDispInfo.h"

#include "System/include/GflUse.h"
#include "App/Finder/include/Field/DebugFinderInfo.h"

#include "Field/FieldRo/include/EnvSound/EnvSoundManager.h"

// cameara
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaFocus.h"

GFL_NAMESPACE_BEGIN(Field)

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

#define VEC4_COLOR_WHITE gfl2::math::Vector4( 1.0f, 1.0f, 1.0f, 1.0f )
#define VEC4_COLOR_RED gfl2::math::Vector4( 1.0f, 0.0f, 0.0f, 1.0f )

  // モーション情報
  s32 DebugInfoWin::m_DebugInfo_MotionUs;
  s32 DebugInfoWin::m_DebugInfo_MotionNum;
  
  // コリジョン情報
  s32 DebugInfoWin::m_DebugInfo_CollisionUs;
  s32 DebugInfoWin::m_DebugInfo_CollisionTriNum;

  // メッシュレンダリング　情報
  s32 DebugInfoWin::m_DebugInfo_MeshRenderUs;
  s32 DebugInfoWin::m_DebugInfo_MeshRenderNum;

  // レイアウト　レンダリング　情報
  s32 DebugInfoWin::m_DebugInfo_LayoutUpperUs;
  s32 DebugInfoWin::m_DebugInfo_LayoutUpperNum;
  s32 DebugInfoWin::m_DebugInfo_LayoutLowerUs;
  s32 DebugInfoWin::m_DebugInfo_LayoutLowerNum;

  // プレイヤー更新
  s32 DebugInfoWin::m_DebugInfo_PlayerUpdateUs;
  // 動作モデル更新
  s32 DebugInfoWin::m_DebugInfo_MmodelUpdateUs;

  // イベントチェック
  s32 DebugInfoWin::m_DebugInfo_EventCheckUs;

  // ギミック更新
  s32 DebugInfoWin::m_DebugInfo_GimmickUs;

  // シーン計算
  s32 DebugInfoWin::m_DebugInfo_SceneCalcUs;
  s32 DebugInfoWin::m_DebugInfo_SceneSubmitviewUs;

  // SubWindow
  s32 DebugInfoWin::m_DebugInfo_SubwindowDrawUs;

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------

  // コンストラクタ・デストラクタ
  //----------------------------------------------------------------------------
  /**
   *	@brief  コンストラクタ
   */
  //-----------------------------------------------------------------------------
  DebugInfoWin::DebugInfoWin( void ) : 
    m_SetupTimeSeq(0),
    m_SetupTimeCount(0),
    m_ProcTimeSeq(0),
    m_ProcTimeCount(0),
    m_FileReadSeq(0),
    m_FileReadCount(0),
    m_IsDebugDrawOver( false )
  {
    m_pFinderInfo = NULL;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  デストラクタ
   */
  //-----------------------------------------------------------------------------
  DebugInfoWin::~DebugInfoWin()
  {
    this->Finalize();
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  初期化
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::Initialize( Fieldmap * p_fieldmap )
  {
    m_pFieldmap = p_fieldmap;
    this->initializeSetupTime();
    this->initializeProcTime();
    this->initializeFileRead();
    this->initializeDrawUtilText();
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  破棄
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::Finalize( void )
  {
    this->finalizeDrawUtilText();
    this->finalizeProcTime();
    this->finalizeSetupTime();
    this->finalizeFileRead();
  }


  //----------------------------------------------------------------------------
  /**
   *	@brief  更新
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::Update( void )
  {
    this->updateSetupTime();
    this->updateProcTime();
    this->updateFileRead();
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  描画
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::Draw( Type type, const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
  {

    gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
    gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );


    // 描画開始
    gfl2::gfx::GFGL::BeginScene();
    // 簡易描画開始
    gfl2::gfx::DrawUtil::BeginRenderExtension();

    // シザー、カリング、フィルの設定
    gfl2::gfx::DrawUtil::SetCullMode(gfl2::gfx::CullMode::None);
    gfl2::gfx::DrawUtil::SetFillMode(gfl2::gfx::FillMode::Fill);

    // マテリアル設定
    gfl2::gfx::DrawUtil::SetMaterialColor(gfl2::math::Vector4(1,1,1,1));
    gfl2::gfx::DrawUtil::SetMaterialAlpha(1.0f);
    
    // 簡易描画のデプステストを無しに設定する
    gfl2::gfx::DrawUtil::SetDepthTestEnable(false);

    const gfl2::gfx::Surface::SurfaceDesc			&rDesc = gfl2::gfx::GFGL::GetRenderTarget()->GetSurfaceDesc();
    gfl2::gfx::GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);
    gfl2::gfx::GFGL::SetScissor(0, 0, rDesc.width, rDesc.height);
    gfl2::gfx::DrawUtil::SetScreenSize(rDesc.width, rDesc.height);

    this->drawSetupTime(pBackBuffer, pDepthBuffer);
    this->drawProcTime(pBackBuffer, pDepthBuffer);
    this->drawFileRead(pBackBuffer, pDepthBuffer);
    this->drawDebugDrawOver(pBackBuffer, pDepthBuffer);
    this->drawWhereIsThis();
    this->drawHioOn( pBackBuffer, pDepthBuffer );

    //this->drawZoneIDInfo(pBackBuffer, pDepthBuffer);  // 他にあるのでOFF。


    if( type == TYPE_INFO_PERFORMANCE )
    {
      //this->drawPerformance(pBackBuffer, pDepthBuffer); ここに書くようなものは、DebugDispInfoに持っていく。
    }

    if( m_pFinderInfo )
    {
      m_pFinderInfo->Draw();
    }

    m_pFieldmap->GetGameManager()->GetFieldResident()->GetEnvSoundManager()->DebugDrawEnvSoundManagerInfo( m_pDrawUtilText, rDrawContext );
   
    if( Field::Debug::DebugTypes::s_FocusCameraCheck.camera_check_on && m_pFieldmap->GetCameraManager()->GetFocus() )
    {
      m_pFieldmap->GetCameraManager()->GetFocus()->DebugDrawCameraCheckInfo( rDrawContext );
    }

    // 描画終了
    gfl2::gfx::DrawUtil::EndRenderExtension();
    gfl2::gfx::GFGL::EndScene();


    this->drawDrawUtilText();
  }

  // セットアップにかかった時間を表示
  void DebugInfoWin::StartSetupTimeDisp( void )
  {
    this->startSetupTimeDisp();
  }

  // ファイル読み込みのエラー結果設定
  void DebugInfoWin::UpdateFileReadError( bool is_error, s32 readUs )
  {
    this->setParamFileRead( is_error, readUs );
  }


  bool DebugInfoWin::IsDebugInfoOn( void ) 
  {
    return ( FIELD_DEBUG_SWITCH(Debug::DebugTypes::Switch::ENABLE_DEBUG_PERFORMANCE_INFO_DRAW) ) || ( FIELD_DEBUG_SWITCH(Debug::DebugTypes::Switch::VISIBLE_ENVSOUND_INFO) ); 
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  読み込み時間表示モジュール　更新
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::initializeSetupTime( void )
  {
    m_SetupTimeSeq = SEQ_SETUP_NONE;
    m_SetupTimeCount = 0;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  破棄
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::finalizeSetupTime( void )
  {
    this->initializeSetupTime();
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  時間の表示開始
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::startSetupTimeDisp( void )
  {
    if( m_SetupTimeSeq == SEQ_SETUP_NONE ){
      m_SetupTimeSeq = SEQ_SETUP_FADEIN;
      m_SetupTimeCount = 0;
    }
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  処理
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::updateSetupTime( void )
  {
    static const u32 UPDATE_TIME = 120;
    
    switch( m_SetupTimeSeq )
    {
    case SEQ_SETUP_FADEIN:
      {
        ++m_SetupTimeCount;
        if( m_SetupTimeCount > FADE_TIME ){
          ++m_SetupTimeSeq;
          m_SetupTimeCount = 0;
        }
      }
      break;

    case SEQ_SETUP_UPDATE:
      {
        ++m_SetupTimeCount;
        if( m_SetupTimeCount > UPDATE_TIME ){
          ++m_SetupTimeSeq;
          m_SetupTimeCount = FADE_TIME;
        }
      }
      break;

    case SEQ_SETUP_FADEOUT:
      {
        m_SetupTimeCount--;
        if( m_SetupTimeCount <= 0 )
        {
          m_SetupTimeSeq = SEQ_SETUP_END;
          m_SetupTimeCount = 0;
        }
      }
      break;
    }
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  描画更新
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::drawSetupTime( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil )
  {
    // CTRでしか動作しない。
#if defined (GF_PLATFORM_CTR)

    if( (m_SetupTimeSeq == SEQ_SETUP_NONE) || (m_SetupTimeSeq == SEQ_SETUP_END) ) {
      return ;
    }

    static const s32 DRAW_WIDTH = 128;
    static const s32 DRAW_HEIGHT = 16;
    static const s32 DRAW_X = 0;
    static const s32 DRAW_Y_MAX = 240;
    static const s32 DRAW_Y_MIN = DRAW_Y_MAX - DRAW_HEIGHT;

    s32 y;



    switch( m_SetupTimeSeq ){
    case SEQ_SETUP_FADEIN:
    case SEQ_SETUP_FADEOUT:
      y = DRAW_Y_MIN + (((FADE_TIME - m_SetupTimeCount) * (DRAW_Y_MAX - DRAW_Y_MIN)) / FADE_TIME);
      break;

    case SEQ_SETUP_UPDATE:
      y = DRAW_Y_MIN;
      break;
    }

    nn::os::Tick ans = Debug::DebugTypes::s_DebugFieldEndTick - Debug::DebugTypes::s_DebugFieldStartTick;
    s32 time = ans.ToTimeSpan().GetMilliSeconds();

    // 矩形の描画
    if( time < (2 * 1000) ){
      gfl2::gfx::DrawUtil::DrawRect( static_cast<f32>(DRAW_X), static_cast<f32>(y), static_cast<f32>(DRAW_WIDTH), static_cast<f32>(DRAW_HEIGHT), gfl2::math::Vector4( 0.3, 0.3, 0.8, 0.5 ) );
    }else{
      gfl2::gfx::DrawUtil::DrawRect( static_cast<f32>(DRAW_X), static_cast<f32>(y), static_cast<f32>(DRAW_WIDTH), static_cast<f32>(DRAW_HEIGHT), gfl2::math::Vector4( 0.8, 0.3, 0.3, 0.5 ) );
    }


    // 文字列の描画
    m_pDrawUtilText->SetTextScale(0.7f, 0.7f);
    m_pDrawUtilText->SetTextColor(VEC4_COLOR_WHITE);
    m_pDrawUtilText->DrawText(DRAW_X + 2, y + 0, L"setup result %d ms", time );
#endif // defined(GF_PLATFORM_CTR)
  }


  //----------------------------------------------------------------------------
  /**
   *	@brief  プロセスタイム生成
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::initializeProcTime( void )
  {
    m_ProcTimeSeq = SEQ_SETUP_UPDATE;
    m_ProcTimeCount = 0;
    for( u32 i=0; i<P_OV_NUM; i++ ){
      m_ProcTimeOverTime[i] = 0;
    }
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  プロセスタイム破棄
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::finalizeProcTime( void )
  {
    this->initializeProcTime();
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  プロセスタイム　更新
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::updateProcTime( void )
  {
    // 処理落ちしてる？
    s32 is_proc_over = 0;
    s32 time, over;
    
#if GFL_DEBUGWIN_USE
    time = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork)->GetTickNow( ::Debug::DebugWin::DEBUGWIN_TICK_UPDATE ) + GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork)->GetTickNow( ::Debug::DebugWin::DEBUGWIN_TICK_DRAW );
    over = time - 32000;
    is_proc_over = over;
    m_ProcTimeOverTime[0] = over;

#if 0
    // nijiでは、UpdateとDrawの間にVsyncは発生しないので、UPDATEにDRAWを統合する。
    time = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork)->GetTickNow( ::Debug::DebugWin::DEBUGWIN_TICK_DRAW );
    over = time - 16000;
    if( is_proc_over < over ){
      is_proc_over = over;
    }
    m_ProcTimeOverTime[1] = over;
#else
    m_ProcTimeOverTime[1] = 0;
#endif

    time = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork)->GetTickNow( ::Debug::DebugWin::DEBUGWIN_TICK_GPU );
    over = time - 32000;
    if( is_proc_over < over ){
      is_proc_over = over;
    }
    m_ProcTimeOverTime[2] = over;

    switch( m_ProcTimeSeq ){
    case SEQ_SETUP_UPDATE:
      if( is_proc_over > 0 ){
        m_ProcTimeCount ++;
        if( m_ProcTimeCount > FADE_TIME ){
          m_ProcTimeCount = FADE_TIME;
        }
      }
      else{
        --m_ProcTimeCount;
        if( m_ProcTimeCount <= 0 ){
          m_ProcTimeCount = 0;
        }
      }
      break;
    }
#endif // GFL_DEBUGWIN_USE
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  プロセスタイム　描画
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::drawProcTime(const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil )
  {
    static const s32 DRAW_WIDTH = 96;
    static const s32 DRAW_HEIGHT = 16;
    static const s32 DRAW_X = 324;
    static const s32 DRAW_Y_MAX = 240;
    static const s32 DRAW_Y_MIN = DRAW_Y_MAX - DRAW_HEIGHT;

    s32 y;
    s32 y_work;

    if( m_ProcTimeCount > 0 ){

      y = DRAW_Y_MIN + (((FADE_TIME - m_ProcTimeCount) * (DRAW_Y_MAX - DRAW_Y_MIN)) / FADE_TIME);

      y_work = y;
      // 矩形の描画
      for( u32 i=0; i<P_OV_NUM; i++ ){
        if( m_ProcTimeOverTime[i] > 0 )
        {
          switch(i){
          case P_OV_UPDATE:
            gfl2::gfx::DrawUtil::DrawRect( DRAW_X, y_work, DRAW_WIDTH, DRAW_HEIGHT, gfl2::math::Vector4( 0.8, 0.3, 0.3, 0.5 ) );
            break;
          case P_OV_DRAW:
            gfl2::gfx::DrawUtil::DrawRect( DRAW_X, y_work, DRAW_WIDTH, DRAW_HEIGHT, gfl2::math::Vector4( 0.3, 0.8, 0.3, 0.5 ) );
            break;
          case P_OV_GPU:
            gfl2::gfx::DrawUtil::DrawRect( DRAW_X, y_work, DRAW_WIDTH, DRAW_HEIGHT, gfl2::math::Vector4( 0.8, 0.8, 0.3, 0.5 ) );
            break;
          }
          y_work -= DRAW_HEIGHT;
        }
      }


      // 文字列の描画
      m_pDrawUtilText->SetTextScale(0.7f, 0.7f);
      m_pDrawUtilText->SetTextColor(VEC4_COLOR_WHITE);

      y_work = y;
      for( u32 i=0; i<P_OV_NUM; i++ ){
        if( m_ProcTimeOverTime[i] > 0 )
        {
#if defined(GF_PLATFORM_WIN32)
          m_pDrawUtilText->DrawTextW(DRAW_X + 2, y_work + 0, L"over %d us", m_ProcTimeOverTime[i] );
#elif defined(GF_PLATFORM_CTR)
          m_pDrawUtilText->DrawText(DRAW_X + 2, y_work + 0, L"over %d us", m_ProcTimeOverTime[i] );
#endif
          y_work -= DRAW_HEIGHT;
        }
      }
    }
  }

  
  // 
  //----------------------------------------------------------------------------
  /**
   *	@brief  読み込み遅延表示モジュール
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::initializeFileRead( void )
  {
    m_FileReadSeq   = SEQ_FILE_NONE; 
    m_FileReadCount = 0;
    m_FileReadLastError = false;
    m_FileReadLastUs = 0;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  読み込み遅延表示モジュール 破棄
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::finalizeFileRead( void )
  {
    this->initializeFileRead();
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  読み込み遅延表示モジュール 更新
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::updateFileRead( void )
  {
    switch( m_FileReadSeq ){
    case SEQ_FILE_NONE:
      if( m_FileReadLastError ){
        m_FileReadSeq = SEQ_FILE_FADEIN;
        m_FileReadCount = 0;
      }
      break;
      
    case SEQ_FILE_FADEIN:
      ++m_FileReadCount;
      if( m_FileReadCount > FADE_TIME ){
        m_FileReadSeq = SEQ_FILE_UPDATE;
        m_FileReadCount = 0;
      }
      break;

    case SEQ_FILE_UPDATE:
      if( m_FileReadLastError ){
        m_FileReadCount = 0;
      }
      ++m_FileReadCount;
      if( m_FileReadCount >= 30 ){
        m_FileReadSeq = SEQ_FILE_FADEOUT;
        m_FileReadCount = FADE_TIME;
      }
      break;

    case SEQ_FILE_FADEOUT:
      if( m_FileReadLastError ){
        m_FileReadSeq = SEQ_FILE_FADEIN;
      }
      else {
        --m_FileReadCount;
        if( m_FileReadCount <= 0 ){
          m_FileReadSeq = SEQ_FILE_NONE;
          m_FileReadCount = 0;
        }
      }
      break;
    }
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  読み込み遅延表示モジュール 描画
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::drawFileRead(const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil )
  {
    static const s32 DRAW_WIDTH = 96;
    static const s32 DRAW_HEIGHT = 16;
    static const s32 DRAW_X = 200;
    static const s32 DRAW_Y_MAX = 240;
    static const s32 DRAW_Y_MIN = DRAW_Y_MAX - DRAW_HEIGHT;

    if( m_FileReadSeq != SEQ_FILE_NONE ){
      s32 y;

      switch( m_FileReadSeq ){
      case SEQ_FILE_FADEIN:
      case SEQ_FILE_FADEOUT:
        y = DRAW_Y_MIN + (((FADE_TIME - m_FileReadCount) * (DRAW_Y_MAX - DRAW_Y_MIN)) / FADE_TIME);
        break;

      case SEQ_FILE_UPDATE:
        y = DRAW_Y_MIN;
        break;
      }

      if( m_FileReadCount >= 0 ){
        gfl2::gfx::DrawUtil::DrawRect( DRAW_X, y, DRAW_WIDTH, DRAW_HEIGHT, gfl2::math::Vector4( 0.2, 0.2, 0.4, 0.5 ) );


        // 文字列の描画
        m_pDrawUtilText->SetTextScale(0.7f, 0.7f);
        m_pDrawUtilText->SetTextColor(VEC4_COLOR_WHITE);
#if defined(GF_PLATFORM_WIN32)
        m_pDrawUtilText->DrawTextW(DRAW_X+2, y, L"fileread %d", m_FileReadLastUs );
#elif defined(GF_PLATFORM_CTR)
        m_pDrawUtilText->DrawText(DRAW_X+2, y, L"fileread %d", m_FileReadLastUs );
#endif
      }
    }
  }

  void DebugInfoWin::setParamFileRead( bool is_error, s32 readUs )
  {
    m_FileReadLastError = is_error;
    if(m_FileReadLastError){
      m_FileReadLastUs    = readUs;
    }
  }

  void DebugInfoWin::drawPerformance(const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil)
  {
    /*
    static const s32 DRAW_WIDTH = 320;
    static const s32 DRAW_HEIGHT = 240;
    static const s32 DRAW_X = 0;
    static const s32 DRAW_X_0 = 0;
    static const s32 DRAW_X_1 = 160;
    static const s32 DRAW_Y = 2;
    static const s32 DRAW_Y_DIFF = 14;

    enum{
      MOTION = 0,
      COLLISION,
      FELLOW,
      MATERIAL,
      LAYOUT_UPPER,
      LAYOUT_LOWER,
      PLAYER_MMODEL,
      EVENT_CHECK,
      SCENE_CALC,
      SUBWIN,

      MOTION_LIMIT = 4000,
      COL_LIMIT = 40,
      MATERIAL_LIMIT = 11000,
      LAYOUT_UPPER_LIMIT = 600,
      LAYOUT_LOWER_LIMIT = 2000,
      PLAYER_LIMIT = 2300,
      MMODEL_LIMIT = 5000,
      EVENTCHECK_LIMIT = 2000,
      SCENE_CALC_LIMIT = 5000,
      SCENE_SUBMIT_LIMIT = 2000,
       
      
      
      
    };

    gfl2::grp::RenderTarget* pUpperBuf = p_grp->GetCurrentFrameBuffer(gfl2::grp::DISPLAY_LOWER);

    if( p_grp->BindCurrentCommandList(pUpperBuf) )
    {
      gfl2::gfx::DrawUtil::BeginRender(pUpperBuf);
      gfl2::gfx::DrawUtil::MaterialInfo    info;
      info.m_BlendEnable = true;
      gfl2::gfx::DrawUtil::SetMaterial( info );
    
      gfl2::gfx::DrawUtil::DrawRect( 0, 0, DRAW_WIDTH, DRAW_HEIGHT, gfl2::math::Vector4( 0.1, 0.1, 0.3, 0.7 ) );
      gfl2::gfx::DrawUtil::EndRender();

      gfl2::gfx::DrawUtil::BeginRender(pUpperBuf);
      // 文字列の描画
      gfl2::gfx::DrawUtil::SetTextScale(0.4f, 0.5f);
      gfl2::gfx::DrawUtil::SetTextColor(VEC4_COLOR_WHITE);

      if( m_DebugInfo_MotionUs > MOTION_LIMIT ){ gfl2::gfx::DrawUtil::SetTextColor(VEC4_COLOR_RED); }
      gfl2::gfx::DrawUtil::DrawText(DRAW_X_0, DRAW_Y + DRAW_Y_DIFF * MOTION, L"motion %d / %d us", m_DebugInfo_MotionUs, MOTION_LIMIT );
      gfl2::gfx::DrawUtil::DrawText(DRAW_X_1, DRAW_Y + DRAW_Y_DIFF * MOTION, L"motion num %d", m_DebugInfo_MotionNum );
      gfl2::gfx::DrawUtil::SetTextColor(VEC4_COLOR_WHITE);

      if( (m_DebugInfo_CollisionUs > COL_LIMIT) ){ gfl2::gfx::DrawUtil::SetTextColor(VEC4_COLOR_RED); }
      gfl2::gfx::DrawUtil::DrawText(DRAW_X_0, DRAW_Y + DRAW_Y_DIFF * COLLISION, L"collision %d / %d us", m_DebugInfo_CollisionUs, COL_LIMIT );
      gfl2::gfx::DrawUtil::DrawText(DRAW_X_1, DRAW_Y + DRAW_Y_DIFF * COLLISION, L"collision tri num %d", m_DebugInfo_CollisionTriNum );
      gfl2::gfx::DrawUtil::SetTextColor(VEC4_COLOR_WHITE);

      if( (m_DebugInfo_MeshRenderUs > MATERIAL_LIMIT) ){ gfl2::gfx::DrawUtil::SetTextColor(VEC4_COLOR_RED); }
      gfl2::gfx::DrawUtil::DrawText(DRAW_X_0, DRAW_Y + DRAW_Y_DIFF * MATERIAL, L"render mesh %d / %d us", m_DebugInfo_MeshRenderUs, MATERIAL_LIMIT );
      gfl2::gfx::DrawUtil::DrawText(DRAW_X_1, DRAW_Y + DRAW_Y_DIFF * MATERIAL, L"mesh num %d", m_DebugInfo_MeshRenderNum );
      gfl2::gfx::DrawUtil::SetTextColor(VEC4_COLOR_WHITE);

      if( (m_DebugInfo_LayoutUpperUs > LAYOUT_UPPER_LIMIT) ){ gfl2::gfx::DrawUtil::SetTextColor(VEC4_COLOR_RED); }
      gfl2::gfx::DrawUtil::DrawText(DRAW_X_0, DRAW_Y + DRAW_Y_DIFF * LAYOUT_UPPER, L"layout upper %d / %d us", m_DebugInfo_LayoutUpperUs, LAYOUT_UPPER_LIMIT );
      gfl2::gfx::DrawUtil::DrawText(DRAW_X_1, DRAW_Y + DRAW_Y_DIFF * LAYOUT_UPPER, L"layout upper num %d", m_DebugInfo_LayoutUpperNum );
      gfl2::gfx::DrawUtil::SetTextColor(VEC4_COLOR_WHITE);

      if( (m_DebugInfo_LayoutLowerUs > LAYOUT_LOWER_LIMIT) ){ gfl2::gfx::DrawUtil::SetTextColor(VEC4_COLOR_RED); }
      gfl2::gfx::DrawUtil::DrawText(DRAW_X_0, DRAW_Y + DRAW_Y_DIFF * LAYOUT_LOWER, L"layout lower %d / %d us", m_DebugInfo_LayoutLowerUs, LAYOUT_LOWER_LIMIT );
      gfl2::gfx::DrawUtil::DrawText(DRAW_X_1, DRAW_Y + DRAW_Y_DIFF * LAYOUT_LOWER, L"layout lower num %d", m_DebugInfo_LayoutLowerNum );
      gfl2::gfx::DrawUtil::SetTextColor(VEC4_COLOR_WHITE);

      if( (m_DebugInfo_PlayerUpdateUs > PLAYER_LIMIT) ){ gfl2::gfx::DrawUtil::SetTextColor(VEC4_COLOR_RED); }
      gfl2::gfx::DrawUtil::DrawText(DRAW_X_0, DRAW_Y + DRAW_Y_DIFF * PLAYER_MMODEL, L"player update %d / %d us", m_DebugInfo_PlayerUpdateUs, PLAYER_LIMIT );
      gfl2::gfx::DrawUtil::SetTextColor(VEC4_COLOR_WHITE);

      if( (m_DebugInfo_MmodelUpdateUs > MMODEL_LIMIT) ){ gfl2::gfx::DrawUtil::SetTextColor(VEC4_COLOR_RED); }
      gfl2::gfx::DrawUtil::DrawText(DRAW_X_1, DRAW_Y + DRAW_Y_DIFF * PLAYER_MMODEL, L"mmodel update %d / %d us", m_DebugInfo_MmodelUpdateUs, MMODEL_LIMIT );
      gfl2::gfx::DrawUtil::SetTextColor(VEC4_COLOR_WHITE);

      if( (m_DebugInfo_EventCheckUs > EVENTCHECK_LIMIT) ){ gfl2::gfx::DrawUtil::SetTextColor(VEC4_COLOR_RED); }
      gfl2::gfx::DrawUtil::DrawText(DRAW_X_0, DRAW_Y + DRAW_Y_DIFF * EVENT_CHECK, L"event check %d / %d us", m_DebugInfo_EventCheckUs, EVENTCHECK_LIMIT );
      gfl2::gfx::DrawUtil::SetTextColor(VEC4_COLOR_WHITE);

      gfl2::gfx::DrawUtil::DrawText(DRAW_X_1, DRAW_Y + DRAW_Y_DIFF * EVENT_CHECK, L"gimmick update %d us", m_DebugInfo_GimmickUs );

      if( (m_DebugInfo_SceneCalcUs > SCENE_CALC_LIMIT) ){ gfl2::gfx::DrawUtil::SetTextColor(VEC4_COLOR_RED); }
      gfl2::gfx::DrawUtil::DrawText(DRAW_X_0, DRAW_Y + DRAW_Y_DIFF * SCENE_CALC, L"scene calc %d / %d us", m_DebugInfo_SceneCalcUs, SCENE_CALC_LIMIT );
      gfl2::gfx::DrawUtil::SetTextColor(VEC4_COLOR_WHITE);


      if( (m_DebugInfo_SceneSubmitviewUs > SCENE_SUBMIT_LIMIT) ){ gfl2::gfx::DrawUtil::SetTextColor(VEC4_COLOR_RED); }
      gfl2::gfx::DrawUtil::DrawText(DRAW_X_1, DRAW_Y + DRAW_Y_DIFF * SCENE_CALC, L"scene submit %d / %d us", m_DebugInfo_SceneSubmitviewUs, SCENE_SUBMIT_LIMIT );
      gfl2::gfx::DrawUtil::SetTextColor(VEC4_COLOR_WHITE);
      

      gfl2::gfx::DrawUtil::DrawText(DRAW_X_0, DRAW_Y + DRAW_Y_DIFF * SUBWIN, L"subwin draw %dus", m_DebugInfo_SubwindowDrawUs );


      gfl2::gfx::DrawUtil::EndRender();
    }
    */
  }

  void DebugInfoWin::drawDebugDrawOver(const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil)
  {
    static const s32 DRAW_WIDTH = 96;
    static const s32 DRAW_HEIGHT = 16;
    static const s32 DRAW_X = 0;
    static const s32 DRAW_Y = 0;

    if( m_IsDebugDrawOver )
    {
      gfl2::gfx::DrawUtil::DrawRect( DRAW_X, DRAW_Y, DRAW_WIDTH, DRAW_HEIGHT, gfl2::math::Vector4( 0.2, 0.2, 0.4, 0.5 ) );

      // 文字列の描画
      m_pDrawUtilText->SetTextScale(0.7f, 0.7f);
      m_pDrawUtilText->SetTextColor(VEC4_COLOR_WHITE);
#if defined(GF_PLATFORM_WIN32)
      m_pDrawUtilText->DrawTextW(DRAW_X+2, DRAW_Y, L"DebugDrawOver" );
#elif defined(GF_PLATFORM_CTR)
      m_pDrawUtilText->DrawText(DRAW_X+2, DRAW_Y, L"DebugDrawOver" );
#endif
    }
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  ZoneID表示モジュール 描画
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::drawZoneIDInfo( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil )
  {
    u16 zone_id = m_pFieldmap->GetZoneID();
    m_pDrawUtilText->SetTextScale(0.7f, 0.7f);
    m_pDrawUtilText->SetTextColor(VEC4_COLOR_WHITE);
#if defined(GF_PLATFORM_WIN32)
    m_pDrawUtilText->DrawTextW( 10, 10, L"%s", m_pFieldmap->GetFieldDebugMenu()->GetDebugMapJump()->GetZoneName( zone_id ) );
#elif defined(GF_PLATFORM_CTR)
    m_pDrawUtilText->DrawText( 10, 10, L"%s", m_pFieldmap->GetFieldDebugMenu()->GetDebugMapJump()->GetZoneName( zone_id ) );
#endif
  }

  /**
   *  HIOデバッグ機能を使っていたらテキストを表示
   */
  void DebugInfoWin::drawHioOn( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil )
  {
    if( !m_pFieldmap->GetGameManager()->GetAsyncFileManager()->GetHioUseFlag() )
    {
      return ;
    }

    m_pDrawUtilText->SetTextScale(0.7f, 0.7f);
    m_pDrawUtilText->SetTextColor(VEC4_COLOR_RED);
#if defined(GF_PLATFORM_WIN32)
    m_pDrawUtilText->DrawTextW( 10, 20, L"DebugHioOn" );
#elif defined(GF_PLATFORM_CTR)
    m_pDrawUtilText->DrawText( 10, 20, L"DebugHioOn" );
#endif
  }


  //----------------------------------------------------------------------------
  /**
   *	@brief テキスト描画ルーチンの初期化
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::initializeDrawUtilText(void)
  {
    m_pDrawUtilText = Debug::DebugTypes::s_pDrawUtilTextUpper;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief テキスト描画ルーチンの破棄
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::finalizeDrawUtilText(void)
  {
    m_pDrawUtilText = NULL;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief テキスト描画ルーチンでの描画
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::drawDrawUtilText(void)
  {
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief ココドコ
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::drawWhereIsThis( void )
  {
    if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::VISIBLE_WHERE_IS_THIS ) == 0 )
    {
      return;
    }

    // 裏板
    gfl2::gfx::DrawUtil::DrawRect( 
      0.0f, 0.0f,
      220.0f, 74.0f,
      gfl2::math::Vector4( 0.3f, 0.3f, 0.3f, 0.5f )
      );

    // 文字
    drawZone();
    drawAttribute();
    drawStep();
    drawPlayer();
    drawTime();
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief ゾーンID表示
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::drawZone( void )
  {
    u32 zoneID = m_pFieldmap->GetZoneID();
    Field::Debug::DebugDispInfo *pDebug = m_pFieldmap->GetFieldDebugMenu()->GetDebugDispInfo();
    if( !pDebug ){ return; }

    m_pDrawUtilText->SetTextScale( 0.7f, 0.7f );
    m_pDrawUtilText->SetTextColor( VEC4_COLOR_WHITE );
#if defined(GF_PLATFORM_WIN32)
    m_pDrawUtilText->DrawTextW( 5, 0, L"Zone[%s]", pDebug->GetZoneName( zoneID ) );
#elif defined(GF_PLATFORM_CTR)
    m_pDrawUtilText->DrawText( 5, 0, L"Zone[%s]", pDebug->GetZoneName( zoneID ) );
#endif
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief アトリビュート表示
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::drawAttribute( void )
  {
    Field::MoveModel::FieldMoveModel *pChara = m_pFieldmap->GetPlayerCharacter();
    Field::Debug::DebugDispInfo *pDebug = m_pFieldmap->GetFieldDebugMenu()->GetDebugDispInfo();
    if( !pChara ){ return; }
    if( !pDebug ){ return; }
    u32 attr = pChara->GetGroundAttribute();

    m_pDrawUtilText->SetTextScale( 0.7f, 0.7f );
    m_pDrawUtilText->SetTextColor( VEC4_COLOR_WHITE );
#if defined(GF_PLATFORM_WIN32)
    m_pDrawUtilText->DrawTextW( 5, 12, L"Attr[%s]", pDebug->GetAttributeName( attr ) );
#elif defined(GF_PLATFORM_CTR)
    m_pDrawUtilText->DrawText( 5, 12, L"Attr[%s]", pDebug->GetAttributeName( attr ) );
#endif
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief 歩数デバッグ表示
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::drawStep( void )
  {
    u32 move_count = Field::FieldEventCheck::m_DebugMoveStepCount;
    u32 step_count = Field::FieldEventCheck::m_DebugMoveStepCount;
    m_pDrawUtilText->SetTextScale( 0.7f, 0.7f );
    m_pDrawUtilText->SetTextColor( VEC4_COLOR_WHITE );
#if defined(GF_PLATFORM_WIN32)
    m_pDrawUtilText->DrawTextW( 10, 24, L"MoveStepCount[%3d] StepCount[%3d]", move_count, step_count );
#elif defined(GF_PLATFORM_CTR)
    m_pDrawUtilText->DrawText( 10, 24, L"MoveStepCount[%3d] StepCount[%3d]", move_count, step_count );
#endif
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief プレイヤーデバッグ表示
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::drawPlayer( void )
  {
    Field::MoveModel::FieldMoveModel *pChara = m_pFieldmap->GetPlayerCharacter();
    if( !pChara ){ return; }
    gfl2::math::Vector3 trans = pChara->GetLocalSRT().translate;
    u32 animID = 0;
    if( pChara->GetCharaDrawInstance() )
    {
      animID = pChara->GetCharaDrawInstance()->GetAnimationId();
    }

    m_pDrawUtilText->SetTextScale( 0.7f, 0.7f );
    m_pDrawUtilText->SetTextColor( VEC4_COLOR_WHITE );
#if defined(GF_PLATFORM_WIN32)
    m_pDrawUtilText->DrawTextW( 10, 36, L"PlayerPos[%7.3f, %7.3f, %7.3f]", trans.x, trans.y, trans.z );
    m_pDrawUtilText->DrawTextW( 10, 48, L"PlayerAnim[%d]", animID );
#elif defined(GF_PLATFORM_CTR)
    m_pDrawUtilText->DrawText( 10, 36, L"PlayerPos[%7.3f, %7.3f, %7.3f]", trans.x, trans.y, trans.z );
    m_pDrawUtilText->DrawText( 10, 48, L"PlayerAnim[%d]", animID );
#endif
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief 時間デバッグ表示
   */
  //-----------------------------------------------------------------------------
  void DebugInfoWin::drawTime( void )
  {
    GameSys::AlolaTime time;
    time.Initialize();

    m_pDrawUtilText->SetTextScale( 0.7f, 0.7f );
    m_pDrawUtilText->SetTextColor( VEC4_COLOR_WHITE );
#if defined(GF_PLATFORM_WIN32)
    m_pDrawUtilText->DrawTextW( 10, 60, L"Time[%02d:%02d:%02d]", time.GetHour(), time.GetMinute(), time.GetSecond() );
#elif defined(GF_PLATFORM_CTR)
    m_pDrawUtilText->DrawText( 10, 60, L"Time[%02d:%02d:%02d]", time.GetHour(), time.GetMinute(), time.GetSecond() );
#endif
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief テキスト描画前のレンダリング　実行
   */
  //-----------------------------------------------------------------------------
  void DebugTextBeforeRenderPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
  {
    if( !m_pFieldmap )
    {
      return ;
    }

    // DrawInfo
    if( DebugInfoWin::IsDebugInfoOn() )
    {
      DebugInfoWin * pInfo = m_pFieldmap->GetFieldDebugInfoWin();

      // 左目にしかレンダリングしません。
      if( rDrawContext.m_pRenderTarget == gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] ) )
      {
        pInfo->Draw( DebugInfoWin::TYPE_INFO_ONLY, rDrawContext );
      }
    }
  }


GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG
