// ============================================================================
/*
 * @file CryptoTestProc.cpp
 * @brief 暗号化関連のライブラリのテストです。
 * @date 2015.03.10
 */
// ============================================================================
#if PM_DEBUG

#include "Test/CryptoTest/include/CryptoTestProc.h"
#include <system/include/GflUse.h>
#include <ui/include/gfl2_UI.h>
#include <crypto/include/gfl2_RandomKeyGenerator.h>
#include <System/include/ThreadPriority.h>
#include <System/include/nijiAllocator.h>


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(CryptoTest)

static const u8 PUBLIC_KEY[91] = { 0x30 , 0x59 , 0x30 , 0x0D , 0x06 , 0x09 , 0x2A , 0x86 , 0x48 , 0x86 , 0xF7 , 0x0D , 0x01 , 0x01 , 0x01 , 0x05 , 0x00 , 0x03 , 0x48 , 0x00 , 0x30 , 0x45 , 0x02 , 0x40 , 0x53 , 0xA2 , 0xEC , 0xB2 , 0x56 , 0xA1 , 0xD4 , 0xA7 , 0xB5 , 0x57 , 0x92 , 0xFE , 0xD2 , 0x04 , 0xC4 , 0xB3 , 0x3B , 0x27 , 0x95 , 0x35 , 0xF2 , 0x41 , 0xAF , 0x61 , 0x75 , 0x3C , 0x8B , 0xCB , 0x05 , 0x29 , 0x2B , 0x04 , 0x87 , 0xEE , 0x41 , 0x12 , 0x03 , 0xD0 , 0x1E , 0x74 , 0xC9 , 0xDE , 0xF4 , 0x11 , 0xE2 , 0xA5 , 0x4C , 0x0F , 0x20 , 0x20 , 0x9C , 0xDF , 0xDA , 0x2A , 0x32 , 0xBA , 0x52 , 0xEF , 0x92 , 0x29 , 0xAE , 0xBF , 0x1A , 0xC9 , 0x02 , 0x01 , 0x25 };
static const u8 PRIVATE_KEY[314] = { 0x30 , 0x82 , 0x01 , 0x36 , 0x02 , 0x01 , 0x00 , 0x02 , 0x40 , 0x53 , 0xA2 , 0xEC , 0xB2 , 0x56 , 0xA1 , 0xD4 , 0xA7 , 0xB5 , 0x57 , 0x92 , 0xFE , 0xD2 , 0x04 , 0xC4 , 0xB3 , 0x3B , 0x27 , 0x95 , 0x35 , 0xF2 , 0x41 , 0xAF , 0x61 , 0x75 , 0x3C , 0x8B , 0xCB , 0x05 , 0x29 , 0x2B , 0x04 , 0x87 , 0xEE , 0x41 , 0x12 , 0x03 , 0xD0 , 0x1E , 0x74 , 0xC9 , 0xDE , 0xF4 , 0x11 , 0xE2 , 0xA5 , 0x4C , 0x0F , 0x20 , 0x20 , 0x9C , 0xDF , 0xDA , 0x2A , 0x32 , 0xBA , 0x52 , 0xEF , 0x92 , 0x29 , 0xAE , 0xBF , 0x1A , 0xC9 , 0x02 , 0x01 , 0x25 , 0x02 , 0x40 , 0x2A , 0xF2 , 0xCC , 0x92 , 0xEE , 0x37 , 0x6D , 0x33 , 0x86 , 0xA2 , 0x97 , 0x97 , 0x9C , 0x47 , 0xA3 , 0x47 , 0x47 , 0xE3 , 0xE4 , 0xD6 , 0x83 , 0x52 , 0x29 , 0xA0 , 0xBF , 0xA9 , 0x78 , 0x37 , 0xD2 , 0x37 , 0xBC , 0x24 , 0x55 , 0x3B , 0x09 , 0x1A , 0x9A , 0x94 , 0x95 , 0x68 , 0xCC , 0x8B , 0x7F , 0x51 , 0x40 , 0x9A , 0x51 , 0xD0 , 0xE0 , 0xA5 , 0xDD , 0x18 , 0x58 , 0x3E , 0x0D , 0x2C , 0x52 , 0xF2 , 0x86 , 0x3E , 0x9C , 0x0D , 0xA1 , 0xF9 , 0x02 , 0x21 , 0x00 , 0x9C , 0x99 , 0xB4 , 0x02 , 0x74 , 0x47 , 0x06 , 0xD0 , 0x7A , 0x64 , 0x2B , 0x58 , 0xF9 , 0xA8 , 0xC4 , 0xFF , 0x0A , 0x64 , 0x4B , 0x23 , 0x18 , 0x50 , 0x76 , 0xF1 , 0xDA , 0x7C , 0xCA , 0x02 , 0x5E , 0x98 , 0x99 , 0x17 , 0x02 , 0x21 , 0x00 , 0x88 , 0xB9 , 0x2A , 0x7D , 0x6F , 0xFB , 0xF4 , 0xAF , 0xCE , 0xA1 , 0x06 , 0xAE , 0xE4 , 0x71 , 0xB1 , 0xD7 , 0xA3 , 0xA1 , 0xBE , 0x21 , 0x8F , 0x45 , 0xF2 , 0xF8 , 0xE4 , 0x64 , 0x9A , 0x4F , 0x12 , 0xC8 , 0x97 , 0x1F , 0x02 , 0x20 , 0x72 , 0x46 , 0xA5 , 0xF3 , 0xF3 , 0xFC , 0x7A , 0x98 , 0x21 , 0xF6 , 0x11 , 0xCB , 0x4E , 0x66 , 0x66 , 0x3D , 0x91 , 0xF6 , 0x28 , 0xFD , 0xEF , 0x25 , 0xF5 , 0xEE , 0xC2 , 0x08 , 0x09 , 0x08 , 0xA5 , 0xE4 , 0xFA , 0x17 , 0x02 , 0x20 , 0x30 , 0x09 , 0xB4 , 0xFB , 0xA3 , 0xE2 , 0xE7 , 0x44 , 0xB0 , 0x62 , 0x17 , 0x1A , 0xDA , 0xA4 , 0x7C , 0xC1 , 0x63 , 0x01 , 0x7A , 0x27 , 0x77 , 0x87 , 0x47 , 0x87 , 0xE1 , 0x8B , 0x21 , 0x75 , 0xBA , 0x7D , 0xD4 , 0x3B , 0x02 , 0x20 , 0x5D , 0xD2 , 0x11 , 0x90 , 0x97 , 0x95 , 0x08 , 0x7A , 0x3B , 0x33 , 0xB7 , 0x60 , 0x50 , 0x62 , 0xA2 , 0xF9 , 0xC6 , 0x43 , 0xD0 , 0xE2 , 0xED , 0x19 , 0x47 , 0xDB , 0x9F , 0x11 , 0xD3 , 0x75 , 0x99 , 0x05 , 0xDA , 0x34 };


