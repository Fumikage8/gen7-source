#ifndef GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_RESOURCE_IRESOURCEDATA_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_RESOURCE_IRESOURCEDATA_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceData.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace resource {

public interface class IResourceData
{
public:
	gfl2::renderingengine::scenegraph::resource::IResourceData* GetNative();
};

}}}}}

#endif