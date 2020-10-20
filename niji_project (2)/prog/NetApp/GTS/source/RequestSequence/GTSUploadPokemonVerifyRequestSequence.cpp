// ============================================================================
/*
 * @file GTSUploadPokemonVerifyRequestSequence.cpp
 * @brief アップロードするポケモンの不正チェックをするリクエストを投げるシーケンスです。
 * @date 2015..04.27
 */
// ============================================================================
#include "NetApp/GTS/source/RequestSequence/GTSUploadPokemonVerifyRequestSequence.h"
#include "NetApp/GTS/source/GTSUtility.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#ifdef GF_PLATFORM_CTR
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#endif // GF_PLATFORM_CTR
#include <util/include/gfl2_std_string.h>
#include "NetStatic/NetLib/include/NetDefine.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


GTSUploadPokemonVerifyRequestSequence::GTSUploadPokemonVerifyRequestSequence() :
NetApp::GTS::RequestSequence::GTSRequestSequenceBase(),
#ifdef GF_PLATFORM_CTR
  m_MyDataStoreID( nn::nex::DataStoreConstants::INVALID_DATAID ),
#endif // GF_PLATFORM_CTR
  m_pUploadPokemonVerifyResponseListener( NULL ),
  m_pDummyPokemon( NULL )
{
}


GTSUploadPokemonVerifyRequestSequence::~GTSUploadPokemonVerifyRequestSequence()
{
  GFL_SAFE_DELETE( m_pDummyPokemon );
}


#if defined( GF_PLATFORM_CTR )


