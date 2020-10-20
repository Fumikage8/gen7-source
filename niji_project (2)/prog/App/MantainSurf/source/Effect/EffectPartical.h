//======================================================================
/**
 * @file EffectPartical.h
 * @date 2016/09/12 16:39:59
 * @author fang_yicheng
 * @brief パーティクル型エフェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __EFFECT_PARTICAL_H_INCLUDED__
#define __EFFECT_PARTICAL_H_INCLUDED__
#pragma once

#include "EffectBase.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Effect)

class EffectPartical : public EffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectPartical);

public:

  EffectPartical();
  virtual ~EffectPartical();

  void Initialize(u32 resId, gfl2::Effect::System* pEffectSystem, gfl2::Effect::GFHeap* pEffectHeap);

  virtual void Terminate();

  virtual bool Start();
  virtual void End();
  virtual void Kill();
  virtual void SetVisible(bool visible);

  virtual void Stop();
  virtual void Resume();

  virtual bool IsAlive();
  virtual void UpdatePosition(const gfl2::math::SRT &srt);

public:

  gfl2::Effect::Handle* GetHandle();
  
private:

  gfl2::Effect::System*     m_pEffectSystem;
  gfl2::Effect::Handle*     m_pHandle;
  s32                       m_resID;

};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __EFFECT_PARTICAL_H_INCLUDED__
