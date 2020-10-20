//=============================================================================
/**
 * @file    MyAlbumParam.h
 * @brief   マイアルバム画面外部設定パラメータ
 * @author  Hiroyuki Nakamura
 * @date    2017.01.25
 */
//=============================================================================
#if !defined( MYALBUM_APP_PARAM_H_INCLUDED )
#define MYALBUM_APP_PARAM_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// momiji
#include "App/FinderStudioStatic/include/FinderStudioDefine.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MyAlbum)

//! 終了モード
enum EndMode
{
  END_MODE_EXIT = 0,  //!< 通常終了
  END_MODE_DECO,      //!< 編集モードへ
  END_MODE_ERROR,     //!< エラー
  END_MODE_FATAL_ERR, //!< フェイタルエラー
};

enum
{
  MYALBUM_PHOTO_FLAG_SIZE = 53,   //!< フラグバッファサイズ ( 420/8 = 52.5 )

  MYALBUM_INIT_SHARE = 0xffff,    //!< init_shareの初期値
};

// ソート定義
enum
{
  SORT_NONE = 0,
  // 並び
  SORT_LOW_OLD = 1,
  SORT_LOW_NEW,
  SORT_LOW_ZUKAN,
  // 飾りつけ
  SORT_DECO_ON = 1,
  SORT_DECO_OFF,
  // お気に入り
  SORT_FAVORITE_ON = 1,
  SORT_FAVORITE_OFF,
  // 撮り方
  SORT_PHOTO_ONLY = 1,
  SORT_PHOTO_TWOSHOT,
  // 共有
  SORT_SHARE_ON = 1,
  SORT_SHARE_OFF
};

//! ソートデータ
struct SORT_DATA
{
  u32 mons_no:15;   //!< ポケモン番号
  u32 bg_no:7;      //!< 背景番号
  u32 low:2;        //!< 並び
  u32 deco:2;       //!< 飾りつけ
  u32 favorite:2;   //!< お気に入り
  u32 photo:2;      //!< 撮り方
  u32 share:2;      //!< 共有
};

//! 外部設定パラメータ
struct APP_PARAM
{
  // [in/out]
  s32 select_pos;       //!< 選択位置
  SORT_DATA sort_data;  //!< ソートデータ
  bool is_sort_mode;    //!< true = 絞込み状態

  // [out]
  App::FinderStudio::Static::IntegratedData photo_data; //!< 選択された写真データ
  u32 select_index;   //!< 選択された写真のインデックス
  u32 photo_count;    //!< 写真枚数
  EndMode end_mode;   //!< 終了モード

  // 内部用
  u16 init_share;   //!< 初期共有インデックス
  u16 rcv_share;    //!< 復帰用共有インデックス
  u8 init_favorite[MYALBUM_PHOTO_FLAG_SIZE];  //!< 初期お気に入りフラグ
  u8 rcv_favorite[MYALBUM_PHOTO_FLAG_SIZE];   //!< 復帰用お気に入りフラグ
};

GFL_NAMESPACE_END(MyAlbum)
GFL_NAMESPACE_END(App)

#endif  // MYALBUM_APP_PARAM_H_INCLUDED
