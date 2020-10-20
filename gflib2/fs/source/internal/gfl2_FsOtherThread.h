#if !defined(GFL2_FSOTHERTHREAD_H_INCLUDED)
#define GFL2_FSOTHERTHREAD_H_INCLUDED
#pragma once


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
 @class     OtherThread
 @brief     その他スレッド
 */
//==============================================================================
class OtherThread : public gfl2::thread::ThreadInterface
{
  GFL_FORBID_COPY_AND_ASSIGN(OtherThread);

public:
  OtherThread(gfl2::heap::HeapBase* heap, AsyncFileManager* manager, int threadPrio);
  virtual ~OtherThread();
  
  virtual void Function(void);

public:
  void RequestEnd(void);
  bool HasEnded(void) const;


private:
  void funcUncompFileEasyRead       (AsyncFileManager::FileEasyReadReq*       req);
  void funcUncompArcFileLoadData    (AsyncFileManager::ArcFileLoadDataReq*    req);
  void funcUncompArcFileLoadDataBuf (AsyncFileManager::ArcFileLoadDataBufReq* req);
  

  //! @name  展開
  //@{

private:
  //! @brief       圧縮データを展開する
  //! @param[out]  ppBuf                  heapForBufから確保したメモリが渡されるポインタ。呼び出し元でGflHeapFreeMemory(*ppBuf)で解放して下さい。
  //! @param[out]  pBufSize               heapForBufから確保したメモリのサイズ。heapの管理領域のサイズは含まれていない。NULLでもよい。
  //! @param[out]  pUncompressedDataSize  展開したデータのサイズ。NULLでもよい。
  //! @param[in]   heapForBuf             展開したデータを入れるバッファを確保するためのヒープ。 
  //! @param[in]   align                  heapForBufから確保する際のアラインメント。2のべき乗(1, 2, 4, 8, 16, 32, 64, 128, ...)にして下さい。
  //! @param[in]   compressedData         圧縮データ
  //! @param[in]   compressedDataSize     圧縮データのサイズ
  //! @param[in]   heapForTemp            展開のために一時的に使用するヒープ
  static Result uncompress(
      void**                ppBuf,
      size_t*               pBufSize,
      size_t*               pUncompressedDataSize,
      gfl2::heap::HeapBase* heapForBuf,
      u32                   align,
      const void*           compressedData,
      size_t                compressedDataSize,
      gfl2::heap::HeapBase* heapForTemp
  );

  //! @brief       圧縮データを展開する(あらかじめ確保してあるメモリに展開する版)
  //! @param[out]  pUncompressedDataSize  展開したデータのサイズ。NULLでもよい。
  //! @param[out]  pBuf                   呼び出し元であらかじめメモリを確保しておいたバッファ。ここに展開したデータを入れる。
  //! @param[in]   bufSize                pBufに確保してあるメモリのサイズ。
  //! @param[in]   compressedData         圧縮データ
  //! @param[in]   compressedDataSize     圧縮データのサイズ
  //! @param[in]   heapForTemp            展開のために一時的に使用するヒープ
  static Result uncompressBuf(
      size_t*               pUncompressedDataSize,
      void*                 pBuf,
      size_t                bufSize,
      const void*           compressedData,
      size_t                compressedDataSize,
      gfl2::heap::HeapBase* heapForTemp
  );
  
  //@}


private:
  gfl2::thread::Thread* m_thread;
  bool                  m_endReq;
  bool                  m_hasEnded;
  AsyncFileManager*     m_manager;
  static const size_t DEFAULT_STACK_SIZE = 4096 * 2; ///< デフォルトスタックサイズ
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_FSOTHERTHREAD_H_INCLUDED
