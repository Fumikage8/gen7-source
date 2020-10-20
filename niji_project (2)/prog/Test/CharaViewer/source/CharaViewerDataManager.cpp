//======================================================================
/**
 * @file CharaViewerDataManager.cpp
 * @date 2015/07/21 16:03:34
 * @author araki_syo
 * @brief CharaViewerのデータマネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include <macro/include/gfl2_Macros.h>

#include <fs/include/gfl2_Fs.h>
#include <AppLib/include/Util/app_util_Heap.h>
#include <System/include/ArcIdGetter.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/CharaViewerLyt.gaix>

//#include <arc_index/zukan_gra.gaix>

#include "../include/CharaViewerDataManager.h"

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(chara_viewer) 

CharaViewerDataManager::CharaViewerDataManager(app::util::Heap* appHeap, gfl2::fs::AsyncFileManager* romAsyncFileManager)
  : m_appHeap(appHeap),
    m_romAsyncFileManager(romAsyncFileManager),
    m_state(STATE_STOP),
    m_seq(SEQ_START_OPEN),
    m_step(0),
    m_dataIdOfSeqLoad(DATA_ID_CHARA_VIEWER_RES2D),
    m_dataLoadBitFlag(0)
{
  for(s32 i=0; i<DATA_ID_NUM; ++i)
  {
    m_dataBuf[i] = NULL;
  }
}
CharaViewerDataManager::~CharaViewerDataManager()
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

bool CharaViewerDataManager::CanDestroy(void) const
{
  return (m_state == STATE_END);  // @todo 通信エラーが途中で起こり得るなら、中断にも対応できるようにしたい。
}

void CharaViewerDataManager::Update(void)
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

void CharaViewerDataManager::Start(void)
{
  if(
       m_state == STATE_STOP
    || m_state == STATE_WORK_WITH_STOP_REQ
  )
  {
    m_state = STATE_WORK;
  }
}

void CharaViewerDataManager::Stop(void)
{
  if(
       m_state == STATE_WORK
  )
  {
    m_state = STATE_WORK_WITH_STOP_REQ;
  }
}
void* CharaViewerDataManager::GetDataBuf(DataId dataId) const
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
void CharaViewerDataManager::changeSeq(Seq nextSeq)
{
  m_seq = nextSeq;
  m_step = 0;
}

void CharaViewerDataManager::update_StartOpen(void)
{
  gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
  req.arcId       = NIJI_ARCID_NUM(ARCID_CHARA_VIEWER);
  req.heapForFile = m_appHeap->GetDeviceHeap();
  req.heapForReq  = m_appHeap->GetDeviceHeap()->GetLowerHandle();
  m_romAsyncFileManager->AddArcFileOpenReq(req);
  this->changeSeq(SEQ_WAIT_OPEN);
}

void CharaViewerDataManager::update_WaitOpen(void)
{
  if( m_romAsyncFileManager->IsArcFileOpenFinished( NIJI_ARCID_NUM(ARCID_CHARA_VIEWER) ) )
  {
    this->changeSeq(SEQ_START_LOAD);
  }
}

void CharaViewerDataManager::update_StartLoad(void)
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId             = NIJI_ARCID_NUM(ARCID_CHARA_VIEWER);
  req.datId             = GARC_CharaViewerLyt_down1_applyt_COMP;
  req.ppBuf             = &m_dataBuf[m_dataIdOfSeqLoad];
  req.heapForBuf        = m_appHeap->GetDeviceHeap();
  req.align             = 128;
  req.heapForReq        = m_appHeap->GetDeviceHeap()->GetLowerHandle();
  req.heapForCompressed = m_appHeap->GetDeviceHeap()->GetLowerHandle();
  m_romAsyncFileManager->AddArcFileLoadDataReq(req);
  this->changeSeq(SEQ_WAIT_LOAD);
}

void CharaViewerDataManager::update_WaitLoad(void)
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

void CharaViewerDataManager::update_StartClose(void)
{
  gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
  req.arcId      = NIJI_ARCID_NUM(ARCID_CHARA_VIEWER);
  req.heapForReq = m_appHeap->GetDeviceHeap()->GetLowerHandle();
  m_romAsyncFileManager->AddArcFileCloseReq(req);
  this->changeSeq(SEQ_WAIT_CLOSE);
}

void CharaViewerDataManager::update_WaitClose(void)
{
  if( m_romAsyncFileManager->IsArcFileCloseFinished( NIJI_ARCID_NUM(ARCID_CHARA_VIEWER) ) )
  {
    this->changeSeq(SEQ_END);
    m_state = STATE_END;
  }
}
void CharaViewerDataManager::update_End(void)
{
  // 何もしない
}


GFL_NAMESPACE_END(chara_viewer)
GFL_NAMESPACE_END(test)

#endif // PM_DEBUG
