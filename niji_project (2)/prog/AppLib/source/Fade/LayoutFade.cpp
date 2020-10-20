//======================================================================
/**
 * @file LayoutFade.cpp
 * @date 2015/07/24 11:51:54
 * @author miyachi_soichi
 * @brief フェード：レイアウト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gfl2
#include <thread/include/gfl2_ThreadStatic.h>
#include <Fade/include/gfl2_FadeManager.h>
#include <Layout/include/gfl2_LytSys.h>
// Fade
#include "AppLib/include/Fade/LayoutFade.h"
#include "AppLib/include/Fade/FadeUtil.h"
// AppLib
#include "AppLib/include/util/app_util_2d.h"
#include "AppLib/include/tool/app_tool_LytTexReplaceSystem.h"
// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/fade.gaix>
#include <niji_conv_header/app/fade/fade.h>
#include <niji_conv_header/app/fade/fade_anim_list.h>
#include "niji_conv_header/app/fade/fade_pane.h"

GFL_NAMESPACE_BEGIN(AppLib)
GFL_NAMESPACE_BEGIN(Fade)

#define TEST_SS_FADE 0

/**
 *  @class  LayoutFade
 *  @brief  フェード：レイアウト
 */
/**
 * @brief  コンストラクタ
 */
LayoutFade::LayoutFade( void )
: gfl2::Fade::FadeSuper()
, m_pAsyncFileManager( NULL )
, m_pAppHeap( NULL )
, m_pRenderManager( NULL )
, m_pG2DUtil( NULL )
, m_pLayoutData( NULL )
, m_eMode( gfl2::Fade::MODE_ALPHA )
, m_eState( STATE_NONE )
, m_nOutAnim( -1 )
, m_nOutPane( -1 )
, m_nInAnim( -1 )
, m_nInPane( -1 )
, m_nFadeLifeNow( 0 )
, m_nFadeLifeMax( 0 )
, m_bPause( false )
, m_homeNixFade( 0 )
, m_isHomeNixRequest( false )
, m_homeNixStartTime( 0 )
, m_FadeResult( gfl2::Fade::FADE_RESULT_IN )
{
  m_ColorNow = gfl2::math::Vector4::GetZero();
  m_ColorAdd = gfl2::math::Vector4::GetZero();
  m_ColorEnd = gfl2::math::Vector4::GetZero();
}

/**
 *  @brief  デストラクタ
 */
