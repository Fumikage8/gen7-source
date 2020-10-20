// ============================================================================
/*
 * @file GTSUtility.cpp
 * @brief GTSアプリで使用する共通処理をまとめたクラスです。
 * @date 2015.04.16
 */
// ============================================================================
#include "NetApp/GTS/source/GTSUtility.h"
#include "App/Box/include/BoxAppParam.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Savedata/include/MyStatus.h"
#include "Battle/Regulation/include/PokeRegulation.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
#include <util/include/gfl2_std_string.h>
#include <niji_conv_header/message/msg_gts.h>
#include <PokeTool/include/PokeTool.h>
#include <pml/include/item/item.h>

// niji互換対応ポケモンチェック
#include "Battle/Regulation/include/PokeRegulation.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)

static const u32 GTS_MESSAGE_ID_TABLE_GENDER[ NetApp::GTS::GENDER_MAX ] =
{
  msg_gts_sel_12,
  msg_gts_sys_cap_07,
  msg_gts_sys_cap_08,
};


static const u32 GTS_MESSAGE_ID_TABLE_LEVEL_BAND[ NetApp::GTS::LEVEL_BAND_MAX ] =
{
  msg_gts_sel_15,
  msg_gts_sel_16,
  msg_gts_sel_17,
  msg_gts_sel_18,
  msg_gts_sel_19,
  msg_gts_sel_20,
  msg_gts_sel_21,
  msg_gts_sel_22,
  msg_gts_sel_23,
  msg_gts_sel_24,
  msg_gts_sel_25,
};

static const u32 GTS_LEVEL_TO_LEVEL_BAND_CONVERT_TABLE_SIZE = 101;

