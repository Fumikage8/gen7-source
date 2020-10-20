//==============================================================================
/**
 * @file	NijiNetworkErrorDetectDefault.cpp
 * @brief	Niji通信エラーリスナーの検知クラス（デフォルト）
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
#include "NijiNetworkErrorDetectDefault.h" // 非公開ヘッダ

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Error)

// コンストラクタ
 NijiNetworkErrorDetectDefault::NijiNetworkErrorDetectDefault( NetLib::Error::NijiNetworkErrorManager* errorManager ) :
 m_errorManager( errorManager )
{
}

// デストラクタ
NijiNetworkErrorDetectDefault::~NijiNetworkErrorDetectDefault()
{
}



//=============================================================================
/**
*  WifiDisconnectListenerのリスナー
*/
//=============================================================================
//------------------------------------------------------------------
/**
* @brief 正規手続きを踏まないWIFI切断の通知
*
* @note  スレッドセーフ関数です
*
* @param[in] errorCode 最後に通知されたネットワークエラーコード
*
* @note WifiSystem::Close()を経ないＡＰ切断が行われた際に通知される
*/
//------------------------------------------------------------------
void NijiNetworkErrorDetectDefault::OnWifiDisconnected( u32 errorCode )
{
  m_error.SetResultWifiDisconnect( errorCode );
  m_errorManager->SetNetworkError( m_error );
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
void NijiNetworkErrorDetectDefault::OnNetGameSessionLeft()
{
  m_error.SetResult( NijiNetworkError::ERROR_TYPE_SESSION_LEFT );
  m_errorManager->SetNetworkError( m_error );
}

//=============================================================================
/**
*  NexErrorListenerのリスナー
*/
//=============================================================================
//------------------------------------------------------------------
/**
* @brief NEX内部で確保されたヒープメモリが枯渇したことを通知
*/
//------------------------------------------------------------------
void NijiNetworkErrorDetectDefault::OnNexErrorOutOfMemory( void )
{
  m_error.SetResult( NijiNetworkError::ERROR_TYPE_MEMORY_FULL );
  m_errorManager->SetNetworkError( m_error );
}

//------------------------------------------------------------------
/**
* @brief NEX内部のエラー通知
*
* @param[in] result     エラー値
*/
//------------------------------------------------------------------
void NijiNetworkErrorDetectDefault::OnNexErrorHandling( const nn::nex::qResult& result )
{
  m_error.SetResultNetZFatal( result );
  m_errorManager->SetNetworkError( m_error );
}

//------------------------------------------------------------------
/**
* @brief ゲームサーバログイン後、ゲームサーバとの接続が切断されたことを通知
*/
//------------------------------------------------------------------
void NijiNetworkErrorDetectDefault::OnNexErrorGameServerShutdown( void )
{
  m_error.SetResult( NijiNetworkError::ERROR_TYPE_LOGINSERVER_DISCONNECT );
  m_errorManager->SetNetworkError( m_error );
}

//=============================================================================
/**
*  IrAdapterErrorListenerのリスナー
*/
//=============================================================================
//------------------------------------------------------------------
/**
* @brief 赤外線壊れている
*/
//------------------------------------------------------------------
void NijiNetworkErrorDetectDefault::OnIrAdapterErrorIrBroken()
{
  m_error.SetResult( NijiNetworkError::ERROR_TYPE_IR_BROKEN );
  m_errorManager->SetNetworkError( m_error );
}

//------------------------------------------------------------------
/**
* @brief 繋いだタイプが違う
*/
//------------------------------------------------------------------
void NijiNetworkErrorDetectDefault::OnIrAdapterErrorMissmatch()
{
  m_error.SetResult( NijiNetworkError::ERROR_TYPE_IR_MISSMATCH );
  m_errorManager->SetNetworkError( m_error );
}

//------------------------------------------------------------------
/**
* @brief IrAdapterの内部エラー
*/
//------------------------------------------------------------------
void NijiNetworkErrorDetectDefault::OnIrAdapterErrorHandling()
{
  m_error.SetResult( NijiNetworkError::ERROR_TYPE_SYSTEM );
  m_errorManager->SetNetworkError( m_error );
}


//=============================================================================
/**
*  P2pConnectionErrorListenerのリスナー
*/
//=============================================================================
//------------------------------------------------------------------
/**
* @brief システムエラー発生通知（ライブラリの失敗通知など）
*/
//------------------------------------------------------------------
void NijiNetworkErrorDetectDefault::OnP2pConnectionErrorSystem()
{
  m_error.SetResult( NijiNetworkError::ERROR_TYPE_SYSTEM );
  m_errorManager->SetNetworkError( m_error );
}

//------------------------------------------------------------------
/**
* @brief Nexエラー発生通知
*
* @param[in] errorCode  エラーアプレット用エラーコード
*/
//------------------------------------------------------------------
void NijiNetworkErrorDetectDefault::OnP2pConnectionErrorNex( u32 errorCode )
{
  m_error.SetResultErrorCode( errorCode );
  m_errorManager->SetNetworkError( m_error );
}

//------------------------------------------------------------------
/**
* @brief P2P切断通知
*/
//------------------------------------------------------------------
void NijiNetworkErrorDetectDefault::OnP2pConnectionErrorSessionLeft()
{
  m_error.SetResult( NijiNetworkError::ERROR_TYPE_SESSION_LEFT );
  m_errorManager->SetNetworkError( m_error );
}

//------------------------------------------------------------------
/**
* @brief マッチメイクエラー通知（独自ハンドリング版）
* 
* @note  エラーアプレットは表示しない。
*/
//------------------------------------------------------------------
void NijiNetworkErrorDetectDefault::OnP2pConnectionErrorMatchMakeFailed()
{
  m_error.SetResult( NijiNetworkError::ERROR_TYPE_SYSTEM );
  m_errorManager->SetNetworkError( m_error );
}

//------------------------------------------------------------------
/**
* @brief 内部で確保されたヒープメモリが枯渇したことを通知
*/
//------------------------------------------------------------------
void NijiNetworkErrorDetectDefault::OnP2pConnectionErrorOutOfMemory()
{
  m_error.SetResult( NijiNetworkError::ERROR_TYPE_MEMORY_FULL );
  m_errorManager->SetNetworkError( m_error );
}

//------------------------------------------------------------------
/**
* @brief タイムアウト発生通知
*/
//------------------------------------------------------------------
void NijiNetworkErrorDetectDefault::OnP2pConnectionErrorTimeout()
{
  m_error.SetResult( NijiNetworkError::ERROR_TYPE_TIMEOUT );
  m_errorManager->SetNetworkError( m_error );
}

//------------------------------------------------------------------
/**
  * @brief 拡張セーブのフェイタルエラー
  */
//------------------------------------------------------------------
void NijiNetworkErrorDetectDefault::OnExtSaveFatalError()
{
  m_error.SetFatalError();
  m_errorManager->SetNetworkError( m_error );
}

GFL_NAMESPACE_END(Error)
GFL_NAMESPACE_END(NetLib)

#endif // GF_PLATFORM_CTR
