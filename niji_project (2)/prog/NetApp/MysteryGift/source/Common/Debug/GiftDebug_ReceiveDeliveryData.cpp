#if PM_DEBUG

//==============================================================================
/**
 * @file   GiftDebug_ReceiveDeliveryData.cpp
 * @date   2015/08/21 Fri. 16:39:32
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../../ApplicationSystem/GiftWork.h"

#include  "./GiftDebug_ReceiveDeliveryData.h"

#include  <Debug/include/gfl2_DebugWinSystem.h>

#include <Debug/DebugWin/include/DebugWinSystem.h>
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"


namespace NetApp  {
namespace Gift    {
namespace Debug   {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
ReceiveDeliveryData::ReceiveDeliveryData(GiftWork* pWork, DeliveryUtil::IRecvListener* pReceiveListener, const wchar_t* const pName)
  : m_pWork(pWork)
  , m_pReceiveListener(pReceiveListener)
  , m_pGroup_Root(NULL)

{
//  gfl2::ui::DeviceManager*  pDevice = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();

  m_pGroup_Root = gfl2::debug::DebugWin_AddGroup(m_pWork->GetDeviceHeap(), pName);

}

// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
ReceiveDeliveryData::ReceiveDeliveryData(GiftWork* pWork, DeliveryUtil::IRecvListener* pReceiveListener, u32 strId )
  : m_pWork(pWork)
  , m_pReceiveListener(pReceiveListener)
  , m_pGroup_Root(NULL)

{
  ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  gfl2::str::MsgData* pMsgData = devWork->GetMessageData();
  m_pGroup_Root = gfl2::debug::DebugWin_AddGroup(m_pWork->GetDeviceHeap(), pMsgData, strId );
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
ReceiveDeliveryData::~ReceiveDeliveryData()
{
  gfl2::debug::DebugWin_RemoveGroup(m_pGroup_Root);
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
ReceiveDeliveryData_Pokemon::ReceiveDeliveryData_Pokemon(GiftWork* pWork, DeliveryUtil::IRecvListener* pReceiveListener)
  : ReceiveDeliveryData(pWork, pReceiveListener, msg_dmenu_recieve_delivery_poke_top)   // "はいしんデータ：Pokemon"
  , m_DeliveryData()
  , m_MonsNo(0)
{
  memset(&m_DeliveryData, 0, sizeof(m_DeliveryData));

  m_DeliveryData.common.delivery_id                   = 0;                                /**<  配信ID                        */
  m_DeliveryData.common.receive_restriction_type      = NetLib::Delivery::RECEIVE_RESTRICTION_TYPE_FREE;    /**<  受信制限種別                  */
  m_DeliveryData.common.movie_promotion               = NetLib::Delivery::MOVIE_PROMOTION_DISABLE;          /**<  映画配信演出フラグ            */
  m_DeliveryData.common.target_version                = 0;                                /**<  対象バージョン                */
  m_DeliveryData.common.target_language               = 0;                                /**<  対象言語                      */
  m_DeliveryData.common.data_type                     = NetLib::Delivery::DATA_TYPE_POKEMON;                /**<  後方に続くデータ本体の内容    */
  m_DeliveryData.common.random_delivery_max_number    = 0;                                /**<  ランダム配信MaxNo.            */
  m_DeliveryData.common.random_delivery_number        = 0;                                /**<  ランダム配信No.               */
  m_DeliveryData.common.random_probability            = 0;                                /**<  ランダム確率                  */


  /*  body  */
//  m_DeliveryData.body.pokemon.monster_number


  /*  win  */
  {
    gfl2::heap::HeapBase* pHeapBase = m_pWork->GetDeviceHeap();
    ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
    gfl2::str::MsgData* pMsgData = devWork->GetMessageData();
    
    gfl2::debug::DebugWin_AddItemValueU32(m_pGroup_Root, pHeapBase, pMsgData, msg_dmenu_recieve_delivery_poke_no, &m_MonsNo, 0, MONSNO_END);    // "monsterNo."

    gfl2::debug::DebugWin_AddItemUpdateFunc(m_pGroup_Root, pHeapBase, pMsgData, msg_dmenu_recieve_delivery_poke_do, this, DoReceive);   // "<DoReceive>"
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
ReceiveDeliveryData_Pokemon::~ReceiveDeliveryData_Pokemon()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void ReceiveDeliveryData_Pokemon::DoReceive(void* _pThis, gfl2::debug::DebugWinItem* pItem)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    ReceiveDeliveryData_Pokemon*  pThis = static_cast<ReceiveDeliveryData_Pokemon*>(_pThis);

    pThis->m_DeliveryData.body.pokemon.monster_number  = static_cast<u16>(pThis->m_MonsNo);
    pThis->m_pReceiveListener->DeliveryUtilRecvListener_OnRecvComplete(&pThis->m_DeliveryData, sizeof(pThis->m_DeliveryData));
  }
}




} /*  namespace Debug   */
} /*  namespace Gift    */
} /*  namespace NetApp  */

#endif
