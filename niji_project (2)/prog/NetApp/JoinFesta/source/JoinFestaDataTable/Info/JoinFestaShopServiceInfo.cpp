//==============================================================================
/**
 * @file   JoinFestaShopServiceInfo.cpp
 * @date   2015/11/04 Wed. 13:20:17
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

#include  "../../../include/JoinFestaDataTable/Info/JoinFestaShopServiceInfo.h"


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
void  ShopServiceInfo::Dump(void) const
{
  GFL_PRINT("---- ShopServiceInfo::Dump ----\n");

#define DUMPVAL(val)          GFL_PRINT("  %d,  /* " #val " */\n", (val));
  DUMPVAL(isHallOfFame);                         /**<  殿堂入り後登場      */
  DUMPVAL(messageID_ServiceName);                /**<  サービス名          */
  DUMPVAL(coinPrice);                            /**<  コイン              */

  GFL_PRINT("{  /*  messageID_Talk */\n");
  for(u32 index = 0; index < GFL_NELEMS(messageID_Talk); ++index)
  {
      GFL_PRINT("%d,  /*  %d */\n", messageID_Talk[index], index);
  }

  DUMPVAL(serviceType);                          /**<  サービス種別                                          */

  GFL_PRINT("{  /*  serviceArgs */\n");
  for(u32 index = 0; index < GFL_NELEMS(serviceArgs); ++index)
  {
      GFL_PRINT("%d,  /*  %d */\n", serviceArgs[index], index);
  }
  GFL_PRINT("--------\n");

#undef  DUMPVAL
}
#endif  /*  #if PM_DEBUG  */



} /*  namespace JoinFesta */
} /*  namespace NetApp    */
