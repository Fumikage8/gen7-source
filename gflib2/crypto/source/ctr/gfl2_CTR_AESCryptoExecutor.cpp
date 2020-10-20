// ============================================================================
/*
 * @file gfl2_CTR_AESCryptoExecutor.cpp
 * @brief AES暗号方式による暗号化、復号化を行うクラスです。
 * @date 2015.03.12
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#include <crypto/source/ctr/gfl2_CTR_AESCryptoExecutor.h>
#include <debug/include/gfl2_Assert.h>
#include <nn/crypto.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(ctr)


// --------------------------------------------------------------------------
/**
  * @brief  暗号化処理を行います。
  * @param  pTemporaryBuffer 暗号化後のデータを格納する為のバッファです
  * @param  pData 暗号化したいデータです
  * @param  dataSize 暗号化したいデータのサイズです
  * @param  pKey 暗号化復号化キー
  * @retval true 成功
  * @retval false 失敗
  */
// --------------------------------------------------------------------------
bool CTR_AESCryptoExecutor::Encrypt( gfl2::crypto::TemporaryBuffer* pTemporaryBuffer, const void* pData, u32 dataSize, const void* pKey )
{
  u32 encryptDataSize = dataSize + nn::crypto::GENERATE_HEADER_SIZE;
  if( encryptDataSize <= pTemporaryBuffer->GetBufferSize() )
  {
    pTemporaryBuffer->ZeroClear();
    nn::crypto::EncryptAndGenerateAes128Ccm( pTemporaryBuffer->GetData(), pData, dataSize, pKey );
    pTemporaryBuffer->SetDataSize( encryptDataSize );

    return true;
  }

  // テンポラリバッファのメモリが不足しているのでもう少し大きいサイズを指定する必要がある
  // IAESCrypto::CreateAESCrypto( pHeap, temporaryBufferSize ) temporaryBufferSize この値
  GFL_ASSERT_MSG( 0, "Error !! : Memory of the temporary buffer is not enough.\n" );

  return false;
}


// --------------------------------------------------------------------------
/**
  * @brief  復号化処理を行います。
  * @param  pTemporaryBuffer 復号化後のデータを格納する為のバッファです
  * @param  pData 復号化したいデータです(暗号化されたデータです)
  * @param  dataSize 復号化したいデータのサイズです(暗号化されたデータのサイズです)
  * @param  pKey 暗号化復号化キー
  * @retval true 成功
  * @retval false 失敗
  */
// --------------------------------------------------------------------------
bool CTR_AESCryptoExecutor::Decrypt( gfl2::crypto::TemporaryBuffer* pTemporaryBuffer, const void* pData, u32 dataSize, const void* pKey )
{
  u32 decryptDataSize = dataSize - nn::crypto::GENERATE_HEADER_SIZE;
  if( decryptDataSize <= pTemporaryBuffer->GetBufferSize() )
  {
    pTemporaryBuffer->ZeroClear();
    nn::crypto::DecryptAndVerifyAes128Ccm( pTemporaryBuffer->GetData(), pData, dataSize, pKey );
    pTemporaryBuffer->SetDataSize( decryptDataSize );

    return true;
  }

  // テンポラリバッファのメモリが不足しているのでもう少し大きいサイズを指定する必要がある
  // IAESCrypto::CreateAESCrypto( pHeap, temporaryBufferSize ) temporaryBufferSize この値
  GFL_ASSERT_MSG( 0, "Error !! : Memory of the temporary buffer is not enough.\n" );

  return false;
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_CTR
