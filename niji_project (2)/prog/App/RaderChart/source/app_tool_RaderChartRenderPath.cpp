//==============================================================================
/**
 @file    app_tool_RaderChart.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.07
 @brief   レーダーチャート
 */
//==============================================================================


// gfl2のインクルード
#include <debug/include/gfl2_DebugPrint.h>
#include <math/include/gfl2_math.h>
#include <fs/include/gfl2_Fs.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

// poke_3d_libのインクルード
#include <model/include/gfl2_BaseModel.h>

// nijiのインクルード
#include <AppLib/include/Util/app_util_heap.h>

#include <App/RaderChart/include/app_tool_RaderChart.h>
#include <App/RaderChart/include/app_tool_RaderChartRenderPath.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)


//==============================================================================
//
//     RaderChartRenderPath
//
//==============================================================================
RaderChartRenderPath::RaderChartRenderPath(void)
  : gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath(),
    m_isCreated(false),
    m_raderChart(NULL)
{
  m_Camera.m_Projection = gfl2::math::Matrix44::GetIdentity();
  m_Camera.m_View       = gfl2::math::Matrix34::GetIdentity();
}

RaderChartRenderPath::RaderChartRenderPath(app::util::Heap* pHeap)
  : gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath(),
    m_isCreated(false),
    m_raderChart(NULL)
{
  m_Camera.m_Projection = gfl2::math::Matrix44::GetIdentity();
  m_Camera.m_View       = gfl2::math::Matrix34::GetIdentity();

  this->Create(pHeap);
}

RaderChartRenderPath::~RaderChartRenderPath()
{
}

void RaderChartRenderPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
  if(m_isCreated)
  {
    u32 displayWidth  = rDrawContext.m_DisplayWidth;
    u32 displayHeight = rDrawContext.m_DisplayHeight;
    
    // m_Cameraを更新する
    {
      // proj
      {
        f32 l_rate   = (static_cast<f32>(displayWidth))/(static_cast<f32>(displayHeight));
        f32 l_height = displayHeight;
        //gfl2::math::PivotDirection pivot = gfl2::math::PIVOT_NONE;
        gfl2::math::PivotDirection pivot = gfl2::math::PIVOT_UPSIDE_TO_TOP;
        //gfl2::math::PivotDirection pivot = gfl2::math::PIVOT_UPSIDE_TO_LEFT;
        gfl2::math::MTX44 l_projMatrix;
        gfl2::math::MTX44OrthoPivot(
          &l_projMatrix,
          l_height * (-0.5f) * l_rate,
          l_height * ( 0.5f) * l_rate,
          l_height * (-0.5f),
          l_height * ( 0.5f),
          //1.1f,
          0.0f,
          1000.0f,
          pivot
        );
        m_Camera.m_Projection = gfl2::math::ConvertNwMtx44ToGfMtx44( l_projMatrix );
      }
      // view
      {
        //gfl2::math::Vector3 eyePos( 0.0f, 0.0f, 10.0f );   // 視点(カメラの位置)
        //gfl2::math::Vector3 aimPos( 0.0f, 0.0f, 0.0f );    // 注視点
        //gfl2::math::Vector3 upDir( 0.0f, 1.0f, 0.0f );     // カメラの上方向
        //m_Camera.m_View = gfl2::math::Matrix34::GetLookAt(eyePos, aimPos, upDir);
        m_Camera.m_View = gfl2::math::Matrix34::GetIdentity();
      }
    }

    // 親のExecuteを呼ぶ
    gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Execute( rDrawContext );
  }
}

void RaderChartRenderPath::Create(app::util::Heap* pHeap)
{
  if(!m_isCreated)
  {
    {
      gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
      gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Initialize( pHeap->GetDevGLHeapAllocator(), desc );
    }
    {
			m_DrawableNodeContainer.CreateBuffer(1,pHeap->GetDevGLHeapAllocator());
    }
    m_isCreated = true;
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。既に生成済み。
  }
}

void RaderChartRenderPath::SetRaderChart(RaderChart* raderChart)
{
  if(m_isCreated)
  {
    if(raderChart)
    {
      m_raderChart = raderChart;
      gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode = m_raderChart->GetModel()->GetModelInstanceNode();
			m_DrawableNodeContainer.push_back(pNode);
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。NULLセットはUnset...関数を呼んで下さい。
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。生成されていない。
  }
}
void RaderChartRenderPath::UnsetRaderChart(void)
{
  if(m_isCreated)
  {
    if(m_raderChart)
    {
      gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode = m_raderChart->GetModel()->GetModelInstanceNode();
      m_DrawableNodeContainer.remove(pNode);
    }
    m_raderChart = NULL;
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。生成されていない。
  }
}


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

