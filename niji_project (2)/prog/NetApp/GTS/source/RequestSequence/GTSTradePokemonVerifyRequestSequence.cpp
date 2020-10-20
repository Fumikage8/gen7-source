// ============================================================================
/*
 * @file GTSTradePokemonVerifyRequestSequence.cpp
 * @brief 交換するポケモンの不正チェックをするリクエストを投げるシーケンスです。
 * @date 2015.04.27
 */
// ============================================================================
#include "NetApp/GTS/source/RequestSequence/GTSTradePokemonVerifyRequestSequence.h"
#include "NetApp/GTS/source/GTSUtility.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include <util/include/gfl2_std_string.h>
#ifdef GF_PLATFORM_CTR
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#endif // GF_PLATFORM_CTR
#include "NetStatic/NetLib/include/NetDefine.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


GTSTradePokemonVerifyRequestSequence::GTSTradePokemonVerifyRequestSequence() :
NetApp::GTS::RequestSequence::GTSRequestSequenceBase(),
#ifdef GF_PLATFORM_CTR
  m_MyDataStoreID( nn::nex::DataStoreConstants::INVALID_DATAID ),
#endif // GF_PLATFORM_CTR
  m_pTradePokemonVerifyResponseListener( NULL )
{
}


GTSTradePokemonVerifyRequestSequence::~GTSTradePokemonVerifyRequestSequence()
{
}


#if defined( GF_PLATFORM_CTR )

