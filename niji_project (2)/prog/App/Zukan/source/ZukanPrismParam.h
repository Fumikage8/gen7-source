#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANPRISMPARAM_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANPRISMPARAM_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanPrismParam.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2016.03.06
 @brief   図鑑のキラキラのパラメータ
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// 図鑑のインクルード
#include "ZukanDefine.h"


#define NIJI_APP_ZUKAN_PRISM_DEVELOP (1)  // これが1のとき、調整したキラキラを使う。必ず1でコミット。


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


#if NIJI_APP_ZUKAN_PRISM_DEVELOP
//==============================================================================
/**
 @class     ZukanPrismDevelopParam
 @brief     図鑑でキラキラを開発中に用いるパラメータ→本番でも用いることになりました
 */
//==============================================================================
class ZukanPrismDevelopParam
{
public:
  enum TargetPlace
  {
    TARGET_PLACE_GRID_CENTER,
    TARGET_PLACE_GRID_CORNER,
    TARGET_PLACE_BG_RT,
    TARGET_PLACE_BG_RB,
    TARGET_PLACE_BG_LB,
    TARGET_PLACE_BG_LT,      
    TARGET_PLACE_NUM,
  };
  enum TargetColor  // 0.0f<=色<=1.0f
  {
    TARGET_COLOR_R,
    TARGET_COLOR_G,
    TARGET_COLOR_B,
    TARGET_COLOR_NUM,
  };

  enum Mode
  {
    MODE_CONST,
    MODE_GYRO,
    MODE_NUM,
  };
  enum GyroAngle
  {
    GYRO_ANGLE_X,
    GYRO_ANGLE_Y,
    GYRO_ANGLE_Z,
    GYRO_ANGLE_AVE,
    GYRO_ANGLE_NUM,
  };
  enum FuncType
  {
    FUNC_TYPE_COS,
    FUNC_TYPE_SIN,
    FUNC_TYPE_NUM,
  };
  enum FuncParam
  {
    FUNC_PARAM_SHINPUKU_MIN,  // 0.0f<=MINが望ましいが0.0f未満でもいい     // BAIRITSU倍していないときの範囲   -10.0f<=  <=  +10.0f
    FUNC_PARAM_SHINPUKU_MAX,  // MAX<=1.0fが望ましいが1.0fより大でもいい   // BAIRITSU倍していないときの範囲   -10.0f<=  <=  +10.0f
    FUNC_PARAM_SYUUKI,  // 0.0f度以上360.0f度未満の角度に掛ける周期        // BAIRITSU倍していないときの範囲 -1000.0f<=  <=+1000.0f
    FUNC_PARAM_ISOU,    // 0.0f度以上360.0f度未満の角度に加える位相        // BAIRITSU倍していないときの範囲 -1000.0f<=  <=+1000.0f
    FUNC_PARAM_ISOU_X,
    FUNC_PARAM_ISOU_Y,      
    FUNC_PARAM_NUM,
  };
  // shinpuku = FUNC_PARAM_SHINPUKU_MAX - FUNC_PARAM_SHINPUKU_MIN
  // angle = ジャイロの角度(0.0f度以上360.0f度未満)
  // x, y = 頂点のインデックス
  // color = 色(0.0f(黒)<=color<=1.0f(白)が望ましいが、振幅によっては0.0f未満、1.0fより大にも成り得る)
  // color = (shinpuku/2.0f) * cos( syuuki*angle + isou_x*x + isou_y*y + isou ) + ((shinpuku/2.0f)+shinpuku_min)

  static const f32 BAIRITSU;  // 100倍

  struct TargetParam
  {
    s32 mode;        // Mode

    // MODE_CONST
    s32 constColor;  // 0.0f<= <=1.0fではなく0<= <=255とする。BAIRITSU倍(100倍)していない。

    // MODE_GYRO
    s32 gyroAngle;   // GyroAngle
    s32 funcType;    // FuncType
    s32 funcParam[FUNC_PARAM_NUM];  // 数値はf32を使わずにs32を使う。BAIRITSU倍(100倍)した値にしておく。この計算結果の色はBAIRITSU倍(100倍)していないときは0.0f<= <=1.0fになる。
  };

  struct ReduceParam
  {
    // ジャイロ稼働中の角度  0度以上360度未満をBAIRITSU倍(100倍)した値
    s32 moveAngleBig;    // 大きく動いているとみなす
    s32 moveAngleSmall;  // 小さく動いているとみなす

    // 明るさ減衰  0%(暗い)～100%(明るい)をBAIRITSU倍(100倍)した値
    s32 reduceRateBig;    // 大きく動いているときの明るさ減衰
    s32 reduceRateSmall;  // 小さく動いているときの明るさ減衰

    // 減衰速度  1秒間に○%(BAIRITSU倍(100倍)した値)ずつ変化させる
    s32 reduceSpeedToDark;    // 暗くなっていくときの速度
    s32 reduceSpeedFromDark;  // 明るくなっていくときの速度
  };

public:
  static TargetParam s_targetParam[TARGET_PLACE_NUM][TARGET_COLOR_NUM];
  static ReduceParam s_reduceParam;

#if PM_DEBUG
public:
  static void Print(void);
  static void Reset(bool allReset);

private:
  static void resetTargetParam(TargetParam* targetParam);

public:
  static s32 s_currTargetPlace;                    // TargetPlace
  static s32 s_currTargetColor[TARGET_PLACE_NUM];  // TargetColor

  static s32 s_gyroAnglePrintFlag;
#endif  // PM_DEBUG

};
#endif  // NIJI_APP_ZUKAN_PRISM_DEVELOP


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANPRISMPARAM_H_INCLUDED

