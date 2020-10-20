//=============================================================================
/**
 * @file    gfl2_StretchBltFrameBufferPath.cpp
 * @brief   StretchBltFrameBufferPathクラス（フレームバッファを引き伸ばしコピーするパス）
 * @author  ishiguro_masateru
 * @date    2015/09/29
 */
//=============================================================================
#include <RenderingEngine/include/Renderer/Util/gfl2_StretchBltFrameBufferPath.h>

#include <gfx/include/gfl2_GFGLUtil.h>
#include <debug/include/gfl2_Assert.h>
#include <gfx/include/gfl2_DrawUtil.h>

#include <RenderingEngine/include/Renderer/gfl2_MeshDrawTag.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>

#if defined(GF_PLATFORM_CTR)
	#include <nn/fs.h>
	#include <nn/hio.h>
#else
	#include <gfx/include/WinOpenGL/gfl2_WinOpenGLStateObject.h>
#endif

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;


namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
StretchBltFrameBufferPath::StretchBltFrameBufferPath() : gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath(),
  m_pFrameBufferTexture( NULL ),
  m_pFrameBufferCopy( NULL ),
  m_ScaleWidth(0.0f),
  m_ScaleHeight(0.0f)
{

}

StretchBltFrameBufferPath::~StretchBltFrameBufferPath()
{
  GFX_SAFE_DELETE( m_pFrameBufferTexture );
  GFX_SAFE_DELETE( m_pFrameBufferCopy );
}

void StretchBltFrameBufferPath::CreateFrameBufferTexture( gfl2::gfx::IGLAllocator* pAllocator, const gfl2::gfx::Surface *pFrameBuffer )
{
  if ( m_pFrameBufferTexture != NULL || m_pFrameBufferCopy != NULL )
  {
    return;
  }

  gfl2::gfx::Surface::SurfaceDesc   surfaceDesc = pFrameBuffer->GetSurfaceDesc();

  u32     screenWidth = surfaceDesc.width;
  u32     screenHeight = surfaceDesc.height;

#if defined(GF_PLATFORM_WIN32)

  static const u32 FRAMEBUFFER_WIDTH = 512;
  static const u32 FRAMEBUFFER_HEIGHT = 256;

  m_ScaleWidth = static_cast<f32>(surfaceDesc.width) / static_cast<f32>(FRAMEBUFFER_WIDTH);
  m_ScaleHeight = static_cast<f32>(surfaceDesc.height) / static_cast<f32>(FRAMEBUFFER_HEIGHT);

  screenWidth = FRAMEBUFFER_WIDTH;
  screenHeight = FRAMEBUFFER_HEIGHT;

#elif defined(GF_PLATFORM_CTR)
  screenWidth = surfaceDesc.bufferWidth;
  screenHeight = surfaceDesc.bufferHeight;

  m_ScaleWidth = static_cast<f32>(surfaceDesc.height) / static_cast<f32>(surfaceDesc.bufferWidth);
  m_ScaleHeight = static_cast<f32>(surfaceDesc.width) / static_cast<f32>(surfaceDesc.bufferHeight);

  m_pFrameBufferTexture = GFGL::CreateTextureFromSurface( pAllocator, pFrameBuffer );

  {
    gfx::Texture::TextureDesc			textureDesc = m_pFrameBufferTexture->GetTextureDesc();
    textureDesc.usage = Usage::RenderTarget;
    m_pFrameBufferTexture->SetTextureDesc(textureDesc);
  }

#endif

  m_pFrameBufferCopy = GFGL::CreateTexture( pAllocator, screenWidth, screenHeight, 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );

#if defined(GF_PLATFORM_CTR)
  {
    gfx::Texture::TextureDesc     textureDesc = m_pFrameBufferCopy->GetTextureDesc();
    u32     swap = textureDesc.height;
    textureDesc.height = textureDesc.width;
    textureDesc.width = swap;
    m_pFrameBufferCopy->SetTextureDesc(textureDesc);
  }
#endif
}

void StretchBltFrameBufferPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
#if defined(GF_PLATFORM_WIN32)
  GFGL::AddReadColorBufferToTextureCommand( m_pFrameBufferCopy );
