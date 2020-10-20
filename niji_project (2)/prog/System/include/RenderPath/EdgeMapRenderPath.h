//======================================================================
/**
 * @file  EdgeMapRenderPath.h
 * @brief 15/05/12
 * @author  ariizumi
 * @data  エッジマップの描画パス
 */
//======================================================================

#ifndef __EDGE_MAP_RENDER_PATH__
#define __EDGE_MAP_RENDER_PATH__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <renderer/include/gfl2_EdgeMapSceneRenderPath.h>

GFL_NAMESPACE_BEGIN(System)

class NijiEdgeMapSceneRenderPath : public poke_3d::renderer::EdgeMapSceneRenderPath
{
public:
  NijiEdgeMapSceneRenderPath(gfl2::gfx::IGLAllocator *allocator,int modelNum);
  virtual ~NijiEdgeMapSceneRenderPath();

  //ARCID_RENDERPATHのGARC_NijiRenderPath_EdgeMapResource_BINを渡してください。
  void SetResource(void *dataBuf);
private:
  gfl2::gfx::IGLAllocator *mGlAllocator;
};

GFL_NAMESPACE_END(System)

#endif  //__EDGE_MAP_RENDER_PATH__