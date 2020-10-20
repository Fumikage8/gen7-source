#ifdef  GF_PLATFORM_CTR

//=============================================================================
/**
 * @file   gflnet2_CTR_TCPPacketIO.cpp
 * @brief  CTRのTCP送受信クラス
 * @author shin kosaka
 * @date   2014.9.29
 */
//=============================================================================
#include <gflnet2/source/Session/CTR/gflnet2_CTR_TCPPacketIO.h>
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
CTR_TCPPacketIO::CTR_TCPPacketIO( gfl2::heap::HeapBase* pHeap )
: SessionPacketIOBase(pHeap)
, m_isInitialized(false)
{
  m_pNetGame = GFL_NEW( m_pHeap ) gflnet2::p2p::NetGame();
}

//------------------------------------------------------------------
/**
* @brief   デストラクタ
*/
//------------------------------------------------------------------
CTR_TCPPacketIO::~CTR_TCPPacketIO()
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
void CTR_TCPPacketIO::Update( void )
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
bool CTR_TCPPacketIO::InputPacket( u32 messageID , const u8* pInput , u16 bufferSize )
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
u32  CTR_TCPPacketIO::OutputPacket( u8* pOutput , u16 bufferSize )
{
  if( m_isInitialized == false )
  {
    return false;
  }

  if( m_pNetGame )
  {
    u16 recvSize = 0;
    int recvTargetID = 0;
    u8 recvCommandID = 0;
    if( m_pNetGame->RecvDataCommand( &recvTargetID , (char*)pOutput,bufferSize ,&recvCommandID, &recvSize) )
    {//データあり
      return recvCommandID;
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