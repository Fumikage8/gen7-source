#if !defined(GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_INSTANCE_VIEWERLOCATORINSTANCENODE_H_INCLUDED)
#define GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_INSTANCE_VIEWERLOCATORINSTANCENODE_H_INCLUDED
#pragma once

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ViewerLocatorInstanceNode.h>
#include <clr/include/math/gfl2_Matrix_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_LocatorInstanceNode_CLR.h>
#ifndef PLATFORM_CLR
#define PLATFORM_CLR
#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>
#undef PLATFORM_CLR
#endif

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

public ref class ViewerLocatorInstanceNode : LocatorInstanceNode
{
public:
	ViewerLocatorInstanceNode(gfl2::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode* pNative);
	virtual ~ViewerLocatorInstanceNode(){ this->!ViewerLocatorInstanceNode(); }
	!ViewerLocatorInstanceNode();

  void SetVisible(bool enable);
  bool IsVisible();

  void SetFocused(bool enable);
  bool IsFocused();

  void SetPokedecoAABBVisible(gfl2::clr::renderingengine::PokedecoType::Enum type, bool visible);
  bool IsPokedecoAABBVisible(gfl2::clr::renderingengine::PokedecoType::Enum type);

private:
};

}}}}}

#endif

