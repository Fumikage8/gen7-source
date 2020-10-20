#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_NETGAMEBASE_H__
#define __GFLNET_NETGAMEBASE_H__
//=============================================================================
/**
 * @file	 gflnet_NetGameBase.h
 * @brief	 送信受信を管理する基本クラス
 * @author k.ohno
 * @date	 2011.6.9
 */
//=============================================================================

#include <gflnet2/include/gflnet2_NetworkDefine.h>
#include <NetZ/src/Core/DOCore/DirectStream.h>
#include "gflnet2/include/p2p/gflnet2_RecvBuffer.h"
#include <gflnet2/source/p2p/proc/ir/gflnet2_P2pMainIr.h>
#include <gflnet2/include/p2p/gflnet2_NetGame.h>
#include "gflnet2/include/p2p/gflnet2_NetGameErrorListener.h"
#include "gflnet2/include/p2p/gflnet2_NetGameMigrationListener.h"

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(p2p)


#define  WAIT_TIME_CONNECT (30)
#define  SEND_TIMEOUT_CONNECT (60*60)


class NetGameBase 
{
public:
  enum
  {
    INVALID_STATION_ID = 0xFFFFFFFF,
  };

  static const int INVALID_NET_ID = -1;

private:


  static const int NETGAME_NUM_MAX = 10;  ///< NetGame作成可能上限
  static const int IR_TEMP_BUFF_MAX = 4096;
   

private:
  static gflnet2::p2p::RecvBuffer* mRecv[GFLNET_P2PCONNECT_MAXNUM]; // 受信バッファ管理構造体
  char mSend[GFLNET_P2PCONNECT_MAXPACKET+COMMAND_MINI_HEADER_SIZE];  // SENDバッファ
  char mTempMySend[GFLNET_P2PCONNECT_MAXPACKET+COMMAND_MINI_HEADER_SIZE];  // 自分の送信を保存       //※削除してよい

  // 相手から送られてきた番号の管理
  // 自分が送った番号であることを確認し 次の接続を行う
  static u8 mRecvInc[GFLNET_P2PCONNECT_MAXNUM];    //インクリメント数
  static u8 mRecvIncFlgBit;  //受け取りフラグBIT

  // 相手のコールバックでもらった番号
  // ダイレクトに通信で相手に返す
  static u8 mMigrationCallbackInc[GFLNET_P2PCONNECT_MAXNUM];    //マイグレーション復帰様インクリメント数
  static u8 mCallbackInc[GFLNET_P2PCONNECT_MAXNUM];    //インクリメント数
  static u8 mCallbackIncFlgBit;  //受け取りフラグBIT

  static nn::nex::ConnectionID mParentCommunicationID[GFLNET_P2PCONNECT_MAXNUM]; //コミュニケーションID 親の順番  子機には最初に送信される
  static nn::nex::ConnectionID mDisconnectCommunicationID[GFLNET_P2PCONNECT_MAXNUM]; //コミュニケーションID 切断リスト

  static nn::nex::ConnectionID mJoinMember[GFLNET_P2PCONNECT_MAXNUM];  //親機から送られてきたCommunicationID順番


  int mSendTypeTimeoutCount[GFLNET_P2PCONNECT_MAXNUM]; // 送ったのが成功したかどうか失敗が 一定以上になるとエラーにする

  static nn::nex::ConnectionID mMyConnectionID;
  static int mSendTimer;
  static int mMax;
  static u8* mIrTempBuff;

  static bool mbMaster;
  static bool mSetCallbackFlag; //受信コールバクを設定したかどうか
  static bool  mbConnectionSend;  //
  static bool  mMigrationStopFlg;

  static gflnet2::InitParam* mpInitParam;

  static proc::P2pMainIr* mpP2pIrLoopProc;

  nn::nex::qVector<nn::nex::DirectStream::Data> mRecvData;

  NetGame* mpNetGameBuffer[NETGAME_NUM_MAX];  //NetGameクラス
  u32 mSendNumber;  //送信番号 連番管理のため

