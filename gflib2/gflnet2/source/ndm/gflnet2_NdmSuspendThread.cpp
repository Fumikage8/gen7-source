#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_NdmSuspendThread.cpp
 * @brief  Wifi接続・切断用用スレッド
 * @author h.suzuki
 */
//=============================================================================


#include "gflnet2_NdmSuspendThread.h"

#include <gflnet2/include/ndm/gflnet2_NdmManager.h>
#include <gflnet2/include/ndm/gflnet2_NdmEventListener.h>


GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(ndm)

//--------------------------------------------------------------------------------
/**
* @brief コンストラクタ
* @param pManager            NdmManagerへのポインタ
* @param pHeap           スレッドのスタック領域に使用するヒープ
* @param stackSize     スレッドのスタック領域のサイズ
* @param pListener       完了通知用リスナ
*/
//--------------------------------------------------------------------------------
NdmSuspendThread::NdmSuspendThread( NdmManager* pManager , gfl2::heap::HeapBase* pHeap , u32 stackSize , NdmEventListener* pListener )
: m_pListener( pListener)
, m_pManager( pManager )
, m_pThread( NULL )
{
  m_pThread = GFL_NEW(pHeap) gfl2::thread::Thread(this, pHeap, true, stackSize);
}

//--------------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//--------------------------------------------------------------------------------
NdmSuspendThread::~NdmSuspendThread()
{
  GFL_SAFE_DELETE( m_pThread );

	m_pListener = NULL;
	m_pManager = NULL;
}




//--------------------------------------------------------------------------------
/**
 * @brief スレッドメイン処理
 */
//--------------------------------------------------------------------------------
void NdmSuspendThread::Function( void )
{
	if( m_pManager )
	{
		nn::Result result = m_pManager->SuspendDeamonSchedulerSync();
		if( m_pListener )
		{
			if( result.IsFailure() )
			{//失敗
				m_pListener->OnNdmSuspendFailed( result );
			}
			else
			{//成功
				m_pListener->OnNdmSuspendSuccess();
			}
		}
	}

 	m_pThread->Kill();
}




GFL_NAMESPACE_END(ndm)
GFL_NAMESPACE_END(gflnet2)



#endif
