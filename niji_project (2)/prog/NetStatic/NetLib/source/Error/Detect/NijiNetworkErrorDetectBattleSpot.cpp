//==============================================================================
/**
 * @file	NijiNetworkErrorDetectBattleSpot.cpp
 * @brief	Niji通信エラーリスナーの検知クラス（バトルスポット専用）
 * @author	ichiraku_katsuhiko
 * @date	2015/05/11 16:54:26
 */
// =============================================================================
#if defined(GF_PLATFORM_CTR)

// エラー情報クラス
#include "NetStatic/NetLib/include/Error/NijiNetworkError.h"
// エラーマネージャ
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

// mine
#include "NijiNetworkErrorDetectBattleSpot.h"

#include "NetStatic/NetEvent/include/BattleSpotEvent.h"

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Error)

// コンストラクタ
 NijiNetworkErrorDetectBattleSpot::NijiNetworkErrorDetectBattleSpot( NetLib::Error::NijiNetworkErrorManager* errorManager ) :
 NijiNetworkErrorDetectDefault( errorManager )
{
}

// デストラクタ
NijiNetworkErrorDetectBattleSpot::~NijiNetworkErrorDetectBattleSpot()
{
}

//=============================================================================
/**
*  NetGameErrorListenerのリスナー
*/
//=============================================================================
//------------------------------------------------------------------
/**
* @brief P2P切断通知
*/
//------------------------------------------------------------------
void NijiNetworkErrorDetectBattleSpot::OnNetGameSessionLeft()
{
  if( m_errorManager->IsP2pDisconnectSpanOn() == false )
  {
    NijiNetworkError error;
    error.SetResult( NijiNetworkError::ERROR_TYPE_SESSION_LEFT );
    error.SetNotError();
    m_errorManager->SetNetworkError( error );
  }
  else
  {
    NetEvent::BattleSpot::BattleSpotEvent::SetSessionLeftFlag( true );
  }
}

//=============================================================================
/**
*  NexErrorListenerのリスナー
*/
//=============================================================================
//------------------------------------------------------------------
/**
* @brief NEX内部のエラー通知
*
* @param[in] result     エラー値
*/
//------------------------------------------------------------------
void NijiNetworkErrorDetectBattleSpot::OnNexErrorHandling( const nn::nex::qResult& result )
{
  // @fix GFNMCat[5723]:バトルスポットのロイヤルでステーション不一致のエラーが発生すると、アプリ終了する為切断数があがる
  ICHI_PRINT("Spot OnNexErrorHandling \n");
  if( m_errorManager->IsP2pDisconnectSpanOn() == false )
  {
    NijiNetworkError error;
    error.SetResultNetZFatal( result );
    error.SetNotError();
    m_errorManager->SetNetworkError( error );
  }
  else
  {
    NetEvent::BattleSpot::BattleSpotEvent::SetSessionLeftFlag( true );
  }
}

//=============================================================================
/**
*  P2pConnectionErrorListenerのリスナー
*/
//=============================================================================
//------------------------------------------------------------------
/**
* @brief P2P切断通知
*/
//------------------------------------------------------------------
void NijiNetworkErrorDetectBattleSpot::OnP2pConnectionErrorSessionLeft()
{
  if( m_errorManager->IsP2pDisconnectSpanOn() == false )
  {
    NijiNetworkError error;
    error.SetResult( NijiNetworkError::ERROR_TYPE_SESSION_LEFT );
    error.SetNotError();
    m_errorManager->SetNetworkError( error );
  }
  else
  {
    NetEvent::BattleSpot::BattleSpotEvent::SetSessionLeftFlag( true );
  }
}

GFL_NAMESPACE_END(Error)
GFL_NAMESPACE_END(NetLib)

#endif // GF_PLATFORM_CTR
