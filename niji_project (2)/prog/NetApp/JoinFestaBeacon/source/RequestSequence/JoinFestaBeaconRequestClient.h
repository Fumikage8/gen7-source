// ============================================================================
/*
 * @file JoinFestaBeaconRequestClient.cpp
 * @brief JoinFestaBeaconアプリで必要な通信リクエストを投げる為のクラスです。
 * @date 2015.06.23
 */
// ============================================================================
#if !defined( JOINFESTABEACONREQUESTCLIENT_H_INCLUDE )
#define JOINFESTABEACONREQUESTCLIENT_H_INCLUDE
#pragma once

#include "NetApp/JoinFestaBeacon/source/RequestSequence/JoinFestaBeaconResponseListener.h"
#include "NetApp/JoinFestaBeacon/source/System/JoinFestaBeaconWork.h"
#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)





//--------------------------------------------------------------------------------------------
/**
  * @brief   チーム選択アプリ用のリクエストクライアント
  */
//--------------------------------------------------------------------------------------------
class JoinFestaBeaconRequestClient
{
public:

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   リクエストパラメータ群
   */
  //--------------------------------------------------------------------------------------------
  struct REQUEST_PARAM
  {
    NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork* m_pJoinFestaBeaconWork; //ワークごと渡してしまう
  };


public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ/デストラクタ
   */
  //--------------------------------------------------------------------------------------------
  JoinFestaBeaconRequestClient( JoinFestaBeaconResponseListener* pListener );
  virtual ~JoinFestaBeaconRequestClient();

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   接続申し込みリクエスト
   * @param   requestParam リクエストパラメータ
   */
  //--------------------------------------------------------------------------------------------
  void InviteRequest( REQUEST_PARAM& requestParam );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   参加リクエスト
   * @param   requestParam リクエストパラメータ
   */
  //--------------------------------------------------------------------------------------------
  void JoinRequest( REQUEST_PARAM& requestParam );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   募集リクエスト
   * @param   requestParam リクエストパラメータ
   */
  //--------------------------------------------------------------------------------------------
  void RecruitRequest( REQUEST_PARAM& requestParam );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   接続中止リクエスト
   * @param   requestParam リクエストパラメータ
   */
  //--------------------------------------------------------------------------------------------
  void CancelConnectRequest( void );



private:

  gflnet2::ServerClient::RequestHandle m_handle;//リクエストハンドル
  JoinFestaBeaconResponseListener* m_pListener;
};



GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTABEACONREQUESTCLIENT_H_INCLUDE
