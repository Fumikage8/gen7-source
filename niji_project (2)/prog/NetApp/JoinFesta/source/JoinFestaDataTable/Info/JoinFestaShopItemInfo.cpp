//==============================================================================
/**
 * @file   JoinFestaShopItemInfo.cpp
 * @date   2015/11/04 Wed. 13:10:00
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <debug/include/gfl2_DebugPrint.h>

#include  "../../../include/JoinFestaDataTable/Info/JoinFestaShopItemInfo.h"


namespace NetApp    {
namespace JoinFesta {
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
#if PM_DEBUG
void  ShopItemInfo::Dump(void) const
{
  GFL_PRINT("---- ShopItemInfo::Dump ----\n");

#define DUMPVAL(val)          GFL_PRINT("  %d,  /* " #val " */\n", (val));

  DUMPVAL(rank);                     /**<  ランク                  */
  DUMPVAL(shopType);                 /**<  店種別(FacilitiesType)  */
  DUMPVAL(messageID_ShopName);       /**<  店名                    */
  DUMPVAL(messageID_Description);    /**<  説明                    */
  DUMPVAL(price);                    /**<  価格                    */
  DUMPVAL(isAvailableSun);           /**<  Sunで有効               */
  DUMPVAL(isAvailableMoon);          /**<  Moonで有効              */


  GFL_PRINT("{  /*  serviceID */\n");
  for(u32 index = 0; index < GFL_NELEMS(serviceID); ++index)
  {
      GFL_PRINT("%d,  /*  %d */\n", serviceID[index], index);
  }
  GFL_PRINT("}\n");


  GFL_PRINT("--------\n");

#undef  DUMPVAL
}
#endif  /*  #if PM_DEBUG  */


} /*  namespace JoinFesta */
} /*  namespace NetApp    */