static const u32 GTS_LEVEL_TO_LEVEL_BAND_CONVERT_TABLE[ GTS_LEVEL_TO_LEVEL_BAND_CONVERT_TABLE_SIZE ] = 
{
  // Level 0
  NetApp::GTS::LEVEL_BAND_NONE,
  // Level 1～10
  NetApp::GTS::LEVEL_BAND_1_10,   NetApp::GTS::LEVEL_BAND_1_10,   NetApp::GTS::LEVEL_BAND_1_10,   NetApp::GTS::LEVEL_BAND_1_10,   NetApp::GTS::LEVEL_BAND_1_10,   NetApp::GTS::LEVEL_BAND_1_10,   NetApp::GTS::LEVEL_BAND_1_10,   NetApp::GTS::LEVEL_BAND_1_10,   NetApp::GTS::LEVEL_BAND_1_10,   NetApp::GTS::LEVEL_BAND_1_10,
  // Level 11～20
  NetApp::GTS::LEVEL_BAND_11_20,  NetApp::GTS::LEVEL_BAND_11_20,  NetApp::GTS::LEVEL_BAND_11_20,  NetApp::GTS::LEVEL_BAND_11_20,  NetApp::GTS::LEVEL_BAND_11_20,  NetApp::GTS::LEVEL_BAND_11_20,  NetApp::GTS::LEVEL_BAND_11_20,  NetApp::GTS::LEVEL_BAND_11_20,  NetApp::GTS::LEVEL_BAND_11_20,  NetApp::GTS::LEVEL_BAND_11_20,
  // Level 21～30
  NetApp::GTS::LEVEL_BAND_21_30,  NetApp::GTS::LEVEL_BAND_21_30,  NetApp::GTS::LEVEL_BAND_21_30,  NetApp::GTS::LEVEL_BAND_21_30,  NetApp::GTS::LEVEL_BAND_21_30,  NetApp::GTS::LEVEL_BAND_21_30,  NetApp::GTS::LEVEL_BAND_21_30,  NetApp::GTS::LEVEL_BAND_21_30,  NetApp::GTS::LEVEL_BAND_21_30,  NetApp::GTS::LEVEL_BAND_21_30,
  // Level 31～40
  NetApp::GTS::LEVEL_BAND_31_40,  NetApp::GTS::LEVEL_BAND_31_40,  NetApp::GTS::LEVEL_BAND_31_40,  NetApp::GTS::LEVEL_BAND_31_40,  NetApp::GTS::LEVEL_BAND_31_40,  NetApp::GTS::LEVEL_BAND_31_40,  NetApp::GTS::LEVEL_BAND_31_40,  NetApp::GTS::LEVEL_BAND_31_40,  NetApp::GTS::LEVEL_BAND_31_40,  NetApp::GTS::LEVEL_BAND_31_40,
  // Level 41～50
  NetApp::GTS::LEVEL_BAND_41_50,  NetApp::GTS::LEVEL_BAND_41_50,  NetApp::GTS::LEVEL_BAND_41_50,  NetApp::GTS::LEVEL_BAND_41_50,  NetApp::GTS::LEVEL_BAND_41_50,  NetApp::GTS::LEVEL_BAND_41_50,  NetApp::GTS::LEVEL_BAND_41_50,  NetApp::GTS::LEVEL_BAND_41_50,  NetApp::GTS::LEVEL_BAND_41_50,  NetApp::GTS::LEVEL_BAND_41_50,
  // Level 51～60
  NetApp::GTS::LEVEL_BAND_51_60,  NetApp::GTS::LEVEL_BAND_51_60,  NetApp::GTS::LEVEL_BAND_51_60,  NetApp::GTS::LEVEL_BAND_51_60,  NetApp::GTS::LEVEL_BAND_51_60,  NetApp::GTS::LEVEL_BAND_51_60,  NetApp::GTS::LEVEL_BAND_51_60,  NetApp::GTS::LEVEL_BAND_51_60,  NetApp::GTS::LEVEL_BAND_51_60,  NetApp::GTS::LEVEL_BAND_51_60,
  // Level 61～70
  NetApp::GTS::LEVEL_BAND_61_70,  NetApp::GTS::LEVEL_BAND_61_70,  NetApp::GTS::LEVEL_BAND_61_70,  NetApp::GTS::LEVEL_BAND_61_70,  NetApp::GTS::LEVEL_BAND_61_70,  NetApp::GTS::LEVEL_BAND_61_70,  NetApp::GTS::LEVEL_BAND_61_70,  NetApp::GTS::LEVEL_BAND_61_70,  NetApp::GTS::LEVEL_BAND_61_70,  NetApp::GTS::LEVEL_BAND_61_70,
  // Level 71～80
  NetApp::GTS::LEVEL_BAND_71_80,  NetApp::GTS::LEVEL_BAND_71_80,  NetApp::GTS::LEVEL_BAND_71_80,  NetApp::GTS::LEVEL_BAND_71_80,  NetApp::GTS::LEVEL_BAND_71_80,  NetApp::GTS::LEVEL_BAND_71_80,  NetApp::GTS::LEVEL_BAND_71_80,  NetApp::GTS::LEVEL_BAND_71_80,  NetApp::GTS::LEVEL_BAND_71_80,  NetApp::GTS::LEVEL_BAND_71_80,
  // Level 81～90
  NetApp::GTS::LEVEL_BAND_81_90,  NetApp::GTS::LEVEL_BAND_81_90,  NetApp::GTS::LEVEL_BAND_81_90,  NetApp::GTS::LEVEL_BAND_81_90,  NetApp::GTS::LEVEL_BAND_81_90,  NetApp::GTS::LEVEL_BAND_81_90,  NetApp::GTS::LEVEL_BAND_81_90,  NetApp::GTS::LEVEL_BAND_81_90,  NetApp::GTS::LEVEL_BAND_81_90,  NetApp::GTS::LEVEL_BAND_81_90,
  // Level 91～100
  NetApp::GTS::LEVEL_BAND_91_100, NetApp::GTS::LEVEL_BAND_91_100, NetApp::GTS::LEVEL_BAND_91_100, NetApp::GTS::LEVEL_BAND_91_100, NetApp::GTS::LEVEL_BAND_91_100, NetApp::GTS::LEVEL_BAND_91_100, NetApp::GTS::LEVEL_BAND_91_100, NetApp::GTS::LEVEL_BAND_91_100, NetApp::GTS::LEVEL_BAND_91_100, NetApp::GTS::LEVEL_BAND_91_100
};


