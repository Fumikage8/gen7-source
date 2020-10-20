//======================================================================
/**
 * @file CameraController.h
 * @date 2016/09/13 12:20:58
 * @author fang_yicheng
 * @brief カメラコントローラー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __CAMERACONTROLLER_H_INCLUDED__
#define __CAMERACONTROLLER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "Camera.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
class MantainSurfAppManager;
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Camera)

class CameraController
{
  GFL_FORBID_COPY_AND_ASSIGN(CameraController);

public:

  static const f32 STARTING_START_LENGTH;
  static const gfl2::math::Vector3 STARTING_START_ROTATION;
  static const f32 STARTING_DURATION;

  static const f32 ENDING_DURATION;
  static const gfl2::math::Vector3 ENDING_POSITION;

  static const f32 ZOOM_LENGTH_NORMAL;
  static const f32 ZOOM_LENGTH_ACCEL;
  static const f32 ZOOM_LENGTH_STEP;

  static const f32 ZOOM_DURATION_SHORT;
  static const f32 ZOOM_DURATION_LONG;

  // カメラの状態
  enum State
  {
    ST_NONE = -1,     // なし
    ST_STARTING,      // 開始演出
    ST_NORMAL,        // 通常状態
    ST_ENDING,        // 終了演出
    ST_MAX,
  };

public:

  CameraController();
  virtual ~CameraController();

  void Initialize(Camera *pCamera, MantainSurfAppManager* pAppManager);
  void Terminate();
  void ResetCameraPosition();

  void SetState(State state);

  void SetTraceModeTarget(Model::ModelBase* pTargetModel);
  void SetTraceModePoint(const gfl2::math::Vector3 &point);

  void StopTracingTarget();

  void SetZoomLevel(const f32 level, const f32 duration = ZOOM_DURATION_LONG);
  void SetZoomLevelNormal(const f32 duration = ZOOM_DURATION_LONG);
  void SetZoomLevelAccel(const f32 duration = ZOOM_DURATION_LONG);

  void Update();

  bool IsZooming();
  bool IsStartingFinished();

  void BeginAerialCamera(f32 level, f32 duration);
  void SetAerialCameraProgress(f32 progress);
  void EndAerialCamera();
  
private:

  void UpdateInput();
  void UpdateState();
  void UpdateCameraPosition();
  void UpdateAerialCamera();

  void StateCheckChange();
  void StateInitialize();
  void StateExecute();

  void ExcecuteStarting();
  void ExcecuteZoom();
  void ExcecuteEnding();

  f32  FullSinLerp(f32 from, f32 to, f32 t);
  gfl2::math::Vector3 FullSinLerp(const gfl2::math::Vector3 &from, const gfl2::math::Vector3 &to, f32 t);

private:

  Camera*                 m_pCamera;  // カメラ
  MantainSurfAppManager*  m_pAppManager;

  f32 m_paramZoomLengthNormal;
  f32 m_paramZoomLengthStep;

  State m_stateNow;     // 現在の状態
  State m_stateNext;    // 次の状態

  enum SEQ_STATE
  {
    SEQ_END = -1,
    SEQ_NONE,
  };

  struct Input
  {
    bool m_isRotationMode;    // カメラ回転モードであるか(Rボタン押下しているか)
    bool m_isReset;           // カメラリセットであるか(Lボタン押下しているか)
    bool m_isKeyUp;           // 方向キーの上を押下しているか
    bool m_isKeyDown;         // 方向キーの下を押下しているか
    bool m_isKeyLeft;         // 方向キーの左を押下しているか
    bool m_isKeyRight;        // 方向キーの右を押下しているか
    f32  m_fStickXVal;        // 左スティックX軸の入力量
    f32  m_fStickYVal;        // 左スティックY軸の入力量

    Input()
    {
      Initialize();
    }

    void Initialize()
    {
      m_isRotationMode  = false;
      m_isReset         = false;
      m_isKeyUp         = false;
      m_isKeyDown       = false;
      m_isKeyLeft       = false;
      m_isKeyRight      = false;
      m_fStickXVal      = 0.0f;
      m_fStickYVal      = 0.0f;
    }
  };
  Input m_Input;    // 入力情報

  struct ZoomParam
  {
    bool m_isZooming;
    f32  m_zoomLevel;
    f32  m_startLength;
    f32  m_zoomTimer;
    f32  m_zoomDuration;

    ZoomParam()
    {
      Initialize();
    }

    void Initialize()
    {
      m_zoomLevel = 0.0f;
      m_startLength = 0.0f;
      m_zoomTimer = 0.0f;
      m_zoomDuration = 0.0f;
      m_isZooming = false;
    }
  };
  ZoomParam m_ZoomParam;

  struct StartingParam
  {
    s32 m_seq;
    gfl2::math::Vector3 m_rotationStart;
    gfl2::math::Vector3 m_rotationEnd;
    f32 m_lengthStart;
    f32 m_lengthEnd;
    f32 m_timer;
    f32 m_duration;

    StartingParam()
    {
      m_seq = 0;
      m_rotationStart = gfl2::math::Vector3::GetZero();
      m_rotationEnd = gfl2::math::Vector3::GetZero();
      m_lengthStart = 0.0f;
      m_lengthEnd = 0.0f;
      m_timer = 0.0f;
      m_duration = 0.0f;
    }
  };
  StartingParam m_StartingParam;

  struct EndingParam
  {
    gfl2::math::Vector3 m_posStart;
    gfl2::math::Vector3 m_posEnd;
    f32 m_timer;
    f32 m_duration;

    EndingParam()
    {
      m_posStart = gfl2::math::Vector3::GetZero();
      m_posEnd = gfl2::math::Vector3::GetZero();
      m_timer = 0.0f;
      m_duration = 0.0f;
    }
  };
  EndingParam m_EndingParam;

  struct AerialCameraParam
  {
    bool m_isEnable;      // エアリアルカメラ演出有効しているか
    f32  m_progress;      // エアリアルの進捗率
    f32  m_duration;      // エアリアルカメラ演出の時間
    f32  m_timer;         // 処理用タイマー
    f32  m_level;         // 演出レベル
    gfl2::math::Vector3 m_startOffset;
    gfl2::math::Vector3 m_maxOffset;
    gfl2::math::Vector3 m_endOffset;
    gfl2::math::Vector3 m_startRotation;
    gfl2::math::Vector3 m_maxRotation;
    gfl2::math::Vector3 m_endRotation;

    AerialCameraParam()
    {
      m_isEnable = false;
      m_progress = 0.0f;
      m_duration = 0.0f;
      m_timer = 0.0f;
      m_level = 0.0f;
    }
  };
  AerialCameraParam m_AerialCameraParam;
};

GFL_NAMESPACE_END(Camera)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __CAMERACONTROLLER_H_INCLUDED__
