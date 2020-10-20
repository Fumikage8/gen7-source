//======================================================================
/**
 * @file SkyBoxManager.h
 * @date 2016/11/16 17:02:00
 * @author fang_yicheng
 * @brief マンタインサーフ天球マネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __SKYBOX_MANAGER_H_INCLUDED__
#define __SKYBOX_MANAGER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include <AppLib/include/Util/app_util_heap.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameProcManager.h>

#include "../MantainSurfRenderingPipeline.h"
#include "../Player/PlayerController.h"
#include "../Nami/NamiController.h"
#include "../Resource/ResourceManager.h"
#include "../Model/ModelSkyBox.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
class MantainSurfAppManager;
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(SkyBox)

/**
 * @class SkyBoxManager
 * @brief 天球マネージャー
 */
class SkyBoxManager
{
  GFL_FORBID_COPY_AND_ASSIGN(SkyBoxManager);

public:

  // 時間帯定義
  enum TimeZone
  {
    TIME_ZONE_NIGHT,          // 夜
    TIME_ZONE_NOON,           // 昼
    TIME_ZONE_NOON_EVENING,   // 昼～夕
    TIME_ZONE_EVENING,        // 夕
    TIME_ZONE_EVENING_NIGHT,  // 夕～夜
    TIME_ZONE_MAX,
  };

  ///< 月の満ち欠け種類
  enum MoonPattern
  {
    MOON_PATTERN_NONE_DRAW,         ///< 新月
    MOON_PATTERN_CRESCENT_BEGIN,    ///< 三日月前半
    MOON_PATTERN_HALF_BEGIN,        ///< 半月前半
    MOON_PATTERN_3_4_BEGIN,         ///< 3/4月前半
    MOON_PATTERN_FULL,              ///< 満月
    MOON_PATTERN_3_4_END,           ///< 3/4月後半
    MOON_PATTERN_HALF_END,          ///< 半月後半
    MOON_PATTERN_CRESCENT_END,      ///< 三日月後半
    MOON_PATTERN_MAX
  };

  struct Time
  {
    s32 m_hour;
    s32 m_minute;
    s32 m_second;

    s32 ToSecond() const
    {
      return m_hour * 3600 + m_minute * 60 + m_second;
    }
  };

  static const f32  DEFAULT_ROTATION_Y;
  static const f32  DEFAULT_POSITION_Y;
  static const Time TIMEZONE_START_TIME[TIME_ZONE_MAX];
  static const gfl2::math::Vector3 TIMEZONE_LIGHTING_COLOR[TIME_ZONE_MAX];
  static const s32  MOON_PATTERN_TABLE[];

public:

  struct SetupParam
  {
    gfl2::gfx::IGLAllocator*              m_pGfxAllocator;
    gfl2::heap::HeapBase*                 m_pHeap;
    MantainSurfRenderingPipeline*         m_pRenderingPipeLine;
    MantainSurfAppManager*                m_pAppManager;
    Resource::ResourceManager*            m_pResourceManager;
    Player::PlayerController*             m_pPlayerController;
    Player::PlayerModelSet*               m_pPlayerModelSet;

    SetupParam()
    {
      m_pGfxAllocator = NULL;
      m_pHeap = NULL;
      m_pRenderingPipeLine = NULL;
      m_pAppManager = NULL;
      m_pResourceManager = NULL;
      m_pPlayerController = NULL;
      m_pPlayerModelSet = NULL;
    }
  };

public:

  SkyBoxManager();
  virtual ~SkyBoxManager();

  void Initialize(const SetupParam &setupParam);
  void Terminate();
  void Update();

  bool IsDayTime();

  static TimeZone GetTimeZone(const Time &time);
  static s32 GetDayByMoonPattern(s32 moonPatterm);

  const Time& GetTimeDefault();
  s32   GetDayDefault();
  s32 GetMoonPattern(s32 day);

  gfl2::math::Vector3 GetLightingColor(s32 timeZone);
  gfl2::math::Vector3 GetCurrentLightingColor();

private:

  void UpdateNormal(bool isUpdateFromInitialize = false);

private:

  SetupParam m_SetupParam;
  s32        m_seq;

  Model::ModelSkyBox*  m_pSkyBox;
  gfl2::math::Vector3  m_position;

  // パラメータ定義
  Time        m_timeDefault;
  s32         m_dayDefault;

  TimeZone    m_timeZone;
  s32         m_moonPattern;
  f32         m_rotationY;
  f32         m_positionY;

};

GFL_NAMESPACE_END(SkyBox)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __SKYBOX_MANAGER_H_INCLUDED__
