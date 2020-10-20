//==============================================================================
/**
 * @file	JoinFestaFacilitiesUtil.cpp
 * @brief	ジョインフェスタ施設モデル
 * @author	ichiraku_katsuhiko
 * @date	2015/10/22　12:52
 */
// =============================================================================
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaFacilitiesUtil/JoinFestaFacilitiesUtil.h"

// ライブラリ
#include <base/include/gfl2_Singleton.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <str/include/gfl2_StrBuf.h>

// system
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// Save
#include "Savedata/include/JoinFestaDataSave.h"
// Script
#include "Savedata/include/EventWork.h"
#include "niji_conv_header/field/flagwork/WorkDefine.h"

// モデルのインデックス
#include <niji_conv_header/field/chara/chara_model_id.h>
// トレーナータイプ
#include <niji_conv_header/trainer/trtype_def.h>

// ジョインフェスタ施設テーブル定義
#include "NetApp/JoinFesta/include/JoinFestaDataTable/JoinFestaDataTable.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFesta)

/************************************************************************/
/* 店員のキャラモデルIDテーブル                                         */
/************************************************************************/
const u32 JoinFestaFacilitiesUtil::CHARAMODEL_ID_TABLE[ JoinFestaFacilitiesUtil::CHARACTER_TYPE_MAX ] = {
  CHARA_MODEL_ID_TR0093_00, // エリートトレーナー ♀ ★
  CHARA_MODEL_ID_TR0092_00, // エリートトレーナー ♂ ★ 
  CHARA_MODEL_ID_TR0095_00, // ベテラントレーナー ♀ ★
  CHARA_MODEL_ID_TR0094_00, // ベテラントレーナー ♂ ★
  CHARA_MODEL_ID_TR0096_00,
  CHARA_MODEL_ID_TR0097_00,
  CHARA_MODEL_ID_TR0111_00,
  CHARA_MODEL_ID_TR0112_00,
  CHARA_MODEL_ID_TR0100_00,
  CHARA_MODEL_ID_TR0101_00,
  CHARA_MODEL_ID_TR0077_00,
  CHARA_MODEL_ID_TR0078_00,
}; 

/************************************************************************/
/* 店員のトレーナータイプテーブル                                       */
/************************************************************************/
const u32 JoinFestaFacilitiesUtil::TRTYPE_TBL[ JoinFestaFacilitiesUtil::CHARACTER_TYPE_MAX ] = {
  trainer::TRTYPE_ELITEW,  // エリートトレーナー ♀ ★
  trainer::TRTYPE_ELITEM,  // エリートトレーナー ♂ ★
  trainer::TRTYPE_VETERANW,  // ベテラントレーナー ♀ ★
  trainer::TRTYPE_VETERANM,  // ベテラントレーナー ♂ ★
  trainer::TRTYPE_BUSINESS,  // ビジネスマン ♂ ★
  trainer::TRTYPE_OL,  // ＯＬ ♀ ★
  trainer::TRTYPE_HEADS,  // バッドガイ ♂ 
  trainer::TRTYPE_BADGIRL,  // バッドガール ♀ 
  trainer::TRTYPE_BREEDERM,  // ポケモンブリーダー ♂ ★
  trainer::TRTYPE_BREEDERW,  // ポケモンブリーダー ♀ ★
  trainer::TRTYPE_TANPAN,  // たんぱんこぞう ♂ 
  trainer::TRTYPE_MINI,  // ミニスカート ♀ 
};

/************************************************************************/
/* 店員のトレーナータイプテーブル                                       */
/************************************************************************/
const u8 JoinFestaFacilitiesUtil::CHARA_SEX_TBL[ JoinFestaFacilitiesUtil::CHARACTER_TYPE_MAX ] = {
  PM_FEMALE,  // エリートトレーナー ♀ ★
  PM_MALE,    // エリートトレーナー ♂ ★
  PM_FEMALE,  // ベテラントレーナー ♀ ★
  PM_MALE,    // ベテラントレーナー ♂ ★
  PM_MALE,    // ビジネスマン ♂ ★
  PM_FEMALE,  // ＯＬ ♀ ★
  PM_MALE,    // バッドガイ ♂ 
  PM_FEMALE,  // バッドガール ♀ 
  PM_MALE,    // ポケモンブリーダー ♂ ★
  PM_FEMALE,  // ポケモンブリーダー ♀ ★
  PM_MALE,    // たんぱんこぞう ♂ 
  PM_FEMALE,  // ミニスカート ♀ 
};

