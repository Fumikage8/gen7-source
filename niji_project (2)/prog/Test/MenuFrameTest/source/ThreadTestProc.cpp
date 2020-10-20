//=============================================================================================
/**
 * @file   MenuTestProc.cpp
 * @brief  呼び出しのProcから   ここは作ってもらう
 * @author k.ohno
 * @date   2015.2.16
 */
//=============================================================================================
#if PM_DEBUG

#include "Test/MenuFrameTest/include/ThreadTestProc.h"
#include "System/include/HeapDefine.h"
#include <system/include/GflUse.h>

#include "Thread/include/gfl2_ThreadStatic.h"


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ThreadTest )



Proc::Proc(void)  //コンストラクタ
{
}


Proc::~Proc(void)  //デストラクタ
{
}




//------------------------------------------------------------------
/**
 * @brief   PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result Proc::InitFunc(gfl2::proc::Manager* pManager)
{
  //HEAPID_APP  本体に関するものはAPPめもりーから確保します
  gfl2::heap::HeapBase *appHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
	gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  // このアプリ全体に必要なメモリを作成
  mpAppHeap = GFL_NEW(appHeap) app::util::Heap();
  // ローカルヒープを作る
	mpAppHeap->LocalHeapCreate(appHeap,devHeap,SIZE_MY_APP,SIZE_MY_APPDEV);


  //以下スレッドテスト
  mLockTest=0;
  m_criticalSection.Initialize();
	mpFunction = GFL_NEW(mpAppHeap->GetDeviceHeap() ) ThreadFunc(this);
  mpThread = GFL_NEW(mpAppHeap->GetDeviceHeap()) gfl2::thread::Thread( mpFunction, mpAppHeap->GetSystemHeap(), false, 4096);

  return gfl2::proc::RES_FINISH;

}




//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result Proc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  
  switch(mSeq){
  case 0:
    mpThread->Start( gfl2::thread::Thread::THREAD_PRI_LOW );

    for ever {
      scopeTest();
      if(mLockTest>10){
        mSeq = 1;
				break;
      }
      gfl2::thread::ThreadStatic::CurrentSleep(100);
    }
    break;
  case 1:
    
    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;

}


//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン描画を行う関数
 *          純粋仮想ではありません。 順番を変更しました
 */
//------------------------------------------------------------------
void Proc::DrawFunc(gfl2::proc::Manager* pManager,gfl2::gfx::CtrDisplayNo no)
{
}


//------------------------------------------------------------------
/**
 * @brief   PROCのメインを行う関数
 *  1/30フレームの②フレーム目の処理です。
 *  1/60フレーム動作時には、使われません。
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result Proc::NextFrameMainFunc(gfl2::proc::Manager* pManager)
{
  return gfl2::proc::RES_CONTINUE;
}


//------------------------------------------------------------------
/**
 * @brief   PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
gfl2::proc::Result Proc::EndFunc(gfl2::proc::Manager* pManager)
{
  GFL_DELETE mpThread;
  GFL_DELETE mpFunction;
  GFL_DELETE mpAppHeap;
  return gfl2::proc::RES_FINISH;
}



//--------------------------------------------------------------
// スコープテスト
//--------------------------------------------------------------
void Proc::scopeTest(void)
{
  gfl2::thread::ScopedLock lock(m_criticalSection);
  mLockTest++;
  GFL_PRINT("足した\n");
}


//-----------------------------------------------------------------------------
/**
 *					実装
 */
//-----------------------------------------------------------------------------


Proc::ThreadFunc::ThreadFunc(Proc* pThreadSampleEvent)
{
	mpThreadThis = pThreadSampleEvent;
}

void Proc::ThreadFunc::Function(void)
{
  gfl2::thread::ScopedLock lock(mpThreadThis->m_criticalSection);

  for ever {
    mpThreadThis->mLockTest--;
    if(mpThreadThis->mLockTest < -10){
      break;
    }
    GFL_PRINT("引いた\n");
    gfl2::thread::ThreadStatic::CurrentSleep(100);
  }
}




GFL_NAMESPACE_END( ThreadTest )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG
