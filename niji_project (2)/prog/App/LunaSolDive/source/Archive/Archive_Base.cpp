//======================================================================
/**
 * @file Archive_Base.cpp
 * @date 2016/11/25 13:30:40
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <fs/include/gfl2_FsAsyncFileManager.h>
#include "./Base.h"
#include "../Assert.h"

namespace LunaSolDive{
namespace Archive{
// constructor
Base::Base()
    : m_pHeap(NULL),
      m_pFileManager(NULL),
      m_ArchiveID(0xffffffff),
      m_DataNumber(0),
      m_DataList(NULL),
      m_IsCompressed(false){}
// destructor
Base::~Base(){}
// Initialize
void Base::Initialize(
            gfl2::heap::HeapBase* pHeap,
            gfl2::fs::AsyncFileManager* pFileManager,
            const u32& archiveID,
            const u32& dataNumber,
            const bool& isCompressed)
{
  // set Heap pointer
  m_pHeap = pHeap;
  LUNA_SOL_DIVE_ASSERT(m_pHeap != NULL);
  // set AsyncFileManager pointer
  m_pFileManager = pFileManager;
  LUNA_SOL_DIVE_ASSERT(m_pFileManager != NULL);
  // set parameter
  m_ArchiveID = archiveID;
  m_DataNumber = dataNumber;
  m_IsCompressed = isCompressed;
  // initialize Data List
  m_DataList = GFL_NEW(m_pHeap) void*[m_DataNumber];
  for(u32 i = 0; i < m_DataNumber; ++i)
  {
    m_DataList[i] = NULL;
  }
}
// Terminate
void Base::Terminate()
{
  // delete Data
  if(m_DataList != NULL)
  {
    for(u32 i = 0; i < m_DataNumber; ++i)
    {
      GflHeapSafeFreeMemory(m_DataList[i]);
    }
    GFL_DELETE_ARRAY(m_DataList);
    m_DataList = NULL;
  }
  // reset parmeter
  m_pHeap = NULL;
  m_pFileManager = NULL;
  m_ArchiveID = 0xffffffff;
  m_DataNumber = 0;
}
// Open this archive
void Base::Open() const
{
  LUNA_SOL_DIVE_ASSERT(m_pFileManager != NULL);
  gfl2::fs::AsyncFileManager::ArcFileOpenReq request;
  request.arcId = m_ArchiveID;
  request.heapForFile = m_pHeap->GetLowerHandle();
  request.heapForReq = m_pHeap->GetLowerHandle();
  m_pFileManager->AddArcFileOpenReq(request);
}
// Whether this archive has been opened or not
bool Base::IsOpened() const
{
  LUNA_SOL_DIVE_ASSERT(m_pFileManager != NULL);
  return m_pFileManager->IsArcFileOpenFinished(m_ArchiveID);
}
// Close this archive
void Base::Close() const
{
  LUNA_SOL_DIVE_ASSERT(m_pFileManager != NULL);
  gfl2::fs::AsyncFileManager::ArcFileCloseReq request;
  request.arcId = m_ArchiveID;
  request.heapForReq = m_pHeap->GetLowerHandle();
  m_pFileManager->AddArcFileCloseReq(request);
}
// Whether this archive has been closed or not
bool Base::IsClosed() const
{
  LUNA_SOL_DIVE_ASSERT(m_pFileManager != NULL);
  return m_pFileManager->IsArcFileCloseFinished(m_ArchiveID);
}
// Load data from archive file
void Base::Load(
            const u32& index) const
{
  LUNA_SOL_DIVE_ASSERT(index < m_DataNumber);
  LUNA_SOL_DIVE_ASSERT(m_pFileManager != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pFileManager->IsArcFileOpenFinished(m_ArchiveID));
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq request;
  request.arcId = m_ArchiveID;
  request.datId = index;
  request.align = 128;
  request.heapForBuf = m_pHeap;
  request.heapForReq = m_pHeap->GetLowerHandle();
  request.heapForCompressed =
          m_IsCompressed
          ? m_pHeap->GetLowerHandle()
          : NULL;
  request.ppBuf = &m_DataList[index];
  if(m_IsCompressed)
  {
    m_pFileManager->SyncArcFileLoadData(request);
  }
  else
  {
    m_pFileManager->AddArcFileLoadDataReq(request);
  }
}
// Whether the data has been loaded or not
bool Base::IsLoaded(
            const u32& index) const
{
  LUNA_SOL_DIVE_ASSERT(index < m_DataNumber);
  LUNA_SOL_DIVE_ASSERT(m_pFileManager != NULL);
  return m_pFileManager->IsArcFileLoadDataFinished(&m_DataList[index]);
}
// Load all data from archive file
void Base::LoadAll() const
{
  for(u32 i = 0; i < m_DataNumber; ++i)
  {
    Load(i);
  }
}
// Whether all data have been loaded or not
bool Base::IsAllLoaded() const
{
  for(u32 i = 0; i < m_DataNumber; ++i)
  {
    if(! IsLoaded(i))
    {
      return false;
    }
  }
  return true;
}
// data
void* Base::Data(
            const u32& index) const
{
  LUNA_SOL_DIVE_ASSERT(index < m_DataNumber);
  LUNA_SOL_DIVE_ASSERT(m_DataList[index] != NULL);
  return m_DataList[index];
}
// The Number of Data
const u32& Base::DataNumber() const
{
  return m_DataNumber;
}
}// namespace Archive
}// namespace LunaSolDive
