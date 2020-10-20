#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_InstanceCreator_CLR.h>
#include <clr/include/util/gfl2_String_CLR.h>
#include <clr/include/gfx/gfl2_GFGL_CLR.h>

using System::IntPtr;
using System::Runtime::InteropServices::Marshal;

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

ModelInstanceNode^ InstanceCreator::CreateModelInstanceNode(resource::ResourceNode^ pSrc)
{
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pNative;
  pNative = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), pSrc->GetNative());
	gfl2::renderingengine::scenegraph::instance::DrawableNode* pDrawableNode = pNative->SafeCast<gfl2::renderingengine::scenegraph::instance::DrawableNode>();
	ModelInstanceNode^ pObject = gcnew ModelInstanceNode(pDrawableNode);
	return pObject;
}

LocatorInstanceNode^ InstanceCreator::CreateLocatorInstanceNode()
{
  gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode* pNative;
  pNative = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLocatorInstanceNode(gfl2::clr::gfx::GFGL::GetGLAllocator());
  LocatorInstanceNode^ pObject = gcnew LocatorInstanceNode(pNative);
  return pObject;
}

ViewerLocatorInstanceNode^ InstanceCreator::CreateViewerLocatorInstanceNode(System::String^ nodeName, System::String^ parentName)
{
  std::string nodeName_ = gfl2::clr::util::String::GetNativeString(nodeName);
  std::string parentName_ = gfl2::clr::util::String::GetNativeString(parentName);

  gfl2::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode* pNative;
  pNative = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateViewerLocatorInstanceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), nodeName_.c_str(), parentName_.c_str());
  ViewerLocatorInstanceNode^ pObject = gcnew ViewerLocatorInstanceNode(pNative);
  return pObject;
}

DrawEnvNode^ InstanceCreator::CreateDrawEnvNode(resource::ResourceNode^ pSrc)
{
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNative;
  pNative = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), pSrc->GetNative());
  DrawEnvNode^ pObject = gcnew DrawEnvNode(pNative);
  return pObject;
}

DrawEnvNode^ InstanceCreator::CreateDrawEnvNode()
{
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNative;
  pNative = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(gfl2::clr::gfx::GFGL::GetGLAllocator());
  DrawEnvNode^ pObject = gcnew DrawEnvNode(pNative);
  return pObject;
}

LightSetNode^ InstanceCreator::CreateLightSetNode()
{
  gfl2::renderingengine::scenegraph::instance::LightSetNode* pNative;
  pNative = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode(gfl2::clr::gfx::GFGL::GetGLAllocator());
  LightSetNode^ pObject = gcnew LightSetNode(pNative);
  return pObject;
}

LightNode^ InstanceCreator::CreateLightNode()
{
  gfl2::renderingengine::scenegraph::instance::LightNode* pNative;
  pNative = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(gfl2::clr::gfx::GFGL::GetGLAllocator());
  LightNode^ pObject = gcnew LightNode(pNative);
  return pObject;
}

MotionConfigInstanceNode^ InstanceCreator::CreateMotionConfigInstanceNode(resource::ResourceNode^ pSrc)
{
  gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode* pNative = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateMotionConfigInstanceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), pSrc->GetNative());
  MotionConfigInstanceNode^ pObject = gcnew MotionConfigInstanceNode(pNative);
  return pObject;
}

}}}}}