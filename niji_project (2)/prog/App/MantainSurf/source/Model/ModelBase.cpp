//======================================================================
/**
* @file BaseModel.cpp
* @date 2016/09/12 16:40:17
* @author fang_yicheng
* @brief ベースモデル
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#include "ModelBase.h"
#include "../MantainSurfAppManager.h"
#include "../Effect/EffectPartical.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)

ModelBase::ModelBase() :
  m_pGfxAllocator(NULL),
  m_pHeap(NULL),
  m_pResourceManager(NULL),
  m_pRenderingPipeLine(NULL),
  m_pBaseModel(NULL),
  m_ppModelParts(NULL),
  m_isReverseTransX(false),
  m_isReverseTransY(false),
  m_isReverseTransZ(false),
  m_transX(gfl2::math::Vector3(1.0f, 0.0f, 0.0f)),
  m_transY(gfl2::math::Vector3(0.0f, 1.0f, 0.0f)),
  m_transZ(gfl2::math::Vector3(0.0f, 0.0f, 1.0f)),
  m_pResourceNode(NULL),
  m_ppAnimeResourceNodes(NULL),
  m_ppEffectListMain(NULL),
  m_ppEffectListBack(NULL),
  m_pEffectManager(NULL),
  m_pEffectPosArray(NULL),
  m_pEffectScaleArray(NULL),
  m_IsEffectVisible(true),
  m_isColliderEnabled(false)
{
  m_SetupParam.m_isDressUpModel = false;
  m_SetupParam.m_resourceId = -1;
  m_SetupParam.m_modelResourceId = -1;
  m_SetupParam.m_modelPartsSize = 0;
  m_SetupParam.m_animationListSize = 0;
  m_SetupParam.m_effectListSize = 0;
  m_SetupParam.m_colliderListSize = 0;
}

ModelBase::~ModelBase()
{
}

void ModelBase::Initialize(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pHeap,
  Resource::ResourceManager *pResourceManager, Effect::EffectManager* pEffectManager)
{
  m_pGfxAllocator = pGfxAllocator;
  m_pHeap = pHeap;
  m_pResourceManager = pResourceManager;
  m_pEffectManager = pEffectManager;

  // モデルの作成
  {
    // モデル作成
    m_pBaseModel = CreateModel(m_SetupParam.m_modelResourceId);
  }

  // モデルパーツ作成
  {
    CreateModelParts();
  }

  // コライダー作成
  if(m_SetupParam.m_colliderListSize > 0)
  {
    m_ColliderList.CreateBuffer(pHeap, m_SetupParam.m_colliderListSize);
    CreateCollider();
    m_isColliderEnabled = true;
  }

  // アニメーション初期化
  {
    InitializeAnimation();
  }

  // エフェクト初期化
  {
    InitializeEffect();
  }
}

void ModelBase::Terminate()
{
  // エフェクトリソース開放
  {
    for(s32 i = 0; i < m_SetupParam.m_effectListSize; i++)
    {
      if(m_ppEffectListMain != NULL)
      {
        m_ppEffectListMain[i]->Terminate();
        GFL_SAFE_DELETE(m_ppEffectListMain[i]);
      }
      if(m_ppEffectListBack != NULL)
      {
        m_ppEffectListBack[i]->Terminate();
        GFL_SAFE_DELETE(m_ppEffectListBack[i]);
      }
    }
    GFL_SAFE_DELETE_ARRAY(m_ppEffectListMain);
    GFL_SAFE_DELETE_ARRAY(m_ppEffectListBack);
  }
  GFL_SAFE_DELETE_ARRAY(m_pEffectScaleArray);
  GFL_SAFE_DELETE_ARRAY(m_pEffectPosArray);

  // アニメーションリソース開放
  if(m_ppAnimeResourceNodes != NULL)
  {
    for(s32 i = 0; i < m_SetupParam.m_animationListSize; i++)
    {
      GFL_SAFE_DELETE(m_ppAnimeResourceNodes[i]);
    }
    GFL_SAFE_DELETE_ARRAY(m_ppAnimeResourceNodes);
  }

  // コリジョン開放
  if(m_ColliderList.Size() > 0)
  {
    gfl2::util::ListIterator<Collision::ColliderBase*> it = m_ColliderList.Begin();
    for(; it != m_ColliderList.End(); it++)
    {
      GFL_SAFE_DELETE(*it);
    }
    m_ColliderList.Clear();
  }

  // モデルパーツ開放
  if(m_ppModelParts != NULL)
  {
    for(s32 i = 0; i < m_SetupParam.m_modelPartsSize; i++)
    {
      GFL_SAFE_DELETE(m_ppModelParts[i]);
    }
    GFL_SAFE_DELETE_ARRAY(m_ppModelParts);
  }

  // 描画対象から削除
  if(m_pRenderingPipeLine && m_pBaseModel)
  {
    if(!m_SetupParam.m_isDressUpModel)
    {
      m_pRenderingPipeLine->RemoveObject(m_pBaseModel);
    }
    else
    {
      poke_3d::model::DressUpModel* pDressUpModel = static_cast<poke_3d::model::DressUpModel*>(m_pBaseModel);
      m_pRenderingPipeLine->RemoveObject(pDressUpModel);
    }
    m_pBaseModel->Destroy();
  }
  // モデル開放
  GFL_SAFE_DELETE(m_pBaseModel);
  GFL_SAFE_DELETE(m_pResourceNode);
}

void ModelBase::CreateModelParts()
{
  if(m_SetupParam.m_modelPartsSize > 0)
  {
    m_ppModelParts = GFL_NEW_ARRAY(m_pHeap) poke_3d::model::BaseModel*[m_SetupParam.m_modelPartsSize];
    for(s32 i = 0; i < m_SetupParam.m_modelPartsSize; i++)
    {
      m_ppModelParts[i] = CreateModel(GetModelPartsResId(i));
      m_ppModelParts[i]->SetParent(m_pBaseModel, GetModelPartsJointId(i));
    }
  }
}

void ModelBase::AddToRenderingPipeLine(MantainSurfRenderingPipeline* pRenderingPipeLine, bool edgeFlag)
{
  GFL_ASSERT(m_pRenderingPipeLine == NULL);
  m_pRenderingPipeLine = pRenderingPipeLine;
  if(!m_SetupParam.m_isDressUpModel)
  {
    pRenderingPipeLine->AddBaseModel(m_pBaseModel, edgeFlag);
  }
  else
  {
    poke_3d::model::DressUpModel* pDressUpModel = static_cast<poke_3d::model::DressUpModel*>(m_pBaseModel);
    pRenderingPipeLine->AddDressUpModel(pDressUpModel, edgeFlag);
  }

  for(s32 i = 0; i < m_SetupParam.m_modelPartsSize; i++)
  {
    pRenderingPipeLine->AddBaseModel(m_ppModelParts[i], edgeFlag);
  }
}

void ModelBase::AddChildModel(ModelBase* pChildModel)
{
  pChildModel->GetModel()->SetParent(m_pBaseModel);
  //m_pBaseModel->GetModelInstanceNode()->AddChild(pChildModel->GetModel()->GetModelInstanceNode());
}

poke_3d::model::BaseModel* ModelBase::CreateModel(s32 resId)
{
  poke_3d::model::BaseModel* pModel = GFL_NEW(m_pHeap) poke_3d::model::BaseModel();
  if(m_SetupParam.m_resourceId >= 0 && resId >= 0)
  {
    gfl2::fs::BinLinkerAccessor accessor(m_pResourceManager->GetResourceData(m_SetupParam.m_resourceId));
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
    resourceData.SetModelData(static_cast<c8*>(accessor.GetData(resId)));

    // リソースノードを作成
    m_pResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pGfxAllocator, &resourceData);

    // モデル作成
    pModel->Create(m_pGfxAllocator, m_pHeap, m_pResourceNode);
  }
  return pModel;
}

void ModelBase::InitializeAnimation()
{
  if(m_SetupParam.m_animationListSize > 0)
  {
    gfl2::fs::BinLinkerAccessor accessor(m_pResourceManager->GetResourceData(m_SetupParam.m_resourceId));
    m_ppAnimeResourceNodes = GFL_NEW_ARRAY(m_pHeap) gfl2::renderingengine::scenegraph::resource::ResourceNode*[m_SetupParam.m_animationListSize];
    for(s32 i = 0; i < m_SetupParam.m_animationListSize; i++)
    {
      gfl2::renderingengine::scenegraph::resource::AnimationResourceNodeFactory factory;
      m_ppAnimeResourceNodes[i] = factory.CreateResourceNode(m_pGfxAllocator, accessor.GetData(GetAnimationResId(i)), 0);
    }
  }
}

void ModelBase::ChangeAnimation(s32 animeId, bool isForce, ANIME_LOOP_FLAG loopFlag, u32 slotId)
{
  if(m_ppAnimeResourceNodes == NULL)
  {
    HOU_PRINT("[ChangeAnimation]Animation Not Initialize.");
    return;
  }
  if(m_ppAnimeResourceNodes[animeId] != NULL)
  {
    if(!isForce && m_ppAnimeResourceNodes[animeId] == m_pBaseModel->GetAnimationResourceNode(slotId))
    {
      // すでに再生中の場合は再生しない
      return;
    }

    m_pBaseModel->ChangeAnimationSmoothByResourceNode(m_ppAnimeResourceNodes[animeId], GetAnimationBlendFrame(animeId), slotId);
    switch(loopFlag)
    {
    case ANIME_LOOP_ON:
      m_pBaseModel->SetAnimationLoop(true, slotId);
      break;
    case ANIME_LOOP_OFF:
      m_pBaseModel->SetAnimationLoop(false, slotId);
      break;
    }
  }
}

void ModelBase::EndAnimation(u32 slotId)
{
  m_pBaseModel->ChangeAnimationByResourceNode(NULL, slotId);
}

bool ModelBase::IsAnimationEnd(u32 slotId, s32 animeId, bool isOneFrameAdvance)
{
  if(animeId >= 0 && m_ppAnimeResourceNodes[animeId] != m_pBaseModel->GetAnimationResourceNode(slotId))
  {
    return true;
  }
  if(isOneFrameAdvance && m_pBaseModel->GetAnimationEndFrame() >= 1.0f)
  {
    return m_pBaseModel->GetAnimationFrame() + 1.0f == m_pBaseModel->GetAnimationEndFrame();
  }
  return m_pBaseModel->IsAnimationLastFrame(slotId);
}

f32 ModelBase::GetAnimationEndFrame(u32 slotId)
{
  f32 frame = 0.0f;
  if(m_pBaseModel->GetAnimationResourceNode(slotId) != NULL)
  {
    frame = m_pBaseModel->GetAnimationEndFrame(slotId);
  }
  return frame;
}

f32 ModelBase::GetAnimationFrame(u32 slotId)
{
  f32 frame = 0.0f;
  if(m_pBaseModel->GetAnimationResourceNode(slotId) != NULL)
  {
    frame = m_pBaseModel->GetAnimationFrame(slotId);
  }
  return frame;
}

// アニメーションの進行具合を取得
f32 ModelBase::GetAnimationProgress(u32 slotId)
{
  f32 rate = 0.0f;
  if(m_pBaseModel->GetAnimationResourceNode(slotId) != NULL)
  {
    rate = m_pBaseModel->GetAnimationFrame(slotId) / m_pBaseModel->GetAnimationEndFrame(slotId);
  }
  return rate;
}

void ModelBase::SetAnimationProgress(f32 progress, u32 slotId)
{
  if(m_pBaseModel->GetAnimationResourceNode(slotId) != NULL)
  {
    progress = gfl2::math::Clamp(progress, 0.0f, 1.0f);
    f32 frame = m_pBaseModel->GetAnimationEndFrame(slotId) * progress;
    m_pBaseModel->SetAnimationFrame(frame, slotId);
  }
}

void ModelBase::InitializeEffect()
{
  if(m_SetupParam.m_effectListSize > 0)
  {
    // エフェクトリソース
    m_ppEffectListMain = GFL_NEW_ARRAY(m_pHeap) Effect::EffectBase*[m_SetupParam.m_effectListSize];
    m_ppEffectListBack = GFL_NEW_ARRAY(m_pHeap) Effect::EffectBase*[m_SetupParam.m_effectListSize];
    // エフェクトスケール
    m_pEffectPosArray = GFL_NEW_ARRAY(m_pHeap) gfl2::math::Vector3[m_SetupParam.m_effectListSize];
    m_pEffectScaleArray = GFL_NEW_ARRAY(m_pHeap) gfl2::math::Vector3[m_SetupParam.m_effectListSize];
    for(s32 i = 0; i < m_SetupParam.m_effectListSize; i++)
    {
      m_ppEffectListMain[i] = m_pEffectManager->CreateEffect(GetEffectResId(i));
      m_ppEffectListBack[i] = m_pEffectManager->CreateEffect(GetEffectResId(i));
      m_pEffectPosArray[i] = gfl2::math::Vector3::GetZero();
      m_pEffectScaleArray[i] = gfl2::math::Vector3(1.0f, 1.0f, 1.0f);
    }
  }
}

void ModelBase::SetEffectPosition(s32 eftId, const gfl2::math::Vector3 &position)
{
  if(eftId >= m_SetupParam.m_effectListSize)
  {
    return;
  }
  m_pEffectPosArray[eftId] = position;
}

void ModelBase::SetEffectScale(s32 eftId, const gfl2::math::Vector3 &scale)
{
  if(eftId >= m_SetupParam.m_effectListSize)
  {
    return;
  }
  m_pEffectScaleArray[eftId] = scale;
}

gfl2::math::Vector3 ModelBase::GetEffectScale(s32 eftId)
{
  if(eftId >= m_SetupParam.m_effectListSize)
  {
    return gfl2::math::Vector3(1.0f, 1.0f, 1.0f);
  }
  return m_pEffectScaleArray[eftId];
}

void ModelBase::StartEffect(s32 eftId, const gfl2::math::Vector3 &startScale)
{
  if(eftId >= m_SetupParam.m_effectListSize)
  {
    return;
  }
  bool isCreate = false;
  if(m_ppEffectListMain[eftId] != NULL)
  {
    isCreate = m_ppEffectListMain[eftId]->Start();
  }
  if(m_ppEffectListBack[eftId] != NULL)
  {
    m_ppEffectListBack[eftId]->Start();
  }
  if(isCreate && m_pEffectManager->IsParticalEffect(GetEffectResId(eftId)))
  {
    // MainとBackのパーティクル放出の乱数を同期するために同じ種を設定しておく
    Effect::EffectPartical* pPartical1 = static_cast<Effect::EffectPartical*>(m_ppEffectListMain[eftId]);
    Effect::EffectPartical* pPartical2 = static_cast<Effect::EffectPartical*>(m_ppEffectListBack[eftId]);
    u32 seed = gfl2::math::Random::CreateGeneralSeed();
    if(pPartical1->GetHandle()->GetEmitterSet()) pPartical1->GetHandle()->GetEmitterSet()->SetRandomSeed(seed);
    if(pPartical2->GetHandle()->GetEmitterSet()) pPartical2->GetHandle()->GetEmitterSet()->SetRandomSeed(seed);
    SetEffectPosition(eftId, GetPosition());
    SetEffectScale(eftId, startScale);
  }
}

void ModelBase::EndEffect(s32 eftId)
{
  if(eftId >= m_SetupParam.m_effectListSize)
  {
    return;
  }
  if(m_ppEffectListMain[eftId] != NULL)
  {
    m_ppEffectListMain[eftId]->End();
  }
  if(m_ppEffectListBack[eftId] != NULL)
  {
    m_ppEffectListBack[eftId]->End();
  }
}

void ModelBase::EndAllEffect()
{
  for(s32 i = 0; i < m_SetupParam.m_effectListSize; i++)
  {
    EndEffect(i);
  }
}

void ModelBase::StopEffect()
{
  for(s32 i = 0; i < m_SetupParam.m_effectListSize; i++)
  {
    if(m_ppEffectListMain[i] != NULL)
    {
      m_ppEffectListMain[i]->Stop();
    }
    if(m_ppEffectListBack[i] != NULL)
    {
      m_ppEffectListBack[i]->Stop();
    }
  }
}

void ModelBase::ResumeEffect()
{
  for(s32 i = 0; i < m_SetupParam.m_effectListSize; i++)
  {
    if(m_ppEffectListMain[i] != NULL)
    {
      m_ppEffectListMain[i]->Resume();
    }
    if(m_ppEffectListBack[i] != NULL)
    {
      m_ppEffectListBack[i]->Resume();
    }
  }
}

void ModelBase::SetEffectVisible(bool isVisible)
{
  m_IsEffectVisible = isVisible;
}

gfl2::math::SRT ModelBase::GetSrtForEffect(s32 eftId)
{
  gfl2::math::SRT srt;
  // translate
  gfl2::math::Vector3 eftOffset = GetEffectOffset(eftId);
  gfl2::math::Vector3 offset =
    m_transX * eftOffset.x +
    m_transY * eftOffset.y +
    m_transZ * eftOffset.z;
  srt.translate = GetEffectPosition(eftId) + offset;
  // rotation
  srt.rotation = GetEffectRotation(eftId);
  // scale
  srt.scale = m_pEffectScaleArray[eftId];
  return srt;
}

gfl2::math::Quaternion ModelBase::GetEffectRotation(s32 eftId)
{
  gfl2::math::Quaternion rot(0.0f, 0.0f, 0.0f, 1.0f);
  GetRotationQuat().QuaternionToRadian(&rot.x, &rot.y, &rot.z);
  return rot;
}

void ModelBase::UpdateEffect()
{
  if(m_ppEffectListMain == NULL)
  {
    return;
  }
  gfl2::math::SRT srt;
  for(s32 i = 0; i < m_SetupParam.m_effectListSize; i++)
  {
    if(m_ppEffectListMain[i] == NULL)
    {
      continue;
    }
    if(!m_ppEffectListMain[i]->IsAlive())
    {
      continue;
    }
    srt = GetSrtForEffect(i);
    m_ppEffectListMain[i]->UpdatePosition(srt);
    m_ppEffectListMain[i]->SetVisible(m_IsEffectVisible);
    srt.translate.z -= MantainSurfAppManager::Z_POS_RESET_THREDSHOLD;
    m_ppEffectListBack[i]->UpdatePosition(srt);
    m_ppEffectListBack[i]->SetVisible(false);
  }
}

void ModelBase::Update()
{
  UpdateAnimation();
  m_prevPosition = GetPosition();
}

void ModelBase::UpdateAfterTraverse()
{
  UpdateEffect();
}

void ModelBase::UpdateAnimation()
{
  m_pBaseModel->UpdateAnimation();
}

const gfl2::math::Vector3 ModelBase::GetTransX()
{
  return m_transX;
}

const gfl2::math::Vector3 ModelBase::GetTransY()
{
  return m_transY;
}

const gfl2::math::Vector3 ModelBase::GetTransZ()
{
  return m_transZ;
}

const gfl2::math::Vector3 ModelBase::GetPosition()
{
  return m_pBaseModel->GetPosition();
}

const gfl2::math::Vector3 ModelBase::GetJointPosition(s32 jointIndex)
{
  GFL_ASSERT(jointIndex >= 0);
  return m_pBaseModel->GetJointWorldPosition(jointIndex);
}

const gfl2::math::Vector3 ModelBase::GetJointPosition(const char* jointName)
{
  return GetJointPosition(m_pBaseModel->GetJointIndex(jointName));
}

void ModelBase::SetPosition(const gfl2::math::Vector3 &position)
{
  m_pBaseModel->SetPosition(position);
}

void ModelBase::SetScale(const gfl2::math::Vector3 &scale)
{
  m_pBaseModel->SetScale(scale);
}

void ModelBase::SetRotation(const gfl2::math::Vector3 &rotation, gfl2::math::EulerRotation::RotationOrder order)
{
  // 回転を作成
  gfl2::math::EulerRotation euler(
    rotation.x,  // X
    rotation.y,  // Y
    rotation.z,  // Z
    order);
  gfl2::math::Quaternion quat = euler.ToQuaternion();
  m_pBaseModel->SetRotationQuat(quat);

  // ワールド空間方向を更新
  if(!quat.IsAlmostZero())
  {
    gfl2::math::Matrix34 mat;
    quat.QuaternionToMatrix(mat);
    m_transX = mat.TransformNormal(gfl2::math::Vector3(1.0f, 0.0f, 0.0f)) * (m_isReverseTransX ? -1.0f : 1.0f);
    m_transY = mat.TransformNormal(gfl2::math::Vector3(0.0f, 1.0f, 0.0f)) * (m_isReverseTransY ? -1.0f : 1.0f);
    m_transZ = mat.TransformNormal(gfl2::math::Vector3(0.0f, 0.0f, 1.0f)) * (m_isReverseTransZ ? -1.0f : 1.0f);
  }
}

gfl2::math::Vector3 ModelBase::GetRotation()
{
  return m_pBaseModel->GetRotation();
}

gfl2::math::Quaternion ModelBase::GetRotationQuat()
{
  return m_pBaseModel->GetRotationQuat();
}

bool ModelBase::IsVisible() const
{
  return GFL_BOOL_CAST(m_pBaseModel->GetModelInstanceNode()->IsVisible());
}

void ModelBase::SetVisible(bool visible)
{
  m_pBaseModel->GetModelInstanceNode()->SetVisible(visible);
  if(m_SetupParam.m_isDressUpModel)
  {
    poke_3d::model::DressUpModel* pDressUpModel = static_cast<poke_3d::model::DressUpModel*>(m_pBaseModel);
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *pInstanceNode;
    for(u32 i = 0; i < poke_3d::model::DressUpModel::PARTS_MODEL_COUNT; ++i)
    {
      pInstanceNode = pDressUpModel->GetPartsModelInstanceNode(static_cast<poke_3d::model::DressUpModel::PartsModel>(i));
      if(pInstanceNode)
      {
        pInstanceNode->SetVisible(visible);
      }
    }
  }
}

poke_3d::model::BaseModel* ModelBase::GetModel()
{
  return m_pBaseModel;
}

void ModelBase::AddSphereCollider(f32 radius, s32 jointIndex)
{
  Collision::ColliderSphere* pCollider = GFL_NEW(m_pHeap) Collision::ColliderSphere();
  pCollider->Initialize(this, jointIndex);
  pCollider->SetRadius(radius);
  AddCollider(pCollider);
}

void ModelBase::AddCylinderCollider(f32 radius, f32 height, s32 jointIndex)
{
  Collision::ColliderCylinder* pCollider = GFL_NEW(m_pHeap) Collision::ColliderCylinder();
  pCollider->Initialize(this, jointIndex);
  pCollider->SetRadius(radius);
  pCollider->SetHeight(height);
  AddCollider(pCollider);
}

void ModelBase::AddCollider(Collision::ColliderBase* pCollider)
{
  if(m_ColliderList.Size() < m_SetupParam.m_colliderListSize)
  {
    m_ColliderList.PushBack(pCollider);
  }
  else
  {
    HOU_PRINT("[AddSphereCollider]SizeOver max=%d\n", m_SetupParam.m_colliderListSize);
    GFL_ASSERT(0);
  }
}

gfl2::util::List<Collision::ColliderBase*>* ModelBase::GetColliderList()
{
  return &m_ColliderList;
}

bool ModelBase::IsColliderEnabled()
{
  return m_isColliderEnabled;
}

void ModelBase::SetColliderEnabled(bool enable)
{
  m_isColliderEnabled = enable;
}

Collision::ColliderBase::CollisionInfo* ModelBase::GetMainCollionInfo()
{
  return &m_MainCollisionInfo;
}

void ModelBase::SetMainCollionInfo(Collision::ColliderBase::CollisionInfo &collisionInfo)
{
  m_MainCollisionInfo = collisionInfo;
}

void ModelBase::ResetZPosition(const f32 resetLength)
{
  gfl2::math::Vector3 pos = GetPosition();
  pos.z -= resetLength;
  SetPosition(pos);
  // エフェクトのメインとバックアップを切り替え
  Effect::EffectBase** pTmp = m_ppEffectListMain;
  m_ppEffectListMain = m_ppEffectListBack;
  m_ppEffectListBack = pTmp;
}

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
