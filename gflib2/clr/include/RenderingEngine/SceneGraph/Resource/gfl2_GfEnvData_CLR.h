#ifndef GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_RESOURCE_GFENVDATA_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_RESOURCE_GFENVDATA_H_INCLUDED

#include <clr/include/RenderingEngine/SceneGraph/Resource/gfl2_IResourceData_CLR.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfEnvData.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace resource {

public ref class GfEnvData : public IResourceData
{
public:
	GfEnvData(array<c8>^ pData);
	GfEnvData(System::String^ filePath);
	~GfEnvData(){this->!GfEnvData();}
	!GfEnvData();

	virtual gfl2::renderingengine::scenegraph::resource::IResourceData* GetNative(){return m_pNative;}

private:
	gfl2::renderingengine::scenegraph::resource::IResourceData* m_pNative;
};

}}}}}

#endif