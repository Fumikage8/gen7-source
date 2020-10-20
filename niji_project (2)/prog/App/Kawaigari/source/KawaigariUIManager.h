//======================================================================
/**
 * @file	KawaigariUIManager.h
 * @brief	Kawaigari All UI Handling (Buttons, Interface, SlideUI)
 * @author	PETE
 * @data	2015.11.02
 */
//======================================================================

#if !defined __KAWAIGARI_UIMANAGER_H_INCLUDED__
#define __KAWAIGARI_UIMANAGER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>

// UI Base:
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"

// Menu Handlers:
#include "KawaigariUIMameMenuBar.h"
#include "KawaigariUITutorialInfo.h"
#include "KawaigariUICareMenuBar.h"

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

class KawaigariMain;

enum
{
  GUI_MODE_NONE = 0,
  GUI_MODE_NORMAL,
  GUI_MODE_MAME,
  GUI_MODE_CARE,
};

enum
{
  GUI_STATE_INIT= 0,
  GUI_STATE_NONE,
  GUI_STATE_MAIN,
  GUI_STATE_MENU_IDLE,
  GUI_STATE_END,
  GUI_STATE_CLEANUP,
  GUI_STATE_FINISH,
};

class KawaigariUIManager : public app::ui::UIView, public app::ui::UIInputListener
{

public:
  KawaigariUIManager(app::util::Heap *heap, app::util::AppRenderingManager *renderManager);
  virtual ~KawaigariUIManager(void);

  void SetCoreSys(KawaigariMain *core) { coreSys = core; }
  KawaigariMain *GetCoreSys(void) { return(coreSys); }
  
  void LoadLayoutResource(void);

  bool Init(void);
  bool End(void);

  virtual void Update(void);
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  bool IsPressedCancelButton(void) { return(mPressedCancelButton); }
  bool IsChangePokemon(void) { return(mChangePokemon); }

  void SetIconPaneVisible(int pane,bool b);
  void StopIconAnimation(int ani);
  void SetIconAnimation(int ani);
  void SetTouchPos(float x,float y);

  void GetTouchPos(float &x,float &y) { x=touchX; y=touchY; }
  void GetTouchRawPos(u32 &x,u32 &y) { x=touchRawX; y=touchRawY; }
  void ConvertRawToTouchPos(int &x,int &y);

  void ResetTouchIcon(void);
  bool IsTouching(void);

  void SetGUIMode(int n);
  int GuiMode(void);

  void ChangeToMainMenu(void);

  void DisableEatModeButton(void);
  void EnableEatModeButton(void);  

  void DisablePokemonSelectButton(void);
  void EnablePokemonSelectButton(void);

  void DisableBackButton(void);
  void EnableBackButton(void);

  void DisableCareButton(void);
  void EnableCareButton(void);

  bool TouchingPane(int layoutIndex,int paneIndex);

  void SetMameButtonLarge(bool b);
  void SetCareButtonNotifyMode(bool b);

  void SetCareTextMessage(int index);

  void DisableMainButtons(void);
  void EnableMainButtons(void);

  app::tool::LytTexReplaceSystem *GetTexSys(void) { return(mTexSys); }  
  gfl2::lyt::LytRes *GetTexInfoRes(void) { return(mTexInfoRes); }

  void SetDryerActive(bool b) { mDryerActive = b; }
  bool DryerActive(void) { return(mDryerActive); }

  app::util::Heap *GetAppHeap(void) { return(mHeap); }

  gfl2::str::MsgData *GetMessage(void) { return(m_pMsgData); }
  print::WordSet *GetWordSet(void) { return(m_pWordSet); }

  void OpenTutorialWindow(void);
  bool IsTutorialOver(void);

  bool IsPressingButton(void);

  void SetHitTouch(int x,int y);

private:
  
  void Initialize2D(void);
  void Terminate2D(void);

  void UpdateUITouchPos(void);
  void SetCursorOffscreen(void);

  void CreateButtons(void);
  void SetButtonInfoEx( app::ui::ButtonInfoEx * info, const gfl2::lyt::LytPaneIndex * parts_tbl, const app::ui::ButtonInfoEx * info_tbl, gfl2::lyt::LytPaneIndex bound, u32 max );
  void CreateExitButton( app::ui::ButtonInfoEx * info );
  void CreateNormalButton( app::ui::ButtonInfoEx * info );

  void CreateNumFont(void);
  bool SetNumFont(void);
  void DeleteNumFont(void);

  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 button_id );

  KawaigariMain *coreSys;

  app::util::Heap *mHeap;
  app::util::AppRenderingManager * mRenderingManager;
  void *resourceBuf;
  gfl2::str::MsgData * m_pMsgData;
  print::WordSet * m_pWordSet;
  bool mCreateNumFont;
  int mInitSeq;  

  bool mPressedCancelButton;
  bool mChangePokemon;
  

  float touchX,touchY;
  float hitTouchX,hitTouchY;
  float touchReleaseX,touchReleaseY;
  u32 touchRawX,touchRawY;
  float lastUITouchX,lastUITouchY;
  int iconPane;
  int guiMode;
  int mEndSeq;

  // GUI Menu Handlers:
  KawaigariUIMameMenuBar mameMenu;
  KawaigariUICareMenuBar careMenu;
  KawaigariUITutorialInfo tutorialInfo;

  // Tex Swapping:
  app::tool::LytTexReplaceSystem *mTexSys;

  // Tutorial Info Window Pictures:  
  gfl2::lyt::LytRes *mTexInfoRes;

  // Dryer Effect
  bool mDryerActive;
  int mButtonDisableCount;

};

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif // __KAWAIGARI_UIManager_H_INCLUDED__
