//======================================================================
/**
 * @file	KawaigariUICareMenuBar.h
 * @brief	Kawaigari Care Menu Bar UI Handling (Buttons, Interface, SlideUI)
 * @author	PETE
 * @data	2015.12.03
 */
//======================================================================

#if !defined __KAWAIGARI_UICAREMENUBAR_H_INCLUDED__
#define __KAWAIGARI_UICAREMENUBAR_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

class KawaigariUIManager;

const int MAX_CARE_ICON = 5;

class KawaigariUICareMenuBar
{

public:
  KawaigariUICareMenuBar(void);
  ~KawaigariUICareMenuBar(void);

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

  void CareToolFadeEffect(void);
  
private:

  void TurnOnDryer(void);
  void TurnOffDryer(void);
  
  KawaigariUIManager *coreUISys;

  // State:  
  void UpdateAgain(void) { mRequireUpdate = true; }

  int mStateReg,mStateRegSub,mStateRegs[MAX_UI_STATE_REGS];
  bool mRequireUpdate;

  gfl2::math::VEC3 posBaseRoot; // Parent Root Pos for Care Buttons
  gfl2::math::VEC3 careBasePos[MAX_CARE_ICON]; // Original Positions of the Care Buttons

};

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif // __KAWAIGARI_UICAREMENUBAR_H_INCLUDED__