AESCryptoTestEventListener::AESCryptoTestEventListener( gfl2::crypto::TemporaryBuffer* pEncryptDataBuffer ) :
  m_pEncryptDataBuffer( pEncryptDataBuffer )
{
}
AESCryptoTestEventListener::~AESCryptoTestEventListener(){}
void AESCryptoTestEventListener::OnEncryptSuccess( const void* pData, u32 dataSize )
{
  GFL_PRINT( "AESCryptoTestEventListener::OnEncryptSuccess\n" );
  m_pEncryptDataBuffer->ZeroClear();
  m_pEncryptDataBuffer->CopyData( pData, dataSize );
  u8* p = m_pEncryptDataBuffer->GetData();
  u32 size = m_pEncryptDataBuffer->GetDataSize();
  GFL_PRINT( "dataSize = %d\n", dataSize );
  for( u32 i = 0; i < size; ++i )
  {
    GFL_PRINT( "%d,", p[0] );
    p++;
  }
  GFL_PRINT( "\n" );
}
void AESCryptoTestEventListener::OnEncryptFailure()
{
  GFL_PRINT( "AESCryptoTestEventListener::OnEncryptFailure\n" );
}
void AESCryptoTestEventListener::OnDecryptSuccess( const void* pData, u32 dataSize )
{
  GFL_PRINT( "AESCryptoTestEventListener::OnDecryptSuccess\n" );
  u8* p = (u8*)pData;
  GFL_PRINT( "dataSize = %d\n", dataSize );
  for( u32 i = 0; i < dataSize; ++i )
  {
    GFL_PRINT( "%d,", p[0] );
    p++;
  }
  GFL_PRINT( "\n" );
}
void AESCryptoTestEventListener::OnDecryptFailure()
{
  GFL_PRINT( "AESCryptoTestEventListener::OnDecryptFailure\n" );
}

