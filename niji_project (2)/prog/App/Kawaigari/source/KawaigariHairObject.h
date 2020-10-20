//======================================================================
/**
 * @file	KawaigariHairObject.h
 * @brief	Kawaigari Hair Object
 * @author	PETE
 * @data	2015.1.21
 */
//======================================================================

#if !defined __KAWAIGARI_HAIR_OBJECT_H_INCLUDED__
#define __KAWAIGARI_HAIR_OBJECT_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>

class PokeTool::PokeModel;

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

class KawaigariMain;
class KawaigariObject_Effect;

class KawaigariHairObject : public KawaigariTroubleObject
{

public:
  KawaigariHairObject(void);
  ~KawaigariHairObject(void);
      
  // Main States:  
  void State_Main(void);  
  void State_End(void);
  void State_Cleanup(void);
  void State_Finish(void);
  void State_Idle(void);

  virtual int GetFixType(void) { return(CARETOOL_HAIR); }

protected:
  virtual int GetEffectNum(void) { return(KAWAIGARI_EFFECT_HAIR); }

};

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif // __KAWAIGARI_HAIR_OBJECT_H_INCLUDED__
