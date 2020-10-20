// ============================================================================
/*
 * @file MiracleTradeVerifyRequestSequence.cpp
 * @brief 交換するポケモンの不正チェックをするリクエストを投げるシーケンスです。
 * @date 2015.05.26
 */
// ============================================================================
#include "NetApp/MiracleTrade/source/RequestSequence/MiracleTradeVerifyRequestSequence.h"
#include <pml/include/item/item.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(RequestSequence)


MiracleTradeVerifyRequestSequence::MiracleTradeVerifyRequestSequence() :
#if defined( GF_PLATFORM_CTR )
  m_pNetAppPokemonValidationUtility( NULL ),
#endif // defined( GF_PLATFORM_CTR )
  m_pMiracleTradeWork( NULL ),
  m_pVerifyResponseListener( NULL ),
  m_FrameCount( 0 )
{
}


MiracleTradeVerifyRequestSequence::~MiracleTradeVerifyRequestSequence()
{
}

#if defined( GF_PLATFORM_CTR )

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE MiracleTradeVerifyRequestSequence::Update()
{
  gflnet2::ServerClient::REQUEST_SEQUENCE_STATE result = gflnet2::ServerClient::REQUEST_STATE_CONTINUE;

  switch( GetSequenceNo() )
  {
    // ---------------------------------------------------------------
    // 
    case SEQUENCE_INITIALIZE:
    {
      gfl2::heap::HeapBase* pHeap = m_pMiracleTradeWork->GetDeviceHeap();

      m_pNetAppPokemonValidationUtility = GFL_NEW( pHeap ) NetAppLib::Util::NetAppPokemonValidationUtility( pHeap );

      SetSequenceNo( SEQUENCE_VERIFY );
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_VERIFY:
    {
      pml::pokepara::CoreParam& selectPokemonCoreParam = m_pMiracleTradeWork->GetSelectPokemonCoreParam();

      if( item::ITEM_CheckPiece( selectPokemonCoreParam.GetItem() ) )
      {
        selectPokemonCoreParam.RemoveItem();
      }

      if( m_pNetAppPokemonValidationUtility->Verify( NetLib::Validation::PokemonValidation::E_GAME_MODE_TRADE_M, &selectPokemonCoreParam ) )
      {
        NetAppLib::Util::NetAppPokemonValidationUtility::VerifyResult eVerifyResult = m_pNetAppPokemonValidationUtility->GetVerifyResult();

        switch( eVerifyResult )
        {
          case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_OK:
          {
            gfl2::std::MemCopy( m_pNetAppPokemonValidationUtility->GetSignature(), m_pMiracleTradeWork->GetSignature(), NetApp::MiracleTrade::SIGNATRUE_SIZE );
            m_pMiracleTradeWork->SetSignatureSize( m_pNetAppPokemonValidationUtility->GetSignatureSize() );

            SetSequenceNo( SEQUENCE_NOTIFICATION_OK );
          }
          break;

          case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_NG:
          {
            SetSequenceNo( SEQUENCE_NOTIFICATION_NG );
          }
          break;

          case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_NET_ERROR:
          {
            SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
          }
          break;
        }
      }
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_NOTIFICATION_OK:
    {
      if( isNotification() )
      {
        if( m_pVerifyResponseListener )
        {
          m_pVerifyResponseListener->OnMiracleTradeVerifyOK();
        }
        SetSequenceNo( SEQUENCE_TERMINATE );
      }
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_NOTIFICATION_NG:
    {
      if( isNotification() )
      {
        if( m_pVerifyResponseListener )
        {
          m_pVerifyResponseListener->OnMiracleTradeVerifyNG();
        }
        SetSequenceNo( SEQUENCE_TERMINATE );
      }
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_NOTIFICATION_ERROR:
    {
      if( isNotification() )
      {
        if( m_pVerifyResponseListener )
        {
          m_pVerifyResponseListener->OnMiracleTradeVerifyError();
        }
        SetSequenceNo( SEQUENCE_TERMINATE );
      }
    }
    break;

    // ---------------------------------------------------------------
    // 
    case SEQUENCE_TERMINATE:
    {
      bool bCanDestroy = true;

      if( m_pNetAppPokemonValidationUtility )
      {
        bCanDestroy = m_pNetAppPokemonValidationUtility->CanDestroy();
      }

      if( bCanDestroy )
      {
        GFL_SAFE_DELETE( m_pNetAppPokemonValidationUtility );

        result = gflnet2::ServerClient::REQUEST_STATE_FINISH;
      }
    }
    break;

  }

  ++m_FrameCount;
  
  return result;
}

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE MiracleTradeVerifyRequestSequence::UpdateCancel()
{
  gflnet2::ServerClient::REQUEST_SEQUENCE_STATE result = gflnet2::ServerClient::REQUEST_STATE_CONTINUE;

  bool bCanDestroy = true;

  if( m_pNetAppPokemonValidationUtility )
  {
    bCanDestroy = m_pNetAppPokemonValidationUtility->CanDestroy();
  }

  if( bCanDestroy )
  {
    GFL_SAFE_DELETE( m_pNetAppPokemonValidationUtility );

    result = gflnet2::ServerClient::REQUEST_STATE_FINISH;
  }

  return result;
}

#elif defined( GF_PLATFORM_WIN32 )

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE MiracleTradeVerifyRequestSequence::Update()
{
  if( m_pVerifyResponseListener )
  {
    m_pVerifyResponseListener->OnMiracleTradeVerifyError();
  }
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE MiracleTradeVerifyRequestSequence::UpdateCancel()
{
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

#endif



GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)
