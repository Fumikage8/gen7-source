// ============================================================================
/*
 * @file CryptoTestProc.h
 * @brief 暗号化関連のライブラリのテストです。
 * @date 2015.03.10
 */
// ============================================================================
#if PM_DEBUG

#if !defined( CRYPTOTESTPROC_H_INCLUDED )
#define CRYPTOTESTPROC_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Font.h>
#include <util/include/gfl2_UtilTextRenderingPipeLine.h>
#include <GameSys/include/GameProc.h>

#include <crypto/include/gfl2_IAESCrypto.h>
#include <crypto/include/gfl2_IRSACrypto.h>
#include <crypto/include/gfl2_ISHA256Calculator.h>

#include <crypto/include/gfl2_IAESCryptoEventListener.h>
#include <crypto/include/gfl2_IRSACryptoEventListener.h>
#include <crypto/include/gfl2_ISHA256CalcEventListener.h>

#include <str/include/gfl2_StrBuf.h>


GFL_NAMESPACE_BEGIN(System)
class nijiAllocator;
GFL_NAMESPACE_END(System)


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(CryptoTest)


class AESCryptoTestEventListener : public gfl2::crypto::IAESCryptoEventListener
{
public:
  AESCryptoTestEventListener( gfl2::crypto::TemporaryBuffer* pEncryptDataBuffer );
  virtual ~AESCryptoTestEventListener();
  virtual void OnEncryptSuccess( const void* pData, u32 dataSize );
  virtual void OnEncryptFailure();
  virtual void OnDecryptSuccess( const void* pData, u32 dataSize );
  virtual void OnDecryptFailure();
private:
  gfl2::crypto::TemporaryBuffer* m_pEncryptDataBuffer;
};


class RSACryptoTestEventListener : public gfl2::crypto::IRSACryptoEventListener
{
public:
  RSACryptoTestEventListener( gfl2::crypto::Signature* pSignature );
  virtual ~RSACryptoTestEventListener();
  virtual void OnCreateSignatureSuccess( const void* pSignature, u32 signatureSize );
  virtual void OnCreateSignatureFailure();
  virtual void OnVerifySignatureSuccess();
  virtual void OnVerifySignatureFailure();
private:
  gfl2::crypto::Signature* m_pSignature;
};


class SHA256CalcTestEventListener : public gfl2::crypto::ISHA256CalcEventListener
{
public:
  SHA256CalcTestEventListener( gfl2::crypto::SHA256Hash* pSHA256Hash );
  virtual ~SHA256CalcTestEventListener();
  virtual void OnCalcHashSuccess( const void* pSHA256Hash, u32 hashSize );
  virtual void OnCalcHashFailure();
private:
  gfl2::crypto::SHA256Hash* m_pSHA256Hash;
};


class CryptoTestProc : public GameSys::GameProc
{
	GFL_FORBID_COPY_AND_ASSIGN( CryptoTestProc );

public:
  CryptoTestProc();
  virtual~CryptoTestProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );

private:
  void DisplayPrint();
  void SetCommandText( u32 mode );
  void AESTest();
  void RSATest();
  void SHATest();

private:

  gfl2::heap::NwAllocator*		                      m_pMemAllocator;
	gfl2::heap::NwAllocator*		                      m_pDevAllocator;
  System::nijiAllocator*                            m_pNijiAllocator;
	gfl2::util::UtilTextRenderingPipeLine*		        m_pRenderingPipeLine;
  gfl2::str::ResFont*				                        m_pResFont;
	gfl2::util::DrawUtilText*                         mpDrawUtilText;

  gfl2::crypto::IAESCrypto*                         m_pAESCrypto;
  gfl2::crypto::IRSACrypto*                         m_pRSACrypto;
  gfl2::crypto::ISHA256Calculator*                  m_pSHA256Calculator;
  AESCryptoTestEventListener*                       m_pAESEventListener;
  RSACryptoTestEventListener*                       m_pRSAEventListener;
  SHA256CalcTestEventListener*                      m_pSHAEventListener;
  gfl2::crypto::TemporaryBuffer*                    m_pEncryptDataBuffer;
  gfl2::crypto::SHA256Hash*                         m_pSHA256Hash;
  gfl2::crypto::Signature*                          m_pSignature;
  u8                                                m_aTestData[128];
  u8                                                m_aAESKey[16];

  u32                                               m_TestMode;
  gfl2::str::StrBuf*                                m_pAButtonCommandText;
  gfl2::str::StrBuf*                                m_pXButtonCommandText;

};


GFL_NAMESPACE_END(CryptoTest)
GFL_NAMESPACE_END(Test)

#endif // CRYPTOTESTPROC_H_INCLUDED

#endif // PM_DEBUG
