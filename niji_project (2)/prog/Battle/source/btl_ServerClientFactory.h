//=============================================================================
/**
 * @file   btl_ServerClientFactory.h
 * @date   2015/07/27 12:25:43
 * @author obata_toshihiro
 * @brief  サーバー・クライアント生成
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_SERVERCLIENTFACTORY_H_INCLUDED
#define BTL_SERVERCLIENTFACTORY_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <Battle/include/battle_SetupParam.h>
#include "./btl_common.h"


GFL_NAMESPACE_BEGIN( btl )

  class MainModule;
  class BTL_SERVER;
  class BTL_CLIENT;
  class POKECON;
  class BtlSound;
  class Adapter;
  class SendDataContainer;
  class ZenryokuWazaStatus;


  /**
   * @class ServerClientFactory
   * @brief クライアント、サーバーを生成する
   */
  class ServerClientFactory
  {
    GFL_FORBID_COPY_AND_ASSIGN( ServerClientFactory );

  public:

    /**
     * @brief 生成に必要なパラメータ
     */
    struct Input
    {
      gfl2::heap::HeapBase* heap;                           // 生成に使用するヒープ
      BtlRule               rule;                           // バトルのルール
      BtlCommMode           commMode;                       // 通信モード
      BtlMultiMode          multiMode;                      // マルチモード
      BtlBagMode            bagMode;                        // バッグモード
      bool                  isRecordPlayMode;               // 録画再生モードか？
      BTL_CLIENT_ID         myClientId;                     // 自分のクライアントID
      bool                  amIServer;                      // 自分がサーバーか？
      bool                  haveZenryokuRing;               // 自分が全力リングを持っているか？
      u32                   randomSeed;                     // 乱数の種
      MainModule*           mainModule;                     // メインモジュール
      BtlSound*             soundSystem;                    // サウンドシステム
      POKECON*              pokeconForServer;               // ポケモンコンテナ( サーバー用 )
      POKECON*              pokeconForClient;               // ポケモンコンテナ( クライアント用 )
      SendDataContainer*    sendDataContainerForServerSend;                      // サーバー送信データ用コンテナ
      SendDataContainer*    sendDataContainerForClientSend[ BTL_CLIENT_NUM ];    // クライアント送信データ用コンテナ
      SendDataContainer*    sendDataContainerForClientReceive[ BTL_CLIENT_NUM ]; // クライアント受信データ用コンテナ
      u32                   clientLimitTime;
    };


    /**
     * @brief 生成結果
     */
    struct Output
    {
      BTL_SERVER*         server;
      BTL_CLIENT*         localClients[ BTL_CLIENT_NUM ];             // ローカルクライアント
      BTL_CLIENT*         dummyClients[ BTL_CLIENT_NUM ];             // ダミークライアント
      ZenryokuWazaStatus* clientZenryokuWazaStatus[ BTL_CLIENT_NUM ]; // 各クライアントの全力技ステータス管理オブジェクト( ローカル・ダミー間で共用 )
    };


    //-------------------------------------------------------------------------
    /**
     * @brief クライアント・サーバーを生成する
     * @param      input   生成に必要なパラメータ
     * @param[out] output  生成結果の格納先
     */
    //-------------------------------------------------------------------------
    static void CreateServerClient( const Input& input, Output* output );




  private:

    static void InitOutput( Output* output );
    static void CreateClientZenryokuWazaStatus( const Input& input, Output* output );
    static ZenryokuWazaStatus* CreateClientZenryokuWazaStatus( const Input& input, BTL_CLIENT_ID clientId );
    static bool HaveClientZenryokuRing( const Input& input, BTL_CLIENT_ID clientID );
    static void CreateLocalClients( const Input& input, Output* output );
    static void CreateDummyClients( const Input& input, Output* output );
    static BTL_CLIENT* CreateLocalClient( const Input& input, BTL_SERVER* server, BTL_CLIENT_ID clientId, ZenryokuWazaStatus* zenryokuWazaStatus );
    static BTL_CLIENT* CreateDummyClient( const Input& input, BTL_SERVER* server, BTL_CLIENT_ID clientId, const BTL_CLIENT* localClient, ZenryokuWazaStatus* zenryokuWazaStatus );
    static BTL_CLIENT* CreateClientObject( const Input& input, BTL_SERVER* server, ZenryokuWazaStatus* zenryokuWazaStatus, BTL_CLIENT_ID clientId, BtlClientType clientType );
    static void CreateRemoteClients( const Input& input, Output* output ); 
    static Adapter* CreateClientAdapter( const Input& input, BTL_CLIENT_ID clientId, BtlClientType clientType );
    static void SetupCommandCheckServer( const Input& input, Output* output );

  };


GFL_NAMESPACE_END( btl )

#endif // BTL_SERVERCLIENTFACTORY_H_INCLUDED