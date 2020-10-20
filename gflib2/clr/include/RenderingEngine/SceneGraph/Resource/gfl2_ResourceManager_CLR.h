#ifndef GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_RESOURCE_RESOURCEMANAGER_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_RESOURCE_RESOURCEMANAGER_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <clr/include/RenderingEngine/SceneGraph/Resource/gfl2_ResourceNode_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Resource/gfl2_IResourceData_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Resource/gfl2_JointAnimationResourceNodeFactory_CLR.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace resource {

public ref class ResourceManager
{
public:
	static void Initialize();
	static void Terminate();

	static ResourceNode^ CreateResourceNode(IResourceData^ resourceData);

  static JointAnimationResourceNodeFactory^ GetJointAnimationResourceNodeFactory();

private:
	ResourceManager();
	~ResourceManager(){this->!ResourceManager();}
	!ResourceManager();
};

}}}}}

#endif