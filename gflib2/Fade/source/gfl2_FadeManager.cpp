//=============================================================================
/**
 *
 *	@file		gfl2_FadeManager.cpp
 *	@brief  フェードシステム  マネージャー
 *	@author k.ohno
 *	@data		2015.04.17
 *
 */
//=============================================================================

#include <Fade/include/gfl2_FadeTypes.h>
#include <Fade/include/gfl2_FadeManager.h>
#include <Fade/include/gfl2_FadeSuper.h>
#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <renderer/include/gfl2_FrameBufferClearAlphaPath.h>

//-----------------------------------------------------------------------------
/**
 *    Singletonインスタンスの実体を定義
 */
//-----------------------------------------------------------------------------
//#if GFL_ENABLE_RO
SINGLETON_INSTANCE_DEFINITION(gfl2::Fade::FadeManager);
template class gfl2::base::SingletonAccessor<gfl2::Fade::FadeManager>;
//#endif

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( Fade )



/**
 *  @brief  クロスフェード用
 */
class CrossFadeRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
public:
  /**
   *  @brief  コンストラクタ
   */
  CrossFadeRenderingPipeLine( gfl2::heap::HeapBase *pHeap )
    : m_RenderPathCnt( 0 )
  {
    m_pFrameBufferClearAlphaPath = GFL_NEW( pHeap ) poke_3d::renderer::FrameBufferClearAlphaPath();
  }
  /**
   *  @brief  デストラクタ
   */
  virtual ~CrossFadeRenderingPipeLine( void )
  {
    GFL_SAFE_DELETE( m_pFrameBufferClearAlphaPath );
  }
  /**
   *  @brief  最初のパスから描画できる状態する
   */
  virtual b32 StartRenderPath( void )
  {
    m_RenderPathCnt = 0;
    return true;
  }
  /**
   *  @brief  現状のパスにあったシーンを返す
   */
  virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath *GetRenderPath( void )
  {
    switch( m_RenderPathCnt )
    {
    case 0:
      return m_pFrameBufferClearAlphaPath;
    }
    return NULL;
  }
  /**
   *  @brief  次のパスへ遷移
   */
  virtual b32 NextRenderPath( void )
  {
    ++ m_RenderPathCnt;
    if( m_RenderPathCnt == 1 )
    {
      return false;
    }
    return true;
  }
  /**
   *  @brief  パスの取得
   */
  poke_3d::renderer::FrameBufferClearAlphaPath *GetFrameBufferClearAlPhaPath( void ){ return m_pFrameBufferClearAlphaPath; }

private:
  u32 m_RenderPathCnt;
  poke_3d::renderer::FrameBufferClearAlphaPath *m_pFrameBufferClearAlphaPath;
};

/**
 * @brief フェードクラス
 */


/**
 *	@brief  フェードマネージャー
 *	@param  heap   通常ヒープ 管理に使用します
 */
FadeManager::FadeManager(void)
{
  for(int i=0;i<DISP_BUFF_MAX;i++){
    m_pFade[i]=NULL;
  }
  for( u32 i = 0; i < COLOR_BUFFER_MAXI; ++ i )
  {
    m_pColorBuffer[i] = NULL;
    m_bTransColorBuffer[ i ] = false;
  }
  m_pPipeLine = NULL;
}

/**
 *	@brief  デストラクタ
 */
FadeManager::~FadeManager(void)
{
  DeleteColorBuffer();
  GFL_SAFE_DELETE( m_pPipeLine );
}

/**
 *  @brief  PipeLine生成
 */
void FadeManager::createPipeLine( gfl2::heap::HeapBase *pHeap, gfl2::gfx::IGLAllocator *pAllocator, void *gfbmdlBinary, void *vertexShaderBinary, void *fragmentShaderBinary )
{
  CrossFadeRenderingPipeLine *pPipeLine = GFL_NEW( pHeap ) CrossFadeRenderingPipeLine( pHeap );
  pPipeLine->GetFrameBufferClearAlPhaPath()->Initialize(
    pAllocator,
    reinterpret_cast<c8*>( gfbmdlBinary ),
    reinterpret_cast<c8*>( vertexShaderBinary ),
    reinterpret_cast<c8*>( fragmentShaderBinary )
    );

  m_pPipeLine = pPipeLine;
}

