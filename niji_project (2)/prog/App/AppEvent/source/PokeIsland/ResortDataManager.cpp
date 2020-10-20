//==============================================================================
/**
 @file    ResortDataManager.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2016.04.11
 @brief   ポケリゾートのデータマネージャ
 */
//==============================================================================

// gfl2のインクルード
#include <thread/include/gfl2_ThreadStatic.h>
#include <base/include/gfl2_Singleton.h>

// nijiのインクルード
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <GameSys/include/GameManager.h>

// データのヘッダ
#include <niji_conv_header/app/resort/resort_pack.h>
#include <arc_def_index/arc_def.h>
#include <arc_index/resort.gaix>
#include <arc_index/PokeIcon.gaix>

// ポケリゾートのインクルード
#include "App/AppEvent/include/PokeIsland/ResortDataManager.h"
#include <App/Resort/include/ResortHeapDefine.h>

//#include "ResortDebugInfo.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)


ResortDataManager* ResortDataManager::s_instance = NULL;

void ResortDataManager::SetStaticInstance(ResortDataManager* instance)
{
  GFL_ASSERT(s_instance == NULL);  // 開発中に気付かせるためのASSERT
  GFL_ASSERT(instance != NULL);  // 開発中に気付かせるためのASSERT
  s_instance = instance;
}
void ResortDataManager::UnsetStaticInstance(void)
{
  GFL_ASSERT(s_instance != NULL);  // 開発中に気付かせるためのASSERT
  s_instance = NULL;
}
ResortDataManager* ResortDataManager::GetStaticInstance(void)
{
  return s_instance;
}


ResortDataManager::ResortDataManager(void)
  : m_created(false),
    m_devMem(NULL),
    m_uncompWorkMem(NULL),
    m_waitUpdateCount(0),
    m_fileOp(0),
    m_subSeq(0),
    m_releaseStopFlag(false)
{
  this->init();
}

ResortDataManager::~ResortDataManager()
{
  this->Destroy();
}

void ResortDataManager::Create(gfl2::heap::HeapBase* parentDevMem)
{
  if(m_created)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。既に生成済み。
    return;
  }

  this->init();

  static const u32 RESORT_DATA_MEM_SIZE = ResortHeapDefine::RESORT_FILE_SIZE + ResortHeapDefine::POKEICON_FILE_SIZE;
  m_devMem = GFL_CREATE_LOCAL_HEAP(parentDevMem, RESORT_DATA_MEM_SIZE,  gfl2::heap::HEAP_TYPE_EXP, false);
  m_uncompWorkMem = GFL_CREATE_LOCAL_HEAP(parentDevMem, ResortHeapDefine::UNCOMP_WORK_SIZE,  gfl2::heap::HEAP_TYPE_EXP, false);
  m_created = true;
}
void ResortDataManager::SetWaitUpdateCount(u32 count)
{
  m_waitUpdateCount = count;
}
void ResortDataManager::Destroy(void)
{
  if(!m_created) return;

  if(!this->CanDestroy())
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。CanDestroyがtrueを返すまでUpdateを呼んで下さい。
  }

  this->ReleaseStop();
  while( !this->CanDestroy() )
  {
    this->Update();
    gfl2::thread::ThreadStatic::CurrentSleep(1);
  }

  for(s32 i=0; i<BUF_NUM; ++i)
  {
    GflHeapSafeFreeMemory(m_buf[i]);
  }

  GFL_DELETE_HEAP(m_uncompWorkMem);
  m_uncompWorkMem = NULL;
  GFL_DELETE_HEAP(m_devMem);
  m_devMem = NULL;

  this->init();
}
bool ResortDataManager::CanDestroy(void) const
{
  if(!m_created) return true;

  if(m_fileOp >= FILE_OP_END)
  {
    return true;
  }

  return false;
}

