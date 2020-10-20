// ============================================================================
/*
 * @file gfl2_CryptoThreadController.cpp
 * @brief スレッド制御クラスです。
 * @date 2015.03.12
 */
// ============================================================================
#include <crypto/source/gfl2_CryptoThreadController.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)


CryptoThreadController::CryptoThreadController( gfl2::heap::HeapBase* pHeap, int threadPriority ) :
  m_pHeap( pHeap ),
  m_pThread( NULL ),
  m_ThreadPriority( threadPriority )
{
}


CryptoThreadController::~CryptoThreadController()
{
}


void CryptoThreadController::Start( gfl2::thread::ThreadInterface* pThreadInterface, bool bMainLoop )
{
  if( m_pThread == NULL )
  {
    m_pThread = GFL_NEW( m_pHeap ) gfl2::thread::Thread( pThreadInterface, m_pHeap, bMainLoop, STACK_SIZE );
    m_pThread->Start( m_ThreadPriority );
  }
}


bool CryptoThreadController::Update()
{
  bool bIsAlive = false;

  if( m_pThread != NULL )
  {
    bIsAlive = m_pThread->IsAlive();
        
    if( !bIsAlive )
    {
      m_pThread->Kill();
          
      GFL_SAFE_DELETE( m_pThread );
    }
  }

  return bIsAlive;
}


bool CryptoThreadController::IsBusy()
{
  if( m_pThread != NULL )
  {
    return true;
  }
  return false;
}


GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)
