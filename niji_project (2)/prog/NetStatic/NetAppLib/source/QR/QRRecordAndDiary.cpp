//==============================================================================
/**
 * @file   QRRecordAndDiary.cpp
 * @date   2016/02/18 Thu. 17:39:39
 * @author muto_yu
 * @brief  
 *   diary:
 *    0 バトルチームのＱＲコード
 *    1 仲間大会用ＱＲコード
 *    2 ライブ大会用ＱＲコード
 *    3 図鑑ＱＲコード
 *    4 外部連携ＱＲコード
 *    5 一般のＱＲコード
 * 
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
/*  record/diary  */
#include  "Savedata/include/Record.h"
#include  "Savedata/include/PokeDiarySave.h"

#include  "../../include/QR/QRRecordAndDiary.h"

// =============================================================================
/**/
//==============================================================================

namespace
{
  static const u16  DiaryCode_BattleTeam  = 0;
  static const u16  DiaryCode_FriendMatch = 1;
  static const u16  DiaryCode_LiveMatch   = 2;
  static const u16  DiaryCode_Zukan       = 3;
  static const u16  DiaryCode_Official    = 4;
  static const u16  DiaryCode_Generally   = 5;
}


namespace NetApp  {
namespace QR      {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void QRRecordFunc(const QR_TYPE_APP dataType)
{
  switch(dataType)
  {
    case  QR_TYPE_APP_ALOLA_NEW:                                    //アローラ新規
    case  QR_TYPE_APP_ALOLA_NEW_OFFICIAL:                           //アローラ新規（20％チャージ）
    case  QR_TYPE_APP_ALOLA_UPDATE:                                 //アローラ更新
    case  QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL:                        //アローラ更新（20％チャージ）
    case  QR_TYPE_APP_OFFICIAL:                                     //アローラ外（公式QR）
    case  QR_TYPE_APP_NORMAL:                                       //アローラ外（一般QR）
    case  QR_TYPE_APP_ALOLA_NEW_OFFICIAL_MAGIANA:                   /**<  マギアナOK: 図鑑未登録/マギアナイベント終了前  */
    case  QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_MAGIANA:                /**<  マギアナOK: 図鑑登録済/マギアナイベント終了前  */
    case  QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_MAGIANA_AFTER_EVENT:    /**<  マギアナOK: 図鑑登録済/マギアナイベント終了後  */
    case  QR_TYPE_APP_ALOLA_NEW_OFFICIAL_M20PIKATYU:                /**<  M20ピカチュウOK: 図鑑未登録/M20ピカチュウイベント終了前  */
    case  QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_M20PIKATYU:             /**<  M20ピカチュウOK: 図鑑登録済/M20ピカチュウイベント終了前  */
    case  QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_M20PIKATYU_AFTER_EVENT: /**<  M20ピカチュウOK: 図鑑登録済/M20ピカチュウイベント終了後  */
      Savedata::IncRecord(Savedata::Record::RECID_QR_SCAN);                 ///< QRスキャンした回数                                          9,999 30
      break;
  }

  switch(dataType)
  {
    case  QR::QR_TYPE_APP_ALOLA_NEW:            //アローラ新規
    case  QR::QR_TYPE_APP_ALOLA_UPDATE:         //アローラ更新
      Savedata::IncRecord(Savedata::Record::RECID_QR_READ_PICTURE);         ///< ＱＲスキャン：ＱＲコード（図鑑ＱＲ）を読み込んだ回数        9,999
      break;
  }


  switch(dataType)
  {
    case  QR_TYPE_APP_NORMAL:                   //アローラ外（一般QR）
      Savedata::IncRecord(Savedata::Record::RECID_QR_READ_POPULAR);         ///< ＱＲスキャン：ＱＲコード（一般）を読み込んだ回数            9,999
      break;
  }

