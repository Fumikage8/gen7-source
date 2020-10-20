#if !defined( __FINDER_STUDIO_DECO_DATA_ACCESSOR_H_INCLUDED__ )
#define __FINDER_STUDIO_DECO_DATA_ACCESSOR_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioDecoDataAccessor.h
 * @date    2017/02/02 11:10:06
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：デコデータアクセサ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioDefine.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_std_string.h>

#include <niji_conv_header/app/data/finder_studio/finder_studio_deco_type_def.h>
#include <niji_conv_header/app/data/finder_studio/finder_studio_stamp_text_def.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    デコデータアクセサ
///
//////////////////////////////////////////////////////////////////////////
class DecoDataAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( DecoDataAccessor ); //コピーコンストラクタ＋代入禁止

public:
  //-------------------------------------------------------
  // @brief   取得関数
  //-------------------------------------------------------
  static u32 GetDecoID( u32 index );
  static u32 GetDecoID( DECO_TYPE type, u32 index );

  static u32 GetDecoTypeMax( DECO_TYPE type );

  static u32 GetDecoTexID( u32 index );
  static u32 GetDecoTexID( DECO_TYPE type, u32 index );

  static u32 GetDecoThumbID( u32 index );
  static u32 GetDecoThumbID( DECO_TYPE type, u32 index );

  static u32 GetDecoViewTexID( const u32 index );
  static u32 GetDecoViewFrameTexID( const u32 frameIndex );

  static u32 GetUnlockCount( DECO_TYPE type );

  static u32 GetDecoDataListIndex( DECO_TYPE type, u32 index )
  {
    return _GetDecoDataListIndex( type, index );
  }

  //-------------------------------------------------------
  // @brief   チェック関数
  //-------------------------------------------------------
  static bool IsUnlock( DECO_TYPE type, u32 index );

private:
  //-------------------------------------------------------
  // @brief   取得関数：非公開
  //-------------------------------------------------------
  static u32  _GetDecoTypeMax( DECO_TYPE type );
  static u32  _GetDecoDataListIndex( DECO_TYPE type, u32 index );

  //-------------------------------------------------------
  // @brief   チェック関数：非公開
  //-------------------------------------------------------
  static bool _IsUnlock( DECO_TYPE type, u32 index );
};

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_DECO_DATA_ACCESSOR_H_INCLUDED__
