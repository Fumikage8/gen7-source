// ============================================================================
/*
 * @file NetworkBattleEvent.h
 * @brief 通信対戦イベント
 * @date 2015.08.18
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NETWORK_BATTLE_EVENT_H_INCLUDE )
#define NETWORK_BATTLE_EVENT_H_INCLUDE
#pragma once

#include <pml/include/pmlib.h>
#include <macro/include/gfl2_Macros.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"
#include "Savedata/include/MyStatus.h"
#include "ExtSavedata/include/BattleRecorderSaveData.h"

// バトル
#include "Battle/include/battle_SetupParam.h"
// ポケモンリスト呼び出しイベント
#include "NetStatic/NetEvent/include/PokeListJoinEvent.h"

// バトルロイヤル結果アプリのパラメータ
#include "App/BattleRoyalResult/include/BattleRoyalResultAppParam.h"
// バトルビデオ録画アプリのパラメータ
#include "NetApp/BattleVideoRecording/include/BattleVideoRecordingEventAppParam.h"


// 前方宣言
class Regulation;

// VSデモのパラメータ
namespace App { namespace AppDemo { class AppDemoProc_VS;  struct AppDemoProcParam_VS;} }

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(NetworkBattle)

/**
 * @struct PersonalData
 * @brief  通信対戦で必要なデータの一人分
 */
struct PersonalData{
  pml::PokeParty*       pPokeParty; //! 参加者のポケパーティ（フル）
  Savedata::MyStatus*   pMyStatus;  //! 参加者のマイステータス
  bool                  qrTeam;     //! QRバトルチームか
  u32                   rate;       //! レート値

  // コンストラクタ
  PersonalData() : pPokeParty(NULL), pMyStatus(NULL), qrTeam(false), rate(0) {}
};

//-------------------------------------
/// イベント呼び出しパラメータ構造体
//=====================================
struct EVENT_PARAM{
  PersonalData*                       pPersonalData[ BTL_CLIENT_NUM ];  //! [in]参加者データ
  Regulation*                         pRegulation;                      //! [in]レギュレーション
  BTL_CLIENT_ID                       standingPos;                      //! [in]戦う立ち位置
  u32                                 bgm;                              //! [in]選択されたBGM

  b8                                  isAfterBattle;                    //! [out]バトル実行後かどうか
  b8                                  isIllegalBattle;                  //! [out]不整合フラグ
  b8                                  isServerVersionHigh;              //! [out]バトルに使用したサーバーバージョンが自分のと違う場合 true
  BtlResult                           battleResult;                     //! [out]勝敗結果
  BattleRecordHeader                  recHeader;                        //! [out]録画データのヘッダ情報（Battleから）
  BATTLE_PGL_RECORD_SET               *pglRecord;                       //! [out]PGL集計データ（NULLなら出力しない）
  ExtSavedata::BattleRecorderSaveData *pBattleRecorderSaveData;         //! [out]リプレイデータ格納先（NULLなら出力しない）
  u8                                  commServerVer;                    //! [out]通信対戦時のサーババージョン（接続したマシン中、最後バージョン）
  // @fix NMCat[2698]：ホストマイグレーションだと通信エラーではないのでこのフラグで見る必要がある。
  b8                                  isDisconnectOccur;                //! [out]バトル中に切断が発生したらtrue、発生していなければfalse（通信エラーではない）
  u8                                  battlePartyMembers[ BTL_CLIENT_NUM ];  //! [out]バトルに参加したポケモンの数

  // コンストラクタ
  EVENT_PARAM() :
    pRegulation(NULL),
    standingPos(),
    bgm(0),
    isAfterBattle(false),
    isIllegalBattle(false),
    isServerVersionHigh(false),
    battleResult(),
    recHeader(),
    pglRecord(NULL),
    pBattleRecorderSaveData(NULL),
    commServerVer(0),
    isDisconnectOccur(false)
  {
    for( int i=0; i<BTL_CLIENT_NUM; i++ ){ pPersonalData[i] = NULL; battlePartyMembers[i] = 0;}
  }
};

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------
/**
 *	@brief  通信対戦受付アプリ起動クラス
 */
//----------------------------------------------------------------------
class NetworkBattleEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( NetworkBattleEvent );

public:
  //--------------------------------------------------------------
  /**
   * @brief  通信対戦受付アプリ呼び出し
   * 
   * @param   pGameManager  ゲームマネージャ
   * @param   pEventParam   イベント呼び出しパラメータ構造体
   */
  //--------------------------------------------------------------
  static void StartEvent( GameSys::GameManager* pGameManager, EVENT_PARAM* pEventParam );

  // コンストラクタ
  NetworkBattleEvent( gfl2::heap::HeapBase* pHeap );
  // デストラクタ
  virtual~NetworkBattleEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

