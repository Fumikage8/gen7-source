#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_WIRLESSCYCLETHREAD_H__
#define __GFLNET_WIRLESSCYCLETHREAD_H__


//=============================================================================================
/**
 * @file	 gfl_NetWirelessCycleThread.h
 * @brief	 ワイヤレス通信 親子切り替えサイクルスレッド
 * @date	 2011.1.6
 */
//=============================================================================================

#include <gflnet2/include/base/gflnet2_NetThread.h>
#include <gflnet2/include/base/gflnet2_Base.h>

#include <gflnet2/include/base/gflnet2_AccessPointHistoryBase.h>
#include <gflnet2/include/base/gflnet2_AccessPointScannerBase.h>


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)


class NetWirelessCycleThread : public gflnet2::base::NetThread {
  GFL_FORBID_COPY_AND_ASSIGN( NetWirelessCycleThread ); // コピーコンストラクタ, 代入演算子を禁止
private:

  enum
  {
    BEACON_DATA_NUM_MAX = 16,//ビーコンバッファ数
    BEACON_BUF_SIZE = 200,//ビーコンバッファ１つのサイズ
    UDSRECEIVE_BUFFER = (4096*20),
  };

  //取得したビーコンデータ
  struct RECV_BEACON_DATA
  {
    u8 subId;
    u8 nowEntry;
    u8 ConnectAsClient;
    size_t size;
    bit64 lfc;
    u8 pRecvBeacon[BEACON_BUF_SIZE];
  };


private:

  gflnet2::base::BeaconManager* m_pBeaconManager;
  u8 m_limitNum;
  u32 m_productID;
  gfl2::heap::HeapBase* m_pHeap;


  u8* mpBeaconScanBuffer;  //スキャン時に一度に探す数
  u8 *mpTempBuff;  //一時的に読み込むバッファ
  u8* mpSendBeaconData;//送信ビーコンバッファ

  nn::uds::NodeInformation mNodeInfo[nn::uds::NODE_MAX];  //

  RECV_BEACON_DATA m_beaconData[BEACON_DATA_NUM_MAX];
  u32 m_beaconDataNum;


  static const u32 BEACON_TRANS_BASE_TIME = 385;						//ビーコン送受信基準時間（1回の受信と同じ385ms）
  static const int BEACON_SEND_MIN = 1;
  static const int BEACON_SEND_MAX = 4;

  static const int BEACON_RECV_MIN = 1;
  static const int BEACON_RECV_MAX = 4;

  static const int BEACON_SCAN_BUFF = 1024*16; //スキャン時に一度に探す数 一個1K

  gflnet2::base::AccessPointScannerBase* mpNetApScan;
  gflnet2::base::AccessPointHistoryBase* mpAccessPointHistory;

  nn::os::CriticalSection    m_criticalSection;               //!< 排他制御用オブジェクト
  nn::os::Event m_connectEvent;  //イベント  
  void* m_pUDSReceiveBuffer;  //受信バッファは4096アライメントのデバイスバッファ
  bool m_isFirstExec;
public:


  //------------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //------------------------------------------------------------------------------------
  NetWirelessCycleThread( gflnet2::base::BeaconManager* pBeaconManager , u8 limitNum , u32 productID , gfl2::heap::HeapBase* pHeap );

  //------------------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //------------------------------------------------------------------------------------
  virtual ~NetWirelessCycleThread();



  //------------------------------------------------------------------
  /**
   * @brief   前更新処理（この関数はメインスレッドから呼ばれる）
   */
  //------------------------------------------------------------------
  virtual void UpdateBefore(void);

  //------------------------------------------------------------------
  /**
   * @brief   後更新処理（この関数はメインスレッドから呼ばれる）
   */
  //------------------------------------------------------------------
  virtual void UpdateAfter(void);

private:

  //------------------------------------------------------------------
  /**
   * @brief   受信ビーコンをビーコンマネージャに移動（この関数はメインスレッドから呼ばれる）
   */
  //------------------------------------------------------------------
  void MoveRecvBeaconDataInternal( void );
  //------------------------------------------------------------------
  /**
   * @brief   送信ビーコンをスレッドに移動（この関数はメインスレッドから呼ばれる）
   */
  //------------------------------------------------------------------
  void MoveSendBeaconDataInternal( void );

  //----------------------------------------------------------------------------
  /**
   * @brief 受信したビーコンをスレッド内のバッファに保存
   */
  //----------------------------------------------------------------------------
  void SetRecvBeaconDataInternal(u8 sudId,const void* pRecvBeacon, size_t size, u8 nowEntry, u8 ConnectAsClient, bit64 lfc);

  //----------------------------------------------------------------------------
  /**
   * @brief AC接続してアクセスポイントの検索を行う
   */
  //----------------------------------------------------------------------------
  nn::Result ScanAccessPointInternal( u8* pBuffer );

  //----------------------------------------------------------------------------
  /**
   * @brief メイン処理
   * ※スレッド関数のループ内で呼ばれ続ける関数です。
   */
  //----------------------------------------------------------------------------
  virtual void Function( void );

  //----------------------------------------------------------------------------
  /**
   * @brief  ビーコン格納バッファの指定
   * @param  pData   データ
   * @param  num     数
   * @param  size    サイズ
   */
  //----------------------------------------------------------------------------
  void SetBeaconDataBuff(u8** pData, int num, int size);
  //------------------------------------------------------------------
  /**
   * @brief	  ポーズ処理
   */
  //------------------------------------------------------------------
  void PauseCheckFunc( void );


public:
  inline void SetNetApScan(gflnet2::base::AccessPointScannerBase* pAp){ mpNetApScan = pAp; };
  inline void SetAccessPointHistory(gflnet2::base::AccessPointHistoryBase* pAp){ mpAccessPointHistory = pAp; };


private:
  nn::Result CreateNetwork( int id );

};

GFL_NAMESPACE_END(gflnet2)


#endif// __GFLNET_WIRLESSCYCLETHREAD_H__
#endif
