#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MotionInstanceNode.h>
#include <RenderingEngine/include/renderer/gfl2_CombinerShaderDriver.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  MotionInstanceNodeクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
MotionInstanceNode::MotionInstanceNode() : NodeTrait<MotionInstanceNode, DrawableNode>()
{

}

MotionInstanceNode::~MotionInstanceNode()
{
	
}


}}}}

