//======================================================================
/**
 * @file CollisionManager.h
 * @date 2016/10/14 16:31:43
 * @author fang_yicheng
 * @brief コリジョンマネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __COLLISION_MANAGER_H_INCLUDED__
#define __COLLISION_MANAGER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <ctype.h>

#include <System/include/HeapDefine.h>
#include <AppLib/include/Util/app_util_heap.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameProcManager.h>

#include "../Model/ModelBase.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
class MantainSurfAppManager;
GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(MantainSurf)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Collision)

class CollisionManager
{
  GFL_FORBID_COPY_AND_ASSIGN(CollisionManager);

public:

  CollisionManager();
  virtual ~CollisionManager();

  void Initialize(gfl2::heap::HeapBase* pHeap, MantainSurfAppManager* pAppManager);
  void Terminate();

  bool AddRootModel(Model::ModelBase* pModel);
  bool AddTargetModel(Model::ModelBase* pModel);

  void RemoveRootModel(Model::ModelBase* pModel);
  void RemoveTargetModel(Model::ModelBase* pModel);

  void PreUpdate();
  void Update();

private:

  void CheckCollision(Model::ModelBase* pRootModel, Model::ModelBase* pTargetModel);

private:

  MantainSurfAppManager* m_pAppManager;

  gfl2::util::List<Model::ModelBase*>   m_rootModelList;    // ルートモデルリスト
  gfl2::util::List<Model::ModelBase*>   m_targetModelList;  // 対象モデルリスト

};

GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
GFL_NAMESPACE_END(Collision)

#endif // __COLLISIONMANAGER_H_INCLUDED__
