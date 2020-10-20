//==============================================================================
/**
 * @file   JoinFestaAffinityAttractionInfo.cpp
 * @brief  タイプ相性診断アトラクションに出てくるポケモンデータ構造
 * @author ichiraku_katsuhiko
 * @date   2016/03/02　20:14
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <debug/include/gfl2_DebugPrint.h>

#include  "../../../include/JoinFestaDataTable/Info/JoinFestaAffinityAttractionInfo.h"


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
void  AffinityAttractionInfo::Dump(void) const
{
  GFL_PRINT("---- AffinityAttractionInfo::Dump ----\n");

#define DUMPVAL(val)          GFL_PRINT("  %d,  /* " #val " */\n", (val));

  DUMPVAL(level);
  DUMPVAL(monsNo);

  GFL_PRINT("}\n");
  GFL_PRINT("--------\n");

#undef  DUMPVAL
}
#endif  /*  #if PM_DEBUG  */


} /*  namespace JoinFesta */
} /*  namespace NetApp    */
