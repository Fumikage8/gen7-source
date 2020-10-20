#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_JointInstanceNode_CLR.h>

using System::IntPtr;
using System::Runtime::InteropServices::Marshal;

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

JointInstanceNode::JointInstanceNode(gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pNative) :
m_pNative(pNative)
{
}

JointInstanceNode::!JointInstanceNode()
{
	// このノードを持つDrawEnvNodeで削除の処理を行うのでネイティブポインタのdeleteはしない
}

void JointInstanceNode::Release()
{
  if (m_pNative)
  {
    delete m_pNative;
    m_pNative = NULL;
  }
}

System::String^ JointInstanceNode::GetName()
{
  return gcnew System::String(m_pNative->GetName());
}
System::String^ JointInstanceNode::GetParentName()
{
  if (m_pNative->GetParent() == NULL)
  {
    return nullptr;
  }

  return gcnew System::String(m_pNative->GetParentName());
}

clr::math::Matrix^ JointInstanceNode::GetWorldMatrix()
{
	gfl2::clr::math::Matrix^ matrix = gcnew gfl2::clr::math::Matrix(m_pNative->GetWorldMatrix());

	return matrix;
}

}}}}}