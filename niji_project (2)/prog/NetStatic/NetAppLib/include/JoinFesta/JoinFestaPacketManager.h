#if defined(GF_PLATFORM_CTR)
#ifndef __JOIN_FESTA_PACKET_MANAGER_H__
#define __JOIN_FESTA_PACKET_MANAGER_H__
#pragma once

//============================================================================================
/**
 * @file    JoinFestaPacketManager.h
 *
 * @author ichiraku_katsuhiko
 */
//============================================================================================
// ライブラリ
#include <nex.h>
#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include <gflnet2/include/friends/gflnet2_FriendManager.h>
#include <gflnet2/include/beacon/gflnet2_Beacon.h>
#include <base/include/gfl2_Singleton.h>

// niji
#include "NetStatic/NetLib/include/Beacon/LocalBeaconListener.h"
#include <NetStatic/NetLib/include/Wifi/SubscriptionPacketListener.h>
//#include <system/event/SystemEventListener.h>

// JoinFesta
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketDefine.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalListener.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttractionListener.h"

// 前方宣言
namespace NetLib{
  namespace Wifi{
  class SubscriptionManager;
  }
}

namespace NetAppLib {



namespace JoinFesta {

// 前方宣言
class JoinFestaPacketEventList;
class JoinFestaPersonalData;
class JoinFestaPersonalDataManager;
class JoinFestaInformationMessage;

//------------------------------------------------------------------
/**
*  @brief 強制切断リスナー
**/
//------------------------------------------------------------------
class ForceDisconnectListener
{
public:
  virtual void OnForceDisconnect( void ){};
};

//------------------------------------------------------------------
/**
 * @brief JoinFesta通信パケット管理クラス。
 *        インターネット通信・ローカル通信時のパケットデータの送信および受信は<p>
 *        本クラスを経由して行われる。
 */
//------------------------------------------------------------------
class JoinFestaPacketManager  : public gfl2::base::Singleton<JoinFestaPacketManager>
                              , public NetLib::Beacon::LocalBeaconListener
                              , public NetLib::Wifi::SubscriptionPacketListener
                              , public NetAppLib::JoinFesta::JoinFestaPersonalListener
                           //public xy_system::event::SystemEventListener,
{
public:
  //------------------------------------------------------------------
  /**
  * @brief    コンストラクタ
  *
  * @param    pGFEachOther  ビーコンクラス
  */
  //------------------------------------------------------------------
  JoinFestaPacketManager( void );

  //------------------------------------------------------------------
  /**
  * @brief    デストラクタ
  */
  //------------------------------------------------------------------
  virtual ~JoinFestaPacketManager(void);

public:

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   JoinFestaパーソナルデータリスナーの登録
   *
   * @param[in]  listener 登録するリスナー
   */
  /* ----------------------------------------------------------------------------------------- */
  void RegistPersonalListener( JoinFestaPersonalListener* listener );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   JoinFestaパーソナルデータリスナーの解除
   *
   * @param[in]  listener 解除するリスナー
   */
  /* ----------------------------------------------------------------------------------------- */
  void RemovePersonalListener( JoinFestaPersonalListener* listener );

public:
  //------------------------------------------------------------------
  /**
  * @brief   初期化処理
  */
  //------------------------------------------------------------------
  void Initialize( void );

  //------------------------------------------------------------------
  /**
  * @brief   終了処理
  */
  //------------------------------------------------------------------
  void Finalize( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新処理
   *
   * @caution ユーザー呼び出しは禁止
   */
  //------------------------------------------------------------------
  void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief アトラクションの更新
   */
  //------------------------------------------------------------------
  void UpdateAttraction();

  //------------------------------------------------------------------
  /**
   * @brief   NetAppLibクラスのセットアップ
   *
   * @caution コンストラクタ後に1度だけコールすること
   *
   * @param[in,out] infoMessage      JoinFestaInformationMessageのインスタンス
   * @param[in,out] personalMgr      JoinFestaPersonalDataManagerのインスタンス
   * @param[in,out] eventList        JoinFestaPacketEventListのインスタンス
   * @param[in,out] subscriptionMgr  SubscriptionManagerのインスタンス
   */
  //------------------------------------------------------------------
  void SetupNetApp( NetAppLib::JoinFesta::JoinFestaInformationMessage* infoMessage,
                    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* personalMgr,
                    NetAppLib::JoinFesta::JoinFestaPacketEventList*     eventList,
                    NetLib::Wifi::SubscriptionManager* subscriptionMgr );


  JoinFestaInformationMessage* GetJoinFestaInformationMessage();

  //------------------------------------------------------------------
  /**
  * @brief   Finalizeの完了チェック
  *
  * @retval  true   Finalize処理完了
  * @retval  false  Finalize処理中
  */
  //------------------------------------------------------------------
  bool IsFinalized(void);

  //------------------------------------------------------------------
  /**
  * @brief 自身のパーソナルデータを送信
  *
  * @note  Wireless時はGFEachOther、WIFI時はSubscriptionにて送信する
  *
  */
  //------------------------------------------------------------------
  void SendMyData( bool isSkipUpdateSubscription = false );

  //------------------------------------------------------------------
  /**
   * @brief 自身のパーソナルデータをMessageClient経由で送信
   *
   * @note  インターネット通信時にのみコール可能
   *
   * @param[in] targetPID 送信相手のプリンシパルID
   */
  //------------------------------------------------------------------
  void SendMyMessage( const nn::nex::PrincipalID targetPID );

  //------------------------------------------------------------------
  /**
   * @brief 自身のパケットコマンドを[PacketGameStatus]にセットする
   */
  //------------------------------------------------------------------
  void SetPacketGameStatusWithMe( void );

  //------------------------------------------------------------------
  /**
  * @brief Subscriptionに対して全データの更新リクエストを登録する
  *
  * @note  インターネット通信時にのみコール可能
  *
  * @note ゲームサーバログイン後、１回だけコールすること
  */
  //------------------------------------------------------------------
  void RequestAllUpdatingWithSubscription( void );

  //Wifi初期リクエスト完了
  bool IsInitRequestEnd();

  //------------------------------------------------------------------
  /**
   * @brief 状態変化を検知する[VIPユーザー]をSubscriptionに登録する
   *
   * @note  インターネット通信時にのみコール可能
   * @note  以前に登録されていたユーザーは登録から除外される。
   * @note  登録されたユーザーのみ、状態変化を検知できる(最大6人まで）。
   *
   * @param[in]  lstPid 更新対象とするユーザのプリンシバルIDリスト
   */
  //------------------------------------------------------------------
  void UpdateVipTarget( nn::nex::qVector<nn::nex::PrincipalID>& lstPid );

  //------------------------------------------------------------------
  /**
   * @brief  パーソナルデータ更新(JoinFestaPacketDataダイレクト指定用)
   *
   * @param[in]  packetData JoinFestaPacketData
   *             @arg       引き渡されたデータアドレス領域は永続保障されない。
   *             @arg       ポインタを保持せず、自前のバッファにコピーすること。
   *
   * @param[in]  isOnline オンライン状態で登録するならtrueを指定する
   *
   * @return 追加・更新されたパーソナルデータ
   **/
  //------------------------------------------------------------------
  NetAppLib::JoinFesta::JoinFestaPersonalData* UpdateWithJoinFestaPacketData( const void* packetData, bool isOnline=true );

  //------------------------------------------------------------------
  /**
  * @brief  ユーザー情報をオフライン状態にするよう各関連クラスへ通知する
  *
  * @note   インターネット/Wireless切り替え時や、スリープ復帰時のWIFI切断時にコールすること
  *
  * @param[in/out]  person 通知するパーソナル情報
  */
  //------------------------------------------------------------------
  void RequestOffline( NetAppLib::JoinFesta::JoinFestaPersonalData* person );

  //------------------------------------------------------------------
  /**
  * @brief  ユーザー情報の更新を各関連クラスへ通知する
  *
  * @note   セーブデータを最新データで更新したい時にコールする
  *
  * @param[in/out]  person 通知するパーソナル情報
  */
  //------------------------------------------------------------------
  void RequestUpdatePacket( NetAppLib::JoinFesta::JoinFestaPersonalData* person );

  //------------------------------------------------------------------
  /**
   * @brief セーブデータ更新処理をロックする
   *
   * @caution セーブ開始時に必ずコールすること
   */
  //------------------------------------------------------------------
  void LockSavedataUpdating( void );

  //------------------------------------------------------------------
  /**
   * @brief セーブデータ更新処理をアンロックする
   *
   * @caution セーブ終了時に必ずコールすること
   */
  //------------------------------------------------------------------
  void UnlockSavedataUpdating( void );

  //------------------------------------------------------------------
  /**
   * @brief セーブデータ更新ロック判定
   *
   * @return セーブデータの更新処理がロックされているならtrueを返却
   */
  //------------------------------------------------------------------
  bool IsSavedataUpdatingLock( void ) const;

public:

  //アトラクション関連
  //------------------------------------------------------------------
  /**
  * @brief  アトラクションに参加
  *
  * @param[in]  person 開催者（参加者）のパーソナルデータ
  */
  //------------------------------------------------------------------
  void JoinAttractionRequest( NetAppLib::JoinFesta::JoinFestaAttractionListener* pListener , const NetAppLib::JoinFesta::JoinFestaPersonalData& person );

  //------------------------------------------------------------------
  /**
  * @brief  アトラクションの開催
  *
  * @param[in]  attractionID 開催するアトラクションのID
  */
  //------------------------------------------------------------------
  void OpenAttractionRequest( NetAppLib::JoinFesta::JoinFestaAttractionListener* pListener , u32 attractionID , u32 attractionTimeLimit );

  //------------------------------------------------------------------
  /**
  * @brief  アトラクションの終了
  *
  * @attention  開催者でも参加者でも終了するときに呼ぶ
  */
  //------------------------------------------------------------------
  void ExitAttractionRequest( NetAppLib::JoinFesta::JoinFestaAttractionListener* pListener );

  //------------------------------------------------------------------
  /**
  * @brief  自分のアトラクションスコアを設定
  *
  * @param[in]  myScore 自分のアトラクションスコア
  */
  //------------------------------------------------------------------
  void SetAttractionScore( u16 myScore );

  //------------------------------------------------------------------
  /**
  * @brief アトラクション参加リクエストを投げる
  */
  //------------------------------------------------------------------
  void RequestJoinAttractionWifi( void );

  //------------------------------------------------------------------
  /**
  * @brief アトラクション開催リクエストを投げる
  */
  //------------------------------------------------------------------
  void RequestOpenAttractionWifi( void );



public:
  //------------------------------------------------------------------
  /**
  * @brief  ビーコン更新通知イベント(for Wireless)
  *
  * @param[in]  beacon 更新されたビーコン情報
  *               @arg 引き渡されたデータアドレス領域は永続保障されない。
  *               @arg ポインタを保持せず、自前のバッファにコピーすること。
  *
  * @param[in] localFriendCode ビーコン送信者のローカルフレンドコード
  *                    @arg    ０以外の値がセットされているならフレンドであることをさす。
  *
  * @param[in] transferdId     ビーコン送信者のTransferdId
  *
  * @caution     引き渡されたデータアドレス領域は永続保障されない。
  * @caution     ポインタを保持せず、自前のバッファにコピーすること。
  */
  //------------------------------------------------------------------
  virtual void OnUpdateBeacon( const void* beacon, u64 localFriendCode, u64 transferdId );

  //------------------------------------------------------------------
  /**
  * @brief  ビーコン削除通知イベント(for Wireless)
  *
  * @param[in]  beacon 削除されたビーコン情報
  *               @arg 引き渡されたデータアドレス領域は永続保障されない。
  *               @arg ポインタを保持せず、自前のバッファにコピーすること。
  *
  * @param[in] localFriendCode ビーコン送信者のローカルフレンドコード
  *                    @arg    ０以外の値がセットされているならフレンドであることをさす。
  *
  * @param[in] transferdId     ビーコン送信者のTransferdId
  *
  * @caution     引き渡されたデータアドレス領域は永続保障されない。
  * @caution     ポインタを保持せず、自前のバッファにコピーすること。
  */
  //------------------------------------------------------------------
  virtual void OnDeleteBeacon( const void* beacon, u64 localFriendCode, u64 transferdId );

  //------------------------------------------------------------------
  /**
  * @brief  パケット更新通知イベント(for Subscription)
  *
  * @param[in]  playerData         更新されたデータの実体
  * @param[in]  principalId        データが更新されたユーザのプリンシバルＩＤ
  * @param[in]  isOnline           オンライン状態
  * @param[in]  isUpdatingStranger 通りすがりの更新通知であればtrueを指定
  */
  //------------------------------------------------------------------
  virtual void OnUpdateSubscription( nn::nex::SubscriptionData& playerData,
                                     u32 principalId, bool isOnline, bool isUpdatingStranger );

  //------------------------------------------------------------------
  /**
  * @brief  パケット更新通知イベント(from MessageClient)
  *
  * @param[in]  packetData  更新されたパケットデータの実体
  * @param[in]  packetSize  受け取ったパケットサイズ
  */
  //------------------------------------------------------------------
  virtual void OnUpdateMessagePacket( void* packetData );

  //------------------------------------------------------------------
  /**
  * @brief  ログアウト通知イベント(for Subscription)
  *
  * @param[in]  principalId ログアウトしたユーザのプリンシバルＩＤ
  */
  //------------------------------------------------------------------
  virtual void OnLogoutSubscription( u32 principalId );

   //------------------------------------------------------------------
   /**
   * @brief ジョインフェスタパーソナルデータ削除イベント通知
   *
   * @param[in]  person  削除されたパーソナル情報
   */
   //------------------------------------------------------------------
   virtual void OnDeleteJoinFestaPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person );


public:
  //------------------------------------------------------------------
  /**
  * @brief ビーコン再開イベント
  */
  //------------------------------------------------------------------
  virtual void OnRestartBeacon( void );


  //------------------------------------------------------------------
  /**
  * @brief   ビーコンリスナイベント通知をロック
  *
  * @note    セーブ中のデータ書き換えを抑制するためにSaveLoadControllerからコールされている。
  */
  //------------------------------------------------------------------
  void LockListener( void );

  //------------------------------------------------------------------
  /**
  * @brief   ビーコンリスナイベント通知をアンロック
  *
  * @note    セーブ中のデータ書き換えを抑制するためにSaveLoadControllerからコールされている。
  */
  //------------------------------------------------------------------
  void UnlockListener( void );


  //------------------------------------------------------------------
  /**
   * @brief   ジョインフェスタへ入場
   */
  //------------------------------------------------------------------
  void EnterJoinFesta( void );

  //------------------------------------------------------------------
  /**
   * @brief   ジョインフェスタから退場
   */
  //------------------------------------------------------------------
  void LeaveJoinFesta( void );

  //------------------------------------------------------------------
  /**
   * @brief  受信拒否するパーソナルかどうか
   *
   * @param[in]  pPersonalInfo 個人情報
   *
   * @return 受信拒否するならばtrue
   */
  //------------------------------------------------------------------
  bool IsRejectPersonal( const NetAppLib::JoinFesta::JoinFestaPersonalData& person );

private:
  //------------------------------------------------------------------
  /**
   * @brief 拡張情報をローテーションで送信する
   */
  //------------------------------------------------------------------
  void UpdateGameStatus( void );

  //------------------------------------------------------------------
  /**
   * @brief SubscriptionClientの更新
   */
  //------------------------------------------------------------------
  void UpdateSubscription( void );

  //------------------------------------------------------------------
  /**
   * @brief  パケットイベントの登録
   *
   * @param[in]  personal 通信で受信したパーソナル情報
   */
  //------------------------------------------------------------------
  void RegistPacketEvent( JoinFestaPersonalData* personal );

  //------------------------------------------------------------------
  /**
   * @brief  インフォメーションメッセージの登録
   *
   * @param[in]  personal 通信で受信したパーソナル情報
   * @param[in]  isLogin  ログイン状態になったのであればtrueを指す
   */
  //------------------------------------------------------------------
  void RegistInformationMessage( JoinFestaPersonalData& personal, bool isLogin );

  //------------------------------------------------------------------
  /**
  * @brief  OnAddJoinFestaPersonイベントのコール
  *
  * @param[in]  person 通知対象パーソナル情報
  */
  //------------------------------------------------------------------
  void Call_AddJoinFestaPersonListener( const NetAppLib::JoinFesta::JoinFestaPersonalData& person );

  //------------------------------------------------------------------
  /**
  * @brief  OnUpdateJoinFestaPersonイベントのコール
  *
  * @param[in]  person   通知対象パーソナル情報
  * @param[in]  isUpdate パケット更新フラグ
  */
  //------------------------------------------------------------------
  void Call_UpdateJoinFestaPersonListener( const NetAppLib::JoinFesta::JoinFestaPersonalData& person, bool isUpdate );

  //------------------------------------------------------------------
  /**
  * @brief  OnDeleteJoinFestaPersonイベントのコール
  *
  * @param[in]  person 通知対象パーソナル情報
  */
  //------------------------------------------------------------------
  void Call_DeleteJoinFestaPersonListener( const NetAppLib::JoinFesta::JoinFestaPersonalData& person );

  //------------------------------------------------------------------
  /**
  * @brief  ビーコンバージョンが同一か判定
  *
  * @param[in]  version ビーコンバージョン
  *
  * @return 同一バージョンのビーコンならtrue、異なるビーコンならfalse返却
  */
  //------------------------------------------------------------------
  bool IsSameBeaconVersion( u32 version ) const;

  //------------------------------------------------------------------
  /**
   * @brief 自分自身のパケットデータかを判定
   *
   * @param[in]  packet 判定対象パケットデータ
   *
   * @return 自分自身のパケットデータであればtrueを返却
   */
  //------------------------------------------------------------------
  bool IsMyPacket( const NetAppLib::JoinFesta::JoinFestaPacketData& packet );

  //------------------------------------------------------------------
  /**
   * @brief  ビーコンコマンドがJoinFesta用のものか判定
   *
   * @param[in]  beaconCmd ビーコンコマンド
   *
   * @return JoinFesta用ビーコンならtrue、異なるビーコンならfalse返却
   */
  //------------------------------------------------------------------
  bool IsJoinFestaBeaconCommand( const gflnet2::base::BeaconCommand& beaconCmd ) const;

  //------------------------------------------------------------------
  /**
   * @brief  パケットを受信拒否するかチェック
   *
   * @param[in]  pPersonalInfo 個人情報
   * @param[in]  relation      関係
   * @param[in]  isFriend      本体フレンドか
   *
   * @return パケットを受信拒否するならばtrue
   */
  //------------------------------------------------------------------
  bool IsRejectPacket( const NetAppLib::JoinFesta::PacketPersonalInfo* pPersonalInfo,
                       const JoinFestaDefine::E_JOIN_FESTA_RELATION relation,
                       const bool isFriend );

  //------------------------------------------------------------------
  /**
   * @brief  アッパーパケットの受信拒否チェック
   *
   * @param[in]  packet 判定対象パケットデータ
   *
   * @return パケットを受信拒否するならばtrue
   */
  //------------------------------------------------------------------
  bool IsUpperRejectPacket( const NetAppLib::JoinFesta::JoinFestaPacketData& packet );

  //------------------------------------------------------------------
  /**
  * @brief  パケットデータによるパーソナルデータの更新
  *
  * @attention JoinFestaPersonalDataManager::JoinFestaPersonalDataListが更新されます。
  * @caution イテレータループ内でこの関数をコールしないでください。
  *
  * @param[in]  packet    受信したパケットデータ
  * @param[in]  isFriend  フレンド関係ならばtrue
  * @param[in]  isOnline  trueならばオンライン状態であることをさす
  * @param[in]  localFriendCode  ローカルフレンドコード（ビーコン受信時のみ格納される）
  *
  * @return 更新されたパーソナルデータ
  */
  //------------------------------------------------------------------
  NetAppLib::JoinFesta::JoinFestaPersonalData* UpdateJoinFestaPersonalDataWithPacket( const NetAppLib::JoinFesta::JoinFestaPacketData* packet, bool isFriend, bool isOnline, const u64 localFriendCode );

  //------------------------------------------------------------------
  /**
  * @brief  パーソナルデータの更新後のアクション
  *
  * @param[in]  personal       更新対象となるパーソナルデータ
  * @param[in]  isOnline       オンライン状態であるならtrueを指定
  * @param[in]  isNewAdd       新規追加されたパーソナルデータならtrueを指定
  * @param[in]  isUpdate       パケットデータによってパーソナルデータが更新されたならtrueを指定
  * @param[in]  isChangeOnline オフラインからオンライン状態に変化したならtrueを指定
  */
  //------------------------------------------------------------------
  void DoJoinFestaPersonalDataUpdated( NetAppLib::JoinFesta::JoinFestaPersonalData* personal,
                                bool isOnline,
                                bool isNewAdd, bool isUpdate, bool isChangeOnline );

  //------------------------------------------------------------------
  /**
  * @brief   JoinFesta用セーブデータ領域の更新
  *
  * @param[in]   personal 更新対象となるパーソナルデータ
  */
  //------------------------------------------------------------------
  void UpdateJoinFestaSaveData( NetAppLib::JoinFesta::JoinFestaPersonalData* personal );

  //------------------------------------------------------------------
  /**
  * @brief   インターネット接続モード判定
  *
  * @return  インターネット接続中であればtrueを返却
  */
  //------------------------------------------------------------------
  bool IsWifiMode( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief レコード情報の加算
   *
   * @note  Lock中は加算処理を行わない
   *
   * @param[in] recId 加算対象となるレコードID
   */
  //------------------------------------------------------------------
  void IncRecordSaveData( /*savedata::Record::RECORD_ID*/int recId );

  //------------------------------------------------------------------
  /**
   * @brief 本体フレンドキーから友達であるかを判定する
   *
   * @param[in] principalId     判定対象となるプリンシパルID
   * @param[in] localFriendCode 判定対象となるローカルフレンドコード
   *
   * @return 友達であるならtrueを返却する
   */
  //------------------------------------------------------------------
  bool IsFriendWithCTRFriendKey( u32 principalId, u64 localFriendCode );

  //------------------------------------------------------------------
  /**
   * @brief 本体フレンドキーから友達であるかを判定する（PIDオンリー版）
   *
   * @param[in] principalId     判定対象となるプリンシパルID
   *
   * @return 友達であるならtrueを返却する
   */
  //------------------------------------------------------------------
  bool IsFriendWithPrincipalId( u32 principalId );

  //------------------------------------------------------------------
  /**
   * @brief JoinFesta通信が許可されているか判定
   *
   * @return JoinFesta通信が許可されているならtrueを返却
   */
  //------------------------------------------------------------------
  bool IsEnableNetwork( void ) const;



public:
  //強制切断
  void SetFroceDisconnectListener( NetAppLib::JoinFesta::ForceDisconnectListener *pListener ){ m_pForceDisconnectListener = pListener; };

private:
  static const u32 BEACON_ROTATE_TIME   = 6000; //!< 拡張情報のローテション間隔（ms）
  static const u32 RETRY_INTERVAL_TIMER = 200;  //!< 通信リトライのインターバル
  static const u32 RETRY_COUNT          = 6;    //!< 通信リトライ回数
  static const u32 LISTNER_MAX          = 2;    //!< リスナー数

private:
  bool                                       m_isEnableWireless;       //!< ワイアレススイッチ有効フラグ
  //
  gflnet2::friends::PrincipalId              m_vipPID;        //!< VIPリスト更新判定用PID
  //
  //
  JoinFestaPersonalDataManager*              m_personalMgr;            //!< パーソナルマネージャ
  NetLib::Wifi::SubscriptionManager*         m_subscriptionMgr;        //!< Subscriptionマネージャ
  //
  JoinFestaPacketEventList*                  m_JoinFestaEventList;           //!< JoinFesta通信イベントリスト
  JoinFestaInformationMessage*               m_infoMessage;            //!< インフォメーションメッセージ
  //
  JoinFestaPersonalListener*                 m_pListener[ LISTNER_MAX ];//!< パーソナルデータ更新リスナー
  //
  bool                                       m_isInit;                 //!< 初期化済みフラグ
  bool                                       m_isSavedataUpdatingLock; //!< セーブデータ更新ロックフラグ
  bool                                       m_isLockListener;         //!< リスナロックフラグ

  u64                                        m_gameStatusRotateTime;   //! 拡張情報ローテションタイム

  NetAppLib::JoinFesta::ForceDisconnectListener* m_pForceDisconnectListener;

  bool                                       m_isOutOfJoinFesta;       //!< ジョインフェスタにいるかどうかフラグ
  PacketAttraction                           m_attractionInfo;
  NetAppLib::JoinFesta::JoinFestaAttractionListener* m_pAttractionListener;
  u32                                        m_attractionUpdateWait;
  u32                                        m_count;
  u16                                        m_totalAttractionCount;

  u64                                        m_lastAttractionUpdateMsec;

#if PM_DEBUG
public:
  // デバッグ用
  void DEBUG_SetPacketAttraction( const PacketAttraction& packet ){ m_attractionInfo = packet; }
#endif
};


}    // JoinFesta
}    // NetAppLib
#endif // __JOIN_FESTA_PACKET_MANAGER_H__
#endif // GF_PLATFORM_CTR
