
//==============================================================================
/**
 * @file   BattleSpotWork.cpp
 * @date   2015/10/02 Fri. 12:50:41
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./BattleSpotWork.h"

#include  "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

#include <gflnet2/include/nex/gflnet2_NexManager.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include "Battle/Regulation/include/Regulation.h"
#include "Battle/Regulation/include/PokeRegulation.h"

#include "NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/BattleVideoRequestClient.h"
#include "NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/BattleVideoResponseListener.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  static const u32  APPHEAP_SIZE              = 0x00400000;
  static const u32  DEVHEAP_SIZE              = 0x00c00000;

  static const u32  LOCALHEAP_APP_REGION_SIZE = 0x00010000;
  static const u32  LOCALHEAP_DEV_REGION_SIZE = 0x00080000;
} /*  namespace  */


namespace NetApp      {
namespace BattleSpot  {

// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     BattleSpot
  *  @brief    ctor
  */
//------------------------------------------------------------------
BattleSpotWork::BattleSpotWork(void)
  : NetAppLib::System::ApplicationWorkBase(APPHEAP_SIZE, DEVHEAP_SIZE)
  , m_pAppParam(NULL)
  , m_pViewContainer(NULL)
  , m_pTrainerIconUtility(NULL)
  , m_gpfEntryFlg(0)
  , m_internetTempEntryWork(0)
  , m_friendTempEntryWork(0)
  , m_pBattleSpotSave( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetBattleSpotData() )
  , m_retireRequest(false)
#if PM_DEBUG
  , m_IsDebugForcedRegionName(0)
#endif
  , m_RegionHeap()
  , m_pBattleVideoClient(NULL)
#if defined( GF_PLATFORM_CTR )
  , m_pCountryNameCache(NULL)
  , m_pRegionNameCache(NULL)
#endif
  , m_isPGLReportEnd(false)
{
  m_internetTempEntryWork = m_pBattleSpotSave->GetInternetEntryWork();
  m_friendTempEntryWork = m_pBattleSpotSave->GetFriendEntryWork();

  m_RegionHeap.LocalHeapCreate( GetDeviceHeap(), GetDeviceHeap(), LOCALHEAP_APP_REGION_SIZE, LOCALHEAP_DEV_REGION_SIZE);
}


//------------------------------------------------------------------
/**
  *  @func     BattleSpot
  *  @brief    ctor
  */
//------------------------------------------------------------------
BattleSpotWork::~BattleSpotWork()
{
#if defined( GF_PLATFORM_CTR )
  NetLib::P2P::NexRatingClient* ratingClient = GFL_SINGLETON_INSTANCE( NetLib::P2P::P2pConnectionManager )->GetRatingClient();
  if( ratingClient )
  {
    ratingClient->UnregistRatingClientListener(this);
  }

  GFL_SAFE_DELETE(m_pRegionNameCache);
  GFL_SAFE_DELETE(m_pCountryNameCache);
#endif
  GFL_SAFE_DELETE( m_pBattleVideoClient );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
u8    BattleSpotWork::GetPokemonLanguageID(void) const
{
  u8 u8PokeLanguageID = System::GetLang();

  if( u8PokeLanguageID == POKEMON_LANG_CHINA
   || u8PokeLanguageID == POKEMON_LANG_TAIWAN )
  {
    u8PokeLanguageID = POKEMON_LANG_ENGLISH; // @fix NMCat[4117] 繁体字、簡体字だったら英語に置き換える
  }

  /*  Debugなどで切り替える場合はAppParamで受け取るなどする  */
  return u8PokeLanguageID;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  BattleSpotWork::SetupCountryRegionNameCache(void)
{
#if defined( GF_PLATFORM_CTR )
  gfl2::heap::HeapBase* pHeapBase = GetDeviceHeap();

  m_pCountryNameCache = GFL_NEW(pHeapBase)  System::CTR::CountryNameCache();
  m_pCountryNameCache->LoadCountryName(m_RegionHeap.GetDeviceHeap(), static_cast<nn::cfg::CTR::CfgLanguageCode>( System::Language::ConvertPM2CTR(GetPokemonLanguageID())));

  m_pRegionNameCache  = GFL_NEW(pHeapBase)  System::CTR::RegionNameCache( m_pCountryNameCache->GetUnkownCountryName(), reinterpret_cast<u8*>(&m_RegionNameWork));
#endif
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
Savedata::MyStatus* BattleSpotWork::GetTargetStatus(const RegionTargetID regionTargetID)
{
  Savedata::MyStatus*   pTargetStatus = NULL;



#if defined(GF_PLATFORM_CTR)
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);

  int myNetID = pP2pConnectionManager->GetNetID();
  if( myNetID < 0 )
  {
    return NULL;
  }

  switch(regionTargetID)
  {
  case  REGIONTARGET_Player:      pTargetStatus = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatus();
    break;
  case  REGIONTARGET_Opponent0:
    {
      if( GetRule() == RULE_FREE_ROYAL )
      {
        for( u32 index = 0 ; index < MULTI_PLAYER_NUM ; ++index )
        {
          if( index != myNetID )
          {
            pTargetStatus = pP2pConnectionManager->GetMyStatus( index );
            break;
          }
        }
      }
      else
      {
        pTargetStatus = pP2pConnectionManager->GetMyStatus( 1 - myNetID );
      }
    }
    break;
  case  REGIONTARGET_Opponent1:
    {
      u32 count = 0;
      for( u32 index = 0 ; index < MULTI_PLAYER_NUM ; ++index )
      {
        if( index != myNetID )
        {
          pTargetStatus = pP2pConnectionManager->GetMyStatus( index );
          if( count == 1 )
          {
            break;
          }
          count++;
        }
      }
    }
    break;
  case  REGIONTARGET_Opponent2:   
    {
      u32 count = 0;
      for( u32 index = 0 ; index < MULTI_PLAYER_NUM ; ++index )
      {
        if( index != myNetID )
        {
          pTargetStatus = pP2pConnectionManager->GetMyStatus( index );
          if( count == 2 )
          {
            break;
          }
          count++;
        }
      }
    }
    break;
  }


#endif


  return pTargetStatus;
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
const gfl2::str::STRCODE* BattleSpotWork::GetCountry(const RegionTargetID regionTargetID)
{

#if defined( GF_PLATFORM_CTR )
  Savedata::MyStatus*   pTargetStatus = GetTargetStatus(regionTargetID);
  if(pTargetStatus)
  {
    return m_pCountryNameCache->GetCountryName(static_cast<nn::cfg::CfgCountryCode>(pTargetStatus->GetCountry()));
  }
#endif

  return L"";
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
const gfl2::str::STRCODE* BattleSpotWork::GetRegion(const RegionTargetID regionTargetID)
{

#if PM_DEBUG
  if(m_IsDebugForcedRegionName)
  {
    return L"Archipielago de San Andres, Providencia y Santa Catalina"; /*  コロンビアの長い地域名  */
  }
#endif


#if defined( GF_PLATFORM_CTR )
  Savedata::MyStatus*   pTargetStatus = GetTargetStatus(regionTargetID);
  if(pTargetStatus)
  {
    nn::cfg::CTR::SimpleAddressId saID;

    saID.id  = (static_cast<u32>(pTargetStatus->GetCountry()) << nn::cfg::CFG_SIMPLE_ADDRESS_ID_COUNTRY_SHIFT) | (static_cast<u32>(pTargetStatus->GetArea()) << nn::cfg::CFG_SIMPLE_ADDRESS_ID_REGION_SHIFT);
    m_pRegionNameCache->LoadRegionName( m_RegionHeap.GetDeviceHeap(), static_cast<nn::cfg::CTR::CfgLanguageCode>(System::Language::ConvertPM2CTR(GetPokemonLanguageID())), static_cast<nn::cfg::CfgCountryCode>(pTargetStatus->GetCountry()));

    return m_pRegionNameCache->GetRegionName(saID);
  }
#endif



  return L"";
}

Savedata::BattleSpotData::ENTRY_COMMUNITY_TYPE BattleSpotWork::GetCategoryID( void )
{

  Savedata::BattleSpotData::ENTRY_COMMUNITY_TYPE type = Savedata::BattleSpotData::ENTRY_COMMUNITY_RATE_S;

  switch( GetRule() )
  {
  case BattleSpotWork::RULE_RATE_SINGLE:
    type = Savedata::BattleSpotData::ENTRY_COMMUNITY_RATE_S;
    break;
  case BattleSpotWork::RULE_RATE_DOUBLE:
    type = Savedata::BattleSpotData::ENTRY_COMMUNITY_RATE_D;
    break;
  case BattleSpotWork::RULE_RATE_SPECIAL:
    type = Savedata::BattleSpotData::ENTRY_COMMUNITY_RATE_SP;
    break;
  case BattleSpotWork::RULE_RATE_WCS:
    type = Savedata::BattleSpotData::ENTRY_COMMUNITY_RATE_WCS;
    break;
  case BattleSpotWork::RULE_NET_CUP:
    type = Savedata::BattleSpotData::ENTRY_COMMUNITY_NET;
    break;
  default:
    GFL_ASSERT(0);//@fix
    break;
  };

  return type;
}


bool BattleSpotWork::CheckPokeParty( u32 netID , u8* pSignatureBuffer , REGULATION_COMP* pRegulationComp )
{        
  bool ret = false;

#if defined( GF_PLATFORM_CTR )
  if( pSignatureBuffer )
  {
    NetLib::Validation::PokemonValidation* pValidation = GFL_NEW_LOW( GetDeviceHeap()->GetLowerHandle() ) NetLib::Validation::PokemonValidation();

    if( pValidation->Initialize(
      GetDeviceHeap()->GetLowerHandle(), GetDeviceHeap()->GetLowerHandle(),
      GFL_SINGLETON_INSTANCE( gflnet2::nex::NexManager )->GetAuthenticationToken(),
      NetLib::Validation::PokemonValidation::E_GAME_MODE_BATTLE_M ) ) // チェック関数なのでモードはnijiでもmomijiでも良い
    {
      // ポケモンセット
      u8 pokeNum = 0;

      for( int pokeIndex=0; pokeIndex< m_pAppParam->m_pPersonalData[netID]->pPokeParty->GetMemberCount(); pokeIndex++ )
      {
        const pml::pokepara::PokemonParam* pp = m_pAppParam->m_pPersonalData[netID]->pPokeParty->GetMemberPointerConst( pokeIndex );

        // コアデータシリアライズ
        NetApp::GTS::GtsSerializePokemonParam gtsSerializePokemonParam;
        gfl2::std::MemClear( &gtsSerializePokemonParam, sizeof( NetApp::GTS::GtsSerializePokemonParam ) );
        pp->Serialize_Core( gtsSerializePokemonParam.data );

        // 不正チェックで処理するデータのセット
        pValidation->SetPokemon( pokeIndex, NULL, &gtsSerializePokemonParam );
        GFL_PRINT("改ざんチェックPokemon[%d] %d \n", pokeIndex, pp->GetMonsNo() );
      }

      // 証明書を使用して改ざんチェック
      ret = pValidation->CheckRsaValidation( m_pAppParam->m_pPersonalData[netID]->pPokeParty->GetMemberCount(), pSignatureBuffer , NetLib::Validation::PokemonValidation::SIGNATURE_SIZE );
    }
    else
    {
      GFL_ASSERT(0);//@fix
    }

    // 破棄
    pValidation->Finalize();
    GFL_SAFE_DELETE( pValidation );
  }
  else
  {//署名なければ検査スキップ
    ret = true;
  }

  //レギュレーション一致チェック
  if( ret == true )
  {
    REGULATION_COMP myRegulationComp;
    ::std::memset( &myRegulationComp , 0 , sizeof(myRegulationComp) );
    SetupRegulationComp( &myRegulationComp );
    if( std::memcmp( &myRegulationComp , pRegulationComp , sizeof(myRegulationComp) ) != 0 )
    {//不一致
      GFL_PRINT("レギュレーション不一致");

      ret = false;
    }
  }

  //正常だったらレギュレーションチェック
  if( ret == true )
  {
    PokeRegulation pokeRegulation( GetDeviceHeap()->GetLowerHandle() );
    pokeRegulation.SetRegulation( &m_pAppParam->m_pRegulationDrawInfo->GetRegulation() );
    // バトルチーム用レギュレーションチェック
    PokeRegulation::ViolationDetails violationDetails; // エラー詳細データ
    if( pokeRegulation.CheckPokePartyForBtlTeam( m_pAppParam->m_pPersonalData[netID]->pPokeParty, violationDetails ) != true )
    {
      GFL_PRINT("ポケパーティ検査でレギュレーション %d\n", netID );
      m_pAppParam->m_pPersonalData[netID]->pPokeParty->Dump();
      
      ret = false;
    }
  }


#endif
  return ret;
}

void BattleSpotWork::SetupRegulationComp( REGULATION_COMP* pOutput )
{
  pOutput->btl_rule = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetBattleRule();
  pOutput->level = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::LEVEL );
  pOutput->level_range = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetLevelRangeType();
  pOutput->reverse = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetBoolParam( Regulation::REVERSE ) ? 1 : 0;
  pOutput->time_limit_type = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetTimeLimitType();
  pOutput->time_vs = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::TIME_VS );
  pOutput->time_command = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::TIME_COMMAND );
  pOutput->nickname = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetBoolParam( Regulation::NICKNAME ) ? 1 : 0;
  pOutput->background = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetBackGroundType();
  pOutput->show_poke = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetBoolParam( Regulation::SHOW_POKE ) ? 1 : 0;
  pOutput->show_poke_time = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::SHOW_POKE_TIME );
  //@fix[#3925 仲間大会：レギュレーションを改ざんしていてもマッチングが成立する]↓再報告追加分
  pOutput->category = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetBattleCategory();
  pOutput->team_num_lo = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::TEAM_NUM_LO );
  pOutput->team_num_hi = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::TEAM_NUM_HI );
  pOutput->select_num_lo = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::SELECT_NUM_LO );
  pOutput->select_num_hi = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::SELECT_NUM_HI );
  pOutput->legend_num_hi = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::LEGEND_NUM_HI );
  pOutput->qr_team = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetBoolParam( Regulation::QR_TEAM ) ? 1 : 0;

  for( u32 index = 0 ; index < Regulation::MUST_POKE_NUM ; ++index )
  {
    pOutput->must_poke_and[ index ]       = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetParamMustPokeAND( index );
    pOutput->must_poke_form_and[ index ]  = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetParamMustPokeFormAND( index );
    pOutput->must_poke_or[ index ]        = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetParamMustPokeOR( index );
    pOutput->must_poke_form_or[ index ]   = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetParamMustPokeFormOR( index );
  }
}