/**
 *	@brief  主処理
 */
void FadeManager::Update(void)
{
  for( u32 i = 0; i < DISP_BUFF_MAX; ++ i )
  {
    if( !m_pFade[ i ] ){ continue; }
    m_pFade[ i ]->UpdateFunc();
  }
}

/**
 *	@brief  描画処理(下準備)
 */
void FadeManager::PreDraw(void)
{
  for( u32 i = 0; i < DISP_BUFF_MAX; ++ i )
  {
    if( !m_pFade[ i ] ){ continue; }
    if( m_pFade[i]->IsEndStatus() ){ continue; }
    m_pFade[ i ]->PreDrawFunc();
  }
}

/**
 *	@brief  描画処理
 */
void FadeManager::Draw(gfl2::gfx::CtrDisplayNo no)
{
  TransferColorBuffer(no);

  if(m_pFade[DISP_UPPER] && (no == gfl2::gfx::CtrDisplayNo::LEFT)){
    if(!m_pFade[DISP_UPPER]->IsEndStatus()){
      m_pFade[DISP_UPPER]->DrawFunc( no );
    }
  }
  if(m_pFade[DISP_UPPER] && (no == gfl2::gfx::CtrDisplayNo::RIGHT)){
    if(!m_pFade[DISP_UPPER]->IsEndStatus()){
      m_pFade[DISP_UPPER]->DrawFunc( no );
    }
  }
  if(m_pFade[DISP_LOWER] && (no == gfl2::gfx::CtrDisplayNo::DOWN)){
    if(!m_pFade[DISP_LOWER]->IsEndStatus()){
      m_pFade[DISP_LOWER]->DrawFunc( no );
    }
  }
  if(m_pFade[DISP_CUSTOM_UPPER] && (no == gfl2::gfx::CtrDisplayNo::LEFT)){
    if(!m_pFade[DISP_CUSTOM_UPPER]->IsEndStatus()){
      m_pFade[DISP_CUSTOM_UPPER]->DrawFunc( no );
    }
  }
  if(m_pFade[DISP_CUSTOM_UPPER] && (no == gfl2::gfx::CtrDisplayNo::RIGHT)){
    if(!m_pFade[DISP_CUSTOM_UPPER]->IsEndStatus()){
      m_pFade[DISP_CUSTOM_UPPER]->DrawFunc( no );
    }
  }
  if(m_pFade[DISP_CUSTOM_LOWER] && (no == gfl2::gfx::CtrDisplayNo::DOWN)){
    if(!m_pFade[DISP_CUSTOM_LOWER]->IsEndStatus()){
      m_pFade[DISP_CUSTOM_LOWER]->DrawFunc( no );
    }
  }
}


//活動しているかどうか、してない場合は true
bool FadeManager::IsEndStatus(gfl2::Fade::DISP disp)
{
  bool bEndSt=false;

  if(disp == DISP_DOUBLE)
  {
    for( u32 i = 0; i < DISP_BUFF_MAX; ++ i )
    {
      if(m_pFade[i]){
        bEndSt = m_pFade[i]->IsEndStatus();
        if(!bEndSt){
          return false;
        }
      }
    }
  }
  else
  {
    if(m_pFade[disp])
    {
      return m_pFade[disp]->IsEndStatus();
    }
  }
  return true;  //fadeは活動してない
}


/**
 *	@brief  画面遷移 終了検知 対称面指定版
 *	@param	DISP disp  対象面
 *	@retval TRUE：終了している 登録が無い / FALSE：終了していない
 */
