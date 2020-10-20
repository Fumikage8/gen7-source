//==============================================================================
/**
 @file    gfl2_FsAsyncFileManager.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   非同期ファイル読み込み書き込みマネージャ
 */
//==============================================================================


// gfl2のインクルード
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// fsのインクルード
#include <fs/include/gfl2_FsNameTable.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


NameTable::NameTable(void)
  : m_buf(NULL),
    m_nameTable(NULL),
    m_nameNum(0)
{}
NameTable::~NameTable()
{
  DestroyTable();
}

void NameTable::SyncCreateTable(
    AsyncFileManager*                    fileManager,
    const char*                          fileName,        
    gfl2::heap::HeapBase*                heapForBuf,      
    gfl2::heap::HeapBase*                heapForReq,      
    gfl2::heap::HeapBase*                heapForCompressed,
    AsyncFileManager::HioUseSetting::Tag hioUseSetting
)
{ 
  Result res;
  void*  buf;

  AsyncFileManager::FileEasyReadReq req;
  req.fileName          = fileName;
  req.ppBuf             = &buf;
  req.heapForBuf        = heapForBuf;
  req.align             = FILE_ALIGN;
  req.heapForReq        = heapForReq;
  req.heapForCompressed = heapForCompressed;
  req.hioUseSetting     = hioUseSetting;
  req.result            = &res;

  fileManager->SyncFileEasyRead(req);

  if(res.IsSuccess())
  {
    this->createTable(heapForBuf, buf);
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付くためのASSERT
    m_buf = NULL;
  }
}

void NameTable::DestroyTable(void)
{
  void* buf = m_buf;
  this->destroyTable();
  GflHeapSafeFreeMemory(buf);
}

void NameTable::CreateTable(gfl2::heap::HeapBase* heap, void* buf)
{
  this->createTable(heap, buf);
}

void NameTable::createTable(gfl2::heap::HeapBase* heap, void* buf)
{
  m_buf = buf;
  
  u8* bufArray = static_cast<u8*>(m_buf);
  u32 byteCount = 0;
  
  /*u32 signature = *(static_cast<u32*>(static_cast<void*>(&bufArray[byteCount])));*/  byteCount += 4;
  /*u16 version   = *(static_cast<u16*>(static_cast<void*>(&bufArray[byteCount])));*/  byteCount += 2;
  
  u16 num       = *(static_cast<u16*>(static_cast<void*>(&bufArray[byteCount])));  byteCount += 2;
  m_nameNum = num; 

  m_nameTable = GFL_NEW_ARRAY(heap) char*[m_nameNum];
  
  for(u16 i=0; i<num; ++i)
  {
    u32 startByteCount = *(static_cast<u32*>(static_cast<void*>(&bufArray[byteCount + 4*i])));
    m_nameTable[i] = static_cast<char*>(static_cast<void*>(&bufArray[startByteCount]));
  }

  /*
  // 確認
  for(u16 i=0; i<num; ++i)
  {
    GFL_PRINT("[%5d] %s\n", i, m_nameTable[i]);
  }
  */
}
void NameTable::destroyTable(void)
{
  m_nameNum = 0;
  GFL_SAFE_DELETE_ARRAY(m_nameTable);
  m_buf = NULL;
}


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)

