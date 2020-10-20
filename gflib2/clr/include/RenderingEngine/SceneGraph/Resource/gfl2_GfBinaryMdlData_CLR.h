#if !defined(GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_RESOURCE_GFBINARYMDLDATA_H_INCLUDED)
#define GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_RESOURCE_GFBINARYMDLDATA_H_INCLUDED

#include <clr/include/RenderingEngine/SceneGraph/Resource/gfl2_IResourceData_CLR.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace resource {

public ref class GfBinaryMdlData : public IResourceData
{
public:
  GfBinaryMdlData(array<u8>^ pData);
  ~GfBinaryMdlData(){this->!GfBinaryMdlData();}
  !GfBinaryMdlData();

  virtual gfl2::renderingengine::scenegraph::resource::IResourceData* GetNative(){return m_pNative;}

private:
	gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData* m_pNative;
  u8* m_pData;
};

}}}}}

#endif