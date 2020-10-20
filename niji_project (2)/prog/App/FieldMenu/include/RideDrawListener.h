//=============================================================================
/**
 * @file    RideDrawListener.h
 * @brief   ライドポケモン選択画面リスナー
 * @author  Hiroyuki Nakamura
 * @date    2015.07.15
 */
//=============================================================================
#if !defined( RIDE_DRAW_LISTENER_H_INCLUDED )
#define RIDE_DRAW_LISTENER_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "Savedata/include/FieldMenuSave.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

//=============================================================================
/**
 * @class RideDrawListener
 * @brief ライドポケモン選択リスナー
 * @date  2015.07.15
 */
//=============================================================================
class RideDrawListener
{
public:
  //--------------------------------------------------------------------------
  /**
   * @func    SelectRide
   * @brief   選択時のリスナー
   * @date    2015.07.15
   *
   * @param   id  入力ID
   */
  //--------------------------------------------------------------------------
  virtual void SelectRide( Savedata::FieldMenu::RideID id ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @func    SelectRideInfo
   * @brief   説明選択時のリスナー
   * @date    2016.03.29
   *
   * @param   id  入力ID
   */
  //--------------------------------------------------------------------------
  virtual void SelectRideInfo( Savedata::FieldMenu::RideID id ) = 0;
};

GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)

#endif  // RIDE_DRAW_LISTENER_H_INCLUDED
