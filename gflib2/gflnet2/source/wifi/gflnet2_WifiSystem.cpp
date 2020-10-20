#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_WifiSystem.cpp
 * @brief  nn::acのラッパークラス。Wifiアクセスポイントを管理する
 * @author h.suzuki
 * @date   2011.10.20
 */
//=============================================================================
#include <gflnet2/include/wifi/gflnet2_WifiSystem.h>
#include "gflnet2_WifiConnectThread.h"
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
#if GFL_RO_ENABLE
SINGLETON_INSTANCE_DEFINITION(gflnet2::wifi::WifiSystem)
template class gfl2::base::SingletonAccessor<gflnet2::wifi::WifiSystem>;
#endif


namespace gflnet2{
namespace wifi{


  //------------------------------------------------------------------
  /**
   * @brief   WifiSystemコンストラクタ
   */
  //------------------------------------------------------------------
  WifiSystem::WifiSystem() 
  {
    m_heapBase    = NULL;
    m_thread      = NULL;
    m_isConnect   = false;
    m_disconnetListener = NULL;
    m_criticalSection.Initialize();
    m_event.Initialize( true );
  }


  //------------------------------------------------------------------
  /**
   * @brief   WifiSystemデストラクタ
   */
  //------------------------------------------------------------------
  WifiSystem::~WifiSystem()
  {
    FinalizeAC();
    m_event.Finalize();
    m_criticalSection.Finalize();
  }

  //------------------------------------------------------------------
  /**
   * @brief   初期化
   *
   * @param   heap 内部スレッドを生成する際に使用されるヒープ。LOWから確保される。
   */
  //------------------------------------------------------------------
  void WifiSystem::Initialize( gfl2::heap::HeapBase *heap )
  {
    m_heapBase    = heap;
  }

  //------------------------------------------------------------------
  /**
   * @brief   終了
   */
  //------------------------------------------------------------------
  void WifiSystem::Terminate( void )
  {
    m_heapBase = NULL;
  }


  //------------------------------------------------------------------
  /**
   * @brief  WifiSystemの初期化
   */
  //------------------------------------------------------------------
  nn::Result WifiSystem::InitializeAC( void )
  {
    {
      nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

      m_isConnect         = false;

      // @fix NMCat[3258]：リスナーセット後にNULLにされていたのでコメントアウト
      //m_disconnetListener = NULL;
    }

    nn::Result     result;

    // 既に初期済み
    if( nn::ac::IsInitialized() ){
      return nn::ResultSuccess();
    }

    result = nn::ac::Initialize();
    if( result.IsFailure() ){
      return result;
    }

    // 接続要求用のパラメータを作成
    result = nn::ac::CreateDefaultConfig( &m_config );
    if (result.IsFailure())
    {
        nn::ac::Finalize();
        return result;
    }


    return nn::ResultSuccess();
  }



  //------------------------------------------------------------------
  /**
   * @brief  ＡＰへの接続を開始(同期接続）
   */
  //------------------------------------------------------------------
  nn::Result WifiSystem::Connect( void )
  {
    m_event.ClearSignal();
    nn::Result result = nn::ac::RegisterDisconnectEvent( &m_event );
    if (result.IsFailure()){
        return result;
    }

    result = nn::ac::Connect( m_config );
    if( result.IsSuccess() )
    {
      nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御
      m_isConnect = true;
    }
    return result;
  }


  //------------------------------------------------------------------
  /**
   * @brief  ＡＰへの接続を終了
   */
  //------------------------------------------------------------------
  nn::Result WifiSystem::Close( void )
  {
    {
      nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

      m_event.ClearSignal();

      if( !m_isConnect )
      {
        return nn::ResultSuccess(); // 切断状態なら何もしない
      }

      m_isConnect = false;
    }

    return nn::ac::Close();
  }


  //------------------------------------------------------------------
  /**
   * @brief  WifiSystemの終了処理
   */
  //------------------------------------------------------------------
  nn::Result WifiSystem::FinalizeAC( void )
  {
    {
      nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御
      m_disconnetListener = NULL;
    }

    nn::Result result = nn::ac::Finalize();
    if( !result.IsSuccess() ){
      return result;
    }

    return nn::ResultSuccess();
  }


