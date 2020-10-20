#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_DrawEnvNode_CLR.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

DrawEnvNode::DrawEnvNode(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode) :
m_pNative(pNode)
{
}

DrawEnvNode::!DrawEnvNode()
{
	if (m_pNative)
	{
		delete m_pNative;
		m_pNative = 0;
	}
}

void DrawEnvNode::AddLightSet(LightSetNode^ node)
{
  m_pNative->AddLightSet(node->GetNative());
}

void DrawEnvNode::RemoveLightSet(LightSetNode^ node)
{
  m_pNative->RemoveLightSet(node->GetNative());
}

LightSetNode^ DrawEnvNode::GetLightSet(s32 setNo)
{
  return gcnew LightSetNode(m_pNative->GetLightSet(setNo));
}

}}}}}