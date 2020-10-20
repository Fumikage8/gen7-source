#ifndef GFLIB2_RENDERINGENGINE_RENDERER_ISHADERDRIVER_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_RENDERER_ISHADERDRIVER_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <RenderingEngine/include/renderer/gfl2_RenderState.h>

namespace gfl2 { namespace renderingengine { namespace renderer { 

class MeshDrawTag;

class IShaderDriver
{
public:

	virtual b32 Apply( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag = NULL ) const = 0;
	virtual void SetTextureOverride( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag ) const 
  {
    (void)rRenderState;
    (void)pMeshDrawTag;
  }
	virtual b32 SetPolygonDrawOverride( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag ) const 
  {
    (void)rRenderState;
    (void)pMeshDrawTag;
    return false;
  }
	virtual ~IShaderDriver(){}
protected:
	IShaderDriver(){}
};

}}}

#endif

