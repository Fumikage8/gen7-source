// ============================================================================
/*
 * @file gflnet2_CTR_CecMessageBoxAccessThread.cpp
 * @brief すれ違い通信のメッセージボックスへアクセスする為のスレッドです
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_CTR
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecMessageBoxAccessThread.h>
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecMessageBoxAccessor.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(ctr)


CTR_CecMessageBoxAccessThread::CTR_CecMessageBoxAccessThread( CTR_CecMessageBoxAccessor* pCecMessageBoxAccessor ) :
  m_pCecMessageBoxAccessor( pCecMessageBoxAccessor ),
  m_pExecuteObject( NULL )
{
}


CTR_CecMessageBoxAccessThread::~CTR_CecMessageBoxAccessThread()
{
}

// --------------------------------------------------------------------------
/**
  * @brief  メッセージボックスへのアクセスを実行します。
  */
// --------------------------------------------------------------------------
void CTR_CecMessageBoxAccessThread::Function()
{
  m_eResultCode = m_pCecMessageBoxAccessor->Execute( m_pExecuteObject );
}


// --------------------------------------------------------------------------
/**
  * @brief  実行するオブジェクトを設定します。
  *         スレッドを走らせる前に設定しておく必要があります。
  * @param  pExecuteObject メッセージボックスへのアクセスコマンドオブジェクト
  */
// --------------------------------------------------------------------------
void CTR_CecMessageBoxAccessThread::SetExecuteObject( CTR_CecControlObject* pExecuteObject )
{
  m_pExecuteObject = pExecuteObject;
}


// --------------------------------------------------------------------------
/**
  * @brief  メッセージボックスへアクセスした時の実行結果を取得します。
  * @return 実行結果
  */
// --------------------------------------------------------------------------
ResultCode CTR_CecMessageBoxAccessThread::GetResultCode() const
{
  return m_eResultCode;
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif
