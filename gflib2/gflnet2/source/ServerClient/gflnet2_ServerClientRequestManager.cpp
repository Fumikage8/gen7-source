//=============================================================================
/**
 * @file   gflnet2_ServerClientRequestManager.cpp
 * @brief  サーバ通信リクエスト管理
 * @author shin kosaka
 * @date   2014.9.10
 */
//=============================================================================
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <thread/include/gfl2_ThreadStatic.h>

#if GFL_RO_ENABLE
SINGLETON_INSTANCE_DEFINITION(gflnet2::ServerClient::ServerClientRequestManager)
template class gfl2::base::SingletonAccessor<gflnet2::ServerClient::ServerClientRequestManager>;
#endif

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(ServerClient)

//------------------------------------------------------------------
/**
* @brief   コンストラクタ
*/
//------------------------------------------------------------------
ServerClientRequestManager::ServerClientRequestManager()
: m_lastRequest(INVALID_HANDLE)
, m_pHeap(NULL)
, m_forcedTimeout(0)
, m_requestSequenceArray()
{
}

//------------------------------------------------------------------
/**
* @brief   デストラクタ
*/
//------------------------------------------------------------------
ServerClientRequestManager::~ServerClientRequestManager()
{
  Finalize();
}

//------------------------------------------------------------------
/**
* @brief   初期化処理
* @param[in] pHeap ヒープ
*/
//------------------------------------------------------------------
void ServerClientRequestManager::Initialize( gfl2::heap::HeapBase* pHeap )
{
  Finalize();

  m_pHeap = pHeap;
}

//------------------------------------------------------------------
/**
* @brief   終了処理
* @note    リクエストが残っているとリクエストの終了を待つので、
  　　　　　IsRequestEmptyがtrueになってから実行すること
*/
//------------------------------------------------------------------
void ServerClientRequestManager::Finalize( void )
{
  if( m_pHeap )
  {
    if( IsRequestEmpty() == false )
    {//処理中のリクエストがあるので全部無くなるまでブロックする
      GFL_PRINT("warning : request is running. sync finalize exec.\n" );
      CancelAllRequest();
      if( WaitRequestSequenceRunningInternal() == false )
      {//強制タイムアウト
        return;
      }
    }

    //リクエスト全削除
    for( u32 index = 0 ; index < m_requestSequenceArray.size() ; ++index )
    {
      GFL_SAFE_DELETE( m_requestSequenceArray[index] );
    }
    m_requestSequenceArray.clear();

    m_pHeap = NULL;
  }
}

//------------------------------------------------------------------
/**
* @brief  更新処理。
*
* @note   	メインループ毎にポーリングすること
*			      通信処理やリスナーに通知されるイベントはすべてこの関数内で実行される
*/
//------------------------------------------------------------------
void ServerClientRequestManager::Update( void )
{
  //現在は逐次実行している
  //並列実行したい場合はここを変更すれば可能だが、
  //個々のリクエストが同時実行可能かどうか、判定/制御する必要がある
  if( m_requestSequenceArray.size() > 0 && m_requestSequenceArray[0] )
  {
    //個別の通信処理をアップデート
    REQUEST_SEQUENCE_STATE state = REQUEST_STATE_FINISH;
    if( m_requestSequenceArray[0]->IsCancelRequested() == false )
    {//通常アップデート
      state = m_requestSequenceArray[0]->Update();
    }
    else
    {//キャンセルリクエスト時のアップデート
      state = m_requestSequenceArray[0]->UpdateCancel();
    }

    if( state == REQUEST_STATE_FINISH )
    {
      //終わったリクエストを削除
      RequestSequenceBase* pEndRequest = m_requestSequenceArray[0];
      m_requestSequenceArray.remove( m_requestSequenceArray[0] );
      GFL_SAFE_DELETE( pEndRequest );
    }
  }
}


//スタティック関数群

