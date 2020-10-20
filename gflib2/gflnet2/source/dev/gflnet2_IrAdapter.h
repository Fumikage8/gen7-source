#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_IRADAPTER_H__
#define __GFLNET_IRADAPTER_H__
//=============================================================================
/**
 * @file	 gflnet_IrAdapter.h
 * @brief	 通信管理クラス Ir用
 * @author k.ohno
 * @date	 2012.3.20
 */
//=============================================================================

#include "gflnet2/include/base/gflnet2_NetConfig.h"
#include "gflnet2/include/base/gflnet2_NetAdapter.h"

#include "gflnet2/include/dev/gflnet2_IrAdapterErrorListener.h" // for ErrorListener

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(dev)

#define GFLNET_IR_MAXPACKET (0xe0)


class IrAdapter : public gflnet2::NetAdapter{
  GFL_FORBID_COPY_AND_ASSIGN(IrAdapter);

  static IrAdapter* mpInstance;
  
  nn::os::Event maReceiveEvent;  //イベント
  nn::os::Event maSendEvent;     //イベント
  nn::os::Event maConnectEvent;  //イベント  //nn::os::Event maStatusUpdateEvent;  //イベント

  static const int IRSEND_BUFFER = 1024;  //送信用バッファ
  static const int COMFIRM_FAILED_MAX = 10;
  static const int RECV_FAILED_MAX = 10;
  
  size_t mReceiveSize;
  size_t mAddRecvSize;
  void* mpSendBuffer;
  void* mpRecvBuffer;
  void* mpInitBuffer;
  InitParam* mpInitParam;  //初期化クラス
  int mCountFailed;

  int mRecvFailedCount;

  bool bConnectStatus;
  bool bInit;

  // エラー通知用リスナー
  static IrAdapterErrorListener* m_errorListener;

public:
  IrAdapter(const gflnet2::InitParameter* pIParam, const gflnet2::InitParameterFixed* pIParamFix);
  virtual ~IrAdapter(){};
  
  static void CreateInstance(gfl2::heap::HeapBase* pHeapBase,const gflnet2::InitParameter* pIParam, const gflnet2::InitParameterFixed* pIParamFix);
  static void DestroyInstance(void);
  static IrAdapter* GetInstance(void){ return mpInstance; };


  virtual bool Initialize(gfl2::heap::HeapBase* pHeapSys);   ///初期化

  virtual bool Connect(void);   ///< 使用禁止

  virtual bool Scan(void);

  virtual bool SetBeacon(const u8* pData, const u32 size);

  virtual void* GetBeacon(const int index, const u32 size);
  void* GetBeaconTest(const int index, const u32 size, nn::fnd::TimeSpan timeOut=0);

  virtual bool CreateMaster(void);       ///< お互いの接続

  virtual bool Matching(int TransferedID);   ///< お互いの接続

  virtual bool Update(void);

  virtual bool SendData(void* pData, const u32 size);
  bool SetBeaconTest(const u8* pData, const u32 size);

 // virtual void* RecvData(int id);

  virtual void* RecvDataFunc(int id, size_t* pSize);

  virtual bool Disconnect(void);  ///<切断

  virtual bool Finalize(void);  ///< 解放＋終了処理

  virtual bool IsMaster(void);

  virtual bool IsMatched(void){ return bConnectStatus; };  ///< 接続済み
  virtual bool IsConnected(void){ return bConnectStatus; };  ///< 接続済み


  //---------------------------------------------------------------------------------
  /**
   * @brief   受信データの破棄
   */
  //---------------------------------------------------------------------------------
  void ClearReceiveBuffer(void);
  
public:
/*
  inline bool SendMode(void){ return bSendRecvSwitch; };
  inline bool RecvMode(void){ return (bSendRecvSwitch==false); };
  inline void SendEnd(void){ bSendRecvSwitch = false; };
  inline void RecvEnd(void){ bSendRecvSwitch = true; };
*/

public:
#if GFL_DEBUG
  int mDebugStatus;
#endif
  
private:
  bool localSendData(const void* pData,int sendsize, void* tempBuff, int tempsize, nn::os::Event* pSendEvent);
//  bool localRecvData(void* tempBuff, int size, int tempsize, nn::os::Event* pRecvEvent);
  int noloopRecvData(bit8* tempBuff, size_t bufferSize, nn::os::Event* pRecvEvent,size_t* pReceiveSize);
  bool localRecvDataNoLoop(void* tempBuff, int size, int tempsize, nn::os::Event* pRecvEvent);
  
  nn::Result ConfirmID(u8 GameID);


  int ConnectBase(u8 GameID);
public:
  int ConnectNonBlock(u8 GameID);

  //------------------------------------------------------------------
  /**
   * @brief   エラーリスナー登録
   * @param   listener   通知を受け取るIrAdapterErrorListener
   */
  //------------------------------------------------------------------
  static void RegistErrorListener( IrAdapterErrorListener* listener );

  //------------------------------------------------------------------
  /**
   * @brief   エラーリスナー解除
   */
  //------------------------------------------------------------------
  static void UnregistErrorListener( void );

};

GFL_NAMESPACE_END(ir)
GFL_NAMESPACE_END(gflnet2)

#endif // __GFLNET_IRADAPTER_H__
#endif
