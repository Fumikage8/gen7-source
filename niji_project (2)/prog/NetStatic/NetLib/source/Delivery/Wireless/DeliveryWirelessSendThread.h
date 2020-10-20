#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_DeliveryWirelessSendThread.h
 * @brief  ワイヤレス配信送信スレッド
 * @author Shin Kosaka
 * @date   2012.7.23
 */
//=============================================================================

#ifndef __GFLNET_WIRELESS_DELIVERY_SEND_THREAD_H__
#define __GFLNET_WIRELESS_DELIVERY_SEND_THREAD_H__

#include <nn.h>


#include <gflnet2/include/beacon/gflnet2_Beacon.h>
#include <gflnet2/include/base/gflnet2_InitParam.h>

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_Threadstatic.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Delivery)

class DeliveryWirelessSystem;

class DeliveryWirelessSendThread : public gfl2::thread::ThreadInterface
{
GFL_FORBID_COPY_AND_ASSIGN(DeliveryWirelessSendThread); //コピーコンストラクタ＋代入禁止

private:

  enum
  {
    UDSRECEIVE_BUFFER = (4096*20),  	///< SampleDemos/uds/simple/uds_Main.cppから持ってきたサイズ,
    BEACON_NO = 0,//アプリ内での識別番号。使わないので0
    BEACON_ROTATE_MILLISECOND = 550,//ビーコン切り替え時間(通常だと350〜400がスコアが良い。混雑時は750ぐらいが安定している。)
    SLEEP_TIME = 1000 / 60,				//スリープ時間(ms)
  };

private:
  gflnet2::InitParam* m_pInitParam;
  DeliveryWirelessSystem* m_pSystem;
  bit8 m_TempBuff[gflnet2::base::BEACON_CONNECT_MAXBYTE];  //一時的に読み込むバッファ
  void* m_pReceiveBuffer;
  nn::os::Event m_aConnectEvent;  //イベント
  s32 m_index;
  s64 m_time;
  gfl2::thread::Thread* m_pThread;

public:
  //--------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param heap           スレッドのスタック領域に使用するヒープ
   * @param stack_size     スレッドのスタック領域のサイズ
   * @param listener       ログイン完了通知用リスナ
   * @param event          ログイン同期用イベント
   * @param timeout        タイムアウト時間
   */
  //--------------------------------------------------------------------------------
  DeliveryWirelessSendThread( gfl2::heap::HeapBase* heap, u32 stack_size , DeliveryWirelessSystem* pSystem );

  //--------------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //--------------------------------------------------------------------------------
  virtual ~DeliveryWirelessSendThread();

  //--------------------------------------------------------------------------------
  /**
   * @brief 開始
   */
  //--------------------------------------------------------------------------------
  void Start();

  void Kill(void){ m_pThread->Kill();};
  bool IsAlive(void){ return m_pThread->IsAlive();};

private:

  bool InitializeInternal( void );
  void FinalizeInternal( void );

private:

  virtual void Function( void );

private:
  //------------------------------------------------------------------
  /**
  * @brief	  ポーズ処理
  */
  //------------------------------------------------------------------
  void _PauseCheckFunc( void );
};

GFL_NAMESPACE_END(Delivery)
GFL_NAMESPACE_END(NetLib)


#endif //__GFLNET_DELIVERY_SEND_THREAD_H__

#endif
