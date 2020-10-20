#ifndef NIJI_PROJECT_HERO_SELECT_APP_PARAM_H_INCLUDED
#define NIJI_PROJECT_HERO_SELECT_APP_PARAM_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file HeroSelectAppParam.h
 * @date 2015/08/26 18:55:03
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <str/include/gfl2_Str.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( Savedata )
class MyStatus;
GFL_NAMESPACE_END( Savedata )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( HeroSelect )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
//! 戻り値
enum EndMode {
  END_MODE_NONE,
  END_MODE_STRINPUT,    //!< 文字入力へ
  END_MODE_NEXT,        //!< イントロデモ後半へ
  END_MODE_NUM,
};

//! 
enum {
  NAME_INPUT_BEFORE,
  NAME_INPUT_AFTER,
  NAME_INPUT_NUM,
};

//------------------------------------------------------------------------------
// @brief   構造体
//------------------------------------------------------------------------------
struct APP_PARAM
{
  // [in]
  EndMode endMode;  //!< 終了モード

  u8 name_input;    //!< 0:入力前、1:入力後

  gfl2::str::StrBuf*    name_buf;   //!< 名前

  Savedata::MyStatus*   my_status;  //!< MyStatus：Proc内で設定

  // [out]
  u32 select_index;   //!< 選択されたインデックス

  // コンストラクタ
  APP_PARAM( void )
    : endMode( END_MODE_NONE )
    , name_input( NAME_INPUT_BEFORE )
    , name_buf( NULL )
    , my_status( NULL )
    , select_index( 0 )
  { ; }
};

GFL_NAMESPACE_END( HeroSelect )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_HERO_SELECT_APP_PARAM_H_INCLUDED