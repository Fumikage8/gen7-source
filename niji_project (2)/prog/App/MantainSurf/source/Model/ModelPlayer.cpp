//======================================================================
/**
 * @file ModelPlayer.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief プレイヤーモデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "ModelPlayer.h"
#include <niji_conv_header/field/chara/chara_model_id.h>
#include <niji_conv_header/app/mantain_surf/chara/p1_base_ms.h>
#include <niji_conv_header/app/mantain_surf/chara/p2_base_ms.h>
#include <arc_index/mantain_chara.gaix>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)

ModelPlayer::ModelPlayer()
{
  m_SetupParam.m_isDressUpModel    = true;
  m_SetupParam.m_modelResourceId   = ARCID_CHARA_MODEL_FIELD_NORMAL;
  m_SetupParam.m_animationListSize = ANIME_MAX;
  m_charaId = 0;
  m_sex = 0;
  m_surfRightMotionBlendFrame = SURF_RIGHT_MOTION_BLEND_FRAME_SHORT;
}

ModelPlayer::~ModelPlayer()
{
}

void ModelPlayer::Terminate()
{
  ModelBaseChara::Terminate();

  m_DressUpModelResourceManager.UnloadDressUpPartsAll();
  m_CharaModelFactory.UnloadModelAll();

  m_DressUpModelResourceManager.Finalize();
  m_CharaModelFactory.Finalize();
}

void ModelPlayer::SetupHeroInfo(s32 sex, const poke_3d::model::DressUpParam &dressUpParam)
{
  m_sex = sex;
  m_DressUpParam = dressUpParam;
  if(sex == PM_MALE)
  {
    m_SetupParam.m_resourceId = GARC_mantain_chara_p1_base_ms_BIN;
    m_charaId = CHARA_MODEL_ID_PC0001_00;
  }
  else if(sex == PM_FEMALE)
  {
    m_SetupParam.m_resourceId = GARC_mantain_chara_p2_base_ms_BIN;
    m_charaId = CHARA_MODEL_ID_PC0002_00;
  }
  else
  {
    GFL_ASSERT(0);
  }
}

poke_3d::model::BaseModel* ModelPlayer::CreateModel(s32 resId)
{
  gfl2::fs::AsyncFileManager* pFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

  // キャラモデルファクトリを初期化
  m_CharaModelFactory.Initialize(pFileManager, m_pHeap, resId);
  m_CharaModelFactory.LoadModelSync(m_charaId);
  m_CharaModelFactory.SetupModel(m_pGfxAllocator, m_charaId);

  // 着せ替えリソースマネージャーを初期化
  System::DressUp::OverwrideDressUpParamRide(m_sex, false, &m_DressUpParam);
  m_DressUpModelResourceManager.Initialize(pFileManager, m_pHeap, System::DressUp::GetDressUpArcIdListField());
  m_DressUpModelResourceManager.LoadDressUpPartsSync(m_DressUpParam);
  m_DressUpModelResourceManager.SetupDressUpParts(m_pGfxAllocator, m_DressUpParam);

  // プレイヤーモデルを作成
  poke_3d::model::BaseModel *pModel;
#if 1
  {
    // 着せ替えあり
    pModel = GFL_NEW(m_pHeap) poke_3d::model::DressUpModel();
    m_CharaModelFactory.CreateDressUpModel(m_pGfxAllocator, m_pHeap, static_cast<poke_3d::model::DressUpModel*>(pModel), &m_DressUpModelResourceManager, m_charaId);
  }
#else
  {
    // 着せ替えなし
    pModel = GFL_NEW(m_pHeap) poke_3d::model::CharaModel();
    m_CharaModelFactory.CreateModel(m_pGfxAllocator, m_pHeap, static_cast<poke_3d::model::CharaModel*>(pModel), m_charaId);
  }
#endif
  return pModel;
}

s32 ModelPlayer::GetAnimationResId(s32 animeId)
{
  switch(animeId)
  {
  case ANIME_SURF01_STRAIGHT:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS001_SURF01_STRAIGHT_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS001_SURF01_STRAIGHT_GFBMOT;
  case ANIME_SURF02_LEFT:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS002_SURF02_LEFT_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS002_SURF02_LEFT_GFBMOT;
  case ANIME_SURF03_RIGHT:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS003_SURF03_RIGHT_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS003_SURF03_RIGHT_GFBMOT;
  case ANIME_SURF04_WAVE_RIGHT:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS026_SURF04_WAVE_RIGHT_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS026_SURF04_WAVE_RIGHT_GFBMOT;
  case ANIME_AERIAL_START:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS004_AERIAL_START_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS004_AERIAL_START_GFBMOT;
  case ANIME_AERIAL_LOOP:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS005_AERIAL_LOOP_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS005_AERIAL_LOOP_GFBMOT;
  case ANIME_AERIAL_END:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS006_AERIAL_END_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS006_AERIAL_END_GFBMOT;
  case ANIME_OVERTURN_START:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS007_OVERTURN_START_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS007_OVERTURN_START_GFBMOT;
  case ANIME_OVERTURN_LOOP:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS008_OVERTURN_LOOP_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS008_OVERTURN_LOOP_GFBMOT;
  case ANIME_OVERTURN_END:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS009_OVERTURN_END_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS009_OVERTURN_END_GFBMOT;
  case ANIME_PADDLING_LOOP:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS011_PADDLING_LOOP_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS011_PADDLING_LOOP_GFBMOT;
  case ANIME_PADDLING_END:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS012_PADDLING_END_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS012_PADDLING_END_GFBMOT;
  case ANIME_SHAKY:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS013_SHAKY_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS013_SHAKY_GFBMOT;
  case ANIME_SKILL_NORMAL_LEFT:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS014_TRICK01_LEFT_SOMERSAULT_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS014_TRICK01_LEFT_SOMERSAULT_GFBMOT;
  case ANIME_SKILL_NORMAL_RIGHT:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS015_TRICK02_RIGHT_SOMERSAULT_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS015_TRICK02_RIGHT_SOMERSAULT_GFBMOT;
  case ANIME_SKILL_NORMAL_FORWARD:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS016_TRICK03_FORWARD_SOMERSAULT_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS016_TRICK03_FORWARD_SOMERSAULT_GFBMOT;
  case ANIME_SKILL_NORMAL_BACKWARD:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS017_TRICK04_BACKWARD_SOMERSAULT_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS017_TRICK04_BACKWARD_SOMERSAULT_GFBMOT;
  case ANIME_SKILL_ID_EX_01:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS019_TRICK06_SCREWDRIVER_RIGHT_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS019_TRICK06_SCREWDRIVER_RIGHT_GFBMOT;
  case ANIME_SKILL_ID_EX_02:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS018_TRICK05_SCREWDRIVER_LEFT_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS018_TRICK05_SCREWDRIVER_LEFT_GFBMOT;
  case ANIME_SKILL_ID_EX_03:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS020_TRICK07_SKYHIGHDIVE_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS020_TRICK07_SKYHIGHDIVE_GFBMOT;
  case ANIME_SKILL_ID_EX_04:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS021_TRICK08_REVERSE_AND_REVERSE_GFBMOT: BL_IDX_P2_BASE_MS_P2_BASE_MS021_TRICK08_REVERSE_AND_REVERSE_GFBMOT;
  case ANIME_SKILL_ID_EX_05:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS022_TRICK09_OVER_THE_RAINBOW_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS022_TRICK09_OVER_THE_RAINBOW_GFBMOT;
  case ANIME_SKILL_ID_EX_06:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS023_TRICK10_SPLASH_TORNADO_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS023_TRICK10_SPLASH_TORNADO_GFBMOT;
  case ANIME_SKILL_ID_EX_07:
    return m_sex == PM_MALE ? BL_IDX_P1_BASE_MS_P1_BASE_MS024_TRICK11_GAMUSHARA_GFBMOT : BL_IDX_P2_BASE_MS_P2_BASE_MS024_TRICK11_GAMUSHARA_GFBMOT;
  default:
    return ModelBaseChara::GetAnimationResId(animeId);
  }
}

s32 ModelPlayer::GetAnimationBlendFrame(s32 animeId)
{
  switch(animeId)
  {
  case ANIME_SURF04_WAVE_RIGHT:
    return m_surfRightMotionBlendFrame;
  case ANIME_SURF01_STRAIGHT:
  case ANIME_SURF02_LEFT:
  case ANIME_SURF03_RIGHT:
  case ANIME_AERIAL_START:
  case ANIME_OVERTURN_START:
  case ANIME_SHAKY:
    return DEFAULT_ANIME_BLEND_FRAME;
  default:
    return ModelBaseChara::GetAnimationBlendFrame(animeId);
  }
}

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
