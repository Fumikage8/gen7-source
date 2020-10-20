// ============================================================================
/*
 * @file GTSSearchPokemonRequestSequence.cpp
 * @brief 探したいポケモンを検索するリクエストを投げるシーケンスです。
 * @date 2015..04.27
 */
// ============================================================================
#include "NetApp/GTS/source/RequestSequence/GTSSearchPokemonRequestSequence.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#ifdef GF_PLATFORM_CTR
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#endif // GF_PLATFORM_CTR
#include <util/include/gfl2_std_string.h>
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
#include "NetStatic/NetLib/include/Validation/PokemonValidation.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(RequestSequence)


GTSSearchPokemonRequestSequence::GTSSearchPokemonRequestSequence() :
NetApp::GTS::RequestSequence::GTSRequestSequenceBase(),
  m_pSearchPokemonResponseListener( NULL )
{
}


GTSSearchPokemonRequestSequence::~GTSSearchPokemonRequestSequence()
{
}


#if defined( GF_PLATFORM_CTR )


void GTSSearchPokemonRequestSequence::updateSequence()
{
  switch( GetSequenceNo() )
  {
    // ---------------------------------------------------------------
    // ポケモンの検索開始
    case SEQUENCE_SEARCH_POKEMON:
    {
      qUnsignedInt32 monsno     = m_pGTSWork->GetSearchPokemonMonsNo();
      qUnsignedInt32 gender     = m_pGTSWork->GetSearchPokemonGender();
      qUnsignedInt32 levelBand  = m_pGTSWork->GetSearchPokemonLevelBand();
      qUnsignedInt32 legend     = m_pGTSWork->GetSearchPokemonLegend();

      GFL_PRINT( "monsno = %d\n", monsno );
      GFL_PRINT( "gender = %d\n", gender );
      GFL_PRINT( "levelBand = %d\n", levelBand );
      GFL_PRINT( "legend = %d\n", legend );

      if( monsno == MONSNO_NULL )
      {
        NetApp::GTS::ApplicationSystem::GtsSearchResultList& searchPokemonResultList = m_pGTSWork->GetSearchPokemonResultList();
        searchPokemonResultList.Erase();

        SetSequenceNo( SEQUENCE_NOTIFICATION_SUCCESS );
      }
      else
      {
        // 検索パラメータセット（自分のポケモン情報）
        nn::nex::GlobalTradeStationSearchPokemonParam searchParam;
        searchParam.SetCondition( nn::nex::GlobalTradeStationConstants::SEARCH_CONDITION_KIND, monsno );
        searchParam.SetCondition( nn::nex::GlobalTradeStationConstants::SEARCH_CONDITION_GENDER, gender );
        searchParam.SetCondition( nn::nex::GlobalTradeStationConstants::SEARCH_CONDITION_LEVEL_BAND, levelBand );
        searchParam.SetCondition( nn::nex::GlobalTradeStationConstants::SEARCH_CONDITION_LEGEND, legend );

        // 検索パラメータ（取得範囲を0-99に）
        nn::nex::ResultRange resultRange;
        resultRange.SetOffset( m_pGTSWork->GetSearchRangeStartOffset() );
        resultRange.SetSize( NetApp::GTS::ApplicationSystem::GTSWork::GTS_SEARCH_POKEMON_RESULT_MAX );
        searchParam.SetResultRange( resultRange );

        Savedata::GtsData* pGTSSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetGtsData();

        const NetApp::GTS::GtsSerializeRecordKey& uploadKey = pGTSSaveData->GetUploadKey();

        nn::nex::GlobalTradeStationRecordKey recordKey;
        recordKey.Deserialize( uploadKey.data, uploadKey.size );
        searchParam.SetPrepareUploadKey( recordKey );

        GFL_PRINT( "m_pNexGTSClient->SearchPokemon()\n" );
        m_pNexGTSClient->SearchPokemon( searchParam );

        SetSequenceNo( SEQUENCE_RESPONSE_WAIT );
      }
    }
    break;

    // ---------------------------------------------------------------
    // 成功を通知する
    case SEQUENCE_NOTIFICATION_SUCCESS:
    {
      if( isNotification() )
      {
        if( m_pSearchPokemonResponseListener )
        {
          m_pSearchPokemonResponseListener->OnSearchPokemonSuccess();
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
        if( m_pSearchPokemonResponseListener )
        {
          m_pSearchPokemonResponseListener->OnSearchPokemonError();
        }
        SetSequenceNo( SEQUENCE_TERMINATE );
      }
    }
    break;

  }
}


void GTSSearchPokemonRequestSequence::notificationCancel()
{
  if( m_pSearchPokemonResponseListener )
  {
    m_pSearchPokemonResponseListener->OnSearchPokemonCancel();
  }
}


//-----------------------------------------------------------------------------
// gflnet2::nex::NexDataStoreClientListener
//-----------------------------------------------------------------------------
void GTSSearchPokemonRequestSequence::OnSearchPokemonSuccess( const nn::nex::GlobalTradeStationSearchPokemonResult& searchResult )
{
  GFL_PRINT( "GTSSearchPokemonRequestSequence::OnSearchPokemonSuccess() Start ------------------------------------------------ \n" );

  NetApp::GTS::ApplicationSystem::GtsSearchResultList& searchPokemonResultList = m_pGTSWork->GetSearchPokemonResultList();
  searchPokemonResultList.Erase();
  GFL_PRINT( "searchPokemonResultList.Erase(); \n" );
  GFL_PRINT( "searchPokemonResultList.Size() = %d \n", searchPokemonResultList.Size() );

  NetApp::GTS::ApplicationSystem::GtsSearchResultList& refineSearchPokemonResultList = m_pGTSWork->GetRefineSearchPokemonResultList();
  refineSearchPokemonResultList.Erase();
  GFL_PRINT( "refineSearchPokemonResultList.Erase(); \n" );
  GFL_PRINT( "refineSearchPokemonResultList.Size() = %d \n", refineSearchPokemonResultList.Size() );

  u32 searchResultCount = searchResult.GetTotalCount();
  GFL_PRINT( "searchResultCount = %d : type %d\n", searchResultCount ,searchResult.GetTotalCountType() );
  if( searchResultCount > 0 )
  {
    const nn::nex::qVector<nn::nex::GlobalTradeStationData>& searchResults = searchResult.GetResult();
    GFL_PRINT( "searchResults.size() = %d \n", searchResults.size() );
    u32 resultVectorSize = searchResults.size();

    for( u32 i = 0; i < resultVectorSize; ++i )
    {
      NetApp::GTS::GtsSearchResultInfo resultInfo;
      gfl2::std::MemClear( &resultInfo, sizeof( NetApp::GTS::GtsSearchResultInfo ) );

      const nn::nex::GlobalTradeStationData& data = searchResults.at(i);

      nn::nex::qVector<u8> indexData;
      data.GetIndexData( indexData );
      GFL_ASSERT_MSG( ( indexData.size() == sizeof( NetApp::GTS::GtsIndexData ) ), "Error : GTSSearchPokemonRequestSequence::OnSearchPokemonSuccess() different indexData size.\n" );
      gfl2::std::MemCopy( reinterpret_cast<NetApp::GTS::GtsIndexData*>(&indexData[0]), &resultInfo.indexData, indexData.size() );

#if defined(GF_PLATFORM_CTR)
      NetLib::Validation::PokemonValidation::ReplacePlayerName( reinterpret_cast<gfl2::str::STRCODE*>( resultInfo.indexData.TrainerName ), System::GetLang(), resultInfo.indexData.romCode );//不正名置換
      NetLib::Validation::PokemonValidation::ReplaceMonsName( reinterpret_cast<gfl2::str::STRCODE*>(resultInfo.indexData.PokeInfo.nickname), System::GetLang(), static_cast<MonsNo>(resultInfo.indexData.PokeInfo.kind) ); //ポケモンのニックネームの置換処理
#endif // defined(GF_PLATFORM_CTR)

      // @fix MMCat[337]：相手がnijiの場合フラグをたてる。
      if( resultInfo.indexData.romCode == VERSION_SUN || resultInfo.indexData.romCode == VERSION_MOON )
      {
        resultInfo.indexData.iconData.dressUpParam.isRomVersionNiji = true;
      }
      
      nn::nex::GlobalTradeStationTradeKey tradeKey;
      nn::nex::qVector<nn::nex::qByte> serializeTradeKey;
      data.GetTradeKey( tradeKey );
      tradeKey.Serialize( serializeTradeKey );
      GFL_ASSERT_MSG( ( serializeTradeKey.size() <= sizeof( resultInfo.serializeTradeKey.data ) ), "Error : GTSSearchPokemonRequestSequence::OnSearchPokemonSuccess() memory of the trade key buffer is not enough.\n" );
      gfl2::std::MemCopy( reinterpret_cast<u8*>(&serializeTradeKey[0]), resultInfo.serializeTradeKey.data, serializeTradeKey.size() );
      resultInfo.serializeTradeKey.size = serializeTradeKey.size();

      searchPokemonResultList.PushBack( resultInfo );
    }
  }

  GFL_PRINT( "searchPokemonResultList.Size() = %d \n", searchPokemonResultList.Size() );
  GFL_PRINT( "refineSearchPokemonResultList.Size() = %d \n", refineSearchPokemonResultList.Size() );

  SetSequenceNo( SEQUENCE_NOTIFICATION_SUCCESS );

  GFL_PRINT( "GTSSearchPokemonRequestSequence::OnSearchPokemonSuccess() End ------------------------------------------------ \n" );

}


void GTSSearchPokemonRequestSequence::OnGlobalTradeStationClientError( const nn::nex::qResult& result, u32 errorCode )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );
  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

  SetSequenceNo( SEQUENCE_NOTIFICATION_ERROR );
}


#elif defined( GF_PLATFORM_WIN32 )

gflnet2::ServerClient::REQUEST_SEQUENCE_STATE GTSSearchPokemonRequestSequence::Update()
{
  if( m_pSearchPokemonResponseListener )
  {
    m_pSearchPokemonResponseListener->OnSearchPokemonError();
  }
  return gflnet2::ServerClient::REQUEST_STATE_FINISH;
}

#endif


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