/************************************************************************/
/* ランクに必要なフェスコインテーブル                                   */
/************************************************************************/
const u32 JoinFestaFacilitiesUtil::RANK_TABLE[20][2] = {
  // { ランク, ランクアップに必要なコイン}
  {101,  300},
  { 91,  270},
  { 81,  270},
  { 71,  270},
  { 61,  240},
  { 51,  210},
  { 41,  180},
  { 31,  150},
  { 21,  120},
  { 11,  100},
  { 10,  90},
  {  9,  80},
  {  8,  70},
  {  7,  60},
  {  6,  50},
  {  5,  40},
  {  4,  30},
  {  3,  15},
  {  2,  10},
  {  1,  6}
};

/**
 *  @brief  キャラクターIDから店員のキャラモデルIDを取得
 */
u32 JoinFestaFacilitiesUtil::GetCharaModelIdFromCharacterId( const u32 characterId )
{
  if( characterId < CHARACTER_TYPE_MAX )
  {
    return CHARAMODEL_ID_TABLE[ characterId ];
  }

  GFL_ASSERT_MSG(0,"%d",characterId);
  return CHARAMODEL_ID_TABLE[ 0 ];
}

/**
 *  @brief  キャラクターIDからトレーナータイプを取得
 */
u32 JoinFestaFacilitiesUtil::GetTrTypeFromCharacterId( const u32 characterId )
{
  if( characterId < CHARACTER_TYPE_MAX )
  {
    return TRTYPE_TBL[ characterId ];
  }
  GFL_ASSERT_MSG(0,"%d",characterId);
  return TRTYPE_TBL[0];
}

/**
 *  @brief  キャラクターIDから店員の性別を取得
 */
u8 JoinFestaFacilitiesUtil::GetCharaSexFromCharacterId( const u32 characterId )
{
  if( characterId < CHARACTER_TYPE_MAX )
  {
    return CHARA_SEX_TBL[ characterId ];
  }

  GFL_ASSERT_MSG(0,"%d",characterId);
  return CHARA_SEX_TBL[ 0 ];
}

/**
 *  @brief  施設抽選（ご褒美イベント）
 *  @return 抽選した施設ID
 */
