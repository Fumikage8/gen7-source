#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_MaterialInstanceNode_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Resource/gfl2_MaterialResourceNode_CLR.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

MaterialInstanceNode::MaterialInstanceNode(gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pNative) :
  m_pNative(pNative)
{
}

MaterialInstanceNode::!MaterialInstanceNode()
{
}

scenegraph::resource::MaterialResourceNode::AttributeParam^ MaterialInstanceNode::GetAttributeParam()
{
  scenegraph::resource::MaterialResourceNode::AttributeParam^ attributeParam;
  attributeParam = gcnew scenegraph::resource::MaterialResourceNode::AttributeParam(m_pNative->GetAttributeParam());
  return attributeParam;
}

}}}}}