RSACryptoTestEventListener::RSACryptoTestEventListener( gfl2::crypto::Signature* pSignature ) :
  m_pSignature( pSignature )
{
}
RSACryptoTestEventListener::~RSACryptoTestEventListener(){}
void RSACryptoTestEventListener::OnCreateSignatureSuccess( const void* pSignature, u32 signatureSize )
{
  GFL_PRINT( "RSACryptoTestEventListener::OnCreateSignatureSuccess\n" );
  m_pSignature->ZeroClear();
  m_pSignature->CopyData( pSignature, signatureSize );
  u8* p = m_pSignature->GetData();
  u32 size = m_pSignature->GetDataSize();
  GFL_PRINT( "signatureSize = %d\n", signatureSize );
  for( u32 i = 0; i < size; ++i )
  {
    GFL_PRINT( "%d,", p[0] );
    p++;
  }
  GFL_PRINT( "\n" );
}
void RSACryptoTestEventListener::OnCreateSignatureFailure()
{
  GFL_PRINT( "RSACryptoTestEventListener::OnCreateSignatureFailure\n" );
}
void RSACryptoTestEventListener::OnVerifySignatureSuccess()
{
  GFL_PRINT( "RSACryptoTestEventListener::OnVerifySignatureSuccess\n" );
}
void RSACryptoTestEventListener::OnVerifySignatureFailure()
{
  GFL_PRINT( "RSACryptoTestEventListener::OnVerifySignatureFailure\n" );
}

SHA256CalcTestEventListener::SHA256CalcTestEventListener( gfl2::crypto::SHA256Hash* pSHA256Hash ) :
  m_pSHA256Hash( pSHA256Hash )
{
}
SHA256CalcTestEventListener::~SHA256CalcTestEventListener(){}
void SHA256CalcTestEventListener::OnCalcHashSuccess( const void* pSHA256Hash, u32 hashSize )
{
  GFL_PRINT( "SHA256CalcTestEventListener::OnCalcHashSuccess\n" );
  m_pSHA256Hash->ZeroClear();
  m_pSHA256Hash->CopyData( pSHA256Hash, hashSize );
  u8* p = m_pSHA256Hash->GetData();
  u32 size = m_pSHA256Hash->GetDataSize();
  GFL_PRINT( "hashSize = %d\n", hashSize );
  for( u32 i = 0; i < size; ++i )
  {
    GFL_PRINT( "%d,", p[0] );
    p++;
  }
  GFL_PRINT( "\n" );
}
void SHA256CalcTestEventListener::OnCalcHashFailure()
{
  GFL_PRINT( "SHA256CalcTestEventListener::OnCalcHashFailure\n" );
}

