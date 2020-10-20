
#if !defined(GIFTAPPPARAM_H_INCLUDED)
#define GIFTAPPPARAM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftAppParam.h
 * @date   2015/07/31 Fri. 15:08:10
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>

#include  "Savedata/include/MysteryGiftSave.h"
#include  "../source/GiftResource.h"

namespace pml {
namespace pokepara  {
  class PokemonParam;
}
}


namespace NetApp  {
namespace Gift {
// =============================================================================
/**/
//==============================================================================
struct  APP_PARAM {
  enum  BootMode
  {
    BOOTMODE_Normal,
    BOOTMODE_FromReceiveDemo,
  };

  enum  ProcResult
  {
    PROCRESULT_Exit,
    PROCRESULT_Exit_Eshop,
    PROCRESULT_ReceiveDemo,
    PROCRESULT_InternalNetworkErrorOccured,   /**<  内部Fatal発生→FatalEventへ  */
    
    PROCRESULT_
  };

  /** 受信データ  */
  struct  ReceiveDataInfo
  {
    Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA   giftData[MAX_RECEIVE_ATONCE];   /**<  最大20件                                            */
    Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA   lastRemovedData;                /**<  最後にgiftData[]から削除されたデータ（結果表示用）  */
    u32                                                 dataNum;                        /**<  有効データ数            */
    u32                                                 selectedIndex;                  /**<  リストで選択されたIndex */
    
    ReceiveDataInfo(void)
      : dataNum(0)
      , selectedIndex(0)
    {
      memset(giftData,          0, sizeof(giftData));
      memset(&lastRemovedData,  0, sizeof(lastRemovedData));
    }
  };




  BootMode                      bootMode;                       /**<  [in]      */
  ProcResult                    procResult;                     /**<  [out]     */
  ReceiveDataInfo               m_ReceiveDataInfo;              /**<  [out]     */
  bool                          m_bReceiveOneData;



  //------------------------------------------------------------------
  /**
    *  @brief    受信データをアプリケーションバッファに複写
    */
  //------------------------------------------------------------------
  bool  SetReceivedDeliveryData(const void* pData, const size_t dataBytes)
  {
    const u32 giftDataSize  = sizeof(Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA);
    
    if(
      pData                                   /*  pDataがNULLではなく、                                                 */
      && (giftDataSize <= dataBytes)          /*  提供データバイトが受信データ構造体よりも大きく、                      */
      && ((dataBytes % giftDataSize) == 0)    /*  提供データバイトが受信データ構造体サイズの整数倍であれば有効とみなす  */
    )
    {
      memcpy(m_ReceiveDataInfo.giftData, pData, dataBytes);
      m_ReceiveDataInfo.dataNum  = dataBytes / giftDataSize;
      m_ReceiveDataInfo.selectedIndex = 0;
      GFL_PRINT("%s: found %d data\n", __FUNCTION__, GetReceivedDataNum());
      return true;
    }
    
    GFL_PRINT("%s: invalid data (dataBytes:%d, giftDataSize:%d, mod:%d)\n", __FUNCTION__, dataBytes, giftDataSize, dataBytes % giftDataSize);
    return false;
  }


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  const Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA* GetReceivedDeliveryData(const u32 index) const
  {
    GFL_ASSERT(index < GFL_NELEMS(m_ReceiveDataInfo.giftData));

    return  &(m_ReceiveDataInfo.giftData[index]);
  }


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  const Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA* GetLastRemovedDeliveryData(void) const
  {
    return  &(m_ReceiveDataInfo.lastRemovedData);
  }


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  const Savedata::MysteryGiftSave::MYSTERY_GIFT_DATA* GetLastRemovedGiftData(void) const
  {
    return  &(GetLastRemovedDeliveryData()->dataBody);
  }

