//======================================================================
/**
 * @file EffectModel.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief モデル型エフェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "EffectModel.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Effect)

EffectModel::EffectModel()
{
  m_pGfxAllocator = NULL;
  m_pEffectHeap = NULL;
  m_pRenderingPipeline = NULL;
  m_pModel = NULL;
  m_pModelResourceNode = NULL;
  m_pAnimeResourceNode = NULL;
  m_isStop = false;
}

EffectModel::~EffectModel()
{
}

void EffectModel::Initialize(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::Effect::GFHeap* pEffectHeap, MantainSurfRenderingPipeline* pRenderingPipeline)
{
  m_pGfxAllocator = pGfxAllocator;
  m_pEffectHeap = pEffectHeap;
  m_pRenderingPipeline = pRenderingPipeline;
}

void EffectModel::Terminate()
{
  GFL_SAFE_DELETE(m_pModel);
  GFL_SAFE_DELETE(m_pAnimeResourceNode);
  GFL_SAFE_DELETE(m_pModelResourceNode);
}

void EffectModel::CreateModelNode(void* pResourceData)
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
  resourceData.SetModelData(static_cast<c8*>(pResourceData));
  // リソースノードを作成
  m_pModelResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pGfxAllocator, &resourceData);
  // モデル作成
  m_pModel = GFL_NEW(m_pEffectHeap->GetGFHeapBase()) poke_3d::model::BaseModel();
  m_pModel->Create(m_pGfxAllocator, m_pEffectHeap->GetGFHeapBase(), m_pModelResourceNode);
  m_pModel->SetVisible(false);
}

void EffectModel::CreateAnimeNode(void* pResourceData)
{
  gfl2::renderingengine::scenegraph::resource::AnimationResourceNodeFactory factory;
  m_pAnimeResourceNode = factory.CreateResourceNode(m_pGfxAllocator, pResourceData, 0);
}

bool EffectModel::Start()
{
  bool isCreate = !IsAlive();
  if(isCreate)
  {
    m_pRenderingPipeline->AddModelEffect(m_pModel);
    m_pModel->ChangeAnimationByResourceNode(m_pAnimeResourceNode);
    m_pModel->SetAnimationFrame(0.0f);
    m_pModel->SetVisible(true);
  }
  return isCreate;
}

void EffectModel::End()
{
  m_pRenderingPipeline->RemoveObject(m_pModel);
  m_pModel->ChangeAnimationByResourceNode(NULL);
  m_pModel->SetVisible(false);
}

void EffectModel::Kill()
{
  End();
}

void EffectModel::SetVisible(bool visible)
{
  if(m_pModel) m_pModel->SetVisible(visible);
}

void EffectModel::Stop()
{
  m_isStop = true;
}

void EffectModel::Resume()
{
  m_isStop = false;
}

bool EffectModel::IsAlive()
{
  if(m_pModel == NULL)
  {
    return false;
  }
  if(!m_pModel->IsVisible())
  {
    return false;
  }
  if(m_pModel->GetAnimationResourceNode() == NULL)
  {
    return false;
  }
  if(!m_pModel->IsAnimationLoop() && m_pModel->IsAnimationLastFrame())
  {
    return false;
  }
  return true;
}

void EffectModel::UpdatePosition(const gfl2::math::SRT &srt)
{
  if(!m_isStop)
  {
    m_pModel->UpdateAnimation();
  }
  m_pModel->GetModelInstanceNode()->SetLocalSRT(srt);
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
