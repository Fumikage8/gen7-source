#ifndef GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_RESOURCE_GFMOTDATA_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_RESOURCE_GFMOTDATA_H_INCLUDED

#include <clr/include/RenderingEngine/SceneGraph/Resource/gfl2_IResourceData_CLR.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace resource {

public ref class GfBinaryMotData : public IResourceData
{
public:
	GfBinaryMotData(array<u8>^ pData);
	~GfBinaryMotData(){this->!GfBinaryMotData();}
	!GfBinaryMotData();

	virtual gfl2::renderingengine::scenegraph::resource::IResourceData* GetNative(){return m_pNative;}

private:
	gfl2::renderingengine::scenegraph::resource::GfBinaryMotData* m_pNative;
  u8* m_pData;
};

}}}}}

#endif