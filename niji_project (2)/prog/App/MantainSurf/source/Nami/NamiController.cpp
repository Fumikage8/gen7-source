//======================================================================
/**
 * @file NamiController.cpp
 * @date 2016/09/13 12:20:41
 * @author fang_yicheng
 * @brief 波コントローラー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "NamiController.h"
#include "NamiSamplingData.h"

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameProcManager.h>
#include "../MantainSurfAppManager.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Nami)

const f32 NamiController::NAMI_MOVE_SPEED_X         = 20.0f;    // 波のX軸移動速度
const f32 NamiController::NAMI_MOVE_SPEED_Y         = 0.002f;   // 波のY軸移動速度
const f32 NamiController::NAMI_MOVE_SPEED_Z         = 24.0f;    // 波のZ軸移動速度
const f32 NamiController::NAMI_HEIGHT_MAX           = 2000.0f;  // 波最高点の高さ
const f32 NamiController::NAMI_LEFT_WIDTH           = 1500.0f;  // 波左平坦部の寛さ
const f32 NamiController::NAMI_AREA_WIDTH           = 4000.0f;  // 波範囲の寛さ
const f32 NamiController::NAMI_AREA_HEIGHT          = 10000.0f; // 波範囲の奥行き
const f32 NamiController::NAMI_COEFFICIENT_MIN      = 0.05f;     // 波の高さ係数の最小値
const f32 NamiController::NAMI_COEFFICIENT_MAX      = 0.05f;     // 波の高さ係数の最大値

NamiController::NamiController() :
  m_pAppManager(NULL),
  m_pCourseDataManager(NULL),
  m_pNamiModelSet(NULL),
  m_mapHeight(0.0f),
  m_mapWidht(0.0f),
  m_mapStartPosX(0.0f),
  m_mapEndPosX(0.0f),
  m_curNamiHeight(0.0f),
  m_isGoingUp(false),
  m_isGoingDown(false),
  m_stateNow(ST_NONE)
{
}

NamiController::~NamiController()
{
}

void NamiController::Initialize(MantainSurfAppManager* pAppManager, Course::CourseDataManager* pCourseDataManager, Nami::NamiModelSet* pNamiModelSet, f32 mapHeight, f32 mapWidth, f32 mapStartPosX, f32 mapEndPosX)
{
  m_pAppManager = pAppManager;
  m_pCourseDataManager = pCourseDataManager;
  m_pNamiModelSet = pNamiModelSet;
  m_mapHeight = mapHeight;
  m_mapWidht = mapWidth;
  m_mapStartPosX = mapStartPosX;
  m_mapEndPosX = mapEndPosX;

  m_NamiArea.Initialize(NAMI_AREA_WIDTH, NAMI_AREA_HEIGHT);
  m_NamiArea.SetPosition(gfl2::math::Vector3(mapStartPosX, 0.0f, 0.0f));
  m_NamiArea.SetCoefficient(NAMI_COEFFICIENT_MIN);

  m_NamiControllData.m_moveSpeedX = NAMI_MOVE_SPEED_X;
  m_NamiControllData.m_moveSpeedY = NAMI_MOVE_SPEED_Y;
  m_NamiControllData.m_heightMax = NAMI_HEIGHT_MAX;
  m_NamiControllData.m_moveDir = gfl2::math::Vector3(0.0f, 1.0f, 0.0f);
  m_NamiControllData.m_coeffientMin = NAMI_COEFFICIENT_MIN;
  m_NamiControllData.m_coeffientMax = NAMI_COEFFICIENT_MAX;

  m_stateNow = ST_START;
}

void NamiController::Update()
{ 
#if PM_DEBUG
  // 入力情報を更新
  {
    GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
    gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
    if(!m_pAppManager->IsTutorialMode() && m_pAppManager->GetDebugMenu()->m_dbvIsHotKeyEnable && pButton->IsTrigger(gfl2::ui::BUTTON_Y) && !m_pAppManager->GetPlayerController()->IsAerialState())
    {
      m_pAppManager->GetDebugMenu()->m_dbvIsInfinityMode = !m_pAppManager->GetDebugMenu()->m_dbvIsInfinityMode;
      m_pAppManager->GetDebugMenu()->m_dbvIsWaveHeightFix = m_pAppManager->GetDebugMenu()->m_dbvIsInfinityMode;
    }
  }
#endif
  UpdateState();
}

void NamiController::UpdateState()
{
  switch(m_stateNow)
  {
  case ST_MAIN:
    UpdateMain();
    break;
  }

  // 波の高さ更新
  bool isUpdateNamiHeight = true;
#if PM_DEBUG
  isUpdateNamiHeight = !m_pAppManager->GetDebugMenu()->m_dbvIsWaveHeightFix;
#endif
  if(isUpdateNamiHeight)
  {
    m_curNamiHeight = m_waveStateInfo.m_waveHeight;
  }
  m_pNamiModelSet->SetHeightAnimationFrame(m_curNamiHeight);
}

void NamiController::UpdateMain()
{
  if(!m_pAppManager->IsTutorialMode())
  {
    UpdateNamiHeight();
  }
}

void NamiController::UpdateNamiHeight()
{
  Course::CourseData* pCourseData = m_pCourseDataManager->GetCourseData();
  bool isFrameStop = false;
  if(m_pAppManager->GetPlayerController()->IsAerialState() || !m_pAppManager->GetGimmickManager()->IsPlayerArrived())
  {
    if(!IsWaveEnding())
    {
      isFrameStop = true;
    }
  }

  m_isGoingUp = false;
  m_isGoingDown = false;
  if(!isFrameStop)
  {
    m_waveStateInfo = pCourseData->GetWaveStateInfo(m_HeightParam.m_curKeyFrameIndex, m_HeightParam.m_curFrame);
    // 波変化をチェック
    if(m_waveStateInfo.m_waveState == Course::CourseData::WAVE_STATE_0_1 && m_waveStateInfo.m_stateProgress == 0.0f)
    {
      // 波上り瞬間
      m_isGoingUp = true;
    }
    if(m_waveStateInfo.m_waveState == Course::CourseData::WAVE_STATE_1_0 && m_waveStateInfo.m_stateProgress == 0.0f)
    {
      // 波下り瞬間
      m_isGoingDown = true;
    }
    // フレームを進む
    m_HeightParam.m_curFrame++;
  }

  // indexを更新
  u32 nextKeyFrame = pCourseData->GetKeyFrame(m_HeightParam.m_curKeyFrameIndex + 1);
  if(m_HeightParam.m_curFrame == nextKeyFrame)
  {
    m_HeightParam.m_curKeyFrameIndex++;
    if(m_HeightParam.m_curKeyFrameIndex == pCourseData->GetKeyFrameCount() - 1)
    {
      // 最後まで進んだ
      // 終了待ち状態へ
      m_stateNow = ST_WAIT_FINISH;
    }
  }
}

f32 NamiController::GetProgress()
{
  return static_cast<f32>(m_HeightParam.m_curFrame) / static_cast<f32>(m_pCourseDataManager->GetCourseData()->GetTotalFrame());
}

const NamiController::NamiPointInfo NamiController::GetNamiPointInfo(const gfl2::math::Vector3 &position)
{
  NamiPointInfo info;
  gfl2::math::Vector3 namiPosition = m_NamiArea.GetPosition();
  if(position.x > namiPosition.x + NAMI_LEFT_WIDTH || position.x < namiPosition.x - m_NamiArea.GetWidth())
  {
    // 波エリアの範囲外
    info.m_point = position;
    info.m_point.y = 0.0f;
    return info;
  }

  f32 diffX = namiPosition.x - position.x;    // 波原点への距離のX軸値

  // サンプリングデータの行列を算出
  f32 unitLenghtX = m_NamiArea.GetWidth() / NamiSamplingData::m_colSize;

  // X軸
  f32 x = diffX / unitLenghtX;

  // Y軸
  f32 y = x > 0.0f ? m_NamiArea.GetCoefficient() * x * x : 0.0f;

  info.m_point.x = position.x;
  info.m_point.y = y * CalcNamiHeightRate();
  info.m_point.z = position.z;
  
  // 傾きの法線を求める
  info.m_normal = NamiSamplingData::CalcNormal(x, y, m_NamiArea.GetCoefficient());
  
  // ZONE情報更新
  gfl2::math::Vector3 topPosition = GetNamiPointInfo(ZONE_TOP).m_point;
  if(position.x == namiPosition.x + NAMI_LEFT_WIDTH)
  {
    info.m_zone = ZONE_LEFT_END;
  }
  else if(position.x < namiPosition.x + NAMI_LEFT_WIDTH && position.x > namiPosition.x)
  {
    info.m_zone = ZONE_LEFT;
  }
  else if(position.x == namiPosition.x)
  {
    info.m_zone = ZONE_UP_START;
  }
  else if(position.x < namiPosition.x && position.x > topPosition.x)
  {
    info.m_zone = ZONE_UP;
  }
  else if(position.x <= topPosition.x)
  {
    info.m_point.x = topPosition.x;
    info.m_point.y = topPosition.y;
    info.m_zone = ZONE_TOP;
  }
  else if(x == NamiSamplingData::m_colSize)
  {
    info.m_zone = ZONE_RIGHT_END;
  }
  else
  {
    info.m_zone = ZONE_DOWN;
  }

  return info;
}

const NamiController::NamiPointInfo NamiController::GetNamiPointInfo(ZONE zone)
{
  NamiPointInfo info;
  f32 x = 0.0f;
  f32 y = 0.0f;
  f32 width = 0.0f;
  gfl2::math::Vector3 namiPosition = m_NamiArea.GetPosition();
  switch(zone)
  {
  case ZONE_LEFT_END:
  case ZONE_LEFT:
    info.m_point.x = namiPosition.x + NAMI_LEFT_WIDTH;
    info.m_point.y = 0.0f;
    info.m_zone = ZONE_LEFT_END;
    break;

  case ZONE_UP_START:
  case ZONE_UP:
    info.m_point.x = namiPosition.x;
    info.m_point.y = 0.0f;
    info.m_zone = ZONE_UP_START;
    break;

  case ZONE_TOP:
    width = m_NamiArea.GetWidth() / 2.0f - gfl2::math::FAbs(m_pNamiModelSet->GetTopPosition().x);
    x = width / (m_NamiArea.GetWidth() / NamiSamplingData::m_colSize);
    y = m_NamiArea.GetCoefficient() * x * x;
    info.m_point.x = namiPosition.x - width;
    info.m_point.y = m_pNamiModelSet->GetTopPosition().y;
    info.m_zone = ZONE_TOP;
    info.m_isJumpPoint = true;
    /*
    x = NamiSamplingData::m_colSize / 2.0f;
    y = NamiSamplingData::GetDataMax(m_NamiArea.GetCoefficient());
    info.m_point.x = m_NamiArea.GetPosition().x - m_NamiArea.GetWidth() / 2.0f;
    info.m_point.y = y * CalcNamiHeightRate();
    info.m_zone = ZONE_TOP;
    info.m_isJumpPoint = true;
    */
    break;

  case ZONE_DOWN:
  case ZONE_RIGHT_END:
    info.m_point.x = namiPosition.x  - m_NamiArea.GetWidth();
    info.m_point.y = 0.0f;
    info.m_zone = ZONE_RIGHT_END;
    break;
  }

  // 傾きの法線を求める(Z軸の傾きは考慮しない)
  info.m_normal = NamiSamplingData::CalcNormal(x, y, m_NamiArea.GetCoefficient());

  return info;
}

