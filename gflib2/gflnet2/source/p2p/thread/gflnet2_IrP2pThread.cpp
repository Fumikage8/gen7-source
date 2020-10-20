#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_IrP2pThread.cpp
 * @brief	 赤外線通信 スレッドで単体相互通信
 * @author k.ohno
 * @date	 2012.9.14
 */
//=============================================================================


#include <nn.h>
#include <nn/Result.h>
#include <nn/ir.h>

#include "gflnet2_IrP2pThread.h"
#include <math/include/gfl2_math_crc.h>
#include <gflnet2/source/dev/gflnet2_IrAdapter.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(p2p)


//------------------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//------------------------------------------------------------------------------------

IrP2pThread::IrP2pThread( gfl2::heap::HeapBase* heap )
     : gflnet2::base::NetThread( heap )
{
  
  mpSendBuff = (u8*)GflHeapAllocMemory( heap, IR_COMMANDBUFFSIZE );   //送信バッファ
  mpRecvBuff = (u8*)GflHeapAllocMemory( heap, IR_COMMANDBUFFSIZE );   //送信バッファ

  mpCommand1Recv = (Command*)GflHeapAllocMemoryAlign( heap, IR_SENDSIZE+16, 16 );   //送受信バッファ
  mpCommand1Send = (Command*)GflHeapAllocMemoryAlign( heap, IR_SENDSIZE+16, 16 );   //送受信バッファ

  gflnet2::dev::IrAdapter* pIrAdapter = gflnet2::dev::IrAdapter::GetInstance();
  GFL_ASSERT(pIrAdapter);  //@fix

  mGetInc=0xff;
  mInc=0;
  mpCommand1Recv->ArkCRC = 0xffff;
  mpCommand1Send->ArkCRC = 0;
  mpCommand1Recv->ArkInc = 0;
  mpCommand1Send->ArkInc = 0;

  mRecvSize = 0;
  mpCommand1Send->TotalSize = 0;  //送っていない場合０ 送っている場合トータルサイズ
  mRecvCRC=1;
  mRecvInc = 0;

  mSendSize = 0;
  //OHNO_PRINT("コンストラクタ\n");

}


IrP2pThread::~IrP2pThread()
{
  GflHeapFreeMemory( mpSendBuff );
  GflHeapFreeMemory( mpRecvBuff );
  GflHeapFreeMemory( mpCommand1Recv );
  GflHeapFreeMemory( mpCommand1Send );
  //OHNO_PRINT("デストラクタ\n");
}



bool IrP2pThread::GetData(void* memory,int size, nn::fnd::TimeSpan timeOut)
{
  gflnet2::dev::IrAdapter* pIrAdapter = gflnet2::dev::IrAdapter::GetInstance();
  void* pRecv = gflnet2::dev::IrAdapter::GetInstance()->GetBeaconTest(0, size, timeOut);
  if(pRecv){
    memcpy(memory, pRecv, size);
    return true;
  }
  return false;
}



bool IrP2pThread::SetData(void* memory,int size)
{
  bool ret = gflnet2::dev::IrAdapter::GetInstance()->SetBeaconTest((const u8*)memory, size);
#ifdef NN_BUILD_VERBOSE
  if(ret){
//    DebugSet(true,mpCommand1Send);
  }
  else{
    //OHNO_PRINT("送信miss\n");
  }
#endif
  return ret;
}