const u32* GTSUtility::GetGenderMessageIDTable()
{
  return GTS_MESSAGE_ID_TABLE_GENDER;
}

u32 GTSUtility::GetGenderMessageIDTableSize()
{
  return GFL_NELEMS( GTS_MESSAGE_ID_TABLE_GENDER );
}

u32 GTSUtility::GetGenderMessageID( u8 gender )
{
  if( gender < NetApp::GTS::GENDER_MAX )
  {
    return GTS_MESSAGE_ID_TABLE_GENDER[ gender ];
  }
  return GTS_MESSAGE_ID_TABLE_GENDER[ NetApp::GTS::GENDER_UNKNOWN ];
}


u8 GTSUtility::ConvertPmlSexToGender( pml::Sex sex )
{
  u8 gender = NetApp::GTS::GENDER_UNKNOWN;

  switch( sex )
  {
    case pml::SEX_MALE:   gender = NetApp::GTS::GENDER_MALE;  break;
    case pml::SEX_FEMALE: gender = NetApp::GTS::GENDER_FMALE; break;
  }

  return gender;
}


pml::Sex GTSUtility::ConvertGenderToPmlSex( u8 gender )
{
  pml::Sex sex = pml::SEX_UNKNOWN;

  switch( gender )
  {
    case NetApp::GTS::GENDER_MALE:  sex = pml::SEX_MALE;   break;
    case NetApp::GTS::GENDER_FMALE: sex = pml::SEX_FEMALE; break;
  }

  return sex;
}


u8 GTSUtility::GetDrawGender( MonsNo monsno, u8 gender, bool haveNickName )
{
  pml::Sex sex = ConvertGenderToPmlSex( gender );
  
  pml::Sex drawSex = PokeTool::GetDrawSex( monsno, sex, haveNickName, false );

  u8 drawGender = ConvertPmlSexToGender( drawSex );

  return drawGender;
}


const u32* GTSUtility::GetLevelBandMessageIDTable()
{
  return GTS_MESSAGE_ID_TABLE_LEVEL_BAND;
}

u32 GTSUtility::GetLevelBandMessageIDTableSize()
{
  return GFL_NELEMS( GTS_MESSAGE_ID_TABLE_LEVEL_BAND );
}

u32 GTSUtility::GetLevelBandMessageID( u8 levelBand )
{
  if( levelBand < NetApp::GTS::LEVEL_BAND_MAX )
  {
    return GTS_MESSAGE_ID_TABLE_LEVEL_BAND[ levelBand ];
  }
  return GTS_MESSAGE_ID_TABLE_LEVEL_BAND[ NetApp::GTS::LEVEL_BAND_NONE ];
}


u8 GTSUtility::ConvertLevelToLevelBand( u32 level )
{
  u8 levelBand = NetApp::GTS::LEVEL_BAND_NONE;
  
  if( level < GTS_LEVEL_TO_LEVEL_BAND_CONVERT_TABLE_SIZE )
  {
    levelBand = GTS_LEVEL_TO_LEVEL_BAND_CONVERT_TABLE[ level ];
  }

  return levelBand;
}


u16 GTSUtility::ConvertLevelBandToLevel( u8 levelBand )
{
  u16 level = 100;

  switch( levelBand )
  {
    case LEVEL_BAND_1_10:   level = 10;  break;
    case LEVEL_BAND_11_20:  level = 20;  break;
    case LEVEL_BAND_21_30:  level = 30;  break;
    case LEVEL_BAND_31_40:  level = 40;  break;
    case LEVEL_BAND_41_50:  level = 50;  break;
    case LEVEL_BAND_51_60:  level = 60;  break;
    case LEVEL_BAND_61_70:  level = 70;  break;
    case LEVEL_BAND_71_80:  level = 80;  break;
    case LEVEL_BAND_81_90:  level = 90;  break;
    case LEVEL_BAND_91_100: level = 100; break;
  }

  return level;
}


