#ifdef  GF_PLATFORM_CTR

//=============================================================================
/**
 * @file   gflnet2_CTR_UDPPacketIO.cpp
 * @brief  CTRのUDP送受信クラス
 * @author shin kosaka
 * @date   2014.9.29
 */
//=============================================================================
#include <gflnet2/source/Session/CTR/gflnet2_CTR_UDPPacketIO.h>
#include <gflnet2/include/p2p/gflnet2_RecvBuffer.h>

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
CTR_UDPPacketIO::CTR_UDPPacketIO( gfl2::heap::HeapBase* pHeap )
: SessionPacketIOBase(pHeap)
, m_isInitialized(false)
{
  m_pNetGame = GFL_NEW( m_pHeap ) gflnet2::p2p::NetGame();
  m_pNetGame->Initialize();
}

//------------------------------------------------------------------
/**
* @brief   デストラクタ
*/
//------------------------------------------------------------------
CTR_UDPPacketIO::~CTR_UDPPacketIO()
{
  GFL_SAFE_DELETE( m_pNetGame );
}

//------------------------------------------------------------------
/**
* @brief  更新処理。
*
* @note   	メインループ毎にポーリングすること
*/
//------------------------------------------------------------------
void CTR_UDPPacketIO::Update( void )
{
  if( m_isInitialized == false )
  {
    if( m_pNetGame )
    {
      if( m_pNetGame->IsInitialize() )
      {
        m_isInitialized = true;
      }
    }
  }
}

//------------------------------------------------------------------
/**
* @brief  メッセージ送信
* @param[in] messageID メッセージID
* @param[in] pInput  メッセージ本体
* @param[in] bufferSize メッセージサイズ
*/
//------------------------------------------------------------------
bool CTR_UDPPacketIO::InputPacket( u32 messageID , const u8* pInput , u16 bufferSize )
{
  if( m_isInitialized == false || m_pNetGame == NULL )
  {
    return false;
  }
  
  return m_pNetGame->SendDataCommand( (char*)pInput , bufferSize , messageID );
}

//------------------------------------------------------------------
/**
* @brief  メッセージ受信
* @param[in] pOutput  メッセージ本体
* @param[in] bufferSize メッセージサイズ
* @retval メッセージID
*/
//------------------------------------------------------------------
u32  CTR_UDPPacketIO::OutputPacket( u8* pOutput , u16 bufferSize )
{
  if( m_isInitialized == false )
  {
    return INVALID_MESSAGE_ID;
  }

  if( m_pNetGame )
  {
    u16 recvSize = 0;
    int recvTargetID = 0;
    if( m_pNetGame->RecvDataCommandLocalCommand( &recvTargetID , (char*)pOutput , bufferSize ,SESSION_UDP_MESSAGE_ID, &recvSize) )
    {//データあり
      return SESSION_UDP_MESSAGE_ID;
    }
    else
    {//なし
      return INVALID_MESSAGE_ID;
    }
  }

  return INVALID_MESSAGE_ID;
}

GFL_NAMESPACE_END(Session)
GFL_NAMESPACE_END(gflnet2)

#endif