#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_IrAutoMatchThread.cpp
 * @brief	 赤外線 自動マッチング
 * @author k.ohno
 * @date	 2012.4.12
 */
//=============================================================================


#include <nn.h>
#include <nn/Result.h>
#include <nn/ir.h>



#include "gflnet2/include/p2p/gflnet2_NetGame.h"
#include "gflnet2/source/p2p/gflnet2_NetGameBase.h"
#include <gflnet2/source/dev/gflnet2_IrAdapter.h>
#include "gflnet2_IrAutoMatchThread.h"

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)


//------------------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//------------------------------------------------------------------------------------

IrAutoMatchThread::IrAutoMatchThread( gflnet2::Base* pnetbase )
     : NetThread( pnetbase->GetHeapBase() )
{
  mpNetBase = pnetbase;
  bSuccess = false;
  mpTransferedIDReceiver = NULL;
}



//------------------------------------------------------------------
/**
 * @brief	  通信初期化
 */
//------------------------------------------------------------------

void IrAutoMatchThread::Main( void )
{
  nn::Result result;
  gflnet2::dev::IrAdapter* pIrAdapter = gflnet2::dev::IrAdapter::GetInstance();
  InitParam* pInit = mpNetBase->GetInitParam();
  u64 id = pInit->GetTransferedID();
  //OHNO_PRINT("自分の %llu \n", id);

  while(Yield()){
    int ret = pIrAdapter->ConnectNonBlock(pInit->GetGameID());
    if(ret==1){
      break;
    }
    else if(ret == -1){
      //OHNO_PRINT("終了 IrAutoMatchThread::Main\n");
      Kill();  //自分を終了させる
      bSuccess = false;
      return;
    }
  }

//  bool ret = pIrAdapter->Connect();

//  if(ret == false){
  //  //OHNO_PRINT("終了 IrAutoMatchThread::Main\n");
    //Kill();  //自分を終了させる
//    return;
  //}

  if(!pIrAdapter->IsMaster()){
    mpTransferedIDReceiver->SetChild();

    while(Yield()){
      void* pRecv = pIrAdapter->GetBeacon(0, sizeof(u64));
      if(pRecv){
        u64* pU64 = (u64*)pRecv;
        //OHNO_PRINT("ビーコン受信 %llu \n", *pU64);
        mpTransferedIDReceiver->Set(*pU64);
        
        break;
      }
    }
    while(Yield()){
      if(pIrAdapter->SetBeacon((const u8*)&id, sizeof(u64))){
        //OHNO_PRINT("ビーコン送信 %llu\n",id);
        break;
      }
    }


    id=0;
    while(Yield()){
      void* pRecv = pIrAdapter->GetBeacon(0, sizeof(u64));
      if(pRecv){
        break;
      }
    }
    while(Yield()){
      if(pIrAdapter->SetBeacon((const u8*)&id, sizeof(u64))){
        break;
      }
    }
    

  }
  else{
    gflnet2::p2p::NetGame::GetNetGameBase()->SetMaster();

    mpTransferedIDReceiver->SetMaster();
    while(Yield()){
      if(pIrAdapter->SetBeacon((const u8*)&id, sizeof(u64))){
        //OHNO_PRINT("ビーコン送信 %llu\n",id);
        break;
      }
    }

    while(Yield()){
      void* pRecv = pIrAdapter->GetBeacon(0, sizeof(u64));
      if(pRecv){
        u64* pU64 = (u64*)pRecv;
        //OHNO_PRINT("ビーコン受信 %llu \n", *pU64);
        mpTransferedIDReceiver->Set(*pU64);
        break;
      }
    }
    //からループ
    while(Yield()){
      id=0;
      if(pIrAdapter->SetBeacon((const u8*)&id, sizeof(u64))){
        break;
      }
    }
    while(Yield()){
      void* pRecv = pIrAdapter->GetBeacon(0, sizeof(u64));
      if(pRecv){
        break;
      }
    }

    
  }
  bSuccess = true;
  Kill();  //自分を終了させる
}


//------------------------------------------------------------------
/**
 * @brief	  
 */
//------------------------------------------------------------------

bool IrAutoMatchThread::Yield( void )
{
  nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );

  gflnet2::dev::IrAdapter* pIrAdapter = gflnet2::dev::IrAdapter::GetInstance();
  pIrAdapter->Update();
  
  return !GetKillNetThread();
}


GFL_NAMESPACE_END(gflnet2)
#endif
