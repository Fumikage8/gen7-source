#include <clr/include/RenderingEngine/SceneGraph/gfl2_SceneGraphManager_CLR.h>
#include <math/include/gfl2_Matrix.h>
#include <clr/include/gfx/gfl2_GFGL_CLR.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph {

InitDescription::InitDescription( u32 nodeSize )
{
	m_pNative = new gfl2::renderingengine::scenegraph::SceneGraphManager::InitDescription( nodeSize );
}

InitDescription::!InitDescription()
{
	if ( m_pNative )
	{
		delete m_pNative;
		m_pNative = 0;
	}
}

void SceneGraphManager::AddChild(clr::renderingengine::scenegraph::instance::DrawableNode^ node)
{
  gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild(node->GetNative());
}

void SceneGraphManager::RemoveChild(clr::renderingengine::scenegraph::instance::DrawableNode^ node)
{
  gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild(node->GetNative());
}

void SceneGraphManager::Initialize(InitDescription^ desc)
{
  gfl2::renderingengine::scenegraph::SceneGraphManager::Initialize(gfl2::clr::gfx::GFGL::GetGLAllocator(), *(desc->GetNative()));
}

void SceneGraphManager::Terminate()
{
	gfl2::renderingengine::scenegraph::SceneGraphManager::Terminate();
}

void SceneGraphManager::TraverseModel(math::Matrix^ M)
{
	gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModel();
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseApplyBillboard(M->GetNative());
}

}}}}