void IrP2pThread::EachDataParent(void* sendmemory,void* recvmemory,int size)
{
  int sendTimeOutVal = 400;// 親機側がタイムアウトしてパケットを送りなおすまでの時間(ms)


  gflnet2::dev::IrAdapter* pIrAdapter = gflnet2::dev::IrAdapter::GetInstance();

  nn::fnd::TimeSpan timeOut = nn::fnd::TimeSpan::FromMilliSeconds(sendTimeOutVal);

  while(1){
    
    if(GetKillNetThread()){
      return;
    }
    if(mRecvSize==0){  //受け取って引き出された時
      CommandSetup(false);  //送信準備
      if(!SetData(sendmemory,size)){  //送れないという事は自分の内部バッファがいっぱいの時
        continue;  //リトライ
      }
      if(GetData(  recvmemory,size, timeOut)){  //取得
        if( CRCCheck(mpCommand1Recv,mpCommand1Recv->CRC) ){  //受け取ったもののCRCを検査し
          if( ((mRecvInc == 0xff)&&( mpCommand1Recv->increment==0)) || (mpCommand1Recv->increment==(mRecvInc+1)) ){   //前回と違うものである
            // 受信処理
            CommandResult();
          }
        }
      }
    }
    else{
      nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(4) );
    }
  }
}




void IrP2pThread::EachDataChild(void* sendmemory,void* recvmemory,int size)
{
//  int sendTimeOutVal = 200;// 親機側がタイムアウトしてパケットを送りなおすまでの時間(ms)
  int receiveTimeOutVal = 5000; //子機が親機のデータを待つ時間はながいになっている


  gflnet2::dev::IrAdapter* pIrAdapter = gflnet2::dev::IrAdapter::GetInstance();

  nn::fnd::TimeSpan timeOut = nn::fnd::TimeSpan::FromMilliSeconds(receiveTimeOutVal);
  
  while(1){

    if(GetKillNetThread()){
      return;
    }
    if(mRecvSize==0){  //受け取って引き出された時
      if(GetData(  recvmemory,size, timeOut)){  //取得
        if( CRCCheck(mpCommand1Recv,mpCommand1Recv->CRC) ){  //受け取ったもののCRCを検査し
          if( ((mRecvInc == 0xff)&&( mpCommand1Recv->increment==0)) || (mpCommand1Recv->increment==(mRecvInc+1)) ){   //前回と違うものである
            // 受信処理
            CommandResult();
          }
        }
      }
      else{
        continue;
      }
    }
    else{
      nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(4) );
      continue;
    }
    CommandSetup(false);  //送信準備
    if(!SetData(sendmemory,size)){  //送れないという事は自分の内部バッファがいっぱいの時 ここではみない
//      //OHNO_PRINT("送れてない\n");
    }
  }
}




bool IrP2pThread::CRCCheck(Command* addr, u16 crc)
{
  u16* pAddr = (u16*)addr;
  
  gfl2::math::Crc MathCrc;
  MathCrc.Initialize();

  u16 retcrc = MathCrc.Crc16(0,(u8*)&pAddr[3], sizeof(Command)-6 );
  if(retcrc == crc){
    return true;
  }
  return false;
}

void IrP2pThread::CRCWrite(Command* cmd)
{
  u16* pAddr = (u16*)cmd;
  cmd->increment++;

    
  gfl2::math::Crc MathCrc;
  MathCrc.Initialize();

  cmd->CRC = MathCrc.Crc16(0,(u8*)&pAddr[3], sizeof(Command)-6 );
}


void IrP2pThread::CommandSetup(bool bfirst)
{
  if(mSendSize){
    if(((mpCommand1Recv->ArkCRC == mpCommand1Send->CRC) && (mpCommand1Recv->ArkInc == mpCommand1Send->increment))   || bfirst){  //これは順当に値が帰ってきた場合  エラーではない
      mInc++;
      if((mInc*IR_SENDSIZE) >= mSendSize){
        mpCommand1Send->TotalSize = 0;
        mSendSize = 0;   //送信終了
      }
      else{
        mpCommand1Send->SendInc = mInc;
        memcpy(mpCommand1Send->data, (u8*)&mpSendBuff[ mInc*IR_SENDSIZE ], IR_SENDSIZE);
        mpCommand1Send->TotalSize = mSendSize;
        CRCWrite(mpCommand1Send);

#ifdef NN_BUILD_VERBOSE
        DebugSet(true,mpCommand1Send);
#endif

      }
    }
  }
}


