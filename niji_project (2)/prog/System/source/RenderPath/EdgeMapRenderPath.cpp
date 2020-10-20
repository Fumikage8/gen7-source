//======================================================================
/**
 * @file  EdgeMapRenderPath.cpp
 * @brief 15/05/12
 * @author  ariizumi
 * @data  エッジマップの描画パス
 */
//======================================================================

#include <fs/include/gfl2_BinLinkerAccessor.h>

#include "System/include/RenderPath/EdgeMapRenderPath.h"


GFL_NAMESPACE_BEGIN(System)
NijiEdgeMapSceneRenderPath::NijiEdgeMapSceneRenderPath(gfl2::gfx::IGLAllocator *allocator,int modelNum)
:poke_3d::renderer::EdgeMapSceneRenderPath(allocator,modelNum)
,mGlAllocator(allocator)
{
}
NijiEdgeMapSceneRenderPath::~NijiEdgeMapSceneRenderPath()
{
}

void NijiEdgeMapSceneRenderPath::SetResource(void *dataBuf)
{
  gfl2::fs::BinLinkerAccessor binData;
  binData.Initialize(dataBuf);

#if defined(GF_PLATFORM_CTR)
  c8* shaderArr[3] = 
  {
    static_cast<c8*>(binData.GetData(3)),
    static_cast<c8*>(binData.GetData(4)),
    static_cast<c8*>(binData.GetData(5))
  };
  c8* texArr[2] = 
  {
    static_cast<c8*>(binData.GetData(1)),
    static_cast<c8*>(binData.GetData(2))
  };
#elif defined(GF_PLATFORM_WIN32)
  c8* shaderArr[2] = 
  {
    static_cast<c8*>(binData.GetData(3)),
    static_cast<c8*>(binData.GetData(4))
  };
  c8* texArr[2] = 
  {
    static_cast<c8*>(binData.GetData(1)),
    static_cast<c8*>(binData.GetData(2))
  };

#endif  //プラットフォーム分岐
  poke_3d::renderer::EdgeMapSceneRenderPath::LoadResources(mGlAllocator,static_cast<c8*>(binData.GetData(0)),shaderArr,GFL_NELEMS(shaderArr),texArr,GFL_NELEMS(texArr));

}

GFL_NAMESPACE_END(System)
