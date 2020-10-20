#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_LightSetNode_CLR.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

LightSetNode::LightSetNode(gfl2::renderingengine::scenegraph::instance::LightSetNode* pNode) :
m_pNative(pNode)
{
}

LightSetNode::!LightSetNode()
{
	// このノードを持つDrawEnvNodeで削除の処理を行うのでネイティブポインタのdeleteはしない
}

void LightSetNode::AddLight(gfl2::clr::renderingengine::scenegraph::instance::LightNode^ pNode)
{
	m_pNative->AddLight(pNode->GetNative());
}

void LightSetNode::RemoveLight(gfl2::clr::renderingengine::scenegraph::instance::LightNode^ pNode)
{
	m_pNative->RemoveLight(pNode->GetNative());
}

void LightSetNode::RemoveAllLight()
{
	m_pNative->RemoveAllLight();
}

u32 LightSetNode::GetLightNum()
{
	return m_pNative->GetLightNum();
}

LightNode^ LightSetNode::GetLightNode(s32 no)
{
	return gcnew LightNode(m_pNative->GetLightNode(no));
}

void LightSetNode::SetSetNo(u32 no)
{
  m_pNative->SetSetNo(no);
}

u32 LightSetNode::GetSetNo()
{
  return m_pNative->GetSetNo();
}

void LightSetNode::Release()
{
  if (m_pNative)
  {
    delete m_pNative;
    m_pNative = NULL;
  }
}

}}}}}