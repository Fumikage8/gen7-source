// ============================================================================
/*
 * @file gfl2_CTR_SHA256CalcThread.cpp
 * @brief shaによるハッシュ値を計算するスレッドです。
 * @date 2015.03.12
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#include <crypto/source/ctr/gfl2_CTR_SHA256CalcThread.h>
#include <crypto/source/ctr/gfl2_CTR_SHA256CalcExecutor.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(ctr)


CTR_SHA256CalcThread::CTR_SHA256CalcThread() :
  m_bSuccess( false ),
  m_pSHA256Hash( NULL ),
  m_pData( NULL ),
  m_DataSize( 0 )
{
}


CTR_SHA256CalcThread::~CTR_SHA256CalcThread()
{
}


// --------------------------------------------------------------------------
/**
  * @brief  スレッドから呼び出される関数です。
  *         この関数内でハッシュの計算が行われます。
  */
// --------------------------------------------------------------------------
void CTR_SHA256CalcThread::Function()
{
  m_bSuccess = gfl2::crypto::ctr::CTR_SHA256CalcExecutor::CalcHash( m_pSHA256Hash, m_pData, m_DataSize );
}


// --------------------------------------------------------------------------
/**
  * @brief  ハッシュの計算に必要なパラメータを設定します。
  * @param  pSHA256Hash 計算したハッシュを格納する為のバッファです
  * @param  pData ハッシュを計算する為のデータ
  * @param  dataSize ハッシュを計算する為のデータのサイズです
  */
// --------------------------------------------------------------------------
void CTR_SHA256CalcThread::SetExecuteParameter( gfl2::crypto::SHA256Hash* pSHA256Hash, const void* pData, u32 dataSize )
{
  m_bSuccess    = false;
  m_pSHA256Hash = pSHA256Hash;
  m_pData       = pData;
  m_DataSize    = dataSize;
}


// --------------------------------------------------------------------------
/**
  * @brief  ハッシュの計算の成否を返します。
  * @retval true 成功
  * @retval false 失敗
  */
// --------------------------------------------------------------------------
bool CTR_SHA256CalcThread::IsSuccess() const
{
  return m_bSuccess;
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_CTR
