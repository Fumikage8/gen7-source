#ifndef __GFL2_THREAD_H__
#define __GFL2_THREAD_H__
//=============================================================================
/**
 * @file	 gfl2_Thread.h
 * @brief	 スレッド
 * @author k.ohno
 * @date	 2014.5.13
 */
//=============================================================================
#pragma once


#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>
#include "thread/include/gfl2_event.h"


/*
    今回のスレッドについては
    ThreadInterface::Functionが一回で呼ばれる引数を用意しております。
    一般的な使用方法の際に利用してください。
    スレッドは非同期であり。処理系によっては同時にうごきもします。（並列スレッド）
    メモリの取り扱いには十分気をつけてください。 

１．独立性を維持する
    使用するメモリを全部渡してスレッドを実行させ、終了後、結果を取得すれば、
    メモリの同時アクセスを気にする必要はありません。

２．メモリを共有する
    メモリを共有する場合には、
    それが１バイトだったとしても気をつけて書いて下さい。
    スレッドの優先順位も意識して設計する事。
    現在はMutexしか用意しませんので、その範囲内ですむように設計をお願いします。

３．スタックに注意をはらう
    スレッドとはメインと別のスタックにて動作します。
    デバッグ時にはスタックを壊していないか確認するプログラムを走らせます。

４．速度
    スレッドのCreateやDeleteはとても時間がかかる処理です。
    頻繁な生成破棄よりも、イベントの受け渡しによる動作が望ましいです。
    そう考えると１．のスレッドはまれなのかもしれません。
    目的に１対１対応した物を設計して利用してください。

*/


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)

class ThreadAdapter;


//-----------------------------------------------------------------------
/**
 * @brief このクラスを継承してスレッドの実装を書くこと
 */
//-----------------------------------------------------------------------
class ThreadInterface{
  GFL_FORBID_COPY_AND_ASSIGN(ThreadInterface);

public:
  ThreadInterface(void){};
  virtual ~ThreadInterface(){};

  //-----------------------------------------------------------------------
  /**
   * @brief 実行関数
   */
  //-----------------------------------------------------------------------
  virtual void Function(void) = 0;

};



class Thread{
  GFL_FORBID_COPY_AND_ASSIGN(Thread);

public:

  ///  プライオリティーの値は機種に依存している。
  ///  抽象的な形で実装すると、Aの上にBといった細かな設定が難しくなるので
  ///  HI,LOWと通常を数値で表している
  enum ThreadPri_e{
    THREAD_PRI_LOW = 31,
    THREAD_PRI_NORMAL = 16,
    THREAD_PRI_HI = 0,
  };




public:
  //-----------------------------------------------------------------------
  /**
   * @brief スレッドコンストラクタ
   * @param pInterFace   上に書いてあるクラス Function 実行関数を書いた物を登録
   * @param pHeap        スレッドのメモリ確保のために使用します
   * @param bMainLoop    Functionを繰り返し呼ぶ場合 true
   * @param stackSize    スタック領域のサイズ
   */
  //-----------------------------------------------------------------------
  Thread(ThreadInterface* pInterFace, gfl2::heap::HeapBase* pHeap, bool bMainLoop, u32 stackSize );
  virtual ~Thread();
  //-----------------------------------------------------------------------
  /**
   * @brief プライオリティーを取得する
   */
  //-----------------------------------------------------------------------
  int GetPriority(void);
  //-----------------------------------------------------------------------
  /**
   * @brief プライオリティーを設定する 
   */
  //-----------------------------------------------------------------------
  void SetPriority(int no);
  //-----------------------------------------------------------------------
  /**
   * @brief スレッドスタート
   * @param priority
   */
  //-----------------------------------------------------------------------
  void Start(void);
  void Start(int priority);
  //-----------------------------------------------------------------------
  /**
   * @brief スレッドが終わるまでブロックします
   */
  //-----------------------------------------------------------------------
  void Wait(void);
  //-----------------------------------------------------------------------
  /**
   * @brief イールド 処理系に再スケジュールの機会を与える。
   */
  //-----------------------------------------------------------------------
  void CurrentYield( void );
  //-------------------------------------------------------------------------
  /**
   * @brief カレントスレッドを指定時間停止させる
   *
   * @param milliSeconds スレッドを停止させる時間（ミリ秒）
   */
  //-------------------------------------------------------------------------
  void CurrentSleep( s32 milliSeconds );
  //-------------------------------------------------------------------------
  /**
   * @brief カレントスレッドを指定時間停止させる
   *
   * @param microSeconds スレッドを停止させる時間（マイクロ秒）
   */
  //-------------------------------------------------------------------------
  void CurrentSleepMicro( s32 microSeconds );
  //-------------------------------------------------------------------------
  /**
   * @brief   スレッドが終了したかどうか確認する
   * @return  終了していたらtrue
   */
  //-------------------------------------------------------------------------
  bool IsAlive(void);
  //-------------------------------------------------------------------------
  /**
   * @brief スレッドを終了処理へいかせる。強制終了ではないのでIsAliveで確認すること。
   */
  //-------------------------------------------------------------------------
  void Kill(void);

private:
  ThreadAdapter* mpThreadAdapter;
};


#if defined(GF_PLATFORM_CTR)

class SimpleStackArea  : public nn::os::StackMemory
{
  GFL_FORBID_COPY_AND_ASSIGN( SimpleStackArea );

  public:

    //-----------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap      スタック領域の確保に使用するヒープ
     * @param stackSize スタックサイズ
     */
    //-----------------------------------------------------------------------
    SimpleStackArea( gfl2::heap::HeapBase* heap, u32 stackSize ) : 
      m_stackArea( NULL ),
      m_stackSize( stackSize )
    {
      m_stackArea = GflHeapAllocMemory( heap, stackSize );
    }

    //-----------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-----------------------------------------------------------------------
    virtual ~SimpleStackArea()
    {
      GflHeapFreeMemory( m_stackArea );
    }

    //-----------------------------------------------------------------------
    /**
     * @brief スタック領域の底辺のアドレスを取得する
     */
    //-----------------------------------------------------------------------
    inline uptr GetStackBottom( void )
    {
      return reinterpret_cast<uptr>(m_stackArea) + m_stackSize;
    }

   protected:
     void* m_stackArea;
     u32 m_stackSize;
};


    class SafeStackArea : public nn::os::StackMemory
    {
      GFL_FORBID_COPY_AND_ASSIGN( SafeStackArea );

      public:

      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       * @param heap      スタック領域の確保に使用するヒープ
       * @param stackSize スタックサイズ
       */
      //-----------------------------------------------------------------------
      SafeStackArea( gfl2::heap::HeapBase* heap, u32 stackSize ) : 
        StackMemory(),
        m_stackArea( NULL ),
        m_stackSize( stackSize )
      {
        GFL_ASSERT( m_stackSize % 4096 == 0 );  //@check
        m_stackArea = GflHeapAllocMemoryAlign4096( heap, m_stackSize );
        this->Initialize( m_stackArea, m_stackSize );
      }

      //-----------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //-----------------------------------------------------------------------
      virtual ~SafeStackArea()
      {
        this->Finalize();
        if( m_stackArea != NULL )
        {
          GflHeapFreeMemory( m_stackArea );
          m_stackArea = NULL;
        }
      }


      protected:
      void* m_stackArea;
      u32 m_stackSize;
    };
#endif defined(GF_PLATFORM_CTR)



GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif __GFL2_THREAD_H__
