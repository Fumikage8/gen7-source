#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_MotionConfigInstanceNode_CLR.h>

using System::IntPtr;
using System::Runtime::InteropServices::Marshal;

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

MotionConfigInstanceNode::MotionConfigInstanceNode(gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode* pNative) :
m_pNative(pNative)
{
}

MotionConfigInstanceNode::!MotionConfigInstanceNode()
{
	// このノードを持つDrawEnvNodeで削除の処理を行うのでネイティブポインタのdeleteはしない
}

void MotionConfigInstanceNode::Release()
{
  if (m_pNative)
  {
    delete m_pNative;
    m_pNative = NULL;
  }
}

}}}}}