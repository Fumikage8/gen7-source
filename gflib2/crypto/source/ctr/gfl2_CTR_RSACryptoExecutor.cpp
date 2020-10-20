// ============================================================================
/*
 * @file gfl2_CTR_RSACryptoExecutor.cpp
 * @brief 署名データの作成と検証を行うクラスです。
 * @date 2015.03.12
 */
// ============================================================================
#ifdef GF_PLATFORM_CTR
#include <crypto/source/ctr/gfl2_CTR_RSACryptoExecutor.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <nn/crypto.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(ctr)


// --------------------------------------------------------------------------
/**
  * @brief  署名データの作成を行います。
  * @param  param 署名データを作成する為に必要なパラメータです
  * @retval true 成功
  * @retval false 失敗
  */
// --------------------------------------------------------------------------
bool CTR_RSACryptoExecutor::CreateSignature( RSACreateSignatureCommandParam& param )
{
  param.pSignature->ZeroClear();

  bool bSuccess = false;

  nn::crypto::RsaContext rsaPrivate;
  
  rsaPrivate.Initialize( param.pPrivateKey->GetData(), param.pPrivateKey->GetDataSize() );
  {
    size_t signatureSize;
  
    nn::Result result = rsaPrivate.SignSha256( &signatureSize, param.pSignature->GetData(), param.pSignature->GetBufferSize(), param.pSHA256Hash );
  
    if( result.IsSuccess() )
    {
      param.pSignature->SetDataSize( signatureSize );
    
      bSuccess = true;
    }
    else
    {
      GFL_PRINT( "Error : SignSha256 failed[0x%x].\n", result.GetPrintableBits() );
    }
  }
  rsaPrivate.Finalize();
  
  return bSuccess;
}


// --------------------------------------------------------------------------
/**
  * @brief  署名データの検証を行います。
  * @param  param 署名データを検証する為に必要なパラメータです
  * @retval true 成功
  * @retval false 失敗
  */
// --------------------------------------------------------------------------
bool CTR_RSACryptoExecutor::VerifySignature( RSAVerifySignatureCommandParam& param )
{
  bool bSuccess = false;

  nn::crypto::RsaContext rsaPublic;
  
  rsaPublic.Initialize( param.pPublicKey->GetData(), param.pPublicKey->GetDataSize() );
  {
    nn::Result result = rsaPublic.VerifySha256( param.pSHA256Hash, param.pSignature, param.signatureSize );

    if( result.IsSuccess() )
    {
      bSuccess = true;
    }
    else
    {
      GFL_PRINT( "Error : VerifySha256 failed[0x%x].\n", result.GetPrintableBits() );
    }
  }
  rsaPublic.Finalize();

  return bSuccess;
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_CTR
