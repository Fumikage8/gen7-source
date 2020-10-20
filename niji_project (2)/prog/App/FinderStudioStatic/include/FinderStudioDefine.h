#if !defined( __FINDER_STUDIO_DEFINE_H_INCLUDED__ )
#define __FINDER_STUDIO_DEFINE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioDefine.h
 * @date    2016/11/18 13:31:40
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：定義
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioViewerDefine.h"

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_Math.h>

#if defined(GF_PLATFORM_CTR)
#include <imagedb/include/gfl2_ImageDBAccessor.h>
#endif

#include <niji_conv_header/app/data/finder_studio/finder_studio_bg_def.h>
#include <niji_conv_header/app/data/finder_studio/finder_studio_frame_def.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )

//////////////////////////////////////////////////////////////////////////
/// ---[ 定数定義 begin ]---
/// ImageDB写真管理枚数
static const u32 IMGDB_PHOTO_MAX = 3000;
/// 保存可能な写真枚数
static const u32 SAVE_PHOTO_MAX = 420;    // アローラ図鑑が収まる数。6 * 70ページ分
/// 保存可能枚数の注意喚起枚数
static const u32 ATTENTION_PHOTO_NUM = 10;
/// 保存不可能枚数
static const u32 SAVE_PHOTO_MIN = 0;
/// 写真の撮影枚数
static const u8  CAPTURE_COUNT = 6;

/// 通常モード用デフォルト背景インデックス
static const u32 DEFAULT_BACKGROUND_INDEX = BG_ID_00;
/// 進化モード専用デフォルト背景インデックス
static const u32 EVOLUTION_BACKGROUND_INDEX = BG_ID_65;

/// 写真バッファサイズ 幅
static const u32 PHOTO_BUFFER_WIDTH = 256;
/// 写真バッファサイズ 高さ
static const u32 PHOTO_BUFFER_HEIGHT = 512;
/// 写真バッファサイズ
static const u32 PHOTO_BUFFER_SIZE = sizeof( u32 ) * PHOTO_BUFFER_WIDTH * PHOTO_BUFFER_HEIGHT;
/// 写真サムネイルバッファサイズ 幅
static const u32 THUMB_BUFFER_WIDTH = 128;    // @caution 128未満はイメージ形式のコンバートでGL_ASSERTになる
/// 写真サムネイルバッファサイズ 高さ
static const u32 THUMB_BUFFER_HEIGHT = 128;   // @caution 128未満はイメージ形式のコンバートでGL_ASSERTになる
/// 写真サムネイルバッファサイズ
static const u32 THUMB_BUFFER_SIZE = sizeof( u32 ) * THUMB_BUFFER_WIDTH * THUMB_BUFFER_HEIGHT;
/// ディスプレイバッファ用アライメント
static const u32 DISPLAY_BUFFER_ALIGN = 16;
/// 写真バッファアライメント
static const u32 PHOTO_BUFFER_ALIGN = 128;
/// ブロック形式のブロックサイズ
static const u32 BLOCK_IMAGE_BLOCK_SIZE = 8;

/// 移動可能範囲(直径)：Viewer
static const f32 VIEWER_MOVE_DIAMETER = 500.0f;
/// 移動可能範囲(半径)：Viewer
static const f32 VIEWER_MOVE_RADIUS = VIEWER_MOVE_DIAMETER / 2.0f;
/// 移動可能範囲(直径)：UI
static const f32 UI_MOVE_DIAMETER = 210.0f;
/// 移動可能範囲(半径)：UI
static const f32 UI_MOVE_RADIUS = UI_MOVE_DIAMETER / 2.0f;
/// 移動可能範囲の比率
static const f32 MOVE_DIAMETER_RATE = UI_MOVE_DIAMETER / VIEWER_MOVE_DIAMETER;

/// 配置可能なデコの数
static const u8  DECO_ITEM_MAX = 10;
/// デコ下画面の上画面とのスケール比
static const f32 DECO_SCALE_RATE = 0.7f;
/// デコのNULL値
static const u32 DECO_NULL = 0xFF;    //!< 8ビットの最大値
/// フレームのNULL値
static const u32 FRAME_NULL = DECO_FRAME_NUM;

/// 保存先のファイルパスの最大文字数
#if defined(GF_PLATFORM_CTR)
static const u8  SAVE_FILEPATH_LENGTH = imgdb::imgio::c_nMaxPathSize;
#else
static const u8  SAVE_FILEPATH_LENGTH = 44;
#endif

/// 難易度
enum DifficultyMode
{
  DIFF_MODE_EASY,       //!< お手軽
  DIFF_MODE_AUTHENTIC,  //!< 本格
  DIFF_MODE_NUM,
};

/// ---[ 定数定義 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
/// ---[ 構造体定義 begin ]---
/// 日時データ
struct Date
{
  u16 year;     //!< 撮影日時：年
  u8  month;    //!< 撮影日時：月
  u8  day;      //!< 撮影日時：日
  u8  hour;     //!< 撮影日時：時
  u8  minute;   //!< 撮影日時：分
  u8  second;   //!< 撮影日時：秒
  u8  padding;  //!< パディング

