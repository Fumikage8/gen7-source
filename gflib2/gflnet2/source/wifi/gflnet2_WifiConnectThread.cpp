#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_WifiConnectThread.cpp
 * @brief  Wifi接続・切断用用スレッド
 * @author h.suzuki
 */
//=============================================================================


#include "gflnet2_WifiConnectThread.h"


namespace gflnet2 {
namespace wifi {

//--------------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 *
 * @param sys         WifiSystemへのポインタ
 * @param heap        スレッドのスタック領域に使用するヒープ
 * @param stack_size  スレッドのスタック領域のサイズ
 * @param mode        動作モード
 * @param listener    チェック完了通知用リスナ
 */
//--------------------------------------------------------------------------------
WifiConnectThread::WifiConnectThread( WifiSystem* sys, gfl2::heap::HeapBase* heap, u32 stack_size, WifiConnectThread::EXEC_MODE mode, WifiConnectListener *listener ) 
 : m_execMode(mode), m_listener(listener), m_wifiSys(sys),m_pThread(NULL)
{
  m_criticalSection.Initialize();

  m_pThread = GFL_NEW(heap) gfl2::thread::Thread(this, heap, true, stack_size );
}


//--------------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//--------------------------------------------------------------------------------
WifiConnectThread::~WifiConnectThread()
{
  GFL_SAFE_DELETE( m_pThread );
  m_criticalSection.Finalize();
}


//--------------------------------------------------------------------------------
/**
 * @brief 開始
 */
//--------------------------------------------------------------------------------
void WifiConnectThread::Start()
{
  m_pThread->Start();
}


//--------------------------------------------------------------------------------
/**
 * @brief スレッドメイン処理
 */
//--------------------------------------------------------------------------------
void WifiConnectThread::Function( void )
{
  if( E_CONNECT == m_execMode )
  {
    ConnectWifi();
  }
  else
  {
    DisconnectWifi();
  }

  m_pThread->Kill();
}


//--------------------------------------------------------------------------------
/**
 * @brief Wifi接続処理
 */
//--------------------------------------------------------------------------------
void WifiConnectThread::ConnectWifi( void )
{
  nn::Result initResult = m_wifiSys->InitializeAC();

  if( initResult.IsFailure() )
  {
    ReportConnectingError( initResult );
    return;
  }

  nn::Result connectResult = m_wifiSys->Connect();

  if( connectResult.IsFailure() )
  {
    ReportConnectingError( connectResult );
    return;
  }

  ReportConnectSuccess();
}


//--------------------------------------------------------------------------------
/**
 * @brief Wifi切断処理
 */
//--------------------------------------------------------------------------------
void WifiConnectThread::DisconnectWifi( void )
{
  nn::Result closeResult = m_wifiSys->Close();

  if( closeResult.IsFailure() )
  {
    ReportDisconnectingError( closeResult );
    return;
  }

  nn::Result finResult = m_wifiSys->FinalizeAC();

  if( finResult.IsFailure() )
  {
    ReportDisconnectingError( finResult );
    return;
  }

  ReportDisconnectSuccess();
}


//--------------------------------------------------------------------------------
/**
 * @brief 通知リスナの解除
 */
//--------------------------------------------------------------------------------
void WifiConnectThread::RemoveListener( void )
{
  nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御
  m_listener = NULL;
}


//--------------------------------------------------------------------------------
/**
 * @brief 接続成功通知
 */
//--------------------------------------------------------------------------------
void WifiConnectThread::ReportConnectSuccess( void )
{
  nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

  if( m_listener != NULL ){
    m_listener->OnWifiConnectSuccess();
  }
}


//--------------------------------------------------------------------------------
/**
 * @brief 切断成功通知
 */
//--------------------------------------------------------------------------------
void WifiConnectThread::ReportDisconnectSuccess( void )
{
  nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

  if( m_listener != NULL ){
    m_listener->OnWifiDisconnectSuccess();
  }
}



//--------------------------------------------------------------------------------
/**
 * @brief Wifi接続エラー処理
 */
//--------------------------------------------------------------------------------
void WifiConnectThread::ReportConnectingError( const nn::Result& result )
{
  nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

  if( m_listener != NULL )
  {
    u32        errorCode;
    gflnet2::wifi::WifiSystem::GetLastErrorCode( &errorCode );

    if( result == nn::ac::ResultNotAgreeEula() ){
      errorCode = gflnet2::wifi::WifiSystem::EULA_DISPLAY_ERROR_CODE;
    }

    m_listener->OnWifiConnectFailed( result, errorCode );
  }
}


//--------------------------------------------------------------------------------
/**
 * @brief Wifi切断エラー処理
 */
//--------------------------------------------------------------------------------
void WifiConnectThread::ReportDisconnectingError( const nn::Result& result )
{
  nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

  if( m_listener != NULL )
  {
    u32        errorCode;
    gflnet2::wifi::WifiSystem::GetLastErrorCode( &errorCode );
    m_listener->OnWifiDisconnecFailed( result, errorCode );
  }
}


} // wifi
} // gflnet


#endif