LayoutFade::~LayoutFade( void )
{
  Terminate();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 *  @param  pAppHeap        ヒープアロケータ
 *  @param  pRenderManager  レンダーマネージャ
 *  @param  pAsyncManager   ファイルマネージャ
 */
void LayoutFade::Initialize( app::util::Heap *pAppHeap, app::util::AppRenderingManager *pRenderManager, gfl2::fs::AsyncFileManager *pAsyncManager )
{
  // 設定
  m_pAppHeap            = pAppHeap;
  m_pRenderManager      = pRenderManager;
  m_pAsyncFileManager   = pAsyncManager;

  // ファイルロード
  fileLoadBlock();
  // レイアウト作成
  createLayout();
}

/**
 *  @brief  破棄
 */
void LayoutFade::Terminate( void )
{
  // 未初期化回避
  if( m_eState == STATE_NONE ) { return; }

  m_pG2DUtil->DeleteLayoutWork();
  m_pG2DUtil->DeleteLayoutEnv();
  m_pG2DUtil->DeleteLayoutSystem();
  m_pG2DUtil->DeleteAppLytAccessor();
  GFL_SAFE_DELETE( m_pG2DUtil );
  GflHeapFreeMemory(m_pLayoutData);
}

/**
 *  @brief  更新処理
 */
void LayoutFade::UpdateFunc( void )
{
  bool isUpdate2D = false;
  do 
  {
    // ポーズ
    if( m_bPause )               { break; }
    // 未初期化回避
    if( m_eState == STATE_NONE ) { break; }
    if( m_eState == STATE_WAIT ) { break; }

    bool bContinue = true;
    switch( m_eMode )
    {
    case gfl2::Fade::MODE_ALPHA:
      bContinue = updateColorMode();
      break;
    case gfl2::Fade::MODE_CROSS:
      bContinue = updateCrossMode();
      break;
    default:
      bContinue = updateLayoutMode();
      break;
    }

    if( !bContinue )
    {
      Reset();
    }

    isUpdate2D = true;
  } while (false);

  //Homeアイコン
  if( UpdateHomeNix() )
  {
    isUpdate2D = true;
  }

  if( isUpdate2D )
  {
    // レイアウト更新
    m_pG2DUtil->Update2D();
  }
}

/**
 *  @brief  描画処理
 */
void LayoutFade::DrawFunc( gfl2::gfx::CtrDisplayNo displayNo )
{
  // 未初期化回避
  if( m_eState == STATE_NONE ) { return; }

  // 描画登録
  m_pG2DUtil->AddDrawableLytWkForOneFrame( m_pRenderManager, displayNo, 0 );
}

/**
 *  @brief  フェードアウト リクエスト
 *  @param  type      演出の種類
 *  @param  start_col スタート時のRGBA (0-1.0f) NULL だと前回の色からスタート
 *  @param  end_col   終了時のRGBA (0-1.0f)
 *  @param  sync      フェードにかけるSYNC
 *  @param  capture   開始時のバッファを保持するか？
 */
void LayoutFade::RequestOut( gfl2::Fade::FADE_TYPE type, const gfl2::math::Vector4 *start_col, const gfl2::math::Vector4 *end_col, u32 sync, bool capture )
{
  // 安全のためOutを消す
  if( m_nOutPane != -1 )
  {
    m_pG2DUtil->SetPaneVisible( 0, m_nOutPane, false );
  }
  if( m_nOutAnim != -1 )
  {
    m_pG2DUtil->StopAnime( 0, m_nOutAnim, true );
  }

  // 再生速度を戻す
  m_pG2DUtil->SetAnimeSpeed( 0, 1.0f );
  m_eState = STATE_OUT;
  m_FadeResult = gfl2::Fade::FADE_RESULT_OUT;
  m_nFadeLifeNow = 0;
  m_nFadeLifeMax = sync;
  sync = gfl2::math::Max<u32>( sync, 1 ); // @fix GFNMCat[1411] 0div対応

  // 画面スナップショットをマウントする
  if( capture || type == gfl2::Fade::FADE_TYPE_CROSS )
  {
    mountSnapshot();
    m_pG2DUtil->SetPaneVisible( 0, PANENAME_FADE_BG_UPPER_PANE_CROSS_FADE, true );
    // @fix GFNMCat[1219]: クロス->Capの場合Paneの状態を戻す必要がある
    m_pG2DUtil->SetPaneVisible( 0, PANENAME_FADE_BG_UPPER_PANE_CROSS_TEXTURE, true );
    m_pG2DUtil->SetPaneAlpha( 0, PANENAME_FADE_BG_UPPER_PANE_CROSS_TEXTURE, 255 );
  }
  else
  {
    m_pG2DUtil->SetPaneVisible( 0, PANENAME_FADE_BG_UPPER_PANE_CROSS_FADE, false );
  }

  u32 eNewMode = AppLib::Fade::Util::GetFadeParamForFadeOut( type, m_nOutAnim, m_nOutPane );
  // Outに必要なパラメータを取得する
  if( eNewMode == gfl2::Fade::MODE_ANIME )
  {
    // 演出
    m_eMode = eNewMode;
    m_pG2DUtil->SetPaneVisible( 0, m_nOutPane, true );
    m_pG2DUtil->StartAnime( 0, m_nOutAnim, true, false );
  }
  else if( eNewMode == gfl2::Fade::MODE_CROSS )
  {
    m_eMode = eNewMode;
    m_ColorNow.Set( 0.0f, 0.0f, 0.0f, 0.0f );
    m_ColorEnd.Set( 0.0f, 0.0f, 0.0f, 255.0f );
    m_ColorAdd.SetW( ( m_ColorEnd.GetW() - m_ColorNow.GetW() ) / static_cast<f32>( sync ) );
    m_pG2DUtil->SetPaneAlpha( 0, m_nOutPane, static_cast<u8>( m_ColorNow.GetW() ) );
    m_pG2DUtil->SetPaneVisible( 0, m_nOutPane, true );
  }
  else
  {
    gfl2::math::Vector4 now_col( m_ColorNow );
    if( start_col )
    {
      now_col.Set( *start_col );
    }
    m_eMode = gfl2::Fade::MODE_ALPHA;
    m_ColorNow.Set( now_col );
    m_ColorEnd.Set( *end_col );
    m_ColorAdd.SetX( ( end_col->GetX() - now_col.GetX() ) / static_cast<f32>( sync ) );
    m_ColorAdd.SetY( ( end_col->GetY() - now_col.GetY() ) / static_cast<f32>( sync ) );
    m_ColorAdd.SetZ( ( end_col->GetZ() - now_col.GetZ() ) / static_cast<f32>( sync ) );
    m_ColorAdd.SetW( ( end_col->GetW() - now_col.GetW() ) / static_cast<f32>( sync ) );
    // 色設定
#if !TEST_SS_FADE
    nw::ut::Color8 stn( static_cast<u8>(m_ColorNow.GetX()), static_cast<u8>(m_ColorNow.GetY()), static_cast<u8>(m_ColorNow.GetZ()), static_cast<u8>(m_ColorNow.GetW()) );
    m_pG2DUtil->SetVtxColorElementOfPicturePane( 0, m_nOutPane, stn );
#endif
    m_pG2DUtil->SetPaneVisible( 0, m_nOutPane, true );
  }
}

/**
 *	@brief  フェードイン リクエスト
 *  @param  type      演出の種類
 *  @param  sync      フェードにかけるSYNC
 */
void LayoutFade::RequestIn( gfl2::Fade::FADE_TYPE type, u32 sync )
{
  // 安全のためOutを消す
  if( m_nOutPane != -1 )
  {
    m_pG2DUtil->SetPaneVisible( 0, m_nOutPane, false );
  }
  if( m_nOutAnim != -1 )
  {
    m_pG2DUtil->StopAnime( 0, m_nOutAnim, true );
  }

  if( type != gfl2::Fade::FADE_TYPE_CROSS )
  {
    m_pG2DUtil->SetPaneVisible( 0, PANENAME_FADE_BG_UPPER_PANE_CROSS_FADE, false );
  }

  // 再生速度を戻す
  m_pG2DUtil->SetAnimeSpeed( 0, 1.0f );
  m_eState = STATE_IN;
  m_FadeResult = gfl2::Fade::FADE_RESULT_IN;
  m_nFadeLifeNow = 0;
  m_nFadeLifeMax = sync;
  sync = gfl2::math::Max<u32>( sync, 1 ); // @fix GFNMCat[1411] 0div対応

  // Inに必要なパラメータを取得する
  u32 eNewMode = AppLib::Fade::Util::GetFadeParamForFadeIn( type, m_nInAnim, m_nInPane );
  if( eNewMode == gfl2::Fade::MODE_ANIME )
  {
    m_eMode = gfl2::Fade::MODE_ANIME;
    // 演出
    m_pG2DUtil->SetPaneVisible( 0, m_nInPane, true );
    // １Frame目からの頭出しで対応
    m_pG2DUtil->StartAnimeFrame( 0, m_nInAnim, 1.0f, true, false );
  }
  else if( eNewMode == gfl2::Fade::MODE_CROSS )
  {
    m_eMode = eNewMode;
    m_ColorNow.Set( 0.0f, 0.0f, 0.0f, 255.0f );
    m_ColorEnd.Set( 0.0f, 0.0f, 0.0f, 0.0f );
    m_ColorAdd.SetW( ( m_ColorEnd.GetW() - m_ColorNow.GetW() ) / static_cast<f32>( sync ) );
    m_pG2DUtil->SetPaneAlpha( 0, m_nInPane, static_cast<u8>( m_ColorNow.GetW() ) );
    m_pG2DUtil->SetPaneVisible( 0, m_nInPane, true );
  }
  else
  {
    if(m_eMode == gfl2::Fade::MODE_ANIME )
    {
      m_ColorNow.Set( 0.0f, 0.0f, 0.0f, 255.0f );
    }
    m_eMode = gfl2::Fade::MODE_ALPHA;
    m_ColorEnd.Set( m_ColorNow );
    m_ColorEnd.SetW( 0.0f );
    m_ColorAdd.SetX( 0.0f );
    m_ColorAdd.SetY( 0.0f );
    m_ColorAdd.SetZ( 0.0f );
    m_ColorAdd.SetW( ( 0.0f - m_ColorNow.GetW() ) / static_cast<f32>( sync ) );
    // 色設定
#if !TEST_SS_FADE
    nw::ut::Color8 stn( static_cast<u8>(m_ColorNow.GetX()), static_cast<u8>(m_ColorNow.GetY()), static_cast<u8>(m_ColorNow.GetZ()), static_cast<u8>(m_ColorNow.GetW()) );
    m_pG2DUtil->SetVtxColorElementOfPicturePane( 0, m_nInPane, stn );
#endif
    m_pG2DUtil->SetPaneVisible( 0, m_nInPane, true );
  }
}

/**
 *  @brief  強制的にフェードアウト状態にする
 *  @param  DISP disp  対象面
 *  @param  end_col    RGBA (0-1.0f)
 */
void LayoutFade::ForceOut(const gfl2::math::Vector4* col)
{
  // アルファフェードにて強制
  RequestOut( gfl2::Fade::FADE_TYPE_ALPHA, col, col, 0, false );
}

/**
 *	@brief  画面遷移 終了検知
 *	@retval TRUE：終了している / FALSE：終了していない
 */
bool LayoutFade::IsEnd(void)
{
  bool bResult = true;

  if( m_eMode != gfl2::Fade::MODE_ANIME )              { bResult = ( m_nFadeLifeNow >= m_nFadeLifeMax ); }
  else if( m_eState == STATE_OUT && m_nOutAnim != -1 ) { bResult = m_pG2DUtil->IsAnimeEnd( 0, m_nOutAnim ); }
  else if( m_eState == STATE_IN  && m_nInAnim  != -1 ) { bResult = m_pG2DUtil->IsAnimeEnd( 0, m_nInAnim ); }

  return bResult;
}

/**
 *  @brief  画面遷移ステータスが終了しているかどうか
 *  @retval TRUE：終了している / FALSE：終了していない
 */
bool LayoutFade::IsEndStatus(void)
{
  // 実行中のステート
  switch( m_eState )
  {
  case STATE_IN:        return false;
  case STATE_OUT:       return false;
  }

  if( IsHomeNixRunning() )
    return false;

  return true;
}

/**
 *	@brief  ポーズON/OFF
 *	@param	is_pause ポーズフラグ
 */
void LayoutFade::SetPause(bool is_pause)
{
  m_bPause = is_pause;
}

/**
 *	@brief  ポーズ状態取得
 *	@retval ポーズ状態
 */
bool LayoutFade::IsPause(void) const
{
  return m_bPause;
}
/**
 *	@brief  フェード状態リセット
 *	@param	DISP disp  対象面
 */
void LayoutFade::Reset(void)
{
  if( m_eMode != gfl2::Fade::MODE_ANIME )
  {
    m_nFadeLifeMax = 0;
    m_nFadeLifeNow = 0;
  }
  if( m_eState == STATE_IN )
  {
    m_pG2DUtil->SetPaneVisible( 0, m_nInPane, false );
    m_pG2DUtil->StopAnime( 0, m_nInAnim, true );
    m_nInAnim = -1;
    m_nInPane = -1;
  }
  if( m_eState == STATE_OUT )
  {
    m_pG2DUtil->SetPaneVisible( 0, m_nOutPane, false );
    m_pG2DUtil->StopAnime( 0, m_nOutAnim, true );
    m_nOutAnim = -1;
    m_nOutPane = -1;
  }
  
  m_FadeResult = gfl2::Fade::FADE_RESULT_IN;
  m_eState = STATE_WAIT;
}

/**
 *  @brief  フェード再生速度をコントロールする
 *
 *  @note   レイアウトアニメーション用に実装しているので
 *          FADE_TYPE_ALPHA,FADE_TYPE_CROSSには非対応
 */
void LayoutFade::SetAnimeSpeed( f32 speed )
{
  if( m_pG2DUtil )
  {
    m_pG2DUtil->SetAnimeSpeed( 0, speed );
  }
}

/**
*	@brief  フェード完了後の状態を取得
*/
gfl2::Fade::FADE_RESULT LayoutFade::GetFadeResult(void) const
{
  return m_FadeResult;
}

/**
*	@brief  フェードのカラーを取得
* @note   一度もフェードアウトリクエストが実行されていない場合は無効
*/
void LayoutFade::GetColor( gfl2::math::Vector4* start_col, gfl2::math::Vector4* end_col ) const
{
  start_col->Set( m_ColorNow );
  end_col->Set( m_ColorEnd );
}

/**
 *  @brief  リソースをクローンしてレイアウト作成する
 *  @param  pParent   リソースのクローン元
 */
void LayoutFade::CloneLayout( LayoutFade *pParent )
{
  // 設定
  m_pAppHeap            = pParent->m_pAppHeap;
  m_pRenderManager      = pParent->m_pRenderManager;
  m_pAsyncFileManager   = pParent->m_pAsyncFileManager;
  m_pLayoutData         = pParent->m_pLayoutData;

  // レイアウト作成
  createLayout( true );
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Private Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  ブロックロード
 */
void LayoutFade::fileLoadBlock( void )
{
  // Open
  {
    // リクエスト設定
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openr;
    openr.arcId         = ARCID_FADE;
    openr.heapForFile   = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
    openr.heapForReq    = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
    // リクエスト
    m_pAsyncFileManager->AddArcFileOpenReq( openr );
    for ever
    {
      // Openをまつ
      if( m_pAsyncFileManager->IsArcFileOpenFinished( ARCID_FADE ) )
      {
        break;
      }
      gfl2::thread::ThreadStatic::CurrentSleep( 2 );
    }
  }
  // Load
  {
    // リクエスト設定
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadr;
    loadr.arcId               = ARCID_FADE;
    loadr.datId               = GARC_fade_fade_applyt_COMP;
    loadr.ppBuf               = &m_pLayoutData;
    loadr.heapForBuf          = m_pAppHeap->GetDeviceHeap();
    loadr.heapForReq          = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
    loadr.heapForCompressed   = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
    loadr.align               = 128;
    // リクエスト
    m_pAsyncFileManager->AddArcFileLoadDataReq( loadr );
    for ever
    {
      // Loadまち
      if( m_pAsyncFileManager->IsArcFileLoadDataFinished( &m_pLayoutData ) )
      {
        break;
      }
      gfl2::thread::ThreadStatic::CurrentSleep( 1 );
    }
  }
  // Close
  {
    // リクエスト設定
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closr;
    closr.arcId       = ARCID_FADE;
    closr.heapForReq  = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
    // リクエスト
    m_pAsyncFileManager->AddArcFileCloseReq( closr );
    for ever
    {
      if( m_pAsyncFileManager->IsArcFileCloseFinished( ARCID_FADE ) )
      {
        break;
      }
      gfl2::thread::ThreadStatic::CurrentSleep( 1 );
    }
  }
}

/**
 *  @brief  レイアウト作成
 *  @param  bClone  クローンか＝下画面か
 */
void LayoutFade::createLayout( bool bClone )
{
  // 初期化完了
  m_eState = STATE_WAIT;
  m_FadeResult = gfl2::Fade::FADE_RESULT_IN;
  m_bClone = bClone;
  // リソースデータ
  app::util::G2DUtil::SETUP_DISPLAY_TYPE type;
  if( bClone )  { type = app::util::G2DUtil::SETUP_LOWER; }
  else          { type = app::util::G2DUtil::SETUP_UPPER; }
  app::util::G2DUtil::LytResData resTbl[] =
  {
    { m_pLayoutData, 2, app::util::G2DUtil::ATTACH_ROOT },
  };
  app::util::G2DUtil::LytwkData setupData[] =
  {
    {
      0,
      LYTRES_FADE_FADE_BG_UPPER_BFLYT,
      LA_FADE_FADE_BG_UPPER___NUM,
      m_pLayoutData,
      false,  // @fix 増田さん要望:フィールド復帰の軽減　非表示ペインへの計算をカット
      type,
      true
    },
  };

  // 生成する
  m_pG2DUtil = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) app::util::G2DUtil( m_pAppHeap );
  m_pG2DUtil->CreateAppLytAccessor(m_pAppHeap);
  m_pG2DUtil->CreateLayoutSystem( m_pAppHeap );
  m_pG2DUtil->CreateLayoutEnv( m_pAppHeap, false );
  m_pG2DUtil->CreateLayoutWork( resTbl, GFL_NELEMS(resTbl), 1, m_pAppHeap );
  m_pG2DUtil->AttachLayoutResourceReference( resTbl, GFL_NELEMS(resTbl) );
  b32 createPaneTbl = app::util::G2DUtil::DEFAULT_CREATE_PANETBL_USE;
  b32 createPaneTblWithHash = app::util::G2DUtil::DEFAULT_CREATE_PANETBL_HASH_USE;
  m_pG2DUtil->SetLayoutWork( 0, setupData, GFL_NELEMS(setupData), m_pAppHeap->GetSystemAllocator(), m_pAppHeap->GetDeviceAllocator(), createPaneTbl, createPaneTblWithHash );

  // PANE_02が消えていない
  m_pG2DUtil->SetPaneVisible( 0, PANENAME_FADE_BG_UPPER_PANE_FADE_ANIME_02, false );
}

/**
 *  @brief  更新処理：レイアウトアニメーション
 */
bool LayoutFade::updateLayoutMode( void )
{
  // Animationではない
  if( m_eState == STATE_IN && m_nInAnim == -1 ){ return true; }
  if( m_eState == STATE_OUT && m_nOutAnim == -1 ){ return true; }

  if( m_eState == STATE_IN )
  {
    // アニメーション終了に伴い終了する
    return !m_pG2DUtil->IsAnimeEnd( 0, m_nInAnim );
  }

  // 更新中
  return true;
}

/**
 *  @brief  更新処理：カラーアニメーション
 */
bool LayoutFade::updateColorMode( void )
{
  bool bContinue = true;;

  if( m_nFadeLifeNow < m_nFadeLifeMax )
  {
    // フェード中
    m_ColorNow += m_ColorAdd;
    ++ m_nFadeLifeNow;
  }
  else
  {
    // フェード終了
    m_nFadeLifeMax = 0;
    m_nFadeLifeNow = 0;
    m_ColorNow.Set( m_ColorEnd );
    if( m_eState == STATE_IN ){ bContinue = false; }
  }
  // 色設定
#if !TEST_SS_FADE
  nw::ut::Color8 stn( static_cast<u8>(m_ColorNow.GetX()), static_cast<u8>(m_ColorNow.GetY()), static_cast<u8>(m_ColorNow.GetZ()), static_cast<u8>(m_ColorNow.GetW()) );
  if( m_eState == STATE_OUT )
  {
    m_pG2DUtil->SetVtxColorElementOfPicturePane( 0, m_nOutPane, stn );
  }
  else if( m_eState == STATE_IN )
  {
    m_pG2DUtil->SetVtxColorElementOfPicturePane( 0, m_nInPane, stn );
  }
#endif

  return bContinue;
}

/**
 *  @brief  更新処理：クロスフェード
 */
bool LayoutFade::updateCrossMode( void )
{
  bool bContinue = true;;

  if( m_nFadeLifeNow < m_nFadeLifeMax )
  {
    // フェード中
    m_ColorNow += m_ColorAdd;
    ++ m_nFadeLifeNow;
  }
  else
  {
    // フェード終了
    m_nFadeLifeMax = 0;
    m_nFadeLifeNow = 0;
    m_ColorNow.Set( m_ColorEnd );
    if( m_eState == STATE_IN ){ bContinue = false; }
  }
  u8 alpha = static_cast<u8>(m_ColorNow.GetW());
  if( m_eState == STATE_OUT )
  {
    m_pG2DUtil->SetPaneAlpha( 0, m_nOutPane, alpha );
  }
  else if( m_eState == STATE_IN )
  {
    m_pG2DUtil->SetPaneAlpha( 0, m_nInPane, alpha );
  }

  return bContinue;
}

/**
 *  @brief  画面スナップショットをマウントする
 */
void LayoutFade::mountSnapshot( void )
{
  u32 target = gfl2::Fade::FadeManager::COLOR_BUFFER_UPPER;
  if( m_bClone ){ target = gfl2::Fade::FadeManager::COLOR_BUFFER_LOWER; }


  // スナップショット申請
  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestTransferColorBuffer( target );
  // テクスチャ取得
  gfl2::gfx::Texture *pTexture = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->GetColorBufferTexture( target );
  // テクスチャ差し替え
  gfl2::lyt::LytResTextureInfo textureInfo;
  app::util::G2DUtil::MakeTextureInfo( &textureInfo, pTexture );
  gfl2::lyt::LytPicture *pPicture = m_pG2DUtil->GetLayoutWork( 0 )->GetPicturePane( PANENAME_FADE_BG_UPPER_PANE_CROSS_TEXTURE );

  gfl2::math::Vector2 uvPos, uvRect;
#if defined(GF_PLATFORM_WIN32)
  if( !m_bClone )
  {
    uvPos.x = 0.0f;
    uvPos.y = 240.0f;
    uvRect.x = 400.0f;
    uvRect.y = -240.0f;
  }
  else
  {
    uvPos.x = 0.0f;
    uvPos.y = 240.0f;
    uvRect.x = 320.0f;
    uvRect.y = -240.0f;
  }
#elif defined(GF_PLATFORM_CTR)
  if( !m_bClone )
  {
    uvPos.x = 112.0f;
    uvPos.y = 16.0f;
    uvRect.x = 400.0f;
    uvRect.y = 240.0f;
  }
  else
  {
    uvPos.x = 152.0f;
    uvPos.y = 16.0f;
    uvRect.x = 400.0f;
    uvRect.y = 240.0f;
  }
#endif
  app::tool::LytTexReplaceSystem::ReplaceTextureStatic( &textureInfo, pPicture, uvPos, uvRect, 0, 0 );
}

/**
 *  @brief  HomeNix更新
 */
bool LayoutFade::UpdateHomeNix( void )
{
#ifdef GF_PLATFORM_CTR

  if( m_isHomeNixRequest )
  {
    if( m_homeNixFade == 0 )
    {//初期化
      m_homeNixFade = HOME_NIX_FADE_COUNT - 1;
      m_pG2DUtil->SetPaneVisible( 0 , PANENAME_FADE_BG_UPPER_PANE_HOME_NIX , true );

      m_homeNixStartTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
    }

    u8 alpha = 255;
    if( m_homeNixFade == 1 )
    {//終了
      alpha = 0;
    }
    else if( HOME_NIX_FADE_COUNT - m_homeNixFade <= HOME_NIX_FADE_IN )
    {//イン
      alpha = ( 255 / HOME_NIX_FADE_IN ) * ( HOME_NIX_FADE_COUNT - m_homeNixFade );
    }
    else if( m_homeNixFade < HOME_NIX_FADE_OUT )
    {//アウト
      alpha = ( 255 / HOME_NIX_FADE_OUT ) * m_homeNixFade;
    }

    m_pG2DUtil->SetPaneAlpha( 0 , PANENAME_FADE_BG_UPPER_PANE_HOME_NIX , alpha );

    m_homeNixFade = (HOME_NIX_FADE_COUNT-1) - ( nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds() - m_homeNixStartTime ) / 33;
    if( m_homeNixFade < 1 || m_homeNixFade > HOME_NIX_FADE_COUNT - 1 )
    {
      m_homeNixFade = 1;
    }

    GFL_PRINT("HOMENIXFADE[%d]\n",m_homeNixFade);

    if( alpha == 0 )
    {//終了
      m_pG2DUtil->SetPaneVisible( 0 , PANENAME_FADE_BG_UPPER_PANE_HOME_NIX , false );
      m_isHomeNixRequest = false;
      m_homeNixFade = 0;
      
      GFL_PRINT("HOMENIXFADE END[%llu]\n",nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds() - m_homeNixStartTime);
    }

    return true;
  }

#endif

  return false;
}


/**
  *  @brief  HomeNixリクエスト
  */
void LayoutFade::RequestHomeNix( void )
{
  m_isHomeNixRequest = true;
}

/**
  *  @brief  HomeNix確認
  */
bool LayoutFade::IsHomeNixRunning( void )
{
  return m_isHomeNixRequest;
}

GFL_NAMESPACE_END(Fade)
GFL_NAMESPACE_END(AppLib)
