//======================================================================
/**
 * @file ModelMantain.h
 * @date 2016/09/12 16:39:59
 * @author fang_yicheng
 * @brief マンタインヤーモデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __MODEL_MANTAIN_H_INCLUDED__
#define __MODEL_MANTAIN_H_INCLUDED__
#pragma once

#include "ModelBaseChara.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)

class ModelMantain : public ModelBaseChara
{
  GFL_FORBID_COPY_AND_ASSIGN(ModelMantain);

private:

  enum
  {
    COLLIDER_MAX = 1,
  };
  static const f32 COLLIDER_RADIUS;

public:

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
    // 海パン用
    ANIME_SEABREAD_RIDE,    // 海パンが乗る用モーション
    ANIME_MAX,
  };

  enum EffectId
  {
    EFT_GLIDE_DROP_WATER,
    EFT_GLIDE,
    EFT_HURDLE_HIT,
    EFT_JET,
    EFT_RUN,
    EFT_RUN_TR,
    EFT_TURN_L,          // 左回転時の水しぶき(1回再生)
    EFT_TURN_R,          // 右回転時の水しぶき(1回再生)
    EFT_TURN_L_LOOP,     // 左回転時の水しぶき(ループ再生)
    EFT_TURN_R_LOOP,     // 右回転時の水しぶき(ループ再生)
    EFT_WAIT,
    EFT_WATER_HIT_L,
    EFT_WATER_HIT_M,
    EFT_WATER_HIT_S,
    EFT_WATER_HIT_AERIAL_MISS,
    EFT_RUN_DASH,         // 加速時の衝撃波(1回再生)
    EFT_RUN_DASH01_LOOP1, // 速度1段階時の衝撃波(ループ再生)
    EFT_RUN_DASH01_LOOP2, // 速度2段階時の衝撃波(ループ再生)
    EFT_RUN_DASH01_LOOP3, // 速度3段階時の衝撃波(ループ再生)
    EFT_RUN_DASH02_LOOP,  // 加速時の衝撃波(ループ再生)
    EFT_SKILL_START,
    EFT_SKILL_END,
    EFT_SKILL_SUCCESS_LV1,
    EFT_SKILL_SUCCESS_LV2,
    EFT_SKILL_SUCCESS_LV3,
    EFT_MAX,
  };

  enum Direction
  {
    DIR_RIGHT = 0,     // 右手方向
    DIR_UP,            // 頭上方向
    DIR_FRONT,         // 手前方向
  };

public:

  ModelMantain();
  virtual ~ModelMantain();

  virtual void Terminate();

  void SetSeaBreadRide(bool isSeaBreadRide) { m_isSeaBreadRide = isSeaBreadRide; }

  gfl2::math::Vector3 GetBodyPosition();

protected:

  virtual poke_3d::model::BaseModel* CreateModel(s32 resId);
  virtual void CreateCollider();

  virtual s32 GetSurfStraightMotionId() { return ANIME_SURF01_STRAIGHT; };
  virtual s32 GetAnimationResId(s32 animeId);
  virtual s32 GetAnimationBlendFrame(s32 animeId);

  virtual s32 GetEffectResId(s32 eftId);
  virtual gfl2::math::Vector3 GetEffectPosition(s32 eftId);
  virtual gfl2::math::Vector3 GetEffectOffset(s32 eftId);
  virtual gfl2::math::Quaternion GetEffectRotation(s32 eftId);

private:

  s32   m_charaId;
  s32   m_bodyJointIndex;

  bool  m_isSeaBreadRide;

  poke_3d::model::CharaModelFactory           m_CharaModelFactory;

};

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __MODEL_MANTAIN_H_INCLUDED__
