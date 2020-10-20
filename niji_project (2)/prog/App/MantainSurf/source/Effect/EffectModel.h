//======================================================================
/**
 * @file EffectModel.h
 * @date 2016/09/12 16:39:59
 * @author fang_yicheng
 * @brief モデル型エフェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __EFFECT_MODEL_H_INCLUDED__
#define __EFFECT_MODEL_H_INCLUDED__
#pragma once

#include "EffectBase.h"
#include "../MantainSurfRenderingPipeline.h"

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryCollisionMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_AnimationResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Effect)

class EffectModel : public EffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectModel);

public:

  EffectModel();
  virtual ~EffectModel();

  void Initialize(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::Effect::GFHeap* pEffectHeap, MantainSurfRenderingPipeline* pRenderingPipeline);

  void CreateModelNode(void* pResourceData);
  void CreateAnimeNode(void* pResourceData);

  virtual void Terminate();
  virtual bool Start();
  virtual void End();
  virtual void Kill();
  virtual void SetVisible(bool visible);

  virtual void Stop();
  virtual void Resume();

  virtual bool IsAlive();
  virtual void UpdatePosition(const gfl2::math::SRT &srt);
  
private:

  // Inputパラメータ
  gfl2::gfx::IGLAllocator* m_pGfxAllocator;
  gfl2::Effect::GFHeap*    m_pEffectHeap;
  MantainSurfRenderingPipeline* m_pRenderingPipeline;

  // 管理パラメータ
  poke_3d::model::BaseModel*                                 m_pModel;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pModelResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pAnimeResourceNode;

  bool m_isStop;

};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __EFFECT_MODEL_H_INCLUDED__
