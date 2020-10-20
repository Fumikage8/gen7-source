//==============================================================================
/**
 @file    KawadaTestInfoWindowSampleDataManager.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.07.01
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


// nijiのインクルード
#include <AppLib/include/Util/app_util_Heap.h>
#include <System/include/ArcIdGetter.h>
#include <Print/include/PrintSystem.h>

// nijiのデータのヘッダーファイルのインクルード
#include <arc_def_index/arc_def.h>
#include <arc_index/kawada_test_data.gaix>
#include <arc_index/debug_message.gaix>

// このテストのインクルード
#include "KawadaTestInfoWindowSampleDataManager.h"


#define KAWA_ARC_OPEN_TEST (0)


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


KawadaTestInfoWindowSampleDataManager::KawadaTestInfoWindowSampleDataManager(app::util::Heap* appHeap, gfl2::fs::AsyncFileManager* romAsyncFileManager)
  : m_appHeap(appHeap),
    m_romAsyncFileManager(romAsyncFileManager),
    m_state(STATE_STOP),
    m_seq(SEQ_START_OPEN),
    m_step(0),
    m_dataId(DATA_ID_SIMPLE_LAYOUT_TEST_APPLYT),
    m_arcId(gfl2::fs::ArcFile::ARCID_NULL),
    m_arcDatId(gfl2::fs::ArcFile::ARCDATID_NULL),
    m_dataLoadBitFlag(0)
{
  for(s32 i=0; i<DATA_ID_NUM; ++i)
  {
    m_dataBuf[i] = NULL;
  }
}
KawadaTestInfoWindowSampleDataManager::~KawadaTestInfoWindowSampleDataManager()
{
#if KAWA_ARC_OPEN_TEST
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
    req.arcId = NIJI_ARCID_NUM(ARCID_KAWADA_TEST_DATA);;
    m_romAsyncFileManager->SyncArcFileClose(req);
  }
#endif

  if( !(this->CanDestroy()) )
  {
    // 破棄してはいけない状態のとき
    GFL_ASSERT(0);  // 破棄してはいけないときに破棄しようとしている。@todo 非同期のケアができていない！
  }

  // ここまで来たら、破棄してもいい状態になったので破棄する
  for(s32 i=0; i<DATA_ID_NUM; ++i)
  {
    GflHeapSafeFreeMemory(m_dataBuf[i]);
  }
}

bool KawadaTestInfoWindowSampleDataManager::CanDestroy(void) const
{
  return (m_state == STATE_END);  // @todo 通信エラーが途中で起こり得るなら、中断にも対応できるようにしたい。
}

void KawadaTestInfoWindowSampleDataManager::Update(void)
{
  s32 totalLoopCount = 100;  // 無限ループ対策
  s32 loopCount = 1;
  while(totalLoopCount > 0 && loopCount > 0)
  {
    Seq prevSeq = m_seq;

    switch(m_seq)
    {
    case SEQ_START_OPEN:
      this->update_StartOpen();
      break;
    case SEQ_WAIT_OPEN:
      this->update_WaitOpen();
      break;
    case SEQ_START_LOAD:
      this->update_StartLoad();
      break;
    case SEQ_WAIT_LOAD:
      this->update_WaitLoad();
      break;
    case SEQ_START_CLOSE:
      this->update_StartClose();
      break;
    case SEQ_WAIT_CLOSE:
      this->update_WaitClose();
      break;
    case SEQ_END:
    default:
      this->update_End();
      break;
    }

    if(prevSeq == m_seq) { --loopCount;      }
    else                 { --totalLoopCount; }
  }
}

void KawadaTestInfoWindowSampleDataManager::Start(void)
{
  if(
       m_state == STATE_STOP
    || m_state == STATE_WORK_WITH_STOP_REQ
  )
  {
    m_state = STATE_WORK;
  }
}
void KawadaTestInfoWindowSampleDataManager::Stop(void)
{
  if(
       m_state == STATE_WORK
  )
  {
    m_state = STATE_WORK_WITH_STOP_REQ;
  }
}
void* KawadaTestInfoWindowSampleDataManager::GetDataBuf(DataId dataId) const
{
  if( m_dataLoadBitFlag & (1 << dataId) )
  {
    return m_dataBuf[dataId];
  }
  else
  {
    return NULL;
  }
}
void KawadaTestInfoWindowSampleDataManager::changeSeq(Seq nextSeq)
{
  m_seq = nextSeq;
  m_step = 0;
}

void KawadaTestInfoWindowSampleDataManager::update_StartOpen(void)
{
  // オープンするファイル、ロードするファイルを決める
  if(m_dataId == DATA_ID_SIMPLE_LAYOUT_TEST_APPLYT)
  {
    m_arcId    = NIJI_ARCID_NUM(ARCID_KAWADA_TEST_DATA);
    m_arcDatId = GARC_kawada_test_data_simple_layout_test_applyt_COMP;
  }
  else if(m_dataId == DATA_ID_MY_DEBUG_MESSAGE_DAT)
  {
#if PM_DEBUG
    m_arcId    = print::GetMessageArcId_Debug();
    m_arcDatId = GARC_debug_message_d_kawada_DAT;
#endif  // PM_DEBUG
  }

  if(m_arcId != gfl2::fs::ArcFile::ARCID_NULL)
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
    req.arcId       = m_arcId;
    req.heapForFile = m_appHeap->GetDeviceHeap();
    req.heapForReq  = m_appHeap->GetDeviceHeap()->GetLowerHandle();
    m_romAsyncFileManager->AddArcFileOpenReq(req);
    this->changeSeq(SEQ_WAIT_OPEN);
  }
  else
  {
    this->changeSeq(SEQ_END);
    m_state = STATE_END;
  }
}
void KawadaTestInfoWindowSampleDataManager::update_WaitOpen(void)
{
  if( m_romAsyncFileManager->IsArcFileOpenFinished(m_arcId) )
  {
    this->changeSeq(SEQ_START_LOAD);
  }
}
void KawadaTestInfoWindowSampleDataManager::update_StartLoad(void)
{
  u32 align = 4;
  gfl2::heap::HeapBase* heapForCompressed = NULL;
  if(m_dataId == DATA_ID_SIMPLE_LAYOUT_TEST_APPLYT)
  {
    align = 128;
    heapForCompressed = m_appHeap->GetDeviceHeap()->GetLowerHandle();
  }
  else if(m_dataId == DATA_ID_MY_DEBUG_MESSAGE_DAT)
  {
    align = 4;
    heapForCompressed = NULL;
  }

  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId             = m_arcId;
  req.datId             = m_arcDatId;
  req.ppBuf             = &m_dataBuf[m_dataId];
  req.heapForBuf        = m_appHeap->GetDeviceHeap();
  req.align             = align;
  req.heapForReq        = m_appHeap->GetDeviceHeap()->GetLowerHandle();
  req.heapForCompressed = heapForCompressed;
  m_romAsyncFileManager->AddArcFileLoadDataReq(req);
  this->changeSeq(SEQ_WAIT_LOAD);
}
void KawadaTestInfoWindowSampleDataManager::update_WaitLoad(void)
{
  if( m_romAsyncFileManager->IsArcFileLoadDataFinished( &m_dataBuf[m_dataId] ) )
  {
    m_dataLoadBitFlag |= (1 << m_dataId);

    // 次を決める
    if(m_dataId == DATA_ID_SIMPLE_LAYOUT_TEST_APPLYT)
    {
#if KAWA_ARC_OPEN_TEST
      m_dataId = static_cast<DataId>( (static_cast<s32>(m_dataId)) +1 );
      this->changeSeq(SEQ_START_OPEN);
#else
      this->changeSeq(SEQ_START_CLOSE);
#endif
    }
    else if(m_dataId == DATA_ID_MY_DEBUG_MESSAGE_DAT)
    {
      this->changeSeq(SEQ_START_CLOSE);
    }
  }
}
void KawadaTestInfoWindowSampleDataManager::update_StartClose(void)
{
  gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
  req.arcId      = m_arcId;
  req.heapForReq = m_appHeap->GetDeviceHeap()->GetLowerHandle();
  m_romAsyncFileManager->AddArcFileCloseReq(req);
  this->changeSeq(SEQ_WAIT_CLOSE);
}
void KawadaTestInfoWindowSampleDataManager::update_WaitClose(void)
{
  if( m_romAsyncFileManager->IsArcFileCloseFinished(m_arcId) )
  {
    // 次を決める
    if(m_dataId == DATA_ID_SIMPLE_LAYOUT_TEST_APPLYT)
    {
      m_dataId = static_cast<DataId>( (static_cast<s32>(m_dataId)) +1 );
      this->changeSeq(SEQ_START_OPEN);
    }
    else if(m_dataId == DATA_ID_MY_DEBUG_MESSAGE_DAT)
    {
      m_dataId = DATA_ID_END;
      this->changeSeq(SEQ_END);
      m_state = STATE_END;
    }
  }
}
void KawadaTestInfoWindowSampleDataManager::update_End(void)
{
  // 何もしない
}


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG

