#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_PIAADAPTER_H__
#define __GFLNET_PIAADAPTER_H__
//=============================================================================
/**
 * @file	 gflnet_PiaAdapter.h
 * @brief	 通信管理クラス PiaAdapter用
           マネージャーは通信ひとつにつき一個のつもりで設計してある
 * @author k.ohno
 * @date	 2011.1.6
 */
//=============================================================================

#include <nex.h>
#include <nex_P2p.h>

#include <pia/local.h>
#include <pia/local/local_UdsNetwork.h>

#include <gflnet2/include/beacon/gflnet2_Beacon.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(dev)


/* スレッド優先度 */
const s32 SEND_THREAD_PRIORITY               = nn::os::DEFAULT_THREAD_PRIORITY -1;
const s32 RECEIVE_THREAD_PRIORITY            = SEND_THREAD_PRIORITY -1;             // 通信では一般的に送信処理よりも受信処理を優先します。
const s32 UDS_NODE_EVENTTHREAD_PRIORITY      = RECEIVE_THREAD_PRIORITY -1;          // イベントスレッドは UdsNode を扱うスレッドの中で最も高く設定する必要があります。

enum ERROR_CHECK_RESULT
{
  RESULT_NONE = 0,
  RESULT_SESSION_LEFT,
  RESULT_ERROR,
};

///< これはPiaをラップするクラスです。シングルトンとして用意されています。
class PiaAdapter{
  GFL_FORBID_COPY_AND_ASSIGN(PiaAdapter);

private:
  static const u32 GATHERING_HOLDER_ARRAY_SIZE = nn::pia::local::UdsNodeBase::GATHERING_HOLDER_ARRAY_SIZE; //<! ネットワーク探索をした際に、１度に保持できる探索結果の最大数。
  nn::pia::local::UdsGatheringHolder mUdsGatheringHolderArray[GATHERING_HOLDER_ARRAY_SIZE];

  void* mpUdsReceiveBuffer;
  void* mpUdsScanBuffer;
  void* mpPiaMem;
  // シングルトンのインスタンスのポインタ。
  static PiaAdapter *s_pInstance;
  
public:

  u32 mLocalCommunicationID; ///ローカルのID
  nn::pia::local::UdsNetwork* s_pUdsNetwork;
 // u32 mProductID;

  // 接続状態更新イベントコールバック
  static void UdsUpdateEventCallbackFunc(nn::pia::local::UdsUpdateEvent updateEvent, u16 nodeId, void* pParam );

  static const char* ToString(enum nn::uds::State e);
  static const char* ToString(enum nn::uds::DisconnectReason e);
  static const char* ToString(enum nn::uds::ConnectType e);
  static const char* ToString(enum nn::uds::PowerSaveMode e);
  static const char* ToString(enum nn::uds::LinkLevel e);

  // インスタンスが作成されていなければ、NULLが返ります。
  static PiaAdapter* GetInstance(void) { return s_pInstance; }

  // インスタンスの作成
  static void CreateInstance(nn::nex::qByte sendOption,gfl2::heap::HeapBase* pHeapBase );

  // インスタンスの破棄
  static void DestroyInstance(void);

  // ローカル通信IDの生成
  u32 CreateLocalCommunicationId(u32 uniqueId, bool isTrial = false);

  // 現在のネットワーク人数を取得
  u8 GetNowEntry(void){ return nn::pia::local::UdsNode::GetInstance()->GetNetworkNowEntry(); };

  // ローカル通信IDの取得
  inline u32 GetLocalCommunicationIdPIA(void){ return mLocalCommunicationID; };

  // UDSネットワークの構築
  bool RegisterGathering( nn::nex::qByte maxEntry,
                         const char passphrase[],
                         nn::nex::qByte passphraseLendth,
                         nn::nex::qByte channel,
                         u8 gatheringID = BEACONTYPE_MATCH ,
                         char* pData = NULL,
                         u32 dataSize = 0
                         );

  // 周辺をFindById()で探索し、ギャザリングリストを更新。関数の実行に失敗した場合はfalse。
  bool UpdateGatheringList( u8 gatheringID = BEACONTYPE_MATCH );


  // 事前にUpdateGatheringList()を呼んでおくことが必要。
  // ネットワークが見つからなかった場合は0
  size_t GetGatheringList(void) const;
  nn::pia::local::UdsGatheringHolder* GetGatheringClass(int index);

  //電波強度取得
  nn::pia::local::UdsLinkLevel GetRadioStrength( int index );

  // UDSネットワークに参加
  bool Participate(nn::pia::local::UdsGatheringHolder *pGathering, const char passphrase[], NAMESPACE_NEX qByte passphraseLendth);

  // UDSネットワークを破棄、または離脱します（ノードがMasterかClientかによって動作が異なります）。
  void Terminate(void);

  // ネットワークのビーコンに文字列データを設定する。UDS Masterのみ呼び出し可能。
  // UDS Clientが呼び出したり、ビーコンに設定可能なデータ量を超過していたら、false。
  bool SetDataToBeacon(const char *pData, int size);

  // UDSに関係する各種情報をプリント。
  // 最新情報のプリントを試みるために、オブジェクトの状態を変更することもあり得ます。
  void PrintLatestInfo(nn::nex::qUnsignedInt32 titleId);

  // 自分のUDS状態を得ます。
  nn::pia::local::UdsState GetState(void) const;

  // 自分はUDSマスターか？
  bool IsMaster(void) const;

  // 自分はUDSクライアントか？
  bool IsClient(void) const;

  // マスターステーションのURLを得ます。
  nn::nex::StationURL* GetMasterUrl(void) const;

  // UDSの接続状態を得ます。
  bool GetConnectionStatus(nn::pia::local::UdsStatus *pStat) const;

  // 未実装
  NODE_ID_TYPE GetLocalNodeId() const;

  void RegisterStationEventCallback(nn::nex::StationEventCallback callback);

  //ディスパッチ
//  bool DispatchJob(qUnsignedInt32 timeout);

  //エラー検査
  ERROR_CHECK_RESULT ErrorCheck(void);

  bool DisconnectCheck(void);

  // デストラクタ。
  ~PiaAdapter(void);


private:
  // シングルトンパターンなので、コンストラクタはprivateにします。
  PiaAdapter(void);

  nn::Result Initialize(nn::nex::qByte sendOption,gfl2::heap::HeapBase* pHeapBase );

  
};

GFL_NAMESPACE_END(dev)
GFL_NAMESPACE_END(gflnet2)

#endif//__GFLNET_PIAADAPTER_H__
#endif
