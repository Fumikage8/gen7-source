#ifdef  GF_PLATFORM_CTR


#ifndef __GFLNET_IRP2PTHREAD_H__
#define __GFLNET_IRP2PTHREAD_H__

#pragma once
//=============================================================================
/**
 * @file	 gflnet_IrP2pThread.h
 * @brief	 赤外線 スレッドでの送受信
 * @author k.ohno
 * @date	 2012.4.12
 */
//=============================================================================


#include <gflnet2/include/base/gflnet2_NetThread.h>


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(p2p)




class IrP2pThread : public gflnet2::base::NetThread {
  GFL_FORBID_COPY_AND_ASSIGN( IrP2pThread ); // コピーコンストラクタ, 代入演算子を禁止
private:

  static const int ERROR_NONE=0;
  static const int ERROR_CRCFAILED=1;
  static const int ERROR_SENDCRCFAILED=2;

  static const int IR_COMMANDBUFFSIZE = 4096;  //ライブラリの受付バッファ
  static const int IR_SENDSIZE = (( 48-10 ) * 4);     //赤外線自体の制御バッファ   256 * 54 = 13824 転送可能

  u8* mpSendBuff;
  u8* mpRecvBuff;

  u8 mInc;  //インクリメント番号
  u8 mGetInc;  //送られてきたインクリメント番号

  u16 mRecvInc;


  u32 mSendSize;
  u32 mRecvSize;
  u16 mRecvCRC;  //さっき受け取った奴ははじくため

  typedef struct {
    u16 ArkCRC;   // 受信したCRC
    u16 ArkInc;   // 受信したInc
    u16 CRC;      // 送信するCRC
    u16 TotalSize;  // 送られるサイズ
    u8 increment;  // インクリメントするだけ
    u8 SendInc;  // 送信側インデックス
    u8 data[ IR_SENDSIZE ];
  } Command;

  Command* mpCommand1Send;
  Command* mpCommand1Recv;



#ifdef NN_BUILD_VERBOSE   //dbmkのときだけ
  static const int DebugBufferNum = 10;
  Command mDebugCommandSend[10];
  Command mDebugCommandRecv[10];
  int debugSend;
  int debugRecv;

  void DebugSet(bool bSend,const Command* x);

#endif

public:

  //------------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //------------------------------------------------------------------------------------
  IrP2pThread( gfl2::heap::HeapBase* heap );
  //------------------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //------------------------------------------------------------------------------------
  virtual ~IrP2pThread();

private:


  //----------------------------------------------------------------------------
  /**
   * @brief メイン処理
   * ※スレッド関数のループ内で呼ばれ続ける関数です。
   */
  //----------------------------------------------------------------------------
  virtual void Function( void );





//  bool Yield( void );


  bool GetData(void* memory,int size,nn::fnd::TimeSpan timeOut=0);
  bool SetData(void* memory,int size);
  void EachDataChild(void* sendmemory,void* recvmemory,int size);
  void EachDataParent(void* sendmemory,void* recvmemory,int size);
  bool CRCCheck(Command* addr, u16 crc);
  void Command1to2Copy(void);
  bool CommandErrCheck(Command* send,Command* recv);
  void CommandSetup(bool bfirst);
  void CommandResult(void);
  void CRCWrite(Command* cmd);


  void ThreadEnd(void);

public:


  //データ送信
  //----------------------------------------------------------------------------
  /**
   * @brief  送信
   * @return 送れたらTRUE
   */
  //----------------------------------------------------------------------------
  bool SendData(void* data, u32 size );

  //----------------------------------------------------------------------------
  /**
   * @brief データ受信
   * @return 0だと何も入っていない
   */
  //----------------------------------------------------------------------------
  u32 RecvData(void* data, u32 size );
  //void* RecvData(u32* size );

 // void Main2( void );

};

GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)


#endif// __GFLNET_IRP2PTHREAD_H__

#endif
