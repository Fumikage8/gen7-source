#if !defined(NIJI_PROJECT_DEBUG_STARTMENU_D_KAWADA_H_INCLUDED)
#define NIJI_PROJECT_DEBUG_STARTMENU_D_KAWADA_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    d_kawada.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.04.22
 @brief   川田テストランチャー
 */
//==============================================================================


#if PM_DEBUG


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/Launcher/include/LauncherListData.h>


GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)


//==============================================================================
/**
 @class     KawadaTestLauncher
 @brief     川田テストランチャー
 */
//==============================================================================
class KawadaTestLauncher : public debug::launcher::ListData
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestLauncher);

private:
  static const u32 LIST_MAX = 11;
  static const u32 ItemList[LIST_MAX];

public:
  KawadaTestLauncher(void);
  virtual ~KawadaTestLauncher();

  virtual gfl2::fs::ArcFile::ARCDATID GetArcDataID(void);
  virtual const u32 * GetListTable(void);
  virtual u32 GetListTableSize(void);
  
  virtual launcher::ListData::RequestID SelectListener( u32 pos, launcher::LauncherProc * launcher );
};


GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)


#endif  // PM_DEBUG

#endif  // NIJI_PROJECT_DEBUG_STARTMENU_D_KAWADA_H_INCLUDED
