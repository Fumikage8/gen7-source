#if !defined(GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_RESOURCE_MCNFDATA_H_INCLUDED)
#define GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_RESOURCE_MCNFDATA_H_INCLUDED
#pragma once

#include <clr/include/RenderingEngine/SceneGraph/Resource/gfl2_IResourceData_CLR.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_McnfData.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace resource {

public ref class McnfData : public IResourceData
{
public:
	McnfData(array<c8>^ pData);
	McnfData(System::String^ filePath);
	~McnfData(){this->!McnfData();}
	!McnfData();

	virtual gfl2::renderingengine::scenegraph::resource::IResourceData* GetNative(){return m_pNative;}

private:
	gfl2::renderingengine::scenegraph::resource::IResourceData* m_pNative;
};

}}}}}

#endif