CryptoTestProc::CryptoTestProc() :
  m_pMemAllocator( NULL ),
  m_pDevAllocator( NULL ),
  m_pNijiAllocator( NULL ),
  m_pRenderingPipeLine( NULL ),
  m_pResFont( NULL ),
  mpDrawUtilText( NULL ),
  m_pAESCrypto( NULL ),
  m_pRSACrypto( NULL ),
  m_pSHA256Calculator( NULL ),
  m_pAESEventListener( NULL ),
  m_pRSAEventListener( NULL ),
  m_pSHAEventListener( NULL ),
  m_pEncryptDataBuffer( NULL ),
  m_pSHA256Hash( NULL ),
  m_pSignature( NULL ),
  m_TestMode( 0 ),
  m_pAButtonCommandText( NULL ),
  m_pXButtonCommandText( NULL )
{
}


CryptoTestProc::~CryptoTestProc()
{
}


gfl2::proc::Result CryptoTestProc::InitFunc( gfl2::proc::Manager* pManager )
{
  gfl2::heap::HeapBase* pMemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  // font
  {
 	  m_pMemAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pMemHeap );
	  m_pDevAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pDevHeap );
    m_pNijiAllocator = GFL_NEW(pDevHeap) System::nijiAllocator( pDevHeap );

    m_pRenderingPipeLine = GFL_NEW( pManager->GetHeap() ) gfl2::util::UtilTextRenderingPipeLine( m_pNijiAllocator, 512, 256 );
    m_pResFont = gfl2::str::CreateResFont( "data/app/test/fukushima_test/layout_sample_data/fonts/cbf_std_kujira.bffnt", pMemHeap, pDevHeap, pMemHeap );
		mpDrawUtilText = GFL_NEW(pManager->GetHeap()) gfl2::util::DrawUtilText(m_pMemAllocator, m_pDevAllocator);
    mpDrawUtilText->Initialize( m_pMemAllocator, m_pDevAllocator, m_pRenderingPipeLine->GetUtilTextRenderPath() );
	  mpDrawUtilText->SetFont( m_pResFont );
  }

  // crypto
  {
    bool bSuccess = gfl2::crypto::RandomKeyGenerator::GenerateRandomKey( m_aAESKey, sizeof( m_aAESKey ) );
    if( bSuccess )
    {
      GFL_PRINT( "GenerateRandomKey success\n" );
      for( u32 i = 0; i < 16; ++i )
      {
        GFL_PRINT( "%d,", m_aAESKey[i] );
      }
      GFL_PRINT( "\n" );
    }
    else
    {
      GFL_PRINT( "GenerateRandomKey failure\n" );
    }

    m_pAESCrypto = gfl2::crypto::IAESCrypto::CreateAESCrypto( pMemHeap, 1024, m_aAESKey, System::ThreadPriority::ASYNC_AES_CRYPTO_THREAD_PRIORITY );
    m_pRSACrypto = gfl2::crypto::IRSACrypto::CreateRSACrypto( pMemHeap, PUBLIC_KEY, sizeof( PUBLIC_KEY ), PRIVATE_KEY, sizeof( PRIVATE_KEY ), System::ThreadPriority::ASYNC_RSA_CRYPTO_THREAD_PRIORITY );
    m_pSHA256Calculator = gfl2::crypto::ISHA256Calculator::CreateSHA256Calculator( pMemHeap, 256, System::ThreadPriority::ASYNC_SHA256_CRYPTO_THREAD_PRIORITY );

    m_pEncryptDataBuffer = GFL_NEW( pMemHeap ) gfl2::crypto::TemporaryBuffer( pMemHeap, 1024 );
    m_pSHA256Hash = GFL_NEW( pMemHeap ) gfl2::crypto::SHA256Hash( pMemHeap, 256 );
    m_pSignature = GFL_NEW( pMemHeap ) gfl2::crypto::Signature( pMemHeap, 128 );
    
    m_pAESEventListener = GFL_NEW( pMemHeap ) AESCryptoTestEventListener( m_pEncryptDataBuffer );
    m_pRSAEventListener = GFL_NEW( pMemHeap ) RSACryptoTestEventListener( m_pSignature );
    m_pSHAEventListener = GFL_NEW( pMemHeap ) SHA256CalcTestEventListener( m_pSHA256Hash );

    for( int i = 0; i < 128; ++i )
    {
      m_aTestData[i] = (u8)i;
    }
  }

  // debug text
  {
    m_pAButtonCommandText = GFL_NEW( pMemHeap ) gfl2::str::StrBuf( 64, pMemHeap );
    m_pXButtonCommandText = GFL_NEW( pMemHeap ) gfl2::str::StrBuf( 64, pMemHeap );
    SetCommandText( m_TestMode );
  }

  return gfl2::proc::RES_FINISH;
}


