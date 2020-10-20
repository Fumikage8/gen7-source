//======================================================================
/**
 * @file SkyBoxManager.cpp
 * @date 2016/11/16 17:02:33
 * @author fang_yicheng
 * @brief マンタインサーフ天球マネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "SkyBoxManager.h"
#include "../MantainSurfAppManager.h"
#include <GameSys/include/NijiTime.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(SkyBox)

// 初期位置
const f32 SkyBoxManager::DEFAULT_POSITION_Y = 0.0f;
const gfl2::math::Vector3 INIT_POSITION     = gfl2::math::Vector3(0.0f, SkyBoxManager::DEFAULT_POSITION_Y, 0.0f);

// デフォルトY回転値(度)
const f32 SkyBoxManager::DEFAULT_ROTATION_Y = 0.0f;

// 各時間帯の開始時間
const SkyBoxManager::Time SkyBoxManager::TIMEZONE_START_TIME[TIME_ZONE_MAX] = {
  { 17, 55, 0 },    // 夜
  {  6,  0, 0 },    // 昼
  { 16, 45, 0 },    // 昼～夕
  { 16, 55, 0 },    // 夕
  { 17, 50, 0 },    // 夕～夜
};

// 各時間帯のライティング色
const gfl2::math::Vector3 SkyBoxManager::TIMEZONE_LIGHTING_COLOR[TIME_ZONE_MAX] = {
  gfl2::math::Vector3(0.75f, 0.84f, 0.91f),    // 夜
  gfl2::math::Vector3(1.0f, 1.0f, 1.0f),       // 昼
  gfl2::math::Vector3(1.0f, 0.93f, 0.85f),     // 昼～夕
  gfl2::math::Vector3(1.0f, 0.87f, 0.72f),     // 夕
  gfl2::math::Vector3(0.81f, 0.85f, 0.87f),    // 夕～夜
};

// 月の満ち欠けテーブル
const s32 SkyBoxManager::MOON_PATTERN_TABLE[31] =
{
  MOON_PATTERN_NONE_DRAW,         // 1
  MOON_PATTERN_NONE_DRAW,         // 2
  MOON_PATTERN_CRESCENT_BEGIN,    // 3
  MOON_PATTERN_CRESCENT_BEGIN,    // 4
  MOON_PATTERN_CRESCENT_BEGIN,    // 5
  MOON_PATTERN_CRESCENT_BEGIN,    // 6
  MOON_PATTERN_HALF_BEGIN,        // 7
  MOON_PATTERN_HALF_BEGIN,        // 8
  MOON_PATTERN_HALF_BEGIN,        // 9
  MOON_PATTERN_HALF_BEGIN,        // 10
  MOON_PATTERN_3_4_BEGIN,         // 11
  MOON_PATTERN_3_4_BEGIN,         // 12
  MOON_PATTERN_3_4_BEGIN,         // 13
  MOON_PATTERN_FULL,              // 14
  MOON_PATTERN_FULL,              // 15
  MOON_PATTERN_FULL,              // 16
  MOON_PATTERN_FULL,              // 17
  MOON_PATTERN_FULL,              // 18
  MOON_PATTERN_FULL,              // 19
  MOON_PATTERN_3_4_END,           // 20
  MOON_PATTERN_3_4_END,           // 21
  MOON_PATTERN_3_4_END,           // 22
  MOON_PATTERN_HALF_END,          // 23
  MOON_PATTERN_HALF_END,          // 24
  MOON_PATTERN_HALF_END,          // 25
  MOON_PATTERN_HALF_END,          // 26
  MOON_PATTERN_CRESCENT_END,      // 27
  MOON_PATTERN_CRESCENT_END,      // 28
  MOON_PATTERN_CRESCENT_END,      // 29
  MOON_PATTERN_NONE_DRAW,         // 30
  MOON_PATTERN_NONE_DRAW,         // 31
};

SkyBoxManager::SkyBoxManager() :
  m_seq(0),
  m_pSkyBox(NULL)
{
  m_rotationY = DEFAULT_ROTATION_Y;
  m_positionY = DEFAULT_POSITION_Y;
  m_timeZone = TIME_ZONE_NOON;
  m_timeDefault = TIMEZONE_START_TIME[m_timeZone];
  m_dayDefault = 0;
  m_moonPattern = 0;
}

SkyBoxManager::~SkyBoxManager()
{
}

void SkyBoxManager::Initialize(const SetupParam &setupParam)
{
  m_SetupParam = setupParam;
  m_seq = 0;

  // 現在時間取得
  GameSys::AlolaTime time;
  time.Initialize();
  m_timeDefault.m_hour = time.GetHour();
  m_timeDefault.m_minute = time.GetMinute();
  m_timeDefault.m_second = time.GetSecond();
#ifdef GF_CONFIG_DEBUG
  // デバッグ版は昼固定
  m_timeDefault = TIMEZONE_START_TIME[TIME_ZONE_NOON];
#endif
  m_timeZone = GetTimeZone(m_timeDefault);

  // 日を取得
  GameSys::DeviceDate date;
  m_dayDefault = date.GetDay();
  m_moonPattern = GetMoonPattern(m_dayDefault);

  // 天球モデル作成
  m_pSkyBox = GFL_NEW(m_SetupParam.m_pHeap) Model::ModelSkyBox();
  m_pSkyBox->Initialize(m_SetupParam.m_pGfxAllocator, m_SetupParam.m_pHeap, m_SetupParam.m_pResourceManager, NULL);
  m_pSkyBox->StartAnimation();
  m_SetupParam.m_pRenderingPipeLine->AddSkyModel(m_pSkyBox->GetModel());
  UpdateNormal(true);
}

void SkyBoxManager::Terminate()
{
  m_pSkyBox->Terminate();
  GFL_SAFE_DELETE(m_pSkyBox);
}

void SkyBoxManager::Update()
{
  m_pSkyBox->Update();
  UpdateNormal();
}

void SkyBoxManager::UpdateNormal(bool isUpdateFromInitialize)
{
  if(m_SetupParam.m_pAppManager->GetGameState() < MantainSurfAppManager::ST_ENDING)
  {
    m_position.z = m_SetupParam.m_pPlayerModelSet->GetPosition().z;
  }

#if PM_DEBUG
  if(!isUpdateFromInitialize)
  {
    // デバッグ値を適用
    Debug::DebugMenu *pDebugMenu = m_SetupParam.m_pAppManager->GetDebugMenu();
    Time time;
    time.m_hour = pDebugMenu->m_dbvSkyBoxTimeHour;
    time.m_minute = pDebugMenu->m_dbvSkyBoxTimeMinute;
    time.m_second = pDebugMenu->m_dbvSkyBoxTimeSecond;
    m_timeZone = GetTimeZone(time);
    m_moonPattern = GetMoonPattern(pDebugMenu->m_dbvSkyBoxDay);
    m_positionY = pDebugMenu->m_dbvSkyBoxPositionY;
    m_rotationY = pDebugMenu->m_dbvSkyBoxRotationY;
    // Y座標設定
    m_position.y = m_positionY;
    // Y回転設定
    m_pSkyBox->SetRotation(gfl2::math::Vector3(0.0f, gfl2::math::ConvDegToRad(m_rotationY), 0.0f));
  }
#endif

  m_pSkyBox->SetPosition(m_position);
  m_pSkyBox->UpdateTimeZone(m_timeZone);
  m_pSkyBox->UpdateMoon(m_moonPattern);
  m_SetupParam.m_pAppManager->GetNamiModelSet()->UpdateTimeZone(m_timeZone);
  m_SetupParam.m_pAppManager->GetGimmickManager()->UpdateTimeZone(m_timeZone);
  m_SetupParam.m_pAppManager->GetBackgroundManager()->UpdateTimeZone(m_timeZone);
}

bool SkyBoxManager::IsDayTime()
{
  return m_timeZone == TIME_ZONE_NOON;
}

SkyBoxManager::TimeZone SkyBoxManager::GetTimeZone(const Time &time)
{
  s32 timeZone = 0;
  s32 timeSecond = time.ToSecond();
  for(s32 i = 0; i < TIME_ZONE_MAX; i++)
  {
    s32 timeSecond1 = TIMEZONE_START_TIME[i].ToSecond();
    s32 timeSecond2 = TIMEZONE_START_TIME[(i + 1) >= TIME_ZONE_MAX ? 0 : i + 1].ToSecond();
    if(timeSecond2 < timeSecond1)
    {
      if(timeSecond >= timeSecond1 || timeSecond < timeSecond2)
      {
        timeZone = i;
        break;
      }
    }
    else
    {
      if(timeSecond >= timeSecond1 && timeSecond < timeSecond2)
      {
        timeZone = i;
        break;
      }
    }
  }
  return static_cast<SkyBoxManager::TimeZone>(timeZone);
}

s32 SkyBoxManager::GetDayByMoonPattern(s32 moonPatterm)
{
  s32 day = 0;
  for(s32 i = 0; i < 31; i++)
  {
    if(MOON_PATTERN_TABLE[i] == moonPatterm)
    {
      day = i + 1;
      break;
    }
  }
  return day;
}

s32 SkyBoxManager::GetMoonPattern(s32 day)
{
  GFL_ASSERT(day >= 1 && day <= 31);
  return MOON_PATTERN_TABLE[day - 1];
}

const SkyBoxManager::Time& SkyBoxManager::GetTimeDefault()
{
  return m_timeDefault;
}

s32 SkyBoxManager::GetDayDefault()
{
  return m_dayDefault;
}

gfl2::math::Vector3 SkyBoxManager::GetLightingColor(s32 timeZone)
{
  return TIMEZONE_LIGHTING_COLOR[timeZone];
}

gfl2::math::Vector3 SkyBoxManager::GetCurrentLightingColor()
{
  gfl2::math::Vector3 color = TIMEZONE_LIGHTING_COLOR[m_timeZone];
#if PM_DEBUG
  // デバッグ値を適用
  Debug::DebugMenu *pDebugMenu = m_SetupParam.m_pAppManager->GetDebugMenu();
  color.x = pDebugMenu->m_dbvLightingColorR;
  color.y = pDebugMenu->m_dbvLightingColorG;
  color.z = pDebugMenu->m_dbvLightingColorB;
#endif
  return color;
}

GFL_NAMESPACE_END(SkyBox)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