bool BattleSpotWork::CheckP2PDisconnect( void )
{
#if defined( GF_PLATFORM_CTR )
  if( GFL_SINGLETON_INSTANCE( NetLib::P2P::P2pConnectionManager )->IsConnectSuccess() )
  {
    if( GFL_SINGLETON_INSTANCE( NetLib::Error::NijiNetworkErrorManager )->IsSessionLeft() )
    {
      GFL_SINGLETON_INSTANCE( NetLib::Error::NijiNetworkErrorManager )->Clear();
      m_pAppParam->m_isSessionLeft = true;
      return true;
    }
  }
#endif
  return false;
}


#if defined(GF_PLATFORM_CTR)

bool BattleSpotWork::SendPglRecord( void )
{
  GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  NetLib::P2P::NexRatingClient* ratingClient = GFL_SINGLETON_INSTANCE( NetLib::P2P::P2pConnectionManager )->GetRatingClient();
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE( NetLib::P2P::P2pConnectionManager );

  int myNetID = pP2pConnectionManager->GetNetID();
  if( myNetID < 0 )
  {
    return false;
  }

  ratingClient->RegistRatingClientListener(this);

  // マイステータス
  Savedata::MyStatus *myst = game_data->GetPlayerStatus();
  NetLib::P2P::NexRatingClient::REPORT_RATE my_rate, other_rate;
  Gbu::GBU_DATA gbu_data;
  memset( &gbu_data , 0 , sizeof(gbu_data) );

  // GBUデータ作成
  pml::PokeParty *temotiParty = GFL_NEW( GetDeviceHeap()->GetLowerHandle() )  pml::PokeParty( GetDeviceHeap()->GetLowerHandle() );
  temotiParty ->Clear();
  temotiParty ->CopyFrom( *m_pAppParam->m_pPersonalData[ myNetID ]->pPokeParty );
  

  // ここでのmyPartyは参加ポケモンになっている
  //@fix GFNMCat[5623]:[#5623 【バトルスポット】対戦ログで送信するポケモンが、対戦に出したものではなく、手持ちのすべてが送られている]
  //@fix NMCat[4274]:[#4274 バトルスポットで送信している対戦ログ内の対戦に使われたポケモンパーティーデータの並び順がおかしい]
  pml::PokeParty* myParty = m_pAppParam->m_pNetworkBattleParam->pBattleRecorderSaveData->GetPokeParty( BTL_CLIENT_PLAYER , GetDeviceHeap()->GetLowerHandle() );
  pml::PokeParty* otherParty = m_pAppParam->m_pNetworkBattleParam->pBattleRecorderSaveData->GetPokeParty( BTL_CLIENT_ENEMY1 , GetDeviceHeap()->GetLowerHandle() );

  if( myParty == NULL )
  {//手持ちを設定
    myParty = GFL_NEW( GetDeviceHeap()->GetLowerHandle() )  pml::PokeParty( GetDeviceHeap()->GetLowerHandle() );
    myParty->Clear();
    myParty ->CopyFrom( *temotiParty );
  }

  if( otherParty == NULL )
  {//クリア状態を設定
    otherParty = GFL_NEW( GetDeviceHeap()->GetLowerHandle() )  pml::PokeParty( GetDeviceHeap()->GetLowerHandle() );
    otherParty->Clear();
  }

  Gbu::ConvertBattleData( &gbu_data, &m_pAppParam->m_pglRecord , temotiParty, myParty, otherParty );

  GFL_SAFE_DELETE( myParty );
  GFL_SAFE_DELETE( otherParty );
  GFL_SAFE_DELETE( temotiParty );

  // バトルビデオ情報
  gbu_data.battleVideoAutoSend = GetBattleVideoAutoUpload();//m_common_data->auto_btlvideo_status;
  gbu_data.battleVideoNumber = m_pAppParam->m_pNetworkBattleParam->pBattleRecorderSaveData->GetBattleRecDataDirect()->dataID;

  m_pAppParam->m_pNetworkBattleParam->pBattleRecorderSaveData->GetBattleRecDataDirect()->dataID = 0;//セットしたので初期化

  // NexUniqueIDをセット
  Gbu::SetNexUniqueID( &gbu_data, myst->GetNexUniqueID(), pP2pConnectionManager->GetMyStatus( 1 - myNetID )->GetNexUniqueID() );

  // 情報表示
  Gbu::Dump( gbu_data );



/************************************************/
#if PM_DEBUG
  // 不正レポート送信フラグ
  if( m_pAppParam->m_IsDebugForcedWinReport )
  {
    //m_common_data->debug_illegal_report = 0;
    // 絶対勝ちにする
    m_pAppParam->m_pNetworkBattleParam->battleResult = BTL_RESULT_WIN;
    GFL_PRINT("!!デバッグで不正レポート送信　強制勝ち\n");
  }
#endif
/************************************************/


  // 勝敗
  if( m_pAppParam->m_pNetworkBattleParam->battleResult == BTL_RESULT_DRAW )
  {
    GFL_PRINT("\n*******レポート送信：引き分け*************\n");
    my_rate = NetLib::P2P::NexRatingClient::E_RATE_DRAW;
    other_rate = NetLib::P2P::NexRatingClient::E_RATE_DRAW;
  }
  else if( m_pAppParam->m_pNetworkBattleParam->battleResult == BTL_RESULT_LOSE || 
            m_pAppParam->m_pNetworkBattleParam->battleResult == BTL_RESULT_RUN
    )
  {
    GFL_PRINT("\n*******レポート送信：負け*************\n");
    my_rate = NetLib::P2P::NexRatingClient::E_RATE_LOSE;
    other_rate = NetLib::P2P::NexRatingClient::E_RATE_WIN;
  }else
  {
    GFL_PRINT("\n*******レポート送信：勝ち*************\n");
    my_rate = NetLib::P2P::NexRatingClient::E_RATE_WIN;
    other_rate = NetLib::P2P::NexRatingClient::E_RATE_LOSE;
  }

  //P2Pロストしていたら勝ちにする
  if( m_pAppParam->m_pNetworkBattleParam->battleResult == BTL_RESULT_COMM_ERROR || m_pAppParam->m_isSessionLeft == true )
  {
    m_pAppParam->m_pNetworkBattleParam->battleResult = BTL_RESULT_WIN;
    GFL_PRINT("P2Pロストなので強制勝利！！！\n");
  }

  // スロット分け
  if( nn::friends::GetMyPrincipalId() > pP2pConnectionManager->GetMyStatus( 1 - myNetID )->GetPrincipalId() )
  {
    SetMyReportStatusInfo( NetLib::P2P::NexRatingClient::E_SLOT1, my_rate, &gbu_data );
    SetOtherReportStatusInfo( NetLib::P2P::NexRatingClient::E_SLOT2, other_rate, &gbu_data );
  } 
  else
  {
    SetOtherReportStatusInfo( NetLib::P2P::NexRatingClient::E_SLOT1, other_rate, &gbu_data );
    SetMyReportStatusInfo( NetLib::P2P::NexRatingClient::E_SLOT2, my_rate, &gbu_data );
  }

  // レーティングサーバーに送信
  bool result = ratingClient->ReportRatingStats();
  if( result == true )
  {
    m_isPGLReportEnd = false;
  }

  return result;
}

