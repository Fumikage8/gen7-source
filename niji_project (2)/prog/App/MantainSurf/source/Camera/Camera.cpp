//======================================================================
/**
 * @file Camera.cpp
 * @date 2016/09/13 12:20:07
 * @author fang_yicheng
 * @brief カメラ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include "Camera.h"
#include "../MantainSurfAppManager.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Camera)

const f32 DELTA_TIME = 1.0f / 30.0f;      // 1フレームの時間間隔(s)

const f32 SOUND_AUDIBLE_MAX_DISTANCE = 5000.0f;   // SEなど聞こえる最大距離

const f32 Camera::DEFAULT_FOVY = gfl2::math::ConvDegToRad(40.0f);
const f32 Camera::DEFAULT_NEAR = 30.0f;
const f32 Camera::DEFAULT_FAR  = 41000.0f;

const gfl2::math::Vector3 POSITION_SUN = gfl2::math::Vector3(5427.0f, 15194.0f, 31232.0f);   // 太陽の位置

const f32 EFFECT_DURATION_SUN               = 30.0f;    // 太陽エフェクトの再生時間
const f32 EFFECT_DURATION_SHOOTING_STAR     = 40.0f;    // 流れ星エフェクトの再生時間
const f32 EFFECT_Z_ROTATION_SHOOTING_STAR   = 1.09f;    // 流れ星エフェクトのZ軸回転

Camera::Camera() :
  m_cameraFovy(DEFAULT_FOVY),
  m_cameraNear(DEFAULT_NEAR),
  m_cameraFar(DEFAULT_FAR),
  m_soundAudibleMaxDistance(SOUND_AUDIBLE_MAX_DISTANCE),
  m_targetOffset(gfl2::math::Vector3()),
  m_length(0.0f),
  m_pTargetModel(NULL),
  m_pAppManager(NULL),
  m_pEffectManager(NULL),
  m_traceMode(MODE_POINT),
  m_isTracingTarget(true)
{
  for(s32 i = 0; i < EFT_MAX; i++)
  {
    m_pEffectList[i] = NULL;
    m_EffectPosition[i] = gfl2::math::Vector3::GetZero();
    m_EffectScale[i] = gfl2::math::Vector3::GetZero();
  }
  m_transX = gfl2::math::Vector3(1.0f, 0.0f, 0.0f);
  m_transY = gfl2::math::Vector3(0.0f, 1.0f, 0.0f);
  m_transZ = gfl2::math::Vector3(0.0f, 0.0f, 1.0f);
}
 
Camera::~Camera()
{
}

void Camera::Initialize(MantainSurfAppManager* pAppManager, Effect::EffectManager* pEffectManager)
{
  m_pAppManager = pAppManager;
  m_pEffectManager = pEffectManager;
  InitializeEffect();
}

void Camera::Terminate()
{
  for(s32 i = 0; i < EFT_MAX; i++)
  {
    m_pEffectList[i]->Terminate();
    GFL_SAFE_DELETE(m_pEffectList[i]);
  }
}

void Camera::Update()
{
  UpdateTracing();
  UpdatePosition();
  UpdateEffect();
  UpdateMaxJumpEffect();
  UpdateNearViewPlane();
}

void Camera::SetupTraceParam(const gfl2::math::Vector3 &moveDir, const f32 initSpeed, const f32 targetSpeed, const f32 duration)
{
  m_traceParam.m_moveDir = moveDir;
  m_traceParam.m_moveSpeed = gfl2::math::FAbs(initSpeed);
  m_traceParam.m_initSpeed = gfl2::math::FAbs(initSpeed);
  m_traceParam.m_targetSpeed = gfl2::math::FAbs(targetSpeed);
  m_traceParam.m_duration = duration;
  m_traceParam.m_timer = 0.0f;
}

bool Camera::IsTracingStabled()
{
  return m_traceParam.m_moveSpeed == m_traceParam.m_targetSpeed;
}

void Camera::UpdateTracing()
{
  m_moveVect = m_traceParam.m_moveDir * m_traceParam.m_moveSpeed * DELTA_TIME;
  // タイマー更新
  if(m_traceParam.m_timer < m_traceParam.m_duration)
  {
    m_traceParam.m_timer++;
    f32 t = gfl2::math::Clamp(m_traceParam.m_timer / m_traceParam.m_duration, 0.0f, 1.0f);
    m_traceParam.m_moveSpeed = gfl2::math::Lerp(m_traceParam.m_initSpeed, m_traceParam.m_targetSpeed, t);
  }
}

void Camera::UpdatePosition()
{
  gfl2::math::Vector3 pos = GetPosition();
  if(m_isTracingTarget)
  {
    pos = GetTracingPosition() - m_transZ * m_length;
  }
  pos += m_moveVect;
  SetPosition(pos);
}

gfl2::math::Vector3 Camera::GetPosition()
{
  return m_baseCamera.GetPosition();
}

void Camera::SetPosition(const gfl2::math::Vector3 &position)
{
  m_baseCamera.SetPosition(position);
}

gfl2::math::Vector3 Camera::GetRotation()
{
  return m_baseCamera.GetRotation();
}

void Camera::SetRotation(const gfl2::math::Vector3 &rotation)
{
  m_baseCamera.SetRotation(rotation);
  // ワールド空間方向を更新
  gfl2::math::Quaternion quat = m_baseCamera.GetRotationQuat();
  if(!quat.IsAlmostZero())
  {
    gfl2::math::Matrix34 mat;
    quat.QuaternionToMatrix(mat);
    m_transX = mat.TransformNormal(gfl2::math::Vector3(1.0f, 0.0f, 0.0f)) * -1.0f;
    m_transY = mat.TransformNormal(gfl2::math::Vector3(0.0f, 1.0f, 0.0f));
    m_transZ = mat.TransformNormal(gfl2::math::Vector3(0.0f, 0.0f, 1.0f)) * -1.0f;
  }
}

void Camera::SetEffectPosition(s32 eftId, const gfl2::math::Vector3 &pos)
{
  m_EffectPosition[eftId] = pos;
}

gfl2::math::Vector3 Camera::GetTargetOffset()
{
  return m_targetOffset;
}

void Camera::SetTargetOffset(const gfl2::math::Vector3 &offset)
{
  m_targetOffset = offset;
}

f32 Camera::GetLength()
{
  return m_length;
}

void Camera::SetLength(const f32 length)
{
  m_length = length;
}

void Camera::SetTraceModeTarget(Model::ModelBase* pTargetModel)
{
  m_traceMode = MODE_TARGET;
  m_pTargetModel = pTargetModel;
}

void Camera::SetTraceModePoint(const gfl2::math::Vector3 &point)
{
  m_traceMode = MODE_POINT;
  m_targetPoint = point;
}

void Camera::StopTracingTarget()
{
  m_isTracingTarget = false;
}

gfl2::math::Vector3 Camera::GetTracingPosition()
{
  switch(m_traceMode)
  {
  case MODE_TARGET:
    return m_pTargetModel->GetPosition();
  case MODE_POINT:
    return m_targetPoint;
  default:
    return gfl2::math::Vector3::GetZero();
  }
}

gfl2::math::Vector3 Camera::TransformDirection(gfl2::math::Vector3 localDirection)
{
  return QuatTransformNormal(localDirection);
}

gfl2::math::Vector3 Camera::QuatTransformNormal(const gfl2::math::Vector3 &rhs)
{
  gfl2::math::Quaternion quat = m_baseCamera.GetRotationQuat();
  if(quat.IsAlmostZero())
  {
    return rhs;
  }
  gfl2::math::Matrix34 mat;
  quat.QuaternionToMatrix(mat);
  return mat.TransformNormal(rhs) * -1.0f;
}

void Camera::SetupMatrix(f32 aspectRatio)
{
#if PM_DEBUG
  // デバッグ値を適用
  m_cameraFovy = gfl2::math::ConvDegToRad(m_pAppManager->GetDebugMenu()->m_dbvCameraFovy);
  m_cameraNear = m_pAppManager->GetDebugMenu()->m_dbvCameraNear;
  m_cameraFar = m_pAppManager->GetDebugMenu()->m_dbvCameraFar;
#endif

  // ProjectionMatrix
  m_projectionMatrix = gfl2::math::Matrix44::GetPerspective(
    m_cameraFovy,		          // 視野角
    aspectRatio,	            // アスペクト比
    m_cameraNear,							// 前方投影面までの距離
    m_cameraFar,							// 後方投影面までの距離
    true);                    // 画面の向きに合わせる
}

// -------------------------------------------
// ニアビュー平面を更新
// -------------------------------------------
void Camera::UpdateNearViewPlane()
{
  gfl2::math::Matrix44 invPrjMtx  = m_projectionMatrix.Inverse();
  gfl2::math::Matrix44 invViewMtx = GetViewMatrix().Inverse();
  m_nearViewPlane.vertex_left_up    = ConvScreenPosToWorldPos(0.0f, 0.0f, 0.0f, &invPrjMtx, &invViewMtx);
  m_nearViewPlane.vertex_left_down  = ConvScreenPosToWorldPos(0.0f, 1.0f, 0.0f, &invPrjMtx, &invViewMtx);
  m_nearViewPlane.vertex_right_up   = ConvScreenPosToWorldPos(1.0f, 0.0f, 0.0f, &invPrjMtx, &invViewMtx);
  m_nearViewPlane.vertex_right_down = ConvScreenPosToWorldPos(1.0f, 1.0f, 0.0f, &invPrjMtx, &invViewMtx);
  m_nearViewPlane.center = ConvScreenPosToWorldPos(0.5f, 0.5f, 0.0f, &invPrjMtx, &invViewMtx);
  m_nearViewPlane.right  = (m_nearViewPlane.vertex_right_up - m_nearViewPlane.vertex_left_up).Normalize();
  m_nearViewPlane.up     = (m_nearViewPlane.vertex_left_up - m_nearViewPlane.vertex_left_down).Normalize();
  m_nearViewPlane.normal = -m_nearViewPlane.right.Cross(m_nearViewPlane.up);
  m_nearViewPlane.width  = (m_nearViewPlane.vertex_left_up - m_nearViewPlane.vertex_right_up).Length();
  m_nearViewPlane.height = (m_nearViewPlane.vertex_left_up - m_nearViewPlane.vertex_left_down).Length();
}

// -------------------------------------------
// スクリーン座標とワールド座標変換
// 引数：スクリーン左上 sx = 0.0f, sy = 0.0f
//       スクリーン右下 sx = 1.0f, sy = 1.0f
//       カメラ最近点   fz = 0.0f
//       カメラ最遠点   fz = 1.0f
// -------------------------------------------
gfl2::math::Vector3 Camera::ConvScreenPosToWorldPos(f32 sx, f32 sy, f32 fz, const gfl2::math::Matrix44 *pInvPrjMatrix, const gfl2::math::Matrix44 *pInvViewMatrix)
{
  // スクリーン中心を原点に座標変換
  f32 screen_x = gfl2::math::Lerp(-1.0f, 1.0f, sx);
  f32 screen_y = gfl2::math::Lerp(1.0f, -1.0f, sy);
  f32 z_range = gfl2::math::Lerp(0.0f, -1.0f, fz);
  // -90度回転
  if((screen_x < 0.0f && screen_y > 0.0f) || (screen_x > 0.0f && screen_y < 0.0f))
  {
    // 左上→右上 or 右下→左下
    screen_x = -screen_x;
  }
  else if((screen_x < 0.0f && screen_y < 0.0f) || (screen_x > 0.0f && screen_y > 0.0f))
  {
    // 左下→左上 or 右上→右下
    screen_y = -screen_y;
  }
#ifdef GF_PLATFORM_WIN32
  screen_x = gfl2::math::Lerp(-1.0f, 1.0f, sx);
  screen_y = gfl2::math::Lerp(1.0f, -1.0f, sy);
  z_range = gfl2::math::Lerp(-1.0f, 1.0f, fz);
#endif
  // 逆行列を求める
  gfl2::math::Matrix44 invPrjMtx = (pInvPrjMatrix == NULL ? m_projectionMatrix.Inverse() : *pInvPrjMatrix);
  gfl2::math::Matrix44 invViewMtx = (pInvViewMatrix == NULL ? GetViewMatrix().Inverse() : *pInvViewMatrix);
  // 座標変換
  gfl2::math::Matrix44 convMtx = invViewMtx * invPrjMtx;
  return convMtx.TransformCoord(gfl2::math::Vector3(screen_x, screen_y, z_range));
}

// -------------------------------------------
// ワールド座標とスクリーン座標変換
// 戻り値：スクリーン左上(0.0f, 0.0f)
//         スクリーン右下(1.0f, 1.0f)
// -------------------------------------------
gfl2::math::Vector2 Camera::ConvWorldPosToScreenPos(const gfl2::math::Vector3 &pos)
{
  gfl2::math::Matrix44 viewMtx = GetViewMatrix();
  gfl2::math::Matrix44 convMtx = m_projectionMatrix * viewMtx;
  gfl2::math::Vector3 vec3 = convMtx.TransformCoord(pos);
  gfl2::math::Vector2 vec2 = gfl2::math::Vector2(vec3.x, -vec3.y);
  // スクリーン左上を原点に座標変換
  vec2.x = vec2.x / 2.0f + 0.5f;
  vec2.y = vec2.y / 2.0f + 0.5f;
  return vec2;
}

gfl2::math::Matrix44 Camera::GetProjectionMatrix()
{
  return m_projectionMatrix;
}

gfl2::math::Matrix34 Camera::GetViewMatrix()
{
  // ViewMatrix
  return gfl2::math::Matrix34::GetLookAt(
    GetPosition(),
    GetTracingPosition() + m_targetOffset,
    gfl2::math::Vector3(0.0f, 1.0f, 0.0f));
}

f32 Camera::GetSoundAudibleMaxDistance()
{
  return m_soundAudibleMaxDistance;
}

f32 Camera::GetCameraNear()
{
  return m_cameraNear;
}

f32 Camera::GetCameraFar()
{
  return m_cameraFar;
}

f32 Camera::GetCameraFovy()
{
  return m_cameraFovy;
}

// 点がカメラの左側か右側かを判定する
// @return -1.0:左側 0.0:真ん中 1.0:右側
f32 Camera::JudgeLeftRightSide(const gfl2::math::Vector3 &point)
{
  f32 radius = (GetPosition() - point).Length();
  f32 maxLen = radius * gfl2::math::SinRad(m_cameraFovy);
  // 左側か右側かを判定
  f32 t = maxLen > 0.0f ? (point - GetPosition()).Dot(m_nearViewPlane.right) / maxLen : 0.0f;
  return gfl2::math::Clamp(t, -1.0f, 1.0f);
}

void Camera::InitializeEffect()
{
  for(s32 i = 0; i < EFT_MAX; i++)
  {
    m_pEffectList[i] = m_pEffectManager->CreateEffect(GetEffectResId(i));
  }
}

gfl2::math::Vector3 Camera::GetEffectPosition(s32 eftId)
{
  gfl2::math::Vector3 position = GetPosition();
  gfl2::math::Vector3 offset = GetEffectOffset(eftId);
  switch(eftId)
  {
  case EFT_SHOOTING_STAR:
    position.x = m_EffectPosition[eftId].x + offset.x;
    position.y = m_EffectPosition[eftId].y + offset.y;
    position.z = offset.z + m_pAppManager->GetPlayerModelSet()->GetPosition().z;
    break;
  case EFT_SUNSHINE:
    position = POSITION_SUN;
    position.z += m_pAppManager->GetPlayerModelSet()->GetPosition().z;
    break;
  default:
    position += m_transX * offset.x + m_transY * offset.y + m_transZ * offset.z;
    break;
  }
  return position;
}

gfl2::math::Quaternion Camera::GetEffectRotation(s32 eftId)
{
  switch(eftId)
  {
  case EFT_SHOOTING_STAR:
    return gfl2::math::EulerRotation(0.0f, 0.0f, EFFECT_Z_ROTATION_SHOOTING_STAR, gfl2::math::EulerRotation::RotationOrderXYZ).ToQuaternion();
  default:
    return m_baseCamera.GetRotationQuat();
  }
}

void Camera::StartEffect(EffectId id, const gfl2::math::Vector3 &scale)
{
  if(m_pEffectList[id] != NULL)
  {
    m_pEffectList[id]->Start();
    m_EffectScale[id] = scale;
  }
}

void Camera::EndEffect(EffectId id)
{
  if(m_pEffectList[id] != NULL)
  {
    m_pEffectList[id]->End();
  }
}

void Camera::EndAllEffect()
{
  for(s32 i = 0; i < EFT_MAX; i++)
  {
    if(m_pEffectList[i] != NULL)
    {
      m_pEffectList[i]->End();
    }
  }
}

void Camera::StopEffect()
{
  for(s32 i = 0; i < EFT_MAX; i++)
  {
    if(m_pEffectList[i] != NULL)
    {
      m_pEffectList[i]->Stop();
    }
  }
}

void Camera::ResumeEffect()
{
  for(s32 i = 0; i < EFT_MAX; i++)
  {
    if(m_pEffectList[i] != NULL)
    {
      m_pEffectList[i]->Resume();
    }
  }
}

void Camera::StartMaxJumpEffect(bool isDayTime)
{
  // 最大ジャンプのエフェクトを再生する
  m_MaxJumpParam.Initialize();
  EffectId eftId;
  f32 scale;
  if(isDayTime)
  {
    // 昼の場合
    eftId = EFT_SUNSHINE;
    scale = 50.0f;
    // パラメータ設定
    m_MaxJumpParam.seIndex = SurfSound::SurfSoundManager::SE_SUNSHINE;
    m_MaxJumpParam.duration = EFFECT_DURATION_SUN;
    m_MaxJumpParam.startSEPan = JudgeLeftRightSide(GetEffectPosition(eftId) + GetEffectOffset(eftId));
    m_MaxJumpParam.endSEPan = m_MaxJumpParam.startSEPan - 0.05f;
  }
  else
  {
    // 夜の場合
    eftId = EFT_SHOOTING_STAR;
    scale = 4.0f;
    // パラメータ設定
    m_MaxJumpParam.seIndex = SurfSound::SurfSoundManager::SE_SHOOTING_START;
    m_MaxJumpParam.duration = EFFECT_DURATION_SHOOTING_STAR;
    m_MaxJumpParam.startSEPan = 0.2f;
    m_MaxJumpParam.endSEPan = -0.8f;
  }

  // エフェクト再生
  StartEffect(eftId, gfl2::math::Vector3(1.0f, 1.0f, 1.0f) * scale);
  SetEffectPosition(eftId, GetPosition());

  // SE再生
  SurfSound::SurfSoundManager::PlaySE(m_MaxJumpParam.seIndex);
}

void Camera::UpdateMaxJumpEffect()
{
  if(m_MaxJumpParam.timer >= m_MaxJumpParam.duration)
  {
    return;
  }

  m_MaxJumpParam.timer++;
  f32 t = m_MaxJumpParam.timer / m_MaxJumpParam.duration;

  // SE再生位置を更新
  f32 pan = gfl2::math::Lerp(m_MaxJumpParam.startSEPan, m_MaxJumpParam.endSEPan, t);
  SurfSound::SurfSoundManager::ChangeSEPan(m_MaxJumpParam.seIndex, pan);
}

void Camera::UpdateEffect()
{
  for(s32 i = 0; i < EFT_MAX; i++)
  {
    if(m_pEffectList[i] == NULL)
    {
      continue;
    }
    if(!m_pEffectList[i]->IsAlive())
    {
      continue;
    }
    m_pEffectList[i]->UpdatePosition(GetSrtForEffect(i));
  }
}

gfl2::math::SRT Camera::GetSrtForEffect(s32 id)
{
  gfl2::math::SRT srt;
  // translate
  srt.translate = GetEffectPosition(id);
  // rotation
  GetEffectRotation(id).QuaternionToRadian(&srt.rotation.x, &srt.rotation.y, &srt.rotation.z);
  // scale
  srt.scale = GetEffectScale(id);
  return srt;
}

s32 Camera::GetEffectResId(s32 eftId)
{
  switch(eftId)
  {
  case EFT_SUNSHINE:
    return GARC_mantain_effect_ea_manta_cam_sunshine_lz_BPTCL;
  case EFT_SHOOTING_STAR:
    return GARC_mantain_effect_ea_manta_cam_shootingstar_lz_BPTCL;
  case EFT_WIND_DASH:
    return GARC_mantain_effect_ea_manta_cam_wind_dash_lz_BPTCL;
  case EFT_WIND_DASH_01_LO:
    return GARC_mantain_effect_ea_manta_cam_wind_dash01_lo_lz_BPTCL;
  case EFT_WIND_DASH_02_LO:
    return GARC_mantain_effect_ea_manta_cam_wind_dash02_lo_lz_BPTCL;
  case EFT_WIND_DASH_03_LO:
    return GARC_mantain_effect_ea_manta_cam_wind_dash03_lo_lz_BPTCL;
  case EFT_ZOOM:
    return GARC_mantain_effect_ea_manta_cam_zoom_lz_BPTCL;
  case EFT_SKILL_SUCCESS_LV1:
    return GARC_mantain_effect_ea_manta_cam_air_level01_lz_BPTCL;
  case EFT_SKILL_SUCCESS_LV2:
    return GARC_mantain_effect_ea_manta_cam_air_level02_lz_BPTCL;
  case EFT_SKILL_SUCCESS_LV3:
    return GARC_mantain_effect_ea_manta_cam_air_level03_lz_BPTCL;
  default:
    return -1;
  }
}

gfl2::math::Vector3 Camera::GetEffectOffset(s32 id)
{
  switch(id)
  {
  case EFT_SHOOTING_STAR:
    return gfl2::math::Vector3(3500.0f, 1000.0f, 30000.0f);
  case EFT_SKILL_SUCCESS_LV1:
  case EFT_SKILL_SUCCESS_LV2:
  case EFT_SKILL_SUCCESS_LV3:
    return gfl2::math::Vector3(20.0f, 0.0f, 325.0f);
  default:
    return gfl2::math::Vector3(0.0f, 0.0f, 600.0f);
  }
}

gfl2::math::Vector3 Camera::GetEffectScale(s32 eftId)
{
  return m_EffectScale[eftId];
}

GFL_NAMESPACE_END(Camera)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
