//======================================================================
/**
 * @file FieldFinderModel_define.h
 * @date 2015/10/08 11:53:29
 * @author miyachi_soichi
 * @brief ポケファインダーモデルで使用する定義関連
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_FINDERMODEL_DEFINE_H_INCLUDED__
#define __FIELD_FINDERMODEL_DEFINE_H_INCLUDED__
#pragma once
// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Vector3.h>
// pokelib
#include <pml/include/pmlib.h>
// PokeTool
#include <PokeTool/include/PokeModelPokeFinderMotion.h>
// conv
#include "niji_conv_header/poke_lib/monsno_def.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(TrialModel)

//! ポケファインダー座標最大数
static const u32 FINDER_MOVEPOINT_MAX = 6;
//! 最大アピールポイント数
static const u32 FINDER_APPEAL_MAX = 30;
//! 最大接続先
static const u32 FINDER_CONECTION_MAX = 5;
//! 演技最大数
static const u32 FINDER_ANIM_MAX = 5;
//! エラー用識別子
static const u32 FINDER_ANIM_ERROR = 0xFFFF;
//! カメラ注視解除までの時間
static const u32 FINDER_CAMINTEREST_RESET_FRAME = 10;

/**
 *  @brief  リソース用データ
 */
struct FinderPokeData
{
  bool    enable;   //!< 首向け有効
  f32     deg;      //!< 角度
  f32     frame;    //!< 時間
  f32     wait;     //!< 旋回後の待機時間
  u32     move;     //!< 移動種別
  u32     voice;

  gfl2::math::Vector4 min;
  gfl2::math::Vector4 max;

  FinderPokeData()
    : enable( false )
    , min( -1.0f, -1.0f, -1.0f, 0.0f )
    , max(  1.0f,  1.0f,  1.0f, 0.0f )
  {
  }
};

/**
 *  @brief  分岐パスデータ
 */
struct FinderRouteData
{
  gfl2::math::Vector3 pos;              //!< 座標
  s32 next_idx[FINDER_CONECTION_MAX];   //!< 接続先

  //! コンストラクタ
  FinderRouteData( void )
    : pos()
  {
    for( u32 i = 0; i < FINDER_CONECTION_MAX; ++ i ){ next_idx[i] = -1; }
  }
};
/**
 *  @brief  アピールデータ
 */
struct FinderAppealData
{
  u32 start;      // 開始地点
  u32 end;        // 終了地点
  f32 progress;   // 進捗率
  u32 ratio;      // 確率

  //! コンストラクタ
  FinderAppealData( void )
    : start( 0 ), end( 0 ), progress( 0.0f ), ratio( 0 )
  {
    // @note start=0,end=0で未初期化扱いとする
  }
};
/**
 *  @brief  移動パスデータ
 */
struct FinderPathData
{
  u32 pos_num;                                    //!< 座標の数
  FinderRouteData route[FINDER_MOVEPOINT_MAX];    //!< 分岐パスデータ
  FinderAppealData appeal[FINDER_APPEAL_MAX];     //!< アピール

  //! コンストラクタ
  FinderPathData( void )
    : pos_num( 0 ), route(), appeal()
  {
  }
};
/**
 *  @brief  待機データ
 */
struct FinderStatuePointData
{
  gfl2::math::Vector3 position;
  f32 rotation;
  u16 cool_min;
  u16 cool_max;
};
struct FinderStatueData
{
  u32 num;
  FinderStatuePointData data[3];
};
/**
 *  @brief  レアパスデータ
 */
struct FinderOnewayData
{
  u32 num;
  gfl2::math::Vector3 position[6];
};

/**
 *  @brief  拡張挙動
 */
enum
{
  FINDER_APPEAL_CUSTOM_START  = 1000,  //!< 連鎖
  FINDER_APPEAL_CUSTOM_DROWSE = 1000,  //!< うたたね
  FINDER_APPEAL_CUSTOM_CAMERA = 1001,  //!< カメラ注視
  FINDER_APPEAL_CUSTOM_SEARCH = 1002,  //!< キョロキョロ
};
/**
 *  @brief  撮影用容姿
 */
enum
{
  FIGURE_NONE,    //!< 未設定
  FIGURE_COOL,    //!< かっこいい
  FIGURE_CUTE,    //!< かわいい
  FIGURE_WISE,    //!< かしこい
  FIGURE_TOUGH,   //!< たくましい
  FIBURE_BEAUTY,  //!< うつくしい
};
/**
 *  @brief  パス使用
 */
enum
{
  FINDER_PATH_USE_NONE,   //!< 未設定
  FINDER_PATH_USE_MOVE_A, //!< 移動A
  FINDER_PATH_USE_MOVE_B, //!< 移動B
  FINDER_PATH_USE_WAIT,   //!< 待機
  FINDER_PATH_USE_RARE,   //!< レア

  FINDER_PATH_A = 0,      //!< 移動パスA：配列識別用
  FINDER_PATH_B = 1,      //!< 移動パスB：配列識別用
  FINDER_PATH_MAX = 2,    //!< ファインダー移動パス最大数
};
/**
 *  @brief  モデルデータ
 */
