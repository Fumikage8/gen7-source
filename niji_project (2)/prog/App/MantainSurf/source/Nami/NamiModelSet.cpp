//======================================================================
/**
 * @file NamiModelSet.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief 波のモデルセット
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "NamiModelSet.h"
#include "../MantainSurfAppManager.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Nami)

const f32 WAVE_SYNC_OFFSET = -1000.0f;    // 波位置同期のオフセット

NamiModelSet::NamiModelSet() :
  m_pAppManager(NULL)
{
  for(u32 i = 0; i < NAMI_BASE_NUM; i++)
  {
    m_pNamiBaseModels[i] = NULL;
  }
  for(u32 i = 0; i < NAMI_MAX_NUM; i++)
  {
    m_pAllNamiModels[i] = NULL;
  }
}

NamiModelSet::~NamiModelSet()
{
}

void NamiModelSet::Create(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pHeap, Resource::ResourceManager *pResourceManager, Effect::EffectManager* pEffectManager, MantainSurfAppManager* pAppManager, const MapData &mapData)
{
  m_pAppManager = pAppManager;
  m_MapData = mapData;

  bool isUseLodModel = false;
#if PM_DEBUG
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
  if(pButton->IsHold(gfl2::ui::BUTTON_L))
  {
    //isUseLodModel = true;
  }
#endif

  // 波ベース部分を作成
  for(s32 i = 0; i < NAMI_BASE_NUM; i++)
  {
    m_pNamiBaseModels[i] = GFL_NEW(pHeap) Model::ModelNami(isUseLodModel);
    CreateCommon(m_pNamiBaseModels[i], pGfxAllocator, pHeap, pResourceManager, pEffectManager);
    m_pAllNamiModels[i] = m_pNamiBaseModels[i];
  }
  // 水しぶきエフェクト
  m_pNamiBaseModels[0]->StartEffect(Model::ModelNami::EFT_WATER_WALL);
  
  // 終了部分を作成
  for(s32 i = 0; i < END_PARTS_MAX; i++)
  {
    m_NamiEndParts[i].m_pNamiEndModel = GFL_NEW(pHeap) Model::ModelNamiEnd();
    m_NamiEndParts[i].m_pNamiModel = GFL_NEW(pHeap) Model::ModelNami(isUseLodModel);
    CreateCommon(m_NamiEndParts[i].m_pNamiEndModel, pGfxAllocator, pHeap, pResourceManager, pEffectManager);
    CreateCommon(m_NamiEndParts[i].m_pNamiModel, pGfxAllocator, pHeap, pResourceManager, pEffectManager);
    // 前方終端
    m_pAllNamiModels[NAMI_BASE_NUM] = m_NamiEndParts[i].m_pNamiModel;
    m_pAllNamiModels[NAMI_BASE_NUM + 1] = m_NamiEndParts[i].m_pNamiEndModel;
  }
  UpdatePosition(m_MapData.m_playerInitPos);
}

void NamiModelSet::CreateCommon(Model::ModelNami* pNamiModel, gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pHeap, Resource::ResourceManager *pResourceManager, Effect::EffectManager* pEffectManager)
{
  pNamiModel->Initialize(pGfxAllocator, pHeap, pResourceManager, pEffectManager);
  pNamiModel->SetRotation(gfl2::math::Vector3(0.0f, PI, 0.0f));
  pNamiModel->SetPosition(m_MapData.m_mapInitPos);
  pNamiModel->StartAnimation();
}

void NamiModelSet::Destory()
{
  for(s32 i = 0; i < NAMI_MAX_NUM; i++)
  {
    m_pAllNamiModels[i]->Terminate();
    GFL_SAFE_DELETE(m_pAllNamiModels[i]);
  }
}

void NamiModelSet::Update(const gfl2::math::Vector3 &playerPosition)
{
  if(m_pAppManager->GetGameState() < MantainSurfAppManager::ST_ENDING)
  {
    UpdatePosition(playerPosition);
    m_pNamiBaseModels[0]->SetPlayerPosition(playerPosition);
  }

  for(s32 i = 0; i < NAMI_MAX_NUM; i++)
  {
    m_pAllNamiModels[i]->Update();
  }
}

void NamiModelSet::UpdateAfterTraverse()
{
  for(s32 i = 0; i < NAMI_MAX_NUM; i++)
  {
    m_pAllNamiModels[i]->UpdateAfterTraverse();
  }
}

void NamiModelSet::UpdateTimeZone(s32 timeZone)
{
  for(s32 i = 0; i < NAMI_MAX_NUM; i++)
  {
    m_pAllNamiModels[i]->UpdateTimeZone(timeZone);
  }
}

void NamiModelSet::AddToRenderingPipeLine(MantainSurfRenderingPipeline* pRenderingPipeLine, bool edgeFlag)
{
  for(s32 i = 0; i < NAMI_MAX_NUM; i++)
  {
    pRenderingPipeLine->AddNamiModel(m_pAllNamiModels[i]->GetModel());
  }
}

void NamiModelSet::SetHeightAnimationFrame(f32 heigthRate)
{
  for(s32 i = 0; i < NAMI_MAX_NUM; i++)
  {
    m_pAllNamiModels[i]->SetHeightAnimationFrame(heigthRate);
  }
}

gfl2::math::Vector3 NamiModelSet::GetTopPosition()
{
  return m_pNamiBaseModels[0]->GetTopPosition();
}

void NamiModelSet::StopEffect()
{
  for(s32 i = 0; i < NAMI_MAX_NUM; i++)
  {
    m_pAllNamiModels[i]->StopEffect();
  }
}

void NamiModelSet::ResumeEffect()
{
  for(s32 i = 0; i < NAMI_MAX_NUM; i++)
  {
    m_pAllNamiModels[i]->ResumeEffect();
  }
}

void NamiModelSet::UpdatePosition(const gfl2::math::Vector3 &playerPosition)
{
#if PM_DEBUG
  if(!m_pAppManager->GetDebugMenu()->m_dbvIsSyncWavePosition) return;
#endif

  gfl2::math::Vector3 diff = playerPosition - m_MapData.m_mapInitPos;   // プレイヤーとマップ原点の距離
  f32 rate = diff.z / m_MapData.m_mapDepth;
  s32 count = static_cast<s32>(rate);
  if(diff.z < 0.0f)
  {
    count--;
    rate += gfl2::math::FAbs(count);
  }
  else
  {
    rate -= count;
  }
  
  // 波ベースの位置をセット
  {
    f32 start_z = -NAMI_BASE_NUM / 2 * m_MapData.m_mapDepth;
    f32 offset_z = count * m_MapData.m_mapDepth;
    gfl2::math::Vector3 newPos;
    for(s32 i = 0; i < NAMI_BASE_NUM; i++)
    {
      newPos = m_MapData.m_mapInitPos;
      newPos.z += start_z + i * m_MapData.m_mapDepth + offset_z + WAVE_SYNC_OFFSET;
      m_pNamiBaseModels[i]->SetPosition(newPos);
    }
  }

  // 終端部分の位置を調整
  {
    // 前方終端
    f32 base_distance = gfl2::math::FCeil(NAMI_BASE_NUM / 2.0f) * m_MapData.m_mapDepth;
    gfl2::math::Vector3 newPos = m_NamiEndParts[END_PARTS_FRONT].m_pNamiEndModel->GetPosition();
    newPos.z = playerPosition.z + base_distance + WAVE_SYNC_OFFSET;
    m_NamiEndParts[END_PARTS_FRONT].m_pNamiEndModel->SetPosition(newPos);
    // 可変長部分の位置調整
    f32 depth = m_MapData.m_mapDepth * rate;
    newPos.z -= depth;
    m_NamiEndParts[END_PARTS_FRONT].m_pNamiModel->SetPosition(newPos);
    m_NamiEndParts[END_PARTS_FRONT].m_pNamiModel->SetScale(gfl2::math::Vector3(1.0f, 1.0f, rate));
  }
}

void NamiModelSet::ResetZPosition(const f32 resetLength)
{
  for(s32 i = 0; i < NAMI_MAX_NUM; i++)
  {
    m_pAllNamiModels[i]->ResetZPosition(resetLength);
  }
}

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)