#if !defined(QRREADERFIELDFLAGUTILITY_H_INCLUDED)
#define QRREADERFIELDFLAGUTILITY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   QRReaderFieldFlagUtility.h
 * @date   2016/01/22 Fri. 11:19:31
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

#include  "Field/FieldStatic/include/Zone\ZoneDataLoaderTypes.h"

namespace NetApp    {
namespace QRReader  {
  class QRReaderWork;
// =============================================================================
/**/
//==============================================================================
class FieldFlagUtility
{
public:
  static bool DoFieldScan(QRReaderWork* pWork, u8* pIslandID, Field::ZoneID* pZoneID, MonsNo* pTargetMonsNo, FormNo* pTargetFormNo, pml::Sex* pTargetGender);
  static u32  GetIslandID(void);
  static bool QueryFieldScan(u32* pRemainingMinutes = NULL, u32* pRemainingSeconds = NULL);   /**<  trueは継続中  */
  static u32  IslandIDtoIslandNameMessageID(const u8 islandID);

private:
  static void FieldScan_FlagClear(void);
};


} /*  namespace QRReader  */
} /*  namespace NetApp    */
#endif  /*  #if !defined(QRREADERFIELDFLAGUTILITY_H_INCLUDED)  */