private:

  void SetEventParam( EVENT_PARAM* pEventParam ){ m_pEventParam = pEventParam; };

private:

  /**
   * @enum Sequence
   *       シーケンス定義
   */
  enum Sequence{
    SEQ_CALL_APP,   //! アプリ呼び出し
    SEQ_RETURN_APP, //! アプリ戻り
    SEQ_END,        //! 終了
  };

  /**
   * @enum CallAppType
   *       呼び出しアプリ種類
   */
  enum CallAppType{
    CALL_POKELIST,          //! 参加ポケモン選択
    CALL_BEFORE_BATTLE_DEMO,//! 戦闘前VSデモ
    CALL_BATTLE,            //! バトル
    CALL_AFTER_BATTLE_DEMO, //! 戦闘後VSデモ
    CALL_BATTLE_VIDEO_SAVE, //! バトルビデオ録画確認
    CALL_BATTLE_ROYAL_RESULT, //! バトルロイヤル結果
  };

  //! 呼び出すアプリ設定
  void SetCallApp( CallAppType callAppType );
  //! アプリ呼び出し
  void CallApp( GameSys::GameManager* pGameManager );
  //! アプリ戻り
  void ReturnApp();


  //! ポケモンリストアプリ呼び出しパラメータの初期化
  void InitializePokeListAppParam();
  //! 戦闘前デモアプリ呼び出しパラメータの初期化
  void InitializeBeforeBattleDemoParam( App::AppDemo::AppDemoProcParam_VS& procParam );
  //! 戦闘後デモアプリ呼び出しパラメータの初期化
  void InitializeAfterBattleDemoParam( App::AppDemo::AppDemoProcParam_VS& procParam );
  //! バトルロイヤル結果アプリのパラメータ初期化
  void InitializeBattleRoyalResultSetupParam();
  //! バトルビデオ録画アプリのパラメータ初期化
  void InitializeBattleVideoRecordingParam();
  //! バトル呼び出しパラメータの初期化
  void InitializeBattleSetupParam();


  //! バトルへの参加ポケモン決定
  void DecideBattleParty();

  //! ライブ大会観戦用の選択ポケモンインデックス送信処理
  void SendSelectedIndexForLiveCup();

  //! 見せ合いデータ設定
  void SetupMiseaiData();

  //! バトルクライアントIDをデモIndexにする
  u32 BtlClientId2DemoIndex( const int btlClientId );

  //! バトル後のレコード加算
  void AfterBattleRecordAdd();

private:

  EVENT_PARAM*                                  m_pEventParam;            //! Event呼び出しパラメータ
  Sequence                                      m_sequence;               //! シーケンス
  bool                                          m_isStartBattleBgm;       //! 戦闘曲再生フラグ

  // 呼び出しアプリ
  CallAppType                                   m_callAppType;            //! 呼び出しアプリ種類

  // アプリ呼び出しパラメータ
  BATTLE_SETUP_PARAM                            m_battleSetupParam;       //! バトルセットアップパラメータ
  NetEvent::PokeListJoin::EVENT_PARAM           m_pokeListJoinParam;      //! 参加ポケモン選択イベント
  App::BattleRoyalResult::APP_PARAM             m_battleRoyalResultParam; //! バトルロイヤル結果アプリパラメータ
  BattleVideoRecording::EVENT_APP_PARAM         m_battleVideoRecordingParam;  //! バトルビデオ録画アプリのパラメータ 

  // 実体
  pml::PokeParty*                               m_pBattleParty[ BTL_CLIENT_NUM ]; //! 戦闘に出すパーティ
  MiseaiData*                                   m_pMiseaiData[ BTL_CLIENT_NUM ]; //! 見せ合いデータ
  ExtSavedata::BattleRecorderSaveData           m_battleRecorderSaveData; //! バトルビデオ拡張セーブクラス

#if PM_DEBUG
  static u8 s_dbgLiveRecSendFlag;//!観戦機能ONフラグ
  public:
  static u8* GetDbgLiveRecSendFlagPrt(void){ return &s_dbgLiveRecSendFlag; }
#endif
};


GFL_NAMESPACE_END(NetworkBattle)
GFL_NAMESPACE_END(NetEvent)

#endif // NETWORK_BATTLE_EVENT_H_INCLUDE
