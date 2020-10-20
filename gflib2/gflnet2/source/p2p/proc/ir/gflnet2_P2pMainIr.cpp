#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet2_P2pMainIr.cpp
 * @brief	P2Pプロセス：IRメイン
 * @author	ichiraku_katsuhiko
 * @date	2015/03/18 16:23:42
 */
// =============================================================================

#include <nn/ir.h>
#include <gflnet2/include/p2p/gflnet2_NetGame.h>
#include <gflnet2/source/p2p/gflnet2_NetGameBase.h>

#include "gflnet2/source/p2p/proc/ir/gflnet2_P2pMainIr.h"

GFL_NAMESPACE_BEGIN( gflnet2 )
GFL_NAMESPACE_BEGIN( p2p )
GFL_NAMESPACE_BEGIN( proc )

//---------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   Manager  管理クラス
 * @return  RES_FINISHで終了
 */
//---------------------------------------------------------------------------------
gfl2::proc::Result P2pMainIr::InitFunc(gfl2::proc::Manager* )
{
  m_criticalSection.Initialize();
  m_thread = GFL_NEW(GetInitParam()->GetHeapBase()) IrP2pThread( GetInitParam()->GetHeapBase() );
  m_thread->Start( GetP2pManager()->GetThreadPriority() );
  NetGame::GetNetGameBase()->SetP2pIrLoopProc( this );
  return gfl2::proc::RES_FINISH;
}

//---------------------------------------------------------------------------------
/**
 * @brief   実行処理
 * @param   Manager  管理クラス
 * @return  RES_FINISHで終了
 */
//---------------------------------------------------------------------------------
gfl2::proc::Result P2pMainIr::UpdateFunc(gfl2::proc::Manager*)
{
  // 外からの終了リクエスト（切断以外ないはず）
  if( mbEnd )
  {
    NetGame::GetNetGameBase()->SetP2pIrLoopProc( NULL );
    m_thread->KillNetThread();
    SetProcReturn( P2pManager::E_PROC_RETURN_SUCCESS );
    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

//---------------------------------------------------------------------------------
/**
 * @brief   終了処理
 * @param   Manager  管理クラス
 * @return  RES_FINISHで終了
 */
//---------------------------------------------------------------------------------
gfl2::proc::Result P2pMainIr::EndFunc(gfl2::proc::Manager*)
{
  if( m_thread->IsAlive() ){
    return gfl2::proc::RES_CONTINUE;
  }
  GFL_SAFE_DELETE( m_thread );
  m_criticalSection.Finalize();
  return gfl2::proc::RES_FINISH;
}


//----------------------------------------------------------------------------
/**
 * @brief  送信
 * @return 送れたらTRUE
 */
//----------------------------------------------------------------------------
bool P2pMainIr::SendData(void* data, u32 size )
{
  if( m_thread ){
    nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御
    return m_thread->SendData(data,size);
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 * @brief データ受信
 * @return 0だと何も入っていない
 */
//----------------------------------------------------------------------------
u32 P2pMainIr::RecvData(void* data, u32 size )
{
  if( m_thread ){
    nn::os::CriticalSection::ScopedLock lock(m_criticalSection);    // 排他制御
    return m_thread->RecvData(data,size);
  }
  return 0;
}

GFL_NAMESPACE_END( proc )
GFL_NAMESPACE_END( p2p )
GFL_NAMESPACE_END( gflnet2 )

#endif // GF_PLATFORM_CTR


