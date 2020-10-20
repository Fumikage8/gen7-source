//==============================================================================
/**
 * @file   QRReaderFieldFlagUtility.cpp
 * @date   2016/01/22 Fri. 12:56:15
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <debug/include/gfl2_Assert.h>

/*  gamesys  */
#include  "GameSys/include/GameData.h"
#include  "GameSys/include/NijiTime.h"

/*  field  */
#include  "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include  "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include  "Field/FieldStatic/include/Zone/ZoneDataLoaderTypes.h"
#include  "Field/FieldStatic/include/Encount/FieldEncountUtil.h"
//#include  "Field/FieldRo/include/Encount/FieldEncountData.h"
#include  <niji_conv_header/field/field_resident/field_resident_pack.h>
#include  "Field/FieldStatic/include/PokemonSearch/FieldPokemonSearchDataAccessor.h"

#include  "Savedata/include/EventWork.h"

#include  "NetStatic/NetAppLib/include/QR/QRRecordAndDiary.h"

#include  "../../System/QRReaderWork.h"
#include  "../../QRReaderResource.h"
#include  "./QRReaderFieldFlagUtility.h"


// =============================================================================
/**/
//==============================================================================
namespace {
  static const u32  FieldScanFlagTableNum = 4;  /**<  島x曜日テーブルの要素数  */

  /**
    島ごとに、曜日別フラグIDを持つ

    ＜島１＞
      島1日曜/2ばんどうろ・草むら                         //ZONE_ID:Z_F0102_S0401   //PLACE_NAME_ID:PLACENAME_Z_F0201_S0401_01
      島1月曜/3ばんどうろ・草むら                         //ZONE_ID:Z_F0102_S0301   //PLACE_NAME_ID:PLACENAME_Z_F0201_S0301_01
      島1火曜/うみつなぎのどうけつ・洞窟                  //ZONE_ID:Z_F0102D0502    //PLACE_NAME_ID:PLACENAME_Z_F0102D0502_01
      島1水曜/テンカラットヒル・洞窟                      //ZONE_ID:Z_F0102D0301    //PLACE_NAME_ID:PLACENAME_Z_F0102D0301_01
      島1木曜/ハウオリ霊園・草むら                        //ZONE_ID:Z_F0102D0401    //PLACENAME_Z_F0102D0401_01
      島1金曜/ハウオリシティ：ショッピングエリア・草むら  //ZONE_ID:Z_F0102_C0102 //PLACENAME_Z_F0102_C0102_01
      島1土曜/カーラエわん・海                            //ZONE_ID:Z_F0102_S0501 //PLACENAME_Z_F0102_S0501_01

    ＜島２＞
      島2日曜/せせらぎの丘・草むら                        //ZONE_ID:Z_F0201H0301  ＆Z_F0201H0302 //PLACE_NAME_ID:PLACENAME_Z_F0201H0301_01
      島2月曜/４番道路・草むら                            //ZONE_ID:Z_F0201_S0101 //PLACE_NAME_ID:PLACENAME_Z_F0201_S0101_01
      島2火曜/アーカラアイランドはずれ                    //ZONE_ID:Z_F0201D0302  //PLACE_NAME_ID:PLACENAME_Z_F0201D0302_01
      島2水曜/５番道路・草むら                            //ZONE_ID:Z_F0201_S0201 //PLACE_NAME_ID:PLACENAME_Z_F0201_S0201_01
      島2木曜/８番道路・草むら                            //ZONE_ID:Z_F0201_S0701 //PLACE_NAME_ID:PLACENAME_Z_F0201_S0701_01
      島2金曜/７番道路・海                                //ZONE_ID:Z_F0201_S0401 //PLACE_NAME_ID:PLACENAME_Z_F0201_S0401_01
      島2土曜/６番道路・草むら                            //ZONE_ID:Z_F0201_S0301 //PLACE_NAME_ID:PLACENAME_Z_F0201_S0301_01

    ＜島３＞
      島3日曜/ウラウラの花園・草むら　                    //ZONE_ID:Z_F0301D0201  //PLACE_NAME_ID:PLACENAME_Z_F0301D0201_01
      島3月曜/ホテリ山・草むら（枯れ草）                  //ZONE_ID:Z_F0301D0601  //PLACE_NAME_ID:PLACENAME_Z_F0301D0601_01
      島3火曜/10番道路・草むら　                          //ZONE_ID:Z_F0301_S0101 //PLACE_NAME_ID:PLACENAME_Z_F0301_S0101_01
      島3水曜/14番道路・草むら　                          //ZONE_ID:Z_F0301_S0501 //PLACE_NAME_ID:PLACENAME_Z_F0301_S0501_01
      島3木曜/16番道路・草むら　                          //ZONE_ID:Z_F0301_S0801 //PLACE_NAME_ID:PLACENAME_Z_F0301_S0801_01
      島3金曜/ホクラニだけ・草むら　                      //ZONE_ID:Z_F0301D0501  //PLACE_NAME_ID:PLACENAME_Z_F0301D0501_01
      島3土曜/11番道路・草むら　                          //ZONE_ID:Z_F0301_S0201 //PLACE_NAME_ID:PLACENAME_Z_F0301_S0201_01

    ＜島４＞
      島4日曜/ゾーンC(ポニのじゅりん)・草むら　           //ZONE_ID:Z_Z_F0401_S0401 //PLACE_NAME_ID:PLACENAME_Z_F0401_S0401_01
      島4月曜/ゾーンD(ポニのこうや)・草むら　             //ZONE_ID:Z_Z_F0401_S0501 //PLACE_NAME_ID:PLACENAME_Z_F0401_S0501_01
      島4火曜/ゾーンA（ポニのげんや）・草むら　           //ZONE_ID:Z_Z_F0401_S0101 //PLACE_NAME_ID:PLACENAME_Z_F0401_S0101_01
      島4水曜/ゾーンB（ポニのこどう）・草むら　           //ZONE_ID:Z_Z_F0401_S0201 //PLACE_NAME_ID:PLACENAME_Z_F0401_S0201_01
      島4木曜/ゾーンF（ポニのけんろ）・草むら　           //ZONE_ID:Z_Z_F0401_S0701 //PLACE_NAME_ID:PLACENAME_Z_F0401_S0701_01
      島4金曜/メレメレの花園・草むら　                    //ZONE_ID:Z_Z_F0401D0101 //PLACE_NAME_ID:PLACENAME_Z_F0401D0101_01
      島4土曜/ナッシーアイランド・草むら　                //ZONE_ID:Z_Z_F0401D0801 //PLACENAME_Z_F0401D0801_01


      2016/05/19  Yu Muto
      寺地さん要望によりテーブル変更

        NMcat #2280 島スキャンのポケモンとUBが同時に出現した際の挙動について

        出現場所を変更いたします。
        お手数ですが、地名を表示するテキストラベルの差し替えをお願いしたく思います。
        島２の水曜の設定で、「アーカラじまはずれ」が表示されれば正しいです。

        新：SYS_FLAG_POKE_SEARCH_SCAN_10→PLACENAME_Z_F0201D0302_01
        旧：SYS_FLAG_POKE_SEARCH_SCAN_10→PLACENAME_Z_F0201D0301_01
  */
  static const struct FieldScanFlagTable
  {
    struct WeekdayInfo
    {
      u16      fieldFlagID;
      ZoneID   zoneID;
//      MonsNo          targetMonsNo;
    }weekdayInfo[7];   /**<  曜日ごとのフラグ  */
  }fieldScanFlagTable[FieldScanFlagTableNum]  = {
    /*  island:0  */
    {
      {
        {SYS_FLAG_POKE_SEARCH_SCAN_00,  Z_F0102_S0301,  /*  MONSNO_SUPIAA,     */ },    /*  SUN.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_01,  Z_F0102D0502,   /*  MONSNO_BATAHURII,  */ },    /*  MON.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_02,  Z_F0102D0301,   /*  MONSNO_DOKUKEIRU,  */ },    /*  TUE.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_03,  Z_F0102_S0501,  /*  MONSNO_RUNPAPPA,   */ },    /*  WED.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_04,  Z_F0102_C0102,  /*  MONSNO_DAATENGU,   */ },    /*  THU.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_05,  Z_F0102_S0401,  /*  MONSNO_ONIDORIRU,  */ },    /*  FRI.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_06,  Z_F0102D0401,   /*  MONSNO_AABOKKU,    */ },    /*  SAT.  */
      },
    },
    /*  island:1  */
    {
      {
        {SYS_FLAG_POKE_SEARCH_SCAN_07,  Z_F0201_S0301,  /*  MONSNO_GIGAIASU,   */ },    /*  SUN.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_08,  Z_F0201_S0401,  /*  MONSNO_ROOBUSIN,   */ },    /*  MON.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_09,  Z_F0201_S0701,  /*  MONSNO_DORYUUZU,   */ },    /*  TUE.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_10,  Z_F0201D0302,   /*  MONSNO_GAMAGEROGE, */ },    /*  WED.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_11,  Z_F0201_S0101,  /*  MONSNO_HUSIGIBANA, */ },    /*  THU.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_12,  Z_F0201_S0201,  /*  MONSNO_RIZAADON,   */ },    /*  FRI.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_13,  Z_F0201H0301,   /*  MONSNO_KAMEKKUSU,  */ },    /*  SAT.  */
      },
    },
    /*  island:2  */
    {
      {
        {SYS_FLAG_POKE_SEARCH_SCAN_14,  Z_F0301D0601,   /*  MONSNO_GIGAIASU,   */ },   /*  SUN.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_15,  Z_F0301_S0501,  /*  MONSNO_ROOBUSIN,   */ },   /*  MON.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_16,  Z_F0301_S0801,  /*  MONSNO_DORYUUZU,   */ },   /*  TUE.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_17,  Z_F0301D0201,   /*  MONSNO_GAMAGEROGE, */ },   /*  WED.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_18,  Z_F0301_S0101,  /*  MONSNO_HUSIGIBANA, */ },   /*  THU.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_19,  Z_F0301_S0201,  /*  MONSNO_RIZAADON,   */ },   /*  FRI.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_20,  Z_F0301D0501,   /*  MONSNO_KAMEKKUSU,  */ },   /*  SAT.  */
      },
    },
    /*  island:3  */
    {
      {
        {SYS_FLAG_POKE_SEARCH_SCAN_21,  Z_F0401_S0401,  /*  MONSNO_GIGAIASU,   */ },   /*  SUN.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_22,  Z_F0401_S0501,  /*  MONSNO_ROOBUSIN,   */ },   /*  MON.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_23,  Z_F0401_S0701,  /*  MONSNO_DORYUUZU,   */ },   /*  TUE.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_24,  Z_F0401D0101,   /*  MONSNO_GAMAGEROGE, */ },   /*  WED.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_25,  Z_F0401D0801,   /*  MONSNO_HUSIGIBANA, */ },   /*  THU.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_26,  Z_F0401_S0101,  /*  MONSNO_RIZAADON,   */ },   /*  FRI.  */
        {SYS_FLAG_POKE_SEARCH_SCAN_27,  Z_F0401_S0201   /*  MONSNO_KAMEKKUSU,  */ },   /*  SAT.  */
      },
    },
  };

  
  bool  IsValidWeekday(const s32 weekday)                 {return ((0 <= weekday) && (weekday <= 6));}
  bool  IsFieldScanValidIslandID(const u8 islandID)       {return ((1 <=islandID) && (islandID <= 4));}   /**<  [1, 5]がゲーム的に有効な値で、FieldScanが必要な島は1,2,3,4  */
  u32   IslandIDToTableIndex(const u8 islandID)
        {
          const bool  isValidID = IsFieldScanValidIslandID(islandID);

          GFL_ASSERT(isValidID);
          return (isValidID ? (islandID - 1) : 0);
        }
  const FieldScanFlagTable::WeekdayInfo&  GetWeekdayInfo(const s32 weekday, const u8 islandID)
        {
          return  (fieldScanFlagTable[IslandIDToTableIndex(islandID)].weekdayInfo[weekday]);
        }
  
} /*  namespace  */


namespace NetApp    {
namespace QRReader  {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool FieldFlagUtility::DoFieldScan(QRReaderWork* pWork, u8* pIslandID, Field::ZoneID* pZoneID, MonsNo* pTargetMonsNo, FormNo* pTargetFormNo, pml::Sex* pTargetGender)
{
  Savedata::QRReaderSaveData*   pQRReaderSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetQRReaderSave();


  if(!pQRReaderSaveData->IsScanPowerEnoughForFieldScan()) return false;   /**<  条件を満たしていない  */

  /*  成功したらFieldEvent側のフラグを立てる  */


  /**
    @note
      gfl2::system::Date::GetWeek() が nn::fnd::DateTime::GetWeek()の値を直接返してくる
      型は nn::fnd::Week で、本来は enumで評価する
      ここでは0:SUN. - 6:SAT. として扱う
  */
  GameSys::DeviceDate currentDate;    /**<  現在時刻  */
  const s32           weekday = currentDate.GetDate()->GetWeek();

  /*  weekdayチェック */
  {
    const bool  isValidWeekday = IsValidWeekday(weekday);
  
    GFL_ASSERT(isValidWeekday);
    if(!isValidWeekday)  return false;
  }


  GameSys::GameData*      pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  const Field::ZoneID     zoneId    = pGameData->GetNowZoneID();
  void*                   pZoneData = pGameData->GetFieldZoneDataLoader()->GetZoneData(zoneId);
  Field::ZoneDataAccessor zoneDataAccessor(pZoneData);
  const u8                islandID  = zoneDataAccessor.GetIslandIndex();

  /*  islandIDチェック */
  if(!IsFieldScanValidIslandID(islandID))  return false;


  const FieldScanFlagTable::WeekdayInfo&  rWeekdayInfo  = GetWeekdayInfo(weekday, islandID);

  /*  (weekday, islandID) -> flagset  */
  {
    Field::EventWork*   pEventWork    = pGameData->GetEventWork();
  
    pEventWork->SetEventFlag(rWeekdayInfo.fieldFlagID);
  }

  /*  output  */
  {
    void*                               pFieldResidentData  = pWork->GetResourceManager()->GetResourceBuffer(RESOURCE_FieldResident);
    u32                                 symbolEncountID;
    Field::Encount::SymbolEncountParam  symbol_encount_param;
    gfl2::fs::BinLinkerAccessor         binLinkerAccessor;


    binLinkerAccessor.Initialize(pFieldResidentData);
    {
      Field::PokemonSearch::DataAccessor  searchDataAccessor(binLinkerAccessor.GetData(BL_IDX_FIELD_RESIDENT__POKEMON_SEARCH_PACK));

      symbolEncountID = searchDataAccessor.GetSymbolEncountDataID(rWeekdayInfo.fieldFlagID);
    }

    Field::Encount::EncountUtil::SyncLoadSymbolEncountParam(pWork->GetDeviceHeap(), GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(), symbolEncountID, &symbol_encount_param);

    const MonsNo    monsNo  = static_cast<MonsNo>(symbol_encount_param.monsNo);
    const FormNo    formNo  = static_cast<FormNo>(symbol_encount_param.form);
    const pml::Sex  gender  = static_cast<pml::Sex>(symbol_encount_param.sex);


    if(pIslandID)     *pIslandID      = islandID;
    if(pZoneID)       *pZoneID        = rWeekdayInfo.zoneID;  /**<  対象ポケモンの出現エリアのID  */
    if(pTargetMonsNo) *pTargetMonsNo  = monsNo;
    if(pTargetFormNo) *pTargetFormNo  = formNo;
    if(pTargetGender) *pTargetGender  = gender;

    /*  結果をキャッシュ  */
    pQRReaderSaveData->SetFieldScanTargetInfo(islandID, rWeekdayInfo.zoneID, monsNo, formNo, gender);
  }

  pQRReaderSaveData->ActivateFieldScan();
  NetApp::QR::FieldScanRecordFunc();      /*  record  */

  return true;  /*  success */
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
u32  FieldFlagUtility::GetIslandID(void)
{
  GameSys::GameData*      pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  const Field::ZoneID     zoneId    = pGameData->GetNowZoneID();
  void*                   pZoneData = pGameData->GetFieldZoneDataLoader()->GetZoneData(zoneId);
  Field::ZoneDataAccessor zoneDataAccessor(pZoneData);
  const u8                islandID  = zoneDataAccessor.GetIslandIndex();


  return islandID;
}


//------------------------------------------------------------------
/**
  *  @brief    
  *  @return   trueは継続中かtimeout
  */
//------------------------------------------------------------------
bool FieldFlagUtility::QueryFieldScan(u32* pRemainingMinutes, u32* pRemainingSeconds)
{
  Savedata::QRReaderSaveData*                     pQRReaderSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetQRReaderSave();
  Savedata::QRReaderSaveData::FieldScanQueryInfo  info;


  pQRReaderSaveData->FieldScanQuery(NULL, &info);

  if(pRemainingMinutes)
  {
    *pRemainingMinutes  = info.RemainingMinutes();
  }

  if(pRemainingSeconds)
  {
    *pRemainingSeconds  = info.RemainingSeconds();
  }

  if(!info.IsActive())
  {
    FieldScan_FlagClear();
  }

  return (info.IsActiveORTimeout());
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  FieldFlagUtility::FieldScan_FlagClear(void)
{
  GameSys::GameData*  pGameData   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  Field::EventWork*   pEventWork  = pGameData->GetEventWork();

  for(u32 infoIndex = 0; infoIndex < FieldScanFlagTableNum; ++infoIndex)
  {
    for(u32 weekday = 0; weekday < 7; ++weekday)
    {
      pEventWork->ResetEventFlag(fieldScanFlagTable[infoIndex].weekdayInfo[weekday].fieldFlagID);
    }
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
u32 FieldFlagUtility::IslandIDtoIslandNameMessageID(const u8 islandID)
{
  switch(islandID)
  {
    case  2:  return qrscan_sys_02_03;    /**<  アーカラアイランド  */
    case  3:  return qrscan_sys_02_04;    /**<  ウラウラアイランド  */
    case  4:  return qrscan_sys_02_05;    /**<  ポニアイランド      */
    case  5:  return qrscan_sys_02_10;    /**<  エーテルパラダイス */
    case  7:  return qrscan_sys_02_11;    /**<  ウルトラスペース   */

    case  1:
    default:
      return  qrscan_sys_02_02;   /**<  メレメレアイランド  */
  }
}



} /*  namespace QRReader  */
} /*  namespace NetApp    */
