#include "MyRenderingPipeLine.h"

#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_GFGLUtil.h>
#include <RenderingEngine/include/Renderer/gfl2_MeshDrawTag.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;
using namespace gfl2::renderingengine::renderer::util;

MyRenderingPipeLine::MyRenderingPipeLine()
{
  m_ColorShaderTextureRenderPaths.CreateBuffer(100);    // 100枚のカラーテクスチャに対応
	SceneRenderPath::InitDescription			desc;
	m_SceneRenderPath.Initialize( desc );
}

MyRenderingPipeLine::~MyRenderingPipeLine()
{

}

b32 MyRenderingPipeLine::StartRenderPath()
{
	m_RenderPathCnt = 0;
	return true;
}

void MyRenderingPipeLine::SetColorShaderRenderTargetPlane(scenegraph::instance::ModelInstanceNode* pNode)
{
  m_ColorShaderRenderTargetPlane = pNode;
}

void MyRenderingPipeLine::AddColorShaderTexture(
  const gfl2::gfx::Texture* pOriginalColorTexture,
  const gfl2::gfx::Texture* pColorShaderTexture)
{
  ColorShaderTextureRenderPath* path = new ColorShaderTextureRenderPath();
  path->SetColorShaderRenderTargetPlane(m_ColorShaderRenderTargetPlane);
  path->SetOriginalColorTexture(pOriginalColorTexture);
  path->SetColorShaderTexture(pColorShaderTexture);
  m_ColorShaderTextureRenderPaths.push_back(path);
}

void MyRenderingPipeLine::RemoveColorShaderTexture(u32 index)
{
  renderer::util::ColorShaderTextureRenderPath* path = m_ColorShaderTextureRenderPaths[index];
  m_ColorShaderTextureRenderPaths.remove(path);
  delete path;
}

void MyRenderingPipeLine::SetColorShaderTextureColor(
  u32 startPassIndex,
  u32 passNum,
  const math::Vector& color)
{
  u32 size = m_ColorShaderTextureRenderPaths.size();
  assert(startPassIndex + passNum <= size);

  for (u32 i = startPassIndex; i < startPassIndex + passNum; ++i)
  {
    m_ColorShaderTextureRenderPaths[i]->SetColorShaderTextureColor(color);
  }
}

RenderingPipeLine::IRenderPath* MyRenderingPipeLine::GetRenderPath()
{
  // カラーシェーダ用の描画パスを画面のレンダリングの前に実行
  u32 colorShaderRenderPathNum = m_ColorShaderTextureRenderPaths.size();
  if (m_RenderPathCnt < colorShaderRenderPathNum)
  {
    return m_ColorShaderTextureRenderPaths[m_RenderPathCnt];
  }

  // カラーシェーダ用の描画パスの更新が終わっていたら, 描画パスの更新数だけ引いて画面描画開始
  u32 renderPathCnt = m_RenderPathCnt - colorShaderRenderPathNum;

	switch( renderPathCnt ){
	case 0:
		return &m_SceneRenderPath;
	case 1:
		return &m_PresentPath;
	}
	return NULL;
}

b32 MyRenderingPipeLine::NextRenderPath()
{
	  ++m_RenderPathCnt;
  u32 colorShaderRenderPathNum = m_ColorShaderTextureRenderPaths.size();
  if (m_RenderPathCnt < colorShaderRenderPathNum)
  {
    return true;
  }

  u32 renderPathCnt = m_RenderPathCnt - colorShaderRenderPathNum;

  if (renderPathCnt == 2)
  {
    return false;
  }

  m_RenderPathCnt = renderPathCnt + colorShaderRenderPathNum;
	return true;
}
