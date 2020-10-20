//======================================================================
/**
 * @file NamiController.h
 * @date 2016/09/13 12:20:58
 * @author fang_yicheng
 * @brief 波コントローラー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __NAMI_CONTROLLER_H_INCLUDED__
#define __NAMI_CONTROLLER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "NamiModelSet.h"
#include "NamiSamplingData.h"
#include "../Course/CourseDataManager.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
class MantainSurfAppManager;
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Nami)

class NamiArea
{
  GFL_FORBID_COPY_AND_ASSIGN(NamiArea);

public:
  NamiArea() :
    m_width(0.0f),
    m_height(0.0f),
    m_coefficient(0.0f)
  {}

  virtual ~NamiArea() {}

  void Initialize(f32 width, f32 height) { m_width = width; m_height = height; }

  gfl2::math::Vector3 GetPosition()              { return m_position; }
  void SetPosition(gfl2::math::Vector3 position) { m_position = position; }

  f32 GetWidth()  { return m_width; }
  f32 GetHeight() { return m_height; }

  f32 GetCoefficient() { return m_coefficient; }
  void SetCoefficient(f32 coefficient)           { m_coefficient = coefficient; }

private:
  f32 m_width;
  f32 m_height;
  f32 m_coefficient;  // 波の高さ係数
  gfl2::math::Vector3 m_position;

};

class NamiController
{
  GFL_FORBID_COPY_AND_ASSIGN(NamiController);

public:

  // 波の区域情報
  enum ZONE
  {
    ZONE_NONE = -1,    // 範囲外
    ZONE_LEFT_END,     // 左境界
    ZONE_LEFT,         // 左平坦部
    ZONE_UP_START,     // 上り開始部
    ZONE_UP,           // 上り部
    ZONE_TOP,          // 頂上(現在値)
    ZONE_DOWN,         // 下り部
    ZONE_RIGHT_END,    // 右境界
    ZONE_MAX
  };

  // 指定位置の波情報
  struct NamiPointInfo
  {
    ZONE m_zone;                  // 波の区域
    f32 m_speed;                  // 波の速度
    gfl2::math::Vector3 m_point;  // 波ポイントの座標
    gfl2::math::Vector3 m_normal; // 傾き
    bool m_isJumpPoint;           // ジャンプポイント

    NamiPointInfo() :
      m_zone(ZONE_NONE),
      m_speed(NAMI_MOVE_SPEED_Z),
      m_point(gfl2::math::Vector3(0.0f, 0.0f, 0.0f)),
      m_normal(gfl2::math::Vector3(0.0f, 1.0f, 0.0f)),
      m_isJumpPoint(false)
    {}
  };

public:

  NamiController();
  ~NamiController();

  void Initialize(MantainSurfAppManager* pAppManager, Course::CourseDataManager* pCourseDataManager, Nami::NamiModelSet* pNamiModelSet, f32 mapHeight, f32 mapWidth, f32 mapStartPosX, f32 mapEndPosX);

  const NamiPointInfo GetNamiPointInfo(const gfl2::math::Vector3 &position);
  const NamiPointInfo GetNamiPointInfo(ZONE zone);
  const NamiPointInfo GetNamiPointInfo(f32 height);

  gfl2::math::Vector3 GetNamiTopMaxPosition();

  void Update();

  f32 GetNamiUpZoneMaxWidth();
  f32 GetNamiHeightRate();

  bool IsWaitFinish() { return m_stateNow == ST_WAIT_FINISH; }
  bool IsFinish()     { return m_stateNow == ST_FINISH; }
  void SetMain();
  void SetFinish();
  f32  GetProgress();

  bool IsGoingUp();
  bool IsGoingDown();
  bool IsWaveEnding();
  bool IsLastWave();

private:

  void UpdateState();
  void UpdateMain();
  void UpdateNamiHeight();

  f32 CalcNamiHeightRate();

private:

  static const f32 NAMI_MOVE_SPEED_X;
  static const f32 NAMI_MOVE_SPEED_Y;
  static const f32 NAMI_MOVE_SPEED_Z;
  static const f32 NAMI_HEIGHT_RATE;
  static const f32 NAMI_HEIGHT_MAX;
  static const f32 NAMI_LEFT_WIDTH;
  static const f32 NAMI_AREA_WIDTH;
  static const f32 NAMI_AREA_HEIGHT;
  static const f32 NAMI_COEFFICIENT_MIN;
  static const f32 NAMI_COEFFICIENT_MAX;

private:

  enum State
  {
    ST_NONE,
    ST_START,       // 開始状態
    ST_MAIN,        // メイン状態(高さ変化)
    ST_WAIT_FINISH, // 終了待ち状態
    ST_FINISH,      // 終了状態
    ST_MAX
  };
  State m_stateNow;

  struct NamiControllData
  {
    f32 m_moveSpeedX;               // 波のX軸移動速度
    f32 m_moveSpeedY;               // 波のY軸移動速度
    f32 m_heightMax;                // 波最高点の高さ
    f32 m_coeffientMin;             // 波の高さ係数の最小値
    f32 m_coeffientMax;             // 波の高さ係数の最大値
    gfl2::math::Vector3 m_moveDir;  // 波の移動方向
  };
  NamiControllData m_NamiControllData;

  struct HeightParam
  {
    u32  m_curFrame;
    u32  m_curKeyFrameIndex;

    HeightParam() :
      m_curFrame(0),
      m_curKeyFrameIndex(0)
    {
    }
  };
  HeightParam m_HeightParam;

  MantainSurfAppManager*     m_pAppManager;
  Course::CourseDataManager* m_pCourseDataManager;
  Nami::NamiModelSet* m_pNamiModelSet;

  f32 m_mapHeight;      // マップの長さ
  f32 m_mapWidht;       // マップの幅
  f32 m_mapStartPosX;   // マップ端の開始位置(波はX軸方向移動するため、X軸の位置を取る)
  f32 m_mapEndPosX;     // マップ端の終了位置(波はX軸方向移動するため、X軸の位置を取る)
  
  f32 m_curNamiHeight;  // 現在の波の高さ

  Course::CourseData::WaveStateInfo m_waveStateInfo;

  bool m_isGoingUp;
  bool m_isGoingDown;

  NamiArea m_NamiArea;  // 波エリア
};

GFL_NAMESPACE_END(Nami)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __NAMI_CONTROLLER_H_INCLUDED__