void GTSUtility::SetupUploadGTSIndexData( NetApp::GTS::ApplicationSystem::GTSWork& gtsWork, const u64 dataStoreID )
{
  app::util::Heap* pAppHeap = gtsWork.GetAppHeap();
  pml::pokepara::CoreParam& uploadPokemonParam = gtsWork.GetSelectPokemonCoreParam();

  if( item::ITEM_CheckPiece( uploadPokemonParam.GetItem() ) )
  {
    uploadPokemonParam.RemoveItem();
  }

  NetApp::GTS::GtsIndexData gtsIndexData;
  gfl2::std::MemClear( &gtsIndexData, sizeof( NetApp::GTS::GtsIndexData ) );

  Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();

  gtsIndexData.PokeInfo.kind                = static_cast<u16>(uploadPokemonParam.GetMonsNo());
  gtsIndexData.PokeInfo.gender              = ConvertPmlSexToGender( uploadPokemonParam.GetSex() );
  gtsIndexData.PokeInfo.levelBand           = ConvertLevelToLevelBand( uploadPokemonParam.GetLevel() );
  gtsIndexData.PokeInfo.requestKind         = static_cast<u16>(gtsWork.GetRequestPokemonMonsNo());
  gtsIndexData.PokeInfo.requestGender       = static_cast<u8>(gtsWork.GetRequestPokemonGender());
  gtsIndexData.PokeInfo.requestLevelBand    = static_cast<u8>(gtsWork.GetRequestPokemonLevelBand());

  if( PokeRegulation::CheckLegend( gtsIndexData.PokeInfo.requestKind, 0 ) ||
      PokeRegulation::CheckSubLegend( gtsIndexData.PokeInfo.requestKind )
    )
  {
    gtsIndexData.PokeInfo.requestNoLegend = 0;
  }
  else
  {
    gtsIndexData.PokeInfo.requestNoLegend = 1;
  }
  
  gtsIndexData.PokeInfo.searchDisable       = 0;
  gtsIndexData.PokeInfo.form                = static_cast<u8>(uploadPokemonParam.GetFormNo());
  gtsIndexData.PokeInfo.level               = static_cast<u8>(uploadPokemonParam.GetLevel());
  uploadPokemonParam.GetNickName( reinterpret_cast<gfl2::str::STRCODE*>(gtsIndexData.PokeInfo.nickname), NetApp::GTS::POKEMON_NAME_SIZE );
  gtsIndexData.PokeInfo.item                = uploadPokemonParam.GetItem();
  gtsIndexData.PokeInfo.isRare              = static_cast<u8>(uploadPokemonParam.IsRare());
  gtsIndexData.PokeInfo.haveNickName        = static_cast<u8>(uploadPokemonParam.HaveNickName());
  gtsIndexData.PokeInfo.ballID              = static_cast<u8>(uploadPokemonParam.GetGetBall());
  gtsIndexData.PokeInfo.personalRandom      = uploadPokemonParam.GetPersonalRnd();

  gtsIndexData.uniqId                       = pMyStatus->GetNexUniqueID();
  gtsIndexData.dataStoreId                  = dataStoreID;
  gtsIndexData.principalId                  = pMyStatus->GetPrincipalId();

  gfl2::str::StrBuf trainerName( NetApp::GTS::TRAINER_NAME_SIZE, pAppHeap->GetDeviceHeap() );
  pMyStatus->GetNameString( &trainerName );
  trainerName.PutStr( reinterpret_cast<gfl2::str::STRCODE*>( gtsIndexData.TrainerName ), trainerName.GetSize() );

  gtsIndexData.messageID                    = static_cast<u16>(gtsWork.GetRequestMessageID());
  gtsIndexData.Country                      = static_cast<u16>( pMyStatus->GetCountry() );
  gtsIndexData.Region                       = static_cast<u16>( pMyStatus->GetArea() );
  gtsIndexData.iconData                     = pMyStatus->GetIcon();
  gtsIndexData.romCode                      = pMyStatus->GetRomCode();
  gtsIndexData.getVersion                   = static_cast<u8>(uploadPokemonParam.GetCassetteVersion());
  gtsIndexData.trainerGender                = ConvertPmlSexToGender( static_cast<pml::Sex>( pMyStatus->GetSex() ) );

  gtsIndexData.pokemonLanguageCode          = static_cast<u8>( uploadPokemonParam.GetLangId() );
  gtsIndexData.languageCode                 = System::GetLang();

  GFL_PRINT( "gtsIndexData.PokeInfo.requestKind = %d\n", gtsIndexData.PokeInfo.requestKind );
  GFL_PRINT( "gtsIndexData.PokeInfo.requestGender = %d\n", gtsIndexData.PokeInfo.requestGender );
  GFL_PRINT( "gtsIndexData.PokeInfo.requestLevelBand = %d\n", gtsIndexData.PokeInfo.requestLevelBand );

#if PM_DEBUG
  {
    GFL_PRINT("//======== GtsIndexData Dump Start ========\n");
    u8 * rowData = (u8*)&gtsIndexData;
    for( int i = 0; i < sizeof(gtsIndexData); ++i )
    {
      GFL_PRINT("0x%x,", rowData[i]);
      if((i+1)%16==0)
      GFL_PRINT("\n");
    }
    GFL_PRINT("\n//======== GtsIndexData Dump End ========\n");
    GFL_PRINT("GtsIndexData size %d\n", sizeof(gtsIndexData));
    GFL_PRINT("GtsIndexData.iconData offset %d\n", offsetof(GtsIndexData, iconData));
    GFL_PRINT("GtsIndexData.romCode offset %d\n", offsetof(GtsIndexData, romCode));
    GFL_PRINT("GtsIndexData.romCode offset %d\n", offsetof(GtsIndexData, isNijiNg));
    GFL_PRINT("GtsIndexData.reserve offset %d\n", offsetof(GtsIndexData, reserve));
    GFL_PRINT("sizeof(Savedata::MyStatus::ICON_DATA) = %d\n", sizeof(Savedata::MyStatus::ICON_DATA));
  }
#endif

  gtsWork.SetUploadGTSIndexData( gtsIndexData );

}


