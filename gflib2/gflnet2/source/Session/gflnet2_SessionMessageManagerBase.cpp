//=============================================================================
/**
 * @file   gflnet2_SessionMessageManagerBase.cpp
 * @brief  メッセージ管理基底クラス
 * @author shin kosaka
 * @date   2014.9.29
 */
//=============================================================================
#include <gflnet2/include/Session/gflnet2_SessionMessageManagerBase.h>
#include <gflnet2/include/Session/gflnet2_SessionPacketManager.h>

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <thread/include/gfl2_ThreadStatic.h>


GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(Session)

//------------------------------------------------------------------
/**
* @brief   コンストラクタ
*/
//------------------------------------------------------------------
SessionMessageManagerBase::SessionMessageManagerBase( u32 sessionID , SessionPacketManager* pPakectManager )
: m_sessionID( sessionID )
, m_pListener( NULL ) 
, m_pPacketManager( pPakectManager )
, m_targeMessageIDArray()
{
  GFL_ASSERT_MSG(pPakectManager,"m_pPacketManager is NULL");
}

//------------------------------------------------------------------
/**
* @brief   デストラクタ
*/
//------------------------------------------------------------------
SessionMessageManagerBase::~SessionMessageManagerBase()
{
  Finalize();
}

//------------------------------------------------------------------
/**
* @brief   初期化処理
* @param[in] pHeap ヒープ
*/
//------------------------------------------------------------------
void SessionMessageManagerBase::Initialize( gfl2::heap::HeapBase* pHeap )
{

}

//------------------------------------------------------------------
/**
* @brief   終了処理
*/
//------------------------------------------------------------------
void SessionMessageManagerBase::Finalize( void )
{
  m_pListener = NULL;
  m_pPacketManager = NULL;
  m_targeMessageIDArray.clear();
}

//------------------------------------------------------------------
/**
* @brief   管理するメッセージIDを追加
*/
//------------------------------------------------------------------
void SessionMessageManagerBase::AddTargetMessageID( u16 messageID )
{
  m_targeMessageIDArray.push_back( messageID );
}



//------------------------------------------------------------------
/**
* @brief   リスナー設定
* @param[in] pListener
*/
//------------------------------------------------------------------
void SessionMessageManagerBase::SetListener( SessionMessageListener* pListener )
{
  m_pListener = pListener;
}

//------------------------------------------------------------------
/**
* @brief   リスナー削除
*/
//------------------------------------------------------------------
void SessionMessageManagerBase::RemoveListener( void )
{
  m_pListener = NULL;
}

//------------------------------------------------------------------
/**
* @brief  UDPメッセージ受信
* @param[in] messageID メッセージID
* @param[in] pMessage  メッセージ本体
* @param[in] messageSize メッセージサイズ
*/
//------------------------------------------------------------------
bool SessionMessageManagerBase::SetRecvUDPMessage( u32 messageID , const u8* pMessage , u16 messageSize )
{
  if( IsMyTargetMessageID( messageID ) == false )
  {
    return false;
  }

  if( m_pListener )
  {
    m_pListener->OnRecvUDPMessage( messageID , pMessage , messageSize );
  }

  return true;
}

//------------------------------------------------------------------
/**
* @brief  TCPメッセージ受信
* @param[in] messageID メッセージID
* @param[in] pMessage  メッセージ本体
* @param[in] messageSize メッセージサイズ
*/
//------------------------------------------------------------------
bool SessionMessageManagerBase::SetRecvTCPMessage( u32 messageID , const u8* pMessage , u16 messageSize )
{
  if( IsMyTargetMessageID( messageID ) == false )
  {
    return false;
  }

  if( m_pListener )
  {
    m_pListener->OnRecvTCPMessage( messageID , pMessage , messageSize );
  }

  return true;
}

//------------------------------------------------------------------
/**
* @brief   自分が管理するメッセージIDかどうか判定
* @param[in] messageID
*/
//------------------------------------------------------------------
bool SessionMessageManagerBase::IsMyTargetMessageID( u16 messageID )
{
  for( u32 index = 0 ; index < m_targeMessageIDArray.size() ; ++index )
  {
    if( m_targeMessageIDArray[index] == messageID )
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------
/**
* @brief  TCPメッセージ送信
* @param[in] messageID メッセージID
* @param[in] pMessage  メッセージ本体
* @param[in] messageSize メッセージサイズ
*/
//------------------------------------------------------------------
bool SessionMessageManagerBase::SendTCPMessageInternal( u32 messageID , const u8* pMessage , u16 messageSize )
{
  if( IsMyTargetMessageID( messageID ) == false )
  {
    GFL_ASSERT_MSG(0,"is not my target messageID");
    return false;
  }

  if( m_pPacketManager )
  {
    return m_pPacketManager->SendTCPMessage( messageID , pMessage , messageSize );
  }
  else
  {
    return false;
  }
}

//------------------------------------------------------------------
/**
* @brief  UDPメッセージ送信
* @param[in] messageID メッセージID
* @param[in] pMessage  メッセージ本体
* @param[in] messageSize メッセージサイズ
*/
//------------------------------------------------------------------
bool SessionMessageManagerBase::SendUDPMessageInternal( u32 messageID , const u8* pMessage , u16 messageSize )
{
  if( IsMyTargetMessageID( messageID ) == false )
  {
    GFL_ASSERT_MSG(0,"is not my target messageID");
    return false;
  }

  if( m_pPacketManager )
  {
    return m_pPacketManager->SendUDPMessage( messageID , pMessage , messageSize );
  }
  else
  {
    return false;
  }
}


GFL_NAMESPACE_END(Session)
GFL_NAMESPACE_END(gflnet2)

