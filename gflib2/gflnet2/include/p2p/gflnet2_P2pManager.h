#ifndef __GFLNET_P2PMANAGER_H__
#define __GFLNET_P2PMANAGER_H__
//=============================================================================
/**
 * @file	 gflnet_P2pManager.h
 * @brief	 通信管理クラス P2PNex用
           マネージャーは通信ひとつにつき一個のつもりで設計してある
 * @author k.ohno -> ichiraku（niji移植 大幅改造）
 *
 *  メモ：接続開始を呼ぶと、ConnectionProcが動く。
 *        ・P2P接続成功した場合は、ConnectionProcが終了し、MainProcが動く。
 *        ・切断する場合は、MainProcを終了しDisconnectProcを動かす。
 *
 *        ・P2P接続中にエラーが起きた場合は、ConnectionProcを自動終了し、空ループしながら指示を待つ。
 *        ・P2P接続成功後にエラーが起きた場合は、MainProcを自動終了し、空ループしながら指示を待つ。
 *        ・エラー後に、切断する場合は、DisconnectProcを動かす。
 *        ・エラー後に、接続リトライする場合は、DisconnectProcの後にConnectionProcを動かす。
 *
 * @date	 2011.1.6
 */
//=============================================================================
#pragma once

#ifdef  GF_PLATFORM_CTR
#include <nex.h>
#include <nex_P2p.h>
#endif // GF_PLATFORM_CTR 

#include <gflnet2/include/p2p/gflnet2_P2pDefine.h>
#include <gflnet2/include/p2p/gflnet2_P2pConfig.h>
#include <gflnet2/include/p2p/gflnet2_NetGame.h>
#include <gflnet2/include/base/gflnet2_InitParam.h>
#include <gflnet2/include/beacon/gflnet2_BeaconManager.h>
#include <gflnet2/include/nex/gflnet2_NexMatchMakingClient.h>
#include <gflnet2/include/p2p/gflnet2_P2pManagerEventListener.h>
#include <util/include/gfl2_List.h>
#include <proc/include/gfl2_ProcManager.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(p2p)

/**
 * @brief P2p通信の管理を行うクラス
 */
class P2pManager : public gfl2::proc::Manager
{
  GFL_FORBID_COPY_AND_ASSIGN(P2pManager);

public:
  //-------------------------------------
  /// ProcReturn定義
  //=====================================
  typedef enum{
    E_PROC_RETURN_SUCCESS,  ///< 成功
    E_PROC_RETURN_ERROR,    ///< エラー
  }E_PROC_RETURN;

private:
  //-------------------------------------
  /// 状態定義
  //=====================================
  typedef enum{
    E_STATE_NONE = 0,     ///< なし
    E_STATE_INITIALIZE,   ///< 初期化済み
    E_STATE_CONNECT_WAIT, ///< 接続待ち
    E_STATE_CONNECT_START,///< 接続開始
    E_STATE_CONNECT_PARENT_WAIT,///< 親からの開始号令待ち
    E_STATE_MAIN,         ///< 接続中
    E_STATE_DISCONNECT_IDLING,///< 切断リクエスト待機
    E_STATE_MAIN_PROC_FINISH_WAIT,///< メインプロック終了待ち
    E_STATE_DISCONNECT_WAIT,  ///< 切断待ち
    E_STATE_DISCONNECT_END,   ///< 切断完了
  }E_STATE;

private:
  static gfl2::heap::HeapBase* m_pHeapSys;
  static gfl2::heap::HeapBase* m_pHeapDev;
  static P2pManagerEventListener*  m_listener;     ///< イベントリスナー

  gflnet2::InitParam        m_InitParam;    ///< 通信初期化設定クラス
  E_STATE                   m_state;        ///< 状態
  E_PROC_RETURN             m_procReturn;   ///< P2Pプロセス戻り値
  bit64                     m_callTransferedID[ CONNECT_MAX_FOUR ];  ///< 相手のTransferedID
  bool                      m_requestDisconnect;  ///< 切断リクエストフラグ
  s32                       m_threadPriority;///<スレッド優先度
  //
  bool                      m_useSimpleMatchmake;  //!< シンプルマッチメイク使用するか（ミラクル交換専用）

  //ビーコン情報
  const void* m_pBeaconData;
  u32 m_beaconSize;
  u8 m_beaconGameID;
  u8 m_beaconModeID;

private:
  // 独自メモリ確保関数
  static void* MallocFunction(size_t stSize);
  static void FreeFunction(void* pMemory);

public:

  //---------------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   pHeapSys  システムHEAP
   * @param   pHeapDev  デバイスHEAP
   */
  //---------------------------------------------------------------------------------
  P2pManager( gfl2::heap::HeapBase* pHeapSys, gfl2::heap::HeapBase* pHeapDev );

  //---------------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //---------------------------------------------------------------------------------
  virtual ~P2pManager();

  //------------------------------------------------------------------
  /**
   * @brief   イベントリスナー登録
   * @param   listener   通知を受け取るP2pManagerEventListener
   */
  //------------------------------------------------------------------
  static void RegistListener( P2pManagerEventListener* listener );

  //------------------------------------------------------------------
  /**
   * @brief   イベントリスナー解除
   */
  //------------------------------------------------------------------
  static void UnregistListener( void );

  //---------------------------------------------------------------------------------
  /**
   * @brief   通信の初期化
   * @param   pIParam     動的初期化パラメータ
   * @param   pIParamFix  静的初期化パラメータ
   * @return  初期化に成功 true 失敗 false
   */
  //---------------------------------------------------------------------------------
  bool Initialize( const gflnet2::InitParameter* pIParam, const gflnet2::InitParameterFixed* pIParamFix );

  //---------------------------------------------------------------------------------
  /**
   * @brief   シンプルマッチメイク使用するかどうか
   * @attention ミラクル交換専用です。
   */
  //---------------------------------------------------------------------------------
  void UseSimpleMatchmake();

  //---------------------------------------------------------------------------------
  /**
   * @brief   通信のメインループ この中でproc::Mainを呼んでいる
   */
  //---------------------------------------------------------------------------------
  void NetMain(void);

  //------------------------------------------------------------------
  /**
    * @brief   ビーコン設定（ローカルの親のみ）
    */
  //------------------------------------------------------------------
#ifdef  GF_PLATFORM_CTR
  void SetLocalP2PMasterBeacon( const void* pBeaconData , u32 beaconSize , u8 gameID , u8 modeID );
#endif // GF_PLATFORM_CTR 

  //------------------------------------------------------------------
  /**
   * @brief   接続開始
   *
   * @note    正常終了時は、「OnP2pManagerConnectSuccess」がコールされる。
   * @note    エラー時は、「OnP2pManagerError」がコールされる。
   *
   * @param   connectType     接続種類（自動、親、子）
   * @param   transferdIdList 接続相手のTransferdIDリスト（自動ならNULL）
   * @param   pBeaconManager  ビーコンマネージャークラスポインタ ゲームで1つもっているはずなのでそれを渡す
   * @return  trueで開始成功
   * @note 呼び出し後は、IsConnectErrorとIsConnectEndでハンドリングする
   */
  //------------------------------------------------------------------
#ifdef  GF_PLATFORM_CTR
  bool ConnectStart( const CONNECT_TYPE connectType, gfl2::util::List<bit64>* transferdIdList, gflnet2::base::BeaconManager* pBeaconManager );
#endif // GF_PLATFORM_CTR 

  //------------------------------------------------------------------
  /**
   * @brief   接続開始(Wifi接続時のランダムマッチング専用）

   * @note    正常終了時は、「OnP2pManagerConnectSuccess」がコールされる。
   * @note    エラー時は、「OnP2pManagerError」がコールされる。

   * @param   connectType     接続種類（自動、親、子）
   * @param   ruleLest        マッチング条件リスト
   * @return  trueで開MAIN始成功
   * @note 呼び出し後は、IsConnectErrorとIsConnectEndでハンドリングする
   */
  //------------------------------------------------------------------
#ifdef  GF_PLATFORM_CTR
  bool ConnectStartWifi( const CONNECT_TYPE connectType, gflnet2::nex::NexMatchMakingClient::NexMatchMakingRuleList* ruleList );
#endif // GF_PLATFORM_CTR 

  //------------------------------------------------------------------
  /**
   * @brief   切断開始

   * @note    正常終了時は、「OnP2pManagerDisconnectSuccess」がコールされる。
   * @note    エラー時は、「OnP2pManagerError」がコールされる。

   * @return  trueで開始成功
   */
  //------------------------------------------------------------------
  bool DisconnectStart();

  //------------------------------------------------------------------
  /**
   * @brief   通信初期化構造体を取得
   * @return  InitParamのポインタ
   */
  //------------------------------------------------------------------
  InitParam* GetInitParam( void ){ return &m_InitParam; };

  //------------------------------------------------------------------
  /**
   * @brief   通信状態PROCの戻り値をセット
   */
  //------------------------------------------------------------------
  void SetProcReturn( E_PROC_RETURN procReturn ){ m_procReturn = procReturn; };

