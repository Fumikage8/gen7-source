#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANSEARCHTHREAD_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANSEARCHTHREAD_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanSearchThread.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.08
 @brief   図鑑の検索のスレッド
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_heap.h>
#include <thread/include/gfl2_Thread.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class Heap;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
class ZukanViewSearch;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanSearchThread
 @brief     図鑑の検索のスレッド
 */
//==============================================================================
class ZukanSearchThread : public gfl2::thread::ThreadInterface
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanSearchThread);

public:
  ZukanSearchThread(app::util::Heap* appHeap, ZukanViewSearch* viewSearch, int threadPrio);
  virtual ~ZukanSearchThread();
  
  virtual void Function(void);

public:
  void RequestEnd(void);
  bool HasEnded(void) const;

private:
  gfl2::thread::Thread* m_thread;
  bool                  m_endReq;
  bool                  m_hasEnded;
  
  ZukanViewSearch* m_viewSearch;

  static const size_t DEFAULT_STACK_SIZE = 4096 * 2; ///< デフォルトスタックサイズ
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANSEARCHTHREAD_H_INCLUDED