struct FinderModelData
{
  MonsNo monsNo;                                  //!< ポケモン
  pml::FormNo formNo;                             //!< フォルム
  pml::Sex sex;                                   //!< 性別
  u32 pathUse;                                    //!< パス使用
  u32 basePoint;                                  //!< 基礎ポイント
  u8  ext_prm1;                                   //!< 拡張パラメータ1
  u8  ext_prm2;                                   //!< 拡張パラメータ2

  s32 figure;                                     //!< 容姿
  u32 animNo[FINDER_ANIM_MAX];                    //!< 実行するモーション
  u32 voice[FINDER_ANIM_MAX];                     //!< 実行する鳴き声
  s32 figureAnime[FINDER_ANIM_MAX];               //!< アピールのようし

  f32 adjustAngle;                                //!< カメラ注視補正角

  //! コンストラクタ
  FinderModelData( void )
    : monsNo( MONSNO_NULL ), formNo( 0 ), sex( pml::SEX_MALE )
    , pathUse( FINDER_PATH_USE_NONE )
    , basePoint( 0 )
    , ext_prm1( 0 ), ext_prm2( 0 )
    , adjustAngle( 0.0f )
  {
    for( u32 i = 0; i < FINDER_ANIM_MAX; ++ i )
    {
      animNo[i] = FINDER_ANIM_ERROR;
      voice[i] = 0;
    }
  }
};
/**
 *  @brief  スポットデータ
 */
struct FinderSpotData
{
  f32 position[3];  //!< カメラ座標
  f32 rotation[3];  //!< カメラ方向
  f32 limit[4];     //!< カメラ旋回限界角 x-min, x-max, y-min, y-max
  u32 spot_score;   //!< スポットが持つ基礎特典
  u32 spot_type;    //!< スポットが持つ属性
  u32 base_dof;     //!< 被写界深度初期値
};

//! コメント最大数
static const u32 FINDER_VALUE_COMMENT_MAX = 15;
//! 同時登場ポケモン数
static const u32 FINDER_POKE_MODEL_MAX = 2;
//! 写真サイズ
static const u32 SIZE_OF_FINDER_PHOTO = 512*256;
//! サムネイルサイズ
static const u32 SIZE_OF_FINDER_THUMBNAIL = 128*64;
static const u32 SIZE_OF_FINDER_JPEG = 65536;
//! 写真枚数 メモリの関係で２枚まで
static const u32 FINDER_PHOTO_MAX = 6;
//! 評価最大点数
static const u32 FINDER_VALUE_MAX = 99999;

/**
 *  @brief  写真評価データ
 */
struct FinderPhotoValue
{
  u16 comment[FINDER_VALUE_COMMENT_MAX];  //!< コメント*15
  u32 value;                              //!< 点数
#if PM_DEBUG
  f32 base_point[FINDER_POKE_MODEL_MAX];  // 個体が持つ基礎ポイント
  f32 appeal[FINDER_POKE_MODEL_MAX];      // 特別なモーション
  f32 dir[FINDER_POKE_MODEL_MAX];         // 向き
  f32 screen[FINDER_POKE_MODEL_MAX];      // 画面占有率
  f32 focus[FINDER_POKE_MODEL_MAX];       // ピント
  f32 out[FINDER_POKE_MODEL_MAX];         // はみ出具合
  u32 place_score;                        // 場所が持つポイント
  u32 rand_score;                         // 乱数
#endif
};

/**
 *  @brief  写真実体
 */
struct FinderPhotoSnap
{
  u32 data[SIZE_OF_FINDER_PHOTO];          //! 写真
};

/**
 *  @brief  写真サムネイル
 */
struct FinderPhotoThumbnail
{
  u32 data[SIZE_OF_FINDER_THUMBNAIL];  //! サムネイル
};

/**
 *  @brief  写真Jpeg
 */
struct FinderPhotoJpeg
{
  u32 size;
  u8 data[SIZE_OF_FINDER_JPEG];
};
/**
 *  @brief  写真データ
 */
struct FinderPhotoData
{
  FinderPhotoValue      data;       //!< 評価データ
  FinderPhotoJpeg       jpeg;
};

/**
 *  @brief  スポットにひもづくポケモン予約データ
 */
struct FinderSpotReserveData
{
  s32 monsIdx[FINDER_POKE_MODEL_MAX];   //!< 予約しているポケモンのデータ配列番号
  s32 year;                       //!< 年：予約を行った時間
  s32 month;                      //!< 月：予約を行った時間
  s32 day;                        //!< 日：予約を行った時間
  s32 hour;                       //!< 時：予約を行った時間
  s32 minute;                     //!< 分：予約を行った時間
  s32 second;                     //!< 秒：予約を行った時間
  b8  bReserve;                   //!< 予約の有無
  u8  timezone;                   //!< 時間帯
  u8  padding[2];


  // ctor
  FinderSpotReserveData( void )
    : year(0), month(0), day(0), hour(0), minute(0), second(0)
    , bReserve( false )
  {
    monsIdx[0]=-1;
    monsIdx[1]=-1;
    timezone = 0;
    padding[0]=0;
    padding[1]=0;
  }
};

GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_FINDERMODEL_DEFINE_H_INCLUDED__
