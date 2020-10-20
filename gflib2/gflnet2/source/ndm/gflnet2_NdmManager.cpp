#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NdmManager.cpp
 * @brief  nn::ndmのラッパークラス。デーモンを管理する
 * @author Shin Kosaka
 * @date   2012.08.02
 */
//=============================================================================
#include <gflnet2/include/ndm/gflnet2_NdmManager.h>
#include "gflnet2_NdmSuspendThread.h"
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
#if GFL_RO_ENABLE
SINGLETON_INSTANCE_DEFINITION(gflnet2::ndm::NdmManager)
template class gfl2::base::SingletonAccessor<gflnet2::ndm::NdmManager>;
#endif


GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(ndm)


//------------------------------------------------------------------
/**
* @brief   NdmManagerコンストラクタ
*/
//------------------------------------------------------------------
NdmManager::NdmManager( gfl2::heap::HeapBase *pHeap )
: m_pHeapBase( pHeap )
, m_pThread( NULL )
, m_isInitialized( false )
{
}


//------------------------------------------------------------------
/**
* @brief   NdmManagerデストラクタ
*/
//------------------------------------------------------------------
NdmManager::~NdmManager()
{
  Finalize();
}


//------------------------------------------------------------------
/**
* @brief  NdmManagerの初期化
*/
//------------------------------------------------------------------
nn::Result NdmManager::Initialize( void )
{
  nn::Result     result;

  if( m_isInitialized )
  {//既に初期化済み
    return nn::ResultSuccess();
  }

  result = nn::ndm::Initialize();
  if( !result.IsSuccess() )
  {//失敗
     return result;
  }

  m_isInitialized = true;

  return nn::ResultSuccess();
}

//------------------------------------------------------------------
/**
* @brief  NdmManagerの終了処理
*/
//------------------------------------------------------------------
nn::Result NdmManager::Finalize( void )
{
  if( !m_isInitialized )
  {//終了処理済み
    return nn::ResultSuccess();
  }

  DeleteAsyncInternal();

  nn::Result result = nn::ndm::Finalize();
  if( !result.IsSuccess() )
  {
      return result;
  }

  m_isInitialized = false;

  return nn::ResultSuccess();
}

//------------------------------------------------------------------
/**
* @brief  無線LANスイッチが有効かどうか
*
* @return 有効true : 無効false
*/
//------------------------------------------------------------------
bool NdmManager::IsWLANSwitchEnable( void )
{
  return nn::ndm::IsDaemonStartEnable();
}

//------------------------------------------------------------------
/**
* @brief  すれちがいデーモンを停止（nn::cec）
* @note
* すれちがい通信を行うためのデーモンを停止します。
* 起動直後は、停止されていない状態になっています。
* @return 実行結果
*/
//------------------------------------------------------------------
nn::Result NdmManager::SuspendCec( void )
{
  return SuspendInternal( nn::ndm::DN_CEC );
}

//------------------------------------------------------------------
/**
* @brief  すれちがいデーモンの停止解除
*
* @return 実行結果
*/
//------------------------------------------------------------------
nn::Result NdmManager::ResumeCec( void )
{
  return ResumeInternal( nn::ndm::DN_CEC );
}


//------------------------------------------------------------------
/**
* @brief  フレンド・プレゼンスデーモンを停止（nn::friends）
* @note
* フレンド・プレゼンス通信を行うためのデーモンを停止します。
* 起動直後は、停止されていない状態になっています。
* @return 実行結果
*/
//------------------------------------------------------------------
nn::Result NdmManager::SuspendFriends( void )
{
  return SuspendInternal( nn::ndm::DN_FRIENDS );
}

//------------------------------------------------------------------
/**
* @brief  フレンド・プレゼンスデーモンの停止解除
*
* @return 実行結果
*/
//------------------------------------------------------------------
nn::Result NdmManager::ResumeFriends( void )
{
  return ResumeInternal( nn::ndm::DN_FRIENDS );
}

//------------------------------------------------------------------
/**
* @brief  BOSSデーモンを停止（nn::boss）
* @note
* BOSS通信を行うためのデーモンを停止します。
* このデーモンのみ、起動直後は、停止されている状態になっています。
* @return 実行結果
*/
//------------------------------------------------------------------
nn::Result NdmManager::SuspendBoss( void )
{
  nn::Result result = SuspendInternal( nn::ndm::DN_BOSS );
  if( !result.IsSuccess() )
  {
      return result;
  }

  /*
    nn::ndm::CTR::DN_NIM はシステム寄りの処理を行うための補助モジュールです。
    nn::ndm::CTR::DN_BOSS を停止する場合は一緒に nn::ndm::DN_NIM も停止することを推奨します。
  */

  result = SuspendInternal( nn::ndm::DN_NIM );
  if( !result.IsSuccess() )
  {
    result = ResumeInternal( nn::ndm::DN_BOSS );
    if( !result.IsSuccess() )
    {
      GFL_PRINT("NdmManager::SuspendBoss: failed\n");
    }

    return result;
  }

  return nn::ResultSuccess();
}

