#if !defined( __FINDER_STUDIO_ALBUM_WORK_H_INCLUDED__ )
#define __FINDER_STUDIO_ALBUM_WORK_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioAlbumWork.h
 * @date    2016/11/22 13:12:20
 * @author  fukushima_yuya
 * @brief   ファインダースタジオアルバム内共通データ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDefine.h"

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_std_string.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )

//////////////////////////////////////////////////////////////////////////
/// ---[ 定数定義 begin ]---

// イベントリザルト
enum EventResult {
  EVE_RES_CONTINUE,
  EVE_RES_CALL_PHOTO_LIST,
  EVE_RES_CALL_DECORATION,
  EVE_RES_FINISH,           //!< 通常終了
  EVE_RES_ERROR,            //!< エラー終了
  EVE_RES_FATAL_ERROR,      //!< フェイタルエラー
  EVE_RES_NUM,
};

/// ---[ 定数定義 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///
/// @brief    共通データ構造体
///
/// @note     マイアルバム、デコのアプリ共通データ。
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   マイアルバム・デコ間のやり取り用データ
 */
struct AlbumWork
{
  /// [in]
  IntegratedData  photo_data;   //!< 写真１枚分のデータ

  u32     photo_data_index;     //!< 写真データインデックス：拡張セーブデータから読み込む際のインデックス

  u32     photo_count;          //!< 写真の保存枚数

  /// [out]
  EventResult   event_result;

  AlbumWork( void )
    : photo_data_index( 0 )
    , photo_count( 0 )
    , event_result( EVE_RES_CONTINUE )
  {
    gfl2::std::MemClear( &photo_data, sizeof( IntegratedData ) );
  }
};

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_ALBUM_WORK_H_INCLUDED__