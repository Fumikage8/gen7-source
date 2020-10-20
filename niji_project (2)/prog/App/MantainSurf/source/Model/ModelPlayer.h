//======================================================================
/**
 * @file ModelPlayer.h
 * @date 2016/09/12 16:39:59
 * @author fang_yicheng
 * @brief プレイヤーモデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __MODEL_PLAYER_H_INCLUDED__
#define __MODEL_PLAYER_H_INCLUDED__
#pragma once

#include "ModelBaseChara.h"

#include <model/include/gfl2_DressUpModel.h>
#include <model/include/gfl2_DressUpParam.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)

class ModelPlayer : public ModelBaseChara
{
  GFL_FORBID_COPY_AND_ASSIGN(ModelPlayer);

public:

  enum SurfRightMotionBlendFrame
  {
    SURF_RIGHT_MOTION_BLEND_FRAME_SHORT = 6,    // 右移動モーションブレンドフレーム(短)
    SURF_RIGHT_MOTION_BLEND_FRAME_LONG  = 8,    // 右移動モーションブレンドフレーム(長)
  };

  enum AnimeId
  {
    // サーフモーション
    ANIME_SURF01_STRAIGHT = 0,
    ANIME_SURF02_LEFT,
    ANIME_SURF03_RIGHT,
    ANIME_SURF04_WAVE_RIGHT,
    ANIME_AERIAL_START,
    ANIME_AERIAL_LOOP,
    ANIME_AERIAL_END,
    ANIME_OVERTURN_START,
    ANIME_OVERTURN_LOOP,
    ANIME_OVERTURN_END,
    ANIME_PADDLING_LOOP,
    ANIME_PADDLING_END,
    ANIME_SHAKY,
    // 技モーション
    ANIME_SKILL_NORMAL_LEFT,      // 左回転
    ANIME_SKILL_NORMAL_RIGHT,     // 右回転
    ANIME_SKILL_NORMAL_FORWARD,   // 下回転
    ANIME_SKILL_NORMAL_BACKWARD,  // 上回転
    ANIME_SKILL_ID_EX_01,   // スクリュードライバー（右）
    ANIME_SKILL_ID_EX_02,   // スクリュードライバー（左）
    ANIME_SKILL_ID_EX_03,   // スカイハイダイブ
    ANIME_SKILL_ID_EX_04,   // リバースアンドリバース
    ANIME_SKILL_ID_EX_05,   // オーバーザレインボー
    ANIME_SKILL_ID_EX_06,   // スプラッシュトルネード
    ANIME_SKILL_ID_EX_07,   // がむしゃら
    ANIME_MAX,
  };

public:

  ModelPlayer();
  virtual ~ModelPlayer();

  virtual void Terminate();

  void SetupHeroInfo(s32 sex, const poke_3d::model::DressUpParam &dressUpParam);
  void SetSurfRightMotionBlendFrame(SurfRightMotionBlendFrame frame) { m_surfRightMotionBlendFrame = static_cast<s32>(frame); }

protected:

  virtual poke_3d::model::BaseModel* CreateModel(s32 resId);

  virtual s32 GetSurfStraightMotionId() { return ANIME_SURF01_STRAIGHT; };
  virtual s32 GetAnimationResId(s32 animeId);
  virtual s32 GetAnimationBlendFrame(s32 animeId);

protected:

  s32   m_charaId;
  s32   m_sex;

  poke_3d::model::CharaModelFactory           m_CharaModelFactory;
  poke_3d::model::DressUpModelResourceManager m_DressUpModelResourceManager;
  poke_3d::model::DressUpParam                m_DressUpParam;

  s32   m_surfRightMotionBlendFrame;

};

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __MODEL_PLAYER_H_INCLUDED__
