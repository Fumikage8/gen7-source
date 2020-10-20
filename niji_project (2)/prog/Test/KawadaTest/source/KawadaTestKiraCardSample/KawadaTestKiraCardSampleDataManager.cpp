//==============================================================================
/**
 @file    KawadaTestKiraCardSampleDataManager.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.12.23
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
#include <arc_index/kawada_test_data.gaix>

// 図鑑のインクルード
#include "KawadaTestKiraCardSampleDataManager.h"


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


KawadaTestKiraCardSampleDataManager::KawadaTestKiraCardSampleDataManager(app::util::Heap* appHeap, gfl2::fs::AsyncFileManager* romAsyncFileManager)
  : m_appHeap(appHeap),
    m_romAsyncFileManager(romAsyncFileManager),
    m_state(STATE_STOP),
    m_seq(SEQ_START_OPEN),
    m_step(0),
    m_dataIdOfSeqLoad(DATA_ID_KIRA_CARD_TEST_APPLYT),
    m_dataLoadBitFlag(0)
{
  for(s32 i=0; i<DATA_ID_NUM; ++i)
  {
    m_dataBuf[i] = NULL;
  }
}
KawadaTestKiraCardSampleDataManager::~KawadaTestKiraCardSampleDataManager()
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

bool KawadaTestKiraCardSampleDataManager::CanDestroy(void) const
{
  return (m_state == STATE_END);  // @todo 通信エラーが途中で起こり得るなら、中断にも対応できるようにしたい。
}

void KawadaTestKiraCardSampleDataManager::Update(void)
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

void KawadaTestKiraCardSampleDataManager::Start(void)
{
  if(
       m_state == STATE_STOP
    || m_state == STATE_WORK_WITH_STOP_REQ
  )
  {
    m_state = STATE_WORK;
  }
}
void KawadaTestKiraCardSampleDataManager::Stop(void)
{
  if(
       m_state == STATE_WORK
  )
  {
    m_state = STATE_WORK_WITH_STOP_REQ;
  }
}
void* KawadaTestKiraCardSampleDataManager::GetDataBuf(DataId dataId) const
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
void KawadaTestKiraCardSampleDataManager::changeSeq(Seq nextSeq)
{
  m_seq = nextSeq;
  m_step = 0;
}

void KawadaTestKiraCardSampleDataManager::update_StartOpen(void)
{
  gfl2::fs::ArcFile::ARCID    arcId    = gfl2::fs::ArcFile::ARCID_NULL;
  gfl2::fs::ArcFile::ARCDATID arcDatId = gfl2::fs::ArcFile::ARCDATID_NULL;
  this->getArcInfoOfDataId(m_dataIdOfSeqLoad, &arcId, &arcDatId);

  gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
  req.arcId       = arcId;
  req.heapForFile = m_appHeap->GetDeviceHeap();
  req.heapForReq  = m_appHeap->GetDeviceHeap()->GetLowerHandle();
  m_romAsyncFileManager->AddArcFileOpenReq(req);
  this->changeSeq(SEQ_WAIT_OPEN);
}
void KawadaTestKiraCardSampleDataManager::update_WaitOpen(void)
{
  gfl2::fs::ArcFile::ARCID    arcId    = gfl2::fs::ArcFile::ARCID_NULL;
  gfl2::fs::ArcFile::ARCDATID arcDatId = gfl2::fs::ArcFile::ARCDATID_NULL;
  this->getArcInfoOfDataId(m_dataIdOfSeqLoad, &arcId, &arcDatId);
  
  if( m_romAsyncFileManager->IsArcFileOpenFinished(arcId) )  // ファイルマネージャAdd-Finishを確認するなら、ここをわざとコメントアウトしてやれば引っ掛かります。
  {
    this->changeSeq(SEQ_START_LOAD);
  }
}
void KawadaTestKiraCardSampleDataManager::update_StartLoad(void)
{
  gfl2::fs::ArcFile::ARCID    arcId    = gfl2::fs::ArcFile::ARCID_NULL;
  gfl2::fs::ArcFile::ARCDATID arcDatId = gfl2::fs::ArcFile::ARCDATID_NULL;
  this->getArcInfoOfDataId(m_dataIdOfSeqLoad, &arcId, &arcDatId);

  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId             = arcId;
  req.datId             = arcDatId;
  req.ppBuf             = &m_dataBuf[m_dataIdOfSeqLoad];
  req.heapForBuf        = m_appHeap->GetDeviceHeap();
  req.align             = 128;
  req.heapForReq        = m_appHeap->GetDeviceHeap()->GetLowerHandle();
  req.heapForCompressed = m_appHeap->GetDeviceHeap()->GetLowerHandle();
  m_romAsyncFileManager->AddArcFileLoadDataReq(req);
  this->changeSeq(SEQ_WAIT_LOAD);
}
void KawadaTestKiraCardSampleDataManager::update_WaitLoad(void)
{
  if( m_romAsyncFileManager->IsArcFileLoadDataFinished( &m_dataBuf[m_dataIdOfSeqLoad] ) )
  {
    m_dataLoadBitFlag |= (1 << m_dataIdOfSeqLoad);
    this->changeSeq(SEQ_START_CLOSE);
  }
}
void KawadaTestKiraCardSampleDataManager::update_StartClose(void)
{
  gfl2::fs::ArcFile::ARCID    arcId    = gfl2::fs::ArcFile::ARCID_NULL;
  gfl2::fs::ArcFile::ARCDATID arcDatId = gfl2::fs::ArcFile::ARCDATID_NULL;
  this->getArcInfoOfDataId(m_dataIdOfSeqLoad, &arcId, &arcDatId);

  gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
  req.arcId      = arcId;
  req.heapForReq = m_appHeap->GetDeviceHeap()->GetLowerHandle();
  m_romAsyncFileManager->AddArcFileCloseReq(req);
  this->changeSeq(SEQ_WAIT_CLOSE);
}
void KawadaTestKiraCardSampleDataManager::update_WaitClose(void)
{
  if( m_romAsyncFileManager->IsArcFileCloseFinished( NIJI_ARCID_NUM(ARCID_KAWADA_TEST_DATA) ) )
  {
    m_dataIdOfSeqLoad = static_cast<DataId>( (static_cast<s32>(m_dataIdOfSeqLoad)) +1 );
    if(m_dataIdOfSeqLoad == DATA_ID_END)
    {
      this->changeSeq(SEQ_END);
      m_state = STATE_END;
    }
    else
    {
      this->changeSeq(SEQ_START_OPEN);
    }
  }
}
void KawadaTestKiraCardSampleDataManager::update_End(void)
{
  // 何もしない
}

void KawadaTestKiraCardSampleDataManager::getArcInfoOfDataId(const DataId a_dataId, gfl2::fs::ArcFile::ARCID* a_arcId, gfl2::fs::ArcFile::ARCID* a_arcDatId) const
{
  gfl2::fs::ArcFile::ARCID    arcId    = gfl2::fs::ArcFile::ARCID_NULL;
  gfl2::fs::ArcFile::ARCDATID arcDatId = gfl2::fs::ArcFile::ARCDATID_NULL;
  if(a_dataId == DATA_ID_KIRA_CARD_TEST_APPLYT)
  {
    arcId    = NIJI_ARCID_NUM(ARCID_KAWADA_TEST_DATA);
    arcDatId = GARC_kawada_test_data_kira_card_test_applyt_COMP;
  }
  *a_arcId    = arcId;
  *a_arcDatId = arcDatId;
}


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG

