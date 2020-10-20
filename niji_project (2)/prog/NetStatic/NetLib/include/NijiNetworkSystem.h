//============================================================================================
/**
 * @file NijiNetworkSystem.h
 *
 * @author kousaka_shin
 */
//============================================================================================
#ifndef __NIJI_NETWORK_SYSTEM_
#define __NIJI_NETWORK_SYSTEM_
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>

#include <Battle/include/battle_SetupParam.h>
#include "ExtSavedata/include/BattleRecorderSaveData.h"

#include <gflnet2/include/nex/gflnet2_NexP2pStationManager.h>

#include <gflnet2/include/nex/gflnet2_NexUtilityListener.h>

// 前方宣言
namespace NetLib{
  namespace Error{
    class NijiNetworkErrorDialog;
    class NijiNetworkErrorDetectDefault;
    class NijiNetworkErrorDetectBattleSpot;
  }
#if defined(GF_PLATFORM_CTR)
  namespace Beacon{
    class LocalBeaconTrader;
  }
#endif
}
namespace NetAppLib {
  namespace JoinFesta {
    class JoinFestaInformationMessage;
  }

  namespace P2PTrade {
    class P2PTradeRequestClient;
  }
}


GFL_NAMESPACE_BEGIN(NetLib)

//--------------------------------------------------------------------------------------------
/**
* @brief    録画送信クラス
*/
//--------------------------------------------------------------------------------------------
class LiveRecSender
{
public:

  enum
  {
    SEQ_INIT = 0,
    SEQ_START_FIEST,
    SEQ_START_SECOND,
    SEQ_START_MAIN,
    SEQ_MAIN,
    SEQ_RELOAD,
  };

  LiveRecSender();
  ~LiveRecSender();

  void Update();

  void ResetReload();
  void Reload();

  void Stop(void);

  void SetMyNetID( u32 id )
  { 
    m_myNetID = id; 
  }
  void SetBattleSetupParam( BATTLE_SETUP_PARAM* param )
  { 
    m_pBattleSetupParam = param;
  }
  
  //@fix[fix]NMCat[4203]:[#4203 【観戦機能】対戦側と観戦側で別のBGMが再生される]
  void SetRegulation( Regulation* pRegulation )
  {
    m_pRegulation = pRegulation;
  }

  //------------------------------------------------------------------
  /**
    * @brief   選出インデックス送信
    */
  //------------------------------------------------------------------
  void SendLiveRecSelectedIndex( s8 join_order[ BTL_CLIENT_NUM ][ pml::PokeParty::MAX_MEMBERS ] );

private:
  u32 m_seq;
  u32 m_startSeq;
  ExtSavedata::BattleRecorderSaveData*  m_pRecorderSaveData;
  u32 m_myNetID;
  BATTLE_SETUP_PARAM* m_pBattleSetupParam;
  
  s8 m_join_order[ BTL_CLIENT_NUM ][ pml::PokeParty::MAX_MEMBERS ];
  bool m_isSelectIndexRequest;

  //@fix[fix]NMCat[4203]:[#4203 【観戦機能】対戦側と観戦側で別のBGMが再生される]
  Regulation* m_pRegulation;
};

#if defined(GF_PLATFORM_CTR)

//--------------------------------------------------------------------------------------------
/**
* @brief  バトルスポット通信監視システム
*/
//--------------------------------------------------------------------------------------------
class BattleSpotNetworkMonitor
  : public gflnet2::nex::NexUtilityListener
{
public:
  BattleSpotNetworkMonitor();
  virtual ~BattleSpotNetworkMonitor();


  void Update( void );

public:

  void ResetMonitor( void );

  //------------------------------------------------------------------
  /**
   *	@brief ゲームサーバ疎通テスト結果通知
   */
  //------------------------------------------------------------------
  virtual void OnTestConnectivityResult( const bool isSuccess );

private:

  gflnet2::nex::NexP2pStationManager m_nexP2pStationManager;
  bool   m_isTestRunning;
};

#endif

//--------------------------------------------------------------------------------------------
/**
* @brief  Niji通信システム
*         通信系のシングルトンクラスを生成しアップデートする
*/
//--------------------------------------------------------------------------------------------
class NijiNetworkSystem
{
public:
  //-------------------------------------
  /// 通信モード
  //=====================================
  enum NETWORK_MODE
  {
    NETWORK_MODE_OFFLINE = 0,//オフラインモード
    NETWORK_MODE_LOCAL,      //ローカル通信モード
    NETWORK_MODE_WIFI,       //Wifi通信モード
  }; 

private:

  //-------------------------------------
  /// エラー検知種類
  //=====================================
  typedef enum{
    ERROR_DETECT_TYPE_DEFAULT,    ///< デフォルト
    ERROR_DETECT_TYPE_BATTLESPOT, ///< バトルスポット
  } ENUM_ERROR_DETECT_TYPE;


public:
  //------------------------------------------------------------------
  /**
   * @brief  コンストラクタ/デストラクタ
   */
  //------------------------------------------------------------------
  NijiNetworkSystem( gfl2::heap::HeapBase* pHeap );
  ~NijiNetworkSystem();

private:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //------------------------------------------------------------------
  void InitializeInternal( gfl2::heap::HeapBase* pHeap );

  //------------------------------------------------------------------
  /**
   * @brief   終了
   */
  //------------------------------------------------------------------
  void TerminateInternal( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   前更新
   */
  //------------------------------------------------------------------
  void UpdateBefore( void );

  //------------------------------------------------------------------
  /**
   * @brief   後更新
   */
  //------------------------------------------------------------------
  void UpdateAfter( void );

  //------------------------------------------------------------------
  /**
    * @brief   アプレット更新
    */
  //------------------------------------------------------------------
  void UpdateErrorApplet( void );

private:

  //------------------------------------------------------------------
  /**
   * @brief   通信の共通の初期化
   */
  //------------------------------------------------------------------
  static void InitializeNetworkInternal( void );


  //------------------------------------------------------------------
  /**
    * @brief   通信エラー検知の作成
    * @param   detectType エラー検知種類
    */
  //------------------------------------------------------------------
  static void CreateErrorDetect( ENUM_ERROR_DETECT_TYPE detectType );

  //------------------------------------------------------------------
  /**
    * @brief   通信エラー検知の削除
    */
  //------------------------------------------------------------------
  static void DeleteErrorDetect( void );

public:

  //------------------------------------------------------------------
  /**
   * @brief   Wifi通信の初期化
   */
  //------------------------------------------------------------------
  static void InitializeWifiNetwork( void );

  //------------------------------------------------------------------
  /**
   * @brief   ローカル通信の初期化
   */
  //------------------------------------------------------------------
  static void InitializeLocalP2PNetwork( void );

  //------------------------------------------------------------------
  /**
   * @brief   通信の終了(Wifi、ローカル共通)
   */
  //------------------------------------------------------------------
  static void TerminateNetwork( void );

  //------------------------------------------------------------------
  /**
   * @brief   通信用ヒープの取得
   */
  //------------------------------------------------------------------
  static gfl2::heap::HeapBase* GetNetworkHeap( void );

  //------------------------------------------------------------------
  /**
   * @brief   通信エラー検知をバトルスポット専用に切り替える
   */
  //------------------------------------------------------------------
  static void SwitchErrorDetectToBattleSpot( void );

  //------------------------------------------------------------------
  /**
   * @brief   通信エラー検知をデフォルトに切り替える
   */
  //------------------------------------------------------------------
  static void SwitchErrorDetectToDefault( void );

  // @fix NMCat[4197]：バトルスポットからの参加選択画面で通信エラーーが発生した時に、通知されない不具合修正用
  //------------------------------------------------------------------
  /**
   * @brief   通信エラー検知がバトルスポット専用かどうか
   */
  //------------------------------------------------------------------
  static bool IsErrorDetectForBattleSpot( void );

  //------------------------------------------------------------------
  /**
   * @brief   ネットワークモードの取得
   */
  //------------------------------------------------------------------
  static NijiNetworkSystem::NETWORK_MODE GetNetworkMode( void );

  //------------------------------------------------------------------
  /**
   * @brief   ビーコンシステムの開始
   */
  //------------------------------------------------------------------
  static void StartBeaconSystem( void );

  //------------------------------------------------------------------
  /**
    * @brief   送信するビーコンデータを設定
    */
  //------------------------------------------------------------------
  static void SetBeaconData( const u8* pSendData, u32 size );

  //------------------------------------------------------------------
  /**
    * @brief   ビーコンシステムの停止(trueが返るまで呼ぶ)
    *          この関数で停止すると勝手に再開しなくなる
    */
  //------------------------------------------------------------------
  static bool UpdateStopBeaconSystem( void );

  //------------------------------------------------------------------
  /**
   * @brief   ビーコンシステムの再開
   */
  //------------------------------------------------------------------
  static void RestartBeaconSystem( void );

  //------------------------------------------------------------------
  /**
  * @brief  無線LANスイッチが有効かどうか
  *
  * @return 有効true : 無効false
  */
  //------------------------------------------------------------------
  static bool IsWLANSwitchEnable( void );


  //------------------------------------------------------------------
  /**
  * @brief  ゲームサーバ接続中かどうか
  *
  * @return 有効true : 無効false
  */
  //------------------------------------------------------------------
  static bool IsGameServerConnecting( void );

  //------------------------------------------------------------------
  /**
  * @brief  一時的に取得するビーコンのIDを設定
  */
  //------------------------------------------------------------------
  static void SetTempBeaconID( u8 tempID );


  //------------------------------------------------------------------
  /**
  * @brief  オンライン中の他プレイヤーの人数を取得
  */
  //------------------------------------------------------------------
  static u32 GetOnlinePlayerNum( void );


  //------------------------------------------------------------------
  /**
  * @brief  ペアレンタルモード判定
  */
  //------------------------------------------------------------------
  static bool  IsParentalMode( void );
  static bool  IsParentalModeOnlyInternet( void );

  //------------------------------------------------------------------
  /**
    * @brief   P2P交換通信処理クラスの設定
    * @note    設定すると UpdateBefore で Update が呼ばれる
    */
  //------------------------------------------------------------------
  static void SetP2PTradeRequestClient( NetAppLib::P2PTrade::P2PTradeRequestClient* pP2PTradeRequestClient );

  //------------------------------------------------------------------
  /**
    * @brief   P2P交換通信処理クラスの登録解除
    */
  //------------------------------------------------------------------
  static void RemoveP2PTradeRequestClient();

  //------------------------------------------------------------------
  /**
    * @brief   ライブ大会観戦送信機能作成
    */
  //------------------------------------------------------------------
  static void CreateLiveRecSender( void );


  //------------------------------------------------------------------
  /**
    * @brief   ライブ大会観戦送信機能削除
    */
  //------------------------------------------------------------------
  static void DeleteLiveRecSender( void );


  //------------------------------------------------------------------
  /**
    * @brief   ライブ大会観戦送信機能セットアップ
    */
  //------------------------------------------------------------------
  static void SetupLiveRecSender( u32 myNetID , BATTLE_SETUP_PARAM* param );

  //@fix[fix]NMCat[4203]:[#4203 【観戦機能】対戦側と観戦側で別のBGMが再生される]
  //------------------------------------------------------------------
  /**
    * @brief   ライブ大会観戦送信機能レギュレーションセット
    */
  //------------------------------------------------------------------
  static void SetRegulation( Regulation* pRegulation );

  //------------------------------------------------------------------
  /**
    * @brief   ライブ大会観戦送信機能リセット
    */
  //------------------------------------------------------------------
  static void ResetLiveRecSender( void );

  //------------------------------------------------------------------
  /**
    * @brief   ライブ大会観戦送信機能リロード
    */
  //------------------------------------------------------------------
  static void ReloadLiveRecSender( void );

  //------------------------------------------------------------------
  /**
    * @brief   ライブ大会観戦機能が有効か
    */
  //------------------------------------------------------------------
  static bool IsLiveRecSenderEnable( void );

  //------------------------------------------------------------------
  /**
    * @brief   選出インデックス送信
    */
  //------------------------------------------------------------------
  static void SendLiveRecSelectedIndex( s8 join_order[ BTL_CLIENT_NUM ][ pml::PokeParty::MAX_MEMBERS ] );


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  static void  NijiNetworkSystem::StopLiveRecSender(void);


  //------------------------------------------------------------------
  /**
    * @brief   バトルスポット通信監視機能作成
    */
  //------------------------------------------------------------------
  static void CreateBattleSpotNetworkMonitor( void );

  //------------------------------------------------------------------
  /**
    * @brief   バトルスポット通信監視機能削除
    */
  //------------------------------------------------------------------
  static void DeleteBattleSpotNetworkMonitor( void );

  //------------------------------------------------------------------
  /**
    * @brief   バトルスポット通信監視機能更新
    */
  //------------------------------------------------------------------
  static void UpdateBattleSpotNetworkMonitor( void );

  //------------------------------------------------------------------
  /**
    * @brief   バトルスポット通信監視機能更新
    */
  //------------------------------------------------------------------
  static void ResetBattleSpotNetworkMonitor( void );

#if PM_DEBUG
  //P2Pマッチング履歴クリア
  static void SetClearCacheFlag( const bool flag );
#endif

private:
  Error::NijiNetworkErrorDialog*                        m_networkErrorDialog;       ///< エラーダイアログ管理クラス
  static Error::NijiNetworkErrorDetectDefault*          m_errorDetectDefault;       ///< 通信エラー検知クラス(デフォルト)
  static Error::NijiNetworkErrorDetectBattleSpot*       m_errorDetectBattleSpot;    ///< 通信エラー検知クラス(バトルスポット)
  static NijiNetworkSystem::NETWORK_MODE                m_networkMode;              ///< ネットワークモード（オフライン、Wifi、ローカル）
#if defined(GF_PLATFORM_CTR)
  static Beacon::LocalBeaconTrader*                     m_pBeaconTrader;            ///< ローカルビーコン送受信クラス
  static BattleSpotNetworkMonitor*                      m_pBSNetworkMonitor;        ///< バトルスポット通信監視
#endif
  NetAppLib::JoinFesta::JoinFestaInformationMessage*    m_pInfomationMessage;       ///< ストリーミングクラス
  static NetAppLib::P2PTrade::P2PTradeRequestClient*    m_pP2PTradeRequestClient;   ///< P2P交換通信処理クラス
  static LiveRecSender*                                 m_pLiveRecSender;           ///< ライブ録画送信クラス

};


GFL_NAMESPACE_END(NetLib)

#endif // __NIJI_NETWORK_SYSTEM_