#ifndef MyRenderingPipeLine_H_INCLUDED
#define MyRenderingPipeLine_H_INCLUDED

#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/renderer/util/gfl2_ColorShaderTextureRenderPath.h>
#include <util/include/gfl2_FixedSizeContainer.h>

class MyRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
public:
	MyRenderingPipeLine();
	virtual ~MyRenderingPipeLine();

	void AddDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode)
  {
    m_SceneRenderPath.AddDrawEnv(pNode);
  }

  void SetColorShaderRenderTargetPlane(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pNode);
  void AddColorShaderTexture(const gfl2::gfx::Texture* pOriginalColorTexture, const gfl2::gfx::Texture* pColorShaderTexture);
  void RemoveColorShaderTexture(u32 index);
  void SetColorShaderTextureColor(u32 startPassIndex, u32 passNum, const gfl2::math::Vector& color);

private:
  virtual b32 StartRenderPath();
  virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath();
  virtual b32 NextRenderPath();

  u32 m_RenderPathCnt;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_ColorShaderRenderTargetPlane;
  gfx::GLFixedSizeContainer<gfl2::renderingengine::renderer::util::ColorShaderTextureRenderPath*> m_ColorShaderTextureRenderPaths;
  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath m_SceneRenderPath;
  gfl2::renderingengine::renderer::RenderingPipeLine::PresentPath m_PresentPath;
};

#endif
