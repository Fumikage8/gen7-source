#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_ViewerLocatorInstanceNode_CLR.h>
#include <math/include/gfl2_EulerRotation.h>
#include <math/include/gfl2_SRT.h>
#include <math/include/gfl2_Quaternion.h>
#ifndef PLATFORM_CLR
#define PLATFORM_CLR
#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>
#undef PLATFORM_CLR
#endif

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

ViewerLocatorInstanceNode::ViewerLocatorInstanceNode(gfl2::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode* pNative) :
  LocatorInstanceNode(pNative)
{
  for (s32 i = 0; i < gfl2::renderingengine::PokedecoType::NumberOf; ++i)
  {
    pNative->SetPokedecoAABBVisible(static_cast<gfl2::renderingengine::PokedecoType>(i), false);
  }
}

ViewerLocatorInstanceNode::!ViewerLocatorInstanceNode()
{
}

void ViewerLocatorInstanceNode::SetVisible(bool enable)
{
  gfl2::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode* pNative = 
    GetNative()->SafeCast<gfl2::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode>();
  if (enable)
  {
    pNative->SetState(gfl2::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode::State::Visible);
  }
  else
  {
    pNative->ResetState(gfl2::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode::State::Visible);
  }
}

bool ViewerLocatorInstanceNode::IsVisible()
{
  gfl2::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode* pNative = 
    GetNative()->SafeCast<gfl2::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode>();
  return pNative->CheckState(gfl2::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode::State::Visible);
}

void ViewerLocatorInstanceNode::SetFocused(bool enable)
{
  gfl2::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode* pNative = 
    GetNative()->SafeCast<gfl2::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode>();
  if (enable)
  {
    pNative->SetState(gfl2::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode::State::Focused);
  }
  else
  {
    pNative->ResetState(gfl2::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode::State::Focused);
  }
}

bool ViewerLocatorInstanceNode::IsFocused()
{
  gfl2::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode* pNative = 
    GetNative()->SafeCast<gfl2::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode>();
  return pNative->CheckState(gfl2::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode::State::Focused);
}

void ViewerLocatorInstanceNode::SetPokedecoAABBVisible(gfl2::clr::renderingengine::PokedecoType::Enum type, bool visible)
{
  gfl2::renderingengine::PokedecoType type_ = static_cast<gfl2::renderingengine::PokedecoType>((u32)type);
  gfl2::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode* pNative = 
    GetNative()->SafeCast<gfl2::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode>();
  pNative->SetPokedecoAABBVisible(type_, visible ? 1 : 0);
}

bool ViewerLocatorInstanceNode::IsPokedecoAABBVisible(gfl2::clr::renderingengine::PokedecoType::Enum type)
{
  gfl2::renderingengine::PokedecoType type_ = static_cast<gfl2::renderingengine::PokedecoType>((u32)type);
  gfl2::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode* pNative = 
    GetNative()->SafeCast<gfl2::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode>();
  return pNative->IsPokedecoAABBVisible(type_) ? true : false;
}

}}}}}