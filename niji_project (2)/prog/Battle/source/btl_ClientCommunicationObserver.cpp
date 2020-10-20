//=============================================================================
/**
 * @file   btl_ClientCommunicationObserver.cpp
 * @date   2015/08/19 12:07:08
 * @author obata_toshihiro
 * @brief  リモートクライアントとの通信接続を監視する
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_ClientCommunicationObserver.h"
#include "./btl_net.h"

GFL_NAMESPACE_BEGIN( btl )



//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
ClientCommunicationObserver::ClientCommunicationObserver( void )
{
  for( u32 clientId=0; clientId<BTL_CLIENT_NUM; ++clientId )
  {
    m_isClientCommunicationExist[ clientId ] = false;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
ClientCommunicationObserver::~ClientCommunicationObserver()
{
}

//-----------------------------------------------------------------------------
/**
 * @brief 全クライアントとの通信接続状況を監視する
 */
//-----------------------------------------------------------------------------
void ClientCommunicationObserver::ObserveClinetCommunication( void )
{
  for( u32 clientId=0; clientId<BTL_CLIENT_NUM; ++clientId )
  {
    m_isClientCommunicationExist[ clientId ] = net::CheckConnectedClient( clientId );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief 指定したクライアントとの通信が接続されているか？
 * @param clientId  チェック対象クライアントのID
 * @retval true   接続されている
 * @retval false  接続されていない
 */
//-----------------------------------------------------------------------------
bool ClientCommunicationObserver::IsClientCommunicationExist( BTL_CLIENT_ID clientId ) const
{
  if( BTL_CLIENT_NUM <= clientId )
  {
    GFL_ASSERT(0);
    return false;
  }
  return m_isClientCommunicationExist[ clientId ];
}



GFL_NAMESPACE_END( btl )