bool FadeManager::IsEnd(gfl2::Fade::DISP disp)
{
  bool resultValue = true;

  if( disp == DISP_DOUBLE )
  {
    if( m_pFade[DISP_UPPER] && m_pFade[DISP_UPPER]->IsEnd() == false )
    {
      resultValue = false;
    }

    if( m_pFade[DISP_LOWER] && m_pFade[DISP_LOWER]->IsEnd() == false )
    {
      resultValue = false;
    }
  }
  else
  {
    GFL_ASSERT(disp < DISP_BUFF_MAX);
    if( m_pFade[disp] )
    {
      resultValue = m_pFade[disp]->IsEnd();
    }
  }

  return resultValue;
}





/**
 *	@brief  ポーズON/OFF
 *	@param	is_pause ポーズフラグ
 */
void FadeManager::SetPause(gfl2::Fade::DISP disp, bool is_pause)
{
  if( disp == DISP_DOUBLE )
  {
    if( m_pFade[DISP_UPPER] )
    {
      m_pFade[DISP_UPPER]->SetPause( is_pause );
    }

    if( m_pFade[DISP_LOWER] )
    {
      m_pFade[DISP_LOWER]->SetPause( is_pause );
    }
  }
  else
  {
    GFL_ASSERT(disp < DISP_BUFF_MAX);
    if( m_pFade[disp] )
    {
      m_pFade[disp]->SetPause( is_pause );
    }
  }
}


/**
 *	@brief  ポーズ状態取得
 *	@retval ポーズ状態
 */
bool FadeManager::IsPause(gfl2::Fade::DISP disp) const
{
  bool resultValue = true;

  if( disp == DISP_DOUBLE )
  {
    if( m_pFade[DISP_UPPER] && m_pFade[DISP_UPPER]->IsPause() == false )
    {
      resultValue = false;
    }

    if( m_pFade[DISP_LOWER] && m_pFade[DISP_LOWER]->IsPause() == false )
    {
      resultValue = false;
    }
  }
  else
  {
    GFL_ASSERT(disp < DISP_BUFF_MAX);
    if( m_pFade[disp] )
    {
      resultValue = m_pFade[disp]->IsPause();
    }
  }

  return resultValue;

}

/**
 *	@brief  フェード状態リセット
 */
void FadeManager::Reset(gfl2::Fade::DISP disp)
{
  if( disp == DISP_DOUBLE )
  {
    if( m_pFade[DISP_UPPER] )
    {
      m_pFade[DISP_UPPER]->Reset();
    }
    if( m_pFade[DISP_LOWER] )
    {
      m_pFade[DISP_LOWER]->Reset();
    }
  }
  else
  {
    GFL_ASSERT(disp < DISP_BUFF_MAX);
    if( m_pFade[disp] )
    {
      m_pFade[disp]->Reset();
    }
  }
}

/**
 *  @brief  フェード再生速度をコントロールする
 */
void FadeManager::SetAnimeSpeed(gfl2::Fade::DISP disp, f32 speed )
{
  if( disp == DISP_DOUBLE )
  {
    if( m_pFade[DISP_UPPER] )
    {
      m_pFade[DISP_UPPER]->SetAnimeSpeed(speed);
    }
    if( m_pFade[DISP_LOWER] )
    {
      m_pFade[DISP_LOWER]->SetAnimeSpeed(speed);
    }
  }
  else
  {
    GFL_ASSERT(disp < DISP_BUFF_MAX);
    if( m_pFade[disp] )
    {
      m_pFade[disp]->SetAnimeSpeed(speed);
    }
  }
}

/**
 *	@brief  フェード登録
 */
void FadeManager::AddFade(gfl2::Fade::DISP disp,FadeSuper* pFade)
{
  GFL_ASSERT(disp < DISP_BUFF_MAX);
  m_pFade[ disp ] = pFade;
}

  /**
   *	@brief  フェード登録解除（クラス解放は行いません）
   *	@param	DISP disp  対象面
   */