//------------------------------------------------------------------
/**
 * @brief 自分の送信情報をセット
 * @param slot  セットするスロット
 * @param rate  勝敗レート
 * @param gbu_data  GBU用データ
 */
//------------------------------------------------------------------
void BattleSpotWork::SetMyReportStatusInfo( NetLib::P2P::NexRatingClient::REPORT_SLOT_ID slot, NetLib::P2P::NexRatingClient::REPORT_RATE rate, Gbu::GBU_DATA *gbu_data )
{
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);

  u32 category_id = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::DISCRIMINANT_ID );
  bool ret;

  int myNetID = pP2pConnectionManager->GetNetID();
  if( myNetID < 0 )
  {
    return;
  }

  if( m_pAppParam->m_isQRTeam )
  {//QRチーム使用
    memcpy( gbu_data->battleTeamID , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetQRBattleTeamID() , sizeof(gbu_data->battleTeamID) );
  }

  // 情報1
  NetLib::P2P::NexRatingClient* ratingClient = GFL_SINGLETON_INSTANCE( NetLib::P2P::P2pConnectionManager )->GetRatingClient();
  ret = ratingClient->SetReportStatusInfo(
            slot,                                    //設定するスロットID
            nn::friends::GetMyPrincipalId(),          //プリンシバルID
            pP2pConnectionManager->GetMyStatus( myNetID )->GetNexUniqueID(),                            //NEXユニークID
            category_id,                              // カテゴリId
            nn::nex::RatingStatsFlags::Finished,      //通信切断フラグ
            rate,                                     //対戦結果レート
            gbu_data,                                 //バトルログ情報
            sizeof(Gbu::GBU_DATA),                     //バトルログ情報サイズ
            m_pAppParam->m_pNetworkBattleParam->recHeader.result2
            );

  GFL_ASSERT(ret); // @check
  GFL_PRINT("-----SLOT%d-------------------------\n", slot);
  GFL_PRINT("プリンシパルID %d \n", nn::friends::GetMyPrincipalId());
  GFL_PRINT("NEXユニークID %llu \n", pP2pConnectionManager->GetMyStatus( myNetID )->GetNexUniqueID() );
  GFL_PRINT("カテゴリID %d \n", category_id );
  GFL_PRINT("対戦結果レート %d \n", rate );
  GFL_PRINT("--------------------------\n");

}

