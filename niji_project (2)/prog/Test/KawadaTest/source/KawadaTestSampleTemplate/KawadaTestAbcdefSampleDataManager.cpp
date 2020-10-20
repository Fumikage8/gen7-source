//==============================================================================
/**
 @file    KawadaTestAbcdefSampleDataManager.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    0000.00.00
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


// gfl2のインクルード
#include <fs/include/gfl2_Fs.h>

// nijiのインクルード
#include <AppLib/include/Util/app_util_Heap.h>
#include <System/include/ArcIdGetter.h>

// nijiのデータのヘッダーファイルのインクルード
#include <arc_def_index/arc_def.h>
#include <arc_index/ghijkl.gaix>

// 図鑑のインクルード
#include "KawadaTestAbcdefSampleDataManager.h"


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


KawadaTestAbcdefSampleDataManager::KawadaTestAbcdefSampleDataManager(app::util::Heap* appHeap, gfl2::fs::AsyncFileManager* romAsyncFileManager)
  : m_appHeap(appHeap),
    m_romAsyncFileManager(romAsyncFileManager),
    m_state(STATE_STOP),
    m_seq(SEQ_START_OPEN),
    m_step(0),
    m_dataIdOfSeqLoad(DATA_ID_GHIJKL),
    m_dataLoadBitFlag(0)
{
  for(s32 i=0; i<DATA_ID_NUM; ++i)
  {
    m_dataBuf[i] = NULL;
  }
}
KawadaTestAbcdefSampleDataManager::~KawadaTestAbcdefSampleDataManager()
{
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

bool KawadaTestAbcdefSampleDataManager::CanDestroy(void) const
{
  return (m_state == STATE_END);  // @todo 通信エラーが途中で起こり得るなら、中断にも対応できるようにしたい。
}

void KawadaTestAbcdefSampleDataManager::Update(void)
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

void KawadaTestAbcdefSampleDataManager::Start(void)
{
  if(
       m_state == STATE_STOP
    || m_state == STATE_WORK_WITH_STOP_REQ
  )
  {
    m_state = STATE_WORK;
  }
}
void KawadaTestAbcdefSampleDataManager::Stop(void)
{
  if(
       m_state == STATE_WORK
  )
  {
    m_state = STATE_WORK_WITH_STOP_REQ;
  }
}
void* KawadaTestAbcdefSampleDataManager::GetDataBuf(DataId dataId) const
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
void KawadaTestAbcdefSampleDataManager::changeSeq(Seq nextSeq)
{
  m_seq = nextSeq;
  m_step = 0;
}

void KawadaTestAbcdefSampleDataManager::update_StartOpen(void)
{
  gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
  req.arcId       = NIJI_ARCID_NUM(ARCID_GHIJKL);
  req.heapForFile = m_appHeap->GetDeviceHeap();
  req.heapForReq  = m_appHeap->GetDeviceHeap()->GetLowerHandle();
  m_romAsyncFileManager->AddArcFileOpenReq(req);
  this->changeSeq(SEQ_WAIT_OPEN);
}
void KawadaTestAbcdefSampleDataManager::update_WaitOpen(void)
{
  if( m_romAsyncFileManager->IsArcFileOpenFinished( NIJI_ARCID_NUM(ARCID_GHIJKL) ) )
  {
    this->changeSeq(SEQ_START_LOAD);
  }
}
void KawadaTestAbcdefSampleDataManager::update_StartLoad(void)
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId             = NIJI_ARCID_NUM(ARCID_GHIJKL);
  req.datId             = GARC_ghijkl;
  req.ppBuf             = &m_dataBuf[m_dataIdOfSeqLoad];
  req.heapForBuf        = m_appHeap->GetDeviceHeap();
  req.align             = 128;
  req.heapForReq        = m_appHeap->GetDeviceHeap()->GetLowerHandle();
  req.heapForCompressed = m_appHeap->GetDeviceHeap()->GetLowerHandle();
  m_romAsyncFileManager->AddArcFileLoadDataReq(req);
  this->changeSeq(SEQ_WAIT_LOAD);
}
void KawadaTestAbcdefSampleDataManager::update_WaitLoad(void)
{
  if( m_romAsyncFileManager->IsArcFileLoadDataFinished( &m_dataBuf[m_dataIdOfSeqLoad] ) )
  {
    m_dataLoadBitFlag |= (1 << m_dataIdOfSeqLoad);
    m_dataIdOfSeqLoad = static_cast<DataId>( (static_cast<s32>(m_dataIdOfSeqLoad)) +1 );
    if(m_dataIdOfSeqLoad == DATA_ID_END)
    {
      this->changeSeq(SEQ_START_CLOSE);
    }
  }
}
void KawadaTestAbcdefSampleDataManager::update_StartClose(void)
{
  gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
  req.arcId      = NIJI_ARCID_NUM(ARCID_GHIJKL);
  req.heapForReq = m_appHeap->GetDeviceHeap()->GetLowerHandle();
  m_romAsyncFileManager->AddArcFileCloseReq(req);
  this->changeSeq(SEQ_WAIT_CLOSE);
}
void KawadaTestAbcdefSampleDataManager::update_WaitClose(void)
{
  if( m_romAsyncFileManager->IsArcFileCloseFinished( NIJI_ARCID_NUM(ARCID_GHIJKL) ) )
  {
    this->changeSeq(SEQ_END);
    m_state = STATE_END;
  }
}
void KawadaTestAbcdefSampleDataManager::update_End(void)
{
  // 何もしない
}


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG

