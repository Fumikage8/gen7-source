//======================================================================
/**
* @file FieldCameraAreaTypes.h
* @date 2015/07/03 18:52:30
* @author ikuta_junya
* @brief フィールドカメラエリア関連の定義
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if !defined __FIELDCAMERAAREAERTYPES_H_INCLUDED__
#define __FIELDCAMERAAREAERTYPES_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Math.h>
#include <math/include/gfl2_Easing.h>


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );

// @brief フィールドカメラエリアのタイプ定義
struct Type
{
  enum Value
  {
    FOLLOW = 0,             // 通常追従型
    FOLLOW_IPO,             // 2つのパラメータ間を遷移する追従型
    
    // 以下、未実装
    HOLD,
    INHERIT,
    PATH,
    NECROZMA,
    MAX,
    INVALID = 255,          // 無効値
  };
};

struct SupportType
{
  enum Value
  {
    NONE = 0,             // 無し
    DIRECTION,            // 方向サポート
    DISTANCE,             // 距離サポート
    ALL,                  // 全サポート

    MAX
  };
};

// @brief カメラ設定の基底データ
struct MAIN_PARAM
{
  gfl2::math::Vector3 offset;
  gfl2::math::Vector3 rotation;
  f32                 fovY;
  f32                 distance;
};

// @brief カメラエリアデータ番号のタイプ定義
struct AreaDataNo
{
  enum Value
  {
    DEFAULT = 0xFFFF,       // デフォルト設定データ 
    INVALID = 0xFFFE,       // 無効設定データ 
  };
};


// 補間設定データ
struct IPO_SETTING_DATA
{
  u32 frame;                                      // 補間にかけるフレーム    
  gfl2::math::Easing::EaseFunc  easeFunc;         // 補間に使用するイージング関数
};

// カメラサポート、移動設定
struct SUPPORT_MOVE_SETTING_DATA
{
  u16 waitFrame;      // 移動開始までの待ち時間
  u16 moveFrame;      // 移動にかける時間
  gfl2::math::Easing::EaseFunc  easeFunc;  // 移動に使用するイージング関数
};

// カメラサポート、方向サポート設定
struct SUPPORT_DIRECTION_SETTING_DATA
{
  b32 singleAxisFlag;
  f32 singleAxisDegree;
};

// カメラエリアサポートデータ
struct CAMERA_AREA_SUPPORT_SETTING_DATA
{
  SupportType::Value              type;  // サポートタイプ

  // 共通パラメータ
  f32                             maxValue;
  SUPPORT_MOVE_SETTING_DATA       outData;
  SUPPORT_MOVE_SETTING_DATA       inData;

  // 方向サポート用
  SUPPORT_DIRECTION_SETTING_DATA  directionData;
};

struct FocusType
{
  enum Value
  {
    NONE = 0,             // 無し
    TEST_00,
    TEST_DISTANCE_2,
    TEST_DISTANCE_3,
    MAX
  };
};

struct FocusDistanceType
{
  enum Value
  {
    FOVY = 0,             // 無し
    POS,
    MAX
  };
};

struct FocusTargetType
{
  enum Value
  {
    TR_ALL = 0,             // 無し
    TR_STOP_ONLY,
    ALL,
    MAX
  };
};

struct FOCUS_SETTING_DATA
{
  s32 type;
  s32                             distanceMode;
  s32                             targetMode;
  f32                             hitMove;
  f32                             hitDistance;
  f32                             outDistance;
  f32                             eyeDistance;
  gfl2::math::Vector3             targetOffset;
  f32                             maxValue;
  f32                             bgmVolume;
  f32                             hitBgmVolume;
  SUPPORT_MOVE_SETTING_DATA       outData;
  SUPPORT_MOVE_SETTING_DATA       inData;
  SUPPORT_MOVE_SETTING_DATA       moveData;
};

// カメラエリアデータ
struct CAMERA_AREA_SETTING_DATA
{
  Type::Value       type;             // エリアタイプ

  // 共通データ部
  IPO_SETTING_DATA  areaChangeIpo;    // エリア間の補間設定

  // エリアタイプ別データ部 @note 共用体にする予定だったが、Vector3が含まれているため出来なかった
  MAIN_PARAM          paramA;
  MAIN_PARAM          paramB;
  gfl2::math::Easing::EaseFunc  paramEaseFunc;    // パラメータ補間に使用するイージング関数


  CAMERA_AREA_SUPPORT_SETTING_DATA  supportData;
};


GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );


#endif // __FIELDCAMERAAREAERTYPES_H_INCLUDED__