  u16 mSendBufferIndex;  //mSendのどこまでデータが入っているか
  u8 mMySendInc;
  u8 mSendFlg;  //送信したら1 送信したことが帰ってきたら０
  u8 mSendFailedFlg;  //
  bool m_isEnableStartGame;//開始準備完了フラグ

  // エラー通知用リスナー
  static NetGameErrorListener* m_errorListener;
  // マイグレーション通知用リスナー
  static NetGameMigrationListener* m_migrationListener;

#if GFL_DEBUG
  static const int DEBUGCOMMNAD_BUFF = 50;
  static const int DEBUGCOMMNAD_NUM = 10;
  static int DebugBuffCount;
  static u8 mpDebugBuff[DEBUGCOMMNAD_NUM][DEBUGCOMMNAD_BUFF];
#endif


  
private:

  static int ConnectionID2RecvBufferIndex(nn::nex::ConnectionID id);
  static int SetConnectionIDRecvBuffer(nn::nex::ConnectionID id,int line=0);
  static void DirectSetConnectionIDRecvBuffer(nn::nex::ConnectionID id);
  static void SetConnectionIDRecvBufferChild(void);
  static int ResetConnectionIDRecvBuffer(nn::nex::ConnectionID id);
  void sendNumberCheck(void);
  bool SendDataStream(const qByte* pData,qUnsignedInt32 size);
  int ParentCommunicationIDNum(void);

  
  //---------------------------------------------------------------------------------
  /**
   * @brief  NetGameが何番目の登録か得る
   * @param  pNetGame NetGameクラスのポインタ
   * @return 何番目なのか
   */
  //---------------------------------------------------------------------------------
  int getNetGameNo(NetGame* pNetGame);

  //---------------------------------------------------------------------------------
  /**
   * @brief  コマンドの解析処理
   * @param  pRecvBuff  受信バッファ管理構造体
   * @return 解析したときのコマンド数
   */
  //---------------------------------------------------------------------------------
  int queueSet(int netid, RecvBuffer* pRecvBuff);
  //---------------------------------------------------------------------------------
  /**
   * @brief  通信コマンドの引き出し処理（データを引き出し消す）
   * @param  pRecvBuff  受信バッファ管理構造体
   * @param  pHead  受信バッファヘッダ構造体
   *@param  コマンド
   * @return 受け取れたら true
   */
  //---------------------------------------------------------------------------------
  bool queuePop(RecvBuffer* pRecvBuff, RecvHead* pHead,int command);


  //---------------------------------------------------------------------------------
  /**
   * @brief  タイミングコマンドの解析処理
   * @param  pRecvBuff  受け取ったデータの管理構造体
   */
  //---------------------------------------------------------------------------------
  void timingCheck(RecvBuffer* pRecvBuff, RecvHead* pHead);
  //---------------------------------------------------------------------------------
  /**
   * @brief  親機でのタイミングコマンドの解析処理
   * @param  pRecvBuff  受け取ったデータの管理構造体
   * @return bool      タイミングがそろったらtrue
   */
  //---------------------------------------------------------------------------------
  bool parentTimingCheck(void);
  //---------------------------------------------------------------------------------
  /**
   * @brief  親機でのタイミングコマンドの解析処理
   * @param  pRecvBuff  受け取ったデータの管理構造体
   * @return bool      タイミングがそろったらtrue
   */
  //---------------------------------------------------------------------------------
  bool endTimingCheck(void);


  bool dataSendMain(void);


  //---------------------------------------------------------------------------------
  /**
   * @brief  受信コールバック関数
   * @param  pRecvBuff  受け取ったデータの管理構造体
   * @return bool      タイミングがそろったらtrue
   */
  //---------------------------------------------------------------------------------
  static void onUserReceiveUpdated(nn::nex::ConnectionID sourceId, nn::nex::qByte* buffer, nn::nex::qUnsignedInt32 size);
  //---------------------------------------------------------------------------------
  /**
   * @brief  接続コールバック関数
   * @param  hStation
   * @param  oEvent
   */
  //---------------------------------------------------------------------------------
  static void GameStationEventCallback( nn::nex::DOHandle hStation, nn::nex::StationEvent::Type oEvent );
  static void GameStationEventCallbackWifi(int no);//wifi用


