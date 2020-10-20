#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_LocatorInstanceNode_CLR.h>
#include <math/include/gfl2_EulerRotation.h>
#include <math/include/gfl2_SRT.h>
#include <math/include/gfl2_Quaternion.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

void LocatorInstanceNode::Util::AttachToJointInstanceNode(JointInstanceNode^ pJointNodeTop, LocatorInstanceNode^ pLocatorNodeTop )
{
  gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode::Util::AttachToJointInstanceNode(
    pJointNodeTop->GetNative(),
    pLocatorNodeTop->GetNative()
    );
}

LocatorInstanceNode::LocatorInstanceNode(gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode* pNative) :
m_pNative(pNative)
{
}

LocatorInstanceNode::!LocatorInstanceNode()
{
}

//<! @brief 名前取得
System::String^ LocatorInstanceNode::GetName()
{
  return gcnew System::String(m_pNative->GetName());
}

//<! @brief 名前取得
System::String^ LocatorInstanceNode::GetParentName()
{
  return gcnew System::String(m_pNative->GetParentName());
}

//<! @brief 平行移動値設定
void LocatorInstanceNode::SetTranslate(gfl2::clr::math::Vector^ translate)
{
  m_pNative->SetLocalTranslate(gfl2::math::Vector3(translate->x, translate->y, translate->z));
}

//<! @brief 回転設定
void LocatorInstanceNode::SetRotation(gfl2::clr::math::Quaternion^ quaternion)
{
  m_pNative->SetLocalRotation(*quaternion->GetNative());
}

//<! @brief スケール設定
void LocatorInstanceNode::SetScale(gfl2::clr::math::Vector^ scale)
{
  m_pNative->SetLocalScale(gfl2::math::Vector3(scale->x, scale->y, scale->z));
}

}}}}}