#if !defined(DELIVERYKEYS_H_INCLUDED)
#define DELIVERYKEYS_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   DeliveryKeys.h
 * @date   2016/07/05 Tue. 16:02:31
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


namespace NetLib    {
namespace Delivery  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/07/05 Tue. 16:03:54
  */
//==============================================================================
class DeliveryKeys
{
public:
  static const u8*    PublicKey(void);
  static u32          PublicKeyBytes(void);

  static const u8*    SecretKey(void);         /**<  !PM_DEBUGはNULLが返る   */
  static u32          SecretKeyBytes(void);    /**<  !PM_DEBUGは0が返る      */

  static const u32    OverheadBytes  = 8;
  static const u32    ModulesBits    = 768;
  static const u32    TempBufBytes   = (ModulesBits / 8) * 3;
private:
  DeliveryKeys(void)  {}
  ~DeliveryKeys() {}
};





} /*  namespace Delivery  */
} /*  namespace NetLib    */
#endif  /*  #if !defined(DELIVERYKEYS_H_INCLUDED)  */