  static void GameOperationStartCallback(nn::nex::StationOperation * pOperation);
  static void GameOperationEndCallback(nn::nex::StationOperation *pOperation);


  
public:
  static int GetMaxNum(void){ return mMax; };  //NetGam


  static void GameStationEventCallbackIr(int bAdd);//赤外線用
  static nn::nex::ConnectionID RecvBufferIndex2ConnectionID(int index);
  //---------------------------------------------------------------------------------
  /**
   * @brief  コンストラクタ&デストラクタ
   */
  //---------------------------------------------------------------------------------
  NetGameBase(gflnet2::InitParam* pInit);
  virtual ~NetGameBase();

  //---------------------------------------------------------------------------------
  /**
   * @brief  NetGameの登録
   * @param  pNetGame NetGameクラスのポインタ
   */
  //---------------------------------------------------------------------------------
  void SetNetGame(NetGame* pNetGame);
  //---------------------------------------------------------------------------------
  /**
   * @brief  NetGameの削除
   * @param  pNetGame NetGameクラスのポインタ
   */
  //---------------------------------------------------------------------------------
  void RemoveNetGame(NetGame* pNetGame);
  //---------------------------------------------------------------------------------
  /**
   * @brief  接続前に呼ぶ関数  コールバック関数を設定している
   */
  //---------------------------------------------------------------------------------
  void ConnectSetting(void);
  void ConnectSettingWifi(void);
  //---------------------------------------------------------------------------------
  /**
   * @brief  切断に呼ぶ関数  コールバック関数をリセット
   */
  //---------------------------------------------------------------------------------
  void DisConnect(void);

  //---------------------------------------------------------------------------------
  /**
   * @brief  動かす処理 1/30 か 1/60でまわす必要がある
   */
  //---------------------------------------------------------------------------------
  void Func(void);
  //---------------------------------------------------------------------------------
  /**
   * @brief  データ送信 内部用
   * @param  command   ヘッダー
   * @param  sendData  送るデータ  UDS_PACKET_PAYLOAD_MAX_SIZE を超えると、関数の実行は失敗します。
   * @param  size      送信サイズ
   * @return bool      送る準備ができたらtrue だめならfalse
   */
  //---------------------------------------------------------------------------------
  bool SendData(NetGame* pNetGame, u8 sysCommand, u8 targetBit,u8 userCommand, char* sendData, int size);

  //---------------------------------------------------------------------------------
  /**
   * @brief  データ受信
   * @param  id        送ってきた人の通信ID
   * @param  RecvData  受け取るバッファ
   * @param  size      受信バッファサイズ これ以上は受け取りません
   * @return bool      データがあればtrue なければfalse
   */
  //---------------------------------------------------------------------------------
  bool RecvData(int id, char* pRecvData, int size);
  //---------------------------------------------------------------------------------
  /**
   * @brief  データコピー
   * @param  id        送ってきた人の通信ID
   * @param  RecvData  受け取るバッファ
   * @param  size      受信バッファサイズ これ以上は受け取りません
   */
  //---------------------------------------------------------------------------------
  void DataCopy(int id, int index, char* pRecvData, int size);

  //---------------------------------------------------------------------------------
  /**
   * @brief  受信アップデート
   */
  //---------------------------------------------------------------------------------
  void RecvUpdate(void);
  //---------------------------------------------------------------------------------
  /**
   * @brief  コネクションした人数を得る
   */
  //---------------------------------------------------------------------------------
  int GetConnectionCount(void);

  //------------------------------------------------------------------
  /**
   * @brief   マイグレーション後 送信再開の為にダミーを送信する
   */
  //------------------------------------------------------------------
  static bool SetDummySend(void);
  
