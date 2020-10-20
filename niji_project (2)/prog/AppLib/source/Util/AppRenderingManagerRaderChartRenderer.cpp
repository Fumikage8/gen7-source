//==============================================================================
/**
 @file    AppRenderingManagerRaderChartRenderer.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.08
 @brief   KawadaTest
 */
//==============================================================================


// gfl2のインクルード
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>

// nijiのインクルード
#include <AppLib/include/Util/app_util_heap.h>
#include <App/RaderChart/include/app_tool_RaderChartRenderPath.h>

#include "AppRenderingManagerRaderChartRenderer.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
GFL_NAMESPACE_BEGIN(app_rendering_manager_internal) 


RaderChartRenderingPipeLine::RaderChartRenderingPipeLine(app::util::Heap* pHeap)
  : gfl2::renderingengine::renderer::RenderingPipeLine(),
    m_RaderChartRenderPath(NULL),
    m_RenderPathCnt(0)
{
  m_RaderChartRenderPath = GFL_NEW( pHeap->GetDeviceHeap() ) app::tool::RaderChartRenderPath(pHeap);
}

RaderChartRenderingPipeLine::~RaderChartRenderingPipeLine()
{
  GFL_SAFE_DELETE(m_RaderChartRenderPath);
}


b32 RaderChartRenderingPipeLine::StartRenderPath()
{
  m_RenderPathCnt = 0;
  return true;
}

gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* RaderChartRenderingPipeLine::GetRenderPath()
{
  switch( m_RenderPathCnt ){
  case 0:
    return m_RaderChartRenderPath;
    //returnするからbreak要らない
  }
  return NULL;
}

b32 RaderChartRenderingPipeLine::NextRenderPath()
{
  m_RenderPathCnt++;
  if ( m_RenderPathCnt == 1 )
    return false;
  return true;
}

app::tool::RaderChartRenderPath* RaderChartRenderingPipeLine::GetRaderChartRenderPath(void)
{
  return m_RaderChartRenderPath;
}


GFL_NAMESPACE_END(app_rendering_manager_internal) 
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

