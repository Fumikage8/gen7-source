//======================================================================
/**
 * @file	KawaigariUIMameMenuBar.h
 * @brief	Kawaigari Mame Menu Bar UI Handling (Buttons, Interface, SlideUI)
 * @author	PETE
 * @data	2015.12.03
 */
//======================================================================

#if !defined __KAWAIGARI_UIMAMEMENUBAR_H_INCLUDED__
#define __KAWAIGARI_UIMAMEMENUBAR_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

class KawaigariUIManager;

const int MAX_UI_STATE_REGS = 8;
const int MAX_MAME_ICON = 7;

class KawaigariUIMameMenuBar
{

public:
  KawaigariUIMameMenuBar(void);
  ~KawaigariUIMameMenuBar(void);

  void SetCoreUISys(KawaigariUIManager *core) { coreUISys = core; }


  // State Functions:
  void SetState(int s);
  void SetSubState(int s);

  // Main States:
  void State_Init(void);
  void State_Main(void);  
  void State_Menu_Idle(void);
  void State_End(void);
  void State_Cleanup(void);
  void State_Finish(void);

  void PrintState(void);
  
  void Update(void);

  void SaveOriginalPositions(void);
  void RestoreOriginalPositions(void);

private:
  
  KawaigariUIManager *coreUISys;

  // State:  
  void UpdateAgain(void) { mRequireUpdate = true; }
  
  bool IsTouchSlide(int speed);
  bool IsTouchSlideY(int speed);

  int mStateReg,mStateRegSub,mStateRegs[MAX_UI_STATE_REGS];
  bool mRequireUpdate;

  int lastTouchX,lastTouchY;
  int touchX,touchY;
  bool mTouching;

  gfl2::math::VEC3 posBaseRoot; // Parent Root Pos for Mame Buttons
  gfl2::math::VEC3 mameBasePos[MAX_MAME_ICON]; // Original Positions of the Mame Buttons
    
};

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif // __KAWAIGARI_UIMAMEMENUBAR_H_INCLUDED__
