#include "MyRenderingPipeLine.h"

#include <renderingengine/include/renderer/gfl2_MeshDrawTag.h>
#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_GFGLUtil.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;
using namespace gfl2::renderingengine::renderer::util;

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
MyRenderingPipeLine::MyRenderingPipeLine()
{
  {
    // シーンレンダーパス初期化
    SceneRenderPath::InitDescription desc;
    m_SceneRenderPath.Initialize(desc);
  }

  {
    // ジョイントレンダリングパス初期化
    JointRenderPath::InitDescription desc;
    m_JointRenderPath.Initialize(desc);
  }
}

MyRenderingPipeLine::~MyRenderingPipeLine()
{

}

b32 MyRenderingPipeLine::StartRenderPath()
{
	m_RenderPathCnt = 0;
	return true;
}

RenderingPipeLine::IRenderPath* MyRenderingPipeLine::GetRenderPath()
{
	//前のパスの結果を踏まえて、パスをスキップすることも可能。

	switch( m_RenderPathCnt ){
	case 0:
		return &m_SceneRenderPath;
		break;
  case 1:
    return &m_JointRenderPath;
	case 2:
		return &m_PresentPath;
		break;
	}
	return NULL;
}

b32 MyRenderingPipeLine::NextRenderPath()
{
	m_RenderPathCnt++;
	if ( m_RenderPathCnt == 3 )
		return false;
	return true;
}


