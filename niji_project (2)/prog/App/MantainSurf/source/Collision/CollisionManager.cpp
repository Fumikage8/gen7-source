//======================================================================
/**
 * @file CollisionManager.cpp
 * @date 2016/10/14 16:31:14
 * @author fang_yicheng
 * @brief コリジョンマネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "CollisionManager.h"
#include "../MantainSurfAppManager.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Collision)

const s32 MAX_ROOT_MODEL = 4;     // ルートモデルの最大数
const s32 MAX_MODEL      = 256;   // モデルの最大数

CollisionManager::CollisionManager() :
  m_pAppManager(NULL)
{
}

CollisionManager::~CollisionManager()
{
}

void CollisionManager::Initialize(gfl2::heap::HeapBase* pHeap, MantainSurfAppManager* pAppManager)
{
  m_pAppManager = pAppManager;
  m_rootModelList.CreateBuffer(pHeap, MAX_ROOT_MODEL);
  m_targetModelList.CreateBuffer(pHeap, MAX_MODEL);
}

void CollisionManager::Terminate()
{
  m_rootModelList.Clear();
  m_targetModelList.Clear();
}

bool CollisionManager::AddRootModel(Model::ModelBase* pModel)
{
  if(m_rootModelList.Size() < MAX_ROOT_MODEL)
  {
    m_rootModelList.PushBack(pModel);
    return true;
  }
  HOU_PRINT("[AddRootModel]SizeOver max=%d\n", MAX_ROOT_MODEL);
  GFL_ASSERT(0);
  return false;
}

bool CollisionManager::AddTargetModel(Model::ModelBase* pModel)
{
  if(m_targetModelList.Size() < MAX_MODEL)
  {
    m_targetModelList.PushBack(pModel);
    return true;
  }
  HOU_PRINT("[AddModel]SizeOver max=%d\n", MAX_MODEL);
  GFL_ASSERT(0);
  return false;
}

void CollisionManager::RemoveRootModel(Model::ModelBase* pModel)
{
  m_rootModelList.Remove(pModel);
}

void CollisionManager::RemoveTargetModel(Model::ModelBase* pModel)
{
  m_targetModelList.Remove(pModel);
}

void CollisionManager::PreUpdate()
{
  // 位置更新前の座標を記録する
  gfl2::util::ListIterator<Model::ModelBase*> itRootModel = m_rootModelList.Begin();
  gfl2::util::ListIterator<ColliderBase*> itRootCollider;
  for(; itRootModel != m_rootModelList.End(); itRootModel++)
  {
    itRootCollider = (*itRootModel)->GetColliderList()->Begin();
    for(; itRootCollider != (*itRootModel)->GetColliderList()->End(); itRootCollider++)
    {
      (*itRootCollider)->PreSetup();
    }
  }
}

void CollisionManager::Update()
{
  // コリジョンチェック
  gfl2::util::ListIterator<Model::ModelBase*> itRoot = m_rootModelList.Begin();
  gfl2::util::ListIterator<Model::ModelBase*> itTarget;
  for(; itRoot != m_rootModelList.End(); itRoot++)
  {
    (*itRoot)->GetMainCollionInfo()->Initialize();
    if(!(*itRoot)->IsColliderEnabled())
    {
      // コライダー無効の場合はチェックしない
      continue;
    }
    itTarget = m_targetModelList.Begin();
    for(; itTarget != m_targetModelList.End(); itTarget++)
    {
      if(!(*itTarget)->IsColliderEnabled() || !(*itTarget)->IsVisible())
      {
        // コライダー無効の場合はチェックしない
        continue;
      }
      CheckCollision(*itRoot, *itTarget);
    }
  }
}

void CollisionManager::CheckCollision(Model::ModelBase* pRootModel, Model::ModelBase* pTargetModel)
{
  gfl2::util::List<ColliderBase*>* pRootColliderList = pRootModel->GetColliderList();
  gfl2::util::List<ColliderBase*>* pTargetColliderList = pTargetModel->GetColliderList();

  gfl2::util::ListIterator<ColliderBase*> itRoot = pRootColliderList->Begin();
  gfl2::util::ListIterator<ColliderBase*> itTarget;
  for(; itRoot != pRootColliderList->End(); itRoot++)
  {
    itTarget = pTargetColliderList->Begin();
    for(; itTarget != pTargetColliderList->End(); itTarget++)
    {
#if PM_DEBUG
      if(!m_pAppManager->GetDebugMenu()->m_dbvIsCollisionCheckOn)
      {
        // 当たり判定無効
        (*itRoot)->GetCollisionInfo()->Initialize();
        continue;
      }
#endif
      (*itRoot)->TestCollider(*itTarget);
      ColliderBase::CollisionInfo* pCollisonInfo = (*itRoot)->GetCollisionInfo();
      if(pCollisonInfo->m_isCollided)
      {
        // 衝突したため押し返し
        pRootModel->SetMainCollionInfo(*pCollisonInfo);
        pRootModel->SetPosition(pRootModel->GetPosition() + pCollisonInfo->m_solveVector);
      }
    }
  }
}

GFL_NAMESPACE_END(Collision)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
