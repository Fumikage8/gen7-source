//======================================================================
/**
 * @file  OutLinePostRenderPath.h
 * @brief 15/05/12
 * @author  ariizumi
 * @data  エッジマップの描画パス
 */
//======================================================================

#ifndef __OUT_LINE_POST_RENDER_PATH__
#define __OUT_LINE_POST_RENDER_PATH__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <renderer/include/gfl2_OutLinePostRenderPath.h>

GFL_NAMESPACE_BEGIN(System)

class NijiOutLinePostSceneRenderPath : public poke_3d::renderer::OutLinePostRenderPath
{
public:
  NijiOutLinePostSceneRenderPath(gfl2::gfx::IGLAllocator *allocator);
  virtual ~NijiOutLinePostSceneRenderPath();

  //ARCID_RENDERPATHのGARC_NijiRenderPath_OutLineResource_BINを渡してください。
  void SetResource(void *dataBuf);
private:
  gfl2::gfx::IGLAllocator *mGlAllocator;
};

GFL_NAMESPACE_END(System)

#endif  //__OUT_LINE_POST_RENDER_PATH__