//------------------------------------------------------------------
/**
 * @brief 相手の送信情報をセット
 * @param slot  セットするスロット
 * @param rate  勝敗レート
 * @param gbu_data  GBU用データ
 */
//------------------------------------------------------------------
void BattleSpotWork::SetOtherReportStatusInfo( NetLib::P2P::NexRatingClient::REPORT_SLOT_ID slot, NetLib::P2P::NexRatingClient::REPORT_RATE rate, Gbu::GBU_DATA *gbu_data )
{
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
  u32 category_id = m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::DISCRIMINANT_ID );
  bool ret;

  int myNetID = pP2pConnectionManager->GetNetID();
  if( myNetID < 0 )
  {
    return;
  }

  // 通信切断フラグ
  nn::nex::RatingStatsFlags::RatingStatsFlags statsFlag;

  // バトルでの不整合フラグチェック
  if( m_pAppParam->m_pNetworkBattleParam->isIllegalBattle != 0 )
  {
    // 不正フラグ
    statsFlag = nn::nex::RatingStatsFlags::Missmatched;
  }
  else if( m_pAppParam->m_pNetworkBattleParam->battleResult == BTL_RESULT_COMM_ERROR ||
           m_pAppParam->m_isSessionLeft == true )
  {// P2P切断チェック
    // 切断されたことを示すフラグ
    statsFlag = nn::nex::RatingStatsFlags::Disconnected;
  }
  else
  {
    // 正常フラグ
    statsFlag = nn::nex::RatingStatsFlags::Finished;
  }

  // 情報2
  NetLib::P2P::NexRatingClient* ratingClient = GFL_SINGLETON_INSTANCE( NetLib::P2P::P2pConnectionManager )->GetRatingClient();
  ret = ratingClient->SetReportStatusInfo(
            slot,                                    //設定するスロットID
            pP2pConnectionManager->GetMyStatus( 1 - myNetID )->GetPrincipalId(),                  //プリンシバルID
            pP2pConnectionManager->GetMyStatus( 1 - myNetID )->GetNexUniqueID(),            //NEXユニークID
            category_id,                              // カテゴリId
            statsFlag,                                //通信切断フラグ
            rate,                                     //対戦結果レート
            gbu_data,                                 //バトルログ情報
            sizeof(Gbu::GBU_DATA),                     //バトルログ情報サイズ
            m_pAppParam->m_pNetworkBattleParam->recHeader.result2
            );
  GFL_ASSERT(ret); // @check
  GFL_PRINT("-----SLOT%d-------------------------\n", slot);
  GFL_PRINT("プリンシパルID %d \n", pP2pConnectionManager->GetMyStatus( 1 - myNetID )->GetPrincipalId() );
  GFL_PRINT("NEXユニークID %llu \n", pP2pConnectionManager->GetMyStatus( 1 - myNetID )->GetNexUniqueID() );
  GFL_PRINT("カテゴリID %d \n", category_id );
  GFL_PRINT("対戦結果レート %d \n", rate );
  GFL_PRINT("通信切断フラグ %d \n", statsFlag );
  GFL_PRINT("--------------------------\n");
}


