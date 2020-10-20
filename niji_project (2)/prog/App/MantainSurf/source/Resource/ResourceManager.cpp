//======================================================================
/**
 * @file ResourceManager.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief リソースマネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "ResourceManager.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Resource)

ResourceManager::ResourceManager() :
  m_pHeap(NULL),
  m_ppResourceData(NULL),
  m_arcId(-1),
  m_datNum(0),
  m_isCompressed(false),
  m_align(0),
  m_pFileManager(NULL),
  m_seq(SEQ_NONE)
{
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::Initialize(gfl2::heap::HeapBase* pHeap, s32 arcId, u32 datNum, u32 isCompressed, u32 align)
{
  GFL_ASSERT(arcId >= 0);
  m_pHeap = pHeap;
  m_arcId = arcId;
  m_datNum = datNum;
  m_isCompressed = (isCompressed > 0 ? true : false);
  m_align = align;
  m_pFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
}

void ResourceManager::Terminate()
{
  ReleaseResource();
}

bool ResourceManager::LoadResource()
{
  if(m_seq == SEQ_NONE)
  {
    HOU_PRINT("[LoadResourceData]Started\n");
    m_seq = SEQ_START;
  }

  switch(m_seq)
  {
  case SEQ_START:
    StartOpenArcFile();
    m_seq = SEQ_OPEN;
    /* Through */

  case SEQ_OPEN:
    if(!IsArcFileOpened())
    {
      return false;
    }
    StartLoadResource();
    m_seq = SEQ_LOAD;
    /* Through */

  case SEQ_LOAD:
    if(!IsResourceLoaded())
    {
      return false;
    }
    OnResourceLoadFinished();
    StartCloseArcFile();
    m_seq = SEQ_CLOSE;
    /* Through */

  case SEQ_CLOSE:
    if(!IsArcFileClosed())
    {
      return false;
    }
    m_seq = SEQ_END;
    HOU_PRINT("[LoadResourceData]End\n");
    /* Through */

  case SEQ_END:
    return true;

  default:
    // ありえない
    GFL_ASSERT(0);
    return false;
  }
}

void ResourceManager::StartOpenArcFile()
{
  // ArcFileをオープンする
  gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
  req.arcId = m_arcId;
  req.heapForFile = m_pHeap->GetLowerHandle();
  req.heapForReq = m_pHeap->GetLowerHandle();
  m_pFileManager->AddArcFileOpenReq(req);
}

bool ResourceManager::IsArcFileOpened()
{
  return m_pFileManager->IsArcFileOpenFinished(m_arcId);
}

void ResourceManager::StartLoadResource()
{
  // データをロード
  if(m_ppResourceData == NULL && m_datNum > 0)
  {
    // 管理リスト初期化
    m_ppResourceData = GFL_NEW_ARRAY(m_pHeap) void*[m_datNum];
  }
  for(u32 i = 0; i < m_datNum; i++)
  {
    // バイナリデータを読み込み
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
    req.arcId = m_arcId;
    req.datId = i;
    req.ppBuf = &m_ppResourceData[i];
    req.heapForBuf = m_pHeap;
    req.heapForReq = m_pHeap->GetLowerHandle();
    req.heapForCompressed = m_isCompressed ? m_pHeap->GetLowerHandle() : NULL;
    req.align = m_align;
    m_pFileManager->AddArcFileLoadDataReq(req);
  }
}

bool ResourceManager::IsResourceLoaded()
{
  for(u32 i = 0; i < m_datNum; i++)
  {
    if(!m_pFileManager->IsArcFileLoadDataFinished(&m_ppResourceData[i]))
    {
      return false;
    }
  }
  
  return true;
}

void ResourceManager::StartCloseArcFile()
{
  // ArcFileをクローズする
  gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
  req.arcId = m_arcId;
  req.heapForReq = m_pHeap->GetLowerHandle();
  m_pFileManager->AddArcFileCloseReq(req);
}

bool ResourceManager::IsArcFileClosed()
{
  return m_pFileManager->IsArcFileCloseFinished(m_arcId);
}

void ResourceManager::LoadResourceSync()
{
  gfl2::fs::AsyncFileManager* pFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

  // 管理リスト初期化
  if(m_datNum > 0)
  {
    m_ppResourceData = GFL_NEW_ARRAY(m_pHeap) void*[m_datNum];
  }

  // ArcFileをオープンする
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
    req.arcId       = m_arcId;
    req.heapForFile = m_pHeap->GetLowerHandle();
    req.heapForReq  = m_pHeap->GetLowerHandle();
    pFileManager->SyncArcFileOpen(req);
  }

  // データをロード
  size_t buf_size, read_size;
  for(u32 i = 0; i < m_datNum; i++)
  {
    // バイナリデータを読み込み
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
    {
      req.arcId      = m_arcId;
      req.datId      = i;
      req.ppBuf      = &m_ppResourceData[i];
      req.pBufSize   = &buf_size;
      req.pRealReadSize = &read_size;
      req.heapForBuf = m_pHeap;
      req.heapForReq = m_pHeap->GetLowerHandle();
      req.heapForCompressed = m_isCompressed ? m_pHeap->GetLowerHandle() : NULL;
      req.align      = m_align;
      pFileManager->SyncArcFileLoadData(req);
      HOU_PRINT("[LoadResourceData]id=%d buf_size=%d read_size=%d\n", i, buf_size, read_size);
    }
  }

  // ArcFileをクローズする
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
    req.arcId = m_arcId;
    pFileManager->SyncArcFileClose(req);
  }
}

void ResourceManager::ReleaseResource()
{
  if(m_ppResourceData != NULL)
  {
    for(u32 i = 0; i < m_datNum; i++)
    {
      GflHeapSafeFreeMemory(m_ppResourceData[i]);
    }
  }
  GFL_SAFE_DELETE_ARRAY(m_ppResourceData);
}

s32 ResourceManager::GetResourceNum()
{
  return m_datNum;
}

void* ResourceManager::GetResourceData(s32 resId) const
{
  return m_ppResourceData[resId];
}

void** ResourceManager::GetResourceDataList()
{
  return m_ppResourceData;
}

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