#elif defined(GF_PLATFORM_CTR)
  // DrawUtilに設定されている現在のテクスチャとスクリーンのサイズを記録(処理終了後に元に戻す)
  const gfl2::gfx::Texture* originalDrawUtilTexture = gfl2::gfx::DrawUtil::GetTexture();
  u32 originalDrawUtilScreenWidth = gfl2::gfx::DrawUtil::GetScreenWidth();
  u32 originalDrawUtilScreenHeight = gfl2::gfx::DrawUtil::GetScreenHeight();

  u32                         frameBufferWidth, frameBufferHeight;
  const gfx::Surface          *pRenderTarget = m_pFrameBufferCopy->GetSurface();
  gfx::Surface::SurfaceDesc   surfaceDesc = pRenderTarget->GetSurfaceDesc();

  frameBufferWidth = surfaceDesc.height;
  frameBufferHeight = surfaceDesc.width;

  GFGL::SetRenderTarget( 0, pRenderTarget );
  gfl2::gfx::GFGL::SetDepthStencil( NULL );

  gfl2::gfx::GFGL::BeginScene();

  gfl2::gfx::GFGL::SetViewport(0, 0, frameBufferWidth, frameBufferHeight);
  gfl2::gfx::GFGL::SetScissor(0, 0, frameBufferWidth, frameBufferHeight);

  {
    gfl2::gfx::DrawUtil::BeginRender();
    gfl2::gfx::DrawUtil::SetScreenSize(frameBufferWidth, frameBufferHeight);

    gfl2::gfx::DepthStencilStateObject::StateData depthStateData;
    depthStateData.m_DepthTestEnable = false;
    depthStateData.m_DepthWriteEnable = false;
    gfl2::gfx::DrawUtil::SetDepthTestState(depthStateData);

    gfl2::gfx::DrawUtil::SetBlendTestState(
      false,
      gfl2::gfx::BlendOp::Add,
      gfl2::gfx::BlendFunc::One,
      gfl2::gfx::BlendFunc::One,
      gfl2::gfx::BlendFunc::Zero,
      gfl2::gfx::BlendFunc::Zero);
      gfl2::gfx::DrawUtil::SetMaterialAlpha(1.0f);

    gfl2::gfx::DrawUtil::SetTexture(m_pFrameBufferTexture);
    gfl2::gfx::DrawUtil::DrawRect( 0.0f, 0.0f, frameBufferWidth, frameBufferHeight, 0.0f, 0.0f, m_ScaleWidth, m_ScaleHeight );

    gfl2::gfx::DrawUtil::EndRender();
  }

  gfl2::gfx::GFGL::EndScene();

  // 描画が終わったので本来のレンダーターゲットに戻す
  gfl2::gfx::GFGL::SetRenderTarget(rDrawContext.m_pRenderTarget);
  gfl2::gfx::GFGL::SetDepthStencil(rDrawContext.m_pDepthStencil);

  // 描画が終わったのでDrawUtilのテクスチャとスクリーンサイズを元に戻す
  gfl2::gfx::DrawUtil::SetTexture(originalDrawUtilTexture);
  gfl2::gfx::DrawUtil::SetScreenSize(originalDrawUtilScreenWidth, originalDrawUtilScreenHeight);

#if 0 //デバッグ用確認コード。コピーされた内容の確認に用います。
  {
    // 何が描画されているか一旦矩形で表示
    gfl2::gfx::GFGL::BeginScene();
    gfl2::gfx::DrawUtil::BeginRender();

    gfl2::gfx::GFGL::SetViewport(0, 0, 400, 240);
    gfl2::gfx::GFGL::SetScissor(0, 0, 400, 240);
    gfl2::gfx::DrawUtil::SetScreenSize(400, 240);

    gfl2::gfx::DrawUtil::SetMaterialAlpha(1.0f);
    gfl2::gfx::DrawUtil::SetBlendTestState(
      false,
      gfl2::gfx::BlendOp::Add,
      gfl2::gfx::BlendFunc::One,
      gfl2::gfx::BlendFunc::Zero,
      gfl2::gfx::BlendFunc::One,
      gfl2::gfx::BlendFunc::Zero);

    // デプスの設定を戻す
    gfl2::gfx::DepthStencilStateObject::StateData resetStateData;
    resetStateData.m_DepthTestEnable = false;
    gfl2::gfx::DrawUtil::SetDepthTestState(resetStateData);

    gfl2::gfx::DrawUtil::SetTexture(m_pFrameBufferCopy);
    gfl2::gfx::DrawUtil::DrawRect(0.0f, 0.0f, 400.0f, 240.0f);

    gfl2::gfx::DrawUtil::EndRender();
    gfl2::gfx::GFGL::EndScene();
  }
#endif

#endif
}

}}}}
