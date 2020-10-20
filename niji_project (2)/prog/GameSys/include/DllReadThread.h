#ifndef __DLL_READ_THREAD_H__
#define __DLL_READ_THREAD_H__
//================================================================================
/**
 * @file   DllReadThread.h
 * @brief  DLL呼び出しスレッド
 * @author k.ohno
 * @date   2013.2.27

  2015.12.13 tamada バトル専用だったのを汎用化してnijiに移植開始
 */
//================================================================================
#pragma once

#include <ro/include/gfl2_RoManager.h>
#include <macro/include/Gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <thread/include/gfl2_Thread.h>


GFL_NAMESPACE_BEGIN(GameSys)

  class GameProc;  //前方参照宣言

class DllReadThread : public gfl2::thread::ThreadInterface
{
  GFL_FORBID_COPY_AND_ASSIGN( DllReadThread );

public:
  static const int _MAX=10;

  gfl2::ro::Module * mRoArr[_MAX];
  int mRoNum;

  const char* mCroFileNameArr[_MAX];
  
public:
  //--------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param heap        スレッドのスタック領域に使用するヒープ
   * @param stack_size  スレッドのスタック領域のサイズ
   */
  //--------------------------------------------------------------------------------
  DllReadThread( gfl2::heap::HeapBase* heap, u32 stack_size );

  //--------------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //--------------------------------------------------------------------------------
  virtual ~DllReadThread();

  //--------------------------------------------------------------------------------
  /**
   * @brief DLL読み込みに必要なデータ設定
   * @param croFileNameArr[] 読み込む.croファイル名
   * @param num 個数
   * @param heap DLL読み込みに使用するヒープ
   * @note heapを設定しない場合はRoManagerのヒープを使用する
   */
  //--------------------------------------------------------------------------------
  void SetData( const char* croFileNameArr[],int num,gfl2::heap::HeapBase* heap = NULL );

  void Link( GameSys::GameProc* proc, gfl2::heap::HeapBase* pTmpHeap );

  void Start( int priority ) { m_pThread->Start(priority); }
  bool IsAlive( void ) { return m_pThread->IsAlive(); }

  //--------------------------------------------------------------------------------
  /**
   * @brief DLL破棄
   */
  //--------------------------------------------------------------------------------
  void DisposeModule();
  void DisposeModule(int index);

private:
  gfl2::thread::Thread* m_pThread;
  gfl2::heap::HeapBase* m_pHeap;

  virtual void Function( void );
};

GFL_NAMESPACE_END(GameSys)

#endif//__DLL_READ_THREAD_H__