gfl2::proc::Result CryptoTestProc::EndFunc( gfl2::proc::Manager* pManager )
{
  // debug text
  {
    GFL_SAFE_DELETE( m_pXButtonCommandText );
    GFL_SAFE_DELETE( m_pAButtonCommandText );
  }

  // crypto
  {
    GFL_SAFE_DELETE( m_pSHAEventListener );
    GFL_SAFE_DELETE( m_pRSAEventListener );
    GFL_SAFE_DELETE( m_pAESEventListener );

    GFL_SAFE_DELETE( m_pSignature );
    GFL_SAFE_DELETE( m_pSHA256Hash );
    GFL_SAFE_DELETE( m_pEncryptDataBuffer );
    
    GFL_SAFE_DELETE( m_pSHA256Calculator );
    GFL_SAFE_DELETE( m_pRSACrypto );
    GFL_SAFE_DELETE( m_pAESCrypto );
  }

  // font
  {
	  mpDrawUtilText->Terminate();
		GFL_SAFE_DELETE(mpDrawUtilText);

    void* pFontBuffer = m_pResFont->RemoveResource();
	  GflHeapFreeMemory( pFontBuffer );
	  GFL_SAFE_DELETE( m_pResFont );

    GFL_SAFE_DELETE( m_pRenderingPipeLine );

    GFL_SAFE_DELETE( m_pNijiAllocator );
    GFL_SAFE_DELETE( m_pDevAllocator );
	  GFL_SAFE_DELETE( m_pMemAllocator );
  }

  return gfl2::proc::RES_FINISH;
}


gfl2::proc::Result CryptoTestProc::UpdateFunc( gfl2::proc::Manager* pManager )
{
  gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();

  gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);
  
  DisplayPrint();

  m_pAESCrypto->Update();
  m_pRSACrypto->Update();
  m_pSHA256Calculator->Update();

  switch( m_TestMode )
  {
    case 0:
      AESTest();
    break;

    case 1:
      RSATest();
    break;
    
    case 2:
      SHATest();
    break;
  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
  {
    m_TestMode = ( ++m_TestMode > 2 ) ? 0 : m_TestMode;
    SetCommandText( m_TestMode );
  }

  if( !m_pAESCrypto->IsBusy() && !m_pRSACrypto->IsBusy() && !m_pSHA256Calculator->IsBusy() )
  {
    if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
    {
      return gfl2::proc::RES_FINISH;
    }
  }

  return gfl2::proc::RES_CONTINUE;
}


void CryptoTestProc::DisplayPrint()
{
  mpDrawUtilText->ClearDrawText();

	mpDrawUtilText->SetTextScale( 1.0f, 1.0f );
  mpDrawUtilText->DrawText( 0,  30, L"B Exit" );
  mpDrawUtilText->DrawText( 0,  60, L"Y Change mode" );

  mpDrawUtilText->DrawText( 0,  90, m_pAButtonCommandText->GetPtr() );
  mpDrawUtilText->DrawText( 0, 120, m_pXButtonCommandText->GetPtr() );

}


