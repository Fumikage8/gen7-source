#if !defined( __FINDER_STUDIO_CAPTURE_WORK_H_INCLUDED__ )
#define __FINDER_STUDIO_CAPTURE_WORK_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureWork.h
 * @date    2016/11/09 19:29:50
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：共通データ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioCapture/source/Frame/FinderStudioCaptureFrameDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioViewerDefine.h>

#include <PokeTool/include/PokeTool.h>

#include <util/include/gfl2_std_string.h>
#include <model/include/gfl2_DressUpParam.h>
#include <pml/include/pmlib.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 定数定義 begin ]---
static const u8 PHOTO_MAX = Static::CAPTURE_COUNT;
/// ---[ 定数定義 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///
/// @brief    共通データ構造体
///
/// @note     撮影イベント中は生存し、イベント終了で破棄されるデータ。
///           着せ替えアプリなどイベント内でのアプリ遷移では消えない。
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   アプリ用データ
 */
struct AppWork
{
  Frame::FrameID          start_frame_id;         //!< 開始フレームID
  bool                    evolution_mode;         //!< 進化専用撮影モード
  bool                    call_kisekae;           //!< 着せ替えコールフラグ
  bool                    fatal_error;            //!< フェイタルエラーフラグ
  Static::DifficultyMode  diff_mode;              //!< 難易度設定
  u32                     save_photo_count;       //!< 写真保存可能枚数
  u32                           dressup_save_index;     //!< 着せ替えアプリで変更するセーブ領域のインデックス
  poke_3d::model::DressUpParam  dressup_save_param;     //!< 着せ替え元のデータ

  /// コンストラクタ
  AppWork( void )
    : start_frame_id( Frame::FRAME_ID_SDCARD_LOAD )
    , evolution_mode( false )
    , call_kisekae( false )
    , fatal_error( false )
    , diff_mode( Static::DIFF_MODE_EASY )
    , save_photo_count(Static::SAVE_PHOTO_MAX)
    , dressup_save_index( 0 )
  {
    gfl2::std::MemClear( &dressup_save_param, sizeof( poke_3d::model::DressUpParam ) );
  }
};

/**
 * @brief   準備用データ
 */
struct PrepareWork
{
  Viewer::CaptureMode           capture_mode;     //!< 撮影モード
  pml::pokepara::PokemonParam*  pokemon_param;    //!< ポケモン
  u32                           member_id;        //!< メンバーID
  u32                           background_id;    //!< 背景ID
  u32                           dressup_id;       //!< 衣装ID
  Viewer::Composition           composition_id;   //!< 構図ID

  /// コンストラクタ
  PrepareWork( void )
    : capture_mode( Viewer::CAPTURE_MODE_BOTH )
    , pokemon_param( NULL )
    , member_id( 0 )
    , background_id( 0 )
    , dressup_id( 0 )
    , composition_id( Viewer::COMPOSITION_MAX )
  {
    ;
  }
};

/**
 * @brief   Viewer用
 */
struct ViewerWork
{
  Viewer::CaptureMode           capture_mode;       //!< 撮影モード
  PokeTool::SimpleParam         simple_param;       //!< ポケモン
  u32                           background_id;      //!< 背景ID
  Viewer::TimeZone              time_zone;          //!< 時間帯
  poke_3d::model::DressUpParam  dressup_param;      //!< 着せ替えデータ
  Viewer::ZMotionType           z_motion_type;      //!< Zモーション
  Viewer::PokeMotionType        poke_motion_type;   //!< ポケモンモーションタイプ

  void* photo_tex[PHOTO_MAX];                       //!< ディスプレイバッファ

  Viewer::PhotoParam  rest_viewer_param;            //!< 復帰用データ
  Viewer::PhotoParam  photo_param[PHOTO_MAX];       //!< 写真データ

  /// コンストラクタ
  ViewerWork( void )
    : capture_mode( Viewer::CAPTURE_MODE_BOTH )
    , simple_param()
    , background_id( 0 )
    , time_zone( Viewer::TIME_ZONE_NOON )
    , dressup_param()
    , z_motion_type( Viewer::Z_MOTION_TYPE_NORMAL )
    , poke_motion_type( Viewer::POKE_MOTION_TYPE_BATTLE )
  {
    gfl2::std::MemClear( &simple_param, sizeof( PokeTool::SimpleParam ) );
    gfl2::std::MemClear( &dressup_param, sizeof( poke_3d::model::DressUpParam ) );
    gfl2::std::MemClear( &rest_viewer_param, sizeof( Viewer::PhotoParam ) );

    for( u8 i = 0; i < PHOTO_MAX; ++i )
    {
      photo_tex[i] = NULL;
      gfl2::std::MemClear( &photo_param[i], sizeof( Viewer::PhotoParam ) );
    }
  }
};

/**
 * @brief   ワーク
 */
struct Work
{
  AppWork       app;
  PrepareWork   prepare;
  ViewerWork    viewer;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_WORK_H_INCLUDED__