void GTSUploadPokemonVerifyRequestSequence::updateSequence()
{
  switch( GetSequenceNo() )
  {
    // ---------------------------------------------------------------
    // 自分のパーソナルデータのアップロード
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
    // アップロードするポケモンの不正チェック
    case SEQUENCE_UPLOAD_POKEMON_VERIFY:
    {
      if( m_pNetAppPokemonValidationUtility == NULL )
      {
        gfl2::heap::HeapBase* pHeap = m_pGTSWork->GetDeviceHeap();

        m_pNetAppPokemonValidationUtility = GFL_NEW( pHeap ) NetAppLib::Util::NetAppPokemonValidationUtility( pHeap );

        NetApp::GTS::GTSUtility::SetupUploadGTSIndexData( *m_pGTSWork, m_MyDataStoreID );
      }

      // niji通信互換対応：アップロードするデータを検査してniji表示禁止フラグを立てる
      NetApp::GTS::GTSUtility::CheckNijiNgFlag( *m_pGTSWork );

      NetApp::GTS::GtsIndexData& uploadGTSIndexData = m_pGTSWork->GetUploadGTSIndexData();

      pml::pokepara::CoreParam& uploadPokemonParam = m_pGTSWork->GetSelectPokemonCoreParam();

      if( m_pNetAppPokemonValidationUtility->Verify( NetLib::Validation::PokemonValidation::E_GAME_MODE_GTS_UPLOAD_M, &uploadGTSIndexData, &uploadPokemonParam ) )
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

            SetSequenceNo( SEQUENCE_WANT_POKEMON_VERIFY );
          }
          break;

          case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_NG:
          {
            // 検索不可フラグ設定
            // ここでGTS_INDEXD_ATAの値を書き換えることでアップロード時の認証に失敗する
            // →検索に引っかからなくなる
            uploadGTSIndexData.PokeInfo.searchDisable = IGNORE_MYPOKE_VALIDATION_FAIL; // 不正ポケ

            SetSequenceNo( SEQUENCE_WANT_POKEMON_VERIFY );
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
    // 欲しいポケモンの不正チェック
    case SEQUENCE_WANT_POKEMON_VERIFY:
    {
      if( m_pDummyPokemon == NULL )
      {
        gfl2::heap::HeapBase* pHeap = m_pGTSWork->GetDeviceHeap();

        MonsNo monsno = static_cast<MonsNo>( m_pGTSWork->GetRequestPokemonMonsNo() );
        u16    level  = GTSUtility::ConvertLevelBandToLevel( m_pGTSWork->GetRequestPokemonLevelBand() );

        m_pDummyPokemon = GFL_NEW( pHeap ) pml::pokepara::CoreParam( pHeap, monsno, level, 0 );

        m_pDummyPokemon->SetDefaultWaza();
      }

      NetApp::GTS::GtsIndexData& uploadGTSIndexData = m_pGTSWork->GetUploadGTSIndexData();

      if( m_pNetAppPokemonValidationUtility->Verify( NetLib::Validation::PokemonValidation::E_GAME_MODE_GTS_CHECK_M, &uploadGTSIndexData, m_pDummyPokemon ) )
      {
        NetAppLib::Util::NetAppPokemonValidationUtility::VerifyResult eVerifyResult = m_pNetAppPokemonValidationUtility->GetVerifyResult();

        MonsNo monsno = static_cast<MonsNo>( m_pGTSWork->GetRequestPokemonMonsNo() );

        switch( eVerifyResult )
        {
          case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_OK:
          {
            if( monsno == MONSNO_NULL )
            {
              // 直接入力で存在しないポケモンを指定している
              uploadGTSIndexData.PokeInfo.searchDisable = IGNORE_REQUESTPOKE_NOT_EXIST;
            }

            SetSequenceNo( SEQUENCE_NOTIFICATION_SUCCESS );
          }
          break;

          case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_NG:
          {
            if( monsno == MONSNO_NULL )
            {
              // 直接入力で存在しないポケモンを指定している
              uploadGTSIndexData.PokeInfo.searchDisable = IGNORE_REQUESTPOKE_NOT_EXIST;
            }
            else
            {
              // 欲しいポケモンの条件が不正だった
              uploadGTSIndexData.PokeInfo.searchDisable = IGNORE_REQUESTPOKE_VALIDATION_FAIL;
            }

            SetSequenceNo( SEQUENCE_NOTIFICATION_SUCCESS );
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
    // 成功を通知する
    case SEQUENCE_NOTIFICATION_SUCCESS:
    {
      if( isNotification() )
      {
        if( m_pUploadPokemonVerifyResponseListener )
        {
          m_pUploadPokemonVerifyResponseListener->OnUploadPokemonVerifySuccess();
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
        if( m_pUploadPokemonVerifyResponseListener )
        {
          m_pUploadPokemonVerifyResponseListener->OnUploadPokemonVerifyError();
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
void GTSUploadPokemonVerifyRequestSequence::OnNexDataStoreUploadSuccess( const nn::nex::qUnsignedInt64 dataId )
{
  GFL_PRINT( "GTSUploadPokemonVerifyRequestSequence::OnNexDataStoreUploadSuccess() \n" );

  Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
  pMyStatus->SetPssPersonalDataStoreId( dataId );

  m_MyDataStoreID = dataId;

  SetSequenceNo( SEQUENCE_UPLOAD_POKEMON_VERIFY );
}


void GTSUploadPokemonVerifyRequestSequence::OnNexDataStoreChangeMetaDataFinish( const nn::nex::qVector<nn::nex::qResult>& results )
{
  GFL_PRINT( "GTSUploadPokemonVerifyRequestSequence::OnNexDataStoreChangeMetaDataFinish()\n" );

  Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();

  nn::nex::qResult ret = results.at( 0 );
  
  if( (nn::nex::qBool)ret )
  {
    m_MyDataStoreID = pMyStatus->GetPssPersonalDataStoreId();

    SetSequenceNo( SEQUENCE_UPLOAD_POKEMON_VERIFY );
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


void GTSUploadPokemonVerifyRequestSequence::OnNexDataStoreClientError( const nn::nex::qResult& result, u32 errorCode )
{
  GFL_PRINT( "GTSUploadPokemonVerifyRequestSequence::OnNexDataStoreClientError()\n" );

  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
}


#elif defined( GF_PLATFORM_WIN32 )

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GTSUploadPokemonVerifyRequestSequence::Update()
{
  if( m_pUploadPokemonVerifyResponseListener )
  {
    m_pUploadPokemonVerifyResponseListener->OnUploadPokemonVerifyError();
  }
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

#endif


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