  switch(dataType)
  {
    case  QR_TYPE_APP_ALOLA_NEW_OFFICIAL:                           //アローラ新規（20％チャージ）
    case  QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL:                        //アローラ更新（20％チャージ）
    case  QR_TYPE_APP_OFFICIAL:                                     //アローラ外（公式QR）
    case  QR_TYPE_APP_ALOLA_NEW_OFFICIAL_MAGIANA:                   /**<  マギアナOK: 図鑑未登録/マギアナイベント終了前  */
    case  QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_MAGIANA:                /**<  マギアナOK: 図鑑登録済/マギアナイベント終了前  */
    case  QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_MAGIANA_AFTER_EVENT:    /**<  マギアナOK: 図鑑登録済/マギアナイベント終了後  */
    case  QR_TYPE_APP_ALOLA_NEW_OFFICIAL_M20PIKATYU:                /**<  M20ピカチュウOK: 図鑑未登録/M20ピカチュウイベント終了前  */
    case  QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_M20PIKATYU:             /**<  M20ピカチュウOK: 図鑑登録済/M20ピカチュウイベント終了前  */
    case  QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_M20PIKATYU_AFTER_EVENT: /**<  M20ピカチュウOK: 図鑑登録済/M20ピカチュウイベント終了後  */
      Savedata::IncRecord(Savedata::Record::RECID_QR_READ_OUTSIDE);         ///< ＱＲスキャン：ＱＲコード（外部連携系）を読み込んだ回数      9,999
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void FieldScanRecordFunc(void)
{
  Savedata::IncRecord(Savedata::Record::RECID_FIELDSCAN);         // RECID_FIELDSCAN,                    ＱＲスキャン：フィールドスキャンを発動した回数              9,999
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void QRDiaryFunc(const QR_TYPE_APP  dataType)
{

  switch(dataType)
  {
    case  QR::QR_TYPE_APP_ALOLA_NEW:            //アローラ新規
    case  QR::QR_TYPE_APP_ALOLA_UPDATE:         //アローラ更新
      PokeDiary::Set(PDID_COUNT_QR, DiaryCode_Zukan);
      break;
  }


  switch(dataType)
  {
    case  QR_TYPE_APP_NORMAL:                   //アローラ外（一般QR）
      PokeDiary::Set(PDID_COUNT_QR, DiaryCode_Generally);
      break;
  }

  switch(dataType)
  {
    case  QR_TYPE_APP_ALOLA_NEW_OFFICIAL:       //アローラ新規（20％チャージ）
    case  QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL:    //アローラ更新（20％チャージ）
    case  QR_TYPE_APP_OFFICIAL:                 //アローラ外（公式QR）
      PokeDiary::Set(PDID_COUNT_QR, DiaryCode_Official);
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void QRDiaryFunc(const QR_TYPE_BATTLE_TEAM dataType)
{
  if(dataType == QR_TYPE_BATTLE_TEAM_OK)
  {
    Savedata::IncRecord(Savedata::Record::RECID_QR_READ_BATTLE);
    PokeDiary::Set(PDID_COUNT_QR, DiaryCode_BattleTeam);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void QRDiaryFunc(const QR_TYPE_FRIEND_CUP  dataType)
{
  if(dataType == QR_TYPE_FRIEND_CUP_OK)
  {
    PokeDiary::Set(PDID_COUNT_QR, DiaryCode_FriendMatch);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void QRDiaryFunc(const QR_TYPE_LIVE_CUP    dataType)
{
  if(dataType == QR_TYPE_LIVE_CUP_OK)
  {
    PokeDiary::Set(PDID_COUNT_QR, DiaryCode_LiveMatch);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
u32  GetQRScanCount(void)
{
  const Savedata::Record::RECORD_ID targetRecordIDs[] =
  {
    Savedata::Record::RECID_QR_SCAN,
  };
  u32   total = 0;


  for(u32 index = 0; index < GFL_NELEMS(targetRecordIDs); ++index)
  {
    total += Savedata::GetRecord(targetRecordIDs[index]);
  }


  return total;
}


} /*  namespace QR      */
} /*  namespace NetApp  */
