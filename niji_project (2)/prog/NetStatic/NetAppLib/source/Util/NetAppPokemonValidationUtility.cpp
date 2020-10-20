// ============================================================================
/*
 * @file NetAppPokemonValidationUtility.cpp
 * @brief ポケモンの不正チェックユーティリティ
 * @date 2016.01.14
 * @author endo_akira
 */
// ============================================================================
#if defined( GF_PLATFORM_CTR )
#include "NetStatic/NetAppLib/include/Util/NetAppPokemonValidationUtility.h"
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include <util/include/gfl2_std_string.h>
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)



NetAppPokemonValidationUtility::NetAppPokemonValidationUtility( gfl2::heap::HeapBase* pHeap ) :
  m_pHeap( pHeap ),
  m_pPokemonValidation( NULL ),
  m_aeVerifyResult(),
  m_eState( STATE_INITIALIZE ),
  m_Signature(),
  m_SignatureSize( 0 ),
  m_bVerifyResultSignature( false ),
  m_pServerErrorListener( NULL )
#if PM_DEBUG
  , m_resultCode()
#endif
{
}


NetAppPokemonValidationUtility::~NetAppPokemonValidationUtility()
{
  GFL_SAFE_DELETE( m_pPokemonValidation );
}


bool NetAppPokemonValidationUtility::Verify(
  NetLib::Validation::PokemonValidation::GameMode eGameMode,
  const pml::pokepara::CoreParam* pPokemons,
  u32 pokemonCount )
{
  return verify( eGameMode, NULL, pPokemons, pokemonCount );
}


bool NetAppPokemonValidationUtility::Verify(
  NetLib::Validation::PokemonValidation::GameMode eGameMode,
  const NetApp::GTS::GtsIndexData* pGTSIndexData,
  const pml::pokepara::CoreParam* pPokemon )
{
  return verify( eGameMode, pGTSIndexData, pPokemon, 1 );
}

#if PM_DEBUG
bool NetAppPokemonValidationUtility::Verify(
  NetLib::Validation::PokemonValidation::GameMode eGameMode,
  pml::PokeParty* pParty )
{
  bool bComplete = false;

  switch( m_eState )
  {
    // -----------------------------------------------------------------------------------
    case STATE_INITIALIZE:
    {
      setVerifyResult( VERIFY_RESULT_NG );

      GFL_SAFE_DELETE( m_pPokemonValidation );

      m_pPokemonValidation = GFL_NEW( m_pHeap ) NetLib::Validation::PokemonValidation();
      
      m_pPokemonValidation->SetPokemonValidationListener( this );

      m_pPokemonValidation->Initialize(
        m_pHeap,
        m_pHeap,
        GFL_SINGLETON_INSTANCE( gflnet2::nex::NexManager )->GetAuthenticationToken(),
        eGameMode );

      u32 pokemonCount = pParty->GetMemberCount();

      for( u32 i = 0; i < pokemonCount; ++i )
      {
        NetApp::GTS::GtsIndexData gtsIndexData;
        NetApp::GTS::GtsSerializePokemonParam gtsSerializePokemonParam;

        gfl2::std::MemClear( &gtsIndexData, sizeof( NetApp::GTS::GtsIndexData ) );
        gfl2::std::MemClear( &gtsSerializePokemonParam, sizeof( NetApp::GTS::GtsSerializePokemonParam ) );

        pParty->GetMemberPointer(i)->Serialize_Core(gtsSerializePokemonParam.data);

        m_pPokemonValidation->SetPokemon( i, &gtsIndexData, &gtsSerializePokemonParam );
      }

      if( m_pPokemonValidation->CheckPokemonASync( m_pHeap, pokemonCount ) )
      {
        setState( STATE_VERIFY_WAIT );
      }
      else
      {
        setVerifyResult( VERIFY_RESULT_NET_ERROR );
        setState( STATE_COMPLETE );
      }
    }
    break;

    // -----------------------------------------------------------------------------------
    case STATE_VERIFY_WAIT:
    {
      // リスナーの返答待ち
    }
    break;

    // -----------------------------------------------------------------------------------
    case STATE_COMPLETE:
    {
      if( CanDestroy() )
      {
        GFL_SAFE_DELETE( m_pPokemonValidation );

        bComplete = true;

        setState( STATE_INITIALIZE );
      }
    }
    break;
  }

  return bComplete;
}
#endif


bool NetAppPokemonValidationUtility::verify(
  NetLib::Validation::PokemonValidation::GameMode eGameMode,
  const NetApp::GTS::GtsIndexData* pGTSIndexData,
  const pml::pokepara::CoreParam* pPokemons,
  u32 pokemonCount )
{
  bool bComplete = false;

  switch( m_eState )
  {
    // -----------------------------------------------------------------------------------
    case STATE_INITIALIZE:
    {
      setVerifyResult( VERIFY_RESULT_NG );

      GFL_SAFE_DELETE( m_pPokemonValidation );

      m_pPokemonValidation = GFL_NEW( m_pHeap ) NetLib::Validation::PokemonValidation();
      
      m_pPokemonValidation->SetPokemonValidationListener( this );

      m_pPokemonValidation->Initialize(
        m_pHeap,
        m_pHeap,
        GFL_SINGLETON_INSTANCE( gflnet2::nex::NexManager )->GetAuthenticationToken(),
        eGameMode );

      for( u32 i = 0; i < pokemonCount; ++i )
      {
        NetApp::GTS::GtsIndexData gtsIndexData;
        NetApp::GTS::GtsSerializePokemonParam gtsSerializePokemonParam;

        gfl2::std::MemClear( &gtsIndexData, sizeof( NetApp::GTS::GtsIndexData ) );
        gfl2::std::MemClear( &gtsSerializePokemonParam, sizeof( NetApp::GTS::GtsSerializePokemonParam ) );

        if( pGTSIndexData )
        {
          gfl2::std::MemCopy( pGTSIndexData, &gtsIndexData, sizeof( NetApp::GTS::GtsIndexData ) );
        }

        pPokemons[i].Serialize_Core( gtsSerializePokemonParam.data );

        m_pPokemonValidation->SetPokemon( i, &gtsIndexData, &gtsSerializePokemonParam );
      }

      if( m_pPokemonValidation->CheckPokemonASync( m_pHeap, pokemonCount ) )
      {
        setState( STATE_VERIFY_WAIT );
      }
      else
      {
        setVerifyResult( VERIFY_RESULT_NET_ERROR );
        setState( STATE_COMPLETE );
      }
    }
    break;

    // -----------------------------------------------------------------------------------
    case STATE_VERIFY_WAIT:
    {
      // リスナーの返答待ち
    }
    break;

    // -----------------------------------------------------------------------------------
    case STATE_COMPLETE:
    {
      if( CanDestroy() )
      {
        GFL_SAFE_DELETE( m_pPokemonValidation );

        bComplete = true;

        setState( STATE_INITIALIZE );
      }
    }
    break;
  }

  return bComplete;
}


bool NetAppPokemonValidationUtility::VerifySignature(
  bool& bVerifyResult,
  NetLib::Validation::PokemonValidation::GameMode eGameMode,
  u8* pSignature,
  u32 signatureSize,
  const pml::pokepara::CoreParam* pPokemons,
  u32 pokemonCount )
{
  bool bComplete = false;

  switch( m_eState )
  {
    // -----------------------------------------------------------------------------------
    case STATE_INITIALIZE:
    {
      m_bVerifyResultSignature = false;

      GFL_SAFE_DELETE( m_pPokemonValidation );

      m_pPokemonValidation = GFL_NEW( m_pHeap ) NetLib::Validation::PokemonValidation();
      
      m_pPokemonValidation->SetPokemonValidationListener( this );

      m_pPokemonValidation->Initialize(
        m_pHeap,
        m_pHeap,
        GFL_SINGLETON_INSTANCE( gflnet2::nex::NexManager )->GetAuthenticationToken(),
        eGameMode );

      for( u32 i = 0; i < pokemonCount; ++i )
      {
        NetApp::GTS::GtsSerializePokemonParam gtsSerializePokemonParam;

        gfl2::std::MemClear( &gtsSerializePokemonParam, sizeof( NetApp::GTS::GtsSerializePokemonParam ) );

        pPokemons[i].Serialize_Core( gtsSerializePokemonParam.data );

        m_pPokemonValidation->SetPokemon( i, NULL, &gtsSerializePokemonParam );
      }

      setState( STATE_VERIFY_WAIT );
    }
    break;

    // -----------------------------------------------------------------------------------
    case STATE_VERIFY_WAIT:
    {
      m_bVerifyResultSignature = m_pPokemonValidation->CheckRsaValidation( pokemonCount, pSignature, signatureSize );

      setState( STATE_COMPLETE );
    }
    break;

    // -----------------------------------------------------------------------------------
    case STATE_COMPLETE:
    {
      if( CanDestroy() )
      {
        GFL_SAFE_DELETE( m_pPokemonValidation );

        bVerifyResult = m_bVerifyResultSignature;

        bComplete = true;

        setState( STATE_INITIALIZE );
      }
    }
    break;
  }

  return bComplete;
}


