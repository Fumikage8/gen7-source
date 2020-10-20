#if !defined(NIJI_PROJECT_TEST_KAWADATESTRADERCHARTSAMPLE_H_INCLUDED)
#define NIJI_PROJECT_TEST_KAWADATESTRADERCHARTSAMPLE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    KawadaTestRaderChartSample.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.08
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG




// テスト内容の切り替え
#define KAWADA_TEST_RADER_CHART_TEST_VERSION_MAKE_PIPE_LINE            (0)
#define KAWADA_TEST_RADER_CHART_TEST_VERSION_USE_APP_RENDERING_MANAGER (1)
#define KAWADA_TEST_RADER_CHART_TEST_VERSION                           (KAWADA_TEST_RADER_CHART_TEST_VERSION_USE_APP_RENDERING_MANAGER)




// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameManager.h>

// KawadaTestのインクルード
#include "../KawadaTestSampleBase.h"




// 前方宣言
GFL_NAMESPACE_BEGIN(app)
  GFL_NAMESPACE_BEGIN(util)
    class Heap;
    class AppRenderingManager;
  GFL_NAMESPACE_END(util)
  GFL_NAMESPACE_BEGIN(tool)
    class RaderChart;
    class StatusRaderChart;
    class BoxRaderChart;
  GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


// 前方宣言
class KawadaTestRaderChartRenderingPipeLine;


//==============================================================================
/**
 @class     KawadaTestRaderChartSample
 @brief     KawadaTest
 */
//==============================================================================
class KawadaTestRaderChartSample : public KawadaTestSampleBase
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestRaderChartSample);

  ////////////////////////////////////////////////////////////////////////////////
public:
  KawadaTestRaderChartSample(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem);
  virtual ~KawadaTestRaderChartSample();

  virtual void Update(void);
  virtual void PreDraw(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);

private:
  virtual void initialize(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem);
  virtual void terminate(void);

private:
  enum Step
  {
    STEP_NONE,
    STEP_CREATE,
    STEP_WORKING,
    STEP_DESTROY,
  };

private:
  void changeStep(Step step);
  void updateFunc_None(void);
  void updateFunc_Create(void);
  void updateFunc_Working(void);
  void updateFunc_Destroy(void);
  void updateForDraw(void);

private:
  Step m_step;
  u32  m_stepCount;

  ////////////////////////////////////////////////////////////////////////////////
private:
  app::util::Heap*                            m_appHeap;

#if   KAWADA_TEST_RADER_CHART_TEST_VERSION == KAWADA_TEST_RADER_CHART_TEST_VERSION_MAKE_PIPE_LINE
  KawadaTestRaderChartRenderingPipeLine*      m_renderingPipeLine;
  app::tool::RaderChart*                      m_raderChart;
#elif KAWADA_TEST_RADER_CHART_TEST_VERSION == KAWADA_TEST_RADER_CHART_TEST_VERSION_USE_APP_RENDERING_MANAGER
  app::util::AppRenderingManager*             m_appRenderingManager;
  app::tool::StatusRaderChart*                m_statusRaderChart;
#endif  // KAWADA_TEST_RADER_CHART_TEST_VERSION
};


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG


#endif  // NIJI_PROJECT_TEST_KAWADATESTRADERCHARTSAMPLE_H_INCLUDED