  //------------------------------------------------------------------
  /**
    *  @brief    一件削除
    *  @return   削除後の件数
    */
  //------------------------------------------------------------------
  u32 RemoveReceivedDeliveryData(const u32 index)
  {
    if(GetReceivedDataNum() && (index < GFL_NELEMS(m_ReceiveDataInfo.giftData)))
    {
      /*  前詰め  */
      const u32 moveDataBeginIndex  = index+1;
      const u32 moveDataEndIndex    = GetReceivedDataNum()-1;
      
      memcpy(&(m_ReceiveDataInfo.lastRemovedData), &(m_ReceiveDataInfo.giftData[index]), sizeof(m_ReceiveDataInfo.lastRemovedData));
      if(moveDataBeginIndex <= moveDataEndIndex)
      {
        const u32 moveDataNum = (moveDataEndIndex - moveDataBeginIndex)+1;

        memmove(&(m_ReceiveDataInfo.giftData[index]), &(m_ReceiveDataInfo.giftData[moveDataBeginIndex]), sizeof(m_ReceiveDataInfo.giftData[0]) * moveDataNum);
      }
      --m_ReceiveDataInfo.dataNum;
    }
    
    
    return GetReceivedDataNum();
  }

  //------------------------------------------------------------------
  /**
    *  @brief    一件削除
    *  @return   削除後の件数
    */
  //------------------------------------------------------------------
  u32 RemoveSelectedDeliveryData(void)
  {
    return RemoveReceivedDeliveryData(m_ReceiveDataInfo.selectedIndex);
  }



  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  const Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA* GetSelectedDeliveryData(void) const
  {
    return  GetReceivedDeliveryData(m_ReceiveDataInfo.selectedIndex);
  }



  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  const Savedata::MysteryGiftSave::MYSTERY_GIFT_DATA* GetReceivedGiftData(const u32 index) const
  {
    return  &(GetReceivedDeliveryData(index)->dataBody);
  }

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  const Savedata::MysteryGiftSave::MYSTERY_GIFT_DATA* GetSelectedGiftData(void) const
  {
    return  GetReceivedGiftData(m_ReceiveDataInfo.selectedIndex);
  }



  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  const wchar_t*  GetDeliveryEventName(const u32 index = 0) const
  {
    GFL_ASSERT(index < GetReceivedDataNum());
    return GetReceivedGiftData(index)->eventName;
  }


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  const wchar_t*  GetReceiveDataDetailText(const u32 index = 0) const
  {
    return (GetReceivedDeliveryData(index)->detailText);
  }


  //------------------------------------------------------------------
  /**
    *  @brief    有効データ（受け取れるもの）数
    */
  //------------------------------------------------------------------
  u32 GetRemainingDataNum(void)
  {
    u32 count = 0;
    
    for(u32 index = 0; index < GetReceivedDataNum(); ++index)
    {
      const Savedata::MysteryGiftSave::ReceiveCheckResult  checkResult  = Savedata::MysteryGiftSave::ReceiveCheck(GetReceivedDeliveryData(index));

      if((checkResult == Savedata::MysteryGiftSave::RECEIVE_CHECK_RESULT_OK) || (checkResult == Savedata::MysteryGiftSave::RECEIVE_CHECK_RESULT_NG_FULL))
      {
        ++count;
      }
    }
    
    return count;
  }

  //------------------------------------------------------------------
  /**
    *  @brief    受信済みデータ数
    */
  //------------------------------------------------------------------
  u32 GetReceivedDataNum(void )const 
  {
    return m_ReceiveDataInfo.dataNum;
  }



  /*  ctor  */
  APP_PARAM(void)
    : bootMode(BOOTMODE_Normal)
    , procResult(PROCRESULT_Exit)
    , m_bReceiveOneData(false)
  {}


  void  ClearResult(void)
  {
    procResult  = PROCRESULT_Exit;
  }
};


} /*  namespace NetApp  */
} /*  namespace Gift    */
#endif  /*  #if !defined(GIFTAPPPARAM_H_INCLUDED)  */

