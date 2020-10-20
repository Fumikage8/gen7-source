//======================================================================
/**
 * @file ModelBaseChara.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief ベースプレイヤーモデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "ModelMantain.h"
#include "../MantainSurfAppManager.h"
#include <model/include/gfl2_CharaModel.h>
#include <niji_conv_header/field/chara/chara_model_id.h>
#include <niji_conv_header/app/mantain_surf/chara/pm0226_00_ms.h>
#include <arc_index/mantain_chara.gaix>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)

const f32 ModelMantain::COLLIDER_RADIUS = 100.0f;

ModelMantain::ModelMantain()
{
  m_SetupParam.m_resourceId         = GARC_mantain_chara_pm0226_00_ms_BIN;
  m_SetupParam.m_modelResourceId    = ARCID_CHARA_MODEL_FIELD_NORMAL;
  m_SetupParam.m_animationListSize  = ANIME_MAX;
  m_SetupParam.m_effectListSize     = EFT_MAX;
  m_SetupParam.m_colliderListSize   = COLLIDER_MAX;
  m_charaId                         = CHARA_MODEL_ID_PM0226_00;
  m_isSeaBreadRide                  = false;
  m_bodyJointIndex = -1;
}

ModelMantain::~ModelMantain()
{
}

void ModelMantain::Terminate()
{
  ModelBaseChara::Terminate();
  m_CharaModelFactory.UnloadModelAll();
  m_CharaModelFactory.Finalize();
}

poke_3d::model::BaseModel* ModelMantain::CreateModel(s32 resId)
{
  gfl2::fs::AsyncFileManager* pFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

  // キャラモデルファクトリを初期化
  m_CharaModelFactory.Initialize(pFileManager, m_pHeap, resId);
  m_CharaModelFactory.LoadModelSync(m_charaId);
  m_CharaModelFactory.SetupModel(m_pGfxAllocator, m_charaId);

  // モデルを作成
  poke_3d::model::BaseModel *pModel = GFL_NEW(m_pHeap) poke_3d::model::CharaModel();
  m_CharaModelFactory.CreateModel(m_pGfxAllocator, m_pHeap, static_cast<poke_3d::model::CharaModel*>(pModel), m_charaId);
  m_bodyJointIndex = pModel->GetJointIndex("Waist");
  return pModel;
}

void ModelMantain::CreateCollider()
{
  AddSphereCollider(COLLIDER_RADIUS);
}

s32 ModelMantain::GetAnimationResId(s32 animeId)
{
  switch(animeId)
  {
  case ANIME_SURF01_STRAIGHT:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS001_SURF01_STRAIGHT_GFBMOT;
  case ANIME_SURF02_LEFT:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS002_SURF02_LEFT_GFBMOT;
  case ANIME_SURF03_RIGHT:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS003_SURF03_RIGHT_GFBMOT;
  case ANIME_SURF04_WAVE_RIGHT:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS026_SURF04_WAVE_RIGHT_GFBMOT;
  case ANIME_AERIAL_START:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS004_AERIAL_START_GFBMOT;
  case ANIME_AERIAL_LOOP:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS005_AERIAL_LOOP_GFBMOT;
  case ANIME_AERIAL_END:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS006_AERIAL_END_GFBMOT;
  case ANIME_OVERTURN_START:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS007_OVERTURN_START_GFBMOT;
  case ANIME_OVERTURN_LOOP:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS008_OVERTURN_LOOP_GFBMOT;
  case ANIME_OVERTURN_END:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS009_OVERTURN_END_GFBMOT;
  case ANIME_PADDLING_LOOP:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS011_PADDLING_LOOP_GFBMOT;
  case ANIME_PADDLING_END:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS012_PADDLING_END_GFBMOT;
  case ANIME_SHAKY:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS013_SHAKY_GFBMOT;
  case ANIME_SKILL_NORMAL_LEFT:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS014_TRICK01_LEFT_SOMERSAULT_GFBMOT;
  case ANIME_SKILL_NORMAL_RIGHT:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS015_TRICK02_RIGHT_SOMERSAULT_GFBMOT;
  case ANIME_SKILL_NORMAL_FORWARD:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS016_TRICK03_FORWARD_SOMERSAULT_GFBMOT;
  case ANIME_SKILL_NORMAL_BACKWARD:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS017_TRICK04_BACKWARD_SOMERSAULT_GFBMOT;
  case ANIME_SKILL_ID_EX_01:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS019_TRICK06_SCREWDRIVER_RIGHT_GFBMOT;
  case ANIME_SKILL_ID_EX_02:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS018_TRICK05_SCREWDRIVER_LEFT_GFBMOT;
  case ANIME_SKILL_ID_EX_03:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS020_TRICK07_SKYHIGHDIVE_GFBMOT;
  case ANIME_SKILL_ID_EX_04:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS021_TRICK08_REVERSE_AND_REVERSE_GFBMOT;
  case ANIME_SKILL_ID_EX_05:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS022_TRICK09_OVER_THE_RAINBOW_GFBMOT;
  case ANIME_SKILL_ID_EX_06:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS023_TRICK10_SPLASH_TORNADO_GFBMOT;
  case ANIME_SKILL_ID_EX_07:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS024_TRICK11_GAMUSHARA_GFBMOT;
  case ANIME_SEABREAD_RIDE:
    return BL_IDX_PM0226_00_MS_PM0226_00_MS025_SEABREAD_RIDE_GFBMOT;
  default:
    return ModelBaseChara::GetAnimationResId(animeId);
  }
}

s32 ModelMantain::GetAnimationBlendFrame(s32 animeId)
{
  switch(animeId)
  {
  case ANIME_SURF01_STRAIGHT:
  case ANIME_SURF02_LEFT:
  case ANIME_SURF03_RIGHT:
  case ANIME_SURF04_WAVE_RIGHT:
  case ANIME_AERIAL_START:
  case ANIME_OVERTURN_START:
  case ANIME_SHAKY:
    return DEFAULT_ANIME_BLEND_FRAME;
  default:
    return ModelBaseChara::GetAnimationBlendFrame(animeId);
  }
}

s32 ModelMantain::GetEffectResId(s32 eftId)
{
  switch(eftId)
  {
  case EFT_GLIDE_DROP_WATER:
    return GARC_mantain_effect_ea_manta_glide_dropwater_lo_lz_BPTCL;
  case EFT_GLIDE:
    return GARC_mantain_effect_ea_manta_glide_lo_lz_BPTCL;
  case EFT_HURDLE_HIT:
    return GARC_mantain_effect_ea_manta_hurdle_hit_lz_BPTCL;
  case EFT_JET:
    return GARC_mantain_effect_ea_manta_jet_lo_lz_BPTCL;
  case EFT_RUN:
    return GARC_mantain_effect_ea_manta_run_lo_lz_BPTCL;
  case EFT_RUN_TR:
    return GARC_mantain_effect_ea_manta_run_fol_lo_lz_BPTCL;
  case EFT_TURN_L:
    return GARC_mantain_effect_ea_manta_turn01_l_lz_BPTCL;
  case EFT_TURN_R:
    return GARC_mantain_effect_ea_manta_turn01_r_lz_BPTCL;
  case EFT_TURN_L_LOOP:
    return GARC_mantain_effect_ea_manta_turn02_l_lo_lz_BPTCL;
  case EFT_TURN_R_LOOP:
    return GARC_mantain_effect_ea_manta_turn02_r_lo_lz_BPTCL;
  case EFT_WAIT:
    return GARC_mantain_effect_ea_manta_wait_lo_lz_BPTCL;
  case EFT_WATER_HIT_L:
  case EFT_WATER_HIT_AERIAL_MISS:
    return GARC_mantain_effect_ea_manta_water_hit_l_lz_BPTCL;
  case EFT_WATER_HIT_M:
    return GARC_mantain_effect_ea_manta_water_hit_m_lz_BPTCL;
  case EFT_WATER_HIT_S:
    return GARC_mantain_effect_ea_manta_water_hit_s_lz_BPTCL;
  case EFT_RUN_DASH:
    return GARC_mantain_effect_ea_manta_run_dash_lz_BPTCL;
  case EFT_RUN_DASH01_LOOP1:
    return GARC_mantain_effect_ea_manta_run_dash01_lo_lz_BPTCL;
  case EFT_RUN_DASH01_LOOP2:
    return GARC_mantain_effect_ea_manta_run_dash01_lo_yl_lz_BPTCL;
  case EFT_RUN_DASH01_LOOP3:
    return GARC_mantain_effect_ea_manta_run_dash01_lo_or_lz_BPTCL;
  case EFT_RUN_DASH02_LOOP:
    return GARC_mantain_effect_ea_manta_run_dash02_lo_lz_BPTCL;
  case EFT_SKILL_START:
    return GARC_mantain_effect_ea_manta_air_success_start_lz_BPTCL;
  case EFT_SKILL_END:
    return GARC_mantain_effect_ea_manta_air_success_end_lz_BPTCL;
  case EFT_SKILL_SUCCESS_LV1:
    return GARC_mantain_effect_ea_manta_air_success_level01_lz_BPTCL;
  case EFT_SKILL_SUCCESS_LV2:
    return GARC_mantain_effect_ea_manta_air_success_level02_lz_BPTCL;
  case EFT_SKILL_SUCCESS_LV3:
    return GARC_mantain_effect_ea_manta_air_success_level03_lz_BPTCL;
  default:
    return ModelBaseChara::GetEffectResId(eftId);
  }
}

gfl2::math::Vector3 ModelMantain::GetEffectPosition(s32 eftId)
{
  switch(eftId)
  {
  case EFT_GLIDE:
    return GetJointPosition("Tail1");
  case EFT_GLIDE_DROP_WATER:
  case EFT_WATER_HIT_L:
  case EFT_WATER_HIT_M:
  case EFT_WATER_HIT_S:
  case EFT_SKILL_START:
  case EFT_SKILL_END:
  case EFT_SKILL_SUCCESS_LV1:
  case EFT_SKILL_SUCCESS_LV2:
  case EFT_SKILL_SUCCESS_LV3:
    return GetJointPosition("Waist");
  case EFT_WATER_HIT_AERIAL_MISS:
    return m_pEffectPosArray[eftId];
  default:
    return ModelBaseChara::GetEffectPosition(eftId);
  }
}

gfl2::math::Vector3 ModelMantain::GetEffectOffset(s32 eftId)
{
  switch(eftId)
  {
  case EFT_RUN:
  case EFT_WAIT:
    return gfl2::math::Vector3(0.0f, 5.0f, 0.0f);
  case EFT_JET:
    return gfl2::math::Vector3(0.0f, 20.0f, 0.0f);
  case EFT_RUN_DASH:
  case EFT_RUN_DASH01_LOOP1:
  case EFT_RUN_DASH01_LOOP2:
  case EFT_RUN_DASH01_LOOP3:
  case EFT_RUN_DASH02_LOOP:
    return gfl2::math::Vector3(0.0f, 50.0f, 35.0f);
  default:
    return ModelBaseChara::GetEffectOffset(eftId);
  }
}

gfl2::math::Quaternion ModelMantain::GetEffectRotation(s32 eftId)
{
  switch(eftId)
  {
  case EFT_JET:
    {
      gfl2::math::Quaternion rot = ModelBaseChara::GetEffectRotation(eftId);
      rot.x = gfl2::math::Clamp(static_cast<s32>(rot.x * 2.0f / PI), -1, 1) * PI;
      return rot;
    }
  case EFT_RUN:
    {
      gfl2::math::Quaternion rot = ModelBaseChara::GetEffectRotation(eftId);
      if(m_isSeaBreadRide)
      {
        // 海パン乗りの場合は向きを180度反転する
        rot.y += PI;
      }
      return rot;
    }
  default:
    return ModelBaseChara::GetEffectRotation(eftId);
  }
}

gfl2::math::Vector3 ModelMantain::GetBodyPosition()
{
  return GetJointPosition(m_bodyJointIndex);
}

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
