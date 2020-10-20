//======================================================================
/**
 * @file	KawaigariMameDataManager.h
 * @brief	Kawaigari Mame Data Handling
 * @author	PETE
 * @data	2015.2.18
 */
//======================================================================

#if !defined __KAWAIGARI_MAMEDATAMANAGER_H_INCLUDED__
#define __KAWAIGARI_MAMEDATAMANAGER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>

class PokeTool::PokeModel;

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

 const int NUM_MAME_SLOT = 15;

class KawaigariMain;

class KawaigariMameDataManager
{

public:
  KawaigariMameDataManager(void);
  ~KawaigariMameDataManager(void);
  void Init(void);
  void Finalize(void);

  void SetupMameIcons(void);
  void SetupSelectedMameIcon(int type,int index);
  void RestoreSelectedMameIcon(int index);
  int GetScrollPos(void) { return(scrollPos); }
  bool CanScrollLeft(void) { return(scrollPos>0); }
  bool CanScrollRight(void);
  void ScrollLeft(void) { scrollPos--; SetupMameIcons(); }
  void ScrollRight(void) { scrollPos++; SetupMameIcons(); }

  int GetMameType(int pos);

  void RemoveBean(int type);
  void AddBean(int type);

  void SetCoreSys(KawaigariMain *core) { coreSys = core; }

private:

  KawaigariMain *coreSys;      
  int mameType[NUM_MAME_SLOT];
  u8 mameCount[NUM_MAME_SLOT];
  int scrollPos;

};

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif // __KAWAIGARI_MAMEDATAMANAGER_H_INCLUDED__
