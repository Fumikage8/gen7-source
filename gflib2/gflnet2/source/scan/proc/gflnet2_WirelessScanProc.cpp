#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gfl_NetWirelessIdleProc.cpp
 * @brief	 ワイヤレス通信 アイドルプロセス
 * @author k.ohno
 * @date	 2011.1.6
 */
//=============================================================================

#include <gflnet2/source/scan/proc/gflnet2_WirelessScanProc.h>

#include <nn.h>
#include <nn/Result.h>
#include <nn/hid.h>
#include <nn/uds.h>


#include <gflnet2/source/scan/thread/gflnet2_WirelessCycleThread.h>
#include <gflnet2/source/connect/thread/gflnet2_WirelessInitThread.h>

#include <proc/include/gfl2_ProcManager.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)


//------------------------------------------------------------------
/**
 * @brief	  PROCの初期化を行う関数
            通信初期化
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
 */
//------------------------------------------------------------------

gfl2::proc::Result NetWirelessScanProc::InitFunc(gfl2::proc::Manager* pManager)
{
  switch(GetSubSeq()){
  case 0:
    //OHNO_PRINT("NetWirelessScanProc::InitFuncはいった\n");
    mpThread = GFL_NEW(mpNetBase->GetHeapBase()) NetWirlessInitThread( mpNetBase );
    if(mpThread){
      mpThread->Start( gfl2::thread::Thread::THREAD_PRI_NORMAL + 1 );
    }
    else{
      GFL_ASSERT(0); //@fix
    }
    AddSubSeq();
    break;
  case 1:
//    gfl2::thread::ThreadStatic::CurrentYield();
    if(mpThread){
      if( false == mpThread->IsAlive() ){
        //OHNO_PRINT("NetWirelessScanProc::InitFuncK\n");
        AddSubSeq();
      }
    }
    break;
  case 2:
    delThread();
    ResetSubSeq();
    //OHNO_PRINT("NetWirelessScanProc::InitFunc でた\n");
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}


//------------------------------------------------------------------
/**
 * @brief	  PROCのメインを行う関数
            親機ならネットワークの構築
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------

gfl2::proc::Result NetWirelessScanProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  NetWirelessCycleThread* pTH=NULL;
  switch(GetSubSeq()){
  case 0:
    if(mbEndReq){
      //OHNO_PRINT("UpdateFunc0抜け\n");
      return gfl2::proc::RES_FINISH;
    }
    //OHNO_PRINT("NetWirelessScanProc::UpdateFunc はいった\n");
    pTH = GFL_NEW(mpNetBase->GetHeapBase()) NetWirelessCycleThread( mpNetBase->GetBeaconManager(),mpNetBase->GetInitParam()->GetLimitNum(),
                                                                    mpNetBase->GetInitParam()->GetProductID(),mpNetBase->GetHeapBase() );
    mpThread = pTH;
    if(mpThread){
      pTH->SetNetApScan(mpAccessPointScanner);
      pTH->SetAccessPointHistory(mpAccessPointHistory);
      mpThread->Start( gfl2::thread::Thread::THREAD_PRI_NORMAL + 1 );
      AddSubSeq();
    }
    else{
      GFL_ASSERT(0); //@fix
      return gfl2::proc::RES_FINISH;
    }
    break;
  case 1:
    if(mbEndReq){
      //OHNO_PRINT("UpdateFunc1抜け\n");
      mpThread->KillNetThread();
    }
//    gfl2::thread::ThreadStatic::CurrentYield();
    if(mpThread){
      if( false == mpThread->IsAlive() ){
        AddSubSeq();
      }
      else
      {//スレッド生存中
        mpThread->UpdateBefore();//受信データ＆送信データの受け渡し
        mpThread->UpdateAfter();//受信データ＆送信データの受け渡し
      }
    }
    break;
  case 2:
    delThread();
    ResetSubSeq();
    //OHNO_PRINT("NSP::UpdateFunc でた\n");
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}



//------------------------------------------------------------------
/**
 * @brief	  PROCの終了を行う関数
            なにもしていない
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------

gfl2::proc::Result NetWirelessScanProc::EndFunc(gfl2::proc::Manager* pManager)
{
  nn::uds::Finalize();
  return gfl2::proc::RES_FINISH;
}


//------------------------------------------------------------------
/**
 * @brief	  終了処理
            なにもしていない
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------

int NetWirelessScanProc::ReqEnd(void)
{
  mbEndReq = true;
  return 1;
}

GFL_NAMESPACE_END(gflnet2)
#endif