  Date( void )
    : year( 2000 ), month( 1 ), day( 1 ), hour( 0 ), minute( 0 ), second( 0 ), padding( 0 )
  {
    ;
  }
};
/// 管理データ
struct ManagementData
{
  u16   filepath[App::FinderStudio::Static::SAVE_FILEPATH_LENGTH];    //!< ファイルパス
  u16   mons_no;        //!< 種族No
  Date  date;           //!< 日時
  u8    deco_flg;       //!< 編集フラグ：0=未編集、1=編集済み
  u8    favorite_flg;   //!< お気に入りフラグ：0=未登録、1=登録
  u8    cap_mode;       //!< 撮影モード：App::FinderStudio::Viewer::CaptureMode参照
  u8    bg_index;       //!< 背景インデックス
  u8    joint_flg;      //!< 共有フラグ：0=未共有、1=共有
};
/// スタンプデータ
struct StampData
{
  u32   id : 8;
  u32   x : 9;
  u32   y : 9;
  u32   frame_id : 6;   //! フレームID （通信のサイズがきついのでこの場所を借りる。要素0に正しいデータが入る。）
};
/// デコデータ
struct DecorationData
{
  //
  //  ※末尾でcrc計算しているので並びは変えないでください！！！ ichiraku
  //
  StampData deco_param[App::FinderStudio::Static::DECO_ITEM_MAX];   //!< スタンプデータ
  u32       crc;
};
/// 写真用データ
struct PhotoData
{
  App::FinderStudio::Viewer::PhotoParam photo_param;    //!< 写真用パラメータ
  DecorationData deco_param;    //!< デコデータ
};
/// 統合データ
struct IntegratedData
{
  ManagementData management_data;   //!< 管理用データ
  PhotoData      photo_data;        //!< 写真用データ
};


/// スタンプデータのNULLデータ定義
static const StampData STAMP_NULL_DATA = { DECO_NULL, 0, 0, FRAME_NULL };

/// ---[ 構造体定義 end ]---
//////////////////////////////////////////////////////////////////////////



//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
static inline bool  IsAvailableDecoID(const u32 decoID)
{
  return (decoID < DECO_NULL);
}

//------------------------------------------------------------------
/**
*  @brief
*/
//------------------------------------------------------------------
static inline bool  IsAvailableDecoID( const StampData& rStampData )
{
  return IsAvailableDecoID( rStampData.id );
}

//------------------------------------------------------------------
/**
*  @brief
*/
//------------------------------------------------------------------
static inline bool  IsAvailableFrameID( const u32 frameID )
{
  return ( frameID < FRAME_NULL );
}

//------------------------------------------------------------------
/**
*  @brief
*/
//------------------------------------------------------------------
static inline bool  IsAvailableFrameID( const StampData& rStampData )
{
  return IsAvailableFrameID( rStampData.frame_id );
}



//------------------------------------------------------------------
/**
*  @brief レイアウト座標系からスクリーン座標系へ変換()
*/
//------------------------------------------------------------------
static inline gfl2::math::VEC3 ConvertUpperVectorLayoutToScreen( gfl2::math::VEC3 vec )
{
  gfl2::math::VEC3 vecScreen( 0, 0, 0 );
  {
    vecScreen.x = vec.x + 200.0f;
    vecScreen.y = ( vec.y * -1.0f ) + 120.0f;
  }

  return vecScreen;
}

//------------------------------------------------------------------
/**
*  @brief スクリーン座標系からレイアウト座標系へ変換
*/
//------------------------------------------------------------------
static inline gfl2::math::VEC3 ConvertUpperVectorScreenToLayout( gfl2::math::VEC3 vec )
{
  gfl2::math::VEC3 vecLayout( 0, 0, 0 );
  {
    vecLayout.x = vec.x - 200.0f;
    vecLayout.y = ( vec.y * -1.0f ) + 120.0f;
  }

  return vecLayout;
}


//////////////////////////////////////////////////////////////////////////
/// ---[ デバッグ用データ定義 begin ]---
#if PM_DEBUG
// デフォルト写真パラメータ
static const App::FinderStudio::Viewer::PhotoParam c_DefPhotoParam = {
  103,
  1,
  0,
  1,
  1,
  1,
  1,
  1,
  1,

  8,
  1,
  1,
  1,
  34,

  1,
  1,
  1,
  1,
  0,

  0,
  0,
  0,

  0,
  0,

  0,
  0,
  0,
  1,
  0,
  8,
  30,
  0,

  1,
  1,
  1,
  1,
  30,
  30,

  30,
  30,
  1,
  1,
  2,

  10,
  180,
  10,

  1,
  90,
  90,
  100,
  10,

  24000,
  12000,

  0,
  0,
  0,
  0
};
#endif
/// ---[ デバッグ用データ定義 end ]---
//////////////////////////////////////////////////////////////////////////

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_DEFINE_H_INCLUDED__