  //------------------------------------------------------------------
  /**
   * @brief ＡＰ不正切断通知用リスナを登録する
   *
   * @param[in,out] listener 通知先のリスナ。NULLを指定することで登録を解除できる。
   */
  //------------------------------------------------------------------
  void WifiSystem::SetDisconnectingListener( WifiDisconnectListener* listener )
  {
    nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

    m_disconnetListener = listener;
  }


  //------------------------------------------------------------------
  /**
   * @brief 更新処理。ポーリングしてください。
   */
  //------------------------------------------------------------------
  void WifiSystem::Update( void )
  {
    nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

    if( m_isConnect )
    {
      if( m_event.Wait( 0 ) )
      {
        if( m_disconnetListener != NULL )
        {
          u32        errorCode;
          nn::Result result = nn::ac::CTR::GetLastErrorCode(&errorCode);

          if( result.IsSuccess() )
          {
            m_disconnetListener->OnWifiDisconnected( errorCode );
          }
        }
        m_isConnect = false;
        m_event.ClearSignal();
      }
    }
  }


  //------------------------------------------------------------------
  /**
   * @brief 接続確認
   *
   * @return 接続中ならtrueを返却する
   */
  //------------------------------------------------------------------
  bool WifiSystem::IsConnect( void )
  {
    nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

    return m_isConnect;
  }


  //------------------------------------------------------------------
  /**
   * @brief  スレッドにて非同期で[Initialize(), Connect()]を実行する
   *
   * @attention 通信終了イベント通知後、必ずDeleteAsync()をコールしてください。
   *
   * @param  listener          接続結果通知用イベントリスナ
   * @param  thread_priority   スレッドの実行優先度
   *
   * @return スレッドの開始に成功したならtrue、失敗したならfalseを返却
   */
  //------------------------------------------------------------------
  bool WifiSystem::StartWifiASync( WifiConnectListener *listener, int thread_priority )
  {
    if( m_heapBase == NULL )
    {
      return false;
    }

    this->DeleteAsync();

    m_thread = GFL_NEW_LOW( m_heapBase ) WifiConnectThread( this, m_heapBase->GetLowerHandle(), THREAD_STACK_SIZE, WifiConnectThread::E_CONNECT, listener );

    if( m_thread == NULL ){
      return false;
    }
    m_thread->Start();

    return true;
  }


  //------------------------------------------------------------------
  /**
   * @brief  スレッドにて非同期で[Close(), Finalize()]を実行する
   *
   * @attention 通信終了イベント通知後、必ずDeleteAsync()をコールしてください。
   *
   * @param  listener          切断結果通知用イベントリスナ
   * @param  thread_priority   スレッドの実行優先度
   *
   * @return スレッドの開始に成功したならtrue、失敗したならfalseを返却
   */
  //------------------------------------------------------------------
  bool WifiSystem::EndWifiASync( WifiConnectListener *listener, int thread_priority )
  {
    if( m_heapBase == NULL )
    {
      return false;
    }

    this->DeleteAsync();

    m_thread = GFL_NEW_LOW( m_heapBase ) WifiConnectThread( this, m_heapBase, THREAD_STACK_SIZE, WifiConnectThread::E_DISCONNECT, listener );

    if( m_thread == NULL ){
      return false;
    }
    m_thread->Start();

    return true;
  }


  //------------------------------------------------------------------
  /**
   * @brief  最終エラーコードを取得
   *
   * @param  errorCode エラーコード格納先
   *
   * @param  エラーコード取得結果
   */
  //------------------------------------------------------------------
  nn::Result WifiSystem::GetLastErrorCode( u32* errorCode )
  {
    *errorCode = 0;
    return nn::ac::GetLastErrorCode( errorCode );
  }


