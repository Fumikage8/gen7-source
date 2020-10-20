#ifndef GFLIB2_RENDERINGENGINE_RENDERER_UtilEffectLocatorRenderPath_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_RENDERER_UtilEffectLocatorRenderPath_H_INCLUDED

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include <gfx/include/gfl2_DrawUtil.h>
#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>

namespace gfl2 { namespace renderingengine { namespace renderer {  namespace util {

//-----------------------------------------------------------
//! @brief ジョイント描画パスクラス
//-----------------------------------------------------------
class EffectLocatorRenderPath : public RenderingPipeLine::IRenderPath
{
public:

  static const gfl2::math::AABB s_PokedecoBoundingBox[gfl2::renderingengine::PokedecoType::NumberOf];

  struct InitDescription
  {
    InitDescription()
    {
    }
  };

  EffectLocatorRenderPath();
  virtual ~EffectLocatorRenderPath();

  void Initialize(gfl2::gfx::IGLAllocator * pGLAllocator, const InitDescription& desc);

  virtual const IShaderDriver* ShaderOverride(){ return NULL; }
  virtual const gfx::GLFixedSizeContainer<scenegraph::instance::DrawableNode*>* DrawableNodeOverride(){ return NULL; }

  virtual void SetRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil );
  virtual void ClearRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil );

  virtual void Execute( const RenderingPipeLine::DrawContext& rDrawContext );

protected:
  static const s32 LOCATOR_MAX = 256;
  InitDescription m_InitDescription;            // 初期化設定
  gfl2::gfx::DrawUtil::VertexFormat* m_pVertexFormatList; // 頂点フォーマットリスト
  u32 m_VertexFormatLimit;                                // 頂点フォーマットリスト確保数

};

}}}}

#endif

