//======================================================================
/**
 * @file PokeViewerDataManager.cpp
 * @date 2015/12/01 9:37:19
 * @author araki_syo
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include "../include/PokeViewerDataManager.h"
#include "System/include/ArcIdGetter.h"
#include "arc_def_index/arc_def.h"
#include "arc_index/PokeViewerLyt.gaix"

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(pokeviewer)

/**
 * @fn PokeViewerDataManager
 * @brief ctor
 */
PokeViewerDataManager::PokeViewerDataManager(app::util::Heap* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager)
: m_pHeap(pHeap),
  m_pAsyncFileManager(pAsyncFileManager),
  m_State(STATE_STOP),
  m_Seq(SEQ_START_OPEN),
  m_StepCounter(0),
  m_DataID(DATA_ID_POKEVIEWER_RES2D),
  m_DataLoadBitFlag(0)
{
  for (u32 i = 0; i < DATA_ID_NUM; ++i)
  {
    m_pDataBuf[i] = NULL;
  }
}

/**
 * @fn ~PokeViewerDataManager
 * @brief dtor
 */
PokeViewerDataManager::~PokeViewerDataManager()
{
  GFL_ASSERT_MSG(this->IsDisposable(), "data manager is not disposable yet.");

  for(u32 i = 0; i < DATA_ID_NUM; ++i)
  {
    GflHeapSafeFreeMemory(m_pDataBuf[i]);
  }
}

b32 PokeViewerDataManager::IsDisposable() const
{
  return (m_State == STATE_END);
}

void PokeViewerDataManager::Update()
{
  s32 totalLoopCount = 100;  // 無限ループ対策
  s32 loopCount = 1;
  while(totalLoopCount > 0 && loopCount > 0)
  {
    Seq prevSeq = m_Seq;

    switch(m_Seq)
    {
    case SEQ_START_OPEN:
      this->Update_StartOpen();
      break;
    case SEQ_WAIT_OPEN:
      this->Update_WaitOpen();
      break;
    case SEQ_START_LOAD:
      this->Update_StartLoad();
      break;
    case SEQ_WAIT_LOAD:
      this->Update_WaitLoad();
      break;
    case SEQ_START_CLOSE:
      this->Update_StartClose();
      break;
    case SEQ_WAIT_CLOSE:
      this->Update_WaitClose();
      break;
    case SEQ_END:
    default:
      this->Update_End();
      break;
    }

    if(prevSeq == m_Seq) { --loopCount;      }
    else                 { --totalLoopCount; }
  }
}

void PokeViewerDataManager::Start()
{
  if( m_State == STATE_STOP || m_State == STATE_WORK_WITH_STOP_REQ )
  {
    m_State = STATE_WORK;
  }
}

void PokeViewerDataManager::Stop()
{
  if(m_State == STATE_WORK)
  {
    m_State = STATE_WORK_WITH_STOP_REQ;
  }
}

void* PokeViewerDataManager::GetDataBuf(DataID dataId) const
{
  if( m_DataLoadBitFlag & (1 << dataId) )
  {
    return m_pDataBuf[dataId];
  }
  else
  {
    return NULL;
  }
}

void PokeViewerDataManager::ChangeSeq(Seq seq)
{
  m_Seq = seq;
  m_StepCounter = 0;
}

void PokeViewerDataManager::Update_StartOpen(void)
{
  gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
  req.arcId       = NIJI_ARCID_NUM(ARCID_POKE_VIEWER);
  req.heapForFile = m_pHeap->GetDeviceHeap();
  req.heapForReq  = m_pHeap->GetDeviceHeap()->GetLowerHandle();
  m_pAsyncFileManager->AddArcFileOpenReq(req);
  this->ChangeSeq(SEQ_WAIT_OPEN);
}

void PokeViewerDataManager::Update_WaitOpen(void)
{
  if( m_pAsyncFileManager->IsArcFileOpenFinished( NIJI_ARCID_NUM(ARCID_POKE_VIEWER) ) )
  {
    this->ChangeSeq(SEQ_START_LOAD);
  }
}

void PokeViewerDataManager::Update_StartLoad(void)
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId             = NIJI_ARCID_NUM(ARCID_POKE_VIEWER);
  req.datId             = GARC_PokeViewerLyt_down1_applyt_COMP;
  req.ppBuf             = &m_pDataBuf[m_DataID];
  req.heapForBuf        = m_pHeap->GetDeviceHeap();
  req.align             = 128;
  req.heapForReq        = m_pHeap->GetDeviceHeap()->GetLowerHandle();
  req.heapForCompressed = m_pHeap->GetDeviceHeap()->GetLowerHandle();
  m_pAsyncFileManager->AddArcFileLoadDataReq(req);
  this->ChangeSeq(SEQ_WAIT_LOAD);
}

void PokeViewerDataManager::Update_WaitLoad(void)
{
  if( m_pAsyncFileManager->IsArcFileLoadDataFinished( &m_pDataBuf[m_DataID] ) )
  {
    m_DataLoadBitFlag |= (1 << m_DataID);
    m_DataID = static_cast<DataID>( (static_cast<s32>(m_DataID)) +1 );
    if(m_DataID == DATA_ID_END)
    {
      this->ChangeSeq(SEQ_START_CLOSE);
    }
  }
}

void PokeViewerDataManager::Update_StartClose(void)
{
  gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
  req.arcId      = NIJI_ARCID_NUM(ARCID_POKE_VIEWER);
  req.heapForReq = m_pHeap->GetDeviceHeap()->GetLowerHandle();
  m_pAsyncFileManager->AddArcFileCloseReq(req);
  this->ChangeSeq(SEQ_WAIT_CLOSE);
}

void PokeViewerDataManager::Update_WaitClose(void)
{
  if( m_pAsyncFileManager->IsArcFileCloseFinished( NIJI_ARCID_NUM(ARCID_POKE_VIEWER) ) )
  {
    this->ChangeSeq(SEQ_END);
    m_State = STATE_END;
  }
}

void PokeViewerDataManager::Update_End(void)
{
  // 何もしない
}

GFL_NAMESPACE_END(pokeviewer)
GFL_NAMESPACE_END(test)

#endif // PM_DEBUG
