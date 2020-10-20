//======================================================================
/**
 * @file KisekaeShopID.h
 * @date 2015/10/18 14:14:10
 * @author uchida_yuto
 * @brief きせかえShopID定義
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __KISEKASE_SHOP_ID_H_INCLUDED__
#define __KISEKASE_SHOP_ID_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
GFL_NAMESPACE_BEGIN(Kisekae)

  /**
   * @enum KisekasShopID
   * きせかえ購入ショップのid
   */
  enum KisekaeShopID
  {
    KISEKAE_SHOP_ID_0 = 0,
    KISEKAE_SHOP_ID_1,
    KISEKAE_SHOP_ID_2,
    KISEKAE_SHOP_ID_3,
    KISEKAE_SHOP_ID_4,
    KISEKAE_SHOP_ID_5,
    KISEKAE_SHOP_ID_6,
    KISEKAE_SHOP_ID_MAX
  };

  /**
   * @enum KisekasColorShopID
   * きせかえ購入ショップのid
   * @note
   *    最大数が多いので enumの定義名はやめる
   */
  enum
  {
    //  色染めショップはの最大数
    KISEKAE_COLOR_SHOP_ID_MAX = 24
  };

GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

#endif // __KISEKASE_SHOP_ID_H_INCLUDED__