void ResortDataManager::ReleaseStop(void)
{
  if(!m_created) return;
  m_releaseStopFlag = true;
}
void ResortDataManager::Update(void)
{
  if(!m_created) return;

  gfl2::fs::AsyncFileManager* fileMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

  if(m_waitUpdateCount > 0)
  {
    --m_waitUpdateCount;
    return;
  }

  bool continueFlag = false;
  do
  {
    continueFlag = false;
    switch(m_fileOp)
    {
    case FILE_OP_RESORT_OPEN:
      {
        switch(m_subSeq)
        {
        case 0:
//#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
//          ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_DATA_MANAGER_START);
//#endif
          app::util::FileAccessor::FileOpen(ARCID_RESORT, m_devMem);
          ++m_subSeq;
          continueFlag = true;
          break;
        case 1:
          if( app::util::FileAccessor::IsFileOpen(ARCID_RESORT) )
          {
            this->changeFileOp(FILE_OP_RESORT_PACK_LOAD);
            continueFlag = true;
          }
          break;
        }
        break;
      }
    case FILE_OP_RESORT_PACK_LOAD:
      {
        switch(m_subSeq)
        {
        case 0:
          {
            gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
            {
              req.arcId             = ARCID_RESORT;
              req.datId             = GARC_resort_resort_pack_pack_COMP;
              req.ppBuf             = &m_buf[BUF_RESORT_PACK];
              req.heapForBuf        = m_devMem;
              req.align             = 128;
              req.heapForReq        = m_devMem->GetLowerHandle();
              req.heapForCompressed = m_uncompWorkMem;
            }
            fileMan->AddArcFileLoadDataReq(req);

            ++m_subSeq;
            continueFlag = true;
          }
          break;
        case 1:
          if( app::util::FileAccessor::IsFileLoad(&m_buf[BUF_RESORT_PACK]) )
          {
            // パックに入れる
            m_pack[PACK_RESORT].Initialize( m_buf[PACK_RESORT] );
            m_pack[PACK_ISLANDBIN].Initialize( m_pack[PACK_RESORT].GetData(BL_IDX_RESORT_PACK_ISLANDBIN_PACK_PACK) );
            m_pack[PACK_ISLANDSHADOW].Initialize( m_pack[PACK_RESORT].GetData(BL_IDX_RESORT_PACK_ISLANDSHADOW_PACK_PACK) );
            m_pack[PACK_ISLANDGROUND].Initialize( m_pack[PACK_RESORT].GetData(BL_IDX_RESORT_PACK_ISLANDGROUND_PACK_PACK) );
//#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
//          ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_DATA_MANAGER_RESORT_PACK_LOAD_END);
//#endif

            this->changeFileOp(FILE_OP_POKEICON_OPEN_LOAD);
            continueFlag = true;
          }
          break;
        }
        break;
      }
    case FILE_OP_POKEICON_OPEN_LOAD:
      {
        switch(m_subSeq)
        {
        case 0:
          app::util::FileAccessor::FileOpen(ARCID_POKEICON_3D, m_devMem);
          ++m_subSeq;
          continueFlag = true;
          break;
        case 1:
          if( app::util::FileAccessor::IsFileOpen(ARCID_POKEICON_3D) )
          {
            m_arcFile[ARC_FILE_POKEICON] = fileMan->GetArcFile(ARCID_POKEICON_3D);

            {
              gfl2::fs::ArcFile::ARCDATID startDatId = 0;
              gfl2::fs::ArcFile::ARCDATID endDatId   = GARC_PokeIcon_DATA_NUM -1;

              size_t startOffsetFromArchiveTop = 0;
              m_arcFile[ARC_FILE_POKEICON]->GetOffsetFromArchiveTop(&startOffsetFromArchiveTop, startDatId);
              
              size_t endOffsetFromArchiveTop = 0;
              m_arcFile[ARC_FILE_POKEICON]->GetOffsetFromArchiveTop(&endOffsetFromArchiveTop, endDatId);
              size_t endDataSize = 0;
              m_arcFile[ARC_FILE_POKEICON]->GetDataSize(&endDataSize, endDatId, m_devMem->GetLowerHandle());  // 一気読みしているので、ArcSrcを使いたくない。fs側で防ぐフラグがないのでこのままにしておく。
              
              size_t readSize = endOffsetFromArchiveTop + endDataSize - startOffsetFromArchiveTop;
              m_buf[BUF_POKEICON] = GflHeapAllocMemoryAlign(m_devMem, readSize, 128);
              
              gfl2::fs::AsyncFileManager::ArcFileLoadDataPieceBufReq  req;
              req.arcId             = ARCID_POKEICON_3D;
              req.datId             = startDatId;
              req.readSize          = readSize;
              req.pBuf              = m_buf[BUF_POKEICON];
              req.bufSize           = readSize;
              req.heapForReq        = m_devMem->GetLowerHandle();
              fileMan->AddArcFileLoadDataPieceBufReq(req);
            } 

            ++m_subSeq;
            continueFlag = true;
          }
          break;
        case 2:
          if( fileMan->IsArcFileLoadDataPieceBufFinished(m_buf[BUF_POKEICON]) )
          {
//#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
//          ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_DATA_MANAGER_POKEICON_OPEN_LOAD_END);
//#endif

            this->changeFileOp(FILE_OP_STOP);
            continueFlag = true;
          }
          break;
        }
        break;
      }
    case FILE_OP_STOP:
      {
        if(m_releaseStopFlag)
        {
          this->changeFileOp(FILE_OP_CLOSE);
          continueFlag = true;
        }
        break;
      }
    case FILE_OP_CLOSE:
      {
        switch(m_subSeq)
        {
        case 0:
          app::util::FileAccessor::FileClose(ARCID_POKEICON_3D, m_devMem);
          app::util::FileAccessor::FileClose(ARCID_RESORT, m_devMem);
          ++m_subSeq;
          continueFlag = true;
          break;
        case 1:
          if(    app::util::FileAccessor::IsFileClose(ARCID_POKEICON_3D)
              && app::util::FileAccessor::IsFileClose(ARCID_RESORT) )
          {
            this->changeFileOp(FILE_OP_END);
            continueFlag = true;
          }
          break;
        }
        break;
      }
    case FILE_OP_END:
    default:
      {
        // 終了している。何もしない。
        break;
      }
    }
  }
  while(continueFlag);
}

