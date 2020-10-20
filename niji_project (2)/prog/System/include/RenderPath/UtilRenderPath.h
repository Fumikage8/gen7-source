//======================================================================
/**
 * @file UtilRenderPath.h
 * @date 2016/03/09 20:52:58
 * @author ariizumi_nobuhiko
 * @brief 汎用レンダーパス群
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=====================================================================

#if !defined __UTIL_RENDER_PATH_H__
#define __UTIL_RENDER_PATH_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <renderingengine/include/renderer/gfl2_RenderingPipeLine.h>

GFL_NAMESPACE_BEGIN(System)

  //デプスバッファクリアパス
class ClearDepthRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:
  ClearDepthRenderPath(){};
  ~ClearDepthRenderPath(){};
  void Execute(const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext);

private:
};

GFL_NAMESPACE_END(System)

#endif // __UTIL_RENDER_PATH_H__