#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_DeliveryWirelessRecvThread.h
 * @brief  ワイヤレス配信受信スレッド
 * @author Shin Kosaka
 * @date   2012.7.23
 */
//=============================================================================

#ifndef __GFLNET_WIRELESS_DELIVERY_RECV_THREAD_H__
#define __GFLNET_WIRELESS_DELIVERY_RECV_THREAD_H__

#include <nn.h>

#include <gflnet2/include/base/gflnet2_InitParam.h>

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ThreadStatic.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Delivery)

class DeliveryWirelessSystem;

class DeliveryWirelessRecvThread : public gfl2::thread::ThreadInterface
{
GFL_FORBID_COPY_AND_ASSIGN(DeliveryWirelessRecvThread); //コピーコンストラクタ＋代入禁止
	
private:
	enum
	{
		BEACON_SCAN_BUFF = 1024 * 16,		//1Kバイト16件
		UDSRECEIVE_BUFFER = (4096*20),  	///< SampleDemos/uds/simple/uds_Main.cppから持ってきたサイズ,
		SLEEP_TIME = 1000 / 60,				//スリープ時間(ms)
	};
private:
	
	gflnet2::InitParam* m_pInitParam;
	DeliveryWirelessSystem* m_pSystem;
	bit8 m_TempBuff[nn::uds::NET_DESC_APPDATA_SIZE_MAX];  //一時的に読み込むバッファ
	u8* m_pBeaconScanBuffer;
	void* m_pReceiveBuffer;
	nn::os::Event m_aConnectEvent;  //イベント
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
	DeliveryWirelessRecvThread( gfl2::heap::HeapBase* heap, u32 stack_size , DeliveryWirelessSystem* pSystem );

	//--------------------------------------------------------------------------------
	/**
	* @brief デストラクタ
	*/
	//--------------------------------------------------------------------------------
	virtual ~DeliveryWirelessRecvThread();


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

private:

	virtual void Function( void );

private:
	
	void _PauseCheckFunc();
};

GFL_NAMESPACE_END(Delivery)
GFL_NAMESPACE_END(NetLib)


#endif //__GFLNET_WIRELESS_DELIVERY_RECV_THREAD_H__

#endif
