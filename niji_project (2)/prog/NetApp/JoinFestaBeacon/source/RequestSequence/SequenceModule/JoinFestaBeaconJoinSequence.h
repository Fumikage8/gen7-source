// ============================================================================
/*
 * @file JoinFestaBeaconJoinSequence.h
 * @brief 招待への参加を行うリクエストです。
 * @date 2015.06.23
 */
// ============================================================================
#if !defined( JOINFESTABEACONJOINREQUESTSEQUENCE_H_INCLUDE )
#define JOINFESTABEACONJOINREQUESTSEQUENCE_H_INCLUDE
#pragma once
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>

#include <macro/include/gfl2_macros.h>
#include "NetApp/JoinFestaBeacon/source/RequestSequence/JoinFestaBeaconRequestClient.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketDefine.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaNetDefine.h"
#ifdef GF_PLATFORM_CTR
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#endif // GF_PLATFORM_CTR


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)

//------------------------------------------------------------------
/**
* @brief  リクエスト
*/
//------------------------------------------------------------------
class JoinFestaBeaconJoinSequence :
  public gflnet2::ServerClient::RequestSequenceBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaBeaconJoinSequence );
private:

  enum
  {
    DUMMY_WAIT = 30 * 2,//ダミーウェイト2秒
    MAX_CLIENT_NUM = 3,
  };

public:
  //------------------------------------------------------------------
  /**
  * @brief  コンストラクタ/デストラクタ
  */
  //------------------------------------------------------------------
  JoinFestaBeaconJoinSequence();
  virtual~JoinFestaBeaconJoinSequence();

public://RequestSequenceBaseのオーバーライド

  //------------------------------------------------------------------
  /**
  * @brief  更新処理。
  *
  * @note   	メインループ毎にポーリングされる
  *			      継承して実装する通信シーケンス本体
  *           クラスに用意されているセッター/ゲッターを使用して個別の
  *           通信シーケンスを実装してください
  *           キャンセル操作が行われた後はこのアップデートは呼ばれなくなり、
  *           UpdateCancelが呼ばれるようになります。
  */
  //------------------------------------------------------------------
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update( void );

  //------------------------------------------------------------------
  /**
  * @brief  キャンセル処理の更新
  *
  * @note   	
  *			      リクエストをキャンセルするとUpdateではなくこちらが呼ばれるようになります。
  *           キャンセル処理が完了したらREQUEST_STATE_FINISHを返すとリクエストは終了します。
  *           キャンセルが行われなかった場合、この関数は呼ばれません。
  */
  //------------------------------------------------------------------
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE UpdateCancel( void );


  //------------------------------------------------------------------
  /**
  * @brief  リスナー登録解除
  */
  //------------------------------------------------------------------
  virtual void RemoveListener( void );

  //------------------------------------------------------------------
  /**
  * @brief  リスナー設定
  */
  //------------------------------------------------------------------
  void SetListener( JoinFestaBeaconResponseListener* pListener );

  //------------------------------------------------------------------
  /**
  * @brief  リクエストパラメータ設定
  */
  //------------------------------------------------------------------
  void SetRequestParam( JoinFestaBeaconRequestClient::REQUEST_PARAM& pParam );

private:
  //招待者がキャンセルしたか
  bool CheckOwnerCanceled( void );

  //接続開始
  void StartP2PConnect( void );
  //　初期化
  void Setup( void );

  //ブラックリストチェック
  bool CheckBlackList( void );

private:

  JoinFestaBeaconResponseListener*           m_pListener;
  JoinFestaBeaconRequestClient::REQUEST_PARAM m_requestParam;
#ifdef GF_PLATFORM_CTR
  NetAppLib::JoinFesta::JoinFestaPacketManager* m_pPacketManager;
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* m_pPersonalDataManager;
  NetLib::P2P::P2pConnectionManager* m_pP2pConnectionManager;
  NetAppLib::JoinFesta::JoinFestaPersonalData* m_pOwnerPersonalData;
  u32 m_ownerTicketID;
  bool m_isWifi;
  bool m_isTrade;
  u32 m_connectNum;
  u64 m_friendID[MAX_CLIENT_NUM];
  JoinFestaFriendKey m_friendKey[MAX_CLIENT_NUM];
  u32 m_sessionHostIndex;//子のうち一番IDが大きい人がセッションホスト
  bool m_isSessionMyHost;
#endif
};


GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTABEACONJOINREQUESTSEQUENCE_H_INCLUDE
