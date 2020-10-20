//=============================================================================
/**
 * @file   btl_ServerClientFactory.cpp
 * @date   2015/07/27 12:25:43
 * @author obata_toshihiro
 * @brief  サーバー・クライアント生成
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_BattleRule.h"
#include "./btl_PokemonPosition.h"
#include "./btl_server.h"
#include "./btl_client.h"
#include "./btl_adapter.h"
#include "./btl_ZenryokuWazaStatus.h"
#include "./btl_ServerClientFactory.h"


GFL_NAMESPACE_BEGIN( btl )


  //---------------------------------------------------------------------------
  /**
   * @brief クライアント・サーバーを生成する
   * @param      input   生成に必要なパラメータ
   * @param[out] output  生成結果の格納先
   */
  //---------------------------------------------------------------------------
  void ServerClientFactory::CreateServerClient( const Input& input, Output* output )
  {
    ServerClientFactory::InitOutput( output );

    // サーバーを生成
    output->server = GFL_NEW(input.heap) BTL_SERVER( input.mainModule, input.randomSeed, input.pokeconForServer, input.sendDataContainerForServerSend, input.heap, input.clientLimitTime );

    // 全力技ステータス管理オブジェクトを生成
    ServerClientFactory::CreateClientZenryokuWazaStatus( input, output );

    // ローカルクライアントを生成
    ServerClientFactory::CreateLocalClients( input, output );

    // リモートクライアントを生成
    ServerClientFactory::CreateRemoteClients( input, output );

    // ダミークライアントを生成
    if( BattleRule::IsDummyClientSwitchEnable( input.rule ) ) 
    {
      ServerClientFactory::CreateDummyClients( input, output );
    }

    // 自分がサーバーでないなら、生成したサーバーを、コマンド整合性チェックサーバーとしてセットアップ
    if( !input.amIServer )
    {
      ServerClientFactory::SetupCommandCheckServer( input, output );
    }
  }

  /**
   * @brief 出力結果を初期化する
   * @param output  初期化対象のインスタンス
   */
  void ServerClientFactory::InitOutput( ServerClientFactory::Output* output )
  {
    output->server = NULL;
    for( u32 i=0; i<BTL_CLIENT_NUM; ++i )
    {
      output->localClients[i] = NULL;
      output->dummyClients[i] = NULL;
      output->clientZenryokuWazaStatus[i] = NULL;
    }
  }

  /**
   * @brief クライアントの全力技ステータス管理オブジェクトを生成する
   * @param      input   生成に必要なパラメータ
   * @param[out] output  生成するクライアントの格納先
   */
  void ServerClientFactory::CreateClientZenryokuWazaStatus( const Input& input, Output* output )
  {
    for( u32 i=0; i<BTL_CLIENT_NUM; ++i )
    {
      BTL_CLIENT_ID clientId = static_cast<BTL_CLIENT_ID>( i );
      output->clientZenryokuWazaStatus[ clientId ] = ServerClientFactory::CreateClientZenryokuWazaStatus( input, clientId );
    }
  }

  /**
   * @brief クライアントの全力技ステータス管理オブジェクトを生成する
   * @param input     生成に必要なパラメータ
   * @param clientId  生成するクライアントのID
   * @return 生成したオブジェクト
   */
  ZenryokuWazaStatus* ServerClientFactory::CreateClientZenryokuWazaStatus( const Input& input, BTL_CLIENT_ID clientId )
  {
    ZenryokuWazaStatus::SetupParam setupParam;
    setupParam.haveZenryokuRing   = ServerClientFactory::HaveClientZenryokuRing( input, clientId );
    return GFL_NEW( input.heap ) ZenryokuWazaStatus( setupParam );
  }

  /**
   * @brief 指定したクライアントが、「ぜんりょくリング」を所持しているか？
   * @param input     生成に必要なパラメータ
   * @param clientId  チェック対象クライアントのID
   * @retval true   所持している
   * @retval false  所持していない
   */
  bool ServerClientFactory::HaveClientZenryokuRing( const Input& input, BTL_CLIENT_ID clientId )
  {
    // 通信対戦なら、全員持っている
    if( input.commMode != BTL_COMM_NONE )
    {
      return true;
    }

    if( clientId == input.myClientId )
    {
      return input.haveZenryokuRing;
    }
  
    // NPC は「ぜんりょくリング」を持っていることにする。
    // ポケモンが「ぜんりょくピース」を持ってさえいれば、ぜんりょくパワーを使用可能。
    return true;
  }

  /**
   * @brief 全ローカルクライアントを生成する
   * @param      input   生成に必要なパラメータ
   * @param[out] output  生成するクライアントの格納先
   */
  void ServerClientFactory::CreateLocalClients(
    const ServerClientFactory::Input&  input,
    ServerClientFactory::Output*       output )
  {
    for( u32 i=0; i<BTL_CLIENT_NUM; ++i )
    {
      BTL_CLIENT_ID clientId = static_cast<BTL_CLIENT_ID>( i );
      BTL_CLIENT* client = ServerClientFactory::CreateLocalClient( input, output->server, clientId, output->clientZenryokuWazaStatus[ clientId ] );
      if( client != NULL ) 
      {
        output->localClients[ clientId ] = client;
        output->server->AttachClient( clientId, client->GetAdapter() );
      }
    }
  }

  /**
   * @brief ローカルクライアントを生成する
   * @param input               生成に必要なパラメータ
   * @param server              生成したクライアントの登録先サーバー
   * @param clientId            生成するクライアントのID
   * @param zenryokuWazaStatus  生成するクライアントにアタッチする全力技ステータス管理オブジェクト
   * @return 生成したクライアント
   * @retval NULL  クライアントが存在しない場合
   */
  BTL_CLIENT* ServerClientFactory::CreateLocalClient( 
    const ServerClientFactory::Input& input, 
    BTL_SERVER*                       server, 
    BTL_CLIENT_ID                     clientId,
    ZenryokuWazaStatus*               zenryokuWazaStatus )
  {
    // 対象クライアントが存在しない
    if( !( BattleRule::IsClientExist( input.rule, input.multiMode, clientId ) ) )
    {
      return NULL;
    }

    // 録画再生なら、必ず生成する
    if( input.isRecordPlayMode )
    {
      return ServerClientFactory::CreateClientObject( input, server, zenryokuWazaStatus, clientId, BTL_CLIENT_TYPE_RECPLAY );
    }

    // 対象クライアントは自分自身
    if( clientId == input.myClientId )
    {
      return ServerClientFactory::CreateClientObject( input, server, zenryokuWazaStatus, clientId, BTL_CLIENT_TYPE_UI );
    }

    // 対象クライアントがAIなら、自分がサーバーの場合のみ生成
    if( ( input.amIServer ) &&
        ( BattleRule::IsClientAi( input.rule, input.commMode, input.multiMode, clientId ) ) )
    {
      return ServerClientFactory::CreateClientObject( input, server, zenryokuWazaStatus, clientId, BTL_CLIENT_TYPE_AI );
    }

    return NULL;
  }

  /**
   * @brief クライアントオブジェクトを生成する
   * @param input               生成に必要なパラメータ
   * @param server              生成したクライアントの登録先サーバー
   * @param zenryokuWazaStatus  生成するクライアントにアタッチする全力技ステータス管理オブジェクト
   * @param clientId            生成するクライアントのID
   * @param clientType          生成するクライアントのタイプ
   */
  BTL_CLIENT* ServerClientFactory::CreateClientObject( 
    const ServerClientFactory::Input& input,
    BTL_SERVER*                       server,
    ZenryokuWazaStatus*               zenryokuWazaStatus,
    BTL_CLIENT_ID                     clientId,
    BtlClientType                     clientType )
  {
    Adapter* adapter = ServerClientFactory::CreateClientAdapter( input, clientId, clientType );

    BTL_CLIENT* client = GFL_NEW( input.heap ) BTL_CLIENT( 
      input.mainModule, 
      server->GetFlowWork(), 
      input.soundSystem, 
      input.pokeconForClient, 
      adapter,
      input.sendDataContainerForClientSend[ clientId ],
      input.sendDataContainerForClientReceive[ clientId ],
      zenryokuWazaStatus,
      input.commMode, 
      clientId, 
      clientType, 
      input.bagMode, 
      false, 
      input.randomSeed,
      input.heap );

    return client;
  }

  /**
   * @brief クライアントに繋ぐアダプタを生成する
   * @param input       生成に必要なパラメータ
   * @param clientId    生成するクライアントのID
   * @param clientType  生成するクライアントのタイプ
   */
  Adapter* ServerClientFactory::CreateClientAdapter( 
    const Input&  input, 
    BTL_CLIENT_ID clientId, 
    BtlClientType clientType )
  {
    const bool fComm = ( ( input.commMode != BTL_COMM_NONE ) && ( clientType == BTL_CLIENT_TYPE_UI ) );

    AdapterFactory* adapterFactory = AdapterFactory::GetInstance();
    Adapter* adapter = adapterFactory->Create( fComm, clientId, input.heap );
    return adapter;
  }

  /**
   * @brief 全ダミークライアントを生成する
   * @param      input   生成に必要なパラメータ
   * @param[out] output  生成するクライアントの格納先
   */
  void ServerClientFactory::CreateDummyClients(
    const ServerClientFactory::Input&  input,
    ServerClientFactory::Output*       output )
  {
    for( u32 i=0; i<BTL_CLIENT_NUM; ++i )
    {
      const BTL_CLIENT_ID clientId = static_cast<BTL_CLIENT_ID>( i );
      const BTL_CLIENT* localClient = output->localClients[ clientId ];
      ZenryokuWazaStatus* zenryokuWazaStatus = output->clientZenryokuWazaStatus[ clientId ];
      BTL_CLIENT* client = ServerClientFactory::CreateDummyClient( input, output->server, clientId, localClient, zenryokuWazaStatus );
      if( client != NULL ) 
      {
        output->dummyClients[ clientId ] = client;
        BTL_PRINT( "[ServerClientFactory] ダミークライアントを作成しました( clientId=%d )\n", clientId );
      }
    }
  }

  /**
   * @brief ダミークライアントを生成する
   * @param input               生成に必要なパラメータ
   * @param server              生成したクライアントの登録先サーバー
   * @param clientId            生成するクライアントのID
   * @param localClient         対象クライアントのローカルクライアントオブジェクト
   * @param zenryokuWazaStatus  生成するクライアントにアタッチする全力技ステータス管理オブジェクト
   * @return 生成したダミークライアント
   * @retval NULL  ダミークライアントが存在しない場合
   */
  BTL_CLIENT* ServerClientFactory::CreateDummyClient( 
    const ServerClientFactory::Input& input,
    BTL_SERVER*                       server,
    BTL_CLIENT_ID                     clientId,
    const BTL_CLIENT*                 localClient,
    ZenryokuWazaStatus*               zenryokuWazaStatus )
  {
    // 対象クライアントが存在しないなら、ダミークライアントは不要
    if( !( BattleRule::IsClientExist( input.rule, input.multiMode, clientId ) ) )
    {
      return NULL;
    }

    // ローカルクライアントが存在するなら、ダミークライアントは不要
    if( localClient != NULL )
    {
      return NULL;
    }

    return ServerClientFactory::CreateClientObject( input, server, zenryokuWazaStatus, clientId, BTL_CLIENT_TYPE_AI );
  }

  /**
   * @brief 全リモートクライアントを生成する
   * @param      input   生成に必要なパラメータ
   * @param[out] output  生成結果の格納先
   */
  void ServerClientFactory::CreateRemoteClients(
    const ServerClientFactory::Input&  input,
    ServerClientFactory::Output*       output )
  {
    for( u32 i=0; i<BTL_CLIENT_NUM; ++i )
    {
      BTL_CLIENT_ID clientId = static_cast<BTL_CLIENT_ID>( i );

      if( ( output->localClients[ clientId ] == NULL ) &&
          ( BattleRule::IsClientExist( input.rule, input.multiMode, clientId ) ) )
      {
        Adapter* adapter = ServerClientFactory::CreateClientAdapter( input, clientId, BTL_CLIENT_TYPE_UI );
        output->server->AttachClient( clientId, adapter );
      }
    }
  }

  /**
   * @brief サーバーを、コマンドチェックサーバーとしてセットアップする
   * @param      input   生成に必要なパラメータ
   * @param[out] output  生成結果の格納先
   */
  void ServerClientFactory::SetupCommandCheckServer( const Input& input, Output* output )
  {
    output->localClients[ input.myClientId ]->AttachCmdCheckServer( output->server );
  }


GFL_NAMESPACE_END( btl )