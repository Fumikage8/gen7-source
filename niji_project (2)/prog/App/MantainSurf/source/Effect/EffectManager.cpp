//======================================================================
/**
 * @file EffectManager.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief エフェクトマネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "EffectManager.h"
#include "EffectPartical.h"
#include "EffectModel.h"
#include "../MantainSurfAppManager.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Effect)

EffectManager::EffectManager() :
  m_pEffectForDebug(NULL),
  m_pEffectLateRelease(NULL)
{
}

EffectManager::~EffectManager()
{
}

void EffectManager::Initialize(const SetupParam &setupParam)
{
  m_SetupParam = setupParam;

  // エフェクトシステム登録
  for(s32 i = 0; i < m_SetupParam.m_pResourceManager->GetResourceNum(); i++)
  {
    if(GetEffectType(i) == EFFECT_PARTICAL)
    {
      m_SetupParam.m_pEffectSystem->EntryResource(m_SetupParam.m_pEffectHeap, m_SetupParam.m_pResourceManager->GetResourceData(i), i);
    }
    else
    {
      HOU_PRINT("[EffectManager]None Partical Effect(%d)\n", i);
    }
  }
}

void EffectManager::Terminate()
{
  if(m_pEffectLateRelease) m_pEffectLateRelease->Terminate();
  GFL_SAFE_DELETE(m_pEffectLateRelease);
  if(m_pEffectForDebug) m_pEffectForDebug->Terminate();
  GFL_SAFE_DELETE(m_pEffectForDebug);
  for(u32 i = 0; i < m_SetupParam.m_pResourceManager->GetResourceNum(); i++)
  {
    if(GetEffectType(i) == EFFECT_PARTICAL)
    {
      m_SetupParam.m_pEffectSystem->ClearResource(m_SetupParam.m_pEffectHeap, i);
    }
  }

}

void EffectManager::Update()
{
  m_SetupParam.m_pEffectSystem->BeginFrame();
  m_SetupParam.m_pEffectSystem->Calc(0);
}

#if PM_DEBUG
void EffectManager::UpdateForDebug(gfl2::math::SRT targetSrt)
{
  if(m_pEffectLateRelease) m_pEffectLateRelease->Terminate();
  GFL_SAFE_DELETE(m_pEffectLateRelease);

  s32 resId = m_SetupParam.m_pAppManager->GetDebugMenu()->m_dbvPlayingEffectResId;
  m_SetupParam.m_pAppManager->GetDebugMenu()->m_dbvPlayingEffectResId = -1;
  if(resId != -1 && GetEffectType(resId) != EFFECT_NONE)
  {
    if(m_pEffectForDebug)
    {
      m_pEffectForDebug->Kill();
      m_pEffectLateRelease = m_pEffectForDebug;
      m_pEffectForDebug = NULL;
    }
    m_pEffectForDebug = CreateEffect(resId);
    m_pEffectForDebug->Start();
    GFL_PRINT("[PlayEffect]id=%d\n", resId);
  }

  if(m_pEffectForDebug)
  {
    m_pEffectForDebug->UpdatePosition(targetSrt);
  }
}

void EffectManager::StopEffectForDebug()
{
  if(m_pEffectForDebug)
  {
    m_pEffectForDebug->Kill();
  }
}
#endif

EffectBase* EffectManager::CreateEffect(s32 resId)
{
  GFL_ASSERT(resId >= 0 && resId < GARC_mantain_effect_DATA_NUM);

  EffectBase*     pBaseEffect = NULL;
  EffectPartical* pEffectPartical;
  EffectModel*    pEffectModel;
  s32 motionResId = -1;
  switch(GetEffectType(resId))
  {
  case EFFECT_PARTICAL:
    pEffectPartical = GFL_NEW(m_SetupParam.m_pEffectHeap->GetGFHeapBase()) EffectPartical();
    pEffectPartical->Initialize(resId, m_SetupParam.m_pEffectSystem, m_SetupParam.m_pEffectHeap);
    pBaseEffect = pEffectPartical;
    break;

  case EFFECT_MODEL:
    pEffectModel = GFL_NEW(m_SetupParam.m_pEffectHeap->GetGFHeapBase()) EffectModel();
    pEffectModel->Initialize(m_SetupParam.m_pGfxAllocator, m_SetupParam.m_pEffectHeap, m_SetupParam.m_pRenderingPipeline);
    pEffectModel->CreateModelNode(m_SetupParam.m_pResourceManager->GetResourceData(resId));
    // モーションを作成
    motionResId = GetMotionResId(resId);
    if(motionResId >= 0)
    {
      pEffectModel->CreateAnimeNode(m_SetupParam.m_pResourceManager->GetResourceData(motionResId));
    }
    pBaseEffect = pEffectModel;
    break;

  default:
    break;
  }
  return pBaseEffect;
}

EffectManager::EFFECT_TYPE EffectManager::GetEffectType(s32 resId)
{
  switch(resId)
  {
  case GARC_mantain_effect_ea_manta_air_success_end_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_air_success_level01_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_air_success_level02_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_air_success_level03_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_air_success_start_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_bg_waterwall01_lo_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_cam_air_level01_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_cam_air_level02_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_cam_air_level03_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_cam_shootingstar_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_cam_sunshine_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_cam_wind_dash_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_cam_wind_dash01_lo_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_cam_wind_dash02_lo_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_cam_wind_dash03_lo_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_cam_zoom_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_glide_dropwater_lo_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_glide_lo_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_hurdle_hit_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_jet_lo_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_run_dash_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_run_dash01_lo_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_run_dash01_lo_or_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_run_dash01_lo_yl_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_run_dash02_lo_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_run_fol_lo_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_run_lo_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_turn01_l_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_turn01_r_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_turn02_l_lo_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_turn02_r_lo_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_wait_lo_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_water_hit_l_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_water_hit_m_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_water_hit_s_lz_BPTCL:
  case GARC_mantain_effect_ea_manta_water_whale_lo_lz_BPTCL:
    return EFFECT_PARTICAL;
  default:
    return EFFECT_NONE;
  }
}

s32 EffectManager::GetMotionResId(s32 resId)
{
  switch(resId)
  {
  default:
    return -1;
  }
}

bool EffectManager::IsParticalEffect(s32 resId)
{
  return GetEffectType(resId) == EFFECT_PARTICAL;
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
