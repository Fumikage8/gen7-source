#if !defined(GFL2_RENDERINGENGINE_CLR_RENDERER_UTIL_MODELRENDERPATH_H_INCLUDED)
#define GFL2_RENDERINGENGINE_CLR_RENDERER_UTIL_MODELRENDERPATH_H_INCLUDED
#pragma once

#include <renderingengine/include/renderer/gfl2_RenderingPipeLine.h>
#include <util/include/gfl2_FixedSizeContainer.h>

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {
 
//-----------------------------------------------------------
//! @brief モデル描画パスクラス
//-----------------------------------------------------------
class ModelRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:
  struct InitDescription
  {
    DrawManager::InitDescription			m_DrawManagerDesc;
    InitDescription() : m_DrawManagerDesc( 256, 1024 * 8, 16 ){}//128*8*8 = r02のmap01_11_11が８つ分くらい。
  };

  struct Camera
  {
    b32               m_UseCamera;    // カメラの情報を使用する/しない
    math::Matrix44    m_Projection;   // 透視変換行列
    math::Matrix34    m_View;					// ビュー変換行列
  };

  ModelRenderPath();
  virtual ~ModelRenderPath();

  void Initialize(gfx::IGLAllocator* pObj, const InitDescription& desc);

  virtual const IShaderDriver* ShaderOverride(){ return NULL; }
  virtual const gfl2::util::FixedSizeContainer<scenegraph::instance::DrawableNode*>* DrawableNodeOverride(){ return NULL; }

  virtual void SetRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil );
  virtual void ClearRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil );
  virtual const Camera* CameraOverride(){ return &m_Camera; }

  virtual void Update( const RenderingPipeLine::DrawContext& rDrawContext );
  virtual void Execute( const RenderingPipeLine::DrawContext& rDrawContext );

  void SetDrawEnvNode(scenegraph::instance::DrawEnvNode* pNode);
  void SetModelInstanceNode(scenegraph::instance::ModelInstanceNode* pNode);
  scenegraph::instance::DrawEnvNode* GetDrawEnvNode() const;
  scenegraph::instance::ModelInstanceNode* GetModelInstanceNode() const;

  void SetCameraData(const Camera& camera);

protected:
  void SetCamera(const RenderingPipeLine::DrawContext& rDrawContext);

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pDrawEnvNode;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_pModelInstanceNode;

  DrawManager *m_pDrawManager;

  u32 m_LightSetNo;   // このパスが使用するライトセット番号

  Camera m_Camera;
};

}}}}

#endif