//------------------------------------------------------------------
/**
* @brief  実行中のリクエストが無いか確認
*
* @retval true： 実行中のリクエストが無い
* @retval false: 実行中のリクエストがある
*/
//------------------------------------------------------------------
bool ServerClientRequestManager::IsRequestEmpty( void )
{
  ServerClientRequestManager* pManager = GFL_SINGLETON_INSTANCE(ServerClientRequestManager);
  if( pManager == NULL )
  {
    // Win版でインスタンスが作られていないのでコメントアウト
    //GFL_ASSERT_MSG( 0 ,"ServerClientRequestManager is NULL" );
    return true;
  }

  return pManager->IsRequestEmptyInternal();
}


//------------------------------------------------------------------
/**
* @brief  指定したハンドルのリクエストが実行中かどうか確認
* @param[in] handle リクエストハンドル
* @retval true： 指定したリクエストが実行中
* @retval false: 指定したリクエストは終了している
*/
//------------------------------------------------------------------
bool ServerClientRequestManager::IsRequestRunning( RequestHandle handle )
{
  ServerClientRequestManager* pManager = GFL_SINGLETON_INSTANCE(ServerClientRequestManager);
  if( pManager == NULL )
  {
    // Win版でインスタンスが作られていないのでコメントアウト
    //GFL_ASSERT_MSG( 0 ,"ServerClientRequestManager is NULL" );
    return false;
  }

  return ( pManager->GetRequestSequenceInternal( handle ) != NULL );
}

//------------------------------------------------------------------
/**
* @brief  リクエストのキャンセル
* @param[in] handle リクエストハンドル
*/
//------------------------------------------------------------------
void ServerClientRequestManager::CancelRequest( RequestHandle handle )
{
  ServerClientRequestManager* pManager = GFL_SINGLETON_INSTANCE(ServerClientRequestManager);
  if( pManager == NULL )
  {
    // Win版でインスタンスが作られていないのでコメントアウト
    //GFL_ASSERT_MSG( 0 ,"ServerClientRequestManager is NULL" );
    return;
  }

  pManager->CancelRequestInternal( handle );
}

//------------------------------------------------------------------
/**
* @brief  全リクエストのキャンセル
*/
//------------------------------------------------------------------
void ServerClientRequestManager::CancelAllRequest( void )
{
  ServerClientRequestManager* pManager = GFL_SINGLETON_INSTANCE(ServerClientRequestManager);
  if( pManager == NULL )
  {
    // Win版でインスタンスが作られていないのでコメントアウト
    //GFL_ASSERT_MSG( 0 ,"ServerClientRequestManager is NULL" );
    return;
  }

  pManager->CancelAllRequestInternal();
}



//------------------------------------------------------------------
/**
* @brief 　 リスナー登録解除
* @param[in] pListener 登録解除するリスナークラス
*/
//------------------------------------------------------------------
void ServerClientRequestManager::RemoveListener( RequestHandle handle )
{
  ServerClientRequestManager* pManager = GFL_SINGLETON_INSTANCE(ServerClientRequestManager);
  if( pManager == NULL )
  {
    // Win版でインスタンスが作られていないのでコメントアウト
    //GFL_ASSERT_MSG( 0 ,"ServerClientRequestManager is NULL" );
    return;
  }

  pManager->RemoveListenerInternal( handle );
}

//------------------------------------------------------------------
/**
* @brief 　 全リスナーの登録解除
*/
//------------------------------------------------------------------
void ServerClientRequestManager::RemoveAllListener( void )
{
  ServerClientRequestManager* pManager = GFL_SINGLETON_INSTANCE(ServerClientRequestManager);
  if( pManager == NULL )
  {
    // Win版でインスタンスが作られていないのでコメントアウト
    //GFL_ASSERT_MSG( 0 ,"ServerClientRequestManager is NULL" );
    return;
  }

  pManager->RemoveAllListenerInternal();
}


//プライベート関数群

//------------------------------------------------------------------
/**
* @brief  実行中のリクエストが無いか確認
*
* @retval true： 実行中のリクエストが無い
* @retval false: 実行中のリクエストがある
*/
//------------------------------------------------------------------
bool ServerClientRequestManager::IsRequestEmptyInternal( void ) const
{
  return (m_requestSequenceArray.size() == 0);
}

//------------------------------------------------------------------
/**
* @brief  通信シーケンス作成用のヒープを取得
*
* @return ヒープ
*/
//------------------------------------------------------------------
gfl2::heap::HeapBase* ServerClientRequestManager::GetHeapInternal( void )
{
  GFL_ASSERT_MSG( m_pHeap != NULL , "heap is null" );

  return m_pHeap;
}



