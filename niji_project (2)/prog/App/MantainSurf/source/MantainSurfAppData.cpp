//======================================================================
/**
 * @file MantainSurfAppData.cpp
 * @date 2016/11/14 18:35:30
 * @author fang_yicheng
 * @brief マンタインサーフアプリケーションデータを管理するクラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "../include/MantainSurfAppData.h"
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)

const f32 MantainSurfAppData::SKILL_SCORE_RATE[SKILL_SCORE_RANK_MAX] = { 1.0f, 1.2f, 1.5f };

//---------------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//---------------------------------------------------------------------------
MantainSurfAppData::MantainSurfAppData() 
{
}

//---------------------------------------------------------------------------
/**
 * @brief	  デストラクタ
 */
//---------------------------------------------------------------------------
MantainSurfAppData::~MantainSurfAppData()
{
}

//---------------------------------------------------------------------------
/**
 * @brief	  スキルが使用可能か
 * @param   skillId 技ID
 * @return  bool true=使用可能 false=使用不可
 */
//---------------------------------------------------------------------------
bool MantainSurfAppData::IsSkillUnlocked(const SkillId skillId)
{
  switch(skillId)
  {
  case SKILL_ID_NONE:
    return false;
  case SKILL_ID_EX_03:
    return GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork()->CheckEventFlag(SYS_FLAG_SURF_SKILL_01);
  case SKILL_ID_EX_04:
    return GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork()->CheckEventFlag(SYS_FLAG_SURF_SKILL_02);
  case SKILL_ID_EX_06:
    return GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork()->CheckEventFlag(SYS_FLAG_SURF_SKILL_03);
  case SKILL_ID_EX_05:
    return GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork()->CheckEventFlag(SYS_FLAG_SURF_SKILL_04);
  default:
    // 上記以外は常に使用可能
    return true;
  }
}

//---------------------------------------------------------------------------
/**
 * @brief	  スキルの基本得点を取得
 * @param   skillId 技ID
 * @param   counter 発動回数
 * @return  f32 技の基本得点
 */
//---------------------------------------------------------------------------
f32 MantainSurfAppData::GetSkillBaseScore(const SkillId skillId, const u32 counter)
{
  switch(skillId)
  {
  case SKILL_ID_EX_01:        // スクリュードライバー(右)
  case SKILL_ID_EX_02:        // スクリュードライバー(左)
    return 800.0f;
  case SKILL_ID_EX_03:        // スカイハイダイブ
  case SKILL_ID_EX_04:        // リバースアンドリバース
    return 1400.0f;
  case SKILL_ID_EX_05:        // オーバーザレインボー
    return 0.0f;
  case SKILL_ID_EX_06:        // スプラッシュトルネード
    return 2000.0f;
  case SKILL_ID_EX_07:        // がむしゃら
    return counter == 0 ? 3000.0f : 100.0f;
  case SKILL_ID_NORMAL_01:    // 上
    return 250.0f;
  case SKILL_ID_NORMAL_02:    // 下
    return 200.0f;
  case SKILL_ID_NORMAL_03:    // 左
  case SKILL_ID_NORMAL_04:    // 右
    return 150.0f;
  default:
    GFL_ASSERT(0);
    return 0.0f;
  }
}

//---------------------------------------------------------------------------
/**
 * @brief	  スキルの得点率を取得
 * @param   rank 技の得点ランク
 * @return  f32 技の得点率
*/
//---------------------------------------------------------------------------
f32 MantainSurfAppData::GetSkillScoreRate(const u32 rank)
{
  GFL_ASSERT(rank < SKILL_SCORE_RANK_MAX);
  return SKILL_SCORE_RATE[rank];
}

GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
