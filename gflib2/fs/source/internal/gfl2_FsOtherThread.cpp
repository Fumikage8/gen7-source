//==============================================================================
/**
 @file    gfl2_FsOtherThread.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   その他スレッド
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <thread/include/gfl2_ThreadStatic.h>
#include <cx/include/gfl2_StreamingUncomp.h>

// fsのインクルード
#include "gfl2_FsDebug.h"
#include <fs/include/gfl2_FsSystem.h>
#include "../gfl2_FsDeviceBase.h"
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/internal/gfl2_FsElemBase.h>
#include "gfl2_FsOtherThread.h"
#include "gfl2_FsCalc.h"


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


OtherThread::OtherThread(gfl2::heap::HeapBase* heap, AsyncFileManager* manager, int threadPrio)
  : m_thread(NULL),
    m_endReq(false),
    m_hasEnded(false),
    m_manager(manager)
{
  m_thread = GFL_NEW(heap) gfl2::thread::Thread(this, heap, false, DEFAULT_STACK_SIZE );
  m_thread->Start(threadPrio);
}
OtherThread::~OtherThread()
{
  this->RequestEnd();
  u32 endlessLoopCount = 0;
  while(!(this->HasEnded()))
  {
    Calc::SleepInLoop();

    if(++endlessLoopCount >= Define::ENDLESS_LOOP_COUNT_MAX_LONG)
    {
      GFL_ASSERT(0);  // 開発中に無限ループに気付かせるためのASSERT
      break;
    }
  }
  GFL_SAFE_DELETE(m_thread);

  // Functionが終了しているのでKillやIsAliveは使っていません。もしFunctionを終了させずに破棄する場合は
  // m_thread->Kill();
  // while(m_thread->IsAlive())
  // {
  //   Calc::SleepInLoop();
  // }
  // GFL_SAFE_DELETE(m_thread);
  // となる。
}

void OtherThread::Function(void)
{
  for ever
  {
    // (A) リクエストが追加されるまで待つ。
    m_manager->GetUncompListExecEvent()->Wait();
    m_manager->GetUncompListExecEvent()->Clear();

    for ever
    {
      // (1) 展開リストの順番は変更しない。 First In & First Out 。並び替えは必要ない。
      
      // (2) 先頭のリクエストを取得
      ElemBase* reqElem = m_manager->GetUncompListHeadElemAndSetUncompListElemNow();
      
      if(reqElem)
      {
        // (3) リクエストのタイプで分岐
        switch(reqElem->GetElemType())
        {
        case AsyncFileManager::ReqElemType::FILE_EASY_READ_REQ:          this->funcUncompFileEasyRead       ( (static_cast<AsyncFileManager::FileEasyReadReqElem*>       (reqElem))->GetReq() );  break;
        case AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_REQ:      this->funcUncompArcFileLoadData    ( (static_cast<AsyncFileManager::ArcFileLoadDataReqElem*>    (reqElem))->GetReq() );  break;
        case AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ:  this->funcUncompArcFileLoadDataBuf ( (static_cast<AsyncFileManager::ArcFileLoadDataBufReqElem*> (reqElem))->GetReq() );  break;
        default:                                                                                                                                                                                  break;
        }
  
        // (4) 先頭のリクエストを削除
        gfl2::thread::Event* syncReqFinishEvent = NULL;  // DeleteUncompListElemAndUnsetUncompListElemNow内でreqElemが消滅することもあるので、消滅前に覚えておく。
        switch(reqElem->GetElemType())
        {
        case AsyncFileManager::ReqElemType::FILE_EASY_READ_REQ:          syncReqFinishEvent = (static_cast<AsyncFileManager::FileEasyReadReqElem*>       (reqElem))->GetReq()->m_syncReqFinishEvent;  break;
        case AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_REQ:      syncReqFinishEvent = (static_cast<AsyncFileManager::ArcFileLoadDataReqElem*>    (reqElem))->GetReq()->m_syncReqFinishEvent;  break;
        case AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ:  syncReqFinishEvent = (static_cast<AsyncFileManager::ArcFileLoadDataBufReqElem*> (reqElem))->GetReq()->m_syncReqFinishEvent;  break;
        default:                                                                                                                                                                                      break;
        }
        m_manager->DeleteUncompListElemAndUnsetUncompListElemNow(reqElem);  // 展開リストの順番は変更しない。 First In & First Out 。だけど、DeleteUncompListHeadElemはないので、DeleteUncompListElemを呼ぶ。

        // (B) 同期リクエストを出したスレッドに処理が終わったことを伝えるために、イベントをシグナル状態にする。
        if(syncReqFinishEvent)
        {
          syncReqFinishEvent->Signal();
        }
      }
      else
      {
        // (A) リクエストがないので内側ループを抜け、外側ループでリクエストが追加されるまで待つ。
        break;
 
      }
 
      // (5 1/2) 終了リクエストのチェック
      if(m_endReq) break;
    }
    
    // (5 2/2) 終了リクエストのチェック
    if(m_endReq) break;
  }

  m_hasEnded = true;
}

void OtherThread::RequestEnd(void)
{
  m_endReq = true;
}
bool OtherThread::HasEnded(void) const
{
  return m_hasEnded;
}

void OtherThread::funcUncompFileEasyRead       (AsyncFileManager::FileEasyReadReq*       req)
{
  Result res;

  // 解凍する
  void*  uncompBuf      = NULL;
  size_t uncompBufSize  = 0;
  size_t uncompDataSize = 0;
  res |= uncompress(&uncompBuf, &uncompBufSize, &uncompDataSize, req->heapForBuf, req->align, *(req->ppBuf), req->m_fileSize, req->heapForCompressed);
  // 解凍データに差し替え
  GflHeapSafeFreeMemory(*(req->ppBuf));
  if(res.IsSuccess())
  {
    *(req->ppBuf) = uncompBuf;
    if(req->pBufSize)  *(req->pBufSize)  = uncompBufSize;
    if(req->pFileSize) *(req->pFileSize) = uncompDataSize;
  }
  else
  {
    GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
  }

  if(req->result) *(req->result) |= res;
}
void OtherThread::funcUncompArcFileLoadData    (AsyncFileManager::ArcFileLoadDataReq*    req)
{
#if GFL_FS_DEBUG_LOG
  m_manager->logFsDebugLogArcFileInfo(AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_REQ, AsyncFileManager::FsDebugLogArcFileInfoKind::ANY, req->arcId, req->datId, AsyncFileManager::FsDebugLogArcFileInfoStep::UNCOMP_START);
#endif

  Result res;

  // 解凍する
  void*  uncompBuf      = NULL;
  size_t uncompBufSize  = 0;
  size_t uncompDataSize = 0;
  res |= uncompress(&uncompBuf, &uncompBufSize, &uncompDataSize, req->heapForBuf, req->align, *(req->ppBuf), req->m_realReadSize, req->heapForCompressed);
  // 解凍データに差し替え
  GflHeapSafeFreeMemory(*(req->ppBuf));
  if(res.IsSuccess())
  {
#if GFL_FS_DEBUG_LOG
    m_manager->setFsDebugLogArcFileInfoSize(AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_REQ, req->arcId, req->datId, uncompDataSize);
#endif
    *(req->ppBuf) = uncompBuf;
    if(req->pBufSize)      *(req->pBufSize)      = uncompBufSize;
    if(req->pRealReadSize) *(req->pRealReadSize) = uncompDataSize;
  }
  else
  {
    GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
  }

  if(req->result) *(req->result) |= res;

#if GFL_FS_DEBUG_LOG
  m_manager->logFsDebugLogArcFileInfo(AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_REQ, AsyncFileManager::FsDebugLogArcFileInfoKind::ANY, req->arcId, req->datId, AsyncFileManager::FsDebugLogArcFileInfoStep::UNCOMP_FINISH);
#endif
}
void OtherThread::funcUncompArcFileLoadDataBuf (AsyncFileManager::ArcFileLoadDataBufReq* req)
{
#if GFL_FS_DEBUG_LOG
  m_manager->logFsDebugLogArcFileInfo(AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ, AsyncFileManager::FsDebugLogArcFileInfoKind::ANY, req->arcId, req->datId, AsyncFileManager::FsDebugLogArcFileInfoStep::UNCOMP_START);
#endif

  Result res;

  // 解凍する
  size_t uncompDataSize = 0;
  res |= this->uncompressBuf(&uncompDataSize, req->pBuf, req->bufSize, req->m_pBuf, req->m_realReadSize, req->heapForCompressed);
  // 解凍データに差し替え
  GflHeapSafeFreeMemory(req->m_pBuf);
  if(res.IsSuccess())
  {
#if GFL_FS_DEBUG_LOG
    m_manager->setFsDebugLogArcFileInfoSize(AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ, req->arcId, req->datId, uncompDataSize);
#endif
    if(req->pRealReadSize) *(req->pRealReadSize) = uncompDataSize;
  }
  else
  {
    GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
  }

  if(req->result) *(req->result) |= res;

#if GFL_FS_DEBUG_LOG
  m_manager->logFsDebugLogArcFileInfo(AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ, AsyncFileManager::FsDebugLogArcFileInfoKind::ANY, req->arcId, req->datId, AsyncFileManager::FsDebugLogArcFileInfoStep::UNCOMP_FINISH);
#endif
}


Result OtherThread::uncompress(
    void**                ppBuf,
    size_t*               pBufSize,
    size_t*               pUncompressedDataSize,
    gfl2::heap::HeapBase* heapForBuf,
    u32                   align,
    const void*           compressedData,
    size_t                compressedDataSize,
    gfl2::heap::HeapBase* heapForTemp
)
{
  Result res;
 
  // 圧縮データのサイズが0
  if(compressedDataSize == 0)
  {
    *ppBuf = NULL;
    if(pBufSize) *pBufSize = 0;
    if(pUncompressedDataSize) *pUncompressedDataSize = 0;
    res |= Result::Bit::UNCOMPRESSED_DATA_SIZE_ZERO;
    return res;
  }

  // 2のべき乗チェック
  if( !( Calc::IsPowerOf2(align) ) )
  {
    res |= Result::Bit::ALIGN_INCORRECT;
  }
  else
  {
    // 展開サイズ
    size_t uncompSize = gfl2::cx::GetUncompressedSize( compressedData, compressedDataSize );
    if(uncompSize < compressedDataSize)
    {
      GFL_PRINT("圧縮データのほうが大きい\n");  // @note 圧縮データのほうが大きい時は、データ作成時にエラーにして欲しいなあ。→データ作成時にエラーにしないことにした。
    }
   
    // メモリ確保
    *ppBuf = GflHeapAllocMemoryAlign(heapForBuf, uncompSize, align);  // *ppBuf = GflHeapAllocMemoryAlign(heapForBuf, static_cast<const int>(uncompSize), static_cast<const int>(align));
    if(*ppBuf)
    {
      size_t bufSize = Calc::AlignVal(uncompSize, align);

      // 展開
      s32 uncompRes = gfl2::cx::SecureUncompress( compressedData, compressedDataSize, *ppBuf, heapForTemp );

      if( uncompRes == 0 )
      {
        if(pBufSize) *pBufSize = bufSize;
        if(pUncompressedDataSize) *pUncompressedDataSize = uncompSize;
      }
      else
      {
        res |= Result::Bit::UNCOMPRESS_FAIL;
        GFL_PRINT( "展開に失敗しました %d\n", uncompRes );
        GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
      }
    }
    else
    {
      res |= Result::Bit::HEAP_ALLOC_FAIL;
    }
  }

  return res;
}

Result OtherThread::uncompressBuf(
    size_t*               pUncompressedDataSize,
    void*                 pBuf,
    size_t                bufSize,
    const void*           compressedData,
    size_t                compressedDataSize,
    gfl2::heap::HeapBase* heapForTemp
)
{
  Result res;
 
  // 圧縮データのサイズが0
  if(compressedDataSize == 0)
  {
    if(pUncompressedDataSize) *pUncompressedDataSize = 0;
    res |= Result::Bit::UNCOMPRESSED_DATA_SIZE_ZERO;
    return res;
  }

  // 展開サイズ
  size_t uncompSize = gfl2::cx::GetUncompressedSize( compressedData, compressedDataSize );
  if(uncompSize < compressedDataSize)
  {
    GFL_PRINT("圧縮データのほうが大きい\n");  // @note 圧縮データのほうが大きい時は、データ作成時にエラーにして欲しいなあ。→データ作成時にエラーにしないことにした。
  }

  // バッファサイズが足りない
  if( bufSize < uncompSize )
  {
    res |= Result::Bit::BUF_SIZE_TOO_SMALL;
    GFL_PRINT( "バッファサイズが足りません(bufSize=0x%x, uncompressedDataSize=0x%x)\n", bufSize, uncompSize );
    GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
  }

  // 展開
  s32 uncompRes = gfl2::cx::SecureUncompress( compressedData, compressedDataSize, pBuf, heapForTemp );

  if( uncompRes == 0 )
  {
    if(pUncompressedDataSize) *pUncompressedDataSize = uncompSize;
  }
  else
  {
    res |= Result::Bit::UNCOMPRESS_FAIL;
    GFL_PRINT( "展開に失敗しました %d\n", uncompRes );
    GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
  }

  return res;
}


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)