//--------------------------------------------------------------------------
/**
 * @brief   バトルビデオを自動送信するかチェック
 * @retval  true:送信する 
 * @retval  false:送信しない 
 */
//--------------------------------------------------------------------------
bool BattleSpotWork::IsAutoSend( void )
{
  /**
   *   条件チェック
   * 
   *  ・自動送信でOKを押下（レートのみ）
   *  ・自分が勝っていたら
   *  ・クロスチェックの不整合フラグが立っていない
   *  ・自分と相手で、低い方のレートがレギュレーション条件を満たしている
   *  ・P2Pロストしていない
   * 
   * 条件に合わないとfalseを返す
   */

  //切断だったら送信しない
  if( m_pAppParam->m_isSessionLeft == true )
  {
    GFL_PRINT("対戦前切断なので送信しない\n");
    return false;
  }


  // 自動送信でOK（レートのみ）
  u32 rule = GetRule();
  if( rule == RULE_RATE_SINGLE ||
      rule == RULE_RATE_DOUBLE || 
      rule == RULE_RATE_SPECIAL || 
      rule == RULE_RATE_WCS 
      )
  {
    if( GetBattleVideoAutoUpload() == false )
    {
      GFL_PRINT("自動送信しない：許可していない\n");
      return false;
    }
  }

  // 自分が勝っていたら
  if( m_pAppParam->m_pNetworkBattleParam->battleResult != BTL_RESULT_WIN &&
      m_pAppParam->m_pNetworkBattleParam->battleResult != BTL_RESULT_RUN_ENEMY
    )
  {
    GFL_PRINT("自動送信しない：勝ってないn");
    return false;
  }

  // クロスチェックの不整合フラグ
  if( m_pAppParam->m_pNetworkBattleParam->isIllegalBattle != 0 )
  {
    GFL_PRINT("自動送信しない：不整合フラグ\n");
    return false;
  }

  // レート
  {
    u32 rate;
    if( GetRankData().rate < GetOpponentRate() )
    {
      rate = GetRankData().rate;
    }else {
      rate = GetOpponentRate();
    }

    // 同じはOKにする
    Regulation& regulation = m_pAppParam->m_pRegulationDrawInfo->GetRegulation();
    if( rate < regulation.GetValueParam( Regulation::BTL_VIDEO ) )
    {
      GFL_PRINT("自動送信しない：レート満たしていない\n");
      /*******************************************************/
#ifdef BATTLE_MATCH_DEBUG_MENU
      if( debug_force_check_ok == 1 )
      {
        GFL_PRINT("デバッグで強制OKにする\n");
        return true;
      }
#endif // BATTLE_MATCH_DEBUG_MENU
/*******************************************************/
      return false;
    }
  }

  GFL_PRINT("自動送信する\n");
  return true;
}



//------------------------------------------------------------------
/**
* @brief Ratingサーバへの結果送信終了通知
*/
//------------------------------------------------------------------
void BattleSpotWork::OnRatingReportSuccess( void )
{
  m_isPGLReportEnd = true;
}

//------------------------------------------------------------------
/**
* @brief Ratingサーバ通信のエラー通知
*
* @param[in] result    実行結果
* @param[in] errorCode 通信エラーコード
*/
//------------------------------------------------------------------
void BattleSpotWork::OnNexRatingClientError( const nn::nex::qResult& result, u32 errorCode )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultNex( result );

  GFL_SINGLETON_INSTANCE( NetLib::Error::NijiNetworkErrorManager )->SetNetworkError( error );
  m_isPGLReportEnd = true;
}




void BattleSpotWork::SendBattleVideo( NetApp::BattleVideo::BattleVideoResponseListener* pListener )
{
  m_pBattleVideoClient = GFL_NEW( GetDeviceHeap()->GetLowerHandle() ) NetApp::BattleVideo::BattleVideoRequestClient( pListener , GetDeviceHeap()->GetLowerHandle() );

  NetApp::BattleVideo::BattleVideoRequestClient::UPLOAD_REQUEST_PARAM_FOR_BS param;
  std::memset( &param , 0 , sizeof(param) );
  param.pUploadData = m_pAppParam->m_pNetworkBattleParam->pBattleRecorderSaveData->GetBattleRecDataDirect();
  param.pBSWork = this;
  param.pBattleRecorderSaveData = m_pAppParam->m_pNetworkBattleParam->pBattleRecorderSaveData;

  m_pBattleVideoClient->UploadBattleVideoRequestForBattleSpot( param );
}

void BattleSpotWork::EndBattleVideo()
{
  GFL_SAFE_DELETE( m_pBattleVideoClient );
}


bool BattleSpotWork::IsRegulationTimeOver( u8 hour )
{
  if( m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam(Regulation::START_ENABLE_TIME) == 0 &&
    m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam(Regulation::END_ENABLE_TIME) == 0 )
  {
    return false;
  }

  return m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam(Regulation::END_ENABLE_TIME) < hour;
}

bool BattleSpotWork::IsRegulationTimeBefore( u8 hour )
{
  if( m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam(Regulation::START_ENABLE_TIME) == 0 &&
    m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam(Regulation::END_ENABLE_TIME) == 0 )
  {
    return false;
  }

  return m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam(Regulation::START_ENABLE_TIME) > hour;
}

bool BattleSpotWork::IsRegulationTimeOut( u8 hour )
{
  return IsRegulationTimeOver( hour ) || IsRegulationTimeBefore( hour );
}


#endif



//////////////////////GBU/////////////////////////////

//--------------------------------------------------------------------------
/**
 * @brief バトルのPGL集計データからGBU用のデータに変換
 * @param gbu_data      [out]GBUデータ
 * @param pgl_record    [in]PGL集計データ
 * @param temotiParty   [in]自分の手持ち
 * @param myParty       [in]自分の対戦ポケパーティ
 * @param enemyParty    [in]相手の対戦ポケパーティ
 */
