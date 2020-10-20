// ============================================================================
/*
 * @file gfl2_RandomKeyGenerator.cpp
 * @brief ランダムなキーを生成するクラスです。
 * @date 2015.03.11
 */
// ============================================================================
#include <crypto/include/gfl2_RandomKeyGenerator.h>
#ifdef GF_PLATFORM_CTR
#include <nn/crypto.h>
#endif // GF_PLATFORM_CTR

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)


// --------------------------------------------------------------------------
/**
  * @brief  ランダムなキーを生成します。
  * @param  pRandomKey 生成したランダムなキーを格納するバッファのポインタ
  * @param  keySize 生成したランダムなキーを格納するバッファのサイズ
  * @retval true ランダムなキーの生成に成功
  * @retval false ランダムなキーの生成に失敗
  */
// --------------------------------------------------------------------------
bool RandomKeyGenerator::GenerateRandomKey( void* pRandomKey, u32 keySize )
{
  bool bSuccess = false;
#ifdef GF_PLATFORM_CTR
  nn::crypto::GenerateRandomBytes( pRandomKey, keySize );
  bSuccess = true;
#endif
  return bSuccess;
}


GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)