void FadeManager::RemoveFade(gfl2::Fade::DISP disp)
{
  GFL_ASSERT(disp < DISP_BUFF_MAX);
  m_pFade[disp] = NULL;
}

/**
 *  @brief  フェード取得
 *  @param  DISP disp  対象面
 */
FadeSuper *FadeManager::GetFade(gfl2::Fade::DISP disp)
{
  GFL_ASSERT(disp < DISP_BUFF_MAX);
  return m_pFade[disp];
}

/**
 *  @brief  フェードアウト リクエスト
 *  @param  disp        対象
 *  @param  type        種類
 *  @param  start_col   開始色
 *  @param  end_col     終了色
 *  @param  sync        時間
 *  @param  capture     開始時スクリーンキャプチャするか
 */
void FadeManager::RequestOut( gfl2::Fade::DISP disp, gfl2::Fade::FADE_TYPE type, const gfl2::math::Vector4 *start_col, const gfl2::math::Vector4 *end_col, u32 sync/* = FADE_DEFAULT_SYNC*/, bool capture/* = false*/ )
{
  if( DISP_DOUBLE == disp )
  {
    m_pFade[DISP_UPPER]->RequestOut( type, start_col, end_col, sync, capture );
    m_pFade[DISP_LOWER]->RequestOut( type, start_col, end_col, sync, capture );
  }
  else
  {
    GFL_ASSERT(disp < DISP_BUFF_MAX);
    m_pFade[disp]->RequestOut( type, start_col, end_col, sync, capture );
  }
}

/**
 *  @brief  フェードイン リクエスト
 *  @param  disp  対象
 *  @param  type  種類
 *  @param  sync        時間
 */
void FadeManager::RequestIn( gfl2::Fade::DISP disp, gfl2::Fade::FADE_TYPE type, u32 sync/* = FADE_DEFAULT_SYNC*/ )
{
  if( DISP_DOUBLE == disp )
  {
    m_pFade[DISP_UPPER]->RequestIn( type, sync );
    m_pFade[DISP_LOWER]->RequestIn( type, sync );
  }
  else
  {
    GFL_ASSERT(disp < DISP_BUFF_MAX);
    m_pFade[disp]->RequestIn( type, sync );
  }
}

  /**
   *	@brief  強制的にフェードアウト状態にする
   *	@param	DISP disp  対象面
   *	@param	end_col    RGBA (0-1.0f)
   */
void FadeManager::ForceOut(gfl2::Fade::DISP disp, const gfl2::math::Vector4* col)
{
  if(DISP_DOUBLE == disp)
  {
    m_pFade[DISP_UPPER]->ForceOut( col );
    m_pFade[DISP_LOWER]->ForceOut( col );
  }
  else
  {
    GFL_ASSERT(disp < DISP_BUFF_MAX);
    m_pFade[disp]->ForceOut( col );
  }
}

/**
  * @brief  強制的にフェードアウト状態にする(対象面の色が黒固定)
  * @param  DISP disp  対象面
  */
void FadeManager::ForceDefaultOut(gfl2::Fade::DISP disp)
{
  static const gfl2::math::Vector4 cou(0.0f,0.0f,0.0f,255.0f);
  ForceOut( disp, &cou );
}


/**
 *	@brief  デフォルトフェードアウト リクエスト  色は黒、フレーム固定
 *	@param	DISP disp  対象面
 */
void FadeManager::RequestDefaultOut(gfl2::Fade::DISP disp)
{
  static const gfl2::math::Vector4 scou(0.0f,0.0f,0.0f,0.0f);
  static const gfl2::math::Vector4 ecou(0.0f,0.0f,0.0f,255.0f);
  RequestOut( disp, gfl2::Fade::FADE_TYPE_ALPHA, &scou, &ecou);
}

/**
 *  @brief  カラーバッファの作成
 */
