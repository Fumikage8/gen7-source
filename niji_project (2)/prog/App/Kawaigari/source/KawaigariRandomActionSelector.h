//======================================================================
/**
 * @file	KawaigariRandomActionSelector.h
 * @brief	Kawaigari Random Action Selector Class
 * @author	PETE
 * @data	2015.10.22
 */
//======================================================================

#if !defined __KAWAIGARI_RANDOMACTIONSELECTOR_H_INCLUDED__
#define __KAWAIGARI_RANDOMACTIONSELECTOR_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>


GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

class KawaigariMain;
const int MAX_ACTIONS = 32;

class KawaigariRandomActionSelector
{

public:
  KawaigariRandomActionSelector(void);
  ~KawaigariRandomActionSelector(void);
  void SetCoreSys(KawaigariMain *core) { coreSys = core; }

  void Reset(void);
  bool SelectRandomAction(int &action);
  bool GetSelectedAction(int &action);
  bool GetSelectedMotion(int &motion);

  // Appear Type Commands:
  void AddAppearCom(int com,int motion=-1);

  // Idle Type Commands:
  void AddIdleCom(int com,int motion=-1);
  
private:  
  bool CanUseAppearCommand(int command);
  bool CanUseIdleCommand(int command);
  bool CanUseMotion(int motion);

  KawaigariMain *coreSys;
  
  int comTable[MAX_ACTIONS];
  int motionTable[MAX_ACTIONS];
  int curSelection,numCommands;

};

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif // __KAWAIGARI_RANDOMACTIONSELECTOR_H_INCLUDED__
