//==============================================================================
/**
 @file    gfl2_FsFileThread.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   ファイルスレッド
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <thread/include/gfl2_ThreadStatic.h>

// fsのインクルード
#include <fs/include/gfl2_FsDefine.h>
#include "gfl2_FsDebug.h"
#include <fs/include/gfl2_FsSystem.h>
#include "../gfl2_FsDeviceBase.h"
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/internal/gfl2_FsElemBase.h>
#include "gfl2_FsCalc.h"
#include "gfl2_FsFileThread.h"


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


FileThread::FileThread(gfl2::heap::HeapBase* heap, AsyncFileManager* manager, int threadPrio)
  : m_thread(NULL),
    m_endReq(false),
    m_hasEnded(false),
    m_manager(manager)
{
  m_thread = GFL_NEW(heap) gfl2::thread::Thread(this, heap, false, DEFAULT_STACK_SIZE );
  m_thread->Start(threadPrio);
}
FileThread::~FileThread()
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

void FileThread::Function(void)
{
  GFL_FSDEBUG_PRINT_SEQ("%s  START\n", __FUNCTION__);

  for ever
  {
    // (A) リクエストが追加されるまで待つ。
    m_manager->GetReqListExecEvent()->Wait();
    m_manager->GetReqListExecEvent()->Clear();
    
    for ever
    {
      // (1) 並び替えは要素を追加するときに済ませている
  
      // (2) 先頭のリクエストを取得
      ElemBase* reqElem = m_manager->GetReqListHeadElemAndSetReqListElemNow();
  
      if(reqElem)
      {
        // (3) リクエストのタイプで分岐
        switch(reqElem->GetElemType())
        {
        case AsyncFileManager::ReqElemType::FILE_EASY_READ_REQ:                this->funcFileEasyRead           ( (static_cast<AsyncFileManager::FileEasyReadReqElem*>           (reqElem))->GetReq() );  break;
        case AsyncFileManager::ReqElemType::FILE_EASY_WRITE_REQ:               this->funcFileEasyWrite          ( (static_cast<AsyncFileManager::FileEasyWriteReqElem*>          (reqElem))->GetReq() );  break;
        case AsyncFileManager::ReqElemType::ARC_FILE_OPEN_REQ:                 this->funcArcFileOpen            ( (static_cast<AsyncFileManager::ArcFileOpenReqElem*>            (reqElem))->GetReq() );  break;
        case AsyncFileManager::ReqElemType::ARC_FILE_CLOSE_REQ:                this->funcArcFileClose           ( (static_cast<AsyncFileManager::ArcFileCloseReqElem*>           (reqElem))->GetReq() );  break;
        case AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_REQ:            this->funcArcFileLoadData        ( (static_cast<AsyncFileManager::ArcFileLoadDataReqElem*>        (reqElem))->GetReq() );  break;
        case AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ:        this->funcArcFileLoadDataBuf     ( (static_cast<AsyncFileManager::ArcFileLoadDataBufReqElem*>     (reqElem))->GetReq() );  break;
        case AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_PIECE_BUF_REQ:  this->funcArcFileLoadDataPieceBuf( (static_cast<AsyncFileManager::ArcFileLoadDataPieceBufReqElem*>(reqElem))->GetReq() );  break;
        case AsyncFileManager::ReqElemType::ARC_FILE_SEEK_DATA_PTR_REQ:        this->funcArcFileSeekDataPtr     ( (static_cast<AsyncFileManager::ArcFileSeekDataPtrReqElem*>     (reqElem))->GetReq() );  break;
        case AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_PTR_BUF_REQ:    this->funcArcFileLoadDataPtrBuf  ( (static_cast<AsyncFileManager::ArcFileLoadDataPtrBufReqElem*>  (reqElem))->GetReq() );  break;
        default:                                                                                                                                                                                          break;
        }
  
        // (4) 先頭のリクエストを削除
        gfl2::thread::Event* syncReqFinishEvent = NULL;  // DeleteReqListElemAndUnsetReqListElemNow内でreqElemが消滅することもあるので、消滅前に覚えておく。
        switch(reqElem->GetElemType())
        {
        case AsyncFileManager::ReqElemType::FILE_EASY_READ_REQ:                syncReqFinishEvent = (static_cast<AsyncFileManager::FileEasyReadReqElem*>           (reqElem))->GetReq()->m_syncReqFinishEvent;  break;
        case AsyncFileManager::ReqElemType::FILE_EASY_WRITE_REQ:               syncReqFinishEvent = (static_cast<AsyncFileManager::FileEasyWriteReqElem*>          (reqElem))->GetReq()->m_syncReqFinishEvent;  break;
        case AsyncFileManager::ReqElemType::ARC_FILE_OPEN_REQ:                 syncReqFinishEvent = (static_cast<AsyncFileManager::ArcFileOpenReqElem*>            (reqElem))->GetReq()->m_syncReqFinishEvent;  break;
        case AsyncFileManager::ReqElemType::ARC_FILE_CLOSE_REQ:                syncReqFinishEvent = (static_cast<AsyncFileManager::ArcFileCloseReqElem*>           (reqElem))->GetReq()->m_syncReqFinishEvent;  break;
        case AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_REQ:            syncReqFinishEvent = (static_cast<AsyncFileManager::ArcFileLoadDataReqElem*>        (reqElem))->GetReq()->m_syncReqFinishEvent;  break;
        case AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ:        syncReqFinishEvent = (static_cast<AsyncFileManager::ArcFileLoadDataBufReqElem*>     (reqElem))->GetReq()->m_syncReqFinishEvent;  break;
        case AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_PIECE_BUF_REQ:  syncReqFinishEvent = (static_cast<AsyncFileManager::ArcFileLoadDataPieceBufReqElem*>(reqElem))->GetReq()->m_syncReqFinishEvent;  break;
        case AsyncFileManager::ReqElemType::ARC_FILE_SEEK_DATA_PTR_REQ:        syncReqFinishEvent = (static_cast<AsyncFileManager::ArcFileSeekDataPtrReqElem*>     (reqElem))->GetReq()->m_syncReqFinishEvent;  break;
        case AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_PTR_BUF_REQ:    syncReqFinishEvent = (static_cast<AsyncFileManager::ArcFileLoadDataPtrBufReqElem*>  (reqElem))->GetReq()->m_syncReqFinishEvent;  break;
        default:                                                                                                                                                                                          break;
        }
        m_manager->DeleteReqListElemAndUnsetReqListElemNow(reqElem);  // スレッドを使用しているので、他スレッドでリクエストを先頭に追加した直後に、このスレッドのこの位置でDeleteすることもあり得るので、そのときの先頭を削除してしまうDeleteReqListHeadElemは使用しないこと。
        
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
  
  GFL_FSDEBUG_PRINT_SEQ("%s  END\n", __FUNCTION__);
}

void FileThread::RequestEnd(void)
{
  m_endReq = true;
}
bool FileThread::HasEnded(void) const
{
  return m_hasEnded;
}

void FileThread::funcFileEasyRead      (AsyncFileManager::FileEasyReadReq*       req)
{
  GFL_FSDEBUG_PRINT_SEQ_FUNC();
  Result res;
  size_t bufSize = 0; 
  size_t fileSize = 0;

  gfl2::heap::HeapBase* heap = req->heapForBuf;
  if(req->heapForCompressed != NULL)
  {
    heap = req->heapForCompressed;
  }

  File file;
  const DeviceBase* device = m_manager->GetMainDevice();
#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_HIO_ENABLE
  if(m_manager->GetHioFileUseJudgement(req->hioUseSetting)) device = m_manager->GetHioDevice();
#endif
#endif  // GF_PLATFORM_CTR
  res |= file.Open(req->heapForReq, device, req->fileName, Define::OpenMode::READ);
  if(res.IsSuccess())
  {
    res |= file.GetFileSize(&fileSize);
    if(res.IsSuccess())
    {
      size_t realReadSize = 0;
      res |= file.Read(req->ppBuf, &bufSize, &realReadSize, fileSize, heap, req->align);
      if(res.IsSuccess())
      {
        if(!(realReadSize==fileSize))  // ファイル丸ごと読み込みだからrealReadSize==fileSizeとならねばならない。
        {
          GflHeapSafeFreeMemory(*(req->ppBuf));
          res |= Result::Bit::FILE_READ_FAIL;
        }
      }
    }
    res |= file.Close();
  }

  if(res.IsSuccess())
  {
    if(req->heapForCompressed != NULL)
    {
      req->m_fileSize = fileSize;
      m_manager->AddFileEasyReadUncomp(req);
    }
    else
    {
      if(req->pBufSize)  *(req->pBufSize)  = bufSize;
      if(req->pFileSize) *(req->pFileSize) = fileSize;
    }
  }
  else
  {
    GFL_PRINT("FileEasyRead failed. fileName=%s\n", req->fileName);
    GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
  }
  if(req->result) *(req->result) |= res;
}                             
void FileThread::funcFileEasyWrite     (AsyncFileManager::FileEasyWriteReq*      req)
{                             
  GFL_FSDEBUG_PRINT_SEQ_FUNC();
  Result res;

  File file;
  const DeviceBase* device = m_manager->GetMainDevice();
#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_HIO_ENABLE
  if(m_manager->GetHioFileUseJudgement(req->hioUseSetting)) device = m_manager->GetHioDevice();
#endif
#endif  // GF_PLATFORM_CTR
  res |= file.Open(req->heapForReq, device, req->fileName, Define::OpenMode::WRITE);
  if(res.IsSuccess())
  {
    res |= file.Write(req->pBuf, req->bufSize);
    res |= file.Close();
  }

  if(!res.IsSuccess())
  {
    GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
  }
  if(req->result) *(req->result) |= res;
}


void FileThread::funcArcFileOpen       (AsyncFileManager::ArcFileOpenReq*        req)
{
  GFL_FSDEBUG_PRINT_SEQ_FUNC();

  Result res;


  // 既にオープンしたArcFileが存在するかチェック
  {
    ElemBase* elem = NULL;
    if(req->arcId != ArcFile::ARCID_NULL)
    {
      elem = m_manager->GetArcFileElem(req->arcId);
    }
    else
    {
      elem = m_manager->GetFileElem(req->fileName);
    }
    if(elem)
    {
      AsyncFileManager::ArcFileElem* arcFileElem = static_cast<AsyncFileManager::ArcFileElem*>(elem);
      // 既に存在するので、カウントを増やして終わり。
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
      arcFileElem->IncreaseCount(m_manager->GetCallerHeap(), req->m_callerInfo);
      req->m_callerInfo = NULL;  // m_callerInfoの所有権を渡したのでNULLにする。
#else
      arcFileElem->IncreaseCount();
#endif
      if(req->result) *(req->result) |= res;
      return;
    }
  }


#if GFL_FS_DEBUG_LOG
  m_manager->logFsDebugLogArcFileInfo(AsyncFileManager::ReqElemType::ARC_FILE_OPEN_REQ, AsyncFileManager::FsDebugLogArcFileInfoKind::ANY, req->arcId, ArcFile::ARCDATID_NULL, AsyncFileManager::FsDebugLogArcFileInfoStep::FILE_START);
#endif

  ArcFile* file = GFL_NEW(req->heapForFile) ArcFile();
  file->SetArcLang(m_manager->GetLangOfArcFile(req->lang));
#if GFL_FS_GASC_ENABLE
  file->SetArcSrcUseFlag(m_manager->GetArcSrcCreateJudgement(req->arcSrcCreateSetting));
#endif
  
  char* fileName = NULL;
  const char* const_fileName = NULL;

  if(req->arcId != ArcFile::ARCID_NULL)
  {
    file->SetArcId(req->arcId);

    gfl2::fs::AsyncFileManager::ArcFileNameTableType::Tag type = gfl2::fs::AsyncFileManager::ArcFileNameTableType::MAIN;
#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_HIO_ENABLE
    if(m_manager->GetHioUseJudgement(req->hioUseSetting)) type = gfl2::fs::AsyncFileManager::ArcFileNameTableType::HIO;
#endif
#endif  // GF_PLATFORM_CTR
    fileName = m_manager->GetUsableArcFileName(type, req->heapForReq, req->arcId);

    const_fileName = fileName;
  }
  else
  {
    const_fileName = req->fileName;
  }

  const DeviceBase* device = NULL;
  if(req->arcId != ArcFile::ARCID_NULL)
  {
    device = m_manager->GetMainDevice(req->arcId);
  }
  else
  {
    device = m_manager->GetMainDevice(req->heapForReq, req->fileName);  // tmpHeapだが後方確保しない。heapForReq自体がリクエスト中に使うヒープという位置付けで渡してもらっているものなので。
  }
#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_HIO_ENABLE
  if(m_manager->GetHioUseJudgement(req->hioUseSetting)) device = m_manager->GetHioDevice();
#endif
#endif  // GF_PLATFORM_CTR
  res |= file->Open(req->heapForFile, req->heapForReq, device, const_fileName);

  GflHeapSafeFreeMemory(fileName);


#if GFL_FS_GASC_ENABLE
  if(res.IsSuccess())
  {
    Result arcSrcRes;
    gfl2::heap::HeapBase* heapForArcSrc = req->heapForArcSrc;
    if(heapForArcSrc == NULL)
    {
      heapForArcSrc = m_manager->GetArcSrcDefaultHeap();
    }
    if( m_manager->GetArcSrcCreateJudgement(req->arcSrcCreateSetting) && heapForArcSrc )
    {
      const char* arcSrcFileName = NULL;
      char full_arcSrcFileName[Define::FILE_NAME_FULL_LEN_MAX];

      if(req->arcId != ArcFile::ARCID_NULL)
      {
        char* part_arcSrcFileName = m_manager->GetUsableArcFileName(gfl2::fs::AsyncFileManager::ArcFileNameTableType::ARCSRC, req->heapForReq, req->arcId);

        const DeviceBase* arcSrcDevice = m_manager->GetArcSrcDevice();
        if(arcSrcDevice)
        {
          u32 full_arcSrcFileNameLen = 0;
          arcSrcRes |= arcSrcDevice->GetFullPathFileName(&full_arcSrcFileNameLen, full_arcSrcFileName, Define::FILE_NAME_FULL_LEN_MAX, part_arcSrcFileName);
          if(arcSrcRes.IsSuccess())
          {
            arcSrcFileName = full_arcSrcFileName;
          }
          else
          {
            arcSrcRes.Print();
            GFL_PRINT("GetFullPathFileName failed. arcId=%d, fileName=%s, arcSrcFileName=%s\n", req->arcId, (req->fileName)?(req->fileName):"", arcSrcFileName);  // ArcSrcではエラーにしない。
          }
        }
        else
        {
          arcSrcRes |= Result::Bit::ARCSRC_DEVICE_NOT_EXIST;
          GFL_PRINT("GetArcSrcDevice failed. arcId=%d, fileName=%s, arcSrcFileName=%s\n", req->arcId, (req->fileName)?(req->fileName):"", arcSrcFileName);  // ArcSrcではエラーにしない。
        }

        GflHeapSafeFreeMemory(part_arcSrcFileName);
      }
      else
      {
        arcSrcFileName = req->arcSrcFileName;
      }
      if(arcSrcFileName != NULL)
      {
        arcSrcRes |= file->CreateArcSrcAccessor(heapForArcSrc, req->heapForReq, m_manager->GetArcSrcDevice(), arcSrcFileName);
        if(!arcSrcRes.IsSuccess())
        {
          arcSrcRes.Print();
          GFL_PRINT("CreateArcSrcAccessor failed. arcId=%d, fileName=%s, arcSrcFileName=%s\n", req->arcId, (req->fileName)?(req->fileName):"", arcSrcFileName);  // ArcSrcではエラーにしない。
        }
      }
    }
  }
#endif


  if(res.IsSuccess())
  {
    AsyncFileManager::ArcFileElem* arcFileElem = m_manager->AddArcFile(req->heapForFile, file);
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
    // リクエスト発行者のリストを作成しておく。
    arcFileElem->CreateAllCallerList(m_manager->GetCallerHeap());
#endif
    // カウントを増やしておく。
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
    arcFileElem->IncreaseCount(m_manager->GetCallerHeap(), req->m_callerInfo);
    req->m_callerInfo = NULL;  // m_callerInfoの所有権を渡したのでNULLにする。
#else
    arcFileElem->IncreaseCount();
#endif
  }
  else
  {
    res.Print();
    GFL_PRINT("ArcFileOpen failed. arcId=%d, fileName=%s, \n", req->arcId, (req->fileName)?(req->fileName):"" );
    GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
  }

  if(req->result) *(req->result) |= res;

#if GFL_FS_DEBUG_LOG
  m_manager->logFsDebugLogArcFileInfo(AsyncFileManager::ReqElemType::ARC_FILE_OPEN_REQ, AsyncFileManager::FsDebugLogArcFileInfoKind::ANY, req->arcId, ArcFile::ARCDATID_NULL, AsyncFileManager::FsDebugLogArcFileInfoStep::FILE_FINISH);
#endif
}


void FileThread::funcArcFileClose      (AsyncFileManager::ArcFileCloseReq*       req)
{
  GFL_FSDEBUG_PRINT_SEQ_FUNC();

  Result res;


  // 既にオープンしたArcFileが存在するかチェック  // @note これより下の処理とまとめたい。同じif文があるので。
  {
    ElemBase* elem = NULL;
    if(req->arcId != ArcFile::ARCID_NULL)
    {
      elem = m_manager->GetArcFileElem(req->arcId);
    }
    else
    {
      elem = m_manager->GetFileElem(req->fileName);
    }
    if(elem)
    {
      AsyncFileManager::ArcFileElem* arcFileElem = static_cast<AsyncFileManager::ArcFileElem*>(elem);
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
      arcFileElem->DecreaseCount(m_manager->GetCallerHeap(), req->m_callerInfo);
      req->m_callerInfo = NULL;  // m_callerInfoの所有権を渡したのでNULLにする。
#else
      arcFileElem->DecreaseCount();
#endif
      u32 count = arcFileElem->GetCount();
      if(count > 0)
      {
        // 減らした後にカウントがまだ残っているなら、終わり。
        if(req->result) *(req->result) |= res;

#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
        {
          // ArcFileのクローズのとき、クローズ済みフラグを立てておく。
          if(req->arcId != ArcFile::ARCID_NULL)
          {
            m_manager->setMatchCheckArcFileClosedFlagOn(req->arcId);
          }
          else
          {
            m_manager->setMatchCheckArcFileClosedFlagOn(req->fileName);
          }
        }
#endif

        return;
      }
    }
  }


#if GFL_FS_DEBUG_LOG
  m_manager->logFsDebugLogArcFileInfo(AsyncFileManager::ReqElemType::ARC_FILE_CLOSE_REQ, AsyncFileManager::FsDebugLogArcFileInfoKind::ANY, req->arcId, ArcFile::ARCDATID_NULL, AsyncFileManager::FsDebugLogArcFileInfoStep::FILE_START);
#endif
 
  if(req->arcId != ArcFile::ARCID_NULL)
  {
    ElemBase* elem = m_manager->GetArcFileElem(req->arcId);
    if(elem)
    {
      AsyncFileManager::ArcFileElem* el = static_cast<AsyncFileManager::ArcFileElem*>(elem);
      ArcFile* file = el->GetFile();
      
      m_manager->DeleteArcFileElem(req->arcId);  // @note ARCIDでなくelem渡しのもつくっておきたい。file->Close();をしてしまうとファイル名がなくなってしまうので、file->Close();より先に書いた。
     
#if GFL_FS_GASC_ENABLE
      res |= file->DestroyArcSrcAccessor();
#endif
      res |= file->Close();
      if(!res.IsSuccess())
      {
        GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
      }
      GFL_DELETE(file);
    }
    else
    {
      res |= Result::Bit::ASYNC_FILE_MANAGER_NOT_MANAGE_ARC_FILE;
    }
  }
  else
  {
    ElemBase* elem = m_manager->GetFileElem(req->fileName);
    if(elem)
    {
      AsyncFileManager::ArcFileElem* el = static_cast<AsyncFileManager::ArcFileElem*>(elem);
      ArcFile* file = el->GetFile();
      
      m_manager->DeleteFileElem(req->fileName);  // @note ファイル名でなくelem渡しのもつくっておきたい。file->Close();をしてしまうとファイル名がなくなってしまうので、file->Close();より先に書いた。

#if GFL_FS_GASC_ENABLE
      res |= file->DestroyArcSrcAccessor();
#endif
      res |= file->Close();
      if(!res.IsSuccess())
      {
        GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
      }
      GFL_DELETE(file);
    }
    else
    {
      res |= Result::Bit::ASYNC_FILE_MANAGER_NOT_MANAGE_ARC_FILE;

      res.Print();
      GFL_PRINT("ArcFileClose failed. arcId=%d, fileName=%s\n", req->arcId, (req->fileName)?(req->fileName):"" );
      GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
    }
  }
    
  if(req->result) *(req->result) |= res;

#if GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST
  {
    // ArcFileのクローズのとき、クローズ済みフラグを立てておく。
    if(req->arcId != ArcFile::ARCID_NULL)
    {
      m_manager->setMatchCheckArcFileClosedFlagOn(req->arcId);
    }
    else
    {
      m_manager->setMatchCheckArcFileClosedFlagOn(req->fileName);
    }
  }
#endif

#if GFL_FS_DEBUG_LOG
  m_manager->logFsDebugLogArcFileInfo(AsyncFileManager::ReqElemType::ARC_FILE_CLOSE_REQ, AsyncFileManager::FsDebugLogArcFileInfoKind::ANY, req->arcId, ArcFile::ARCDATID_NULL, AsyncFileManager::FsDebugLogArcFileInfoStep::FILE_FINISH);
#endif
}
void FileThread::funcArcFileLoadData   (AsyncFileManager::ArcFileLoadDataReq*    req)
{
  GFL_FSDEBUG_PRINT_SEQ_FUNC();

#if GFL_FS_DEBUG_LOG
  m_manager->logFsDebugLogArcFileInfo(AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_REQ, AsyncFileManager::FsDebugLogArcFileInfoKind::ANY, req->arcId, req->datId, AsyncFileManager::FsDebugLogArcFileInfoStep::FILE_START);
#endif

  Result res;
 
  ElemBase* elem = NULL;
  if(req->arcId != ArcFile::ARCID_NULL)
  {
    elem = m_manager->GetArcFileElem(req->arcId);
  }
  else
  {
    elem = m_manager->GetFileElem(req->fileName);
  }

  if(elem)
  {
    AsyncFileManager::ArcFileElem* el = static_cast<AsyncFileManager::ArcFileElem*>(elem);
    ArcFile* file = el->GetFile();
  
    size_t bufSize; 
    size_t realReadSize;
    gfl2::heap::HeapBase* heap = req->heapForBuf;
    if(req->heapForCompressed != NULL)
    {
      heap = req->heapForCompressed;
    }
#if GFL_FS_GASC_ENABLE
    file->SetArcSrcUseFlag(m_manager->GetArcSrcUseJudgement(req->arcSrcUseSetting));
#endif
#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
    el->LogIntervalCheckInfoAccessTime(false, m_manager->GetIntervalCheckLevel());
#endif
    res |= file->LoadData(req->ppBuf, &bufSize, &realReadSize, req->datId, m_manager->GetLangOfArcFile(req->lang), heap, req->align, req->heapForReq);
#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
    el->LogIntervalCheckInfoAccessTime(true, m_manager->GetIntervalCheckLevel());
#endif
    if(res.IsSuccess())
    {
      if(req->heapForCompressed != NULL)
      {
#if GFL_FS_DEBUG_LOG
        m_manager->setFsDebugLogArcFileInfoCompSize(AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_REQ, req->arcId, req->datId, realReadSize);
#endif
        req->m_realReadSize = realReadSize;
        m_manager->AddArcFileLoadDataUncomp(req);
      }
      else
      {
#if GFL_FS_DEBUG_LOG
        m_manager->setFsDebugLogArcFileInfoSize(AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_REQ, req->arcId, req->datId, realReadSize);
#endif
        if(req->pBufSize)      *(req->pBufSize)      = bufSize;
        if(req->pRealReadSize) *(req->pRealReadSize) = realReadSize;
      }
    }
    else
    {
      res.Print();
      GFL_PRINT("ArcFileLoadData \"Load datId\" failed. arcId=%d, fileName=%s, datId=%d\n", req->arcId, (req->fileName)?(req->fileName):"", req->datId );
      GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
    }
  }
  else
  {
    res |= Result::Bit::ASYNC_FILE_MANAGER_NOT_MANAGE_ARC_FILE;

    res.Print();
    GFL_PRINT("ArcFileLoadData \"Manage ArcFile\" failed. arcId=%d, fileName=%s, datId=%d\n", req->arcId, (req->fileName)?(req->fileName):"", req->datId );
    GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
  }
    
  if(req->result) *(req->result) |= res;

#if GFL_FS_DEBUG_LOG
  m_manager->logFsDebugLogArcFileInfo(AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_REQ, AsyncFileManager::FsDebugLogArcFileInfoKind::ANY, req->arcId, req->datId, AsyncFileManager::FsDebugLogArcFileInfoStep::FILE_FINISH);
#endif
}
void FileThread::funcArcFileLoadDataBuf(AsyncFileManager::ArcFileLoadDataBufReq* req)
{
  GFL_FSDEBUG_PRINT_SEQ_FUNC();

#if GFL_FS_DEBUG_LOG
  m_manager->logFsDebugLogArcFileInfo(AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ, AsyncFileManager::FsDebugLogArcFileInfoKind::ANY, req->arcId, req->datId, AsyncFileManager::FsDebugLogArcFileInfoStep::FILE_START);
#endif

  Result res;
 
  ElemBase* elem = NULL;
  if(req->arcId != ArcFile::ARCID_NULL)
  {
    elem = m_manager->GetArcFileElem(req->arcId);
  }
  else
  {
    elem = m_manager->GetFileElem(req->fileName);
  }

  if(elem)
  {
    AsyncFileManager::ArcFileElem* el = static_cast<AsyncFileManager::ArcFileElem*>(elem);
    ArcFile* file = el->GetFile();

    void*  compBuf      = NULL;  // heapForCompressedがNULLでないときのみ使用。
    size_t compBufSize  = 0;     // heapForCompressedがNULLでないときのみ使用。
    size_t realReadSize = 0;
#if GFL_FS_GASC_ENABLE
    file->SetArcSrcUseFlag(m_manager->GetArcSrcUseJudgement(req->arcSrcUseSetting));
#endif
#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
    el->LogIntervalCheckInfoAccessTime(false, m_manager->GetIntervalCheckLevel());
#endif
    if(req->heapForCompressed != NULL)
    {
      res |= file->LoadData(&compBuf, &compBufSize, &realReadSize, req->datId, m_manager->GetLangOfArcFile(req->lang), req->heapForCompressed, req->alignForCompressed, req->heapForReq);
    }
    else
    {
      res |= file->LoadData(&realReadSize, req->pBuf, req->bufSize, req->datId, m_manager->GetLangOfArcFile(req->lang), req->heapForReq);
    }
#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
    el->LogIntervalCheckInfoAccessTime(true, m_manager->GetIntervalCheckLevel());
#endif
    if(res.IsSuccess())
    {
      if(req->heapForCompressed != NULL)
      {
#if GFL_FS_DEBUG_LOG
        m_manager->setFsDebugLogArcFileInfoCompSize(AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ, req->arcId, req->datId, realReadSize);
#endif
        req->m_pBuf         = compBuf;
        req->m_realReadSize = realReadSize;
        m_manager->AddArcFileLoadDataBufUncomp(req);
      }
      else
      {
#if GFL_FS_DEBUG_LOG
        m_manager->setFsDebugLogArcFileInfoSize(AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ, req->arcId, req->datId, realReadSize);
#endif
        if(req->pRealReadSize) *(req->pRealReadSize) = realReadSize;
      }
    }
    else
    {
      GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
    }
  }
  else
  {
    res |= Result::Bit::ASYNC_FILE_MANAGER_NOT_MANAGE_ARC_FILE;

    res.Print();
    GFL_PRINT("ArcFileLoadDataBuf failed. arcId=%d, fileName=%s, datId=%d\n", req->arcId, (req->fileName)?(req->fileName):"", req->datId );
    GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
  }
    
  if(req->result) *(req->result) |= res;

#if GFL_FS_DEBUG_LOG
  m_manager->logFsDebugLogArcFileInfo(AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_BUF_REQ, AsyncFileManager::FsDebugLogArcFileInfoKind::ANY, req->arcId, req->datId, AsyncFileManager::FsDebugLogArcFileInfoStep::FILE_FINISH);
#endif
}

void FileThread::funcArcFileLoadDataPieceBuf(AsyncFileManager::ArcFileLoadDataPieceBufReq* req)
{
  GFL_FSDEBUG_PRINT_SEQ_FUNC();

#if GFL_FS_DEBUG_LOG
  m_manager->logFsDebugLogArcFileInfo(AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_PIECE_BUF_REQ, AsyncFileManager::FsDebugLogArcFileInfoKind::ANY, req->arcId, req->datId, AsyncFileManager::FsDebugLogArcFileInfoStep::FILE_START);
#endif
 
  Result res;
 
  ElemBase* elem = NULL;
  if(req->arcId != ArcFile::ARCID_NULL)
  {
    elem = m_manager->GetArcFileElem(req->arcId);
  }
  else
  {
    elem = m_manager->GetFileElem(req->fileName);
  }

  if(elem)
  {
    AsyncFileManager::ArcFileElem* el = static_cast<AsyncFileManager::ArcFileElem*>(elem);
    ArcFile* file = el->GetFile();
  
    size_t realReadSize;
#if GFL_FS_GASC_ENABLE
    //file->SetArcSrcUseFlag(m_manager->GetArcSrcUseJudgement(req->arcSrcUseSetting));  // 無意味だからコメントアウト。リクエストにarcSrcUseSettingメンバがないし。
#endif
#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
    el->LogIntervalCheckInfoAccessTime(false, m_manager->GetIntervalCheckLevel());
#endif
    res |= file->LoadDataPiece(&realReadSize, req->pBuf, req->bufSize, req->datId, m_manager->GetLangOfArcFile(req->lang), req->offsetFromData, req->readSize);
#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
    el->LogIntervalCheckInfoAccessTime(true, m_manager->GetIntervalCheckLevel());
#endif
    if(res.IsSuccess())
    {
#if GFL_FS_DEBUG_LOG
      m_manager->setFsDebugLogArcFileInfoSize(AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_PIECE_BUF_REQ, req->arcId, req->datId, realReadSize);
#endif
      if(req->pRealReadSize) *(req->pRealReadSize) = realReadSize;
    }
    else
    {
      GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
    }
  }
  else
  {
    res |= Result::Bit::ASYNC_FILE_MANAGER_NOT_MANAGE_ARC_FILE;
  }
    
  if(req->result) *(req->result) |= res;

#if GFL_FS_DEBUG_LOG
  m_manager->logFsDebugLogArcFileInfo(AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_PIECE_BUF_REQ, AsyncFileManager::FsDebugLogArcFileInfoKind::ANY, req->arcId, req->datId, AsyncFileManager::FsDebugLogArcFileInfoStep::FILE_FINISH);
#endif
}
void FileThread::funcArcFileSeekDataPtr(AsyncFileManager::ArcFileSeekDataPtrReq* req)
{
  GFL_FSDEBUG_PRINT_SEQ_FUNC();

#if GFL_FS_DEBUG_LOG
  m_manager->logFsDebugLogArcFileInfo(AsyncFileManager::ReqElemType::ARC_FILE_SEEK_DATA_PTR_REQ, AsyncFileManager::FsDebugLogArcFileInfoKind::ANY, req->arcId, req->datId, AsyncFileManager::FsDebugLogArcFileInfoStep::FILE_START);
#endif

  Result res;
 
  ElemBase* elem = NULL;
  if(req->arcId != ArcFile::ARCID_NULL)
  {
    elem = m_manager->GetArcFileElem(req->arcId);
  }
  else
  {
    elem = m_manager->GetFileElem(req->fileName);
  }

  if(elem)
  {
    AsyncFileManager::ArcFileElem* el = static_cast<AsyncFileManager::ArcFileElem*>(elem);
    ArcFile* file = el->GetFile();
#if GFL_FS_GASC_ENABLE
    //file->SetArcSrcUseFlag(m_manager->GetArcSrcUseJudgement(req->arcSrcUseSetting));  // 無意味だからコメントアウト。リクエストにarcSrcUseSettingメンバがないし。
#endif
#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
    el->LogIntervalCheckInfoAccessTime(false, m_manager->GetIntervalCheckLevel());
#endif
    res |= file->SeekDataPtr(req->datId, m_manager->GetLangOfArcFile(req->lang), req->offsetFromData);
#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
    el->LogIntervalCheckInfoAccessTime(true, m_manager->GetIntervalCheckLevel());
#endif
    if(!res.IsSuccess())
    {
      GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
    }
  }
  else
  {
    res |= Result::Bit::ASYNC_FILE_MANAGER_NOT_MANAGE_ARC_FILE;
  }
  
  if(req->result) *(req->result) |= res;

#if GFL_FS_DEBUG_LOG
  m_manager->logFsDebugLogArcFileInfo(AsyncFileManager::ReqElemType::ARC_FILE_SEEK_DATA_PTR_REQ, AsyncFileManager::FsDebugLogArcFileInfoKind::ANY, req->arcId, req->datId, AsyncFileManager::FsDebugLogArcFileInfoStep::FILE_FINISH);
#endif
}
void FileThread::funcArcFileLoadDataPtrBuf(AsyncFileManager::ArcFileLoadDataPtrBufReq* req)
{
  GFL_FSDEBUG_PRINT_SEQ_FUNC();

#if GFL_FS_DEBUG_LOG
  m_manager->logFsDebugLogArcFileInfo(AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_PTR_BUF_REQ, AsyncFileManager::FsDebugLogArcFileInfoKind::ANY, req->arcId, ArcFile::ARCDATID_NULL, AsyncFileManager::FsDebugLogArcFileInfoStep::FILE_START);
#endif
 
  Result res;
 
  ElemBase* elem = NULL;
  if(req->arcId != ArcFile::ARCID_NULL)
  {
    elem = m_manager->GetArcFileElem(req->arcId);
  }
  else
  {
    elem = m_manager->GetFileElem(req->fileName);
  }

  if(elem)
  {
    AsyncFileManager::ArcFileElem* el = static_cast<AsyncFileManager::ArcFileElem*>(elem);
    ArcFile* file = el->GetFile();
  
    size_t realReadSize;
#if GFL_FS_GASC_ENABLE
    //file->SetArcSrcUseFlag(m_manager->GetArcSrcUseJudgement(req->arcSrcUseSetting));  // 無意味だからコメントアウト。リクエストにarcSrcUseSettingメンバがないし。
#endif
#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
    el->LogIntervalCheckInfoAccessTime(false, m_manager->GetIntervalCheckLevel());
#endif
    res |= file->LoadDataPtr(&realReadSize, req->pBuf, req->bufSize, req->readSize);
#if GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL
    el->LogIntervalCheckInfoAccessTime(true, m_manager->GetIntervalCheckLevel());
#endif
    if(res.IsSuccess())
    {
#if GFL_FS_DEBUG_LOG
      m_manager->setFsDebugLogArcFileInfoSize(AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_PTR_BUF_REQ, req->arcId, ArcFile::ARCDATID_NULL, realReadSize);
#endif
      if(req->pRealReadSize) *(req->pRealReadSize) = realReadSize;
    }
    else
    {
      GFL_FSDEBUG_ASYNC_FILE_MANAGER_ATTENTION(0);
    }
  }
  else
  {
    res |= Result::Bit::ASYNC_FILE_MANAGER_NOT_MANAGE_ARC_FILE;
  }
 
  if(req->result) *(req->result) |= res;

#if GFL_FS_DEBUG_LOG
  m_manager->logFsDebugLogArcFileInfo(AsyncFileManager::ReqElemType::ARC_FILE_LOAD_DATA_PTR_BUF_REQ, AsyncFileManager::FsDebugLogArcFileInfoKind::ANY, req->arcId, ArcFile::ARCDATID_NULL, AsyncFileManager::FsDebugLogArcFileInfoStep::FILE_FINISH);
#endif
}


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)
