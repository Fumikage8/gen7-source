#include "rendering_pipeline.h"

#include <RenderingEngine/include/Renderer/gfl2_MeshDrawTag.h>
#include <gfx/include/gfl2_GFGLUtil.h>

//#include <FontString/include/gfl2_FontString.h>
#include <FontRenderer/include/gfl2_FontRenderer.h>
#include <system/include/Timer/gfl2_PerformanceCounter.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;
using namespace gfl2::renderingengine::renderer::util;

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
KawaigariRenderingPipeLine::KawaigariRenderingPipeLine()
{
	SceneRenderPath::InitDescription			desc;
	m_SceneRenderPath.Initialize( desc );
	m_IDRenderPath.Initialize( desc );

  {
    // ジョイントレンダリングパス初期化
    JointRenderPath::InitDescription desc;
    m_JointRenderPath.Initialize(desc);

    m_JointRenderEnabled = true;
  }

  {
    // モーションコンフィグレンダリングパス初期化
    McnfRenderPath::InitDescription desc;
    m_McnfRenderPath.Initialize(desc);
  }
}

KawaigariRenderingPipeLine::~KawaigariRenderingPipeLine()
{
}

b32 KawaigariRenderingPipeLine::StartRenderPath()
{
	m_RenderPathCnt = 0;
	return true;
}

RenderingPipeLine::IRenderPath* KawaigariRenderingPipeLine::GetRenderPath()
{
	//前のパスの結果を踏まえて、パスをスキップすることも可能。

	switch( m_RenderPathCnt ){
	case 0:
		return &m_IDRenderPath;
		break;
	case 1:
		return &m_SceneRenderPath;
		break;
	case 2:
    return (m_JointRenderEnabled) ? &m_JointRenderPath : NULL;
		break;
  case 3:
    return &m_McnfRenderPath;
    break;
	case 4:
		return &m_PresentPath;
		break;
	}
	return NULL;
}

b32 KawaigariRenderingPipeLine::NextRenderPath()
{
	m_RenderPathCnt++;
	if ( m_RenderPathCnt == 5 )
		return false;
	return true;
}

void KawaigariRenderingPipeLine::AddIDObject( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
  m_IDRenderPath.AddDrawableNode(pNode);
}

void KawaigariRenderingPipeLine::AddDrawEnv( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode )
{
	m_SceneRenderPath.AddDrawEnv(pNode);
}

