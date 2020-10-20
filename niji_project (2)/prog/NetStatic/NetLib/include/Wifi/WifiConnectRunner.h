#if defined(GF_PLATFORM_CTR)
//============================================================================================
/**
 * @file WifiConnectRunner.h
 *
 * @author suzuki_hisanao
 */
//============================================================================================
#ifndef __WIFI_CONNECT_RUNNNER_
#define __WIFI_CONNECT_RUNNNER_
#pragma once

#include <gflnet2/include/friends/gflnet2_FriendLoginListener.h>
#include <gflnet2/include/nex/gflnet2_NexGameServerListener.h>
#include <gflnet2/include/wifi/gflnet2_WifiConnectListener.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include <gflnet2/include/friends/gflnet2_FriendManager.h>
#include <gflnet2/include/wifi/gflnet2_WifiSystem.h>

#include <NetStatic/NetLib/include/Wifi/SubscriptionManager.h>

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Wifi)

/**
 * @namespace NetLib::wifi
 * @brief     WIFI接続・切断プロセス関連
 */

  //--------------------------------------------------------------------------------------------
  /**
  * @brief インターネットへの接続・切断を行う実行クラス。<p>
  *
  * @li    本クラスによって以下の処理が実行される。
  *       - APへの接続
  *       - HTTPモジュールの初期化
  *       - ソケットモジュールの初期化
  *       - フレンドサーバへのログイン
  *       - NEXサーバへのログイン、およびアクセストークンの発行
  *       - Subscriptionサーバへのバインド
  *       - Subscriptionサーバからのアンバインド
  *       - NEXサーバからのログアウト
  *       - フレンドサーバからのログアウト
  *
  * @attention  本クラスでは、ローカル通信の切断・再開処理は担当しない。<p>
  *             事前・事後にローカル通信の切断・再開を実行する必要がある。
  */
  //--------------------------------------------------------------------------------------------
  class WifiConnectRunner : public gflnet2::friends::FriendLoginListener,
                            public gflnet2::nex::NexGameServerListener,
                            public gflnet2::wifi::WifiConnectListener

  {
    public:
      /**
       * @brief 実行ステータス
       */
      typedef enum
      {
        E_RET_CONTINUE,     //!< 実行中
        E_RET_SUCCESS,      //!< 正常終了
        E_RET_FAILED,       //!< エラー終了
        E_RET_FAILED_PATCH, //!< ネット接続にはパッチが必要
      }E_RET_CODE;

      /**
       * @brief 実行モード
       */
      typedef enum
      {
        E_MODE_CONNECT,                   //!< 接続(非同期通信）
        E_MODE_CONNECT_AND_CREATE_NEXID,  //!< 接続(非同期通信）& NEXユニークIDを発行
        E_MODE_DISCONNECT,                //!< 切断(非同期通信）
        E_MODE_DISCONNECT_SYNC,           //!< 切断(同期通信）
      }E_EXEC_MODE;

      enum
      {
        PATCH_ERROR = 20120,//必須パッチエラー
      };

    public:
      //--------------------------------------------------------------------------------------------
      /**
       * @brief  コンストラクタ
       *
       * @param[in]  execMode 実行モード
       */
      //--------------------------------------------------------------------------------------------
      WifiConnectRunner( E_EXEC_MODE mode , bool isSkipParentalLock = false );

      //--------------------------------------------------------------------------------------------
      /**
       * @brief   デストラクタ
       */
      //--------------------------------------------------------------------------------------------
      virtual      ~WifiConnectRunner();

    public:
      //--------------------------------------------------------------------------------------------
      /**
       * @brief   終了処理。必ずコールすること
       */
      //--------------------------------------------------------------------------------------------
      void        Finalize( void );

      //--------------------------------------------------------------------------------------------
      /**
       * @brief   更新処理
       *
       * @return  実行ステータスコード
       */
      //--------------------------------------------------------------------------------------------
      E_RET_CODE   Update( void );

      //--------------------------------------------------------------------------------------------
      /**
       * @brief   発行されたNEXユニークIDを取得
       *
       * @note    返却値が0の場合は未取得状態を指す
       *
       * @return  発行されたNEXユニークID
       */
      //--------------------------------------------------------------------------------------------
      nn::nex::UniqueId GetNexUniqueId( void ) const;

      //--------------------------------------------------------------------------------------------
      /**
       * @brief   エラー時に切断の手続きが必要か判断する
       *
       * @note    本関数がtrueを返却した際は、
       * @note    E_MODE_DISCONNECTまたはE_MODE_DISCONNECT_SYNCで本クラスを再生成してください
       *
       * @return  切断の手続きが必要であればtrueを返却する
       */
      //--------------------------------------------------------------------------------------------
      bool IsNeedDisconnectProcess( void ) const;

      //--------------------------------------------------------------------------------------------
      /**
       * @brief アクセスステップを取得する（デバッグ用）
       *
       * @return アクセスステップ
       */
      //--------------------------------------------------------------------------------------------
      u32 GetActionStep( void ) const;

    public:
      // 以下関数はイベントリスナ関数の為、ユーザがコールしてはならない
      virtual void OnFriendServerLoginFinish( const nn::Result &result );
      //
      virtual void OnGameServerLoginSuccess( nn::nex::Credentials* credential );
      virtual void OnGameServerLoginFailed( const nn::nex::qResult& loginFuncResult, qInt32 lastLoginErrorCode );
      virtual void OnGameServerLogoutSuccess( void );
      virtual void OnGameServerLogoutFailed( const nn::nex::qResult& logoutFuncResult, qInt32 errorCode );
      virtual void OnAcquireNexUniqueIdSuccess( nn::nex::UniqueId uniqueId );
      virtual void OnAcquireNexUniqueIdFailed( const nn::nex::qResult& result, qInt32 errorCode );
      //
      virtual void OnWifiConnectSuccess( void );
      virtual void OnWifiConnectFailed( const nn::Result& result, u32 errorCode );
      virtual void OnWifiDisconnectSuccess( void );
      virtual void OnWifiDisconnecFailed( const nn::Result& result, u32 errorCode );

    private:

      //------------------------------------------------------------------
      /**
       * @brief   終了判定
       * @return  セクションを終了させるならtrueを返却
       */
      //------------------------------------------------------------------
      bool         IsFinished( void ) const;

      //------------------------------------------------------------------
      /**
       * @brief   Ｗｉｆｉアクセスポイントへの接続を開始する
       * @return  エラーが発生したならtrueを返却する
       */
      //------------------------------------------------------------------
      bool         StartupWifiSystem( void );

      //------------------------------------------------------------------
      /**
       * @brief   Ｓｏｃｋｅｔシステムの開始
       * @return  エラーが発生したならtrueを返却する
       */
      //------------------------------------------------------------------
      bool         StartupSocketSystem( void );

      //------------------------------------------------------------------
      /**
       * @brief  CTRのフレンドキーリストをマネージャにストア
       *
       * @return エラーが発生したならtrueを返却する
       */
      //------------------------------------------------------------------
      bool         StoreFriendList( void );

      //------------------------------------------------------------------
      /**
       * @brief   フレンドサーバへのログインを開始する
       * @return  エラーが発生したならtrueを返却する
       */
      //------------------------------------------------------------------
      bool         StartupFriendService( void );

      //------------------------------------------------------------------
      /**
       * @brief   NEXサービスを開始する
       * @return  エラーが発生したならtrueを返却する
       */
      //------------------------------------------------------------------
      bool         StartupNexService( void );

      //------------------------------------------------------------------
      /**
       * @brief   アクセスポイントとの接続を終了する
       *
       * @param[in]   isAsync 同期処理を行うならtrueを指定
       *
       * @return  エラーが発生したならtrueを返却する
       */
      //------------------------------------------------------------------
      bool         EndWifiSystem( bool isAsync );

      //------------------------------------------------------------------
      /**
       * @brief   Ｓｏｃｋｅｔシステムを終了する
       * @return  エラーが発生したならtrueを返却する
       */
      //------------------------------------------------------------------
      bool         EndSocketSystem( void );

      //------------------------------------------------------------------
      /**
       * @brief   フレンドサーバからのログアウトを開始する
       */
      //------------------------------------------------------------------
      void         EndFriendService( void );

      //------------------------------------------------------------------
      /**
       * @brief   NEXサービスを終了する
       */
      //------------------------------------------------------------------
      void         EndNexService( void );

      //------------------------------------------------------------------
      /**
       * @brief   HTTPシステムの利用を開始する
       * @return  エラーが発生したならtrueを返却する
       */
      //------------------------------------------------------------------
      bool         StartupHttpSystem( void );

      //------------------------------------------------------------------
      /**
       * @brief   HTTPシステムを終了する
       * @return  エラーが発生したならtrueを返却する
       */
      //------------------------------------------------------------------
      bool         EndHttpSystem( void );

      //------------------------------------------------------------------
      /**
       * @brief   ゲームサーバーへのログインを開始する
       */
      //------------------------------------------------------------------
      bool         LoginGameServer( void );

      //------------------------------------------------------------------
      /**
       * @brief   ゲームサーバーからログアウトする
       */
      //------------------------------------------------------------------
      bool         LogoutGameServer( void );

      //------------------------------------------------------------------
      /**
       * @brief   Subscriptionの開始
       */
      //------------------------------------------------------------------
      void         StartSubscription( void );

      //------------------------------------------------------------------
      /**
       * @brief   Subscriptionの終了
       *
       * @param[in]   isAsync 終了処理を非同期で実行するならtrueを指定
       *
       * @return  終了処理が完了したならtrueを返却
       */
      //------------------------------------------------------------------
      bool         EndSubscription( bool isAsync );

      //------------------------------------------------------------------
      /**
       * @brief   接続処理
       */
      //------------------------------------------------------------------
      void         Connect( void );

      //------------------------------------------------------------------
      /**
       * @brief   切断処理
       *
       * @param[in] isAsync 通信を非同期で実行するならtrueを指定
       */
      //------------------------------------------------------------------
      void         Disconnect( bool isAsync );

      //------------------------------------------------------------------
      /**
       * @brief   NEXユニークIDユーティリティのBind
       *
       * @return  BINDに失敗したならtrueを返却
       */
      //------------------------------------------------------------------
      bool         BindUtilityClient( void );

      //------------------------------------------------------------------
      /**
       * @brief   NEXユニークIDユーティリティのUnbind
       */
      //------------------------------------------------------------------
      void         UnbindUtilityClient( void );

      //------------------------------------------------------------------
      /**
       * @brief   NEXユニークIDの発行
       *
       * @return  発行処理に失敗したならtrueを返却
       */
      //------------------------------------------------------------------
      bool         AcquireNexUniqueId( void );

    private:
      static const u32            D_LOGIN_TIMEOUT  = 40000;     //!< ログインタイムアウト

    private:
      E_EXEC_MODE                 m_execMode;                   //!< 実行モード
      //
      bool                        m_isError;                    //!< エラーフラグ（即時終了）
      bool                        m_isErrorReserved;            //!< エラー予約（最終的にはエラーとするが、終了処理を行う）
      bool                        m_isFinish;                   //!< 終了フラグ
      //
      bool                        m_isFriendLoginFinish;        //!< フレンドサーバログイン完了フラグ
      bool                        m_isGameServerLoginFinish;    //!< ゲームサーバログイン完了フラグ
      bool                        m_isGameServerLogoutFinish;   //!< ゲームサーバログアウト完了フラグ
      bool                        m_isWifiConnectFinish;        //!< アクセスポイント接続完了フラグ
      bool                        m_isWifiDisconnectFinish;     //!< アクセスポイント切断完了フラグ
      //
      u32                         m_step;                       //!< 実行ステップ
      u32                         m_errorCode;                  //!< 発生したエラーコード
      //
      nn::nex::UniqueId           m_nexUniqueId;                //!< 発行されたNEXユニークID
      bool m_isSkipParentalLock;
      //
      nn::os::CriticalSection                    m_criticalSection;  //!< クリティカルセクション
      gflnet2::friends::FriendManager*            m_friendMgr;        //!< フレンドマネージャインスタンス
      gflnet2::nex::NexManager*                   m_nexMgr;           //!< NEXマネージャインスタンス
      NetLib::Wifi::SubscriptionManager* m_subscrptionMgr;   //!< Subscriptionマネージャインスタンス
      gflnet2::wifi::WifiSystem*                  m_wifiSystem;       //!< アクセスポイント管理システムインスタンス
      //gflnet2::err::ErrManager*                   m_errorMgr;         //!< エラーマネージャインスタンス


    private:
      bool         m_isGameServerTerminateFinish;              //!< ゲームサーバ終了手続完了フラグ
    public:
      virtual void OnGameServerTerminated( void );
    private:
      void         TerminateGameServer( void );

  };



  //------------------------------------------------------------------
  /**
   * @brief スリープ遷移時用インターネット接続の強制切断処理
   *
   * @note 同期通信にてインターネット接続の強制切断を実行する
   *
   * @return インターネット接続を切断したならtrueを返却する
   */
  //------------------------------------------------------------------
  extern bool ForceDisconnectWifi( void );

GFL_NAMESPACE_END(Wifi)
GFL_NAMESPACE_END(NetLib)

#endif // __WIFI_CONNECT_RUNNNER_

#endif