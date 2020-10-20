//=============================================================================
/**
 * @file   btl_SendDataContainerFactory.h
 * @date   2015/08/31 15:55:16
 * @author obata_toshihiro
 * @brief  SendDataContainer のファクトリ
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_SENDDATACONTAINERFACTORY_H_INCLUDED
#define BTL_SENDDATACONTAINERFACTORY_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "./btl_common.h"

GFL_NAMESPACE_BEGIN( btl )

  class SendDataContainer;


  /**
   * @class btl_SendDataContainerFactory
   * @brief SendDataContainer の生成・破棄・インスタンスの管理を行う
   */
  class SendDataContainerFactory
  {
    GFL_FORBID_COPY_AND_ASSIGN( SendDataContainerFactory );

  public:

    //-------------------------------------------------------------------------
    /**
     * @brief SendDataContainer を生成する
     * @param heap  コンテナの生成に使用するヒープ
     */
    //-------------------------------------------------------------------------
    static void CreateContainer( HeapHandle heap );

    //-------------------------------------------------------------------------
    /**
     * @brief SendDataContainer を破棄する
     */
    //-------------------------------------------------------------------------
    static void DeleteContainer( void );

    //-------------------------------------------------------------------------
    /**
     * @brief サーバーの送信データ用のコンテナを取得する
     */
    //-------------------------------------------------------------------------
    static SendDataContainer* GetServerSendContainer( void );

    //-------------------------------------------------------------------------
    /**
     * @brief クライアントの送信データ用のコンテナを取得する
     * @param clientId  対象クライアントのID
     */
    //-------------------------------------------------------------------------
    static SendDataContainer* GetClientSendContainer( BTL_CLIENT_ID clientId );

    //-------------------------------------------------------------------------
    /**
     * @brief クライアントの受信データ用のコンテナを取得する
     * @param clientId  対象クライアントのID
     */
    //-------------------------------------------------------------------------
    static SendDataContainer* GetClientReceiveContainer( BTL_CLIENT_ID clientId );



  private:

    // コンテナごとのバッファ数
    //
    // なんで "2" なのか？
    //
    // 記録しておいた送受信データは、バトルロイヤルにおいてサーバー機が居なくなった際に、
    // 「新サーバーが処理を開始すべきシーケンス番号」を特定するために使用されます。
    //
    // 新サーバーが立ち上がった時点で、各クライアントとサーバーのやり取りの進捗にズレが生じることがあり、
    // その場合、新サーバーは、最も進捗が遅いクライアントに合わせて処理を開始する必要があります。
    //
    // しかし、最も進捗が遅いクライアントに合わせて処理を開始すると、進捗が進んでいたクライアントは、前回と同じコマンドを受信することになります。
    // そこで、クライアントは、すでに返信したコマンドに対しては、過去に返信したデータをそのまま返します。
    //
    // このように、サーバーが居なくなり、クライアント間でサーバーとのやり取りにズレが生じた場合に
    // サーバーの処理を巻き戻すために、過去の送受信データが使用されます。
    //
    // 「全クライアントからの返信を受け付けたら、次のシーケンスに進む」サーバーの性質上、
    // 最大ズレ幅は "1" になります。
    // つまり、
    // ・最後に送信( 受信 )したデータ
    // ・１つ前に送信( 受信 )したデータ
    // の２つが記録できれば、新サーバーは１つ前のシーケンスから処理が開始できます。
    static const u32 CONTAINER_BUFFER_NUM = 2;

    static SendDataContainer* s_containerForServerSend;
    static SendDataContainer* s_containerForClientSend[ BTL_CLIENT_NUM ];
    static SendDataContainer* s_containerForClientRecieve[ BTL_CLIENT_NUM ];

  };


GFL_NAMESPACE_END( btl )

#endif // BTL_SENDDATACONTAINERFACTORY_H_INCLUDED