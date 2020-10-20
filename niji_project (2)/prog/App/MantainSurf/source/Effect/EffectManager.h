//======================================================================
/**
 * @file EffectManager.h
 * @date 2016/09/12 16:39:59
 * @author fang_yicheng
 * @brief エフェクトマネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __EFFECT_MANAGER_H_INCLUDED__
#define __EFFECT_MANAGER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <ctype.h>

#include <System/include/HeapDefine.h>
#include <AppLib/include/Util/app_util_heap.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameProcManager.h>

#include <Effect/include/gfl2_EffectSystem.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/mantain_effect.gaix>

#include "../MantainSurfRenderingPipeline.h"
#include "../Resource/ResourceManager.h"
#include "EffectBase.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
class MantainSurfAppManager;
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Effect)

class EffectManager
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectManager);

public:

  struct SetupParam
  {
    gfl2::gfx::IGLAllocator*      m_pGfxAllocator;
    gfl2::Effect::GFHeap*         m_pEffectHeap;
    gfl2::Effect::System*         m_pEffectSystem;
    Resource::ResourceManager*    m_pResourceManager;
    MantainSurfRenderingPipeline* m_pRenderingPipeline;
    MantainSurfAppManager*        m_pAppManager;

    SetupParam() :
      m_pGfxAllocator(NULL),
      m_pEffectHeap(NULL),
      m_pResourceManager(NULL),
      m_pEffectSystem(NULL),
      m_pRenderingPipeline(NULL),
      m_pAppManager(NULL)
    {
    }
  };

public:

  EffectManager();
  virtual ~EffectManager();

  void Initialize(const SetupParam &setupParam);
  void Terminate();

  void Update();

  EffectBase* CreateEffect(s32 resId);

  bool IsParticalEffect(s32 resId);

#if PM_DEBUG
  void UpdateForDebug(gfl2::math::SRT targetSrt);
  void StopEffectForDebug();
#endif

private:

  enum EFFECT_TYPE
  {
    EFFECT_NONE = -1,
    EFFECT_PARTICAL,
    EFFECT_MODEL,

    EFFECT_MAX,
  };

private:

  EFFECT_TYPE GetEffectType(s32 resId);
  s32         GetMotionResId(s32 resId);

private:

  EffectBase*     m_pEffectForDebug;
  EffectBase*     m_pEffectLateRelease; // 遅延開放の対象

  SetupParam      m_SetupParam;

};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __BASEEFFECT_H_INCLUDED__
