//==============================================================================
/**
 * @file   JoinFestaDataTable.cpp
 * @date   2015/11/04 Wed. 12:59:11
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../../include/JoinFestaDataTable/JoinFestaDataTable.h"


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
void  JoinFestaDataTable::Dump(void) const
{
  if(pShopItemInfoTable)    pShopItemInfoTable->Dump();
  if(pShopServiceInfoTable) pShopServiceInfoTable->Dump();
  if(pLotInfoTable)         pLotInfoTable->Dump();
}
#endif



} /*  namespace JoinFesta */
} /*  namespace NetApp    */
