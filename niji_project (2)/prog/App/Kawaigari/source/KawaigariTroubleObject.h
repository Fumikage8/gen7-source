//======================================================================
/**
 * @file	KawaigariPokemon.h
 * @brief	Kawaigari Trouble Object (Base Class for Dirt, Hair, etc)
 * @author	PETE
 * @data	2015.1.21
 */
//======================================================================

#if !defined __KAWAIGARI_TROUBLE_OBJECT_H_INCLUDED__
#define __KAWAIGARI_TROUBLE_OBJECT_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>

class PokeTool::PokeModel;

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

class KawaigariMain;
class KawaigariObject_Effect;

enum
{
  AI_TROUBLE_STATE_INIT= 0,
  AI_TROUBLE_STATE_MAIN,
  AI_TROUBLE_STATE_IDLE,
  AI_TROUBLE_STATE_END,
  AI_TROUBLE_STATE_CLEANUP,
  AI_TROUBLE_STATE_FINISH,
};

class KawaigariTroubleObject
{

public:
  KawaigariTroubleObject(void);
  virtual ~KawaigariTroubleObject(void);
  void SetCoreSys(KawaigariMain *core) { coreSys = core; }
  void SetPokeModel(PokeTool::PokeModel *model) { mModel = model; }

  void Update(void);

  void Setup(int index,bool isStatusEffect=false);

  void DoFinishEffect(void);

  virtual bool CheckDistance(float dist);
  virtual int GetFixType(void) { return(CARETOOL_NONE); }
  
  // State Functions:
  int GetState(void) { return(mStateReg); }
  int GetSubState(void) { return(mStateRegSub); }
  void SetState(int s);
  void SetSubState(int s);
  void SaveState(void);
  void RestoreState(void);
  void PrintState(void);

  void Scrub(void) { mScrub = true; }
  bool ScrubFinished(void) { return(GetScale()<=0.01f); }
  bool IsFinished(void) { return(mStateReg==AI_TROUBLE_STATE_END);}
  
  void GetPosition(gfl2::math::Vector3 &pos);
  int GetIndex(void) { return(mIndex); }

  // Main States:
  virtual void State_Init(void);
  virtual void State_Main(void) {}  
  virtual void State_End(void) {}
  virtual void State_Cleanup(void) {}
  virtual void State_Finish(void) {}

  // Idle States:
  virtual void State_Idle(void) {}

  float GetFinalScale(void);
  float GetScale(void) { return(mBaseScale); }

  void SetScale(float f) { mBaseScale=f; }
  
  KawaigariObject_Effect *mEffect;

  bool IsStatusEffect(void) { return(mIsStatusEffect); }

  bool IsSleepyStatus(void) { return(GetEffectNum()==KAWAIGARI_EFFECT_SLEEPY); }
  bool IsFreezeStatus(void) { return(GetEffectNum()==KAWAIGARI_EFFECT_ICE); }

  bool PlayFinishSound(void) { return(mPlayFinishSound); }

protected:
  void ConvertWorldPointToScreenPoint(f32 &resultX,f32 &resultY,gfl2::math::Vector3 world_point);
  virtual int GetEffectNum(void) { return(0); }

  void UpdateAgain(void) { mRequireUpdate = true; }
  bool mRequireUpdate;

  bool mSetupFinished;
  float mStartScale,mBaseScale;
  PokeTool::PokeModel *mModel;
  KawaigariMain *coreSys;
  

  // Attached Node & Index:
  int mNode,mIndex,mEffectID;

  // State:  
  int mStateReg,mStateRegSub,mStateRegs[MAX_STATE_REGS];
  int mStateRegBackup,mStateRegSubBackup,mStateRegsBackup[MAX_STATE_REGS];
 
  bool mScrub,mShake;
  bool mIsStatusEffect;
  bool mPlayFinishSound;

  gfl2::math::Vector3 mFinalPosition,mFinalOffset;
};

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif // __KAWAIGARI_TROUBLE_OBJECT_H_INCLUDED__
