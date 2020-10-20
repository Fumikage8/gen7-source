#if !defined(GFL2_FSFILETHREAD_H_INCLUDED)
#define GFL2_FSFILETHREAD_H_INCLUDED
#pragma once


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
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_heap.h>
#include <thread/include/gfl2_Thread.h>

// fsのインクルード
#include <fs/include/gfl2_FsToolDefine.h>
#include <fs/include/gfl2_FsDefine.h>
#include <fs/include/gfl2_FsResult.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)

class AsyncFileManager;

GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)




GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     FileThread
 @brief     ファイルスレッド
 */
//==============================================================================
class FileThread : public gfl2::thread::ThreadInterface
{
  GFL_FORBID_COPY_AND_ASSIGN(FileThread);

public:
  FileThread(gfl2::heap::HeapBase* heap, AsyncFileManager* manager, int threadPrio);
  virtual ~FileThread();

  virtual void Function(void);

public:
  void RequestEnd(void);
  bool HasEnded(void) const;

private:
  void funcFileEasyRead           (AsyncFileManager::FileEasyReadReq*            req);
  void funcFileEasyWrite          (AsyncFileManager::FileEasyWriteReq*           req);
  void funcArcFileOpen            (AsyncFileManager::ArcFileOpenReq*             req);
  void funcArcFileClose           (AsyncFileManager::ArcFileCloseReq*            req);
  void funcArcFileLoadData        (AsyncFileManager::ArcFileLoadDataReq*         req);
  void funcArcFileLoadDataBuf     (AsyncFileManager::ArcFileLoadDataBufReq*      req);
  void funcArcFileLoadDataPieceBuf(AsyncFileManager::ArcFileLoadDataPieceBufReq* req);
  void funcArcFileSeekDataPtr     (AsyncFileManager::ArcFileSeekDataPtrReq*      req);
  void funcArcFileLoadDataPtrBuf  (AsyncFileManager::ArcFileLoadDataPtrBufReq*   req);

private:
  gfl2::thread::Thread* m_thread;
  bool                  m_endReq;
  bool                  m_hasEnded;
  AsyncFileManager*     m_manager;
  static const size_t DEFAULT_STACK_SIZE = 4096 * 2; ///< デフォルトスタックサイズ
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_FSFILETHREAD_H_INCLUDED
