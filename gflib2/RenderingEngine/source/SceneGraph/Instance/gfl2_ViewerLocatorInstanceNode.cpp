#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ViewerLocatorInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>
#include <RenderingEngine/include/renderer/gfl2_CombinerShaderDriver.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  ModelInstanceNodeクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
ViewerLocatorInstanceNode::ViewerLocatorInstanceNode() :
  NodeTrait<ViewerLocatorInstanceNode, LocatorInstanceNode>(),
  m_State(0)
{
  for (s32 i = 0; i < gfl2::renderingengine::PokedecoType::NumberOf; ++i)
  {
    m_PokedecoAABBVisibility[i] = false;
  }
}

ViewerLocatorInstanceNode::ViewerLocatorInstanceNode(const c8* nodeName, const c8* parentName) :
  NodeTrait<ViewerLocatorInstanceNode, LocatorInstanceNode>(),
  m_State(0)
{
  SetNodeName(nodeName);
#ifdef WIN32
  strcpy_s(m_ParentName, parentName);
#else
	std::strcpy(m_ParentName, parentName);
#endif
}

ViewerLocatorInstanceNode::~ViewerLocatorInstanceNode()
{
}

void ViewerLocatorInstanceNode::SetState(u32 state)
{
  m_State |= state;
}

void ViewerLocatorInstanceNode::ResetState(u32 state)
{
  m_State &= ~state;
}

bool ViewerLocatorInstanceNode::CheckState(u32 state)
{
  return (m_State & state) ? true : false;
}

void ViewerLocatorInstanceNode::SetPokedecoAABBVisible(gfl2::renderingengine::PokedecoType type, b32 visible)
{
  m_PokedecoAABBVisibility[type] = visible;
}

b32 ViewerLocatorInstanceNode::IsPokedecoAABBVisible(gfl2::renderingengine::PokedecoType type)
{
  return m_PokedecoAABBVisibility[type];
}

}}}}

