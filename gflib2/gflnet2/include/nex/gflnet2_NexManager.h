#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexManager.h
 *
 * @brief  nn::nexのラッパークラス。NEXライブラリを管理する
 *
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_NEX_MANAGER_H__
#define __GFLNET_NEX_MANAGER_H__
#pragma once



#include <nex.h>
#include <nex_P2p.h>
#include <nn/friends.h>
#include <RendezVous/Services/NATTraversal.h>
#include <RendezVous/Services/Jugem/DataStore/src/Client/DataStoreClient.h>
#include <OnlineCore/src/Platform/Core/SystemErrorResult.h>



#include <gflnet2/include/gflnet2_NetworkDefine.h>
#include <gflnet2/include/nex/gflnet2_NexGameServerListener.h>
#include <gflnet2/include/nex/gflnet2_NexErrorListener.h>
#include <gflnet2/include/nex/gflnet2_NexUtilityListener.h>
#include <gflnet2/include/nex/gflnet2_NexError.h>
#include <gflnet2/include/nex/gflnet2_NexTemplate.h>
#include <gflnet2/include/nex/gflnet2_NexDispatchThreadListener.h>
#include <gflnet2/include/nex/gflnet2_NexDataStoreClient.h>

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ThreadStatic.h>

#include <base/include/gfl2_Singleton.h>

namespace gflnet2{

/**
 * @namespace gflnet2::nex
 * @brief     NEXアクセス関連
 */
namespace nex{


class NexDispatchThread;

//------------------------------------------------------------------
/**
*  @brief ＮＥＸゲームサーバアクセス用マネージャ
*
*  @attention 事前にfs, ac, socketライブラリを初期化しておく必要がある。
*
**/
//------------------------------------------------------------------
class NexManager : public gfl2::base::Singleton<NexManager>
{
  GFL_FORBID_COPY_AND_ASSIGN(NexManager); //コピーコンストラクタ＋代入禁止
  //
  friend class NexDispatchThread;

public:
  static const nn::nex::TimeInterval DEFAULT_TIMEOUT    = 30000;   //!< デフォルトタイムアウト時間(30秒)
  static const u32                   D_DISPATCH_TIMEOUT = 30;      //!< ディスパッチの待ち時間(msec)
  static const nn::nex::TimeInterval DEFAULT_CALLCONTEXT_TIMEOUT = 60000;//!< ProtocolCallContextのデフォルトタイムアウト

  /** @brief スレッドモード */
  enum ThreadMode
  {
    ThreadModeUndefined             = nn::nex::Core::ThreadModeUndefined,             //!< 未定義
    ThreadModeUnsafeTransportBuffer = nn::nex::Core::ThreadModeUnsafeTransportBuffer, //!< 転送をNEXスレッド（スレッドアンセーフ）で実行。
    ThreadModeSafeTransportBuffer   = nn::nex::Core::ThreadModeSafeTransportBuffer,   //!< 転送をNEXスレッド（スレッドセーフ）で実行。
  };

  /** @brief トレースモード */
  enum TraceMode
  {
    E_TRACE_NEVER  = TRACE_NEVER,   //!< 非表示
    E_TRACE_ALWAYS = TRACE_ALWAYS   //!< 表示
  };

  /** @brief ログレベル */
  enum LogLevel
  {
    Debug   = nn::nex::EventLog::Debug,   //!< NEX内部のデバッグ情報
    Info    = nn::nex::EventLog::Info,    //!< NEX内部の処理の詳細情報
    Warning = nn::nex::EventLog::Warning, //!< 修正すべきだが無視しても動作可能なワーニング情報
    Error   = nn::nex::EventLog::Error,   //!< 修正すべきだが復帰可能なエラー
    Fatal   = nn::nex::EventLog::Fatal,   //!< フェータルエラー。復帰不可能なエラー
    Always  = nn::nex::EventLog::Always,  //!< 常に出力されるログ情報(デフォルト)
  };

