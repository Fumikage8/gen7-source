// ============================================================================
/*
 * @file gflnet2_CTR_CecController.cpp
 * @brief すれ違い通信を制御するクラスです
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_CTR
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecController.h>
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecMessageBoxAccessor.h>
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecMessageBoxAccessThread.h>
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecControlObject.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(ctr)


CTR_CecController::CTR_CecController( gfl2::heap::HeapBase* pHeap ) :
  m_pHeap( pHeap ),
  m_pCecMessageBoxAccessor( NULL ),
  m_pCecMessageBoxAccessThread( NULL ),
  m_pExecuteObject( NULL ),
  m_pThread( NULL ),
  m_eState( START_WAIT ),
  m_ThreadPriority( gfl2::thread::Thread::THREAD_PRI_NORMAL )
{
}


CTR_CecController::~CTR_CecController()
{
  GFL_SAFE_DELETE( m_pCecMessageBoxAccessThread );
  GFL_SAFE_DELETE( m_pCecMessageBoxAccessor );
}


// --------------------------------------------------------------------------
/**
  * @brief  初期化処理を行います。
  * @param  pInitParam 初期化用のパラメータです
  * @param  pCecAllocator すれ違い通信ライブラリ内で使用するメモリアロケータです
  */
// --------------------------------------------------------------------------
void CTR_CecController::Initialize( const gflnet2::cec::CecSystemInitParam* pInitParam, CTR_CecAllocator* pCecAllocator )
{
  m_pCecMessageBoxAccessor = GFL_NEW( m_pHeap ) gflnet2::cec::ctr::CTR_CecMessageBoxAccessor( pCecAllocator );
  m_pCecMessageBoxAccessThread = GFL_NEW( m_pHeap ) gflnet2::cec::ctr::CTR_CecMessageBoxAccessThread( m_pCecMessageBoxAccessor );

  m_pCecMessageBoxAccessor->Initialize( m_pHeap, pInitParam );

  m_ThreadPriority = pInitParam->threadPriority;
}


// --------------------------------------------------------------------------
/**
  * @brief  更新処理を行います。
  *         制御オブジェクトが登録されたらスレッドを作成してメッセージボックスへアクセスします。
  */
// --------------------------------------------------------------------------
void CTR_CecController::Update()
{
  switch( m_eState )
  {
    case START_WAIT:
      if( m_pExecuteObject != NULL )
      {
        GFL_ASSERT( m_pThread == NULL );

        m_pCecMessageBoxAccessThread->SetExecuteObject( m_pExecuteObject );
        
        m_pThread = GFL_NEW( m_pHeap ) gfl2::thread::Thread( m_pCecMessageBoxAccessThread, m_pHeap, false, THREAD_STACK_SIZE );
        
        m_pThread->Start( m_ThreadPriority );
        
        m_eState = END_WAIT;
      }
    break;

    case END_WAIT:
      if( m_pThread != NULL )
      {
        bool bIsAlive = m_pThread->IsAlive();
        
        if( !bIsAlive )
        {
          m_pThread->Kill();
          
          GFL_SAFE_DELETE( m_pThread );

          ResultCode eResultCode = m_pCecMessageBoxAccessThread->GetResultCode();
          
          m_pExecuteObject->OnCommandResult( eResultCode );
          
          m_pExecuteObject = NULL;

          m_eState = START_WAIT;
        }
      }
    break;
  }
}


// --------------------------------------------------------------------------
/**
  * @brief  すれ違い通信の制御オブジェクトを登録します。
  * @param  pCecControlObject すれ違い通信の制御オブジェクト
  */
// --------------------------------------------------------------------------
void CTR_CecController::RegisterControlObject( CTR_CecControlObject* pCecControlObject )
{
  m_pExecuteObject = pCecControlObject;
}


// --------------------------------------------------------------------------
/**
  * @brief  すれ違い通信ライブラリのコマンドを即実行します。
  * @param  pCecControlObject すれ違い通信の制御オブジェクト
  */
// --------------------------------------------------------------------------
void CTR_CecController::QuickExecute( CTR_CecControlObject* pCecControlObject )
{
  m_pCecMessageBoxAccessor->Execute( pCecControlObject );
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif
