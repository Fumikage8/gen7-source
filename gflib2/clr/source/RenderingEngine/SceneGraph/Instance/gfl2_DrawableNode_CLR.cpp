#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_DrawableNode_CLR.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

DrawableNode::UpdateDrawTagInfo::UpdateDrawTagInfo( gfl2::clr::math::Matrix^ viewProjectionMatrix )
{
  m_pNative = new gfl2::renderingengine::scenegraph::instance::DrawableNode::UpdateDrawTagInfo();
  m_pNative->m_ViewMatrix = *viewProjectionMatrix->GetNative();
}

DrawableNode::UpdateDrawTagInfo::!UpdateDrawTagInfo()
{
  if (m_pNative)
  {
    delete m_pNative;
    m_pNative = 0;
  }
}

DrawableNode::DrawableNode( gfl2::renderingengine::scenegraph::instance::DrawableNode *pNative )
{
	m_pNative = pNative;
}

DrawableNode::!DrawableNode()
{
	if (m_pNative) 
	{
		delete m_pNative;
		m_pNative = 0;
	}
}

}}}}}