void GTSUtility::BackupUploadPokemon( NetApp::GTS::ApplicationSystem::GTSWork& gtsWork )
{
  NetApp::GTS::ApplicationSystem::PokemonSelectInfo& pokemonSelectInfo = gtsWork.GetPokemonSelectInfo();
  pml::pokepara::CoreParam& uploadPokemonParam = gtsWork.GetSelectPokemonCoreParam();
  
  u8     tray      = pokemonSelectInfo.tray;
  u8     pos       = pokemonSelectInfo.pos;
  MonsNo monsno    = static_cast<MonsNo>( gtsWork.GetRequestPokemonMonsNo() );
  int    gender    = static_cast<int>( gtsWork.GetRequestPokemonGender() );
  int    levelBand = static_cast<int>( gtsWork.GetRequestPokemonLevelBand() );

  Savedata::GtsData* pGTSSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetGtsData();

  pGTSSaveData->SetBackupPokemonCoreParam( &uploadPokemonParam, tray, pos, monsno, gender, levelBand );
  pGTSSaveData->SetWantPokeName( &gtsWork.GetInputName() );

  if( pokemonSelectInfo.inBox )
  {
    Savedata::BoxPokemon* pBoxPokemon = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();
    pBoxPokemon->ClearPokemon( tray, pos, gtsWork.GetDeviceHeap() ); // ボックスから削除
  }
  else
  {
    pml::PokeParty* pPokeParty = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();
    pPokeParty->RemoveMember( pos ); // てもちから削除
  }

  pGTSSaveData->SetDepositFlag( true );
}