u32 JoinFestaFacilitiesUtil::LotteryFacilitiesId( gfl2::math::Random* pRandom, const NetApp::JoinFesta::JoinFestaDataTable* pjfDataTable )
{
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  // 各ランクの有効な施設IDテーブルを作る
  // 各ランクにいくつお店があるかFIXしていないので余裕を持って50としている
  u32 rankFacilitiesIdTable[ NetApp::JoinFesta::JoinFestaDefine::FACILITIES_RANK_MAX ][ 50 ];
  u32 rank1Max = 0;
  u32 rank2Max = 0;
  u32 rank3Max = 0;
  u32 rank4Max = 0;
  u32 rank5Max = 0;
  {
    for( int facilitiesId=0; facilitiesId<NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_MAX; facilitiesId++ )
    {
      const NetApp::JoinFesta::ShopItemInfo* pShop = pjfDataTable->GetShopItemInfo( facilitiesId );

      // momiji追加
      // こうかんじょはバトルフェスチュートリアルが完了するまでは出さない
      {
        bool isBtlFesTutorialEnd = true;
        Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetEventWork();
        if( *(pEventWork->GetEventWorkAdrs( WK_SYS_BFC_TUTORIAL )) < 2 )
        {// チュートリアル未完了
          isBtlFesTutorialEnd = false;
        }

        if( pShop->shopType == JoinFestaScript::FACILITIES_TYPE_TRADE && isBtlFesTutorialEnd == false )
        {
          continue;
        }
      }

      // RomCode一致チェック
      if( ( GET_VERSION() == VERSION_SUN2 && pShop->isAvailableSun ) ||
          ( GET_VERSION() == VERSION_MOON2 && pShop->isAvailableMoon ) )
      {
        // ランクの場所に格納
        switch( pShop->rank ){
        case 1: rankFacilitiesIdTable[ 0 ][ rank1Max++ ] = facilitiesId; break;
        case 2: rankFacilitiesIdTable[ 1 ][ rank2Max++ ] = facilitiesId; break;
        case 3: rankFacilitiesIdTable[ 2 ][ rank3Max++ ] = facilitiesId; break;
        case 4: rankFacilitiesIdTable[ 3 ][ rank4Max++ ] = facilitiesId; break;
        case 5: rankFacilitiesIdTable[ 4 ][ rank5Max++ ] = facilitiesId; break;
        default:
          GFL_ASSERT_MSG(0,"%d",pShop->rank);
          break;
        }
      }
    }
  }

  // 自分のフェスランクでどのランクが当たるのか決める
  u32 hitRank = 0;  // 当たったランク
  {
    // 抽選確立テーブル
    int rankPercentTable[20][ NetApp::JoinFesta::JoinFestaDefine::FACILITIES_RANK_MAX ] = {
      { 100,	0,	0,	0,	0 },  // 1
      { 100,	0,	0,	0,	0 },  // 2
      { 75,	19,	6,	0,	0 },    // 3
      { 75,	17,	8,	0,	0 },    // 4
      { 75,	15,	10,	0,	0 },    // 5
      { 50,	38,	12,	0,	0 },    // 6
      { 50,	36,	14,	0,	0 },    // 7
      { 50,	34,	16,	0,	0 },    // 8
      { 50,	32,	18,	0,	0 },    // 9
      { 40,	35,	20,	5,	0 },    // 10
      { 30,	40,	22,	7,	1 },    // 11～
      { 25,	40,	24,	9,	2 },    // 21～
      { 20,	35,	31,	11,	3 },    // 31～
      { 15,	30,	38,	13,	4 },    // 41～
      { 10,	25,	45,	15,	5 },    // 51～
      { 10,	20,	47,	17,	6 },    // 61～
      { 10,	15,	49,	19,	7 },    // 71～
      { 10,	15,	46,	21,	8 },    // 81～
      { 10,	15,	43,	23,	9 },    // 91～
      { 10,	15,	40,	25,	10 },   // 100～
    };

    const u16 myFesRank = pSave->GetRank();
    u32 rankPercentTableIndex = 0;
    if( myFesRank >= 100 ){ rankPercentTableIndex = 19; }
    else if( myFesRank >= 91 ){ rankPercentTableIndex = 18; }
    else if( myFesRank >= 81 ){ rankPercentTableIndex = 17; }
    else if( myFesRank >= 71 ){ rankPercentTableIndex = 16; }
    else if( myFesRank >= 61 ){ rankPercentTableIndex = 15; }
    else if( myFesRank >= 51 ){ rankPercentTableIndex = 14; }
    else if( myFesRank >= 41 ){ rankPercentTableIndex = 13; }
    else if( myFesRank >= 31 ){ rankPercentTableIndex = 12; }
    else if( myFesRank >= 21 ){ rankPercentTableIndex = 11; }
    else if( myFesRank >= 11 ){ rankPercentTableIndex = 10; }
    else if( myFesRank >= 10 ){ rankPercentTableIndex = 9; }
    else if( myFesRank >= 9 ){ rankPercentTableIndex = 8; }
    else if( myFesRank >= 8 ){ rankPercentTableIndex = 7; }
    else if( myFesRank >= 7 ){ rankPercentTableIndex = 6; }
    else if( myFesRank >= 6 ){ rankPercentTableIndex = 5; }
    else if( myFesRank >= 5 ){ rankPercentTableIndex = 4; }
    else if( myFesRank >= 4 ){ rankPercentTableIndex = 3; }
    else if( myFesRank >= 3 ){ rankPercentTableIndex = 2; }
    else if( myFesRank >= 2 ){ rankPercentTableIndex = 1; }
    else { rankPercentTableIndex = 0; }

    // どのランクにするか抽選する
    u32 randomValue = pRandom->Next(100) + 1; // 1～100

    // 
    for( int rank=0; rank<NetApp::JoinFesta::JoinFestaDefine::FACILITIES_RANK_MAX; rank++)
    {
      if( randomValue <= rankPercentTable[rankPercentTableIndex][rank] )
      {
        hitRank = rank+1;
        break;
      }
      else
      {
        randomValue -= rankPercentTable[rankPercentTableIndex][rank];
      }
    }
    //ICHI_PRINT(">ICHI 抽選してランク決定:%d \n", hitRank );
  }

  // 抽選する施設のランクが決まったので、ランク施設テーブルからどの施設か決める
  u32 hitFacilitiesId = 0;
  {
    switch( hitRank ){
    case 1: hitFacilitiesId = rankFacilitiesIdTable[ 0 ][ pRandom->Next( rank1Max) ]; break;
    case 2: hitFacilitiesId = rankFacilitiesIdTable[ 1 ][ pRandom->Next( rank2Max) ]; break;
    case 3: hitFacilitiesId = rankFacilitiesIdTable[ 2 ][ pRandom->Next( rank3Max) ]; break;
    case 4: hitFacilitiesId = rankFacilitiesIdTable[ 3 ][ pRandom->Next( rank4Max) ]; break;
    case 5: hitFacilitiesId = rankFacilitiesIdTable[ 4 ][ pRandom->Next( rank5Max) ]; break;
    default:
      GFL_ASSERT_MSG(0,"%d",hitRank);
      break;
    }
    //ICHI_PRINT(">ICHI 抽選して施設決定:%d \n", hitFacilitiesId );
  }
  return hitFacilitiesId;
}

