#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_EffectNodeLocatorInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>
#include <RenderingEngine/include/renderer/gfl2_CombinerShaderDriver.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  ModelInstanceNodeクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
EffectNodeLocatorInstanceNode::EffectNodeLocatorInstanceNode() :
  NodeTrait<EffectNodeLocatorInstanceNode, LocatorInstanceNode>()
{
	
}

EffectNodeLocatorInstanceNode::EffectNodeLocatorInstanceNode(const c8* parentName) :
  NodeTrait<EffectNodeLocatorInstanceNode, LocatorInstanceNode>()
{
#ifdef WIN32
  strcpy_s(m_ParentName, parentName);
#else
	std::strcpy(m_ParentName, parentName);
#endif
}

EffectNodeLocatorInstanceNode::~EffectNodeLocatorInstanceNode()
{
}


}}}}

