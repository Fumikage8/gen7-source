
#if PM_DEBUG

#if !defined(GIFTDEBUG_RECEIVEDELIVERYDATA_H_INCLUDED)
#define GIFTDEBUG_RECEIVEDELIVERYDATA_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftDebug_ReceiveDeliveryData.h
 * @date   2015/08/21 Fri. 16:19:32
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  <Debug/include/gfl2_DebugWinSystem.h>
#include  <Debug/include/gfl2_DebugWinItemTypes.h>

#include  "NetStatic/NetLib/include/Delivery/DeliveryData.h"

#include  "../GiftDeliveryUtil.h"

namespace NetApp  {
namespace Gift    {
  class GiftWork;

namespace Debug   {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  配信データの受け取りテスト基底
  *  @date   2015/08/21 Fri. 16:34:43
  */
//==============================================================================
class ReceiveDeliveryData
{
  GFL_FORBID_COPY_AND_ASSIGN(ReceiveDeliveryData);
public:
  ReceiveDeliveryData(GiftWork* pWork, DeliveryUtil::IRecvListener* pReceiveListener, const wchar_t* const pName);
  ReceiveDeliveryData(GiftWork* pWork, DeliveryUtil::IRecvListener* pReceiveListener, u32 strId );

  virtual ~ReceiveDeliveryData();


protected:
  GiftWork*                       m_pWork;
  DeliveryUtil::IRecvListener*    m_pReceiveListener;

  gfl2::debug::DebugWinGroup*     m_pGroup_Root;
};


//==============================================================================
/**
  *  @brief  配信データの受け取りテスト：Pokemon
  *  @date   2015/08/21 Fri. 16:55:35
  */
//==============================================================================
class ReceiveDeliveryData_Pokemon
  : public  ReceiveDeliveryData
{
  GFL_FORBID_COPY_AND_ASSIGN(ReceiveDeliveryData_Pokemon);
public:
  ReceiveDeliveryData_Pokemon(GiftWork* pWork, DeliveryUtil::IRecvListener* pReceiveListener);
  virtual ~ReceiveDeliveryData_Pokemon();

protected:
  static void DoReceive(void* _pThis, gfl2::debug::DebugWinItem* pItem);

  NetLib::Delivery::DELIVERY_SENDDATA   m_DeliveryData;
  u32                                   m_MonsNo;
};








} /*  namespace Debug   */
} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(GIFTDEBUG_RECEIVEDELIVERYDATA_H_INCLUDED)  */

#endif