//------------------------------------------------------------------
/**
* @brief  BOSSデーモンの停止解除
*
* @return 実行結果
*/
//------------------------------------------------------------------
nn::Result NdmManager::ResumeBoss( void )
{
  nn::Result result = ResumeInternal( nn::ndm::DN_BOSS );
  if( !result.IsSuccess() )
  {
      return result;
  }

  /*
    nn::ndm::CTR::DN_NIM はシステム寄りの処理を行うための補助モジュールです。
    nn::ndm::CTR::DN_BOSS を停止する場合は一緒に nn::ndm::DN_NIM も停止することを推奨します。
  */

  result = ResumeInternal( nn::ndm::DN_NIM );
  if( !result.IsSuccess() )
  {
    result = SuspendInternal( nn::ndm::DN_BOSS );
    if( !result.IsSuccess() )
    {
      GFL_PRINT("NdmManager::ResumeBoss : failed\n");
    }

      return result;
  }

  return nn::ResultSuccess();
}

//------------------------------------------------------------------
/**
* @brief  デーモンスケジューラ全体を停止（同期）
* @note
* すべてのデーモンおよび、アクセスポイントの検索、自動接続等のバックグラウンド動作を
* 完全に停止します。処理が完了するまで呼び出し元のスレッドをブロックします。
* @return 実行結果
*/
//------------------------------------------------------------------
nn::Result NdmManager::SuspendDeamonSchedulerSync( void )
{
  return nn::ndm::SuspendScheduler( false );
}

//------------------------------------------------------------------
/**
* @brief  デーモンスケジューラ全体を停止（非同期）
* @note
* すべてのデーモンおよび、アクセスポイントの検索、自動接続等のバックグラウンド動作を
* 完全に停止します。停止の完了を待たずに処理を返します。結果はイベントで通知されます。
* @return 実行結果
*/
//------------------------------------------------------------------
bool NdmManager::SuspendDeamonSchedulerASync( NdmEventListener *pListener, int threadPriority )
{
    this->CancelAsyncInternal();
    this->DeleteAsyncInternal();

    m_pThread = GFL_NEW_LOW( m_pHeapBase ) NdmSuspendThread( this, m_pHeapBase, THREAD_STACK_SIZE , pListener );

    if( m_pThread == NULL ){
      return false;
    }
    m_pThread->Start();

    return true;
}

//------------------------------------------------------------------
/**
* @brief   デーモンスケジューラ全体の停止解除
*
*自プロセスが SuspendScheduler を複数回呼び出している場合は、ResumeScheduler をその回数分呼び出さないと停止は解除されません。
*
* @return 実行結果
*/
//------------------------------------------------------------------
nn::Result NdmManager::ResumeDeamonScheduler( void )
{
  return nn::ndm::ResumeScheduler();
}


//------------------------------------------------------------------
/**
* @brief  エラー内容をコンソールに出力
*/
//------------------------------------------------------------------
void NdmManager::PrintError( const nn::Result &result )
{
  if( result == nn::ndm::ResultInvalidOperation() )
  {
    GFL_PRINT("NdmManager::PrintError:ResultInvalidOperation\n");
  }
  else if( result == nn::ndm::ResultNotInitialized() )
  {
    GFL_PRINT("NdmManager::PrintError:ResultNotInitialized\n");
  }
  else if( result == nn::ndm::ResultNotSuspended() )
  {
    GFL_PRINT("NdmManager::PrintError:ResultNotSuspended\n");
  }
  else if( result == nn::ndm::ResultAlreadySuspended() )
  {
    GFL_PRINT("NdmManager::PrintError:ResultAlreadySuspended\n");
  }
  else
  {
    GFL_PRINT("NdmManager::PrintError:Unknown[0x%x]\n",result.GetPrintableBits());
  }
}

//-------------------------------------------------------------------------------
/**
* @brief   個別デーモンの停止
*
* @return 実行結果
*/
//-------------------------------------------------------------------------------
nn::Result NdmManager::SuspendInternal( nn::ndm::DaemonName name )
{
  return nn::ndm::Suspend( name );
}

//-------------------------------------------------------------------------------
/**
* @brief   個別デーモンの停止解除
*
* @return 実行結果
*/
//-------------------------------------------------------------------------------
nn::Result NdmManager::ResumeInternal( nn::ndm::DaemonName name )
{
  return nn::ndm::Resume( name );
}


//-------------------------------------------------------------------------------
/**
* @brief 非同期通信が実行中かどうかを調べる
* @retval true   実行中
* @retval false  実行していない
*/
//-------------------------------------------------------------------------------
bool NdmManager::CheckAsyncInternal( void ) const
{
  return ( m_pThread && m_pThread->IsAlive() );
}


//-------------------------------------------------------------------------------
/**
* @brief 実行中の非同期通信をキャンセルする
*/
//-------------------------------------------------------------------------------
void NdmManager::CancelAsyncInternal( void )
{
  if( this->CheckAsyncInternal() )
  {
      m_pThread->Kill();
  }
}


//-------------------------------------------------------------------------------
/**
* @brief スレッドの終了待ちを行います。
*/
//-------------------------------------------------------------------------------
void NdmManager::WaitAsyncFinishedInternal( void ) const
{
  while( this->CheckAsyncInternal() )
  {
      nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
  }
}


//-------------------------------------------------------------------------------
/**
* @brief 非同期処理スレッドを破棄する
*/
//-------------------------------------------------------------------------------
void NdmManager::DeleteAsyncInternal( void )
{
  this->CancelAsyncInternal();
  this->WaitAsyncFinishedInternal();

  GFL_SAFE_DELETE( m_pThread );
}


GFL_NAMESPACE_END(ndm)
GFL_NAMESPACE_END(gflnet2)

#endif
