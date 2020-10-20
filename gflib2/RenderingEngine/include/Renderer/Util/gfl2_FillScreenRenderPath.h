#if !defined(GFL2_RENDERINGENGINE_RENDERER_UTIL_FILLSCREENRENDERPATH_H_INCLUDED)
#define GFL2_RENDERINGENGINE_RENDERER_UTIL_FILLSCREENRENDERPATH_H_INCLUDED
#pragma once

#include <renderingengine/include/renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {

//-----------------------------------------------------------
//! @brief カラーシェーダテクスチャ描画パスクラス
//-----------------------------------------------------------
class FillScreenRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:
  FillScreenRenderPath();
  virtual ~FillScreenRenderPath();

  void Execute( const RenderingPipeLine::DrawContext& rDrawContext );

  void SetFillScreenTexture(const gfx::Texture* pTexture)
  {
    m_pFillScreenTexture = pTexture;
  }

  void SetColorShaderRenderTargetPlane(scenegraph::instance::ModelInstanceNode* pNode)
  {
    m_pRenderTargetModelInstanceNode = pNode;
  }

  void SetConstantColorToRenderTargetPlane(u32 constantNo, const gfl2::math::Vector4& color);

private:
  const gfx::Texture* m_pFillScreenTexture;    //!<@brief カラーシェーダ適用テクスチャ
  scenegraph::instance::ModelInstanceNode* m_pRenderTargetModelInstanceNode;  //!<@brief 板ポリゴンを持つだけのノード
};

}}}}

#endif
