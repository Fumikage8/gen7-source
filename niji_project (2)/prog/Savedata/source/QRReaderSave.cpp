//==============================================================================
/**
 * @file   QRReaderSave.cpp
 * @date   2016/01/19 Tue. 20:47:17
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
#include  <debug/include/gfl2_DebugPrint.h>
#include  <crypto/include/gfl2_ISHA256Calculator.h>

#include  "System/include/ThreadPriority.h"
#include  "GameSys/include/GameManager.h"
#include  "GameSys/include/GameData.h"
#include  "../include/QRReaderSave.h"


// =============================================================================
/**/
//==============================================================================
namespace
{
  void  CalcSHA256(void* pDstBuf, const u32 dstBufBytes, gfl2::heap::HeapBase* pTempHeapBase, const void* pSrcData, const u32 srcDataBytes)
  {
    GFL_ASSERT(pDstBuf);
    GFL_ASSERT(dstBufBytes);
    GFL_ASSERT(pTempHeapBase);
    GFL_ASSERT(pSrcData);
    GFL_ASSERT(srcDataBytes);

    /*  sha256  */
    gfl2::crypto::ISHA256Calculator*  pSHA256Calculator = gfl2::crypto::ISHA256Calculator::CreateSHA256Calculator(pTempHeapBase, 256, System::ThreadPriority::ASYNC_SHA256_CRYPTO_THREAD_PRIORITY);
    gfl2::crypto::SHA256Hash*         pSHA256Hash       = pSHA256Calculator->CalcHash(pSrcData, srcDataBytes);

    GFL_ASSERT_MSG(pSHA256Hash->GetDataSize() == dstBufBytes, "hashSize: %d, dstBufBytes: %d\n", pSHA256Hash->GetDataSize(), dstBufBytes);

    memcpy(pDstBuf, pSHA256Hash->GetData(), pSHA256Hash->GetDataSize());

    GFL_SAFE_DELETE(pSHA256Calculator);
  }

  /*  nijiのマギアナに倣い、QR読み込みの判定をビットではなくマジックナンバーで行う  */
  /*  単純なビット埋めによる改竄を防ぐ意図とのこと                                  */
  static const u64  M20PIKA_MAGIC_NUMBER  = 0xF44E94EA7D19A8D6ULL;
}




