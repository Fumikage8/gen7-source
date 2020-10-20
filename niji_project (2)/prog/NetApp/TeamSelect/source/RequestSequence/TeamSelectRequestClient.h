// ============================================================================
/*
 * @file TeamSelectRequestClient.cpp
 * @brief チーム選択アプリで必要な通信リクエストを投げる為のクラスです。
 * @date 2015.06.23
 */
// ============================================================================
#if !defined( TEAMSELECTREQUESTCLIENT_H_INCLUDE )
#define TEAMSELECTREQUESTCLIENT_H_INCLUDE
#pragma once

#include "NetApp/TeamSelect/source/RequestSequence/TeamSelectResponseListener.h"

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>
#include <Battle/Regulation/include/Regulation.h>
#include <heap/include/gfl2_Heap.h>
GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)





//--------------------------------------------------------------------------------------------
/**
  * @brief   チーム選択アプリ用のリクエストクライアント
  */
//--------------------------------------------------------------------------------------------
class TeamSelectRequestClient
{
public:

  //ポケパーティチェック用リクエストパラメータ
  struct CHECK_REQUEST_PARAM
  {
    pml::PokeParty* pPokeParty;//チェック対象のポケパーティ
    Regulation* pRegulation;//判定に使用するレギュレーション
    gfl2::heap::HeapBase* pHeap;
    bool isQRTeam;//QRチームかどうか
  };

public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ/デストラクタ
   */
  //--------------------------------------------------------------------------------------------
  TeamSelectRequestClient( gfl2::heap::HeapBase* pHeap );
  virtual ~TeamSelectRequestClient();

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   レギュレーションチェックと不正チェックをまとめて行う非同期処理の実行をリクエストする
   * @param   pListener 結果を受け取るリスナー
   * @param   requestParam リクエストパラメータ
   */
  //--------------------------------------------------------------------------------------------
  void CheckRegulationAndValidationRequest( TeamSelectResponseListener* pListener , CHECK_REQUEST_PARAM& requestParam );

private:

  gflnet2::ServerClient::RequestHandle m_handle;//リクエストハンドル
  gfl2::heap::HeapBase* m_pHeap;
};



GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)

#endif // TEAMSELECTREQUESTCLIENT_H_INCLUDE
