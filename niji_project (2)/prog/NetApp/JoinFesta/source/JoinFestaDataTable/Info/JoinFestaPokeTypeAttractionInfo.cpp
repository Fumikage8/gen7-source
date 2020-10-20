//==============================================================================
/**
 * @file   JoinFestaPokeTypeAttractionInfo.cpp
 * @brief  ポケモンタイプ相性アトラクションに出てくるポケモンデータ構造
 * @author ichiraku_katsuhiko
 * @date   2016/02/10　20:14
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <debug/include/gfl2_DebugPrint.h>

#include  "../../../include/JoinFestaDataTable/Info/JoinFestaPokeTypeAttractionInfo.h"


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
void  PokeTypeAttractionInfo::Dump(void) const
{
  GFL_PRINT("---- PokeTypeAttractionInfo::Dump ----\n");

#define DUMPVAL(val)          GFL_PRINT("  %d,  /* " #val " */\n", (val));

  DUMPVAL(pokeType);
  DUMPVAL(monsNo);

  GFL_PRINT("}\n");
  GFL_PRINT("--------\n");

#undef  DUMPVAL
}
#endif  /*  #if PM_DEBUG  */


} /*  namespace JoinFesta */
} /*  namespace NetApp    */
