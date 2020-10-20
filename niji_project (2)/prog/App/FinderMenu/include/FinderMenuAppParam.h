#if !defined( __FINDER_MENU_APP_PARAM_H__ )
#define __FINDER_MENU_APP_PARAM_H__
#pragma once
//======================================================================
/**
 * @file    FinderMenuAppParam.h
 * @date    2015/11/19 20:31:00
 * @author  fukushima_yuya
 * @brief   外部設定パラメータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(TrialModel)
struct FinderPhotoData;
GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_END(Field)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
static const u32 PHOTO_DATA_MAX = 6;

//------------------------------------------------------------------------------
// @brief   構造体
//------------------------------------------------------------------------------
struct APP_PARAM
{
  // [in]
  u8    shutter_count;    //!< 撮影枚数

  Field::TrialModel::FinderPhotoData*   photo_data[PHOTO_DATA_MAX];   //!< 写真データ

  // [out]
  bool  finder_continue;  //!< 撮影を継続するフラグ

  //------------------------------------------------------------------
  //　@brief   コンストラクタ
  //------------------------------------------------------------------
  APP_PARAM( void )
    : shutter_count( PHOTO_DATA_MAX )
    , finder_continue( false )
  {
    for( u32 i=0; i<PHOTO_DATA_MAX; ++i )
    {
      photo_data[i] = 0;
    }
  }
};

GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)

#endif // __FINDER_MENU_APP_PARAM_H__