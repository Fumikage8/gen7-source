// ============================================================================
/*
 * @file gfl2_CTR_SHA256CalcExecutor.cpp
 * @brief shaによるハッシュ値を計算するクラスです。
 * @date 2015.03.12
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#include <crypto/source/ctr/gfl2_CTR_SHA256CalcExecutor.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>
#include <nn/crypto.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(ctr)


// --------------------------------------------------------------------------
/**
  * @brief  ハッシュの計算処理を行います。
  * @param  pSHA256Hash 計算したハッシュを格納する為のバッファです
  * @param  pData ハッシュを計算する為のデータ
  * @param  dataSize ハッシュを計算する為のデータのサイズです
  */
// --------------------------------------------------------------------------
bool CTR_SHA256CalcExecutor::CalcHash( gfl2::crypto::SHA256Hash* pSHA256Hash, const void* pData, u32 dataSize )
{
  bool bSuccess = false;

  nn::crypto::Sha256Context sha256;

  sha256.Initialize();
  sha256.Update( pData, dataSize );
  u32 hashSize = (u32)sha256.GetHashSize();
  if( hashSize <= pSHA256Hash->GetBufferSize() )
  {
    sha256.GetHash( pSHA256Hash->GetData() );
    pSHA256Hash->SetDataSize( hashSize );
    bSuccess = true;
  }
  else
  {
    // ハッシュ値を格納するテンポラリバッファのメモリが不足しているのでもう少し大きいサイズを指定する必要がある
    // ISHA256Calculator::CreateSHA256Calculator( pHeap, hashBufferSize ) hashBufferSize この値
    GFL_ASSERT_MSG( 0, "Error !! : Memory of the temporary buffer is not enough.\n" );
  }
  sha256.Finalize();

  return bSuccess;
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_CTR
