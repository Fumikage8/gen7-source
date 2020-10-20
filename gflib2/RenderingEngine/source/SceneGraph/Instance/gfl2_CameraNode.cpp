#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_CameraNode.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  LightNodeクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
CameraNode::CameraNode() : NodeTrait<CameraNode, TransformNode>(),
  m_Near(0.0f),
  m_Far(0.0f),
  m_Fovy(0.0f),
  m_AspectRatio(0.0f)
{
}

CameraNode::~CameraNode()
{
}

}}}}