  /** @brief 解放したメモリを塗りつぶす設定 */
  enum FREE_MEMORY_FILL_TYPE
  {
    FILL_TYPE_NONE = 0,
    FILL_TYPE_0x00,
    FILL_TYPE_0xFF,
  };

public:
  NexManager( void );
  ~NexManager();

public:
  //------------------------------------------------------------------
  /**
   * @brief  NEXライブラリの初期化
   *
   * @attention  事前にfs, ac, socketを初期化する必要があります！
   *
   * @param[in,out]  heap              内部用バッファ割り当てに使用されるヒープメモリ。LOWから確保される。
   * @param[in]      mainBufferSize    NEX用に確保されるメインメモリサイズ
   * @param[in]      reserveBufferSize メインメモリが不足した際に割り当てられる緊急時メモリサイズ
   * @param[in]      threadMode        動作スレッドモード
   * @param[in]      isUsingAutoStack  AutoStackManagerを使用するならtrueを指定
   * @param[in]      fillType          解放したメモリを塗りつぶす設定
   *
   * @return 初期化結果。
   * @return 成功時には「nn::ResultSuccess()」、メモリ確保失敗時には「nn::nex::ResultOutOfMemory()」が返却される
   */
  //------------------------------------------------------------------
  nn::Result Initialize( gfl2::heap::HeapBase* heap, u32 mainBufferSize, u32 reserveBufferSize, ThreadMode threadMode=ThreadModeUnsafeTransportBuffer, bool isUsingAutoStack=false,FREE_MEMORY_FILL_TYPE fillType = FILL_TYPE_NONE );

