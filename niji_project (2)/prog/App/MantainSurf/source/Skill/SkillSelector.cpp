//======================================================================
/**
 * @file SkillSelector.cpp
 * @date 2016/11/17 13:50:03
 * @author fang_yicheng
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "SkillSelector.h"
#include "../Model/ModelMantain.h"
#include "../Camera/Camera.h"

using namespace App::MantainSurf::Model;

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Skill)

SkillSelector::SkillSelector()
{
  Reset();
  for(u32 i = 0; i < MantainSurfAppData::SKILL_ID_MAX; i++)
  {
    switch(i)
    {
    case MantainSurfAppData::SKILL_ID_EX_01:      // スクリューハンテール
      m_SkillParamList[i].m_animeId = ModelMantain::ANIME_SKILL_ID_EX_01;
      m_SkillParamList[i].m_seIndex = SurfSound::SurfSoundManager::SE_SKILL_EX_01;
      m_SkillParamList[i].m_safeFrame = 2.0f;
      break;
    case MantainSurfAppData::SKILL_ID_EX_02:      // スクリューサクラビス
      m_SkillParamList[i].m_animeId = ModelMantain::ANIME_SKILL_ID_EX_02;
      m_SkillParamList[i].m_seIndex = SurfSound::SurfSoundManager::SE_SKILL_EX_02;
      m_SkillParamList[i].m_safeFrame = 2.0f;
      break;
    case MantainSurfAppData::SKILL_ID_EX_03:      // ランターン３６０
      m_SkillParamList[i].m_animeId = ModelMantain::ANIME_SKILL_ID_EX_03;
      m_SkillParamList[i].m_seIndex = SurfSound::SurfSoundManager::SE_SKILL_EX_03;
      m_SkillParamList[i].m_safeFrame = 2.0f;
      break;
    case MantainSurfAppData::SKILL_ID_EX_04:      // アシレーヌフリップ
      m_SkillParamList[i].m_animeId = ModelMantain::ANIME_SKILL_ID_EX_04;
      m_SkillParamList[i].m_seIndex = SurfSound::SurfSoundManager::SE_SKILL_EX_04;
      m_SkillParamList[i].m_safeFrame = 2.0f;
      break;
    case MantainSurfAppData::SKILL_ID_EX_05:      // オーバーザギャラドス
      m_SkillParamList[i].m_animeId = ModelMantain::ANIME_SKILL_ID_EX_05;
      m_SkillParamList[i].m_seIndex = SurfSound::SurfSoundManager::SE_SKILL_EX_05;
      m_SkillParamList[i].m_safeFrame = 2.0f;
      break;
    case MantainSurfAppData::SKILL_ID_EX_06:      // スターミートルネード
      m_SkillParamList[i].m_animeId = ModelMantain::ANIME_SKILL_ID_EX_06;
      m_SkillParamList[i].m_seIndex = SurfSound::SurfSoundManager::SE_SKILL_EX_06;
      m_SkillParamList[i].m_safeFrame = 2.0f;
      break;
    case MantainSurfAppData::SKILL_ID_EX_07:      // コイキングスペシャル
      m_SkillParamList[i].m_animeId = ModelMantain::ANIME_SKILL_ID_EX_07;
      m_SkillParamList[i].m_seIndex = SurfSound::SurfSoundManager::SE_SKILL_EX_07;
      m_SkillParamList[i].m_safeFrame = 2.0f;
      break;
    case MantainSurfAppData::SKILL_ID_NORMAL_01:  // 上回転
      m_SkillParamList[i].m_animeId = ModelMantain::ANIME_SKILL_NORMAL_BACKWARD;
      m_SkillParamList[i].m_seIndex = SurfSound::SurfSoundManager::SE_SKILL_NORMAL_02;
      m_SkillParamList[i].m_safeFrame = 2.0f;
      break;
    case MantainSurfAppData::SKILL_ID_NORMAL_02:  // 下回転
      m_SkillParamList[i].m_animeId = ModelMantain::ANIME_SKILL_NORMAL_FORWARD;
      m_SkillParamList[i].m_seIndex = SurfSound::SurfSoundManager::SE_SKILL_NORMAL_02;
      m_SkillParamList[i].m_safeFrame = 2.0f;
      break;
    case MantainSurfAppData::SKILL_ID_NORMAL_03:  // 左回転
      m_SkillParamList[i].m_animeId = ModelMantain::ANIME_SKILL_NORMAL_LEFT;
      m_SkillParamList[i].m_seIndex = SurfSound::SurfSoundManager::SE_SKILL_NORMAL_01;
      m_SkillParamList[i].m_safeFrame = 2.0f;
      break;
    case MantainSurfAppData::SKILL_ID_NORMAL_04:  // 右回転
      m_SkillParamList[i].m_animeId = ModelMantain::ANIME_SKILL_NORMAL_RIGHT;
      m_SkillParamList[i].m_seIndex = SurfSound::SurfSoundManager::SE_SKILL_NORMAL_01;
      m_SkillParamList[i].m_safeFrame = 2.0f;
      break;
    }
  }
}

SkillSelector::~SkillSelector()
{
}

void SkillSelector::Reset()
{
  // コマンドテーブルの初期化
  for(u32 i = 0; i < COMMAND_MAX; i++)
  {
    m_skillCommandTable[i] = COMMAND_NONE;
  }
  m_curCommandIndex = 0;
  m_isKeepingOrder = true;
  m_orderStep = 0;
}

MantainSurfAppData::SkillId SkillSelector::SelectSkill(MantainSurfAppData::SkillId inputSkillId)
{
  // コマンド種別を取得
  SkillCommand command = COMMAND_NONE;
  switch(inputSkillId)
  {
  case MantainSurfAppData::SKILL_ID_NORMAL_01:  // 上方向
    command = COMMAND_U;
    break;
  case MantainSurfAppData::SKILL_ID_NORMAL_02:  // 下方向
    command = COMMAND_D;
    break;
  case MantainSurfAppData::SKILL_ID_NORMAL_03:  // 左方向
    command = COMMAND_L;
    break;
  case MantainSurfAppData::SKILL_ID_NORMAL_04:  // 右方向
    command = COMMAND_R;
    break;
  default:
    // 通常スキルではない
    return inputSkillId;
  }

  // コマンドを格納
  {
    m_skillCommandTable[m_curCommandIndex] = command;
    // 入力順番をチェック
    m_isKeepingOrder &= CheckCommandOrder(command);
    m_curCommandIndex++;
  }

  // 発動する技を決める
  MantainSurfAppData::SkillId outputSkillId = GetSkillIdByCommand(command, m_curCommandIndex);
  if(outputSkillId < MantainSurfAppData::SKILL_ID_NORMAL_01)
  {
    // Ex技発動の場合はリセット
    Reset();
  }
  return outputSkillId;
}

SkillSelector::SkillParam SkillSelector::GetSkillParam(MantainSurfAppData::SkillId skillId, bool isCountUp)
{
  SkillParam param = m_SkillParamList[skillId];
  if(MantainSurfAppData::IsHighScoreSkill(skillId))
  {
    // 大技のエフェクトを設定する
    switch(skillId)
    {
    case MantainSurfAppData::SKILL_ID_EX_01:
    case MantainSurfAppData::SKILL_ID_EX_02:
      param.m_eftIdPlayer = ModelMantain::EFT_SKILL_SUCCESS_LV1;
      param.m_eftIdCamera = Camera::Camera::EFT_SKILL_SUCCESS_LV1;
      break;
    case MantainSurfAppData::SKILL_ID_EX_03:
    case MantainSurfAppData::SKILL_ID_EX_04:
      param.m_eftIdPlayer = ModelMantain::EFT_SKILL_SUCCESS_LV2;
      param.m_eftIdCamera = Camera::Camera::EFT_SKILL_SUCCESS_LV2;
      break;
    case MantainSurfAppData::SKILL_ID_EX_06:
      param.m_eftIdPlayer = ModelMantain::EFT_SKILL_SUCCESS_LV3;
      param.m_eftIdCamera = Camera::Camera::EFT_SKILL_SUCCESS_LV3;
      break;
    case MantainSurfAppData::SKILL_ID_EX_05:
      param.m_eftIdPlayer = ModelMantain::EFT_SKILL_SUCCESS_LV3;
      if(param.m_counter == 0)
      {
        // 初回
        param.m_eftIdCamera = Camera::Camera::EFT_SKILL_SUCCESS_LV3;
      }
      else
      {
        // 2回目以降
        param.m_eftIdCamera = Camera::Camera::EFT_SKILL_SUCCESS_LV1;
      }
      break;
    case MantainSurfAppData::SKILL_ID_EX_07:
      param.m_eftIdPlayer = ModelMantain::EFT_SKILL_SUCCESS_LV1;
      if(param.m_counter == 0)
      {
        // 初回
        param.m_eftIdCamera = Camera::Camera::EFT_SKILL_SUCCESS_LV3;
      }
      else
      {
        // 2回目以降
        param.m_eftIdCamera = Camera::Camera::EFT_SKILL_SUCCESS_LV1;
      }
      break;
    }

    // エフェクトスケールを設定する
    if(skillId == MantainSurfAppData::SKILL_ID_EX_05)
    {
      param.m_eftScaleCamera = 1.0f;
    }
    else
    {
      // その他技
      switch(param.m_rank)
      {
      case 0:   param.m_eftScaleCamera = 0.6f;   break;
      case 1:   param.m_eftScaleCamera = 0.75f;  break;
      case 2:   param.m_eftScaleCamera = 1.0f;   break;
      }
    }
  }

  if(isCountUp)
  {
    // 回数とランクを計上
    CountUp(skillId);
  }
  return param;
}

// 技の発動回数と得点ランクをカウンタアップする
void SkillSelector::CountUp(MantainSurfAppData::SkillId skillId)
{
  // 発動回数をカウンタアップ
  m_SkillParamList[skillId].m_counter++;

  // 得点ランクを更新
  if(!MantainSurfAppData::IsHighScoreSkill(skillId))
  {
    // 大技ではない場合は何もしない
    return;
  }

  for(u32 i = 0; i < MantainSurfAppData::SKILL_ID_MAX; i++)
  {
    SkillParam *pParam = &m_SkillParamList[i];
    if(static_cast<MantainSurfAppData::SkillId>(i) == skillId)
    {
      // 発動した技のランクをリセット
      pParam->m_rank = 0;
    }
    else if(MantainSurfAppData::IsHighScoreSkill(static_cast<MantainSurfAppData::SkillId>(i)))
    {
      // その他大技のランクを上げる
      pParam->m_rank = gfl2::math::Clamp(pParam->m_rank + 1, 0u, MantainSurfAppData::SKILL_SCORE_RANK_MAX - 1u);
    }
  }
}

MantainSurfAppData::SkillId SkillSelector::GetSkillIdByCommand(SkillCommand curCommand, s32 commandCount)
{
  if(commandCount >= 3)
  {
    MantainSurfAppData::SkillId skillIdOut = MantainSurfAppData::SKILL_ID_NONE;
    // 3コマンド技をチェック
    SkillCommand curCommand0 = m_skillCommandTable[commandCount - 3];
    SkillCommand curCommand1 = m_skillCommandTable[commandCount - 2];
    SkillCommand curCommand2 = m_skillCommandTable[commandCount - 1];

    // 右→右→右
    if(curCommand0 == COMMAND_R && curCommand1 == COMMAND_R && curCommand2 == COMMAND_R)
    {
      // スクリュードライバー（右）
      skillIdOut = MantainSurfAppData::SKILL_ID_EX_01;
    }
    else
    // 左→左→左
    if(curCommand0 == COMMAND_L && curCommand1 == COMMAND_L && curCommand2 == COMMAND_L)
    {
      // スクリュードライバー（左）
      skillIdOut = MantainSurfAppData::SKILL_ID_EX_02;
    }
    else
    // 上→下→上
    if(curCommand0 == COMMAND_U && curCommand1 == COMMAND_D && curCommand2 == COMMAND_U)
    {
      // スカイハイダイブ
      skillIdOut = MantainSurfAppData::SKILL_ID_EX_03;
    }
    else
    // 下→下→下
    if(curCommand0 == COMMAND_D && curCommand1 == COMMAND_D && curCommand2 == COMMAND_D)
    {
      // リバースアンドリバース
      skillIdOut = MantainSurfAppData::SKILL_ID_EX_04;
    }

    if(MantainSurfAppData::IsSkillUnlocked(skillIdOut))
    {
      // スキル解放済み
      return skillIdOut;
    }
  }

  if(commandCount == 4)
  {
    MantainSurfAppData::SkillId skillIdOut = MantainSurfAppData::SKILL_ID_NONE;
    // 4コマンド技をチェック
    // 全方向順番に入力
    if(m_isKeepingOrder)
    {
      // オーバーザレインボー
      skillIdOut = MantainSurfAppData::SKILL_ID_EX_05;
    }
    else
    // 右→左→右→上
    if(m_skillCommandTable[0] == COMMAND_R && m_skillCommandTable[1] == COMMAND_L && m_skillCommandTable[2] == COMMAND_R && m_skillCommandTable[3] == COMMAND_U)
    {
      // リバースアンドリバース
      skillIdOut = MantainSurfAppData::SKILL_ID_EX_06;
    }
    else
    // 左→右→左→上
    if(m_skillCommandTable[0] == COMMAND_L && m_skillCommandTable[1] == COMMAND_R && m_skillCommandTable[2] == COMMAND_L && m_skillCommandTable[3] == COMMAND_U)
    {
      // リバースアンドリバース
      skillIdOut = MantainSurfAppData::SKILL_ID_EX_06;
    }
    else
    {
      // 4回転してかつ発動技がないなら、がむしゃら
      skillIdOut = MantainSurfAppData::SKILL_ID_EX_07;
    }
    
    if(MantainSurfAppData::IsSkillUnlocked(skillIdOut))
    {
      // スキル解放済み
      return skillIdOut;
    }
    else
    {
      // スキル未解放の場合はがむしゃら
      return MantainSurfAppData::SKILL_ID_EX_07;
    }
  }

  // 決める技がないなら、現在コマンドによって回転する
  switch(curCommand)
  {
  case COMMAND_U:   // 上
    // 上方向
    return MantainSurfAppData::SKILL_ID_NORMAL_01;
  case COMMAND_D:   // 下
    // 下方向
    return MantainSurfAppData::SKILL_ID_NORMAL_02;
  case COMMAND_L:   // 左
    // 左方向
    return MantainSurfAppData::SKILL_ID_NORMAL_03;
  case COMMAND_R:   // 右
    // 右方向
    return MantainSurfAppData::SKILL_ID_NORMAL_04;
  }

  // ありえない
  GFL_ASSERT(0);
  return MantainSurfAppData::SKILL_ID_NONE;
}

bool SkillSelector::CheckCommandOrder(SkillCommand curCommand)
{
  if(m_curCommandIndex == 0)
  {
    // 一回しか入力していない場合はTRUE
    return true;
  }

  {
    // プラス方向
    s32 commandOld = m_skillCommandTable[m_curCommandIndex - 1];
    if(m_curCommandIndex == 1 || m_orderStep == 1)
    {
      commandOld = (commandOld + 1) % COMMAND_MAX;
      if(commandOld == curCommand)
      {
        m_orderStep = 1;
        return true;
      }
    }
  }

  {
    // マイナス方向
    s32 commandOld = m_skillCommandTable[m_curCommandIndex - 1];
    if(m_curCommandIndex == 1 || m_orderStep == -1)
    {
      commandOld -= 1;
      if(commandOld < 0)
      {
        commandOld = COMMAND_MAX - 1;
      }
      if(commandOld == curCommand)
      {
        m_orderStep = -1;
        return true;
      }
    }
  }
  
  return false;
}

GFL_NAMESPACE_END(Skill)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
