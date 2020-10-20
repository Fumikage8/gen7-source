//==============================================================================
/**
 * @file   JoinFestaLotInfo.cpp
 * @date   2015/11/04 Wed. 13:03:53
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

#include  "../../../include/JoinFestaDataTable/Info/JoinFestaLotInfo.h"


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
void  LotInfo::Dump(void) const
{
  GFL_PRINT("---- LotInfo::Dump ----\n");

  for(u32 index = 0; index < GFL_NELEMS(item); ++index)
  {
    GFL_PRINT("{  /* #[%d] */\n", index);
    GFL_PRINT("  %d,  /* probabilityWeight */\n", item[index].probabilityWeight);
    GFL_PRINT("  %d,  /* itemID */\n",            item[index].itemID);
    GFL_PRINT("},\n");
  }


  GFL_PRINT("--------\n");
}
#endif


} /*  namespace JoinFesta */
} /*  namespace NetApp    */