void CryptoTestProc::SetCommandText( u32 mode )
{
  switch( mode )
  {
    case 0:
      m_pAButtonCommandText->SetStr( L"A EncryptAsync" );
      m_pXButtonCommandText->SetStr( L"X DecryptAsync" );
    break;
    
    case 1:
      m_pAButtonCommandText->SetStr( L"A CreateSignatureAsync" );
      m_pXButtonCommandText->SetStr( L"X VerifySignatureAsync" );
    break;

    case 2:
      m_pAButtonCommandText->SetStr( L"A CalcHashAsync" );
      m_pXButtonCommandText->SetStr( L"X CalcHash" );
    break;
  }
}


void CryptoTestProc::AESTest()
{
  gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    bool isStartSuccess = m_pAESCrypto->EncryptAsync( m_pAESEventListener, m_aTestData, sizeof( m_aTestData ) );
    if( isStartSuccess )
    {
      GFL_PRINT( "EncryptAsync start success\n" );
    }
    else
    {
      GFL_PRINT( "EncryptAsync start failure\n" );
    }
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
  {
    if( m_pEncryptDataBuffer->GetDataSize() > 0 )
    {
      bool isStartSuccess = m_pAESCrypto->DecryptAsync( m_pAESEventListener, m_pEncryptDataBuffer->GetData(), m_pEncryptDataBuffer->GetDataSize() );
      if( isStartSuccess )
      {
        GFL_PRINT( "DecryptAsync start success\n" );
      }
      else
      {
        GFL_PRINT( "DecryptAsync start failure\n" );
      }
    }
  }
}


void CryptoTestProc::RSATest()
{
  gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    if( m_pSHA256Hash->GetDataSize() > 0 )
    {
      bool isStartSuccess = m_pRSACrypto->CreateSignatureAsync( m_pRSAEventListener, m_pSHA256Hash->GetData() );
      if( isStartSuccess )
      {
        GFL_PRINT( "CreateSignatureAsync start success\n" );
      }
      else
      {
        GFL_PRINT( "CreateSignatureAsync start failure\n" );
      }
    }
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
  {
    if( m_pSHA256Hash->GetDataSize() > 0 && m_pSignature->GetDataSize() > 0 )
    {
      bool isStartSuccess = m_pRSACrypto->VerifySignatureAsync( m_pRSAEventListener, m_pSHA256Hash->GetData(), m_pSignature->GetData(), m_pSignature->GetDataSize() );
      if( isStartSuccess )
      {
        GFL_PRINT( "VerifySignatureAsync start success\n" );
      }
      else
      {
        GFL_PRINT( "VerifySignatureAsync start failure\n" );
      }
    }
  }
}


void CryptoTestProc::SHATest()
{
  gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    bool isStartSuccess = m_pSHA256Calculator->CalcHashAsync( m_pSHAEventListener, m_aTestData, sizeof( m_aTestData ) );
    if( isStartSuccess )
    {
      GFL_PRINT( "CalcHashAsync start success\n" );
    }
    else
    {
      GFL_PRINT( "CalcHashAsync start failure\n" );
    }
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
  {
    gfl2::crypto::SHA256Hash* pSHA256Hash = m_pSHA256Calculator->CalcHash( m_aTestData, sizeof( m_aTestData ) );
    if( pSHA256Hash != NULL )
    {
      GFL_PRINT( "CalcHash success\n" );
      m_pSHA256Hash->CopyData( pSHA256Hash->GetData(), pSHA256Hash->GetDataSize() );
    }
    else
    {
      GFL_PRINT( "CalcHash failure\n" );
    }
  }
}


void CryptoTestProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  switch( displayNo )
  {
    case gfl2::gfx::CtrDisplayNo::LEFT:
    case gfl2::gfx::CtrDisplayNo::RIGHT:
    {
      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
      u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

      m_pRenderingPipeLine->SetRenderTarget( pBackBuffer );
      m_pRenderingPipeLine->SetDepthStencil( pDepthBuffer );
      m_pRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

      m_pRenderingPipeLine->Execute();
    }
    break;
  }

}




GFL_NAMESPACE_END(CryptoTest)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
