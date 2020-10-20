//=============================================================================
/**
 * @file   btl_SendDataContainerFactory.cpp
 * @date   2015/08/31 15:55:16
 * @author obata_toshihiro
 * @brief  SendDataContainer のファクトリ
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_SendDataContainerFactory.h"

#include "./btl_SendDataContainer.h"


GFL_NAMESPACE_BEGIN( btl )


SendDataContainer* SendDataContainerFactory::s_containerForServerSend = NULL;
SendDataContainer* SendDataContainerFactory::s_containerForClientSend[ BTL_CLIENT_NUM ] = { NULL, NULL, NULL, NULL };
SendDataContainer* SendDataContainerFactory::s_containerForClientRecieve[ BTL_CLIENT_NUM ] = { NULL, NULL, NULL, NULL };


//-----------------------------------------------------------------------------
/**
 * @brief SendDataContainer を生成する
 * @param heap  コンテナの生成に使用するヒープ
 */
//-----------------------------------------------------------------------------
void SendDataContainerFactory::CreateContainer( HeapHandle heap )
{
  s_containerForServerSend = GFL_NEW( heap ) SendDataContainer( heap, CONTAINER_BUFFER_NUM );

  for( u32 clientId=0; clientId<BTL_CLIENT_NUM; ++clientId )
  {
    s_containerForClientSend[ clientId ]    = GFL_NEW( heap ) SendDataContainer( heap, CONTAINER_BUFFER_NUM );
    s_containerForClientRecieve[ clientId ] = GFL_NEW( heap ) SendDataContainer( heap, CONTAINER_BUFFER_NUM );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief SendDataContainer を破棄する
 */
//-----------------------------------------------------------------------------
void SendDataContainerFactory::DeleteContainer( void )
{
  GFL_SAFE_DELETE( s_containerForServerSend );

  for( u32 clientId=0; clientId<BTL_CLIENT_NUM; ++clientId )
  {
    GFL_SAFE_DELETE( s_containerForClientSend[ clientId ] );
    GFL_SAFE_DELETE( s_containerForClientRecieve[ clientId ] );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief サーバーの送信データ用のコンテナを取得する
 */
//-----------------------------------------------------------------------------
SendDataContainer* SendDataContainerFactory::GetServerSendContainer( void )
{
  return s_containerForServerSend;
}

//-----------------------------------------------------------------------------
/**
 * @brief クライアントの送信データ用のコンテナを取得する
 * @param clientId  対象クライアントのID
 */
//-----------------------------------------------------------------------------
SendDataContainer* SendDataContainerFactory::GetClientSendContainer( BTL_CLIENT_ID clientId )
{
  if( BTL_CLIENT_NUM <= clientId ) {
    GFL_ASSERT_STOP(0);
  }

  return s_containerForClientSend[ clientId ];
}

//-----------------------------------------------------------------------------
/**
 * @brief クライアントの受信データ用のコンテナを取得する
 * @param clientId  対象クライアントのID
 */
//-----------------------------------------------------------------------------
SendDataContainer* SendDataContainerFactory::GetClientReceiveContainer( BTL_CLIENT_ID clientId )
{
  if( BTL_CLIENT_NUM <= clientId ) {
    GFL_ASSERT_STOP(0);
  }

  return s_containerForClientRecieve[ clientId ];
}



GFL_NAMESPACE_END( btl )