//======================================================================
/**
 * @file	KawaigariUITutorialInfo.h
 * @brief	Kawaigari Tutorial Info UI Handling (Buttons, Interface)
 * @author	PETE
 * @data	2016.03.17
 */
//======================================================================

#if !defined __KAWAIGARI_UITUTORIALINFO_H_INCLUDED__
#define __KAWAIGARI_UITUTORIALINFO_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>

GFL_NAMESPACE_BEGIN(app)
  GFL_NAMESPACE_BEGIN(tool)
class MenuWindow;
class InfoWindowSheetType;
class InfoWindowBookType;
GFL_NAMESPACE_END(tool)
  GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

class KawaigariUIManager;
class KawaigariInfoWindowBookTypeListener;

class KawaigariUITutorialInfo
{

public:
  KawaigariUITutorialInfo(void);
  ~KawaigariUITutorialInfo(void);

  void SetCoreUISys(KawaigariUIManager *core) { coreUISys = core; }

  void Setup(void);

  // State Functions:
  void SetState(int s);
  void SetSubState(int s);

  // Main States:
  void State_Init(void);
  void State_Main(void);    
  void State_End(void);
  void State_Cleanup(void);
  void State_Finish(void);

  void Finish(void);
  bool IsFinish(void);

  void PrintState(void);
  
  void Update(void);

  app::tool::InfoWindowBookType *GetInfoWindowBookType(void) { return(mInfoWindowBookType); }
    
  bool IsTutorialOver(void) { return(mIsFinishedTutorial); }
  void SetIsTutorialOver(bool b) { mIsFinishedTutorial = b; }

private:
  
  KawaigariUIManager *coreUISys;

  // State:  
  void UpdateAgain(void) { mRequireUpdate = true; }
  

  int mStateReg,mStateRegSub,mStateRegs[MAX_UI_STATE_REGS];
  bool mRequireUpdate;
  bool mInit;

  // InfoWindow:
  app::tool::InfoWindowBookType *mInfoWindowBookType;
  KawaigariInfoWindowBookTypeListener *mInfoWindowBookTypeListener;
  gfl2::math::Vector2 mTouchPos; 
  bool mInfoWindowBookTypeOpenReq;
  bool mInfoWindowBookTypeCloseReq;
    
  app::tool::InfoWindowBookType *mBook;
  KawaigariInfoWindowBookTypeListener *mBookListener;


  static const u32 STR_BUF_NUM          = 4*2;
  static const u32 STR_BUF_CHAR_NUM_MAX = 512;
  gfl2::str::StrBuf*   m_strBuf[STR_BUF_NUM];

  bool mIsFinishedTutorial;

};

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif // __KAWAIGARI_UITUTORIALINFO_H_INCLUDED__
