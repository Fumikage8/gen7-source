#if !defined(NIJI_PROJECT_APPLIB_APP_TOOL_RADERCHARTRENDERPATH_H_INCLUDED)
#define NIJI_PROJECT_APPLIB_APP_TOOL_RADERCHARTRENDERPATH_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    app_tool_RaderChart.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.07
 @brief   レーダーチャート
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <math/include/gfl2_Vector2.h>

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
  class AsyncFileManager;
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(renderingengine)
GFL_NAMESPACE_BEGIN(scenegraph)
GFL_NAMESPACE_BEGIN(resource)
  class ResourceNode;
GFL_NAMESPACE_END(resource)
GFL_NAMESPACE_END(scenegraph)
GFL_NAMESPACE_END(renderingengine)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(model)
  class BaseModel;
GFL_NAMESPACE_END(model)
GFL_NAMESPACE_END(poke_3d)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
  class RaderChart;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)








GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)


//==============================================================================
/**
 @class     RaderChartRenderPath
 @brief     レーダーチャート描画パス
 */
//==============================================================================
class RaderChartRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
  GFL_FORBID_COPY_AND_ASSIGN(RaderChartRenderPath);

public:
  //! @brief  コンストラクタ
  //! @caution  別途Create関数を呼んで生成して下さい。
  RaderChartRenderPath(void);
  
  //! @brief  コンストラクタ
  //! @param[in]  pHeap  生成に使用するヒープ。m_pDevHeap, m_pDevHeapAllocator, m_pDevGLHeapAllocatorしか使っていない。
  //! @caution  このコンストラクタ内からCreate関数は呼ばれるので、別途Create関数を呼ぶ必要はありません。
  RaderChartRenderPath(app::util::Heap* pHeap);

  virtual ~RaderChartRenderPath();

  virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

  virtual const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride() { return &m_DrawableNodeContainer; }
  virtual void ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil ) {}  // クリアしないように何もしない
  virtual const gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* CameraOverride() { return &m_Camera; }

public:
  //! @brief  生成
  //! @param[in]  pHeap  生成に使用するヒープ。m_pDevHeap, m_pDevHeapAllocator, m_pDevGLHeapAllocatorしか使っていない。
  void Create(app::util::Heap* pHeap);

  //! @brief  描画するレーダーチャートをセットする
  //! @note  アンセットする場合は(NULLをセットする場合は)Unset...関数を呼んで下さい。
  void SetRaderChart(RaderChart* raderChart);
  //! @brief  レーダーチャートをアンセットする
  void UnsetRaderChart(void);

private:
  gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*> m_DrawableNodeContainer;
  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera                 m_Camera;

private:
  bool        m_isCreated;
  RaderChart* m_raderChart;
};


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APPLIB_APP_TOOL_RADERCHARTRENDERPATH_H_INCLUDED

