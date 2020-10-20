#include <clr/include/RenderingEngine/SceneGraph/Resource/gfl2_MaterialResourceNode_CLR.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace resource {

MaterialResourceNode::AttributeParam::AttributeParam(gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam* pNative) :
  m_pNative(pNative)
{
}

MaterialResourceNode::AttributeParam::!AttributeParam()
{
}

void MaterialResourceNode::AttributeParam::SetLightSetNo(s16 no)
{
  m_pNative->m_LightSetNo = no;
}

s16 MaterialResourceNode::AttributeParam::GetLightSetNo()
{
  return m_pNative->m_LightSetNo;
}

}}}}}
