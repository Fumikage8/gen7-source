
#if !defined(DELIVERYUTILRETRYHELPER_H_INCLUDED)
#define DELIVERYUTILRETRYHELPER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   DeliveryUtilRetryHelper.h
 * @date   2015/07/06 Mon. 17:23:10
 * @author muto_yu
 * @brief  無線スイッチがOFFからONにされた際にDeliveryUtilのInitializeを自動的に行うためのHelper
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_Macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  "./GiftDeliveryUtil.h"


namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  DeliveryUtilRetryHelper
  *  @brief  無線スイッチがOFFからONにされた際にDeliveryUtilのInitializeを自動的に行うためのHelper
  *  @date   2015/07/06 Mon. 17:24:26
  */
//==============================================================================
class DeliveryUtilRetryHelper
{
  GFL_FORBID_COPY_AND_ASSIGN( DeliveryUtilRetryHelper );
public:
  DeliveryUtilRetryHelper(DeliveryUtil::IRecvListener* pListener);
  virtual ~DeliveryUtilRetryHelper();


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  void  Start(const DeliveryUtil::Mode connectionMode, const bool isRetryEnable = true);

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    状態更新
    */
  //------------------------------------------------------------------
  void  Update(void);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    （エラー発生時など）Finalizeをリクエストする
    */
  //------------------------------------------------------------------
  void  RequestFinalize(const bool isAutoInitializeEnable = true);


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void  SetSerialCode(const u32 serialCode);



  /** 内部状態  */
  enum  Status
  {
    STATUS_Available,                               /**<  有効                      */
    STATUS_NotAvailable,                            /**<  無効                      */
    STATUS_NotAvailable_WirelessSwitchTurnedOff,    /**<  無効かつ無線スイッチがOFF */
    STATUS_Finalized,                               /**<  Finalize後                */

    STATUS_
  };
  Status  GetStatus(void) const;


protected:
  bool  CheckWirelessSwitchState(void) const;

  DeliveryUtil::IRecvListener*  m_pListener;
  u32                           m_State;
  DeliveryUtil::Mode            m_ConnectionMode;
  bool                          m_bRetryEnable;
};

} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(DELIVERYUTILRETRYHELPER_H_INCLUDED)  */
