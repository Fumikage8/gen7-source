#if defined(GF_PLATFORM_WIN32)

#ifndef GFLIB2_RENDERINGENGINE_RENDERER_UtilJointRenderPath_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_RENDERER_UtilJointRenderPath_H_INCLUDED

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include <gfx/include/gfl2_DrawUtil.h>

namespace gfl2 { namespace renderingengine { namespace renderer {  namespace util {

//-----------------------------------------------------------
//! @brief ジョイント描画パスクラス
//-----------------------------------------------------------
class JointRenderPath : public RenderingPipeLine::IRenderPath
{
public:

  struct InitDescription
  {
    u32 maxJointCount;                  // 描画可能な最大ジョイント数
    gfl2::math::Vector4 jointColor;      // ジョイントカラー
    f32 xyzScale;                       // XYZ軸のスケール
    f32 boneScale;                      // ボーンのスケール

    InitDescription() :
      maxJointCount(1024),
      jointColor(0.6f, 0.8f, 0.6f),
      xyzScale(3.0f),
      boneScale(0.8f)
    {
    }
  };

  JointRenderPath();
  virtual ~JointRenderPath();

  void Initialize(const InitDescription& desc);

  virtual const IShaderDriver* ShaderOverride(){ return NULL; }
  virtual const gfx::GLFixedSizeContainer<scenegraph::instance::DrawableNode*>* DrawableNodeOverride(){ return NULL; }

  virtual void SetRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil );
  virtual void ClearRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil );

  virtual void Execute( const RenderingPipeLine::DrawContext& rDrawContext );

protected:

  InitDescription m_InitDescription;                                              // 初期化設定
  gfl2::gfx::DrawUtil::VertexFormat* m_pVertexFormatList;                         // 頂点フォーマットリスト
  u32 m_VertexFormatLimit;                                                        // 頂点フォーマットリスト確保数
};

}}}}

#endif

#endif