  //------------------------------------------------------------------
  /**
   * @brief  NEXライブラリの終了
   */
  //------------------------------------------------------------------
  void Finalize( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief  ゲームサーバへのログイン＆セッショントークンの取得(非同期関数）
   *
   * @attention  事前に[gflnet_FriendManager]にてフレンドサーバへのログインを完了してください
   * @attention  完了の通知を検出する為に、事前に「SetGameServerListener()」でイベントリスナを登録してください。
   *
   * @note  非同期通信終了時に「NexGameServerListener::OnGameServerLoginSuccess()」がコールされる。
   * @note  エラー時は「NexGameServerListener::OnGameServerLoginFailed()」がコールされる。
   *
   * @param[in]  gameId      ゲームＩＤ
   * @param[in]  pAccessKey  アクセスキー
   * @param[in]  pAuthToken  アクセストークン取得用ハッシュキー
   * @param[in]  uiTimeout   通信タイムアウト時間
   * @param[in]  principalId 本体のプリンシバルID
   *
   * @return ログイン開始処理の実行結果
   */
  //------------------------------------------------------------------
  nn::nex::qResult LoginAndRequestAuthenticationToken( qUnsignedInt32 gameId, const qChar* pAccessKey,
                                                       const char* pAuthToken, nn::nex::TimeInterval uiTimeout,
                                                       nn::nex::PrincipalID principalId = INVALID_PRINCIPALID , bool isCheckParentalLock = true );

  //------------------------------------------------------------------
  /**
   * @brief  ゲームサーバへのログイン処理(非同期関数）
   *
   * @attention  事前に[gflnet_FriendManager]にてフレンドサーバへのログインを完了してください
   * @attention  完了の通知を検出する為に、事前に「SetGameServerListener()」でイベントリスナを登録してください。
   *
   * @note  非同期通信終了時に「NexGameServerListener::OnGameServerLoginSuccess()」がコールされる。
   * @note  エラー時は「NexGameServerListener::OnGameServerLoginFailed()」がコールされる。
   *
   * @param[in]  gameId     ゲームＩＤ
   * @param[in]  pAccessKey アクセスキー
   * @param[in]  uiTimeout  通信タイムアウト時間
   *
   * @return ログイン開始処理の実行結果
   */
  //------------------------------------------------------------------
  nn::nex::qResult LoginGameServerAsync( qUnsignedInt32 gameId, const qChar* pAccessKey, nn::nex::TimeInterval uiTimeout=DEFAULT_TIMEOUT);

  //------------------------------------------------------------------
  /**
   * @brief  ゲームサーバからのログアウト処理（非同期関数）
   *
   * @attention 完了の通知を検出する為に、事前に「SetGameServerListener()」でイベントリスナを登録してください。
   *
   * @note  非同期通信終了時に「NexGameServerListener::OnGameServerLogoutSuccess()」がコールされる。
   * @note  エラー時は「NexGameServerListener::OnGameServerLogoutFailed()」がコールされる。
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  nn::nex::qResult LogoutGameServerAsync( void );

  //------------------------------------------------------------------
  /**
   * @brief  ゲームサーバからのログアウト処理（同期関数）
   *
   * @attention 完了の通知を検出する為に、事前に「SetGameServerListener()」でイベントリスナを登録してください。
   *
   * @note  同期通信終了時に「NexGameServerListener::OnGameServerLogoutSuccess()」がコールされる。
   * @note  エラー時は「NexGameServerListener::OnGameServerLogoutFailed()」がコールされる。
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  nn::nex::qResult LogoutGameServer( void );

  //------------------------------------------------------------------
  /**
   * @brief  ゲームサーバへの接続状況を取得
   *
   * @return ゲームサーバに接続しているならtrueを返却する
   */
  //------------------------------------------------------------------
  bool IsGameServerConnecting( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief ゲームサーバへ非同期通信を行い疎通テストを行う
   *
   * @param timeout タイムアウトに設定する時間（ミリ秒）
   *
   * @note  非同期通信終了時に「NexGameServerListener::OnTestConnectivityResult()」がコールされる。
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  bool TestConnectivity( u32 timeout );

public:
  //------------------------------------------------------------------
  /**
   * @brief  ゲームサーバからのイベント通知先リスナの登録
   *
   * @param[in,out] listener 通知先のリスナ。NULLを指定することで登録を解除できる。
   */
  //------------------------------------------------------------------
  void SetGameServerListener( NexGameServerListener* listener );

  //------------------------------------------------------------------
  /**
   * @brief NEXエラー通知先リスナの登録
   *
   * @param[in,out] listener 通知先のリスナ。NULLを指定することで登録を解除できる。
   */
  //------------------------------------------------------------------
  void SetNexErrorListener( NexErrorListener* listener );

  //------------------------------------------------------------------
  /**
   * @brief  Utilityイベント通知先リスナの登録
   *
   * @param[in,out] listener 通知先のリスナ。NULLを指定することで登録を解除できる。
   */
  //------------------------------------------------------------------
  void SetUtilityListener( NexUtilityListener* listener );

  //------------------------------------------------------------------
  /**
   * @brief NEXゲームサーバ用イベントの通知先リスナを登録
   *
   * @param[in,out] event 登録する通知イベントハンドラー
   *
   * @return 成功ならtrue、失敗ならfalseを返却
   */
  //------------------------------------------------------------------
  qBool RegisterNotificationEventHandler( nn::nex::NotificationEventHandler *event );

  //------------------------------------------------------------------
  /**
   * @brief NEXゲームサーバ用イベントの通知先リスナを解除
   *
   * @param[in,out] event 解除する通知イベントハンドラー
   *
   * @return 成功ならtrue、失敗ならfalseを返却
   */
  //------------------------------------------------------------------
  qBool UnregisterNotificationEventHandler( nn::nex::NotificationEventHandler *event );

  //------------------------------------------------------------------
  /**
   * @brief Dispatchスレッドポーリング処理用イベントの通知先リスナを登録
   *
   * @param[in,out] event 登録する通知イベントハンドラー
   *
   * @return 成功ならtrue、失敗ならfalseを返却
   */
  //------------------------------------------------------------------
  qBool RegisterDispatchThreadEventHandler( NexDispatchThreadListener *event );

  //------------------------------------------------------------------
  /**
   * @brief Dispatchスレッドポーリング処理用イベントの通知先リスナを解除
   *
   * @param[in,out] event 解除する通知イベントハンドラー
   *
   * @return 成功ならtrue、失敗ならfalseを返却
   */
  //------------------------------------------------------------------
  qBool UnregisterDispatchThreadEventHandler( NexDispatchThreadListener *event );

public:
  //------------------------------------------------------------------
  /**
   * @brief  確保した緊急メモリが使用されている状況か判定
   *
   * @return 緊急メモリが使用されている状況ならtrueを返却
   */
  //------------------------------------------------------------------
  qBool IsLowMemory( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief  ゲームサーバログイン時に発行された証明書を取得
   *
   * @return 証明書へのポインタ。
   * @return ログアウト時はNULLが返却される。
   */
  //------------------------------------------------------------------
  nn::nex::Credentials* GetCredentials( void );

  //------------------------------------------------------------------
  /**
   * @brief  ゲームサーバログイン情報を取得
   *
   * @return ゲームサーバログイン情報へのポインタ。
   * @return ログアウト時はNULLが返却される。
   */
  //------------------------------------------------------------------
  nn::nex::NgsFacade* GetNgsFacade( void );

  //------------------------------------------------------------------
  /**
   * @brief NEXのログ動作モードを設定する
   *
   * @param[in] logLevel  出力レベル
   *
   * @param[in] traceMode トレースモード
   */
  //------------------------------------------------------------------
  void SetLogMode( LogLevel logLevel=Fatal, TraceMode traceMode=E_TRACE_NEVER );

  //------------------------------------------------------------------
  /**
   * @brief ProtocolCallContextの生成
   *
   * @note  タイムアウトの設定も内部で行います（ProtocolCallContextへのタイムアウトの設定は一部関数（Login、Logout）を除いて必須）
   * @return ProtocolCallContext
   */
  //------------------------------------------------------------------
  static nn::nex::ProtocolCallContext* CreateProtocolCallContext( bool isEnableTimeout = true , u32 timeoutMSec = DEFAULT_CALLCONTEXT_TIMEOUT );

public:
  //------------------------------------------------------------------
  /**
   * @brief 通信中か判定
   *
   * @return 通信中ならtrueを返却する
   */
  //------------------------------------------------------------------
  bool IsConnecting( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief 通信キャンセルリクエストを発行する
   * @brief キャンセルが実施された場合、完了時にOnNexRatingClientCanceled()がコールされる。
   *
   * @note  通信状態にかかわらず、コールしても良い
   *
   * @return キャンセル処理が実施されたならtrueを返却する
   */
  //------------------------------------------------------------------
  bool CancelConnecting( void );

public:

  //ディスパッチのロック
  //ロックされている間はNexのディスパッチが動かない
  bool GetIsLockDispatch( void ) const { return m_isLockDispatch; };
  void SetIsLockDispatch( bool isLock ) { m_isLockDispatch = isLock; };

  //------------------------------------------------------------------
  /**
   * @brief  ＮＥＸエンジンの処理を実行する。
   *
   * @attention  内部スレッドモード以外のスレッドモードを指定した場合のみ実行すること。
   * @attention  ポーリング処理で毎回コールしてください。
   *
   * @param[in]  time          タイムアウト時間(msec)。指定した時間内に処理が終わらない場合は次回コール時まで処理を中断する
   * @param[in]  isDispatchAll 全てのディスパッチコマンドを昇華させるならtrueを指定する
   */
  //------------------------------------------------------------------
  void Dispatch( u32 time=D_DISPATCH_TIMEOUT, bool isDispatchAll=false );

  //------------------------------------------------------------------
  /**
   * @brief  非同期ディスパッチ処理を開始する
   *
   * @attention 終了時に必ずEndDispatchAsync()をコールしてください。
   * @attention Dispatch()とは必ず排他させてください
   *
   * @param[in]  heap              スレッド用に使用するヒープメモリ
   *
   * @return スレッドの開始に成功したならtrue、失敗したならfalseを返却
   */
  //------------------------------------------------------------------
  bool StartDispatchAsync( gfl2::heap::HeapBase* heap );

  //------------------------------------------------------------------
  /**
   * @brief 非同期ディスパッチ処理用スレッドを破棄する
   */
  //------------------------------------------------------------------
  void EndDispatchAsync( void );

public:

  //------------------------------------------------------------------
  /**
   * @brief  独自サーバアクセス用認証トークンを取得する
   *
   * @attention   事前にRequestAuthenticationToken()をコールしてください
   *
   * @return 独自サーバアクセス用認証トークン
   */
  //------------------------------------------------------------------
  const char* GetAuthenticationToken( void ) const;

public:
  //------------------------------------------------------------------
  /**
   * @brief  NEXユニークIDを発行する(非同期関数）
   *
   * @attention   事前にBindUtilityClient()をコールしてください
   * @attention   完了の通知を検出する為に、事前に「SetGameServerListener()」でイベントリスナを登録してください。
   *
   * @param[out]  uniqueId NEXユニークIDの格納先。非同期で格納されるので、スタック指定はNG
   *
   * @return 実行に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool AcquireNexUniqueIdASync( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief NATトラバーサルをBind
   *
   * @attention ゲームサーバにログイン後に実行してください
   *
   * @return 実行に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool BindNATTraversalClient( void );

  //------------------------------------------------------------------
  /**
   * @brief NATトラバーサルをUnbind
   *
   * @attention ゲームサーバからログアウトする前に実行してください
   *
   * @return 実行に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool UnbindNATTraversalClient( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief データストアクライアントをBind
   *
   * @note ゲームサーバにログイン後に実行してください
   *
   * @return 実行に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool BindDataStoreClient( void );

  //------------------------------------------------------------------
  /**
   * @brief データストアクライアントのインスタンスを取得
   *
   * @note  事前に「BindDataStoreClient」をコールしてください
   *
   * @return データストアクライアントのインスタンス
   */
  //------------------------------------------------------------------
  NexDataStoreClient* GetDataStoreClient( void );

  //------------------------------------------------------------------
  /**
   * @brief データストアクライアントをUnbind
   *
   * @note ゲームサーバからログアウトする前に実行してください
   *
   * @return 実行に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool UnbindDataStoreClient( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief NEXユニークIDユーティリティをBind
   *
   * @note ゲームサーバにログイン後に実行してください
   *
   * @return 実行に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool BindUtilityClient( void );

  //------------------------------------------------------------------
  /**
   * @brief NEXユニークIDユーティリティをUnbind
   *
   * @note ゲームサーバからログアウトする前に実行してください
   *
   * @return 実行に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool UnbindUtilityClient( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief NAT セッションを開始し、P2P通信に必要な前処理を行います
   *
   * @attention NetZを作成する前に必ず本関数を実行してください
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  nn::nex::qResult StartNATSession( void );

  //------------------------------------------------------------------
  /**
   * @brief NAT セッションを終了します
   *
   * @attention NetZ::Terminate() 完了後、本関数を呼び出してください
   *
   * @return 実行に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool StopNATSession( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief NetZインスタンスの作成
   *
   * @return 作成に成功したならtrueが返却される
   */
  //------------------------------------------------------------------
  bool CreateNetZ( void );

  //------------------------------------------------------------------
  /**
   * @brief NetZインスタンスの削除
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  nn::nex::qResult DeleteNetZ( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief  使用されているメモリをコンソール出力する（デバッグ用）
   */
  //------------------------------------------------------------------
  void PrintMemoryUsage( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief 使用されているメモリをダンプ表示する（デバッグ用）
   */
  //------------------------------------------------------------------
  void PrintMemoryDump( void );

  //------------------------------------------------------------------
  /**
   * @brief  ＮＥＸログレベルをＤＥＢＵＧモードに設定（デバッグ用）
   */
  //------------------------------------------------------------------
  static void EnableDebugLog( void );


public:
  //------------------------------------------------------------------
  /**
   * @brief  NEXライブラリで内部使用されるメモリ確保処理
   *
   * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
   *
   * @param[in]  size  確保するメモリサイズ
   *
   * @return 確保されたヒープメモリアドレス
   */
  //------------------------------------------------------------------
  void* AllocFunction( size_t size );

  //------------------------------------------------------------------
  /**
   * @brief  NEXライブラリで内部使用されるメモリ解放処理。
   *
   * @attention[in,out] コールバック関数にのみ開示するため、ユーザー使用禁止！
   *
   * @param  pointer 解放対象となるポインタ
   */
  //------------------------------------------------------------------
  void  FreeFunction( void *pointer );

  //------------------------------------------------------------------
  /**
   * @brief  ゲームサーバ非同期ログイン検知用関数
   *
   * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
   *
   * @param[in,out]  pContext 完了通知対象となるコンテキスト
   */
  //------------------------------------------------------------------
  void  OnGameServerLoginCompletion(nn::nex::CallContext *pContext);

  //------------------------------------------------------------------
  /**
   * @brief  ゲームサーバ非同期ログアウト検知用関数
   *         コールバック関数にのみ開示するため、ユーザー使用禁止！
   *
   * @param[in,out]  pContext 完了通知対象となるコンテキスト
   */
  //------------------------------------------------------------------
  void  OnGameServerLogoutCompletion(nn::nex::CallContext *pContext);

  //------------------------------------------------------------------
  /**
   * @brief  NEXユニークID発行の非同期終了検知用関数
   *
   * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
   *
   * @param[in,out]  pContext 完了通知対象となるコンテキスト
   */
  //------------------------------------------------------------------
  void  OnAcquireNexUniqueIdCompletion(nn::nex::CallContext *pContext);

  //------------------------------------------------------------------
  /**
   * @brief  ゲームサーバ非同期疎通テスト検知用関数
   *
   * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
   *
   * @param[in,out]  pContext 完了通知対象となるコンテキスト
   */
  //------------------------------------------------------------------
  void  OnTestConnectivity(nn::nex::CallContext *pContext);

private:
  //------------------------------------------------------------------
  /**
   * @brief Dispatchスレッドポーリングイベント用リスナ関数の呼び出し
   */
  //------------------------------------------------------------------
  void CallDispatchThreadEventListener( void );

private:
  void  AllocateReservedMemory( void );
  void  ReleaseReservedMemory( void );
  void* InnerAlloc( size_t size );
  void  InnerFree( void* ptr );
  bool  LockConnection( void );
  void  UnlockConnection( void );

private:
  static const u32              THREAD_STACK_SIZE = 4096*6;  //!< スレッドスタック用サイズ
  static const u32              LISTENER_NUM = 4;            //!< リスナ最大登録可能数

private:
  bool                          m_isInit;             //!< 初期化フラグ
  bool                          m_gameServerLogin;    //!< ゲームサーバログインフラグ
  bool                          m_isDisconnectHandle; //!< ゲームサーバ切断通知フラグ
  bool                          m_isConnecting;       //!< 通信フラグ
  //
  void*                         m_mainBuffer;         //!< メインメモリ
  void*                         m_reservedBuffer;     //!< 緊急用メモリ
  u32                           m_mainBufferSize;     //!< メインメモリサイズ
  u32                           m_reserveBufferSize;  //!< 緊急用メモリサイズ
  u32                           m_currentUsage;       //!< 現在の消費メモリサイズ
  u32                           m_maxUsage;           //!< 最大消費メモリサイズ(確認用）
  nn::nex::UniqueId             m_uniqueId;           //!< MEXユニークID
  //
  nn::nex::qChar8               m_authToken[nn::nex::IndependentServer::AUTH_TOKEN_SIZE]; //!< 認証トークン
  //
  nn::nex::ProtocolCallContext* m_currentContext;     //!< カレントコンテキスト
  nn::nex::NATTraversalClient*  m_natClient;          //!< NATトラバーサル
  nn::nex::NetZ*                m_netZ;               //!< NetZインスタンス
  nn::nex::UtilityClient*       m_utilityClient;      //!< NEXユニークIDユーティリティ
  nn::nex::NgsFacade*           m_ngsFacade;          //!< ゲームサーバログイン情報
  nn::fnd::ThreadSafeExpHeap    m_nexHeap;            //!< NEX用ヒープ
  //
  NexDataStoreClient*           m_dataStoreClient;    //!< データストアクライアント
  NexDispatchThread*            m_dispatchThread;     //!< Dispatch用スレッド
  //
  nn::os::CriticalSection       m_criticalSection;    //!< クリティカルセクション
  //
  ThreadMode                    m_threadMode;         //!< カレントスレッドモード
  FREE_MEMORY_FILL_TYPE         m_fillType;           //!< 解放したメモリを塗りつぶす設定
  bool                          m_isLockDispatch;     //!< ディスパッチのロック
  //
  NexGameServerListener*        m_gameServerListener; //!< ゲームサーバイベント通知用リスナ
  NexErrorListener*             m_errorListener;      //!< エラー通知用リスナ
  NexUtilityListener*           m_utilityListener;    //!< Utilityイベント通知用リスナ
  NexDispatchThreadListener*    m_dispatchListener[LISTENER_NUM]; //!< Dispatchスレッドポーリングイベント用リスナ

  // @fix NMCat[4155]：NetZFatal発生時に、エラーアプレット送るまで相手をはめれるのでKickMeで離脱する
  bool m_isKickMe;  // sessionをキックしたフラグ



  //======================================================================================
  // FOR UPPER FIX
  //======================================================================================

public:
  //------------------------------------------------------------------
  /**
   * @brief  TerminateFacadeの非同期終了検知用関数
   *
   * @attention コールバック関数にのみ開示するため、ユーザー使用禁止！
   *
   * @param[in,out]  pContext 完了通知対象となるコンテキスト
   */
  //------------------------------------------------------------------
  void  OnTerminateNgsFacadeCompletion(nn::nex::CallContext *pContext);

  //------------------------------------------------------------------
  /**
   * @brief  ゲームサーバ通信の終了手続き(非同期処理）
   *
   * @attention 完了の通知を検出する為に、事前に「SetGameServerListener()」でイベントリスナを登録してください。
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  nn::nex::qResult TerminateFacade( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief 強制終了モード設定
   *
   * @note アクセスポイントとの接続状況を確認し、切断が確認されたなら強制終了モードに切り替える。
   *
   * @param isForceExec 強制終了を有効にするならtrueを指定する
   */
  //------------------------------------------------------------------
  void SetTerminateImmediately( bool isForceExec=false );



};



} // nex
} // gflnet


#endif  // __GFLNET_NEX_MANAGER_H__
#endif
