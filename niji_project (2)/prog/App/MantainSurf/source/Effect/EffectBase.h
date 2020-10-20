//======================================================================
/**
 * @file EffectBase.h
 * @date 2016/09/12 16:39:59
 * @author fang_yicheng
 * @brief ベースエフェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __EFFECT_BASE_H_INCLUDED__
#define __EFFECT_BASE_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <ctype.h>

#include <System/include/HeapDefine.h>
#include <AppLib/include/Util/app_util_heap.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameProcManager.h>

#include <Effect/include/gfl2_EffectSystem.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Effect)

class EffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectBase);

public:

  EffectBase(){};
  virtual ~EffectBase() {};
  
  virtual void Terminate() {};

  virtual bool Start() { return false; };
  virtual void End() {};
  virtual void Kill() {};
  virtual void SetVisible(bool visible) {};

  virtual void Stop() {};
  virtual void Resume() {};

  virtual bool IsAlive() { return false; };
  virtual void UpdatePosition(const gfl2::math::SRT &srt) {};

};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __EFFECT_BASE_H_INCLUDED__