  //---------------------------------------------------------------------------------
  /**
   * @brief  親機を記憶
   */
  //---------------------------------------------------------------------------------
  static inline void SetMaster(void){ mbMaster = true; };
  static inline void SetMaster(bool bMaster){ mbMaster = bMaster; };
  //---------------------------------------------------------------------------------
  /**
   * @brief  親機かどうかを得る
   */
  //---------------------------------------------------------------------------------
  static inline bool IsMaster(void){ return mbMaster; };
  //---------------------------------------------------------------------------------
  /**
   * @brief  親機からコネクションを送信 内部用
   * @return bool  送る準備ができたらtrue だめならfalse
   */
  //---------------------------------------------------------------------------------
  bool SendConnectionID(void);

  //---------------------------------------------------------------------------------
  /**
   * @brief  親機からの開始号令確認
   * @return bool  準備ができたらtrue 
   */
  //---------------------------------------------------------------------------------
  bool IsEnableStartGame(void);

  //---------------------------------------------------------------------------------
  /**
   * @brief  親機のindexから受信バッファindexを得る
   * @param  index
   * @return index
   */
  //---------------------------------------------------------------------------------
  int ParentIndex2RecvBuffer(int index);

  //---------------------------------------------------------------------------------
  /**
   * @brief  ビーコンのデータを送る（ライブラリにセットする）
   * @param  pData       データ
   * @param  size        サイズ
   * @return  成功したら true
   */
  //---------------------------------------------------------------------------------
  bool SetDataToBeacon(void* pData, int size);
  //---------------------------------------------------------------------------------
  /**
   * @brief  タイミングをそろえるコマンドの送信
   * @param  timingUniqueNo    タイミングをそろえるためのユニーク番号 ０以外
   * ※必ず送るので戻り値は無いですが 同じ番号を連続したりすると挙動がおかしくなります
   * 通信の遷移をしっかり使う側で把握してください
   */
  //---------------------------------------------------------------------------------
  void TimingStart(int timingUniqueNo);
  //---------------------------------------------------------------------------------
  /**
   * @brief  タイミングをそろえるコマンドが到着したかどうか
   * @param  timingUniqueNo    タイミングをそろえるためのユニーク番号 ０以外
   * @return bool              到着したタイミングから次のスタートを行うまでずっとtrue
   */
  //---------------------------------------------------------------------------------
  bool IsTimingEnd(int timingUniqueNo);
  int GetNetID(void);
  bool IsDisconnectNetID(int netID );

  void SetP2pIrLoopProc(proc::P2pMainIr* pProc){ mpP2pIrLoopProc = pProc; }; 

  //---------------------------------------------------------------------------------
  /**
   * @brief  netgameの順番を調べる
   * @param  NetGameクラスのポインタ
   * @return no
   */
  //---------------------------------------------------------------------------------
  int CheckNetGameNo(NetGame* pNetGame);

  static int SetNetID(nn::nex::ConnectionID id);

  //------------------------------------------------------------------
  /**
   * @brief   エラーリスナー登録
   * @param   listener   通知を受け取るNetGameErrorListener
   */
  //------------------------------------------------------------------
  static void RegistErrorListener( NetGameErrorListener* listener );

  //------------------------------------------------------------------
  /**
   * @brief   エラーリスナー解除
   */
  //------------------------------------------------------------------
  static void UnregistErrorListener( void );

  //------------------------------------------------------------------
  /**
   * @brief   マイグレーション用リスナー登録
   * @param   listener   通知を受け取るNetGameMigrationListener
   */
  //------------------------------------------------------------------
  static void RegistMigrationListener( NetGameMigrationListener* listener );

  //------------------------------------------------------------------
  /**
   * @brief   マイグレーション用リスナー解除
   */
  //------------------------------------------------------------------
  static void UnregistMigrationListener( void );

  //------------------------------------------------------------------
  /**
   * @brief   マイグレーション有効かどうか（リスナー設定されていたら有効）
   */
  //------------------------------------------------------------------
  static bool IsMigrationEnable( void );
};


GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)

#endif // __GFLNET_NETGAMEBASE_H__
#endif