namespace Savedata  {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
QRReaderSaveData::QRReaderSaveData(void)
  : SaveDataInterface()
  , m_CoreData()
{
  STATIC_ASSERT(COREDATA_BYTES==456);
  Clear(NULL);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
QRReaderSaveData::~QRReaderSaveData()
{
  /*  nop  */
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void*   QRReaderSaveData::GetData(void)
{
  return &m_CoreData;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
size_t  QRReaderSaveData::GetDataSize(void)
{
  return COREDATA_BYTES;
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void    QRReaderSaveData::Clear(gfl2::heap::HeapBase* pTempHeap)
{
  /*    */
  memset(&m_CoreData, 0, sizeof(m_CoreData));


  m_CoreData.bTutorialFinished  = false;
  m_CoreData.fieldScanStatus    = FIELDSCAN_STATUS_InActive;

  m_CoreData.battery            = 100;
  m_CoreData.scanPower          = 0;            /**<  FieldScanのための解析パワー値   */

  m_CoreData.timer1             = 3600;
  m_CoreData.timer2             = 3600;


  m_CoreData.fieldSacnStartDate = 0LL;

  memset(m_CoreData.histories, 0, sizeof(m_CoreData.histories));
  m_CoreData.historyCount       = 0;

  /*  fieldScanTargetInfo  */
  m_CoreData.fieldScanTargetInfo.islandID = 0;
  m_CoreData.fieldScanTargetInfo.zoneID   = 0;
  m_CoreData.fieldScanTargetInfo.monsNo   = 0;
  m_CoreData.fieldScanTargetInfo.formNo   = 0;
  m_CoreData.fieldScanTargetInfo.gender   = 0;


  m_CoreData.batteryChangeDate  = 0LL;
}


//------------------------------------------------------------------
/**
  *  @brief    読み取り履歴に追加する
  */
//------------------------------------------------------------------
void  QRReaderSaveData::RegisterScannedDataHistory(gfl2::heap::HeapBase* pTempHeapBase, const void* pScannedData, const u32 dataBytes)
{
  if(m_CoreData.historyCount < UINT_MAX)
  {
    const u32 historyIndex  = m_CoreData.historyCount % QRREAD_HISTORY_MAX;
    
    /*  sha256  */
    CalcSHA256(m_CoreData.histories[historyIndex].sha256, sizeof(m_CoreData.histories[historyIndex].sha256), pTempHeapBase, pScannedData, dataBytes);
    ++m_CoreData.historyCount;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    読み取り履歴に追加されているデータであるか
  */
//------------------------------------------------------------------
bool  QRReaderSaveData::IsRegisteredData(gfl2::heap::HeapBase* pTempHeapBase, const void* pScannedData, const u32 dataBytes) const
{
  bool                      isRegistered      = false;
  const u32                 historyNum        = (QRREAD_HISTORY_MAX < m_CoreData.historyCount) ? QRREAD_HISTORY_MAX : m_CoreData.historyCount;         /**<  蓄積数：※最大QRREAD_HISTORY_MAX)                                                 */
//  const u32                 historyIndexBack  = (m_CoreData.historyCount < QRREAD_HISTORY_MAX) ? 0 : (m_CoreData.historyCount % QRREAD_HISTORY_MAX);   /**<  最も古いデータのIndex：データ先頭の次のIndex（つまり現在のカウント値）が最も古い  */
  CoreData::ScanDataHistory dataHistory;

  CalcSHA256(dataHistory.sha256, sizeof(dataHistory.sha256), pTempHeapBase, pScannedData, dataBytes);
  for(u32 num = 0; num < historyNum; ++num)
  {
//    const u32 historyIndex  = (historyIndexBack + num) % QRREAD_HISTORY_MAX;  /**<  評価中のIndex  */
    const u32 historyIndex  = num;    /**<  評価中のIndex：※古い順に評価する必要は特にない  */
    
    if(!memcmp(dataHistory.sha256, m_CoreData.histories[historyIndex].sha256, sizeof(dataHistory.sha256))) /*  ==0で一致  */
    {
      isRegistered  = true;
      break;
    }
  }


  return isRegistered;
}


//------------------------------------------------------------------
/**
  *  @brief    データ履歴のリセット
  */
//------------------------------------------------------------------
void  QRReaderSaveData::ResetScannedDataHistory(void)
{
  m_CoreData.historyCount = 0;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
u8  QRReaderSaveData::BatteryQuery(void)
{
  const GameSys::DeviceDate currentDate;
  const s64                 currentDateSecond = GameSys::INijiDate::GetDateToSecond(*(currentDate.GetDateConst()));
  s64                       dateDiff          = currentDateSecond - m_CoreData.batteryChangeDate;
  u8                        chargeValue       = 0;      /**<  回復量  */

  /*  batteryChangeDateが初期状態であったり、何らかの理由で現在時刻のほうがキャッシュより過去であった場合はタイマをリセット  */
  if((!m_CoreData.batteryChangeDate) || (dateDiff < 0))
  {
    ResetBatteryChangeDate(currentDateSecond);
    dateDiff  = 0;
  }

  /*  既にFullなら以降の処理は不要  */
  if(IsBatteryFull())
  {
    return GetBattery();
  }

  /*  バッテリー回復量を反映  */
  if(dateDiff >= QRREAD_BATTERY_CHARGE_SECOND)
  {
    /*  桁あふれ回避のため、念のため QRREAD_BATTERY_FULLCHARGE_SECOND に丸める  */
    const u32 dateDiffU32   = (static_cast<s64>(QRREAD_BATTERY_FULLCHARGE_SECOND) < dateDiff) ? QRREAD_BATTERY_FULLCHARGE_SECOND : static_cast<u32>(dateDiff);
    const u8  oldBattery    = GetBattery();
    const u8  burnedBattery = QRREAD_BATTERY_MAX - oldBattery;    /**<  バッテリー消費量  */
    
    chargeValue = (dateDiffU32 / QRREAD_BATTERY_CHARGE_SECOND);
    chargeValue = (burnedBattery < chargeValue) ? burnedBattery : chargeValue;

    /*  回復と時刻の記録  */
    m_CoreData.battery  += chargeValue;
    ResetBatteryChangeDate(m_CoreData.batteryChangeDate + static_cast<s64>(chargeValue * QRREAD_BATTERY_CHARGE_SECOND));    /*  実際に回復両として反映した秒数分進める  */
  }

  return GetBattery();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  QRReaderSaveData::ResetBatteryChangeDate(const s64 targetDate)
{
  m_CoreData.batteryChangeDate  = targetDate;
}


//------------------------------------------------------------------
/**
  *  @brief    QRScanの実行によるバッテリー消費
  */
//------------------------------------------------------------------
bool  QRReaderSaveData::UseBatteryOnQRScan(void)
{
  bool  isSuccess = false;
  
  if(IsBatteryEnoughForQRRead())
  {
    m_CoreData.battery -= QRREAD_BATTERY_REQUIREMENT;
    {
      const GameSys::DeviceDate currentDate;
      const s64                 currentDateSecond = GameSys::INijiDate::GetDateToSecond(*(currentDate.GetDateConst()));

      ResetBatteryChangeDate(currentDateSecond);
    }
    isSuccess = true;
  }
  
  return isSuccess;
}


//------------------------------------------------------------------
/**
  *  @brief    FieldScan状態の問い合わせ
  *  @param    [in] pHandler  NULL可、Query結果をハンドラで受け取りたい場合は設定する
  *  @param    [in] pInfo     NULL可、現在の状態を受け取りたい場合は設定する
  */
//------------------------------------------------------------------
QRReaderSaveData::FieldScanStatus   QRReaderSaveData::FieldScanQuery(IFieldScanQueryHandler* pHandler, FieldScanQueryInfo* pInfo)
{
  FieldScanQueryInfo  info;
  
  info.InitializeFromCoreData(m_CoreData);
  /*
    更新
  */
  switch(m_CoreData.fieldScanStatus)
  {
    case  FIELDSCAN_STATUS_Active:
    {
      /*
        2016/03/28  Yu Muto
        残り時間をゲージで表現することはしない方針となった
      */
#if 0
      /*  演出用にscanPower値を変動させる  */
      m_CoreData.scanPower  = FIELDSCAN_SCANPOWER_MAX * info.remainingSeconds / FIELDSCAN_DURATION_SECONDS;
#endif
      if(!info.remainingSeconds)
      {
        m_CoreData.fieldScanStatus  = FIELDSCAN_STATUS_Timeout;
      }
    }
      break;


    case  FIELDSCAN_STATUS_Timeout:   /*  fall through  */
    case  FIELDSCAN_STATUS_InActive:  /*  fall through  */
    default:
      /*  nop  */
      break;
  }
  
  info.status         = static_cast<FieldScanStatus>(m_CoreData.fieldScanStatus);

  /*
    状態の返却
  */
  return GetFieldScanStatus(pHandler, pInfo);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
QRReaderSaveData::FieldScanStatus   QRReaderSaveData::GetFieldScanStatus(IFieldScanQueryHandler* pHandler, FieldScanQueryInfo* pInfo) const
{
  FieldScanQueryInfo  info;


  info.InitializeFromCoreData(m_CoreData);

  /*
    状態の返却
  */
  if(pHandler)
  {
    pHandler->FieldScan_OnQuery(static_cast<FieldScanStatus>(m_CoreData.fieldScanStatus), info);
  }
  
  if(pInfo)
  {
    *pInfo  = info;
  }
  
  return info.status;
}


//------------------------------------------------------------------
/**
  *  @brief    FieldScanを発動する
  *  @param    [in] isForced  scanPower値に関わらず強制的に発動する（PM_DEBUG時のみ）
  *  @return   発動できたか
  */
//------------------------------------------------------------------
bool  QRReaderSaveData::ActivateFieldScan(const bool isForced)
{
#if PM_DEBUG
  if(isForced)
  {
    m_CoreData.scanPower = FIELDSCAN_POWER_REQUIREMENT;
  }
#endif

  if(FIELDSCAN_POWER_REQUIREMENT <= m_CoreData.scanPower)
  {
    m_CoreData.fieldScanStatus    = FIELDSCAN_STATUS_Active;
    m_CoreData.fieldSacnStartDate = GameSys::INijiDate::GetDateToSecond(*(GameSys::DeviceDate().GetDate()));
    m_CoreData.scanPower          -= FIELDSCAN_POWER_REQUIREMENT;
  }

  return (m_CoreData.fieldScanStatus == FIELDSCAN_STATUS_Active);
}



//------------------------------------------------------------------
/**
  *  @brief    FieldScanを無効化する
  *  @note     （Query結果が FIELDSCAN_STATUS_Timeout であった場合に）本関数をコールすることで各種フラグがクリアされ、
               以後 FIELDSCAN_STATUS_InActive が返るようになる
  */
//------------------------------------------------------------------
void  QRReaderSaveData::InactivateFieldScan(void)
{
  /*
    2016/03/28  Yu Muto
    FieldScan発動中にも先行してScanPowerをためられるようにとの要望
  */
//  m_CoreData.scanPower          = 0;
  m_CoreData.fieldScanStatus    = FIELDSCAN_STATUS_InActive;
  m_CoreData.fieldSacnStartDate = 0;
}


//------------------------------------------------------------------
/**
  *  @brief    FieldScanで気配を察知した対象の情報をキャッシュする（シルエット表示用）
  */
//------------------------------------------------------------------
void  QRReaderSaveData::SetFieldScanTargetInfo(const u16 islandID, const u16 zoneID, const MonsNo monsNo, const FormNo formNo, const pml::Sex gender)
{
  m_CoreData.fieldScanTargetInfo.islandID = islandID;
  m_CoreData.fieldScanTargetInfo.zoneID   = zoneID;
  m_CoreData.fieldScanTargetInfo.monsNo   = static_cast<u16>(monsNo);
  m_CoreData.fieldScanTargetInfo.formNo   = static_cast<u16>(formNo);
  m_CoreData.fieldScanTargetInfo.gender   = static_cast<u16>(gender);
}


//------------------------------------------------------------------
/**
  *  @brief    FieldScanで気配を察知した対象の情報のキャッシュを取得する（シルエット表示用）
  */
//------------------------------------------------------------------
void  QRReaderSaveData::GetFieldScanTargetInfo(u16* pIslandID, u16* pZoneID, MonsNo* pMonsNo, FormNo* pFormNo, pml::Sex* pGender)
{
  if(pIslandID) *pIslandID  = m_CoreData.fieldScanTargetInfo.islandID;
  if(pZoneID)   *pZoneID    = m_CoreData.fieldScanTargetInfo.zoneID;
  if(pMonsNo)   *pMonsNo    = static_cast<MonsNo>(m_CoreData.fieldScanTargetInfo.monsNo);
  if(pFormNo)   *pFormNo    = static_cast<FormNo>(m_CoreData.fieldScanTargetInfo.formNo);
  if(pGender)   *pGender    = static_cast<pml::Sex>(m_CoreData.fieldScanTargetInfo.gender);
}


//------------------------------------------------------------------
/**
  *  @brief    RTC改変通知
  */
//------------------------------------------------------------------
void  QRReaderSaveData::OnModifiedRTCOffset(void)
{
  /*  battery  */
  {
    /*  基準時刻を現在時刻でリセット  */
    const GameSys::DeviceDate currentDate;
    const s64                 currentDateSecond = GameSys::INijiDate::GetDateToSecond(*(currentDate.GetDateConst()));

    ResetBatteryChangeDate(currentDateSecond);
  }


  /*  FieldScanがActiveなら、Timeoutへ移行  */
  if(IsFieldScanActive())
  {
    /*  ※アプリケーションがTimeoutを検知できるよう、InactivateFieldScanは使用しない  */

    m_CoreData.fieldSacnStartDate   = 0;
    m_CoreData.fieldScanStatus      = FIELDSCAN_STATUS_Timeout;
    m_CoreData.scanPower            = 0;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
u32  QRReaderSaveData::ChargeScanPower(const bool isPromotional, u32* pChargedPower)
{
  return ChargeScanPower(GetChargeScanPower(isPromotional), pChargedPower);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
u32  QRReaderSaveData::ChargeScanPower(const u32 power, u32* pChargedPower)
{
  const u32 oldPower  = m_CoreData.scanPower;
  u32       tempPower = m_CoreData.scanPower + power;

  tempPower = ((tempPower < FIELDSCAN_SCANPOWER_MAX) ? tempPower : FIELDSCAN_SCANPOWER_MAX); /*  max  */
  m_CoreData.scanPower = tempPower;

  if(pChargedPower)
  {
    *pChargedPower  = (m_CoreData.scanPower - oldPower);
  }

  return  m_CoreData.scanPower;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void QRReaderSaveData::FieldScanQueryInfo::InitializeFromCoreData(const CoreData& rCoreData)
{
  const GameSys::DeviceDate currentDate;

  this->currentDate       = GameSys::INijiDate::GetDateToSecond(*(currentDate.GetDateConst()));
  this->endDate           = rCoreData.fieldSacnStartDate + FIELDSCAN_DURATION_SECONDS;
  this->remainingSeconds  = (this->endDate > this->currentDate) ? (this->endDate - this->currentDate) : 0;
  this->status            = static_cast<FieldScanStatus>(rCoreData.fieldScanStatus);
}

#define MAGIANA_MAGIC_NUMBER 0xCBE05F18356504AC

//------------------------------------------------------------------
/**
  *  @brief    マギアナQRを読んだ状態にする
  */
//------------------------------------------------------------------
void  QRReaderSaveData::ReadMagianaQR(void)
{
  m_CoreData.isReadMagianaQR = MAGIANA_MAGIC_NUMBER;
}
//------------------------------------------------------------------
/**
  *  @brief    マギアナQRを読んだかどうか
  */
//------------------------------------------------------------------
bool  QRReaderSaveData::IsReadMagianaQR(void)
{
  return ( m_CoreData.isReadMagianaQR == MAGIANA_MAGIC_NUMBER );
}


//------------------------------------------------------------------
/**
  *  @brief    M20ピカチュウQRを読んだ状態にする
  */
//------------------------------------------------------------------
void  QRReaderSaveData::ReadM20PikaQR(void)
{
  m_CoreData.isReadM20PikatyuQR = M20PIKA_MAGIC_NUMBER;
}


//------------------------------------------------------------------
/**
  *  @brief    M20ピカチュウQRを読んだか
  */
//------------------------------------------------------------------
bool  QRReaderSaveData::IsReadM20PikaQR(void)
{
  return ( m_CoreData.isReadM20PikatyuQR == M20PIKA_MAGIC_NUMBER );
}



} /*  namespace Savedata  */