void NetAppPokemonValidationUtility::setVerifyResult( u32 index, VerifyResult eVerifyResult )
{
  m_aeVerifyResult[ index ] = eVerifyResult;
}


void NetAppPokemonValidationUtility::setVerifyResult( VerifyResult eVerifyResult )
{
  for( s32 i = 0; i < NetLib::Validation::PokemonValidation::POKEMON_NUM; ++i )
  {
    m_aeVerifyResult[i] = eVerifyResult;
  }
}


void NetAppPokemonValidationUtility::setState( State eState )
{
  m_eState = eState;
}


NetAppPokemonValidationUtility::VerifyResult NetAppPokemonValidationUtility::GetVerifyResult( u32 index ) const
{
  return m_aeVerifyResult[ index ];
}


bool NetAppPokemonValidationUtility::CanDestroy()
{
  bool bCanDestory = true;

  if( m_pPokemonValidation )
  {
    if( m_pPokemonValidation->IsConnecting() )
    {
      m_pPokemonValidation->CancelConnecting();

      bCanDestory = false;
    }
  }

  return bCanDestory;
}


u8* NetAppPokemonValidationUtility::GetSignature()
{
  return m_Signature;
}


u32 NetAppPokemonValidationUtility::GetSignatureSize()
{
  return m_SignatureSize;
}


void NetAppPokemonValidationUtility::OnPokemonValidationSuccess( const nn::nex::qInt32* checkResult, const u32 resultNum, const u8* certData, const u32 certDataSize )
{
  for( u32 i = 0; i < resultNum; ++i )
  {
    switch( checkResult[i] )
    {
      case NetLib::Validation::RESULT_VALID:
        setVerifyResult( i, VERIFY_RESULT_OK );
      break;

      case NetLib::Validation::RESULT_INVALID:
        setVerifyResult( i, VERIFY_RESULT_NG );
      break;
      
      case NetLib::Validation::RESULT_RSA_ERROR:
        setVerifyResult( i, VERIFY_RESULT_NG );
      break;
    }
#if PM_DEBUG
    m_resultCode[i] = checkResult[i];
#endif
  }

  gfl2::std::MemCopy( certData, m_Signature, certDataSize );
  m_SignatureSize = certDataSize;

  setState( STATE_COMPLETE );
}


void NetAppPokemonValidationUtility::OnPokemonValidationError( const s32 httpStatus )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultValidation5xx( httpStatus );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  //@fix GFNMCat[#5699 P2P交換（インターネット）の不正チェック時にエラーアプレットを表示し続けると相手を擬似的にはまり状態にできる]
  if( m_pServerErrorListener != NULL )
  {
    m_pServerErrorListener->OnServerError();
  }

  setVerifyResult( VERIFY_RESULT_NET_ERROR );
  setState( STATE_COMPLETE );
}


void NetAppPokemonValidationUtility::OnPokemonValidationError( const nn::nex::qResult &result, u32 errorCode )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  //@fix GFNMCat[#5699 P2P交換（インターネット）の不正チェック時にエラーアプレットを表示し続けると相手を擬似的にはまり状態にできる]
  if( m_pServerErrorListener != NULL )
  {
    m_pServerErrorListener->OnServerError();
  }

  setVerifyResult( VERIFY_RESULT_NET_ERROR );
  setState( STATE_COMPLETE );
}


void NetAppPokemonValidationUtility::OnPokemonValidationCanceled()
{
}


GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)
#endif // defined( GF_PLATFORM_CTR )
