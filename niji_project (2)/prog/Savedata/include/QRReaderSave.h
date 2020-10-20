#if !defined(QRREADERSAVE_H_INCLUDED)
#define QRREADERSAVE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   QRReaderSave.h
 * @date   2016/01/19 Tue. 20:25:09
 * @author muto_yu
 * @brief  QR読み込みセーブ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>


/*  gamesys  */
#include  "GameSys/include/NijiTime.h"
#include  "Savedata/include/SaveDataInterface.h"

namespace Savedata  {
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/01/19 Tue. 20:25:53
  */
//==============================================================================
class QRReaderSaveData
  : public  SaveDataInterface
{
  GFL_FORBID_COPY_AND_ASSIGN(QRReaderSaveData);
public:
  enum  FieldScanStatus
  {
    FIELDSCAN_STATUS_InActive,  /**<  未起動                              */
    FIELDSCAN_STATUS_Timeout,   /**<  起動していたが、継続できなくなった  */
    FIELDSCAN_STATUS_Active,    /**<  起動状態                            */

    FIELDSCAN_STATUS_MAX
  };



  static const u32  QRREAD_HISTORY_MAX  = 10;


  struct CoreData
  {
    /*  4  */
    b8    bTutorialFinished;    /**<  チュートリアルが終了した        */
    u8    fieldScanStatus;
    u8    battery;              /**<  QRスキャンのためのバッテリー値  */
    u8    scanPower;            /**<  FieldScanのための解析パワー値   */

    /*  4  */
    u16   timer1;               /**<  ※未使用  */
    u16   timer2;               /**<  ※未使用  */

    /*  8  */
    s64   fieldSacnStartDate;        /**<  FieldScan開始日時：  2000/01/01 00:00:00 からのオフセット秒  */

    /*  32 * 10  = 320  */
    /*  スキャン履歴  */
    struct ScanDataHistory
    {
      u8  sha256[32];
    }histories[QRREAD_HISTORY_MAX];

    /*  4  */
    u32   historyCount;         /**<  履歴蓄積数、QRREAD_HISTORY_MAX以上もカウントしていく  */

    /*  FieldScanターゲットのキャッシュ  */
    /*  10  */
    struct  FieldScanTargetInfo
    {
      u16   islandID;
      u16   zoneID;
      u16   monsNo;
      u16   formNo;
      u16   gender;
    }fieldScanTargetInfo;

    /*  2 */
    u16   __padding;             /**<  8bytes align */

    /*  8  */
    s64   batteryChangeDate;     /**<  battery値が最後に変動した日時：  2000/01/01 00:00:00 からのオフセット秒  */

    /*  8  */
    u64   isReadMagianaQR;       /**<  マギアナQR読んだかマジックナンバー     */
    u64   isReadM20PikatyuQR;    /**<  M20ピカチュウ読んだかマジックナンバー  */

    u8    __reserve[456-(4+4+8+320+4+10+2+8+8+8)];
  };

  static const u32  COREDATA_BYTES  = sizeof(CoreData);


  /*
    general
  */
  bool  IsTutorialFinished(void) const    {return (m_CoreData.bTutorialFinished != 0);}
  void  TutorialFinished(void)            {m_CoreData.bTutorialFinished = true;}


  /*
    qrread
  */
  static const u32  QRREAD_BATTERY_MAX                      = 100;              /**<  バッテリー値最大                      */
  static const u32  QRREAD_BATTERY_REQUIREMENT              =  10;              /**<  読み込みに必要なバッテリー量          */
  static const u32  QRREAD_BATTERY_CHARGE_SECOND            = (3600 * 2) / 10;  /**<  バッテリー値を1回復するのに必要な秒数 */
  static const u32  QRREAD_BATTERY_FULLCHARGE_SECOND        = QRREAD_BATTERY_CHARGE_SECOND * QRREAD_BATTERY_MAX;
  static const u32  QRREAD_SCANPOWER_GAIN_VALUE_NORMAL      = 10;               /**<  通常QRのScanPower回復量               */
  static const u32  QRREAD_SCANPOWER_GAIN_VALUE_PROMOTIONAL = 20;               /**<  プロモーションQRのScanPower回復量     */

  void  RegisterScannedDataHistory(gfl2::heap::HeapBase* pTempHeapBase, const void* pScannedData, const u32 dataBytes);     /**<  読み取り履歴に追加する                      */
  bool  IsRegisteredData(gfl2::heap::HeapBase* pTempHeapBase, const void* pScannedData, const u32 dataBytes) const;         /**<  読み取り履歴に追加されているデータであるか  */
  void  ResetScannedDataHistory(void);                                                                                      /**<  データ履歴のリセット                        */

  /*  battery  */
  u8    BatteryQuery(void);
  u8    GetBattery(void) const                  {return m_CoreData.battery;}
  void  ResetBatteryChangeDate(const s64 targetDate);
  bool  IsBatteryFull(void) const               {return (GetBattery() == QRREAD_BATTERY_MAX);}
  bool  IsBatteryEnoughForQRRead(void) const    {return (QRREAD_BATTERY_REQUIREMENT <= GetBattery());}    /**<  バッテリー残量がQRReadに必要な分以上に残っているか  */
  bool  UseBatteryOnQRScan(void);                                                                         /**<  QRScanの実行によるバッテリー消費                    */

  /*  magiana  */
  void  ReadMagianaQR(void);
  bool  IsReadMagianaQR(void);

  /*  m20pika  */
  void  ReadM20PikaQR(void);
  bool  IsReadM20PikaQR(void);


  /*
    fieldscan
  */
  static const s64  FIELDSCAN_DURATION_SECONDS  = 3600;                       /**<  FieldScan寿命  */
  static const u8   FIELDSCAN_SCANPOWER_MAX     = 100;
  static const u8   FIELDSCAN_POWER_REQUIREMENT = FIELDSCAN_SCANPOWER_MAX;    /**<  FieldScan発動に必要なScanPower  */


  //==============================================================================
  /**
    *  @brief  query情報
    *  @date   2016/02/03 Wed. 16:52:59
    */
  //==============================================================================
  struct  FieldScanQueryInfo
  {
    s64             currentDate;        /**<  現在時刻：  2000/01/01 00:00:00 からのオフセット秒  */
    s64             endDate;            /**<  終了時刻：  2000/01/01 00:00:00 からのオフセット秒（fieldSacnStartDate + FIELDSCAN_DURATION_SECONDS）  */
    u64             remainingSeconds;   /**<  残り時間（endDate - currentDate） */
    FieldScanStatus status;             /**<  動作状態                          */
    
    /*  funcs  */
    u32   RemainingMinutes(void) const  {return (remainingSeconds / 60UL);}
    u32   RemainingSeconds(void) const  {return (remainingSeconds % 60UL);}
    bool  IsActive(void) const          {return (status == FIELDSCAN_STATUS_Active);}
    bool  IsTimeout(void) const         {return (status == FIELDSCAN_STATUS_Timeout);}
    bool  IsActiveORTimeout(void) const {return (IsActive() || IsTimeout());}

    FieldScanQueryInfo(void)
      : currentDate(0)
      , endDate(0)
      , remainingSeconds(0)
      , status(FIELDSCAN_STATUS_InActive)
    {}

    void  InitializeFromCoreData(const CoreData& rCoreData);
  };

  //==============================================================================
  /**
    *  @brief  Query結果を受け取るハンドラ
    *  @date   2016/02/03 Wed. 16:52:00
    */
  //==============================================================================
  class IFieldScanQueryHandler
  {
  public:
    virtual void  FieldScan_OnQuery(const FieldScanStatus fieldScanStatus, const FieldScanQueryInfo& rInfo) = 0;

    virtual ~IFieldScanQueryHandler()  {}
  };
  


  //------------------------------------------------------------------
  /**
    *  @brief    FieldScan内部状態の更新と問い合わせ
    *            active時、endDateに到達したらtimeoutへ遷移する
    *  @param    [in] pHandler  NULL可、Query結果をハンドラで受け取りたい場合は設定する
    *  @param    [in] pInfo     NULL可、現在の状態を受け取りたい場合は設定する
    */
  //------------------------------------------------------------------
  FieldScanStatus   FieldScanQuery(IFieldScanQueryHandler* pHandler = NULL, FieldScanQueryInfo* pInfo = NULL);


  //------------------------------------------------------------------
  /**
    *  @brief    FieldScan内部状態の取得
    *            FieldScanQueryの内部状態更新を行わない版
    */
  //------------------------------------------------------------------
  FieldScanStatus   GetFieldScanStatus(IFieldScanQueryHandler* pHandler = NULL, FieldScanQueryInfo* pInfo = NULL) const;


  //------------------------------------------------------------------
  /**
    *  @brief    FieldScanを発動する
    *  @param    [in] isForced  scanPower値に関わらず強制的に発動する
    *  @return   発動できたか
    */
  //------------------------------------------------------------------
  bool  ActivateFieldScan(const bool isForced = false);

  //------------------------------------------------------------------
  /**
    *  @brief    FieldScanを無効化する
    *  @note     （Query結果が FIELDSCAN_STATUS_Timeout であった場合に）本関数をコールすることで各種フラグがクリアされ、
                 以後 FIELDSCAN_STATUS_InActive が返るようになる
    */
  //------------------------------------------------------------------
  void  InactivateFieldScan(void);

  //------------------------------------------------------------------
  /**
    *  @brief    FieldScanがActive状態であるか
    */
  //------------------------------------------------------------------
  bool  IsFieldScanActive(void) const {return (GetFieldScanStatus() == FIELDSCAN_STATUS_Active);}


  //------------------------------------------------------------------
  /**
    *  @brief    FieldScanで気配を察知した対象の情報をキャッシュする（シルエット表示用）
    */
  //------------------------------------------------------------------
  void  SetFieldScanTargetInfo(const u16 islandID, const u16 zoneID, const MonsNo monsNo, const FormNo formNo, const pml::Sex gender);

  //------------------------------------------------------------------
  /**
    *  @brief    FieldScanで気配を察知した対象の情報のキャッシュを取得する（シルエット表示用）
    */
  //------------------------------------------------------------------
  void  GetFieldScanTargetInfo(u16* pIslandID, u16* pZoneID, MonsNo* pMonsNo, FormNo* pFormNo, pml::Sex* pGender);


  //------------------------------------------------------------------
  /**
    *  @brief    RTC改変通知
    */
  //------------------------------------------------------------------
  void  OnModifiedRTCOffset(void);


  /*
    scanpower
  */
  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  u8  GetScanPower(void) const  {return m_CoreData.scanPower;}


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  bool  IsScanPowerFull(void) const  {return (GetScanPower() == FIELDSCAN_SCANPOWER_MAX);}


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  bool  IsScanPowerEnoughForFieldScan(void) const    {return (FIELDSCAN_POWER_REQUIREMENT <= GetScanPower());}    /**<  ScanPowerがFieldScanに必要な分以上に残っているか  */


  //------------------------------------------------------------------
  /**
    *  @brief   scanPowerのチャージ
    *  @param   [in]  isPromotional   （販促用）連携データによるチャージであるか
    *  @parma   [out] pChargedPower   チャージされたscanPower
    *  @return  チャージ後のScanPower
    */
  //------------------------------------------------------------------
  u32  ChargeScanPower(const bool isPromotional, u32* pChargedPower = NULL);

  //------------------------------------------------------------------
  /**
    *  @brief   scanPowerのチャージ（直値）
    *  @return  チャージ後のScanPower
    */
  //------------------------------------------------------------------
  u32  ChargeScanPower(const u32 power, u32* pChargedPower = NULL);


  //------------------------------------------------------------------
  /**
    *  @brief   scanPowerの増加量の取得
    */
  //------------------------------------------------------------------
  u32  GetChargeScanPower(const bool isPromotional) {return (isPromotional ? QRREAD_SCANPOWER_GAIN_VALUE_PROMOTIONAL : QRREAD_SCANPOWER_GAIN_VALUE_NORMAL);}

  /*  accessor  */


  /*
    ctor/dtor
  */
  QRReaderSaveData(void);
  virtual ~QRReaderSaveData();

  /*
    implement SaveDataInterface I/F
  */
  virtual void*   GetData(void);
  virtual size_t  GetDataSize(void);
  virtual void    Clear(gfl2::heap::HeapBase* pTempHeap);

private:


  CoreData    m_CoreData;
};





} /*  namespace Savedata  */
#endif  /*  #if !defined(QRREADERSAVE_H_INCLUDED)  */
