//======================================================================
/**
 * @file SkillSelector.h
 * @date 2016/11/17 13:49:44
 * @author fang_yicheng
 * @brief 技セレクタ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __SKILL_SELECTOR_H_INCLUDED__
#define __SKILL_SELECTOR_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include <System/include/HeapDefine.h>
#include <System/include/Skybox/Skybox.h>
#include <AppLib/include/Util/app_util_heap.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameProcManager.h>

#include <App/MantainSurf/include/MantainSurfAppData.h>
#include "../SurfSound/SurfSoundManager.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Skill)

/**
 * @class SkillSelector
 * @brief 技セレクタ
 */
class SkillSelector
{
  GFL_FORBID_COPY_AND_ASSIGN(SkillSelector);

public:

  // 技のパラメータ
  struct SkillParam
  {
    s32  m_animeId;           // アニメーションID
    s32  m_eftIdPlayer;       // プレイヤーエフェクトID
    s32  m_eftIdCamera;       // カメラエフェクトID
    f32  m_eftScaleCamera;    // カメラエフェクトスケール
    u32  m_counter;           // 発動回数カウンタ
    u32  m_rank;              // 得点ランク
    f32  m_safeFrame;         // 着水成功と見なすアニメーション終了前の安全なフレーム数
    SurfSound::SurfSoundManager::SEIndex  m_seIndex; // SE

    SkillParam() :
      m_animeId(-1),
      m_eftIdPlayer(-1),
      m_eftIdCamera(-1),
      m_eftScaleCamera(1.0f),
      m_counter(0),
      m_rank(0),
      m_safeFrame(0.0f),
      m_seIndex(SurfSound::SurfSoundManager::SE_NONE)
    {
    }
  };

  // 技コマンド
  enum SkillCommand
  {
    COMMAND_NONE  = -1,
    COMMAND_U     = 0,  // 上
    COMMAND_R,          // 右
    COMMAND_D,          // 下
    COMMAND_L,          // 左
    COMMAND_MAX,
  };

public:

  SkillSelector();
  virtual ~SkillSelector();

  void Reset();
  MantainSurfAppData::SkillId SelectSkill(MantainSurfAppData::SkillId inputSkillId);

  SkillParam GetSkillParam(MantainSurfAppData::SkillId skillId, bool isCountUp);

private:

  MantainSurfAppData::SkillId GetSkillIdByCommand(SkillCommand curCommand, s32 commandCount);
  bool CheckCommandOrder(SkillCommand curCommand);
  void CountUp(MantainSurfAppData::SkillId skillId);

private:

  SkillCommand m_skillCommandTable[COMMAND_MAX];
  u32 m_curCommandIndex;

  bool m_isKeepingOrder;
  s32  m_orderStep;

  SkillParam m_SkillParamList[MantainSurfAppData::SKILL_ID_MAX];
};

GFL_NAMESPACE_END(Skill)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __SKILL_SELECTOR_H_INCLUDED__
