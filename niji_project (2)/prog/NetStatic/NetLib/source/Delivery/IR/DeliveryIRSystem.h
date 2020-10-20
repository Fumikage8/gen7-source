#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_DeliveryIRSystem.h
 * @brief  赤外線配信システムクラス
 * @author Shin Kosaka
 * @date   2012.8.21
 */
//=============================================================================
#ifndef __GFLNET_DELIVERY_IR_SYSTEM_H__
#define __GFLNET_DELIVERY_IR_SYSTEM_H__

#include <nn.h>

#include <NetStatic/NetLib/source/Delivery/DeliverySystem.h>
#include <NetStatic/NetLib/include/Delivery/DeliveryRecvListener.h>


namespace NetLib
{
  namespace P2P
  {
    class P2pConnectionManager;
  }
}

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(NetLib)
/**
* @namespace NetAppLib::delivery
* @brief     配信関連
*/
GFL_NAMESPACE_BEGIN(Delivery)




/***************************************************************************!
 * @brief  赤外線配信システムクラス
 ***************************************************************************/
class DeliveryIRSystem : public NetLib::Delivery::DeliverySystem
{
  GFL_FORBID_COPY_AND_ASSIGN(DeliveryIRSystem); //コピーコンストラクタ＋代入禁止

private:

  enum
  {
    DELIVERY_DATA_SIZE = 1024 * 200,//データサイズ200キロバイト
    MAX_RECV_INDEX = 4,
    MAX_SEND_SIZE = 2000,
    CHECK_TIMING_NO = 4,
    END_TIMING_NO =5,
    PEER_TYPE_SEND = 1,
    PEER_TYPE_RECV = 2,
  };

  typedef enum
  {
    IR_STATE_START = 0,
    IR_STATE_CONNECT,
    IR_STATE_CONNECT_WAIT,
    IR_STATE_CONNECTION_CHECK,
    IR_STATE_CONNECTION_CHECK_WAIT,
    IR_STATE_CONNECTION_CHECK_TIMING,
    IR_STATE_SEND_HEADER,
    IR_STATE_SEND_BODY,
    IR_STATE_RECV,
    IR_STATE_NETGAME_COMM,
    IR_STATE_TIMING_WAIT,
    IR_STATE_END_START,
    IR_STATE_CONNECT_END,
    IR_STATE_SHUTDOWN,
  }IR_STATE;


  typedef struct
  {
    u16 crc16;
    u16 dataType;
    u32 dataBodySize;
  }IR_DATA_HEADER;

public:
  //------------------------------------------------------------------
  /**
  * @brief   コンストラクタ
  */
  //------------------------------------------------------------------
  DeliveryIRSystem( gfl2::heap::HeapBase* pHeap , gflnet2::InitParam* pInitParam , gflnet2::InitParameter* pIrInitParam , gflnet2::InitParameterFixed* pIrInitParamFixed );

  //------------------------------------------------------------------
  /**
  * @brief   デストラクタ
  */
  //------------------------------------------------------------------
  virtual ~DeliveryIRSystem();

  //------------------------------------------------------------------
  /**
  * @brief  DeliveryIRSystemの初期化
  *
  * @return 実行結果
  */
  //------------------------------------------------------------------
  bool Initialize( const int threadPriority , const u16 deliveryID = 0 , const void* pSendData = 0 , const size_t dataSize = 0 );

  //------------------------------------------------------------------
  /**
  * @brief  DeliveryIRSystemの終了処理
  *
  * @return 実行結果
  */
  //------------------------------------------------------------------
  void Finalize( void );

  //------------------------------------------------------------------
  /**
  * @brief  更新処理。
  *
  * @note   メインループ毎にポーリングすること
  */
  //------------------------------------------------------------------
  virtual void Update( DeliveryRecvListener* pRecvListener );

public:

  //------------------------------------------------------------------
  /**
   * @brief  終了準備
   */
  //------------------------------------------------------------------
  virtual void PrepareFinalize( void );

  //------------------------------------------------------------------
  /**
   * @brief  停止確認
   */
  //------------------------------------------------------------------
  virtual bool IsCompretePrepareFinalize( void );

private:

  //------------------------------------------------------------------
  /**
   * @brief   送信更新処理
   */
  //------------------------------------------------------------------
  void UpdateSendInternal( DeliveryRecvListener* pRecvListener );

  //------------------------------------------------------------------
  /**
   * @brief   受信更新処理
   */
  //------------------------------------------------------------------
  void UpdateRecvInternal( DeliveryRecvListener* pRecvListener );

  //------------------------------------------------------------------
  /**
   * @brief   受信完了しているかどうかチェック
   */
  //------------------------------------------------------------------
  bool IsCompleteRecvDataInternal( u32 index , DeliveryRecvListener* pRecvListener );

private:

  gflnet2::InitParameter* m_pIrInitParam;
  gflnet2::InitParameterFixed* m_pIrInitParamFixed;
  gflnet2::p2p::NetGame* m_pNetGame;

  const void* m_pSendData;
  size_t m_dataSize;
  u16 m_deliveryID;

  IR_STATE m_step;

  void* m_recvDataBuffer[MAX_RECV_INDEX];//[DELIVERY_DATA_SIZE];
  u32 m_recvDataSize[MAX_RECV_INDEX];

  size_t m_sendedSize;
  s32 m_completeIndex;

  u32 m_peerType;

  NetLib::P2P::P2pConnectionManager* m_pP2pConnectionManager;
};

GFL_NAMESPACE_END(Delivery)
GFL_NAMESPACE_END(NetLib)

#endif // __GFLNET_DELIVERY_IR_SYSTEM_H__

#endif
