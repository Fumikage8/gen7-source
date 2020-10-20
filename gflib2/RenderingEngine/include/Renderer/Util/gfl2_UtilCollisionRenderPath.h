#ifndef GFLIB2_RENDERINGENGINE_RENDERER_UtilCollisionPath_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_RENDERER_UtilCollisionPath_H_INCLUDED

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include <gfx/include/gfl2_DrawUtil.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CollisionMeshResourceNode.h>

namespace gfl2 { namespace renderingengine { namespace renderer {  namespace util {

//-----------------------------------------------------------
//! @brief コリジョン描画パスクラス
//-----------------------------------------------------------
class CollisionRenderPath : public RenderingPipeLine::IRenderPath
{
public:

  struct InitDescription
  {
    u32 maxTriangleCount;               // 描画可能な最大トライアングル数
    gfl2::math::Vector4 terrainColor;          // 地形カラー
    gfl2::math::Vector4 currentTerrainColor;   // 現在乗っている地形カラー

    InitDescription() :
      maxTriangleCount(1000),
      terrainColor(0.1f, 0.6f, 0.1f),
      currentTerrainColor(0.6f, 0.1f, 0.1f)
    {
    }
  };

  CollisionRenderPath();
  virtual ~CollisionRenderPath();

  void Initialize(const InitDescription& desc);

  virtual const IShaderDriver* ShaderOverride(){ return NULL; }
  virtual const gfx::GLFixedSizeContainer<scenegraph::instance::DrawableNode*>* DrawableNodeOverride(){ return NULL; }

  virtual void SetRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil );
  virtual void ClearRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil );

  virtual void Execute( const RenderingPipeLine::DrawContext& rDrawContext );

  void SetCurrentTerrain(const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pCurrentTerrain)
  {
    m_pCurrentTerrain = pCurrentTerrain;
  }

protected:

  InitDescription m_InitDescription;                                              // 初期化設定
  gfl2::gfx::DrawUtil::VertexFormat* m_pVertexFormatList;                         // 頂点フォーマットリスト
  u32 m_VertexFormatLimit;                                                        // 頂点フォーマットリスト確保数

  const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* m_pCurrentTerrain; // 現在乗っている地形
};

}}}}

#endif