void FadeManager::CreateColorBuffer( gfl2::gfx::IGLAllocator *pAllocator, gfl2::gfx::Pool poolUpper, gfl2::gfx::Pool poolLower )
{
#if defined(GF_PLATFORM_WIN32)
  static const f32 width = 512;
  static const f32 height = 256;
#elif defined(GF_PLATFORM_CTR)
  static const f32 width = 256;
  static const f32 height = 512;
#endif

  if( !m_pColorBuffer[COLOR_BUFFER_UPPER] )
  {
    m_pColorBuffer[COLOR_BUFFER_UPPER] = gfl2::gfx::GFGL::CreateTexture(
      pAllocator,
      width, height, 1,
      gfl2::gfx::Usage::RenderTarget,
      gfl2::gfx::Format::A8R8G8B8,
      poolUpper
    );
  }
  if( !m_pColorBuffer[COLOR_BUFFER_LOWER] )
  {
    m_pColorBuffer[COLOR_BUFFER_LOWER] = gfl2::gfx::GFGL::CreateTexture(
      pAllocator,
      width, height, 1,
      gfl2::gfx::Usage::RenderTarget,
      gfl2::gfx::Format::A8R8G8B8,
      poolLower
    );
  }
}

/**
 *  @brief  カラーバッファの破棄
 */
void FadeManager::DeleteColorBuffer( void )
{
  GFX_SAFE_DELETE( m_pColorBuffer[COLOR_BUFFER_UPPER] );
  GFX_SAFE_DELETE( m_pColorBuffer[COLOR_BUFFER_LOWER] );
}

/**
 *  @brief  カラーバッファ取得 リクエスト
 */
void FadeManager::RequestTransferColorBuffer( u32 target )
{
  GFL_ASSERT( target < COLOR_BUFFER_MAXI );
  if( target < COLOR_BUFFER_MAXI )
  {
    m_bTransColorBuffer[target] = true;
  }
}

/**
 *  @brief  カラーバッファ取得
 */
void FadeManager::TransferColorBuffer( gfl2::gfx::CtrDisplayNo no )
{
  if( m_bTransColorBuffer[COLOR_BUFFER_UPPER] != false && no == gfl2::gfx::CtrDisplayNo::LEFT )
  {
    if( m_pPipeLine )
    {
      m_pPipeLine->Update();
      m_pPipeLine->Execute();
    }
    gfl2::gfx::GFGL::AddReadColorBufferToTextureCommand( m_pColorBuffer[ COLOR_BUFFER_UPPER ] );
    m_bTransColorBuffer[COLOR_BUFFER_UPPER] = false;
  }
  if( m_bTransColorBuffer[COLOR_BUFFER_LOWER] != false && no == gfl2::gfx::CtrDisplayNo::DOWN )
  {
    if( m_pPipeLine )
    {
      m_pPipeLine->Update();
      m_pPipeLine->Execute();
    }
    gfl2::gfx::GFGL::AddReadColorBufferToTextureCommand( m_pColorBuffer[ COLOR_BUFFER_LOWER ] );
    m_bTransColorBuffer[COLOR_BUFFER_LOWER] = false;
  }
}

/**
 *  @brief  テクスチャの取得
 */
gfl2::gfx::Texture *FadeManager::GetColorBufferTexture( u32 target )
{
  return m_pColorBuffer[ target ];
}

/**
  *  @brief  HomeNixリクエスト
  */
void FadeManager::RequestHomeNix( void )
{
  if( m_pFade[DISP_LOWER] )
  {
    m_pFade[DISP_LOWER]->RequestHomeNix();
  }
}

/**
  *  @brief  HomeNix確認
  */
bool FadeManager::IsHomeNixRunning( void )
{
  if( m_pFade[DISP_LOWER] )
  {
    return m_pFade[DISP_LOWER]->IsHomeNixRunning();
  }

  return false;
}

GFL_NAMESPACE_END( Fade )
GFL_NAMESPACE_END( gfl2 )
