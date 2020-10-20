#if defined( GF_PLATFORM_CTR )
//============================================================================================
/**
 * @file		NetappBossUtility.cpp
 *
 * @brief		属性生成ユーティリティ
 */
//============================================================================================

#include "NetStatic/NetAppLib/include/Util/NetAppBossUtility.h"
//#include <message/msg_hologram_mail.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "System/include/PokemonVersion.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)

static const char* BOSS_TYPE_NAME[] =
{
    "MYS_%s_%s",    //不思議な贈り物
    "SER_%s_%s",    //不思議な贈り物（シリアル用）
    "dummy",        //レギュレーション
    "dummy",        //レギュレーション(レート)
    "CUP_%s_%s",    //大会選手証
    "HRE_%s_%s",    //配信レギュレーション
    "dummy",        //グローバルアトラクション
};

static const char* BOSS_REGION_CODE[] =
{
    "JP",
    "US",
    "EU",
    "EU",
    "EU",//"CH",
    "KO",
    "EU",//"TW",
};

static const char* BOSS_LANG_CODE[] =
{
    "E",
    "J",
    "E",
    "F",
    "I",
    "G",
    "E",
    "S",
    "K",
};

static const char* GIFT_ATTRIBUTE_3 = "N";
static const char* GIFT_SERIAL_ATTRIBUTE_2 = "%s";

static const u32 BOSS_SERIAL_MIN[] =
{
    BossUtility::BOSS_APP_TYPE_ID_MIN_gift,
    BossUtility::BOSS_APP_TYPE_ID_MIN_giftSerial,
    BossUtility::BOSS_APP_TYPE_ID_MIN_regulation,
    BossUtility::BOSS_APP_TYPE_ID_MIN_regulationRate,
    BossUtility::BOSS_APP_TYPE_ID_MIN_cup,
    BossUtility::BOSS_APP_TYPE_ID_MIN_regulationDownload,
    BossUtility::BOSS_APP_TYPE_ID_MIN_globalAttraction,
};

static const u32 BOSS_SERIAL_MAX[] =
{
    BossUtility::BOSS_APP_TYPE_ID_MAX_gift,
    BossUtility::BOSS_APP_TYPE_ID_MAX_giftSerial,
    BossUtility::BOSS_APP_TYPE_ID_MAX_regulation,
    BossUtility::BOSS_APP_TYPE_ID_MAX_regulationRate,
    BossUtility::BOSS_APP_TYPE_ID_MAX_cup,
    BossUtility::BOSS_APP_TYPE_ID_MAX_regulationDownload,
    BossUtility::BOSS_APP_TYPE_ID_MAX_globalAttraction,
};

static const char* BOSS_GLOBAL_ATTRACTION_URL =
{
  "global_attraction",// ：JEFIGSK    j_j1  j_j2  j_j3  j_j4  j_j5
};

static const char* BOSS_FREE_REGULATION_URL =
{
  "free_regulation",// ：JEFIGSK    j_j1  j_j2  j_j3  j_j4  j_j5
};

static const char* BOSS_RATE_REGULATION_URL =
{
  "rate_regulation",// ：JEFIGSK    j_j1  j_j2  j_j3  j_j4  j_j5
};

static const char* BOSS_CUP_REGULATION_URL =
{
  "internet_match",// ：JEFIGSK    j_j1  j_j2  j_j3  j_j4  j_j5
};

static const char* BOSS_REGION_URL[] =
{
  "ja",// ：JEFIGSK    j_j1  j_j2  j_j3  j_j4  j_j5
  "en_US",// ：JEGK   us_e1 us_e2 us_e3 us_e4 us_e5
  "es_US",// ：S      us_s1 us_s2 us_s3 us_s4 us_s5
  "fr_US",// ：F      us_f1 us_f2 us_f3 us_f4 us_f5
  "en",// ：JEK      eu_e1 eu_e2 eu_e3 eu_e4 eu_e5
  "fr",// ：F      eu_f1 eu_f2 eu_f3 eu_f4 eu_f5
  "it",// ：I      eu_i1 eu_i2 eu_i3 eu_i4 eu_i5
  "de",// ：G      eu_g1 eu_g2 eu_g3 eu_g4 eu_g5
  "es",// ：S      eu_s1 eu_s2 eu_s3 eu_s4 eu_s5
  "ko",// ：JEFIGSK    ko_k1 ko_k2 ko_k3 ko_k4 ko_k5
};

/*
static const u32 BOSS_ASK_REGIST_MSG_ID[] =
{
    msg_boss_ask_regist_01,
    msg_boss_ask_regist_01_US,
    msg_boss_ask_regist_01_EU,
    msg_boss_ask_regist_01_EU,
    msg_boss_ask_regist_01_UN,
    msg_boss_ask_regist_01_K,
    msg_boss_ask_regist_01_UN,
};
*/

//--------------------------------------------------------------------------------------------
/**
 * @brief		コンストラクタ(隠蔽化)
 */
