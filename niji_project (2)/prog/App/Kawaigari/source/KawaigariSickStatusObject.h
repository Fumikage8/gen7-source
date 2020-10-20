//======================================================================
/**
 * @file	KawaigariSickStatusObject.h
 * @brief	Kawaigari Sick Status Object (Fire, Poison, Ice, etc)
 * @author	PETE
 * @data	2015.2.19
 */
//======================================================================

#if !defined __KAWAIGARI_SICKSTATUS_OBJECT_H_INCLUDED__
#define __KAWAIGARI_SICKSTATUS_OBJECT_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>

class PokeTool::PokeModel;

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

class KawaigariMain;
class KawaigariObject_Effect;

class KawaigariSickStatusObject : public KawaigariTroubleObject
{

public:
  KawaigariSickStatusObject(int effectNum);
  ~KawaigariSickStatusObject(void);
      
  // Main States:  
  void State_Main(void);  
  void State_End(void);
  void State_Cleanup(void);
  void State_Finish(void);
  void State_Idle(void);

  virtual int GetFixType(void) { return(CARETOOL_BANDAGE); }
  

protected:
  virtual int GetEffectNum(void);

  int mEffectNum;

};

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif // __KAWAIGARI_SICKSTATUS_OBJECT_H_INCLUDED__
