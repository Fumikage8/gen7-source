//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl2_EffectRenderpath.cpp
 *	@brief  エフェクトのレンダリングパス
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <Effect/include/gfl2_EffectRenderPath.h>


// CommandBufferJumpHelperのダミーを作成
#if defined(GF_PLATFORM_WIN32)
namespace nn{ namespace gr{
class CommandBufferJumpHelper
{
public:
  void Put( u32* data ){};

  u32* GetCurrentPtr(){return NULL;}
};
}}  //gr nn
#endif
//-------------------------------------


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(Effect)

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

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
/**
 * @brief  コンストラクタ
 */
EffectRenderPath::EffectRenderPath() : 
  m_Config()
{
}

/**
 * @brief  コンストラクタ
 */
EffectRenderPath::EffectRenderPath( const Config& config ) : 
  m_Config()
{
  this->SetConfig(config);

}

/**
 * @brief  デストラクタ
 */
EffectRenderPath::~EffectRenderPath()
{
}
  

/** 
 * @brief コンフィグ情報を更新
 */
void EffectRenderPath::SetConfig( const Config& config )
{
  // 内容は受け渡し用のデータのみ
  // そのままコピーしてOK
  m_Config = config;

  GFL_ASSERT_STOP( config.m_pSystem );
  GFL_ASSERT_STOP( config.m_cpGroupIDList );
}


/**
 *  @brief  描画実行
 */
void EffectRenderPath::Execute(const renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext)
{
  if( m_Config.m_cpGroupIDList == NULL ){ return; } 
  if( m_Config.m_pSystem == NULL ){ return; }

  BeginDraw(rDrawContext);


  // CTR Win32それぞれのjhを取得
	nn::gr::CommandBufferJumpHelper* jh;
  
#if defined(GF_PLATFORM_CTR)
  jh = gfx::GFGL::GetJumpHelper();
#elif defined(GF_PLATFORM_WIN32)
  nn::gr::CommandBufferJumpHelper _jh;
  jh = &_jh;
#endif
  //-------------------------------------

	const Camera	*pCameraOverride = CameraOverride();
  gfl2::math::Matrix34 viewMtx = rDrawContext.m_View;
  gfl2::math::Matrix44 projMtx = rDrawContext.m_Projection;

  if( pCameraOverride )
  {
    viewMtx = pCameraOverride->m_View;
    projMtx = pCameraOverride->m_Projection;
  }

  
  // View関連コマンド生成
  const f32 CLIP_NEAR = 0.10f;
  const f32 CLIP_FAR = 3000.0f;
  math::Vector3 camPos = viewMtx.GetElemPosition();
  gfl2::math::Matrix34 invCamPos = viewMtx.Inverse();
  camPos = invCamPos.GetElemPosition();
  jh->Put( m_Config.m_pSystem->BeginRender( jh->GetCurrentPtr(), projMtx, viewMtx, camPos, CLIP_NEAR, CLIP_FAR ) );
  
  
  // 描画コマンド生成
  util::List<u32>::Iterator iter = m_Config.m_cpGroupIDList->Begin();
  for( ; iter != m_Config.m_cpGroupIDList->End(); ++iter ){
    jh->Put( m_Config.m_pSystem->RenderParticle( jh->GetCurrentPtr(), *iter, false/*, m_Config.m_FrameRate*/ ) );
  }

  m_Config.m_pSystem->EndRender();

  EndDraw(rDrawContext);
}

/**
 *  @brief  描画前コマンド生成
 */
void EffectRenderPath::BeginDraw(const renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext)
{
  // レンダーターゲットの変更
  gfx::GFGL::SetRenderTarget( rDrawContext.m_pRenderTarget );
  gfx::GFGL::SetDepthStencil( rDrawContext.m_pDepthStencil );


	gfx::GFGL::BeginScene();

	if ( rDrawContext.m_DisplayWidth && rDrawContext.m_DisplayHeight )
	{
		gfx::GFGL::SetViewport(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
		gfx::GFGL::SetScissor(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
	}
	else
	{
		const gfx::Surface::SurfaceDesc			&rDesc = gfx::GFGL::GetRenderTarget()->GetSurfaceDesc();
		gfx::GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);
		gfx::GFGL::SetScissor(0, 0, rDesc.width, rDesc.height);
	}

#if defined(GF_PLATFORM_CTR)
	nn::gr::CommandBufferJumpHelper* jh = gfx::GFGL::GetJumpHelper();

  // 無効コマンドを発行
  gfl2::gfx::ctr::CTRGL::AddDisableAllCommand(jh);

  // 
  {
		const gfx::Surface::SurfaceDesc			&rDesc = gfx::GFGL::GetRenderTarget()->GetSurfaceDesc();
	  nn::gr::Viewport viewport;
	  viewport.Set( 0, 0, rDesc.height, rDesc.width );
	  jh->Put( viewport.MakeCommand( jh->GetCurrentPtr() ) );

	  nn::gr::Scissor scissor;
	  scissor.Set( 0, 0, rDesc.height, rDesc.width );
	  jh->Put( scissor.MakeCommand( jh->GetCurrentPtr() ) );
  }
#endif
}

/**
 *  @brief  描画後コマンド生成
 */
void EffectRenderPath::EndDraw(const renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext)
{
	if ( rDrawContext.m_DisplayWidth && rDrawContext.m_DisplayHeight )
	{
		gfx::GFGL::SetViewport(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
		gfx::GFGL::SetScissor(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
	}
	else
	{
		const gfx::Surface::SurfaceDesc			&rDesc = gfx::GFGL::GetRenderTarget()->GetSurfaceDesc();
		gfx::GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);
		gfx::GFGL::SetScissor(0, 0, rDesc.width, rDesc.height);
	}
	gfx::GFGL::EndScene();


#if defined(GF_PLATFORM_CTR)
  /*
  // コマンドをスプリット
  // CTRの場合 CommandListにVRAM転送処理が詰まれる。
  // 積み先のコマンドリストを明示的に指定されていない場合はカレントコマンドが
  // 使用されるので、対策を取る。
	nn::gr::CommandBufferJumpHelper* jh;
  jh = gfl2::gfx::GFGL::GetJumpHelper();
  gfl2::gfx::ctr::CTRCommandList  * p_cmd = gfl2::gfx::ctr::CTRGL::GetCurrentCommandList();
  gfl2::gfx::ctr::CTRGL::SyncCommandListAndJumpHelperBefore(p_cmd, jh);
  p_cmd->Flush3DCommandNoCacheFlush();
  gfl2::gfx::ctr::CTRGL::SyncCommandListAndJumpHelperAfter(p_cmd, jh);
  */
#endif 
}


GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(gfl2)

