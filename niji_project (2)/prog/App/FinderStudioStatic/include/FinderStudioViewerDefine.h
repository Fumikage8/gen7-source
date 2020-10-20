//======================================================================
/**
 * @file    FinderStudioDefine.h
 * @date    2016/12/05
 * @author  harada_takumi
 * @brief   ファインダースタジオビューワ：定義
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined( __FINDER_STUDIO_VIEWER_DEFINE_H_INCLUDED__ )
#define __FINDER_STUDIO_VIEWER_DEFINE_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Viewer )

/// 写真再現用パラメータ
struct PhotoParam
{
  u32 monsNo : 10;        /// ポケモンのモンスターNo
  u32 formNo : 5;         /// ポケモンのフォルムNo
  u32 sexPoke : 2;        /// ポケモンの性別
  u32 isRare : 1;         /// ポケモンの色違いフラグ
  u32 sexTrainer : 1;     /// トレーナーの性別
  u32 skinColor : 2;      /// トレーナーの体の色
  u32 eyeColor : 4;       /// トレーナーの目の色
  u32 hairColor : 4;      /// トレーナーの髪の色
  u32 lipColor : 3;       /// トレーナーの唇の色

  u32 hairStyle : 5;      /// トレーナーの髪型
  u32 eyeAccessory : 5;   /// トレーナーのアイアクセサリ
  u32 hairAccessory : 6;  /// トレーナーのヘアアクセサリ
  u32 bag : 8;           /// トレーナーのバッグ
  u32 bottoms : 8;       /// トレーナーのボトムス

  u32 hat : 7;            /// トレーナーの帽子
  u32 leg : 7;            /// トレーナーのレッグ
  u32 shoes : 8;         /// トレーナーのシューズ
  u32 tops : 9;           /// トレーナーのトップス
  u32 padding1 : 1;        /// padding

  u32 bgGroundLoopAnmFrame : 10;   /// 背景地面のループアニメーションフレーム
  u32 bgNearLoopAnmFrame : 10;   /// 背景近景のループアニメーションフレーム
  u32 bgFarLoopAnmFrame : 10;   /// 背景遠景のループアニメーションフレーム
  u32 situ : 2;                 /// トレーナーのシチュエーション着替え

  u32 skyCloudAnmFrame : 16; /// 天球の雲のアニメーションフレーム
  u32 skySunAnmFrame : 16;   /// 天球の太陽のアニメーションフレーム（ギラギラ）

  u32 bgSetIndex : 7;      /// 背景セットインデックス
  u32 skyMoonPattern : 3;  /// 天球の月の満ち欠けパターン
  u32 timeZone : 2;        /// 時間帯（昼or夕or夜）
  u32 pokeMotionType : 1;  /// バトルorかわいがり
  u32 pokeMotionNo : 5;    /// モーションNo
  u32 pokeMainMotionFrame : 8; /// 通常モーションフレーム
  u32 bgTimeAnmFrame : 5;   /// 背景の時間アニメーションフレーム（共通）
  u32 captureMode : 1;        /// 撮影モード

  u32 pokeEyeAnmPattern1 : 3;    /// 目パチパターン１
  u32 pokeEyeAnmPattern2 : 3;    /// 目パチパターン２
  u32 pokeEyeAnmPattern3 : 3;    /// 目パチパターン３
  u32 pokeMouthAnmpattern1 : 3;  /// 口パターン１
  u32 pokeLoopAnmFrame1 : 10;   /// ループアニメーションフレーム１
  u32 pokeLoopAnmFrame2 : 10;   /// ループアニメーションフレーム２

  u32 pokeLoopAnmFrame3 : 10;   /// ループアニメーションフレーム３
  u32 pokeLoopAnmFrame4 : 10;   /// ループアニメーションフレーム４
  u32 pokeMouthAnmpattern2 : 3;  /// 口パターン２
  u32 pokeMouthAnmpattern3 : 3;  /// 口パターン３
  u32 trainerMotionType : 6; /// トレーナーモーション種

  u32 trainerMotionFrame : 16;   /// トレーナーモーションフレーム
  u32 cameraDegY : 9;       /// カメラY軸角度
  u32 cameraDegX : 7;        /// カメラX軸角度

  u32 pokeDegY : 9;             /// ポケモンY軸角度
  u32 trainerDegY : 9;          /// トレーナーY軸角度
  u32 bgTimeAnmType : 4;   /// 背景の時間アニメーションタイプ
  u32 bgBuildLoopAnmFrame : 10;   /// 背景頂点アニメのループアニメーションフレーム

  s32 pokePosX : 16;   /// ポケモンX座標
  s32 pokePosZ : 16;   /// ポケモンZ座標

  // 以下、３バイト
  u32 trainerEyeAnmPattern : 2;  /// トレーナー目パチパターン
  u32 cameraHeight : 7;              /// カメラ高さ
  u32 cameraFocusFactor : 7;     /// カメラ係数
  u32 cameraFov : 7;        /// カメラ画角
  u32 composition : 1;         /// 構図

  //////////////////////////////////////////////////////////////////////////
  // ※注意！
  //  通信で末尾の1バイトを使用するので使用禁止！
  //////////////////////////////////////////////////////////////////////////
  u32 packetHeaderSymbol : 8;    /// 通信で使用する

};

/// ビューワモード
enum ViewerMode{
  /// 通常モード：アプリ復帰時などに使用
  VIEWER_MODE_NORMAL,
  /// 写真再現モード：写真閲覧時に使用
  VIEWER_MODE_PHOTO,

  VIEWER_MODE_MAX
};

/// 撮影モード
enum CaptureMode{
  CAPTURE_MODE_POKE,  /// ポケモンだけ撮影
  CAPTURE_MODE_BOTH,   /// ポケモンとトレーナー一緒に撮影

  CAPTURE_MODE_MAX
};

/// Zモーションのタイプ
enum ZMotionType{
  Z_MOTION_TYPE_NORMAL,
  Z_MOTION_TYPE_HONOO,
  Z_MOTION_TYPE_MIZU,
  Z_MOTION_TYPE_KUSA,
  Z_MOTION_TYPE_DENKI,
  Z_MOTION_TYPE_KOORI,
  Z_MOTION_TYPE_KAKUTOU,
  Z_MOTION_TYPE_DOKU,
  Z_MOTION_TYPE_ZIMEN,
  Z_MOTION_TYPE_HIKOU,
  Z_MOTION_TYPE_ESUPAA,
  Z_MOTION_TYPE_MUSI,
  Z_MOTION_TYPE_IWA,
  Z_MOTION_TYPE_GOOSUTO,
  Z_MOTION_TYPE_DORAGON,
  Z_MOTION_TYPE_AKU,
  Z_MOTION_TYPE_HAGANE,
  Z_MOTION_TYPE_FEARII,
  Z_MOTION_TYPE_PIKA,
  Z_MOTION_TYPE_GREET,  // 挨拶モーション

  Z_MOTION_TYPE_MAX
};

/// 時間帯
enum TimeZone{
  TIME_ZONE_NOON,
  TIME_ZONE_EVENING,
  TIME_ZONE_NIGHT,

  TIME_ZONE_MAX
};

/// ポケモンモーションタイプ
enum PokeMotionType{
  POKE_MOTION_TYPE_BATTLE,
  POKE_MOTION_TYPE_KAWAIGARI,

  POKE_MOTION_TYPE_MAX
};

/// 構図
enum Composition{
  COMPOSITION_1,  /// 隣り合わせ
  COMPOSITION_2,  /// 向かい合わせ

  COMPOSITION_MAX
};

// 初期Y軸角度
const u32 DEFAULT_CAMERA_DEGREE_Y = 180;

GFL_NAMESPACE_END( Viewer )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_VIEWER_DEFINE_H_INCLUDED__