//--------------------------------------------------------------------------
void Gbu::ConvertBattleData( GBU_DATA* gbu_data, BATTLE_PGL_RECORD_SET* pgl_record, pml::PokeParty *temotiParty, pml::PokeParty *myParty, pml::PokeParty *enemyParty )
{
  gfl2::std::MemClear( gbu_data, sizeof(GBU_DATA));

  // 報告者の手持ちポケモンセット
  SetTemotiParty( gbu_data, temotiParty );

  // 報告者が対戦に出したポケモン情報セット
  SetBattleMyParty( gbu_data, myParty );

  // 対戦相手が対戦に出したポケモン情報セット
  SetBattleEnemyParty( gbu_data, enemyParty );

  if( pgl_record != NULL )
  {
    for( int i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
    {
      // 自分が倒された情報セット
      SetLosePokemonInfo( &gbu_data->losePokemonInfo[i], &pgl_record->myParty[i] );
      // 自分が倒した情報セット
      SetWinPokemonInfo( &gbu_data->winPokemonInfo[i], &pgl_record->enemysParty[i] );
    }
  }
}

//--------------------------------------------------------------------------
/**
 * @brief GBUデータにNexUniqueIDを設定する
 * @param gbu_data          [out]GBUデータ
 * @param myNexUniqueID     [in]自分のNexUniqueID
 * @param enemyNexUniqueId  [in]相手のNexUniqueID
 */
//--------------------------------------------------------------------------
void Gbu::SetNexUniqueID( GBU_DATA* gbu_data, const u64 myNexUniqueId, const u64 enemyNexUniqueId )
{
#if defined( GF_PLATFORM_CTR )
  // 16文字固定です
  std::snprintf( gbu_data->myNexUniqueId , 17 , "%16llx", myNexUniqueId );
  std::snprintf( gbu_data->enemyNexUniqueId , 17 , "%16llx", enemyNexUniqueId );
#endif
}

//--------------------------------------------------------------------------
/**
 * @brief 報告者の手持ちポケモンセット
 * @param gbu_data    [out] GBUデータポインタ
 * @param party       [in]  自分のパーティ
 */
//--------------------------------------------------------------------------
void Gbu::SetTemotiParty( GBU_DATA* gbu_data, pml::PokeParty *party )
{
  const u32 PokeCount = party->GetMemberCount(); // 手持ち数取得
  for( u32 i=0; i<PokeCount; i++ )
  {
    const pml::pokepara::PokemonParam *pp = party->GetMemberPointerConst( i );

    // ポケモンの情報
    gbu_data->temotiParty[ i ].pokeNumber  = pp->GetMonsNo();
    gbu_data->temotiParty[ i ].formNumber  = pp->GetFormNo();
    gbu_data->temotiParty[ i ].sex         = pp->GetSex();

    // 技
    for( int wazaCnt =0; wazaCnt<pp->GetWazaCount(); wazaCnt++ )
    {
      gbu_data->temotiParty[ i ].waza[ wazaCnt ] = pp->GetWazaNo( wazaCnt );
    }
  }
}

//--------------------------------------------------------------------------
/**
 * @brief 報告者が対戦に出したポケモン情報セット
 * @param gbu_data    [out] GBUデータポインタ
 * @param party       [in]  自分が対戦に出したパーティ
 */
//--------------------------------------------------------------------------
void Gbu::SetBattleMyParty( GBU_DATA* gbu_data, pml::PokeParty *party )
{
  const u32 PokeCount = party->GetMemberCount(); // 手持ち数取得
  for( u32 i=0; i<PokeCount; i++ )
  {
    const pml::pokepara::PokemonParam *pp = party->GetMemberPointerConst( i );

    // ポケモンの情報
    gbu_data->myParty[ i ].mons_no  = pp->GetMonsNo();
    gbu_data->myParty[ i ].form_no  = pp->GetFormNo();
    gbu_data->myParty[ i ].sex      = pp->GetSex();
    gbu_data->myParty[ i ].lv       = pp->GetLevel();
    gbu_data->myParty[ i ].tokusei  = pp->GetTokuseiNo();
    gbu_data->myParty[ i ].seikaku  = pp->GetSeikaku();
    gbu_data->myParty[ i ].item     = pp->GetItem();
  }
}

//--------------------------------------------------------------------------
/**
 * @brief 対戦相手が対戦に出したポケモン情報セット
 * @param gbu_data    [out] GBUデータポインタ
 * @param party       [in]  対戦相手のパーティ
 */
//--------------------------------------------------------------------------
void Gbu::SetBattleEnemyParty( GBU_DATA* gbu_data, pml::PokeParty *party )
{
  const u32 PokeCount = party->GetMemberCount(); // 手持ち数取得
  for( u32 i=0; i<PokeCount; i++ )
  {
    const pml::pokepara::PokemonParam *pp = party->GetMemberPointerConst( i );

    // ポケモンの情報
    gbu_data->enemyParty[ i ].mons_no  = pp->GetMonsNo();
    gbu_data->enemyParty[ i ].form_no  = pp->GetFormNo();
    gbu_data->enemyParty[ i ].sex      = pp->GetSex();
    gbu_data->enemyParty[ i ].lv       = pp->GetLevel();
    gbu_data->enemyParty[ i ].tokusei  = pp->GetTokuseiNo();
    gbu_data->enemyParty[ i ].seikaku  = pp->GetSeikaku();
    gbu_data->enemyParty[ i ].item     = pp->GetItem();
  }
}

//--------------------------------------------------------------------------
/**
 * @brief 自分が倒された情報セット
 * @param info        [out]   自分が倒されたポケモン情報
 * @param pgl_record  [in]    PGL集計データ
 */
//--------------------------------------------------------------------------
void Gbu::SetLosePokemonInfo( LOSE_POKEMON_INFO* info, BATTLE_PGL_RECORD* pgl_record )
{
  info->deadPokeNumber    = pgl_record->deadPokeNumber;
  info->deadPokeForm      = pgl_record->deadPokeForm;
  info->finishWazaNumber  = pgl_record->finishWazaNumber;
  info->attackPokeNumber  = pgl_record->attackPokeNumber;
  info->attackPokeForm    = pgl_record->attackPokeForm;
}

//--------------------------------------------------------------------------
/**
 * @brief 自分が倒したポケモン情報セット
 * @param info        [out]   自分が倒したポケモン情報
 * @param pgl_record  [in]    PGL集計データ
 */
//--------------------------------------------------------------------------
void Gbu::SetWinPokemonInfo( WIN_POKEMON_INFO* info, BATTLE_PGL_RECORD* pgl_record )
{
  info->deadPokeNumber    = pgl_record->deadPokeNumber;
  info->deadPokeForm      = pgl_record->deadPokeForm;
  info->finishWazaNumber  = pgl_record->finishWazaNumber;
  info->attackPokeNumber  = pgl_record->attackPokeNumber;
  info->attackPokeForm    = pgl_record->attackPokeForm;
}

#if PM_DEBUG
/**
 * @brief 情報表示
 * @param gbu_data
 */
void Gbu::Dump( GBU_DATA& gbu_data )
{
  GFL_PRINT("*************************************\n");
  GFL_PRINT("■バトルビデオ\n");
  GFL_PRINT("バトルビデオ送信フラグ: %d\n", gbu_data.battleVideoAutoSend);
  GFL_PRINT("バトルビデオID       : %lu\n", gbu_data.battleVideoNumber);
  GFL_PRINT("*************************************\n");
  GFL_PRINT("■NexUniqueID\n");
  GFL_PRINT("自分       : %s\n", gbu_data.myNexUniqueId);
  GFL_PRINT("相手       : %s\n", gbu_data.enemyNexUniqueId);
  GFL_PRINT("*************************************\n");
  GFL_PRINT("■報告者のポケモン\n");
  for( int i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
  {
    if( gbu_data.temotiParty[i].pokeNumber == 0 ) {
      continue;
    }
    GFL_PRINT("ポケモン番号:  %d \n", gbu_data.temotiParty[i].pokeNumber);
    GFL_PRINT("フォルム番号:  %d \n", gbu_data.temotiParty[i].formNumber);
    GFL_PRINT("性別       :  %d \n", gbu_data.temotiParty[i].sex);
    GFL_PRINT("技1        :  %d \n", gbu_data.temotiParty[i].waza[0]);
    GFL_PRINT("技2        :  %d \n", gbu_data.temotiParty[i].waza[1]);
    GFL_PRINT("技3        :  %d \n", gbu_data.temotiParty[i].waza[2]);
    GFL_PRINT("技4        :  %d \n", gbu_data.temotiParty[i].waza[3]);
  }

  GFL_PRINT("*************************************\n");
  GFL_PRINT("■報告者が対戦に出したポケモン\n");
  for( int i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
  {
    if( gbu_data.myParty[i].mons_no == 0 ) {
      continue;
    }
    GFL_PRINT("ポケモン番号:  %d \n", gbu_data.myParty[i].mons_no);
    GFL_PRINT("フォルム番号:  %d \n", gbu_data.myParty[i].form_no);
    GFL_PRINT("レベル      :  %d \n", gbu_data.myParty[i].lv);
    GFL_PRINT("性別        :  %d \n", gbu_data.myParty[i].sex);
    GFL_PRINT("特性        :  %d \n", gbu_data.myParty[i].tokusei);
    GFL_PRINT("性格        :  %d \n", gbu_data.myParty[i].seikaku);
    GFL_PRINT("持ち物      :  %d \n", gbu_data.myParty[i].item);
  }

  GFL_PRINT("*************************************\n");
  GFL_PRINT("■相手が対戦に出したポケモン\n");
  for( int i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
  {
    if( gbu_data.enemyParty[i].mons_no == 0 ) {
      continue;
    }
    GFL_PRINT("ポケモン番号:  %d \n", gbu_data.enemyParty[i].mons_no);
    GFL_PRINT("フォルム番号:  %d \n", gbu_data.enemyParty[i].form_no);
    GFL_PRINT("レベル      :  %d \n", gbu_data.enemyParty[i].lv);
    GFL_PRINT("性別        :  %d \n", gbu_data.enemyParty[i].sex);
    GFL_PRINT("特性        :  %d \n", gbu_data.enemyParty[i].tokusei);
    GFL_PRINT("性格        :  %d \n", gbu_data.enemyParty[i].seikaku);
    GFL_PRINT("持ち物      :  %d \n", gbu_data.enemyParty[i].item);
  }

  GFL_PRINT("*************************************\n");
  GFL_PRINT("■自分が倒された情報\n");
  for( int i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
  {
    if( gbu_data.losePokemonInfo[i].deadPokeNumber == 0 ) {
      continue;
    }
    GFL_PRINT("倒された自分ポケモン番号:  %d \n", gbu_data.losePokemonInfo[i].deadPokeNumber);
    GFL_PRINT("倒された自分フォルム番号:  %d \n", gbu_data.losePokemonInfo[i].deadPokeForm);
    GFL_PRINT("倒した相手のポケモン番号:  %d \n", gbu_data.losePokemonInfo[i].attackPokeNumber);
    GFL_PRINT("倒した相手のフォルム番号:  %d \n", gbu_data.losePokemonInfo[i].attackPokeForm);
    GFL_PRINT("倒した場合のわざ番号    :  %d \n", gbu_data.losePokemonInfo[i].finishWazaNumber);
  }

  GFL_PRINT("*************************************\n");
  GFL_PRINT("■自分が倒した情報\n");
  for( int i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
  {
    if( gbu_data.winPokemonInfo[i].deadPokeNumber == 0 ) {
      continue;
    }
    GFL_PRINT("倒された相手ポケモン番号:  %d \n", gbu_data.winPokemonInfo[i].deadPokeNumber);
    GFL_PRINT("倒された相手フォルム番号:  %d \n", gbu_data.winPokemonInfo[i].deadPokeForm);
    GFL_PRINT("倒した自分のポケモン番号:  %d \n", gbu_data.winPokemonInfo[i].attackPokeNumber);
    GFL_PRINT("倒した自分のフォルム番号:  %d \n", gbu_data.winPokemonInfo[i].attackPokeForm);
    GFL_PRINT("倒した場合のわざ番号    :  %d \n", gbu_data.winPokemonInfo[i].finishWazaNumber);
  }
}
#endif

//--------------------------------------------------------------------------
/**
 * @brief   切断フラグをON
 * @param   data    ランキングサーバーの共通データ
 *
 * @note    バトル前に呼ばれる。
 *          バトル後は必ず切断フラグOFFを呼んでください
 */
//--------------------------------------------------------------------------
void BattleSpotWork::DisconnectFlagOn( COMMON_DATA& data )
{
  // 切断フラグをON
  data.disconnect_flag |= ((u64)1 << (u64)data.disconnect_bit);

  // 管理コードをインクリメント
  data.disconnect_bit++;
  if( data.disconnect_bit >= 64 ){
    data.disconnect_bit = 0;
  }
  GFL_PRINT("切断管理コードON %d bit %x \n", data.disconnect_bit, data.disconnect_flag );
}

//--------------------------------------------------------------------------
/**
 * @brief   切断フラグをOFF
 * @param   data    ランキングサーバーの共通データ
 */
//--------------------------------------------------------------------------
void BattleSpotWork::DisconnectFlagOff( COMMON_DATA& data )
{
  // 管理コードをデクリメント
  if( data.disconnect_bit == 0 ){
    data.disconnect_bit = 63;
  }
  else
  {
    data.disconnect_bit--;
  }

  // 切断フラグをOFF
  u64 tmp = ((u64)1 << (u64)data.disconnect_bit);
  u64 tmp2 = ~tmp;
  data.disconnect_flag &= tmp2;

  // 管理コードをインクリメント
  data.disconnect_bit++;
  if( data.disconnect_bit >= 64 ){
    data.disconnect_bit = 0;
  }

  GFL_PRINT("切断管理コードOFF %d bit %x \n", data.disconnect_bit, data.disconnect_flag );
}

//--------------------------------------------------------------------------
/**
 * @brief   切断率を求める（フリー）
 * @param   data    ランキングサーバーの共通データ
 * @return  切断率のパーセンテージ数値
 */
//--------------------------------------------------------------------------
u32 BattleSpotWork::GetDisconnectRate( COMMON_DATA * data )
{
  GFL_PRINT("Ranking::GetDisconnectRate フリー\n");
  int on_count = 0; // フラグONのカウンタ

  // フラグがONの総数を求める
  for( u64 i=0; i<64; i++ )
  {
    u64 bit = 1;
    if( data->disconnect_flag & (bit<<i) )
    {
      on_count++;
    }
  }

  float percent = (float)on_count / 64.0f;
  percent *= 100;

  GFL_PRINT("切断率 %d 切断数 %d\n", (u32)percent, on_count );

  return static_cast<u32>(percent);
}

//--------------------------------------------------------------------------
/**
 * @brief   切断率を求める（レート）
 * @param   data    ランキングサーバーの成績データ
 * @return  切断率のパーセンテージ数値
 */
//--------------------------------------------------------------------------
u32 BattleSpotWork::GetDisconnectRate( RANK_DATA * data )
{
  GFL_PRINT("Ranking::GetDisconnectRate レート\n");
  // 試合数
  int battle_num = data->win + data->lose + data->draw;

  // 試合数が0の時は、切断率0
  if( battle_num == 0 )
  {
    return 0;
  }

  // 変数A
  int a = (11 - battle_num) * 10;
  if( a < 0 ){ a = 0; }

  float percent = (static_cast<float>(data->disconnect) / static_cast<float>(battle_num)) * 100.0f;

  GFL_PRINT("補正前切断率[%f]\n",percent);

  if( static_cast<int>(percent) >= a )
  {
    percent = percent - a;
  }
  else
  {
    percent = 0;
  }

  GFL_PRINT(" %f = ( %d / %d ) * 100 - %d \n", percent, data->disconnect, battle_num, a );

  GFL_PRINT("切断率 %d 切断数 %d 変数A %d\n", static_cast<u32>(percent), data->disconnect ,a );
  return static_cast<u32>(percent);
}


//--------------------------------------------------------------------------
/**
 * @brief   マッチングに使用するレートのRange設定を求める
 * @param   rate    自分のレート値
 * @param   count   マッチング試行回数
 * @return  Range
 */
//--------------------------------------------------------------------------
u32 BattleSpotWork::GetRateRange( const u32 rate, const u32 count )
{
  GFL_ASSERT( count < 3 ); // @check

  // 最初の30秒
  if( count == 0 )
  {
    return 100;
  }
  // 次の30秒
  else if( count == 1 )
  {
    // Rage = (自分のレート - 1500)の絶対値*7/20+100
    int range = DEFAULT_RATE - rate;
    if( range < 0 ){
      range = -range;
    }
    ICHI_PRINT("絶対値 %d \n", range );
    range *= 7;
    range /= 20;
    range += 100;
    return range;
  }
  // 最後の30秒
  else if( count == 2 )
  {
    // Rage = (自分のレート - 1500)の絶対値*7/20+100 の1.5倍
    int range = DEFAULT_RATE - rate;
    if( range < 0 ){
      range = -range;
    }
    ICHI_PRINT("絶対値 %d \n", range );
    range *= 7;
    range /= 20;
    range += 100;
    range = (float)range * 1.5f;
    return range;
  }

  // 万が一来た時
  return 100;
}

//--------------------------------------------------------------------------
/**
 * @brief   @fix MMCat[127]：リボンを除外したバトルチームの一致チェック
 * @param   serverTeamData    PGLサーバからDLしたチームデータ
 * @param   targetTeamData    P2Pで受信した相手のチームデータ
 * @param   pokeNum           チームのポケモン数
 * @return  trueで一致した
 */
//--------------------------------------------------------------------------
bool BattleSpotWork::BattleTeamMatchCheckIgnoreRibon( const u8* serverTeamData, u8* targetTeamData, const u8 pokeNum )
{
  pml::PokeParty *pTempParty1 = GFL_NEW( GetDeviceHeap()->GetLowerHandle() )  pml::PokeParty( GetDeviceHeap()->GetLowerHandle() );
  pml::PokeParty *pTempParty2 = GFL_NEW( GetDeviceHeap()->GetLowerHandle() )  pml::PokeParty( GetDeviceHeap()->GetLowerHandle() );


  // リボンを除外したポケパーティを復元する
  {
    pml::pokepara::PokemonParam* pTempPoke = GFL_NEW( GetDeviceHeap()->GetLowerHandle() ) pml::pokepara::PokemonParam( GetDeviceHeap()->GetLowerHandle() );

    const u8* pSerializePtr1 = serverTeamData;
    const u8* pSerializePtr2 = targetTeamData;
    for( u32 index = 0; index < pokeNum; index++ )
    {
      pTempPoke->Clear();
      pTempPoke->Deserialize_Core( pSerializePtr1 );
      pTempPoke->RemoveAllRibbon(); // 全てのリボンをリセットする
      pTempParty1->AddMember( *pTempPoke );
      pSerializePtr1 += pml::pokepara::CoreParam::DATASIZE;

      pTempPoke->Clear();
      pTempPoke->Deserialize_Core( pSerializePtr2 );
      pTempPoke->RemoveAllRibbon(); // 全てのリボンをリセットする
      pTempParty2->AddMember( *pTempPoke );
      pSerializePtr2 += pml::pokepara::CoreParam::DATASIZE;
    }

    GFL_SAFE_DELETE( pTempPoke );
  }

  // ポケパーティのシリアライズを比較する
  pml::PokeParty::SERIALIZE_DATA poke_party_core1;
  pml::PokeParty::SERIALIZE_DATA poke_party_core2;

  pTempParty1->SerializeCore( &poke_party_core1 );
  pTempParty2->SerializeCore( &poke_party_core2 );

  const bool isComp = gfl2::std::MemComp( &poke_party_core1, &poke_party_core2, pml::PokeParty::SERIALIZE_DATA_SIZE );

  GFL_RELEASE_PRINT(">System#127 リボン除外一致チェック　結果[%d]\n", isComp);

  GFL_SAFE_DELETE( pTempParty2 );
  GFL_SAFE_DELETE( pTempParty1 );

  return isComp;
}

} /*  namespace BattleSpot    */
} /*  namespace NetApp        */
