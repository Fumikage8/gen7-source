//=============================================================================
/**
 * @file    TownMapAppParam.h
 * @brief   タウンマップ画面外部設定パラメータ
 * @author  Hiroyuki Nakamura
 * @date    2015.10.13
 */
//=============================================================================
#if !defined( TOWNMAP_APP_PARAM_H_INCLUDED )
#define TOWNMAP_APP_PARAM_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(TownMap)

//! 呼び出しモード
enum CallMode
{
  MODE_TOWNMAP = 0,   //!< タウンマップ
  MODE_FLY,           //!< そらをとぶ
};

enum
{
  FLY_WARPID_CANCEL = -1,   //!< "そらをとぶ"をキャンセルしたときのワープID
};

//! 外部設定パラメータ
struct APP_PARAM
{
  // [in]
  CallMode call_mode;   //!< 呼び出しモード
  u32 land_no;          //!< 島番号

  // [out]
  s32 warp_id;          //!< ワープID（そらをとぶ用）
};

GFL_NAMESPACE_END(TownMap)
GFL_NAMESPACE_END(App)

#endif  // TOWNMAP_APP_PARAM_H_INCLUDED