const NamiController::NamiPointInfo NamiController::GetNamiPointInfo(f32 height)
{
  // 高さによって、波の情報を取得
  if(height <= 0.0f)
  {
    return GetNamiPointInfo(ZONE_UP_START);
  }
  if(height >= m_pNamiModelSet->GetTopPosition().y)
  {
    return GetNamiPointInfo(ZONE_TOP);
  }

  f32 col = NamiSamplingData::m_colSize / 2.0f;
  f32 y = height / CalcNamiHeightRate();
  f32 x = gfl2::math::FSqrt(y / m_NamiArea.GetCoefficient());

  NamiPointInfo info;
  info.m_zone = ZONE_UP;
  info.m_point.x = m_NamiArea.GetPosition().x - x * m_NamiArea.GetWidth() / NamiSamplingData::m_colSize;
  info.m_point.y = height;
  info.m_normal = NamiSamplingData::CalcNormal(x, y, m_NamiArea.GetCoefficient());
  return info;
}

gfl2::math::Vector3 NamiController::GetNamiTopMaxPosition()
{
  gfl2::math::Vector3 position = gfl2::math::Vector3::GetZero();
  position.x = m_NamiArea.GetPosition().x - m_NamiArea.GetWidth() / 2.0f;
  position.y = NAMI_HEIGHT_MAX;
  return position;
}

