#if !defined(NIJI_PROJECT_APPLIB_UTIL_APPRENDERINGMANAGERRADERCHARTRENDERER_H_INCLUDED)
#define NIJI_PROJECT_APPLIB_UTIL_APPRENDERINGMANAGERRADERCHARTRENDERER_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    AppRenderingManagerRaderChartRenderer.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.08
 @brief   KawadaTest
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
  GFL_NAMESPACE_BEGIN(util)
    class Heap;
  GFL_NAMESPACE_END(util)
  GFL_NAMESPACE_BEGIN(tool)
    class RaderChartRenderPath;
  GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
GFL_NAMESPACE_BEGIN(app_rendering_manager_internal) 


//==============================================================================
/**
 @class     RaderChartRenderingPipeLine
 */
//==============================================================================
class RaderChartRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
  GFL_FORBID_COPY_AND_ASSIGN(RaderChartRenderingPipeLine);

public:
  //! @brief  コンストラクタ
  //! @param[in]  pHeap  生成に使用するヒープ。m_pDevHeap, m_pDevHeapAllocator, m_pDevGLHeapAllocatorしか使っていない。
  RaderChartRenderingPipeLine(app::util::Heap* pHeap);

  virtual ~RaderChartRenderingPipeLine();

private:
  virtual b32 StartRenderPath();

  virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath();

  virtual b32 NextRenderPath();

public:
  app::tool::RaderChartRenderPath* GetRaderChartRenderPath(void);

private:
  app::tool::RaderChartRenderPath*  m_RaderChartRenderPath;
  u32                               m_RenderPathCnt;
};


GFL_NAMESPACE_END(app_rendering_manager_internal) 
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APPLIB_UTIL_APPRENDERINGMANAGERRADERCHARTRENDERER_H_INCLUDED

