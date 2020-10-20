#if !defined(GFLIB2_RENDERINGENGINE_RENDERER_UTILNODERENDERPATH_H_INCLUDED)
#define GFLIB2_RENDERINGENGINE_RENDERER_UTILNODERENDERPATH_H_INCLUDED

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include <gfx/include/gfl2_DrawUtil.h>

namespace gfl2 { namespace renderingengine { namespace renderer {  namespace util {

//-----------------------------------------------------------
//! @brief ノード描画パスクラス
//-----------------------------------------------------------
class NodeRenderPath : public RenderingPipeLine::IRenderPath
{
public:

  struct InitDescription
  {
    InitDescription()
    {
    }
  };

  NodeRenderPath();
  virtual ~NodeRenderPath();

  void Initialize(gfl2::gfx::IGLAllocator* pAllocator, const InitDescription& desc);

  virtual const IShaderDriver* ShaderOverride(){ return NULL; }
  virtual const gfx::GLFixedSizeContainer<scenegraph::instance::DrawableNode*>* DrawableNodeOverride(){ return NULL; }

  virtual void SetRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil );
  virtual void ClearRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil );

  virtual void Execute( const RenderingPipeLine::DrawContext& rDrawContext );

  /**
   * @brief 選択中ノードを設定する
   * @param pNode ノード
   */
  void SetSelectedNode(gfl2::renderingengine::scenegraph::DagNode* pNode);

private:

  /**
   * @brief 選択中ノードを描画する
   */
  void DrawSelectedNode();

protected:

  InitDescription m_InitDescription;                                         // 初期化設定
  gfl2::gfx::DepthStencilStateObject          *m_DepthStencilStateObject;    // デプスステンシルオブジェクト

  gfl2::renderingengine::scenegraph::DagNode  *m_pSelectedNode;              // 選択中ノード
};

}}}}

#endif
