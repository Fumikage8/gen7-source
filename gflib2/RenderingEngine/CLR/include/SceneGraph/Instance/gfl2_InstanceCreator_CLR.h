#ifndef GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_INSTANCE_INSTANCECREATOR_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_INSTANCE_INSTANCECREATOR_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LocatorInstanceNode.h>
#include <RenderingEngine/CLR/include/SceneGraph/Instance/gfl2_DrawableNode_CLR.h>
#include <RenderingEngine/CLR/include/SceneGraph/Instance/gfl2_ModelInstanceNode_CLR.h>
#include <RenderingEngine/CLR/include/SceneGraph/Resource/gfl2_ResourceNode_CLR.h>

namespace gfl2 { namespace renderingengine { namespace clr { namespace scenegraph { namespace instance {

public ref class InstanceCreator
{
public:
	static ModelInstanceNode^ InstanceCreator::CreateModelInstanceNode(resource::ResourceNode^ pSrc);
};

}}}}}

#endif