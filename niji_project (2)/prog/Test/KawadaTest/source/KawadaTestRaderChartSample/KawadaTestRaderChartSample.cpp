//==============================================================================
/**
 @file    KawadaTestRaderChartSample.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.08
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


// gfl2のインクルード
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include <gfx/include/gfl2_GFGL.h>

#include <math/include/gfl2_math.h>
#include <fs/include/gfl2_Fs.h>

// poke_3dのインクルード
#include <model/include/gfl2_BaseModel.h>

// nijiのインクルード
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <System/include/GflUse.h>
#include <App/RaderChart/include/app_tool_RaderChart.h>
#include <App/RaderChart/include/app_tool_StatusRaderChart.h>
#include <App/RaderChart/include/app_tool_BoxRaderChart.h>
#include <App/RaderChart/include/app_tool_RaderChartRenderPath.h>

// KawadaTestのインクルード
#include "KawadaTestRaderChartSample.h"





GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


//! @brief  テスト用のレンダリングパイプライン
class KawadaTestRaderChartRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestRaderChartRenderingPipeLine);

public:
  //! @brief  コンストラクタ
  KawadaTestRaderChartRenderingPipeLine(app::util::Heap* pHeap)
    : gfl2::renderingengine::renderer::RenderingPipeLine(),
      m_SceneRenderPath(),
      m_RaderChartRenderPath(pHeap),
      m_PresentPath(),
      m_RenderPathCnt(0)
  {
    SceneRenderPath::InitDescription desc;
    m_SceneRenderPath.Initialize( pHeap->GetDevGLHeapAllocator(), desc );
  }

  virtual ~KawadaTestRaderChartRenderingPipeLine()
  {}

private:
  virtual b32 StartRenderPath()
  {
    m_RenderPathCnt = 0;
    return true;
  }

  virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath()
  {
    switch( m_RenderPathCnt ){
    case 0:
      //return &m_SceneRenderPath;
      return NULL;
      //returnするのでbreak要らない
    case 1:
      return &m_RaderChartRenderPath;
      //returnするのでbreak要らない
    case 2:
      //return &m_PresentPath;
      break;
    }
    return NULL;
  }

  virtual b32 NextRenderPath()
  {
    m_RenderPathCnt++;
    if ( m_RenderPathCnt == 3 )
      return false;
    return true;
  }

public:
  app::tool::RaderChartRenderPath* GetRaderChartRenderPath(void) { return &m_RaderChartRenderPath; }

private:
  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath   m_SceneRenderPath;
  app::tool::RaderChartRenderPath                                       m_RaderChartRenderPath;
  gfl2::renderingengine::renderer::RenderingPipeLine::PresentPath       m_PresentPath;
  u32                                                                   m_RenderPathCnt;
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


KawadaTestRaderChartSample::KawadaTestRaderChartSample(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem)
  : KawadaTestSampleBase(gameManager, parentHeapMem, parentDevMem),
    m_step(STEP_NONE),
    m_stepCount(0),
    m_appHeap(NULL),
#if   KAWADA_TEST_RADER_CHART_TEST_VERSION == KAWADA_TEST_RADER_CHART_TEST_VERSION_MAKE_PIPE_LINE
    m_renderingPipeLine(NULL),
    m_raderChart(NULL)
#elif KAWADA_TEST_RADER_CHART_TEST_VERSION == KAWADA_TEST_RADER_CHART_TEST_VERSION_USE_APP_RENDERING_MANAGER
    m_appRenderingManager(NULL),
    m_statusRaderChart(NULL)
#endif  // KAWADA_TEST_RADER_CHART_TEST_VERSION
{
  this->initialize(gameManager, parentHeapMem, parentDevMem);
}
KawadaTestRaderChartSample::~KawadaTestRaderChartSample()
{
  this->terminate();
}

void KawadaTestRaderChartSample::Update(void)
{
#if   KAWADA_TEST_RADER_CHART_TEST_VERSION == KAWADA_TEST_RADER_CHART_TEST_VERSION_MAKE_PIPE_LINE
  if(m_raderChart) m_raderChart->Update();
#elif KAWADA_TEST_RADER_CHART_TEST_VERSION == KAWADA_TEST_RADER_CHART_TEST_VERSION_USE_APP_RENDERING_MANAGER
  if(m_statusRaderChart) m_statusRaderChart->Update();
  if(m_appRenderingManager) m_appRenderingManager->Update();
#endif  // KAWADA_TEST_RADER_CHART_TEST_VERSION

  switch(m_step)
  {
  case STEP_NONE:    this->updateFunc_None();    break;
  case STEP_CREATE:  this->updateFunc_Create();  break;
  case STEP_WORKING: this->updateFunc_Working(); break;
  case STEP_DESTROY: this->updateFunc_Destroy(); break;
  }
}

void KawadaTestRaderChartSample::PreDraw(void)
{
#if   KAWADA_TEST_RADER_CHART_TEST_VERSION == KAWADA_TEST_RADER_CHART_TEST_VERSION_MAKE_PIPE_LINE
  if(m_step == STEP_WORKING)
  {
    // 何もしない
  }
#elif KAWADA_TEST_RADER_CHART_TEST_VERSION == KAWADA_TEST_RADER_CHART_TEST_VERSION_USE_APP_RENDERING_MANAGER 
  if(m_step == STEP_WORKING)
  {
    m_appRenderingManager->PreDraw();
  }
#endif  // KAWADA_TEST_RADER_CHART_TEST_VERSION
}

void KawadaTestRaderChartSample::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
#if   KAWADA_TEST_RADER_CHART_TEST_VERSION == KAWADA_TEST_RADER_CHART_TEST_VERSION_MAKE_PIPE_LINE
  if(m_step == STEP_WORKING)
  {
    gfl2::gfx::DisplayDeviceHandle displayDeviceHandle = System::GflUse::m_HandleList[displayNo];
    
    gfl2::gfx::Surface* backBuffer         = gfl2::gfx::GFGL::GetBackBuffer(displayDeviceHandle);
    gfl2::gfx::Surface* depthStencilBuffer = gfl2::gfx::GFGL::GetDepthStencilBuffer(displayDeviceHandle);
    s32                 displayWidth       = gfl2::gfx::GFGL::GetDisplayWidth(displayDeviceHandle);
    s32                 displayHeight      = gfl2::gfx::GFGL::GetDisplayHeight(displayDeviceHandle);

#if 0 
    gfl2::math::Matrix44 projMatrix;
    {
      f32 l_rate   = (static_cast<f32>(displayWidth))/(static_cast<f32>(displayHeight));
      f32 l_height = displayHeight;
      //gfl2::math::PivotDirection pivot = gfl2::math::PIVOT_NONE;
      gfl2::math::PivotDirection pivot = gfl2::math::PIVOT_UPSIDE_TO_TOP;
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
        pivot );
      projMatrix = gfl2::math::ConvertNwMtx44ToGfMtx44( l_projMatrix );
    }
    m_renderingPipeLine->SetProjectionMatrix(projMatrix);
 
    //gfl2::math::Vector3 eyePos( 0.0f, 0.0f, 10.0f );   // 視点(カメラの位置)
    //gfl2::math::Vector3 aimPos( 0.0f, 0.0f, 0.0f );    // 注視点
    //gfl2::math::Vector3 upDir( 0.0f, 1.0f, 0.0f );     // カメラの上方向
    //gfl2::math::Matrix34 viewMatrix = gfl2::math::Matrix34::GetLookAt(eyePos, aimPos, upDir);
    gfl2::math::Matrix34 viewMatrix = gfl2::math::Matrix34::GetIdentity();
    m_renderingPipeLine->SetViewMatrix(viewMatrix);
#else
    gfl2::math::Matrix44 projMatrix = gfl2::math::Matrix44::GetIdentity();
    gfl2::math::Matrix34 viewMatrix = gfl2::math::Matrix34::GetIdentity();
    m_renderingPipeLine->SetProjectionMatrix(projMatrix);
    m_renderingPipeLine->SetViewMatrix(viewMatrix);
#endif

    m_renderingPipeLine->SetRenderTarget(backBuffer);
    m_renderingPipeLine->SetDepthStencil(depthStencilBuffer);
    m_renderingPipeLine->SetDisplaySize(displayWidth, displayHeight);
    
    m_renderingPipeLine->Execute();
  }
#elif KAWADA_TEST_RADER_CHART_TEST_VERSION == KAWADA_TEST_RADER_CHART_TEST_VERSION_USE_APP_RENDERING_MANAGER
  if(m_step == STEP_WORKING)
  {
    m_appRenderingManager->Draw(displayNo);
  }
#endif  // KAWADA_TEST_RADER_CHART_TEST_VERSION
}

void KawadaTestRaderChartSample::initialize(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem)
{
  static const s32 HEAP_MEM_SIZE  =  0x100000 - 0x1000;
  static const s32 DEV_MEM_SIZE   = 0x1000000 - 0x1000;
 
  m_enableToEnd = false;
  m_gameManager = gameManager;

  m_heapMem     = GFL_CREATE_LOCAL_HEAP(parentHeapMem, HEAP_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
  m_devMem      = GFL_CREATE_LOCAL_HEAP(parentDevMem, DEV_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
  m_uniMem      = m_devMem;

  // 他はUpdateでつくることにする
  this->changeStep(STEP_CREATE);
}

void KawadaTestRaderChartSample::terminate(void)
{
  if(m_enableToEnd)
  {
    m_uniMem = NULL;
    if(m_devMem)
    {
      GFL_DELETE_HEAP(m_devMem);
      m_devMem = NULL;
    }
    if(m_heapMem)
    {
      GFL_DELETE_HEAP(m_heapMem);
      m_heapMem = NULL;
    }

    m_gameManager = NULL;
  }
  else
  {
    GFL_ASSERT(0);
  }
}

void KawadaTestRaderChartSample::changeStep(Step step)
{
  m_step      = step;
  m_stepCount = 0;
}

void KawadaTestRaderChartSample::updateFunc_None(void)
{}
void KawadaTestRaderChartSample::updateFunc_Create(void)
{
  gfl2::fs::AsyncFileManager* romAsyncFileManager = m_gameManager->GetAsyncFileManager();

#if   KAWADA_TEST_RADER_CHART_TEST_VERSION == KAWADA_TEST_RADER_CHART_TEST_VERSION_MAKE_PIPE_LINE
  static const s32 APP_HEAP_HEAP_MEM_SIZE  =  0x100000 - 0x1000 - 0x2000;
  static const s32 APP_HEAP_DEV_MEM_SIZE   = 0x1000000 - 0x1000 - 0x2000;
  
  m_appHeap = GFL_NEW(m_uniMem) app::util::Heap();
  m_appHeap->LocalHeapCreate(m_heapMem, m_devMem, APP_HEAP_HEAP_MEM_SIZE, APP_HEAP_DEV_MEM_SIZE);
  
  m_renderingPipeLine = GFL_NEW(m_appHeap->GetDeviceHeap()) KawadaTestRaderChartRenderingPipeLine(m_appHeap);

  m_raderChart = GFL_NEW(m_appHeap->GetDeviceHeap()) app::tool::RaderChart();
  m_raderChart->SyncCreate(m_appHeap, romAsyncFileManager);
  m_renderingPipeLine->GetRaderChartRenderPath()->SetRaderChart(m_raderChart);

  {
    gfl2::math::Vector2 pos(100.0f, 0.0f);
    m_raderChart->SetPos(pos);
    
    gfl2::math::Vector4 insideColor(1.0f, 0.0f, 0.0f, 1.0f);
    gfl2::math::Vector4 outsideColor(0.0f, 1.0f, 0.0f, 1.0f);
    m_raderChart->SetColor(insideColor, outsideColor);

    f32 insideRadius  =  99.0f;
    f32 outsideRadius = 100.0f;
    m_raderChart->SetRadius(insideRadius, outsideRadius);

    app::tool::RaderChart::Figure fig = app::tool::RaderChart::FIGURE_INSIDE;
    u32 item = 0;
    f32 val = 1.0f;
    m_raderChart->SetVal(fig, item, val);
  }

  // 描画時は必ず行う更新
  this->updateForDraw();
  
  // 次へ
  this->changeStep(STEP_WORKING);
#elif KAWADA_TEST_RADER_CHART_TEST_VERSION == KAWADA_TEST_RADER_CHART_TEST_VERSION_USE_APP_RENDERING_MANAGER 
  switch(m_stepCount)
  {
  case 0:
    {
      static const s32 APP_HEAP_HEAP_MEM_SIZE  =  0x100000 - 0x1000 - 0x2000;
      static const s32 APP_HEAP_DEV_MEM_SIZE   = 0x1000000 - 0x1000 - 0x2000;
      
      m_appHeap = GFL_NEW(m_uniMem) app::util::Heap();
      m_appHeap->LocalHeapCreate(m_heapMem, m_devMem, APP_HEAP_HEAP_MEM_SIZE, APP_HEAP_DEV_MEM_SIZE);
    
      m_appRenderingManager = GFL_NEW(m_appHeap->GetDeviceHeap()) app::util::AppRenderingManager();
      app::util::AppRenderingManager::Description desc;
      desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT_RADERCHART;
      m_appRenderingManager->StartAsyncCreate(
          desc,
          m_appHeap,
          NULL,
          romAsyncFileManager
      );
 
      ++m_stepCount;
      break;
    }
  case 1:
    {
      if(m_appRenderingManager->IsCreated())
      {
        m_statusRaderChart = GFL_NEW(m_appHeap->GetDeviceHeap()) app::tool::StatusRaderChart();
        m_statusRaderChart->StartAsyncCreate(m_appHeap, romAsyncFileManager);
 
        ++m_stepCount;
      }
      break;
    }
  case 2:
    {
      if(m_statusRaderChart->IsCreated())
      {
        m_appRenderingManager->SetRaderChart(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_statusRaderChart);
       
        {
          gfl2::math::Vector2 pos(50.0f, 0.0f);
          m_statusRaderChart->SetPos(pos);
          
          m_statusRaderChart->SetStatusMode(app::tool::StatusRaderChart::STATUS_MODE_AMA);
          
          gfl2::math::Vector4 insideColor(1.0f, 0.0f, 0.0f, 0.5f);
          gfl2::math::Vector4 outsideColor(0.0f, 1.0f, 0.0f, 1.0f);
          gfl2::math::Vector4 amaColor(0.0f, 0.0f, 1.0f, 1.0f);
          m_statusRaderChart->SetStatusColor(insideColor, outsideColor, amaColor);

          f32 outsideRadius = 100.0f;
          m_statusRaderChart->SetStatusRadius(outsideRadius);
  
          static const u32 shuzokuchi[6] =
          {
             20,
             40,
             60,
             80,
            100,
            120,
          };
          static const u32 doryokuchi[6] =
          {
            240,
            200,
            160,
              0,
              0,
              0,
          };
          static const u32 nouryokuchi[6] =
          {
            300,
            400,
            500,
              0,
             50,
            100,
          };

          app::tool::StatusRaderChart::StatusFigure fig = app::tool::StatusRaderChart::STATUS_FIGURE_PRO_INSIDE;
          for(u32 i=0; i<6; ++i)
          {
            m_statusRaderChart->SetStatusVal(fig, i, shuzokuchi[i]);
          }
          fig = app::tool::StatusRaderChart::STATUS_FIGURE_PRO_OUTSIDE;
          for(u32 i=0; i<6; ++i)
          {
            m_statusRaderChart->SetStatusVal(fig, i, doryokuchi[i]);
          }
          fig = app::tool::StatusRaderChart::STATUS_FIGURE_AMA;
          for(u32 i=0; i<6; ++i)
          {
            m_statusRaderChart->SetStatusVal(fig, i, nouryokuchi[i]);
          }
        }

        ++m_stepCount;
      }
      break;
    case 3:
      {
        // 描画時は必ず行う更新
        this->updateForDraw();
        
        // 次へ
        this->changeStep(STEP_WORKING);
        
        break;
      }
    }
  }
#endif  // KAWADA_TEST_RADER_CHART_TEST_VERSION
}
void KawadaTestRaderChartSample::updateFunc_Working(void)
{
  const gfl2::ui::Button*       button   = m_gameManager->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  /*const gfl2::ui::VectorDevice* crossKey = m_gameManager->GetUiDeviceManager()->GetCrossKey(gfl2::ui::DeviceManager::CROSSKEY_STANDARD);*/
 
  // 描画時は必ず行う更新
  this->updateForDraw();

  if( button->IsTrigger(gfl2::ui::BUTTON_B) )
  {
    // 次へ
    this->changeStep(STEP_DESTROY);
  }
}
void KawadaTestRaderChartSample::updateFunc_Destroy(void)
{
  if(m_stepCount == 0)
  {
    // @todo 非表示にして、1フレ遅れ解放に対応させたい。
#if   KAWADA_TEST_RADER_CHART_TEST_VERSION == KAWADA_TEST_RADER_CHART_TEST_VERSION_MAKE_PIPE_LINE
    m_renderingPipeLine->GetRaderChartRenderPath()->UnsetRaderChart();
#elif KAWADA_TEST_RADER_CHART_TEST_VERSION == KAWADA_TEST_RADER_CHART_TEST_VERSION_USE_APP_RENDERING_MANAGER 
    m_appRenderingManager->UnsetRaderChart(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY);
#endif  // KAWADA_TEST_RADER_CHART_TEST_VERSION
    ++m_stepCount;
  }
  else
  {
    // 破棄
#if   KAWADA_TEST_RADER_CHART_TEST_VERSION == KAWADA_TEST_RADER_CHART_TEST_VERSION_MAKE_PIPE_LINE
    if(m_raderChart->CanDestroy())
    {
      GFL_SAFE_DELETE(m_raderChart);
    }
    GFL_SAFE_DELETE(m_renderingPipeLine);
#elif KAWADA_TEST_RADER_CHART_TEST_VERSION == KAWADA_TEST_RADER_CHART_TEST_VERSION_USE_APP_RENDERING_MANAGER
    if(m_statusRaderChart->CanDestroy())
    {
      GFL_SAFE_DELETE(m_statusRaderChart);
    }
    else
    {
      GFL_ASSERT(0);
    }
    if(m_appRenderingManager->CanDestroy())
    {
      GFL_SAFE_DELETE(m_appRenderingManager);
    }
    else
    {
      GFL_ASSERT(0);
    }
#endif  // KAWADA_TEST_RADER_CHART_TEST_VERSION
    GFL_SAFE_DELETE(m_appHeap);

    // 次へ
    m_enableToEnd = true;
    this->changeStep(STEP_NONE);
  }
}

void KawadaTestRaderChartSample::updateForDraw(void)
{
#if   KAWADA_TEST_RADER_CHART_TEST_VERSION == KAWADA_TEST_RADER_CHART_TEST_VERSION_MAKE_PIPE_LINE
  {
    // ワールド行列更新、この中でシーングラフを辿ってローカル行列からワールド行列を生成しています
    // シーン全体で一回呼ぶだけで大丈夫です
    gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModel();  // これがないと描画されないので忘れないように。TraverseModelFastもある。
  }
#elif KAWADA_TEST_RADER_CHART_TEST_VERSION == KAWADA_TEST_RADER_CHART_TEST_VERSION_USE_APP_RENDERING_MANAGER 
  // 特にやることはない
#endif  // KAWADA_TEST_RADER_CHART_TEST_VERSION
}


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG

