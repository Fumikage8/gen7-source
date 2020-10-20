//======================================================================
/**
* @file ModelBase.h
* @date 2016/09/12 16:39:59
* @author fang_yicheng
* @brief ベースモデル
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if !defined __MODEL_BASE_H_INCLUDED__
#define __MODEL_BASE_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <ctype.h>

#include <System/include/HeapDefine.h>
#include <AppLib/include/Util/app_util_heap.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryCollisionMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_AnimationResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameProcManager.h>

#include <model/include/gfl2_BaseModel.h>

#include "../MantainSurfRenderingPipeline.h"
#include "../Resource/ResourceManager.h"
#include "../Effect/EffectManager.h"
#include "../Collision/ColliderSphere.h"
#include "../Collision/ColliderCylinder.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)

class ModelBase
{
  GFL_FORBID_COPY_AND_ASSIGN(ModelBase);

public:

  enum
  {
    DEFAULT_ANIME_BLEND_FRAME = 6,   // デフォルトアニメーションブレンドフレーム
  };

  enum ANIME_LOOP_FLAG
  {
    ANIME_LOOP_DEFAULT = 0,
    ANIME_LOOP_ON,
    ANIME_LOOP_OFF,
  };

public:

  ModelBase();
  virtual ~ModelBase();

  virtual void Initialize(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pHeap, Resource::ResourceManager *pResourceManager, Effect::EffectManager* pEffectManager);
  virtual void Terminate();
  virtual void Update();
  virtual void UpdateAfterTraverse();
  virtual void UpdateAnimation();

  // アニメーション
  void ChangeAnimation(s32 animeId, bool isForce = false, ANIME_LOOP_FLAG loopFlag = ANIME_LOOP_DEFAULT, u32 slotId = 0);
  void EndAnimation(u32 slotId = 0);
  bool IsAnimationEnd(u32 slotId = 0, s32 animeId = -1, bool isOneFrameAdvance = false);
  f32  GetAnimationEndFrame(u32 slotId = 0);
  f32  GetAnimationFrame(u32 slotId = 0);
  f32  GetAnimationProgress(u32 slotId = 0);
  void SetAnimationProgress(f32 progress, u32 slotId = 0);

  // エフェクト
  void StartEffect(s32 eftId, const gfl2::math::Vector3 &startScale = gfl2::math::Vector3(1.0f, 1.0f, 1.0f));
  void EndEffect(s32 eftId);
  void EndAllEffect();
  void StopEffect();
  void ResumeEffect();
  void SetEffectVisible(bool isVisible);
  void UpdateEffect();
  gfl2::math::SRT GetSrtForEffect(s32 eftId);
  gfl2::math::Vector3  GetEffectScale(s32 eftId);
  void SetEffectPosition(s32 eftId, const gfl2::math::Vector3 &position);
  void SetEffectScale(s32 eftId, const gfl2::math::Vector3 &scale);

  // 
  //gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* GetModelInstanceNode() const;
  void AddToRenderingPipeLine(MantainSurfRenderingPipeline* pRenderingPipeLine, bool edgeFlag);
  void AddChildModel(ModelBase* pChildModel);

  const gfl2::math::Vector3 GetTransX();
  const gfl2::math::Vector3 GetTransY();
  const gfl2::math::Vector3 GetTransZ();
  
  const gfl2::math::Vector3 GetPosition();
  const gfl2::math::Vector3 GetJointPosition(s32 jointIndex = -1);
  const gfl2::math::Vector3 GetJointPosition(const char* jointName);
  void SetPosition(const gfl2::math::Vector3 &position);
  void SetScale(const gfl2::math::Vector3 &scale);

  void SetRotation(const gfl2::math::Vector3 &rotation, gfl2::math::EulerRotation::RotationOrder order = gfl2::math::EulerRotation::RotationOrderZYX);
  gfl2::math::Vector3 GetRotation();
  gfl2::math::Quaternion GetRotationQuat();

  bool IsVisible() const;
  void SetVisible(bool visible);

  poke_3d::model::BaseModel* GetModel();

  gfl2::util::List<Collision::ColliderBase*>* GetColliderList();

  bool IsColliderEnabled();
  void SetColliderEnabled(bool enable);

  Collision::ColliderBase::CollisionInfo* GetMainCollionInfo();
  void SetMainCollionInfo(Collision::ColliderBase::CollisionInfo &collisionInfo);

  //
  void ResetZPosition(const f32 resetLength);

protected:

  void CreateModelParts();
  void InitializeAnimation();
  void InitializeEffect();

  // コリジョン
  void AddSphereCollider(f32 radius, s32 jointIndex = -1);
  void AddCylinderCollider(f32 radius, f32 height, s32 jointIndex = -1);

  virtual poke_3d::model::BaseModel* CreateModel(s32 resId);

  virtual void  CreateCollider()                      {};
  virtual s32   GetModelPartsResId(s32 partsId)       { return -1; };
  virtual s32   GetModelPartsJointId(s32 partsId)     { return -1; };
  virtual s32   GetAnimationResId(s32 animeId)        { return -1; };
  virtual s32   GetAnimationBlendFrame(s32 animeId)   { return 0;  };
  virtual s32   GetEffectResId(s32 eftId)             { return -1; };
  virtual gfl2::math::Vector3 GetEffectPosition(s32 eftId) { return GetPosition(); };
  virtual gfl2::math::Vector3 GetEffectOffset(s32 eftId)   { return gfl2::math::Vector3::GetZero(); };
  virtual gfl2::math::Quaternion GetEffectRotation(s32 eftId);

  //virtual s32   GetPlacementJointIndex()  { return -1; };

private:

  void AddCollider(Collision::ColliderBase* pCollider);

protected:

  gfl2::gfx::IGLAllocator*        m_pGfxAllocator;
  gfl2::heap::HeapBase*           m_pHeap;
  Resource::ResourceManager*      m_pResourceManager;

  MantainSurfRenderingPipeline*   m_pRenderingPipeLine;

  poke_3d::model::BaseModel*      m_pBaseModel;
  poke_3d::model::BaseModel**     m_ppModelParts;

  bool m_isReverseTransX;
  bool m_isReverseTransY;
  bool m_isReverseTransZ;
  gfl2::math::Vector3 m_transY;
  gfl2::math::Vector3 m_transX;
  gfl2::math::Vector3 m_transZ;
  gfl2::math::Vector3 m_prevPosition;

protected:

  // セットアップ
  struct SetupParam
  {
    bool  m_isDressUpModel;
    s32   m_resourceId;
    s32   m_modelResourceId;
    s32   m_modelPartsSize;
    s32   m_animationListSize;
    s32   m_effectListSize;
    s32   m_colliderListSize;
  };
  SetupParam m_SetupParam;

protected:

  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pResourceNode;

  // アニメーション
  gfl2::renderingengine::scenegraph::resource::ResourceNode** m_ppAnimeResourceNodes;

  // エフェクト
  Effect::EffectManager*  m_pEffectManager;
  Effect::EffectBase**    m_ppEffectListMain;   // メインエフェクト
  Effect::EffectBase**    m_ppEffectListBack;   // バックアップエフェクト
  gfl2::math::Vector3*    m_pEffectPosArray;
  gfl2::math::Vector3*    m_pEffectScaleArray;
  bool                    m_IsEffectVisible;

  // コリジョン
  gfl2::util::List<Collision::ColliderBase*> m_ColliderList;
  Collision::ColliderBase::CollisionInfo     m_MainCollisionInfo;
  bool m_isColliderEnabled;

};

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __MODEL_BASE_H_INCLUDED__