  //------------------------------------------------------------------
  /**
   * @brief  エラー内容をコンソールに出力
   */
  //------------------------------------------------------------------
  void WifiSystem::PrintError( const nn::Result &result )
  {
    if( result == nn::ac::ResultNotFoundAccessPoint() )
    {
      GFL_PRINT("nn::ac::Connect() failed  / ResultNotFoundAccessPoint! \n"  );
    }
    else if( result == nn::ac::ResultCanceled() )
    {
      GFL_PRINT("nn::ac::Connect() failed  / ResultCanceled! \n"  );
    }
    else if( result == nn::ac::ResultAlreadyExists() )
    {
      GFL_PRINT("nn::ac::Connect() failed  / ResultAlreadyExists! \n"  );
    }
    else if( result == nn::ac::ResultWifiOff() )
    {
      GFL_PRINT("nn::ac::Connect() failed  / ResultWifiOff! \n"  );
    }
    else if( result == nn::ac::ResultFailedScan() )
    {
      GFL_PRINT("nn::ac::Connect() failed  / ResultFailedScan! \n"  );
    }
    else if( result == nn::ac::ResultConflictIpAddress() )
    {
      GFL_PRINT("nn::ac::Connect() failed  / ResultConflictIpAddress! \n"  );
    }
    else if( result == nn::ac::ResultFailedDhcp() )
    {
      GFL_PRINT("nn::ac::Connect() failed  / ResultFailedDhcp! \n"  );
    }
    else if( result == nn::ac::ResultInvalidDns() )
    {
      GFL_PRINT("nn::ac::Connect() failed  / ResultInvalidDns! \n"  );
    }
    else if( result == nn::ac::ResultInvalidProxy() )
    {
      GFL_PRINT("nn::ac::Connect() failed  / ResultInvalidProxy! \n"  );
    }
    else if( result == nn::ac::ResultUnsupportPlace() )
    {
      GFL_PRINT("nn::ac::Connect() failed  / ResultUnsupportPlace! \n"  );
    }
    else if( result == nn::ac::ResultFailedHotspotAuthentication() )
    {
      GFL_PRINT("nn::ac::Connect() failed  / ResultFailedHotspotAuthentication! \n"  );
    }
    else if( result == nn::ac::ResultOutOfMemory() )
    {
      GFL_PRINT("nn::ac::Connect() failed  / ResultOutOfMemory! \n"  );
    }
    else if( result == nn::ac::ResultAlreadyConnectUnsupportAp() )
    {
      GFL_PRINT("nn::ac::Connect() failed  / ResultAlreadyConnectUnsupportAp! \n"  );
    }
    else if( result == nn::ac::ResultNotAgreeEula() )
    {
      GFL_PRINT("nn::ac::Connect() failed  / ResultNotAgreeEula! \n"  );
    }
    else
    {
      GFL_PRINT("nn::ac::Connect() failed  / unknown! \n"  );
    }
  }

  //------------------------------------------------------------------
  /**
   * @brief  ネットワーク接続要求のレベルを指定(デバッグ関数)
   *
   * @param  result 出力対象となるnn::Result
   */
  //------------------------------------------------------------------
  nn::Result WifiSystem::DebugSetNetworkArea( nn::ac::CTR::NetworkArea netArea )
  {
    nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御

    return nn::ac::CTR::DebugSetNetworkArea( &m_config, netArea );
  }

  //-------------------------------------------------------------------------------
  /**
   * @brief 非同期通信が実行中かどうかを調べる
   * @retval true   実行中
   * @retval false  実行していない
   */
  //-------------------------------------------------------------------------------
  bool WifiSystem::CheckAsync( void ) const
  {
    return ( m_thread && m_thread->IsAlive() );
  }


  //-------------------------------------------------------------------------------
  /**
   * @brief 実行中の非同期通信をキャンセルする
   *
   * @note
   * CheckAsync()で非同期処理の終了を確認した後,
   * EndAsync()を呼んでください。
   */
  //-------------------------------------------------------------------------------
  void WifiSystem::CancelAsync( void )
  {
    if( m_thread != NULL ){
      m_thread->RemoveListener();
    }

    if( this->CheckAsync() )
    {
      if( m_thread != NULL ){
        m_thread->Kill();
      }
    }
  }


  //-------------------------------------------------------------------------------
  /**
   * @brief スレッドの終了待ちを行います。
   */
  //-------------------------------------------------------------------------------
  void WifiSystem::WaitAsyncFinished( void )
  {
    this->CancelAsync();

    while( this->CheckAsync() ) {
      nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
    }
  }


  //-------------------------------------------------------------------------------
  /**
   * @brief 非同期処理スレッドを破棄する
   */
  //-------------------------------------------------------------------------------
  void WifiSystem::DeleteAsync( void )
  {
    this->WaitAsyncFinished();

    GFL_SAFE_DELETE( m_thread );
  }


} // wifi
} // gflnet

#endif
