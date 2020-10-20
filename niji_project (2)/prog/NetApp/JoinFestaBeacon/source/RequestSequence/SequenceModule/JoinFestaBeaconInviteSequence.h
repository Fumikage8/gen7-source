// ============================================================================
/*
 * @file JoinFestaBeaconInviteSequence.h
 * @brief 招待を行うリクエストです。
 * @date 2015.06.23
 */
// ============================================================================
#if !defined( JOINFESTABEACONDELETEREQUESTSEQUENCE_H_INCLUDE )
#define JOINFESTABEACONDELETEREQUESTSEQUENCE_H_INCLUDE
#pragma once
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>

#include <macro/include/gfl2_macros.h>
#include "NetApp/JoinFestaBeacon/source/RequestSequence/JoinFestaBeaconRequestClient.h"
#include "NetApp/JoinFestaBeacon/source/RequestSequence/SequenceModule/JoinFestaBeaconInviteSequence.h"

// タイムアウト計測
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)

//------------------------------------------------------------------
/**
* @brief  バトルビデオの削除を行うリクエスト
*/
//------------------------------------------------------------------
class JoinFestaBeaconInviteSequence :
  public gflnet2::ServerClient::RequestSequenceBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaBeaconInviteSequence );
private:

  enum
  {
    DUMMY_WAIT = 30 * 2,//ダミーウェイト2秒
    MAX_CLIENT_NUM = 3,
    TIMEOUT_MSEC = 45 * 1000
  };

public:
  //------------------------------------------------------------------
  /**
  * @brief  コンストラクタ/デストラクタ
  */
  //------------------------------------------------------------------
  JoinFestaBeaconInviteSequence();
  virtual~JoinFestaBeaconInviteSequence();

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


  //------------------------------------------------------------------
  /**
  * @brief ビーコンを終了ステートにする
  */
  //------------------------------------------------------------------
  void UpdateBeaconStateExit( void );


private:

  JoinFestaBeaconResponseListener*           m_pListener;
  JoinFestaBeaconRequestClient::REQUEST_PARAM m_requestParam;
  NetAppLib::Util::NetAppTimeout            m_timeout;        //! タイムアウトクラス

};


GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTABEACONDELETEREQUESTSEQUENCE_H_INCLUDE
