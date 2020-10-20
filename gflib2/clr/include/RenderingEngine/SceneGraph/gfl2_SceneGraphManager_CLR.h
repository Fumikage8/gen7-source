#ifndef GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_SCENEGRAPHMANAGER_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_SCENEGRAPHMANAGER_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_DrawableNode_CLR.h>
#include <math/include/gfl2_Matrix.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph {

public ref class InitDescription
{
public:
	InitDescription(u32 nodeSize);
	~InitDescription()
	{
		this->!InitDescription();
	}
	!InitDescription();

	gfl2::renderingengine::scenegraph::SceneGraphManager::InitDescription* GetNative()
	{
		return m_pNative;
	}

private:
	gfl2::renderingengine::scenegraph::SceneGraphManager::InitDescription* m_pNative;
};

public ref class SceneGraphManager
{
public:
  static void AddChild(clr::renderingengine::scenegraph::instance::DrawableNode^ node);
	static void RemoveChild(clr::renderingengine::scenegraph::instance::DrawableNode^ node);
	static void Initialize(InitDescription^ desc);
	static void Terminate();
	static void TraverseModel(math::Matrix^ M);

protected:

};

}}}}

#endif