//======================================================================
/**
 * @file CameraController.cpp
 * @date 2016/09/13 12:20:41
 * @author fang_yicheng
 * @brief カメラコントローラー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "CameraController.h"
#include "../MantainSurfAppManager.h"
#include "../Player/PlayerController.h"

#include <GameSys/include/GameManager.h>
#include <ui/include/gfl2_UI_Button.h>
#include <math/include/gfl2_math_control.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Camera)

// 開始演出の定義
const f32 CameraController::STARTING_START_LENGTH  = 500.0f;
const gfl2::math::Vector3 CameraController::STARTING_START_ROTATION = gfl2::math::Vector3(-0.2f, 3.3f, 0.0f);
const f32 CameraController::STARTING_DURATION = 3.5f;

// 終了演出の定義
const f32 CameraController::ENDING_DURATION = 2.0f;
const gfl2::math::Vector3 CameraController::ENDING_POSITION = gfl2::math::Vector3(2500.0f, 250.0f, 0.0f);

// ズームの定義
const f32 CameraController::ZOOM_LENGTH_NORMAL = 800.0f;
const f32 CameraController::ZOOM_LENGTH_ACCEL  = 1700.0f;
const f32 CameraController::ZOOM_LENGTH_STEP   = 200.0f;

const f32 CameraController::ZOOM_DURATION_SHORT = 0.4f;
const f32 CameraController::ZOOM_DURATION_LONG  = 0.8f;

const gfl2::math::Vector3 INIT_ROTATION(-0.2f, 2.8f, 0.0f); // カメラの初期ローテーション
const gfl2::math::Vector3 INIT_OFFSET(50.0f, 120.0f, 0.0f); // カメラの初期オフセット

const gfl2::math::Vector3 AERIAL_MAX_ROTATION(0.0f, 2.85f, 0.0f);    // エアリアルカメラの最大ローテーション
const gfl2::math::Vector3 AERIAL_END_ROTATION(-0.4f, 2.8f, 0.0f);   // エアリアルカメラの終了ローテーション
const gfl2::math::Vector3 AERIAL_MAX_OFFSET(50.0f, 20.0f, 0.0f);     // エアリアルカメラの最大オフセット
const gfl2::math::Vector3 AERIAL_END_OFFSET(50.0f, 20.0f, 0.0f);     // エアリアルカメラの終了オフセット
const f32                 AERIAL_CAMERA_RESET_TIME = 1.0f;          // エアリアルカメラのリセット時間

const f32 DELTA_TIME         = 1.0f / 30.0f;      // 1フレームの時間間隔(s)

CameraController::CameraController() :
  m_pCamera(NULL),
  m_pAppManager(NULL),
  m_stateNow(ST_NONE),
  m_stateNext(ST_NONE),
  m_paramZoomLengthNormal(ZOOM_LENGTH_NORMAL),
  m_paramZoomLengthStep(ZOOM_LENGTH_STEP)
{
}

CameraController::~CameraController()
{
}

void CameraController::Initialize(Camera *pCamera, MantainSurfAppManager* pAppManager)
{
  m_pCamera = pCamera;
  m_pAppManager = pAppManager;
}

void CameraController::Terminate()
{
}

void CameraController::ResetCameraPosition()
{
  if(m_pCamera == NULL) return;

  m_pCamera->SetLength(m_paramZoomLengthNormal);
  m_pCamera->SetTargetOffset(INIT_OFFSET);
  m_pCamera->SetRotation(INIT_ROTATION);
  m_ZoomParam.Initialize();
}

void CameraController::SetTraceModeTarget(Model::ModelBase* pTargetModel)
{
  m_pCamera->SetTraceModeTarget(pTargetModel);
}

void CameraController::SetTraceModePoint(const gfl2::math::Vector3 &point)
{
  m_pCamera->SetTraceModePoint(point);
}

void CameraController::StopTracingTarget()
{
  m_pCamera->StopTracingTarget();
}

void CameraController::SetState(State state)
{
  if(m_stateNow != state)
  {
    m_stateNext = state;
  }
}

void CameraController::Update()
{
  // 入力の更新
  UpdateInput();

  // 状態の更新
  UpdateState();

  // エアリアルカメラ演出の更新
  UpdateAerialCamera();

  // カメラの更新
  m_pCamera->Update();
}

void CameraController::UpdateState()
{
  // 状態変化あるかをチェック
  StateCheckChange();

  // 状態変化時の初期化処理
  StateInitialize();

  // 各状態の実行
  StateExecute();
}

// 状態変化あるかをチェック
void CameraController::StateCheckChange()
{
  Player::PlayerController *pPlayerController = m_pAppManager->GetPlayerController();
  switch(m_stateNow)
  {
  case ST_STARTING:
  case ST_NORMAL:
    break;
  }
}

// 状態変化時の初期化処理
void CameraController::StateInitialize()
{
  while(m_stateNext != ST_NONE)
  {
    State prevState = m_stateNow;
    m_stateNow = m_stateNext;
    m_stateNext = ST_NONE;
    
    switch(m_stateNow)
    {
    case ST_STARTING:
      m_StartingParam.m_seq = 0;
      m_StartingParam.m_lengthStart = STARTING_START_LENGTH;
      m_StartingParam.m_lengthEnd = m_paramZoomLengthNormal;
      m_StartingParam.m_rotationStart = STARTING_START_ROTATION;
      m_StartingParam.m_rotationEnd = INIT_ROTATION;
      m_StartingParam.m_timer = 0.0f;
      m_StartingParam.m_duration = STARTING_DURATION;
      m_pCamera->SetLength(m_StartingParam.m_lengthStart);
      m_pCamera->SetRotation(m_StartingParam.m_rotationStart);
      break;
    case ST_NORMAL:
      break;
    case ST_ENDING:
      m_EndingParam.m_posStart = m_pCamera->GetPosition();
      m_EndingParam.m_posEnd = ENDING_POSITION;
      m_EndingParam.m_duration = ENDING_DURATION;
      m_EndingParam.m_timer = 0.0f;
      break;
    }
  }
}

// 各状態の実行
void CameraController::StateExecute()
{
  switch(m_stateNow)
  {
  case ST_STARTING:
    ExcecuteStarting();
    break;
  case ST_NORMAL:
    UpdateCameraPosition();
    ExcecuteZoom();
    break;
  case ST_ENDING:
    ExcecuteEnding();
  }
}

void CameraController::UpdateInput()
{
#if PM_DEBUG
  // 入力情報を初期化
  m_Input.Initialize();

  // 入力情報を更新
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
  gfl2::ui::VectorDevice* pStick = pGameManager->GetUiDeviceManager()->GetStick(0);

  if(pButton != NULL && m_pAppManager->GetDebugMenu()->m_dbvIsHotKeyEnable)
  {
    m_Input.m_isRotationMode = pButton->IsHold(gfl2::ui::BUTTON_R);
    m_Input.m_isReset = pButton->IsHold(gfl2::ui::BUTTON_L);
    m_Input.m_isKeyUp = pButton->IsHold(gfl2::ui::DIR_UP);
    m_Input.m_isKeyDown = pButton->IsHold(gfl2::ui::DIR_DOWN);
    m_Input.m_isKeyLeft = pButton->IsHold(gfl2::ui::DIR_LEFT);
    m_Input.m_isKeyRight = pButton->IsHold(gfl2::ui::DIR_RIGHT);
  }

  if(pStick != NULL)
  {
    m_Input.m_fStickXVal = pStick->GetX();
    m_Input.m_fStickYVal = pStick->GetY();
  }
#endif
}

void CameraController::UpdateCameraPosition()
{
#if PM_DEBUG
  // 回転の操作
  if(m_Input.m_isRotationMode)
  {
    static const f32 STIC_MIN_VAL = 0.1f;
    static const f32 STIC_ROT_SPD = 0.1f;

    f32 rotX = 0;
    f32 rotY = 0;

    if(gfl2::math::Abs(m_Input.m_fStickXVal) > STIC_MIN_VAL)
    {
      rotY = -m_Input.m_fStickXVal;
    }
    if(gfl2::math::Abs(m_Input.m_fStickYVal) > STIC_MIN_VAL)
    {
      rotX = m_Input.m_fStickYVal;
    }
    // カメラ回転値を更新
    gfl2::math::Vector3 rotation = m_pCamera->GetRotation();
    rotation.x += rotX * STIC_ROT_SPD;
    rotation.x = gfl2::math::Clamp(rotation.x, -PI * 0.45f, PI * 0.45f);
    rotation.y += rotY * STIC_ROT_SPD;
    m_pCamera->SetRotation(rotation);
    
    if(m_pAppManager->GetDebugMenu()->m_dbvIsCameraOffsetMode)
    {
      // 注視点の更新
      gfl2::math::Vector3 targetOffset = m_pCamera->GetTargetOffset();
      if(m_Input.m_isKeyUp)
      {
        targetOffset.y += 10;
      }
      if(m_Input.m_isKeyDown)
      {
        targetOffset.y -= 10;
      }
      if(m_Input.m_isKeyLeft)
      {
        targetOffset.x += 10;
      }
      if(m_Input.m_isKeyRight)
      {
        targetOffset.x -= 10;
      }
      m_pCamera->SetTargetOffset(targetOffset);
    }
    else
    {
      // 距離の操作
      f32 length = m_pCamera->GetLength();
      if(m_Input.m_isKeyDown)
      {
        length += 10;
      }
      if(m_Input.m_isKeyUp)
      {
        length -= 10;
        if(length < 10)
        {
          length = 10;
        }
      }
      m_pCamera->SetLength(length);
    }
  }

  // リセット
  if(m_Input.m_isReset)
  {
    ResetCameraPosition();
  }

  // 距離の変更
  if(m_paramZoomLengthNormal != m_pAppManager->GetDebugMenu()->m_dbvCameraNormalLength ||
     m_paramZoomLengthStep != m_pAppManager->GetDebugMenu()->m_dbvCameraZoomStep)
  {
    m_paramZoomLengthNormal = m_pAppManager->GetDebugMenu()->m_dbvCameraNormalLength;
    m_paramZoomLengthStep = m_pAppManager->GetDebugMenu()->m_dbvCameraZoomStep;
    f32 level = m_ZoomParam.m_zoomLevel;
    m_ZoomParam.m_zoomLevel = -1.0f;
    SetZoomLevel(level, 0.0f);
  }
#endif
}

void CameraController::SetZoomLevel(const f32 level, const f32 duration)
{
  GFL_ASSERT(level >= 0.0f);
  if(m_ZoomParam.m_zoomLevel != level)
  {
    m_ZoomParam.m_zoomLevel = level;
    m_ZoomParam.m_startLength = m_pCamera->GetLength();
    m_ZoomParam.m_zoomTimer = 0.0f;
    m_ZoomParam.m_zoomDuration = duration;
    m_ZoomParam.m_isZooming = true;
  }
}

void CameraController::SetZoomLevelNormal(const f32 duration)
{
  SetZoomLevel(0.0f, duration);
}

void CameraController::SetZoomLevelAccel(const f32 duration)
{
  SetZoomLevel((ZOOM_LENGTH_ACCEL - ZOOM_LENGTH_NORMAL) / ZOOM_LENGTH_STEP);
}

void CameraController::ExcecuteStarting()
{
  switch(m_StartingParam.m_seq)
  {
  case 0:
    {
      m_StartingParam.m_timer += DELTA_TIME;
      f32 t = gfl2::math::Clamp(m_StartingParam.m_timer / m_StartingParam.m_duration, 0.0f, 1.0f);
      // カメラ距離を設定
      f32 length = FullSinLerp(m_StartingParam.m_lengthStart, m_StartingParam.m_lengthEnd, t);
      m_pCamera->SetLength(length);
      // カメラ回転を設定
      gfl2::math::Vector3 rotation = FullSinLerp(m_StartingParam.m_rotationStart, m_StartingParam.m_rotationEnd, t);
      m_pCamera->SetRotation(rotation);
      // タイマーをチェック
      if(t >= 1.0f)
      {
        // 完了
        m_StartingParam.m_seq = SEQ_END;
      }
    }
    break;
  }

#if PM_DEBUG
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
  if(m_pAppManager->GetDebugMenu()->m_dbvIsHotKeyEnable && !gfl2::debug::DebugWin_IsOpen() && pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    // 開始演出をスキップ
    m_pCamera->SetLength(m_paramZoomLengthNormal);
    m_pCamera->SetRotation(INIT_ROTATION);
    m_StartingParam.m_seq = SEQ_END;
  }
#endif
}

void CameraController::ExcecuteZoom()
{
  if(!m_ZoomParam.m_isZooming)
  {
    return;
  }

  m_ZoomParam.m_zoomTimer += DELTA_TIME;
  if(m_ZoomParam.m_zoomTimer > m_ZoomParam.m_zoomDuration)
  {
    m_ZoomParam.m_zoomTimer = m_ZoomParam.m_zoomDuration;
  }

  f32 t = m_ZoomParam.m_zoomDuration > 0.0f ? gfl2::math::SinDeg(m_ZoomParam.m_zoomTimer / m_ZoomParam.m_zoomDuration * 90.0f) : 1.0f;
  f32 endLength = m_paramZoomLengthNormal + m_paramZoomLengthStep * m_ZoomParam.m_zoomLevel;
  f32 length = gfl2::math::Lerp(m_ZoomParam.m_startLength, endLength, t);

  if(length == endLength)
  {
    // 目標距離までズーミング完了
    m_ZoomParam.m_isZooming = false;
  }
  m_pCamera->SetLength(length);
}

void CameraController::ExcecuteEnding()
{
  if(m_EndingParam.m_timer == m_EndingParam.m_duration)
  {
    return;
  }

  m_EndingParam.m_timer += DELTA_TIME;
  if(m_EndingParam.m_timer > m_EndingParam.m_duration)
  {
    m_EndingParam.m_timer = m_EndingParam.m_duration;
  }

  f32 t = m_EndingParam.m_timer / m_EndingParam.m_duration;
  f32 sin_t = gfl2::math::SinDeg(t * 90.0f);
  gfl2::math::Vector3 pos = m_EndingParam.m_posStart;
  pos.Lerp(m_EndingParam.m_posEnd, sin_t);
  pos.z = m_pCamera->GetPosition().z;
  m_pCamera->SetPosition(pos);
}

bool CameraController::IsZooming()
{
  return m_ZoomParam.m_isZooming;
}

bool CameraController::IsStartingFinished()
{
  return m_stateNow == ST_STARTING && m_StartingParam.m_seq == SEQ_END;
}

void CameraController::BeginAerialCamera(f32 level, f32 duration)
{
  m_AerialCameraParam.m_isEnable = true;
  m_AerialCameraParam.m_level = level;
  m_AerialCameraParam.m_progress = 0.0f;
  m_AerialCameraParam.m_duration = duration;
  m_AerialCameraParam.m_timer = 0.0f;
  m_AerialCameraParam.m_startOffset = m_pCamera->GetTargetOffset();
  m_AerialCameraParam.m_maxOffset = INIT_OFFSET + (AERIAL_MAX_OFFSET - INIT_OFFSET) * level;
  m_AerialCameraParam.m_endOffset = INIT_OFFSET + (AERIAL_END_OFFSET - INIT_OFFSET) * level;
  m_AerialCameraParam.m_startRotation = m_pCamera->GetRotation();
  m_AerialCameraParam.m_maxRotation = INIT_ROTATION + (AERIAL_MAX_ROTATION - INIT_ROTATION) * level;
  m_AerialCameraParam.m_endRotation = INIT_ROTATION + (AERIAL_END_ROTATION - INIT_ROTATION) * level;
}

void CameraController::SetAerialCameraProgress(f32 progress)
{
  if(m_AerialCameraParam.m_progress < 0.5f && progress >= 0.5f)
  {
    // 半分過ぎたらタイマーをリセット
    m_AerialCameraParam.m_timer = 0.0f;
  }
  m_AerialCameraParam.m_progress = progress;
}

void CameraController::EndAerialCamera()
{
  m_AerialCameraParam.m_isEnable = false;
  m_AerialCameraParam.m_duration = AERIAL_CAMERA_RESET_TIME;
  m_AerialCameraParam.m_timer = 0.0f;
  m_AerialCameraParam.m_startOffset = m_pCamera->GetTargetOffset();
  m_AerialCameraParam.m_endOffset = INIT_OFFSET;
  m_AerialCameraParam.m_startRotation = m_pCamera->GetRotation();
  m_AerialCameraParam.m_endRotation = INIT_ROTATION;
}

void CameraController::UpdateAerialCamera()
{
  if(m_AerialCameraParam.m_isEnable)
  {
    if(m_AerialCameraParam.m_progress < 0.5f)
    {
      // 上昇段階
      m_AerialCameraParam.m_timer += DELTA_TIME;
      if(m_AerialCameraParam.m_timer > m_AerialCameraParam.m_duration)
      {
        m_AerialCameraParam.m_timer = m_AerialCameraParam.m_duration;
      }
      f32 t = gfl2::math::SinDeg(m_AerialCameraParam.m_timer / m_AerialCameraParam.m_duration * 90.0f);
      gfl2::math::Vector3 rotation = m_AerialCameraParam.m_startRotation;
      rotation.Lerp(m_AerialCameraParam.m_maxRotation, t);
      m_pCamera->SetRotation(rotation);
      gfl2::math::Vector3 offset = m_AerialCameraParam.m_startOffset;
      offset.Lerp(m_AerialCameraParam.m_maxOffset, t);
      m_pCamera->SetTargetOffset(offset);
    }
    else if(m_AerialCameraParam.m_progress >= 0.5f)
    {
      // 降下段階
      m_AerialCameraParam.m_timer += DELTA_TIME;
      f32 t = 1.0f - gfl2::math::SinDeg(90.0f + m_AerialCameraParam.m_timer / m_AerialCameraParam.m_duration * 90.0f);
      gfl2::math::Vector3 rotation = m_AerialCameraParam.m_maxRotation;
      rotation.Lerp(m_AerialCameraParam.m_endRotation, t);
      m_pCamera->SetRotation(rotation);
      gfl2::math::Vector3 offset = m_AerialCameraParam.m_maxOffset;
      offset.Lerp(m_AerialCameraParam.m_endOffset, t);
      m_pCamera->SetTargetOffset(offset);
    }
  }
  else if(m_AerialCameraParam.m_timer < m_AerialCameraParam.m_duration)
  {
    m_AerialCameraParam.m_timer += DELTA_TIME;
    if(m_AerialCameraParam.m_timer > m_AerialCameraParam.m_duration)
    {
      m_AerialCameraParam.m_timer = m_AerialCameraParam.m_duration;
    }
    f32 t = gfl2::math::SinDeg(m_AerialCameraParam.m_timer / m_AerialCameraParam.m_duration * 90.0f);
    gfl2::math::Vector3 rotation = m_AerialCameraParam.m_startRotation;
    rotation.Lerp(m_AerialCameraParam.m_endRotation, t);
    m_pCamera->SetRotation(rotation);
    gfl2::math::Vector3 offset = m_AerialCameraParam.m_startOffset;
    offset.Lerp(m_AerialCameraParam.m_endOffset, t);
    m_pCamera->SetTargetOffset(offset);
  }
}

// -90度～90度のSin関数曲線で補間する
f32 CameraController::FullSinLerp(f32 from, f32 to, f32 t)
{
  f32 deg = gfl2::math::Lerp(-90.0f, 90.0f, t);
  f32 sin_t = (gfl2::math::SinDeg(deg) + 1.0f) / 2.0f;
  return gfl2::math::Lerp(from, to, sin_t);
}

// -90度～90度のSin関数曲線で補間する
gfl2::math::Vector3 CameraController::FullSinLerp(const gfl2::math::Vector3 &from, const gfl2::math::Vector3 &to, f32 t)
{
  f32 deg = gfl2::math::Lerp(-90.0f, 90.0f, t);
  f32 sin_t = (gfl2::math::SinDeg(deg) + 1.0f) / 2.0f;
  gfl2::math::Vector3 ret(from);
  ret.Lerp(to, sin_t);
  return ret;
}

GFL_NAMESPACE_END(Camera)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
