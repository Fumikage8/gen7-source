//==============================================================================
/**
 * @file   JoinFestaKisekaePatternInfo.cpp
 * @brief  アトラクションの着せ替えダミーパターン
 * @author ichiraku_katsuhiko
 * @date   2016/02/10　20:14
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <debug/include/gfl2_DebugPrint.h>

#include  "../../../include/JoinFestaDataTable/Info/JoinFestaKisekaePatternInfo.h"


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
void  KisekaePatternInfo::Dump(void) const
{
  GFL_PRINT("---- KisekaePatternInfo::Dump ----\n");

#define DUMPVAL(val)          GFL_PRINT("  %d,  /* " #val " */\n", (val));

  DUMPVAL(sex)
  DUMPVAL(body_color)
  DUMPVAL(eye_color)
  DUMPVAL(hair_color)
  DUMPVAL(female_lip_color)
  DUMPVAL(face)
  DUMPVAL(hair_style)
  DUMPVAL(accbadge)
  DUMPVAL(acceye)
  DUMPVAL(acchair)
  DUMPVAL(bag)
  DUMPVAL(bngl)
  DUMPVAL(body)
  DUMPVAL(bottoms)
  DUMPVAL(hat)
  DUMPVAL(legs)
  DUMPVAL(shoes)
  DUMPVAL(tops)  

  GFL_PRINT("}\n");
  GFL_PRINT("--------\n");

#undef  DUMPVAL
}
#endif  /*  #if PM_DEBUG  */


} /*  namespace JoinFesta */
} /*  namespace NetApp    */
