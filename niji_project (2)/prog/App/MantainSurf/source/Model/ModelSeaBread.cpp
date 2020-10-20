//======================================================================
/**
 * @file ModelSeaBread.cpp
 * @date 2017/03/02 14:36:00
 * @author fang_yicheng
 * @brief 海パン野郎モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "ModelSeaBread.h"
#include <niji_conv_header/field/chara/chara_model_id.h>
#include <niji_conv_header/app/mantain_surf/chara/tr0082_00_ms.h>
#include <arc_index/mantain_chara.gaix>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)

ModelSeaBread::ModelSeaBread()
{
  m_SetupParam.m_resourceId         = GARC_mantain_chara_tr0082_00_ms_BIN;
  m_SetupParam.m_modelResourceId    = ARCID_CHARA_MODEL_FIELD_NORMAL;
  m_SetupParam.m_animationListSize  = ANIME_MAX;
  m_charaId                         = CHARA_MODEL_ID_TR0082_00;
}

ModelSeaBread::~ModelSeaBread()
{
}

void ModelSeaBread::Terminate()
{
  ModelBaseChara::Terminate();
  m_CharaModelFactory.UnloadModelAll();
  m_CharaModelFactory.Finalize();
}

poke_3d::model::BaseModel* ModelSeaBread::CreateModel(s32 resId)
{
  gfl2::fs::AsyncFileManager* pFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

  // キャラモデルファクトリを初期化
  m_CharaModelFactory.Initialize(pFileManager, m_pHeap, resId);
  m_CharaModelFactory.LoadModelSync(m_charaId);
  m_CharaModelFactory.SetupModel(m_pGfxAllocator, m_charaId);

  // モデルを作成
  poke_3d::model::BaseModel *pModel = GFL_NEW(m_pHeap) poke_3d::model::CharaModel();
  m_CharaModelFactory.CreateModel(m_pGfxAllocator, m_pHeap, static_cast<poke_3d::model::CharaModel*>(pModel), m_charaId);
  return pModel;
}

s32 ModelSeaBread::GetAnimationResId(s32 animeId)
{
  switch(animeId)
  {
  case ANIME_SEABREAD_RIDE:
    return BL_IDX_TR0082_00_MS_TR0082_00_MS025_SEABREAD_RIDE_GFBMOT;
  default:
    return ModelBaseChara::GetAnimationResId(animeId);
  }
}

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
