//=============================================================================
/**
 * @file    StaffListDef.h
 * @brief   momiji追加分のスタッフロール画面共通定義
 * @author  Hiroyuki Nakamura
 * @date    2017.03.06
 */
//=============================================================================
#if !defined( STAFFLIST_DEF_H_INCLUDED )
#define STAFFLIST_DEF_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(StaffList)

//! 特殊リクエスト
enum ExRequestNo
{
  EX_REQUEST_NO_END = 0xffff,   //!< 終了リクエスト
};

//! 背景配置
enum BgPutMode
{
  BG_PUT_LEFT = 0,  //!< 左
  BG_PUT_RIGHT,     //!< 右
  BG_PUT_CENTER,    //!< 中央
};

GFL_NAMESPACE_END(StaffList)
GFL_NAMESPACE_END(App)

#endif  // STAFFLIST_DEF_H_INCLUDED