void GTSUtility::RestoreUploadPokemon( NetApp::GTS::ApplicationSystem::GTSWork& gtsWork )
{
  GameSys::GameData*    pGameData    = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  Savedata::GtsData*    pGTSSaveData = pGameData->GetGtsData();
  Savedata::BoxPokemon* pBoxPokemon  = pGameData->GetBoxPokemon();
  pml::PokeParty*       pPokeParty   = pGameData->GetPlayerParty();
  gfl2::heap::HeapBase* pHeap        = gtsWork.GetDeviceHeap();

  u32 tray = static_cast<u32>( pGTSSaveData->GetReturnTrayNo() );
  u32 pos  = static_cast<u32>( pGTSSaveData->GetReturnPosNo() );

  bool bInBox = ( tray == App::Box::RETURN_TEMOTI ) ? false : true;

  pml::pokepara::PokemonParam backupPokemon( pHeap );
  pGTSSaveData->GetBackupPokemonCoreParam( &backupPokemon );

  // Boxに追加する時
  if( bInBox )
  {
    bool bBoxSpaceFlag = false;

    u32 spaceCount = pBoxPokemon->GetSpaceCountAll( pHeap );

    if( spaceCount >= 1 )
    {
      bBoxSpaceFlag = pBoxPokemon->GetSpacePos( tray, pos, pHeap );
    }

    // Boxに空きがある
    if( bBoxSpaceFlag )
    {
      pBoxPokemon->PutPokemon( &backupPokemon, tray, pos, pHeap );
    }
    // Boxに空きがない
    else
    {
      // 手持ちに余裕がある時
      if( pPokeParty->GetMemberCount() < pml::PokeParty::MAX_MEMBERS )
      {
        pPokeParty->AddMember( backupPokemon );
      }
      // 手持ちに余裕がない時
      else
      {
        GFL_ASSERT_MSG( NULL, "Error !! : Failed to add in the box and party.\n" );
      }
    }
  }
  // 手持ちに追加する時
  else
  {
    // 手持ちに余裕がある時
    if( pPokeParty->GetMemberCount() < pml::PokeParty::MAX_MEMBERS )
    {
      pPokeParty->AddMember( backupPokemon );
    }
    // 手持ちに余裕がない時
    else
    {
      u32 spaceTray = 0;
      u32 spacePos = 0;

      bool bBoxSpaceFlag = false;

      u32 spaceCount = pBoxPokemon->GetSpaceCountAll( pHeap );

      if( spaceCount >= 1 )
      {
        bBoxSpaceFlag = pBoxPokemon->GetSpacePos( spaceTray, spacePos, pHeap );
      }

      // Boxに空きがある
      if( bBoxSpaceFlag )
      {
        pBoxPokemon->PutPokemon( &backupPokemon, spaceTray, spacePos, pHeap );
      }
      // Boxに空きがない
      else
      {
        GFL_ASSERT_MSG( NULL, "Error !! : Failed to add in the party and box.\n" );
      }
    }
  }

  pml::pokepara::CoreParam& demoRecvPokemon = gtsWork.GetDemoRecvPokemon();
  pGTSSaveData->GetBackupPokemonCoreParam( &demoRecvPokemon );

  pGTSSaveData->SetDepositFlag( false );

}


void GTSUtility::SetCriticalSectionFlag( bool bFlag )
{
  if( bFlag )
  {
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SaveCriticalSpanOn();
  }
  else
  {
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SaveCriticalSpanOff();
  }
}

//--------------------------------------------------------------
/**
 * @brief   niji通信互換対応用
 *          アップロードするデータ検査してniji表示禁止フラグを立てる
 */
//--------------------------------------------------------------
void GTSUtility::CheckNijiNgFlag( NetApp::GTS::ApplicationSystem::GTSWork& gtsWork )
{
  NetApp::GTS::GtsIndexData& gtsIndexData = gtsWork.GetUploadGTSIndexData();

  // 欲しいポケモンのチェック
  {
    // フォルムは設定出来ないので0
    if( PokeRegulation::CheckNijiCompatibleMonsNo( gtsIndexData.PokeInfo.requestKind, 0 ) == true )
    {
      GFL_RELEASE_PRINT( ">GTS 欲しいポケモンが新ポケモンだったのでniji禁止フラグON\n" );
      gtsIndexData.isNijiNg = 1;
    }
  }

  // あずけるポケモンのチェック
  {
    pml::pokepara::CoreParam& uploadPokemonParam = gtsWork.GetSelectPokemonCoreParam();

    if( PokeRegulation::CheckNijiCompatible( &uploadPokemonParam ) == true )
    {
      GFL_RELEASE_PRINT( ">GTS 預けるポケモンが互換対応ポケモンだったのでniji禁止フラグON\n" );
      gtsIndexData.isNijiNg = 1;
    }
  }
}


GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
