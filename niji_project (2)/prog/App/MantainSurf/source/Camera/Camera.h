//======================================================================
/**
 * @file Camera.h
 * @date 2016/09/13 12:20:24
 * @author fang_yicheng
 * @brief カメラ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __CAMERA_H_INCLUDED__
#define __CAMERA_H_INCLUDED__
#pragma once

#include "../Model/ModelBase.h"
#include "../Effect/EffectBase.h"
#include "../Effect/EffectManager.h"
#include "../SurfSound/SurfSoundManager.h"

#include <model/include/gfl2_BaseCamera.h>

// 前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
class MantainSurfAppManager;
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Camera)

class Camera// : public poke_3d::model::BaseCamera
{
  GFL_FORBID_COPY_AND_ASSIGN(Camera);

public:

  static const f32 DEFAULT_FOVY;
  static const f32 DEFAULT_NEAR;
  static const f32 DEFAULT_FAR;

  enum TraceMode
  {
    MODE_TARGET,      // ターゲットトレーシングモード
    MODE_POINT,       // 移動するポイントをトレーシング
  };

  enum EffectId
  {
    EFT_SUNSHINE,
    EFT_SHOOTING_STAR,
    EFT_WIND_DASH,
    EFT_WIND_DASH_01_LO,
    EFT_WIND_DASH_02_LO,
    EFT_WIND_DASH_03_LO,
    EFT_ZOOM,
    EFT_SKILL_SUCCESS_LV1,
    EFT_SKILL_SUCCESS_LV2,
    EFT_SKILL_SUCCESS_LV3,
    EFT_MAX,
  };

  // ビュー平面の構造体
  struct ViewPlane
  {
    gfl2::math::Vector3 vertex_left_up;     // 左上頂点座標
    gfl2::math::Vector3 vertex_left_down;   // 左下頂点座標
    gfl2::math::Vector3 vertex_right_up;    // 右上頂点座標
    gfl2::math::Vector3 vertex_right_down;  // 右下頂点座標
    gfl2::math::Vector3 center;             // 平面の中心座標
    gfl2::math::Vector3 normal;             // 平面の法線
    gfl2::math::Vector3 right;              // 平面の右方向法線
    gfl2::math::Vector3 up;                 // 平面の上方向法線
    f32                 width;              // 平面の寛さ
    f32                 height;             // 平面の高さ
  };

public:
  Camera();
  virtual ~Camera();

  void Initialize(MantainSurfAppManager* pAppManager, Effect::EffectManager* m_pEffectManager);
  void Terminate();

  void Update();

  poke_3d::model::BaseCamera* GetBaseCamera() { return &m_baseCamera; };
  void SetupMatrix(f32 aspectRatio);

  gfl2::math::Vector3 GetPosition();
  void SetPosition(const gfl2::math::Vector3 &position);
  
  gfl2::math::Vector3 GetRotation();
  void SetRotation(const gfl2::math::Vector3 &rotation);

  gfl2::math::Vector3 GetTargetOffset();
  void SetTargetOffset(const gfl2::math::Vector3 &offset);

  f32 GetLength();
  void SetLength(const f32 length);

  void SetTraceModeTarget(Model::ModelBase* pTargetModel);
  void SetTraceModePoint(const gfl2::math::Vector3 &point);
  void StopTracingTarget();

  void SetupTraceParam(const gfl2::math::Vector3 &moveDir, const f32 initSpeed, const f32 targetSpeed, const f32 duration);
  bool IsTracingStabled();

  gfl2::math::Vector3 GetTracingPosition();
  gfl2::math::Vector3 TransformDirection(gfl2::math::Vector3 localDirection);
    
  void StartEffect(EffectId id, const gfl2::math::Vector3 &scale = gfl2::math::Vector3(1.0f, 1.0f, 1.0f));
  void EndEffect(EffectId id);
  void EndAllEffect();
  void StopEffect();
  void ResumeEffect();

  void StartMaxJumpEffect(bool isDayTime);

  gfl2::math::Matrix44 GetProjectionMatrix();
  gfl2::math::Matrix34 GetViewMatrix();

  f32 GetSoundAudibleMaxDistance();

  f32 GetCameraNear();
  f32 GetCameraFar();
  f32 GetCameraFovy();

  gfl2::math::Vector3 ConvScreenPosToWorldPos(f32 sx, f32 sy, f32 fz, const gfl2::math::Matrix44 *pInvPrjMatrix = NULL, const gfl2::math::Matrix44 *pInvViewMatrix = NULL);
  gfl2::math::Vector2 ConvWorldPosToScreenPos(const gfl2::math::Vector3 &pos);
  
  ViewPlane GetNearViewPlane() { return m_nearViewPlane; }
  
  f32 JudgeLeftRightSide(const gfl2::math::Vector3 &point);

private:

  void InitializeEffect();

  void UpdateTracing();
  void UpdatePosition();
  void UpdateEffect();
  void UpdateMaxJumpEffect();
  void UpdateNearViewPlane();

  gfl2::math::Vector3 QuatTransformNormal(const gfl2::math::Vector3 &rhs);

  gfl2::math::SRT        GetSrtForEffect(s32 eftId);
  s32                    GetEffectResId(s32 eftId);
  gfl2::math::Vector3    GetEffectPosition(s32 eftId);
  gfl2::math::Vector3    GetEffectOffset(s32 eftId);
  gfl2::math::Quaternion GetEffectRotation(s32 eftId);
  gfl2::math::Vector3    GetEffectScale(s32 eftId);
  void                   SetEffectPosition(s32 eftId, const gfl2::math::Vector3 &pos);
  
private:

  poke_3d::model::BaseCamera m_baseCamera;

  f32 m_soundAudibleMaxDistance;

  f32 m_cameraFovy;
  f32 m_cameraNear;
  f32 m_cameraFar;

  gfl2::math::Vector3 m_transY;
  gfl2::math::Vector3 m_transX;
  gfl2::math::Vector3 m_transZ;

  gfl2::math::Vector3 m_targetOffset;   // カメラの注視位置の差分
  f32                 m_length;         // ターゲットモデルとの距離

  MantainSurfAppManager*  m_pAppManager;

  Effect::EffectManager*  m_pEffectManager;
  Effect::EffectBase*     m_pEffectList[EFT_MAX];
  gfl2::math::Vector3     m_EffectPosition[EFT_MAX];
  gfl2::math::Vector3     m_EffectScale[EFT_MAX];

  gfl2::math::Matrix44  m_projectionMatrix;

private:

  TraceMode m_traceMode;
  bool      m_isTracingTarget;

  // ニア平面
  ViewPlane m_nearViewPlane;

  // トレーシング対象
  Model::ModelBase*   m_pTargetModel;   // カメラが追従するモデル
  gfl2::math::Vector3 m_targetPoint;
  gfl2::math::Vector3 m_moveVect;

  // トレーシングパラメータ
  struct TraceParam
  {
    gfl2::math::Vector3 m_moveDir;      // 移動方向
    f32                 m_moveSpeed;    // 移動スピード(絶対値を取る)
    f32                 m_initSpeed;    // 移動の初期速度(絶対値を取る)
    f32                 m_targetSpeed;  // 移動の目標速度(絶対値を取る)
    f32                 m_duration;     // 移動時間
    f32                 m_timer;        // 処理タイマー

    TraceParam() :
      m_moveDir(gfl2::math::Vector3::GetZero()),
      m_moveSpeed(0.0f),
      m_initSpeed(0.0f),
      m_targetSpeed(0.0f),
      m_duration(0.0f),
      m_timer(0.0f)
    {
    }
  };
  TraceParam m_traceParam;

  // マックスジャンプパラメータ
  struct MaxJumpParam
  {
    SurfSound::SurfSoundManager::SEIndex seIndex;   // SE
    f32 timer;        // タイマー
    f32 duration;     // 継続時間
    f32 startSEPan;   // 開始時のSEパン
    f32 endSEPan;     // 終了時のSEパン

    MaxJumpParam()
    {
      Initialize();
    }

    void Initialize()
    {
      seIndex = SurfSound::SurfSoundManager::SE_NONE;
      timer = 0.0f;
      duration = 0.0f;
      startSEPan = 0.0f;
      endSEPan = 0.0f;
    }
  };
  MaxJumpParam m_MaxJumpParam;

};

GFL_NAMESPACE_END(Camera)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __CAMERA_H_INCLUDED__
