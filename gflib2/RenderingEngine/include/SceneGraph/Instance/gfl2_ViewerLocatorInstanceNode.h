#if !defined(GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_VIEWERLOCATORINSTANCENODE_H_INCLUDED)
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_VIEWERLOCATORINSTANCENODE_H_INCLUDED
#pragma once

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_TransformNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_JointInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LocatorInstanceNode.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

class InstanceCreator;

class ViewerLocatorInstanceNode : public NodeTrait<ViewerLocatorInstanceNode, LocatorInstanceNode>
{
public:
  class State
  {
  public:
    enum Enum
    {
      None = 0,
      Visible = (1 << 0),     //!< 可視状態
      Focused = (1 << 1),     //!< 選択状態
    };
  };

  friend class InstanceCreator;

  virtual ~ViewerLocatorInstanceNode();

  static util::FnvHash GetTypeHash()
  {
    static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode::TransformNode::ViewerLocatorInstanceNode") ) );
    return hash;
  }

  void SetState(u32 state);
  void ResetState(u32 state);
  bool CheckState(u32 state);

  void SetPokedecoAABBVisible(gfl2::renderingengine::PokedecoType type, b32 visible);
  b32 IsPokedecoAABBVisible(gfl2::renderingengine::PokedecoType type);

protected:
  ViewerLocatorInstanceNode();
  ViewerLocatorInstanceNode(const c8* nodeName, const c8* parentName);

private:
  u32 m_State;
  b32 m_PokedecoAABBVisibility[gfl2::renderingengine::PokedecoType::NumberOf];
};

}}}}

#endif