/**
 *  @brief  施設カラーをランダムで取得
 *  @param  facilitiesType  施設種類
 *  @param  facilitiesId    施設ID
 */
JoinFestaScript::FacilitiesColor JoinFestaFacilitiesUtil::GetRandomColor( gfl2::math::Random* pRandom, const JoinFestaScript::FacilitiesType facilitiesType, const u32 facilitiesId )
{
  // 染物屋の場合は、お店によって色が決まる
  if( facilitiesType == JoinFestaScript::FACILITIES_TYPE_DYEING  )
  {
    return ConvertDyeingColor( facilitiesId );
  }
  else
  {
    u32 randomValue = pRandom->Next(100);

    if( randomValue < 5 )       { return JoinFestaScript::FACILITIES_COLOR_4; }
    else if( randomValue < 20 ) { return JoinFestaScript::FACILITIES_COLOR_3; }
    else if( randomValue < 50 ) { return JoinFestaScript::FACILITIES_COLOR_2; }
    else                        { return JoinFestaScript::FACILITIES_COLOR_1; }
  }
}


/**
 *  @brief  染物屋から施設カラー定義に変換
 *  @param  facilitiesId  施設ID
 *  @return 染物屋に対応した施設カラー定義
 */
JoinFestaScript::FacilitiesColor JoinFestaFacilitiesUtil::ConvertDyeingColor( const u32 facilitiesId )
{
  switch( facilitiesId ){
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_RED_00:
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_RED_01:
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_RED_02:
    return JoinFestaScript::FACILITIES_COLOR_RED;
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_GREEN_00:
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_GREEN_01:
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_GREEN_02:
    return JoinFestaScript::FACILITIES_COLOR_GREEN;
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_ORANGE_00:
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_ORANGE_01:
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_ORANGE_02:
    return JoinFestaScript::FACILITIES_COLOR_ORANGE;
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_PURPLE_00:
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_PURPLE_01:
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_PURPLE_02:
    return JoinFestaScript::FACILITIES_COLOR_PURPLE;
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_YELLOW_00:
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_YELLOW_01:
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_YELLOW_02:
    return JoinFestaScript::FACILITIES_COLOR_YELLOW;
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_BLUE_00:
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_BLUE_01:
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_BLUE_02:
    return JoinFestaScript::FACILITIES_COLOR_BLUE;
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_NAVY_00:
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_NAVY_01:
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_NAVY_02:
    return JoinFestaScript::FACILITIES_COLOR_NAVY;
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_PINK_00:
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_PINK_01:
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_PINK_02:
    return JoinFestaScript::FACILITIES_COLOR_PINK;
  default:
    GFL_ASSERT_MSG(0,"%d",facilitiesId);
    return JoinFestaScript::FACILITIES_COLOR_RED;
  }
}


/**
 *  @brief  こうかんじょの残り回数値を取得
 *  @param  facilitiesId  施設ID
 *  @return 対応した残り回数値
 */
u8 JoinFestaFacilitiesUtil::GetTradeRemainCountMax( const u32 facilitiesId )
{
  switch( facilitiesId ){
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_BFES_00:  return 1;
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_BFES_01:  return 2;
  case NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_BFES_02:  return 3;
  }
  return 0;
}

/**
 *  @brief  フェスランクからランクアップに必要なコイン取得
 *  @param  rank  現在ランク
 *  @param  次のランクアップに必要なコイン数
 */
u32 JoinFestaFacilitiesUtil::GetNextRankFesCoin( const u16 rank )
{
  for( int i=0; i<20; i++ )
  {
    if( rank >= RANK_TABLE[i][0] )
    {
      return RANK_TABLE[i][1];
    }
  }
  GFL_ASSERT(0);
  return 100;
}


GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetAppLib)

