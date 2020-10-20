#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gfl_IrScanProc.cpp
 * @brief	 赤外線通信 スキャンプロセス
 * @author k.ohno
 * @date	 2011.1.6
 */
//=============================================================================

#include <gflnet2/source/scan/proc/gflnet2_IrScanProc.h>

#include <nn.h>
#include <nn/Result.h>
#include <nn/hid.h>
#include <nn/uds.h>

#include <gflnet2/source/dev/gflnet2_IrAdapter.h>

GFL_NAMESPACE_BEGIN(gflnet2)


//------------------------------------------------------------------
/**
 * @brief	  PROCの初期化を行う関数
            通信初期化
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
 */
//------------------------------------------------------------------

gfl2::proc::Result IrScanProc::InitFunc(gfl2::proc::Manager* pManager)
{
  gflnet2::dev::IrAdapter::GetInstance()->Initialize(mpNetBase->GetHeapBase());
  ResetSubSeq();
  //OHNO_PRINT("IrScanProc::InitFunc でた\n");
  return gfl2::proc::RES_FINISH;
}


//------------------------------------------------------------------
/**
 * @brief	  PROCのメインを行う関数
            親機ならネットワークの構築
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------

gfl2::proc::Result IrScanProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  gflnet2::dev::IrAdapter* pIrAdapter = gflnet2::dev::IrAdapter::GetInstance();
  u64 id = mpNetBase->GetInitParam()->GetTransferedID();
  nn::fnd::TimeSpan timeOut = nn::fnd::TimeSpan::FromMilliSeconds(100);

  switch(GetSubSeq()){
  case 0:
    {
      int ret = pIrAdapter->ConnectNonBlock(mpNetBase->GetInitParam()->GetGameID());
      if(ret==1){
        AddSubSeq();
      }
    }
    break;
  case 1:
    if(!pIrAdapter->IsMaster()){
      mpTransferedIDReceiver->SetChild();
    }
    else{
      mpTransferedIDReceiver->SetMaster();
    }
    AddSubSeq();
    mCount=20;
    break;
  case 2:
    if(!pIrAdapter->IsMaster()){
      void* pRecv = pIrAdapter->GetBeaconTest(0, sizeof(u64), timeOut);
      if(pRecv){
        u64* pU64 = (u64*)pRecv;
        //OHNO_PRINT("ビーコン受信 %llu \n", *pU64);
        mpTransferedIDReceiver->Set(*pU64);
        AddSubSeq();
        break;
      }
    }
    else{
      if(pIrAdapter->SetBeaconTest((const u8*)&id, sizeof(u64))){
        //OHNO_PRINT("ビーコン送信 %llu\n",id);
        mCount--;
        if(mCount==0){
          AddSubSeq();
        }
        break;
      }
    }
    break;
  case 3:
    if(pIrAdapter->IsMaster()){
      void* pRecv = pIrAdapter->GetBeaconTest(0, sizeof(u64),timeOut);
      if(pRecv){
        u64* pU64 = (u64*)pRecv;
        //OHNO_PRINT("ビーコン受信 %llu \n", *pU64);
        mpTransferedIDReceiver->Set(*pU64);
        AddSubSeq();
        break;
      }
    }
    else{
      if(pIrAdapter->SetBeaconTest((const u8*)&id, sizeof(u64))){
        //OHNO_PRINT("ビーコン送信 %llu\n",id);
        mCount--;
        if(mCount==0){
          AddSubSeq();
        }
        break;
      }
    }
    break;
  case 4:
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

gfl2::proc::Result IrScanProc::EndFunc(gfl2::proc::Manager* pManager)
{
  gflnet2::dev::IrAdapter::GetInstance()->Disconnect();
  gflnet2::dev::IrAdapter::GetInstance()->Finalize();
  gflnet2::dev::IrAdapter::GetInstance()->DestroyInstance();

  return gfl2::proc::RES_FINISH;
}


//------------------------------------------------------------------
/**
 * @brief	  終了処理
            なにもしていない
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------

int IrScanProc::ReqEnd(void)
{
  mbEndReq = true;
  return 1;
}

GFL_NAMESPACE_END(gflnet2)
#endif