  //------------------------------------------------------------------
  /**
   * @brief P2pManager通信のエラー発生
   */
  //------------------------------------------------------------------
  void ErrorOccur( void );

#ifdef GF_PLATFORM_CTR
  //------------------------------------------------------------------
  /**
   * @brief P2pManager通信のエラー発生
   * @param[in] result     実行結果
   * @param[in] errorCode  エラーアプレット用エラーコード
   */
  //------------------------------------------------------------------
  void ErrorOccur( const nn::nex::qResult& result, u32 errorCode );
#endif // GF_PLATFORM_CTR

  //------------------------------------------------------------------
  /**
   * @brief P2P切断発生
   */
  //------------------------------------------------------------------
  void SessionLeftOccur( void );

  //------------------------------------------------------------------
  /**
   * @brief マッチメイクエラー（独自ハンドリング版）
   */
  //------------------------------------------------------------------
  void MatchMakeFailedOccur( void );

  //------------------------------------------------------------------
  /**
   * @brief   自分のTransferdIDを取得
   * @return  TransferdID
   */
  //------------------------------------------------------------------
  bit64 GetTransferedID( void );

  //------------------------------------------------------------------
  /**
   * @brief   呼び出し相手のTransferdIDが設定されているか
   * @return  trueで設定されている
   */
  //------------------------------------------------------------------
  bool IsCallTransferdIdIn( void );

  //------------------------------------------------------------------
  /**
   * @brief   呼び出し相手のTransferdIDか判定
   * @param   判定するTransferdID
   * @retval  true:呼び出し相手だった
   * @retval  false:呼び出し相手ではない
   */
  //------------------------------------------------------------------
  bool CheckCallTransferedID( const bit64 checkTransferedID );


  //------------------------------------------------------------------
  /**
    * @brief   ThreadPriorityを取得
    * @return   ThreadPriority
    */
  //------------------------------------------------------------------
  int GetThreadPriority( void ) const;

  //------------------------------------------------------------------
  /**
    * @brief   ThreadPriorityを設定
    * @param   ThreadPriority
    */
  //------------------------------------------------------------------
  void SetThreadPriority( int threadPriority );

public:

  //=============================================================================
  /**
  *  マイグレーション（親交代）リスナー
  */
  //=============================================================================
  //------------------------------------------------------------------
  /**
   * @brief マイグレーション成功
   */
  //------------------------------------------------------------------
  virtual void OnMigrationSuccess();

  //=============================================================================
  /**
  *  マッチメイクセッション作成成功リスナー
  */
  //=============================================================================
#ifdef GF_PLATFORM_CTR
  //------------------------------------------------------------------
  /**
   * @brief マッチメイクセッション作成成功
   */
  //------------------------------------------------------------------
  void MatchmakingCreateSuccess( const nn::nex::GatheringID& gatheringId );
#endif // GF_PLATFORM_CTR

private:

  //------------------------------------------------------------------
  /**
   * @brief   呼び出し相手のTransferdIDを設定
   * @param   相手のTransferdID
   */
  //------------------------------------------------------------------
  void SetCallTransferedID( const bit64 transferedID );

  //------------------------------------------------------------------
  /**
   * @brief   呼び出し相手のTransferdIDをクリア
   */
  //------------------------------------------------------------------
  void ClearCallTransferedID( void );

  //------------------------------------------------------------------
  /**
   * @brief   接続用IDを送信
   * @return  trueでOK
   */
  //------------------------------------------------------------------
  bool SendConnectionID( void );

  //------------------------------------------------------------------
  /**
   * @brief   親からの接続開始号令待ち
   * @return  trueでOK
   */
  //------------------------------------------------------------------
  bool IsEnableStartGame( void );

  //------------------------------------------------------------------
  /**
   * @brief   P2P接続成功時処理
   */
  //------------------------------------------------------------------
  void P2pConnectSuccess();

  //------------------------------------------------------------------
  /**
   * @brief   P2P接続中のProcをコール
   */
  //------------------------------------------------------------------
  void P2pMainProcCall();

  //------------------------------------------------------------------
  /**
   * @brief   P2P切断のProcをコール
   */
  //------------------------------------------------------------------
  void P2pDisconnectProcCall();

  //------------------------------------------------------------------
  /**
   * @brief   P2P切断成功時処理
   */
  //------------------------------------------------------------------
  void P2pDisconnectEnd();

};


GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)

#endif // __GFLNET_P2PMANAGER_H__

