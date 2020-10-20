#include <clr/include/RenderingEngine/SceneGraph/Resource/gfl2_ResourceManager_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Resource/gfl2_IResourceData_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Resource/gfl2_ResourceNode_CLR.h>
#include <clr/include/gfx/gfl2_GFGL_CLR.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace resource {

void ResourceManager::Initialize()
{
  gfl2::renderingengine::scenegraph::resource::ResourceManager::Initialize(gfl2::clr::gfx::GFGL::GetGLAllocator());
}

void ResourceManager::Terminate()
{
	gfl2::renderingengine::scenegraph::resource::ResourceManager::Terminate();
}

ResourceNode^ ResourceManager::CreateResourceNode(IResourceData^ resourceData)
{
	gfl2::renderingengine::scenegraph::resource::ResourceNode* pNative;
	pNative = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(
    gfl2::clr::gfx::GFGL::GetGLAllocator(),
		resourceData->GetNative()
	);
	ResourceNode^ pObject = gcnew ResourceNode(pNative);
	return pObject;
}

ResourceManager::ResourceManager()
{
}

ResourceManager::!ResourceManager()
{
}

JointAnimationResourceNodeFactory^ ResourceManager::GetJointAnimationResourceNodeFactory()
{
  // なが。。
  //gfl2::renderingengine::scenegraph::resource::JointAnimationResourceNodeFactory* pNative = 
    //dynamic_cast<gfl2::renderingengine::scenegraph::resource::JointAnimationResourceNodeFactory*>(gfl2::renderingengine::scenegraph::resource::ResourceManager::GetResourceNodeFactory(gfl2::renderingengine::scenegraph::resource::IResourceData::Type("jointani")));

  return gcnew JointAnimationResourceNodeFactory();
}

}}}}}