//------------------------------------------------------------------
/**
* @brief 　ハンドルからリクエストシーケンスを取得
* @param[in] handle リクエストハンドル
* @return リクエストシーケンス
*/
//------------------------------------------------------------------
RequestSequenceBase* ServerClientRequestManager::GetRequestSequenceInternal( RequestHandle handle )
{
  for( u32 index = 0 ; index < m_requestSequenceArray.size() ; ++index )
  {
    if( m_requestSequenceArray[index] )
    {
      if( m_requestSequenceArray[index]->GetRequestHandle() == handle )
      {
        return m_requestSequenceArray[index];
      }
    }
  }

  return NULL;
}

//------------------------------------------------------------------
/**
* @brief  リクエストシーケンスの追加
* @param[in] pRequest リクエストシーケンス
*
* @return リクエストハンドル
*/
//------------------------------------------------------------------
RequestHandle ServerClientRequestManager::AddRequestSequenceInternal( RequestSequenceBase* pRequest )
{
  if( m_requestSequenceArray.size() >= MAX_REQUEST_NUM )
  {//リクエストキューが一杯
    GFL_ASSERT_MSG( 0 , "request limit over" );

    if( WaitRequestSequenceRunningInternal() == false )
    {//タイムアウト
      GFL_SAFE_DELETE( pRequest );
      return INVALID_HANDLE;
    }
  }

  m_lastRequest++;
  pRequest->SetRequestHandle( m_lastRequest );

  m_requestSequenceArray.push_back( pRequest );

  return m_lastRequest;
}

//------------------------------------------------------------------
/**
* @brief  リクエストのキャンセル
* @param[in] handle リクエストハンドル
*/
//------------------------------------------------------------------
void ServerClientRequestManager::CancelRequestInternal( RequestHandle handle )
{
  RequestSequenceBase* pRequest = GetRequestSequenceInternal( handle );
  if( pRequest )
  {
    pRequest->Cancel();
  }
}

//------------------------------------------------------------------
/**
* @brief  全リクエストのキャンセル
*/
//------------------------------------------------------------------
void ServerClientRequestManager::CancelAllRequestInternal( void )
{
  for( u32 index = 0 ; index < m_requestSequenceArray.size() ; ++index )
  {
    if( m_requestSequenceArray[index] )
    {
      m_requestSequenceArray[index]->Cancel();
    }
  }
}



//------------------------------------------------------------------
/**
* @brief 　 リスナー登録解除
* @param[in] handle 登録解除するハンドル
*/
//------------------------------------------------------------------
void ServerClientRequestManager::RemoveListenerInternal( RequestHandle handle )
{
  RequestSequenceBase* pRequest = GetRequestSequenceInternal( handle );
  if( pRequest )
  {
    pRequest->RemoveListener();
  }
}

//------------------------------------------------------------------
/**
* @brief 　 全リスナーの登録解除
*/
//------------------------------------------------------------------
void ServerClientRequestManager::RemoveAllListenerInternal( void )
{
  for( u32 index = 0 ; index < m_requestSequenceArray.size() ; ++index )
  {
    if( m_requestSequenceArray[index] )
    {
      m_requestSequenceArray[index]->RemoveListener();
    }
  }
}

//------------------------------------------------------------------
/**
* @brief 　 リクエスト終了待ち
* @retval true： 正常終了
* @retval false:　強制タイムアウト
*/
//------------------------------------------------------------------
bool ServerClientRequestManager::WaitRequestSequenceRunningInternal( void )
{
  m_forcedTimeout = 0;
  while( IsRequestEmpty() == false )
  {
    Update();
    gfl2::thread::ThreadStatic::CurrentSleep(WAIT_SLEEP_MSEC);
    m_forcedTimeout++;
    if( m_forcedTimeout > FORCED_TIMEOUT )
    {//強制タイムアウト
      return false;
    }
  }

  //正常終了
  return true;
}




GFL_NAMESPACE_END(ServerClient)
GFL_NAMESPACE_END(gflnet2)

