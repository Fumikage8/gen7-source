//======================================================================
/**
 * @file  OutLinePostRenderPath.cpp
 * @brief 15/05/12
 * @author  ariizumi
 * @data  エッジマップの描画パス
 */
//======================================================================

#include <fs/include/gfl2_BinLinkerAccessor.h>

#include "System/include/RenderPath/OutLinePostRenderPath.h"


////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////
////////    以下のクラスがこのクラスを継承しています。
////////    app::util::AppOutLinePostRenderPath
////////    このSystem::NijiOutLinePostSceneRenderPathクラスを変更した際は
////////    上記のクラスに影響がないか必ず確認して下さい。
////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN(System)
NijiOutLinePostSceneRenderPath::NijiOutLinePostSceneRenderPath(gfl2::gfx::IGLAllocator *allocator)
:poke_3d::renderer::OutLinePostRenderPath()
,mGlAllocator(allocator)
{
}
NijiOutLinePostSceneRenderPath::~NijiOutLinePostSceneRenderPath()
{
}

void NijiOutLinePostSceneRenderPath::SetResource(void *dataBuf)
{
  gfl2::fs::BinLinkerAccessor binData;
  binData.Initialize(dataBuf);

#if defined(GF_PLATFORM_CTR)
  c8* mdlBin = static_cast<c8*>(binData.GetData(0));
  c8* shaderArr[1] = 
  {
    static_cast<c8*>(binData.GetData(2)),
  };
  c8* texArr[1] = 
  {
    static_cast<c8*>(binData.GetData(1))
  };
  //IDエッジ用
  c8* mdlBinId = static_cast<c8*>(binData.GetData(3));
  c8* shaderArrId[3] = 
  {
    static_cast<c8*>(binData.GetData(5)),
    static_cast<c8*>(binData.GetData(6)),
    static_cast<c8*>(binData.GetData(7)),
  };
  c8* texArrId[1] = 
  {
    static_cast<c8*>(binData.GetData(4))
  };


#elif defined(GF_PLATFORM_WIN32)
  c8* mdlBin = static_cast<c8*>(binData.GetData(0));
  c8* shaderArr[2] = 
  {
    static_cast<c8*>(binData.GetData(2)),
    static_cast<c8*>(binData.GetData(3))
  };
  c8* texArr[1] = 
  {
    static_cast<c8*>(binData.GetData(1))
  };
  //IDエッジ用
  c8* mdlBinId = static_cast<c8*>(binData.GetData(4));
  c8* shaderArrId[4] = 
  {
    static_cast<c8*>(binData.GetData(6)),
    static_cast<c8*>(binData.GetData(7)),
    static_cast<c8*>(binData.GetData(8)),
    static_cast<c8*>(binData.GetData(9)),
  };
  c8* texArrId[1] = 
  {
    static_cast<c8*>(binData.GetData(5))
  };

#endif  //プラットフォーム分岐
  poke_3d::renderer::OutLinePostRenderPath::Initialize(
      mGlAllocator,
      mdlBin,
      shaderArr,GFL_NELEMS(shaderArr),
      texArr[0],
      mdlBinId,
      shaderArrId,GFL_NELEMS(shaderArrId),
      texArrId[0]
      );

}

GFL_NAMESPACE_END(System)
