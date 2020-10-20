//==============================================================================
/**
 @file    KawadaTestProc.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.04.22
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


// nijiのインクルード
#include <System/include/HeapDefine.h>

#include <Test/KawadaTest/include/KawadaTestProc.h>

// KawadaTestのインクルード
#include "KawadaTestSampleBase.h"




// サンプル

// 外部
#include "KawadaTestModelSample/KawadaTestModelSample.h"
#include "KawadaTestLayoutSample/KawadaTestLayoutSample.h"
#include "KawadaTestRaderChartSample/KawadaTestRaderChartSample.h"
#include "KawadaTestZukanSample/KawadaTestZukanSample.h"
#include "KawadaTestZukanRegisterSample/KawadaTestZukanRegisterSample.h"
#include "KawadaTestPokeModelSample/KawadaTestPokeModelSample.h"
#include "KawadaTestInfoWindowSample/KawadaTestInfoWindowSample.h"
#include "KawadaTestKiraCardSample/KawadaTestKiraCardSample.h"

// 内部
#include "KawadaTestLayoutSample/KawadaTestLayoutSampleMain.h"
#include "KawadaTestPokeModelSample/KawadaTestPokeModelSampleMain.h"
#include "KawadaTestInfoWindowSample/KawadaTestInfoWindowSampleMain.h"
#include "KawadaTestKiraCardSample/KawadaTestKiraCardSampleMain.h"




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


KawadaTestProc::KawadaTestProc(KawadaTestProcParam* param)
  : m_param(),
    m_uniMem(NULL),
    m_heapMem(NULL),
    m_devMem(NULL),
    m_sample(NULL)
{
  // 引数
  if(param)
  {
    m_param = *param;
  }
  // 引数が正しいか確認
  if(m_param.sampleId >= KawadaTestProcParam::SAMPLE_ID_NUM)
  {
    m_param.sampleId = KawadaTestProcParam::SAMPLE_ID_DEFAULT;
  }
}
KawadaTestProc::~KawadaTestProc()
{}

gfl2::proc::Result KawadaTestProc::InitFunc( gfl2::proc::Manager* /*pManager*/ )
{
  static const s32 HEAP_MEM_SIZE =  0x100000;
  static const s32 DEV_MEM_SIZE  = 0x1A00000;

  gfl2::heap::HeapBase* parentHeapMem = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
  m_heapMem = GFL_CREATE_LOCAL_HEAP(parentHeapMem, HEAP_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
  
  gfl2::heap::HeapBase* parentDevMem = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
  m_devMem = GFL_CREATE_LOCAL_HEAP(parentDevMem, DEV_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);

  m_uniMem = m_devMem;

  switch(m_param.sampleId)
  {
  // 外部
  case KawadaTestProcParam::SAMPLE_ID_MODEL:
    m_sample = GFL_NEW(m_uniMem) KawadaTestModelSample(GetGameManager(), m_heapMem, m_devMem);
    break;
  case KawadaTestProcParam::SAMPLE_ID_LAYOUT:
    m_sample = GFL_NEW(m_uniMem) KawadaTestLayoutSample(GetGameManager(), m_heapMem, m_devMem);
    break;
  case KawadaTestProcParam::SAMPLE_ID_RADER_CHART:
    m_sample = GFL_NEW(m_uniMem) KawadaTestRaderChartSample(GetGameManager(), m_heapMem, m_devMem);
    break;
  case KawadaTestProcParam::SAMPLE_ID_ZUKAN:
    m_sample = GFL_NEW(m_uniMem) KawadaTestZukanSample(GetGameManager(), m_heapMem, m_devMem);
    break;
  case KawadaTestProcParam::SAMPLE_ID_ZUKAN_REGISTER:
    m_sample = GFL_NEW(m_uniMem) KawadaTestZukanRegisterSample(GetGameManager(), m_heapMem, m_devMem);
    break;
  case KawadaTestProcParam::SAMPLE_ID_POKEMODEL:
    m_sample = GFL_NEW(m_uniMem) KawadaTestPokeModelSample(GetGameManager(), m_heapMem, m_devMem);
    break;
  case KawadaTestProcParam::SAMPLE_ID_INFO_WINDOW:
    m_sample = GFL_NEW(m_uniMem) KawadaTestInfoWindowSample(GetGameManager(), m_heapMem, m_devMem);
    break;
  case KawadaTestProcParam::SAMPLE_ID_KIRA_CARD:
    m_sample = GFL_NEW(m_uniMem) KawadaTestKiraCardSample(GetGameManager(), m_heapMem, m_devMem);
    break;

  // 内部
  case KawadaTestProcParam::SAMPLE_ID_LAYOUT_MAIN:
    m_sample = GFL_NEW(m_uniMem) KawadaTestLayoutSampleMain(GetGameManager(), m_heapMem, m_devMem);
    break;
  case KawadaTestProcParam::SAMPLE_ID_POKEMODEL_MAIN:
    m_sample = GFL_NEW(m_uniMem) KawadaTestPokeModelSampleMain(GetGameManager(), m_heapMem, m_devMem);
    break;
  case KawadaTestProcParam::SAMPLE_ID_INFO_WINDOW_MAIN:
    m_sample = GFL_NEW(m_uniMem) KawadaTestInfoWindowSampleMain(GetGameManager(), m_heapMem, m_devMem);
    break; 
  case KawadaTestProcParam::SAMPLE_ID_KIRA_CARD_MAIN:
    m_sample = GFL_NEW(m_uniMem) KawadaTestKiraCardSampleMain(GetGameManager(), m_heapMem, m_devMem);
    break; 
  }

  return gfl2::proc::RES_FINISH;
}

gfl2::proc::Result KawadaTestProc::UpdateFunc( gfl2::proc::Manager* /*pManager*/ )
{
  gfl2::proc::Result res = gfl2::proc::RES_FINISH;
  if(m_sample)
  {
    m_sample->Update();
    if(!(m_sample->DoesEnableToEnd()))
    {
      res = gfl2::proc::RES_CONTINUE;
    }
  }
  return res;
}

void KawadaTestProc::PreDrawFunc( gfl2::proc::Manager* /*pManager*/ )
{
  if(m_sample) m_sample->PreDraw();
}

void KawadaTestProc::DrawFunc( gfl2::proc::Manager* /*pManager*/, gfl2::gfx::CtrDisplayNo displayNo )
{
  if(m_sample) m_sample->Draw(displayNo);
}

gfl2::proc::Result KawadaTestProc::EndFunc( gfl2::proc::Manager* /*pManager*/ )
{
  GFL_SAFE_DELETE(m_sample);

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

  return gfl2::proc::RES_FINISH;
}


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG

