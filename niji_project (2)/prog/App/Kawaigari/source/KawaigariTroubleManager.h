//======================================================================
/**
 * @file	KawaigariTroubleManager.h
 * @brief	Kawaigari Trouble Manager (Manager for Dirt, Hair, etc)
 * @author	PETE
 * @data	2015.1.21
 */
//======================================================================

#if !defined __KAWAIGARI_TROUBLE_MANAGER_H_INCLUDED__
#define __KAWAIGARI_TROUBLE_MANAGER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>

class PokeTool::PokeModel;

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

enum
{
  CARETROUBLE_NONE = 0,
  CARETROUBLE_HAIR,
  CARETROUBLE_DIRT,
  CARETROUBLE_DUST,
  CARETROUBLE_WATER,
  CARETROUBLE_ICE,
  CARETROUBLE_FIRE,
  CARETROUBLE_POISON,
  CARETROUBLE_PARALYSIS,
  CARETROUBLE_SLEEPY,
};

class KawaigariMain;
class KawaigariTroubleObject;

const int MAX_CAREPROBLEMS = 8;

class KawaigariTroubleManager
{

public:
  KawaigariTroubleManager(void);
  virtual ~KawaigariTroubleManager(void);
  void SetCoreSys(KawaigariMain *core) { coreSys = core; }

  bool HasFreezeStatus(void);
  bool HasSleepyStatus(void);
  
  bool HasObjects(void);

  bool SuccessSoundPlay(void);

  void Update(void);
  void Finalize(void);

  void AddObject(int type,int index=0);

  bool ScrubFinished(void);
  void Scrub(float dist);

  bool CheckDistance(float dist);

  bool HasStatusEffect(void);

  void SetCanUpdate(bool b) { mCanUpdate = b; }

#if PM_DEBUG  
  void TurnOff(void);
  void TurnOn(void);
#endif
   
 // void Scrub(void) { mScrub = true; }
//  bool ScrubFinished(void) { return(GetScale()<=0.01f); }
       
private:    
  KawaigariMain *coreSys;
  KawaigariTroubleObject *objs[MAX_CAREPROBLEMS];
  bool mEnable;
  bool mStatusEffect;
  bool mCanUpdate;

#if PM_DEBUG  
  float scaleBackup[MAX_CAREPROBLEMS];
#endif   

};

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif // __KAWAIGARI_TROUBLE_OBJECT_H_INCLUDED__