//--------------------------------------------------------------------------------------------
BossUtility::BossUtility( void )
{
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		デストラクタ(隠蔽化)
 */
//--------------------------------------------------------------------------------------------
BossUtility::~BossUtility(void)
{
}


//--------------------------------------------------------------------------------------------
/**
* @brief    BOSS用属性値文字列の作成
*/
//--------------------------------------------------------------------------------------------
void BossUtility::CreateXYBossAttribute1( BOSS_APP_TYPE appType , char* pOutput )
{
  u8 region = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus()->GetRegionCode();
  u8 lang = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus()->GetPokeLanguageId();

  if( appType < 0 || appType >= BOSS_APP_TYPE_MAX )
  {
    return;
  }

  const char* pTypeName = BOSS_TYPE_NAME[appType];
  const char* pRegion = NULL;
  const char* pLang = NULL;

  switch( region )
  {
    case nn::cfg::CTR::CFG_REGION_JAPAN:
      if( pLang == NULL )
      {
        pLang = BOSS_LANG_CODE[POKEMON_LANG_JAPAN];
      }
    case nn::cfg::CTR::CFG_REGION_AMERICA:
      if( pLang == NULL )
      {
        switch( lang )
        {
          case POKEMON_LANG_JAPAN:
          case POKEMON_LANG_ENGLISH:
          case POKEMON_LANG_ITALY:
          case POKEMON_LANG_GERMANY:
          case POKEMON_LANG_KOREA:
            pLang = BOSS_LANG_CODE[POKEMON_LANG_ENGLISH];
          break;
          case POKEMON_LANG_FRANCE:
            pLang = BOSS_LANG_CODE[POKEMON_LANG_FRANCE];
          break;
          case POKEMON_LANG_SPAIN:
            pLang = BOSS_LANG_CODE[POKEMON_LANG_SPAIN];
          break;
          default:
            pLang = BOSS_LANG_CODE[POKEMON_LANG_ENGLISH];
          break;
        }
      }
    case nn::cfg::CTR::CFG_REGION_EUROPE:
    case nn::cfg::CTR::CFG_REGION_AUSTRALIA:
      if( pLang == NULL )
      {
        switch( lang )
        {
          case POKEMON_LANG_JAPAN:
          case POKEMON_LANG_ENGLISH:
          case POKEMON_LANG_KOREA:
            pLang = BOSS_LANG_CODE[POKEMON_LANG_ENGLISH];
          break;
          case POKEMON_LANG_FRANCE:
            pLang = BOSS_LANG_CODE[POKEMON_LANG_FRANCE];
          break;
          case POKEMON_LANG_ITALY:
            pLang = BOSS_LANG_CODE[POKEMON_LANG_ITALY];
          break;
          case POKEMON_LANG_GERMANY:
            pLang = BOSS_LANG_CODE[POKEMON_LANG_GERMANY];
          break;
          case POKEMON_LANG_SPAIN:
            pLang = BOSS_LANG_CODE[POKEMON_LANG_SPAIN];
          break;
          default:
            pLang = BOSS_LANG_CODE[POKEMON_LANG_ENGLISH];
          break;
        }
      }
    case nn::cfg::CTR::CFG_REGION_CHINA:
      if( pLang == NULL )
      {
        pLang = BOSS_LANG_CODE[POKEMON_LANG_ENGLISH];
      }
    case nn::cfg::CTR::CFG_REGION_KOREA:
      if( pLang == NULL )
      {
        pLang = BOSS_LANG_CODE[POKEMON_LANG_KOREA];
      }
    case nn::cfg::CTR::CFG_REGION_TAIWAN:
      if( pLang == NULL )
      {
        pLang = BOSS_LANG_CODE[POKEMON_LANG_ENGLISH];
      }

      {//リージョン
        pRegion = BOSS_REGION_CODE[region];
      }
    break;
    default://該当なし
    break;
  }

  if( pRegion == NULL || pLang == NULL )
  {
    return;
  }

  std::memset( pOutput , 0 , ATTRIBUTE_BUFFER_SIZE );
  std::snprintf( pOutput , ATTRIBUTE_BUFFER_SIZE , pTypeName , pRegion , pLang );
  GFL_PRINT("BossUtility::CreateXYBossAttribute1 : create attribute[%s]\n",pOutput);
}

//--------------------------------------------------------------------------------------------
/**
* @brief    BOSS用属性値文字列の作成
*/
//--------------------------------------------------------------------------------------------
/*
void BossUtility::CreateXYBossAttribute2( BOSS_APP_TYPE appType , char* pOutput , s32 serialNumber )
{
  if( appType < 0 || appType >= BOSS_APP_TYPE_MAX )
  {
    return;
  }

  if( appType != BOSS_APP_TYPE_giftSerial )
  {//シリアルのみ
    return;
  }

  std::memset( pOutput , 0 , ATTRIBUTE_BUFFER_SIZE );
  std::snprintf( pOutput , GIFT_SERIAL_ATTRIBUTE_2 , serialNumber );
  GFL_PRINT("BossUtility::CreateXYBossAttribute2 : create attribute[%s]\n",pOutput);
}
*/

//--------------------------------------------------------------------------------------------
/**
* @brief    BOSS用属性値文字列の作成
*/
//--------------------------------------------------------------------------------------------
void BossUtility::CreateXYBossAttribute3( BOSS_APP_TYPE appType , char* pOutput )
{
  if( appType < 0 || appType >= BOSS_APP_TYPE_MAX )
  {
    return;
  }

  if( appType != BOSS_APP_TYPE_gift )
  {//通常配信のみ
    return;
  }

  std::memset( pOutput , 0 , ATTRIBUTE_BUFFER_SIZE );
  std::snprintf( pOutput , ATTRIBUTE_BUFFER_SIZE , GIFT_ATTRIBUTE_3 );
  GFL_PRINT("BossUtility::CreateXYBossAttribute3 : create attribute[%s]\n",pOutput);
}

//------------------------------------------------------------------
/**
* @brief シリアルIDの範囲取得（最大）
*/
//------------------------------------------------------------------
u32 BossUtility::GetSerialIdMax( BOSS_APP_TYPE appType )
{
  if( appType < 0 || appType >= BOSS_APP_TYPE_MAX )
  {
    return 0;
  }

  return BOSS_SERIAL_MAX[appType];
}

//------------------------------------------------------------------
/**
* @brief シリアルIDの範囲取得（最少）
*/
//------------------------------------------------------------------
u32 BossUtility::GetSerialIdMin( BOSS_APP_TYPE appType )
{
  if( appType < 0 || appType >= BOSS_APP_TYPE_MAX )
  {
    return 0;
  }

  return BOSS_SERIAL_MIN[appType];
}

//------------------------------------------------------------------
/**
* @brief GlobalAttractionファイル名取得
*/
//------------------------------------------------------------------
const char* BossUtility::GetGlobalAttractionFileName( void )
{
  return  BOSS_GLOBAL_ATTRACTION_URL;
}

//------------------------------------------------------------------
/**
* @brief フリーレギュレーション　ファイル名取得
*/
//------------------------------------------------------------------
const char* BossUtility::GetFreeRagulationFileName( void )
{
  return  BOSS_FREE_REGULATION_URL;
}

//------------------------------------------------------------------
/**
* @brief レートレギュレーション　ファイル名取得
*/
//------------------------------------------------------------------
const char* BossUtility::GetRateRegulationFileName( void )
{
  return  BOSS_RATE_REGULATION_URL;
}

//------------------------------------------------------------------
/**
* @brief インターネット大会　ファイル名取得
*/
//------------------------------------------------------------------
const char* BossUtility::GetInternetMatchFileName( void )
{
  return  BOSS_CUP_REGULATION_URL;
}

//------------------------------------------------------------------
/**
* @brief リージョン用URL取得
*/
//------------------------------------------------------------------
const char* BossUtility::GetRegionURL( void )
{
  //return BOSS_REGION_URL[GetFileNameIndex()];
  return NULL;//BOSSのリージョン設定は無くなった
}

//------------------------------------------------------------------
/**
* @brief ファイルインデックス取得
*/
//------------------------------------------------------------------
u32 BossUtility::GetFileNameIndex( void )
{
  u8 region = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus()->GetRegionCode();
  u8 lang = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus()->GetPokeLanguageId();

  u32 index = 0;

  switch( region )
  {
    case nn::cfg::CTR::CFG_REGION_JAPAN:
        index = 0;
    break;
    case nn::cfg::CTR::CFG_REGION_AMERICA:
        switch( lang )
        {
          case POKEMON_LANG_JAPAN:
          case POKEMON_LANG_ENGLISH:
          case POKEMON_LANG_ITALY:
          case POKEMON_LANG_GERMANY:
          case POKEMON_LANG_KOREA:
            index = 1;
          break;
          case POKEMON_LANG_FRANCE:
            index = 3;
          break;
          case POKEMON_LANG_SPAIN:
            index = 2;
          break;
          default:
            index = 1;
          break;
        }
    break;
    case nn::cfg::CTR::CFG_REGION_EUROPE:
    case nn::cfg::CTR::CFG_REGION_AUSTRALIA:
        switch( lang )
        {
          case POKEMON_LANG_JAPAN:
          case POKEMON_LANG_ENGLISH:
          case POKEMON_LANG_KOREA:
            index = 4;
          break;
          case POKEMON_LANG_FRANCE:
            index = 5;
          break;
          case POKEMON_LANG_ITALY:
            index = 6;
          break;
          case POKEMON_LANG_GERMANY:
            index = 7;
          break;
          case POKEMON_LANG_SPAIN:
            index = 8;
          break;
          default:
            index = 4;
          break;
        }
    break;
    case nn::cfg::CTR::CFG_REGION_CHINA:
        index = 4;
    break;
    case nn::cfg::CTR::CFG_REGION_KOREA:
        index = 9;
    break;
    case nn::cfg::CTR::CFG_REGION_TAIWAN:
        index = 4;
    break;
    default://該当なし
    break;
  }

  return index;
}


GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)

#endif // defined( GF_PLATFORM_CTR )