f32 NamiController::GetNamiHeightRate()
{
  return m_curNamiHeight;
}

f32 NamiController::CalcNamiHeightRate()
{
  return m_NamiControllData.m_heightMax / NamiSamplingData::GetDataMax(m_NamiControllData.m_coeffientMax);
}

f32 NamiController::GetNamiUpZoneMaxWidth()
{
  return m_NamiArea.GetWidth() / 2.0f;
}

void NamiController::SetFinish()
{
  m_stateNow = ST_FINISH;
}

void NamiController::SetMain()
{
  m_stateNow = ST_MAIN;
  m_HeightParam.m_curFrame = 0;
  m_HeightParam.m_curKeyFrameIndex = 0;
}

// 波が上り始める瞬間か
bool NamiController::IsGoingUp()
{
  return m_isGoingUp;
}

// 波が下り始める瞬間か
bool NamiController::IsGoingDown()
{
  return m_isGoingDown;
}

bool NamiController::IsWaveEnding()
{
  return m_HeightParam.m_curKeyFrameIndex >= m_pAppManager->GetCourseDataManager()->GetCourseData()->GetWaveEndKeyFrameIndex();
}

bool NamiController::IsLastWave()
{
  return m_HeightParam.m_curKeyFrameIndex >= m_pAppManager->GetCourseDataManager()->GetCourseData()->GetLastWaveKeyFrameIndex();
}

GFL_NAMESPACE_END(Nami)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
