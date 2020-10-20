#ifndef GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_INSTANCE_DRAWENVNODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_INSTANCE_DRAWENVNODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_LightSetNode_CLR.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

public ref class DrawEnvNode
{
public:
	DrawEnvNode(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode);
	~DrawEnvNode()
	{
		this->!DrawEnvNode();
	}
	!DrawEnvNode();

	gfl2::renderingengine::scenegraph::instance::DrawEnvNode* GetNative()
	{
		return m_pNative;
	}

  void AddLightSet(LightSetNode^ node);
  void RemoveLightSet(LightSetNode^ node);
  LightSetNode^ GetLightSet(s32 setNo);

private:
	gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pNative;
};

}}}}}

#endif