void GTSTradePokemonVerifyRequestSequence::updateSequence()
{
  switch( GetSequenceNo() )
  {
    // ---------------------------------------------------------------
    // ユーザーのパーソナルデータのアップロード
    case SEQUENCE_UPLOAD_PERSONAL_DATA:
    {
      gflnet2::nex::NexManager* pNexManager = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager);
      gflnet2::nex::NexDataStoreClient* pDataStoreClient = pNexManager->GetDataStoreClient();

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPersonalDataManager );
      NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData = pJoinFestaPersonalDataManager->GetMyData();
      NetAppLib::JoinFesta::JoinFestaPacketData joinFestaPacketData;
      pJoinFestaPersonalData->OutputPacketData( &joinFestaPacketData );

      Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();

      u64 personalDataStoreID = pMyStatus->GetPssPersonalDataStoreId();

      // 新規アップロードの場合
      if( personalDataStoreID == nn::nex::DataStoreConstants::INVALID_DATAID )
      {
        nn::nex::DataStorePostParam param;
        param.SetDataType( DATASTORE_DATATYPE_JF_PERSONAL );
        param.SetAccessPermission( nn::nex::DataStorePermission(nn::nex::DataStoreConstants::PERMISSION_PUBLIC) );
        param.SetUpdatePermission( nn::nex::DataStorePermission(nn::nex::DataStoreConstants::PERMISSION_PUBLIC) );
        param.SetMetaBinary( &joinFestaPacketData, sizeof( NetAppLib::JoinFesta::JoinFestaPacketData ) );
        pDataStoreClient->UploadMetaData( param );
      }
      else
      {
        nn::nex::qVector<nn::nex::DataStoreChangeMetaParam> list;
        nn::nex::DataStoreChangeMetaParam param;
        param.SetMetaBinary( &joinFestaPacketData, sizeof( NetAppLib::JoinFesta::JoinFestaPacketData ) );
        param.SetDataId( personalDataStoreID );
        param.SetModificationFlag( nn::nex::DataStoreConstants::MODIFICATION_FLAG_METABINARY | nn::nex::DataStoreConstants::MODIFICATION_FLAG_UPDATED_TIME );

        list.push_back( param );
        pDataStoreClient->CheageMetaData( list );
        list.clear();
      }

      SetSequenceNo( SEQUENCE_RESPONSE_WAIT );
    }
    break;

    // ---------------------------------------------------------------
    // 交換するポケモンの不正チェック
    case SEQUENCE_TRADE_POKEMON_VERIFY:
    {
      if( m_pNetAppPokemonValidationUtility == NULL )
      {
        gfl2::heap::HeapBase* pHeap = m_pGTSWork->GetDeviceHeap();

        m_pNetAppPokemonValidationUtility = GFL_NEW( pHeap ) NetAppLib::Util::NetAppPokemonValidationUtility( pHeap );
        
        NetApp::GTS::GTSUtility::SetupUploadGTSIndexData( *m_pGTSWork, m_MyDataStoreID );
      }

      const NetApp::GTS::GtsIndexData& uploadGTSIndexData = m_pGTSWork->GetUploadGTSIndexData();

      pml::pokepara::CoreParam& uploadPokemonParam = m_pGTSWork->GetSelectPokemonCoreParam();

      if( m_pNetAppPokemonValidationUtility->Verify( NetLib::Validation::PokemonValidation::E_GAME_MODE_GTS_UPLOAD_NOT_INDEX_CHECK_M, &uploadGTSIndexData, &uploadPokemonParam ) )
      {
        NetAppLib::Util::NetAppPokemonValidationUtility::VerifyResult eVerifyResult = m_pNetAppPokemonValidationUtility->GetVerifyResult();

        switch( eVerifyResult )
        {
          case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_OK:
          {
            NetApp::GTS::GtsSignature signature;

            signature.size = m_pNetAppPokemonValidationUtility->GetSignatureSize();

            gfl2::std::MemCopy( m_pNetAppPokemonValidationUtility->GetSignature(), signature.data, signature.size );

            m_pGTSWork->SetSignature( signature );

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
    // 成功を通知する(正常なポケモン)
    case SEQUENCE_NOTIFICATION_OK:
    {
      if( isNotification() )
      {
        if( m_pTradePokemonVerifyResponseListener )
        {
          m_pTradePokemonVerifyResponseListener->OnTradePokemonVerifyOK();
        }
        SetSequenceNo( SEQUENCE_TERMINATE );
      }
    }
    break;

    // ---------------------------------------------------------------
    // 不正ポケモンを通知する
    case SEQUENCE_NOTIFICATION_NG:
    {
      if( isNotification() )
      {
        if( m_pTradePokemonVerifyResponseListener )
        {
          m_pTradePokemonVerifyResponseListener->OnTradePokemonVerifyNG();
        }
        SetSequenceNo( SEQUENCE_TERMINATE );
      }
    }
    break;

    // ---------------------------------------------------------------
    // エラーを通知する
    case SEQUENCE_NOTIFICATION_ERROR:
    {
      if( isNotification() )
      {
        if( m_pTradePokemonVerifyResponseListener )
        {
          m_pTradePokemonVerifyResponseListener->OnTradePokemonVerifyError();
        }
        SetSequenceNo( SEQUENCE_TERMINATE );
      }
    }
    break;
  }
}


//-----------------------------------------------------------------------------
// gflnet2::nex::NexDataStoreClientListener
//-----------------------------------------------------------------------------
void GTSTradePokemonVerifyRequestSequence::OnNexDataStoreUploadSuccess( const nn::nex::qUnsignedInt64 dataId )
{
  GFL_PRINT( "GTSTradePokemonVerifyRequestSequence::OnNexDataStoreUploadSuccess() \n" );

  Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
  pMyStatus->SetPssPersonalDataStoreId( dataId );

  m_MyDataStoreID = dataId;

  SetSequenceNo( SEQUENCE_TRADE_POKEMON_VERIFY );
}


void GTSTradePokemonVerifyRequestSequence::OnNexDataStoreChangeMetaDataFinish( const nn::nex::qVector<nn::nex::qResult>& results )
{
  GFL_PRINT( "GTSTradePokemonVerifyRequestSequence::OnNexDataStoreChangeMetaDataFinish() \n" );

  Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();

  nn::nex::qResult ret = results.at( 0 );

  if( (nn::nex::qBool)ret )
  {
    m_MyDataStoreID = pMyStatus->GetPssPersonalDataStoreId();
    
    SetSequenceNo( SEQUENCE_TRADE_POKEMON_VERIFY );
  }
  else
  {
    u32 errorCode = gflnet2::nex::NexError::GetNetworkErrorCode( ret );

    switch( errorCode )
    {
      case  61203:  //アクセス権限がない
      case  61204:  //指定したデータIDが見つからない
      case  61207:  //期限切れ
      {
        // 無効なデータとして書き換え
        pMyStatus->SetPssPersonalDataStoreId( nn::nex::DataStoreConstants::INVALID_DATAID );
      }
      break;
    }

    NetLib::Error::NijiNetworkError error;
    error.SetResultNex( ret );
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

    SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
  }
}


void GTSTradePokemonVerifyRequestSequence::OnNexDataStoreClientError( const nn::nex::qResult& result, u32 errorCode )
{
  GFL_PRINT( "GTSTradePokemonVerifyRequestSequence::OnNexDataStoreClientError() \n" );
  
  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
  
}


#elif defined( GF_PLATFORM_WIN32 )

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GTSTradePokemonVerifyRequestSequence::Update()
{
  if( m_pTradePokemonVerifyResponseListener )
  {
    m_pTradePokemonVerifyResponseListener->OnTradePokemonVerifyError();
  }
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

#endif


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