gfl2::heap::HeapBase* ResortDataManager::GetUncompWorkHeap(void) const
{
  if(!m_created) return NULL;

  if(FILE_OP_POKEICON_OPEN_LOAD <= m_fileOp && m_fileOp <= FILE_OP_END)
  {
    return m_uncompWorkMem;
  }
  else
  {
    return NULL;
  }
}

bool ResortDataManager::IsFileAccessible(s32 fileId) const
{
  if(!m_created) return NULL;
 
  bool isAccessible = false;

  if(fileId == FILE_ID_RESORT)
  {
    if(FILE_OP_RESORT_PACK_LOAD <= m_fileOp && m_fileOp <= FILE_OP_STOP)
    {
      isAccessible = true;
    }
  }
  else if(fileId == FILE_ID_POKEICON)
  {
    if(m_fileOp == FILE_OP_STOP)
    {
      isAccessible = true;
    }
  }

  return isAccessible;
}
const gfl2::fs::ArcFile* ResortDataManager::GetArcFile(s32 fileId) const
{
  if(!m_created) return NULL;

  const gfl2::fs::ArcFile* arcFile = NULL;

  if(fileId == FILE_ID_POKEICON)
  {
    if(m_fileOp == FILE_OP_STOP)
    {
      arcFile = m_arcFile[ARC_FILE_POKEICON];
    }
  }

  return arcFile;
}

void* ResortDataManager::GetData(s32 dataId) const
{
  if(!m_created) return NULL;

  void* data = NULL;

  switch(dataId)
  {
  case DATA_RESORT_APPLYT:
    {
      if(FILE_OP_RESORT_PACK_LOAD <= m_fileOp && m_fileOp <= FILE_OP_STOP) data = m_pack[PACK_RESORT].GetData(BL_IDX_RESORT_PACK_RESORT_APPLYT);
      break;
    }
  case DATA_FIELDWINDOW_APPLYT:
    {
      if(FILE_OP_RESORT_PACK_LOAD <= m_fileOp && m_fileOp <= FILE_OP_STOP) data = m_pack[PACK_RESORT].GetData(BL_IDX_RESORT_PACK_FIELDWINDOW_APPLYT);
      break;
    }
  case DATA_ISLANDBIN_PACK:
    {
      if(FILE_OP_RESORT_PACK_LOAD <= m_fileOp && m_fileOp <= FILE_OP_STOP) data = m_pack[PACK_RESORT].GetData(BL_IDX_RESORT_PACK_ISLANDBIN_PACK_PACK);
      break;
    }
  case DATA_ISLANDSHADOW_PACK:
    {
      if(FILE_OP_RESORT_PACK_LOAD <= m_fileOp && m_fileOp <= FILE_OP_STOP) data = m_pack[PACK_RESORT].GetData(BL_IDX_RESORT_PACK_ISLANDSHADOW_PACK_PACK);
      break;
    }
  case DATA_ISLANDGROUND_PACK:
    {
      if(FILE_OP_RESORT_PACK_LOAD <= m_fileOp && m_fileOp <= FILE_OP_STOP) data = m_pack[PACK_RESORT].GetData(BL_IDX_RESORT_PACK_ISLANDGROUND_PACK_PACK);
      break;
    }
  case DATA_POKEICON:
    {
      if(m_fileOp == FILE_OP_STOP) data = m_buf[BUF_POKEICON];
      break;
    }
  }

  return data;
}

void ResortDataManager::init(void)
{
  m_created = false;
  m_devMem = NULL;
  m_uncompWorkMem = NULL;
  m_waitUpdateCount = 0;
  m_fileOp = 0;
  m_subSeq = 0;
  m_releaseStopFlag = false;
  for(s32 i=0; i<BUF_NUM; ++i)
  {
    m_buf[i] = NULL;
  }
  for(s32 i=0; i<PACK_NUM; ++i)
  {
    m_pack[i].Initialize(NULL);  // NULLでクリアして初期化
  }
  for(s32 i=0; i<ARC_FILE_NUM; ++i)
  {
    m_arcFile[i] = NULL;
  }
}
void ResortDataManager::changeFileOp(s32 fileOp)
{
  m_fileOp = fileOp;
  m_subSeq = 0;
}


GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)