void IrP2pThread::CommandResult(void)
{
  if(mRecvSize==0){  //受け取って引き出された時
    if(mpCommand1Recv->TotalSize){  //サイズがある時が到着

#ifdef NN_BUILD_VERBOSE
      DebugSet(false,mpCommand1Recv);
#endif

      mRecvCRC = mpCommand1Recv->CRC;
      mRecvInc = mpCommand1Recv->increment;

      mGetInc++;
      GFL_ASSERT_MSG(mGetInc == mpCommand1Recv->SendInc,"mGetInc != mpCommand1Recv->SendInc %d %d \n",mGetInc, mpCommand1Recv->SendInc); //@fix

      mpCommand1Send->ArkCRC = mRecvCRC;
      mpCommand1Send->ArkInc = mRecvInc;

      memcpy((u8*)&mpRecvBuff[ mGetInc * IR_SENDSIZE ], mpCommand1Recv->data, IR_SENDSIZE);

      if(((mGetInc+1)*IR_SENDSIZE) >= mpCommand1Recv->TotalSize){

        mRecvSize = mpCommand1Recv->TotalSize;  //全部受け取ったとき
      }
    }
    else{
      //サイズがない時はから
      mRecvCRC = mpCommand1Recv->CRC;
      mRecvInc = mpCommand1Recv->increment;
      mpCommand1Send->ArkCRC = mRecvCRC;
      mpCommand1Send->ArkInc = mRecvInc;
      //OHNO_PRINT("サイズがない時はから\n");
    }
    
  }
}

  
//------------------------------------------------------------------
/**
 * @brief	  通信初期化
 */
//------------------------------------------------------------------

void IrP2pThread::Function( void )
{
  gflnet2::dev::IrAdapter* pIrAdapter = gflnet2::dev::IrAdapter::GetInstance();
  if(pIrAdapter->IsMaster()){
    EachDataParent(mpCommand1Send, mpCommand1Recv, sizeof(Command));  //送受信
  }
  else{
    EachDataChild(mpCommand1Send, mpCommand1Recv, sizeof(Command));  //送受信
  }
  Kill();
}

//----------------------------------------------------------------------------
/**
 * @brief  送信開始
 * @return 送れたらTRUE
 */
//----------------------------------------------------------------------------
bool IrP2pThread::SendData(void* data, u32 size )
{
  if((mSendSize == 0) && (size != 0) ){
    memcpy( mpSendBuff , data, size);
    mInc = 0xff;
    mSendSize = size;
//    CommandSetup(true);
    return true;
  }
// GFL_PRINT("IrP2pThread SendFailed %d\n",size);
  return false;
}

//----------------------------------------------------------------------------
/**
 * @brief データ受信
 * @return 0だと何も入っていない
 */
//----------------------------------------------------------------------------
u32 IrP2pThread::RecvData(void* data, u32 size )
{
  if(mRecvSize){
    u32 sizes = mRecvSize;
    memcpy( data, mpRecvBuff, mRecvSize);
    mGetInc=0xff;
    mRecvSize=0;
    return sizes;
  }
  return 0;
}

//----------------------------------------------------------------------------
/**
 * @brief データ受信
 * @return NULLだと何も入っていない
 */
//----------------------------------------------------------------------------
/*
void* IrP2pThread::RecvData(u32* size )
{
  if(mRecvSize){
    *size = mRecvSize;
    mRecvSize=0;
    mGetInc=0xff;
    return mpRecvBuff;
  }
  return NULL;
}
*/


#ifdef NN_BUILD_VERBOSE   //dbmkのときだけ

void IrP2pThread::DebugSet(bool bSend,const Command* x)
{
  int no = 0;
  Command* y = NULL;

  if(bSend){
    no = debugSend;
    y = &mDebugCommandSend[no];
  }
  else{
    no = debugRecv;
    y = &mDebugCommandRecv[no];
  }
  memcpy(y,x,sizeof(Command));
  no++;
  if(no>=10){
    no=0;
  }
  
  if(bSend){
    debugSend = no;
  }
  else{
    debugRecv = no;
  }
}




#endif



GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)
#endif
