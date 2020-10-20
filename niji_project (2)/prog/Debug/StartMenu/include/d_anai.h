#if !defined(NIJI_PROJECT_DEBUG_STARTMENU_D_ANAI_H_INCLUDED)
#define NIJI_PROJECT_DEBUG_STARTMENU_D_ANAI_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    d_anai.h
 @author  anai_shigeo@gamefreak.co.jp
 @date    2015.07.24
 @brief   穴井テストランチャー
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
 @class     AnaiTestLauncher
 @brief     穴井テストランチャー
 */
//==============================================================================
class AnaiTestLauncher : public debug::launcher::ListData
{
  GFL_FORBID_COPY_AND_ASSIGN(AnaiTestLauncher);

private:
  static const u32 LIST_MAX = msg_d_anai_max;
  static const u32 ItemList[LIST_MAX];

public:
  AnaiTestLauncher(void);
  virtual ~AnaiTestLauncher();

  virtual gfl2::fs::ArcFile::ARCDATID GetArcDataID(void);
  virtual const u32 * GetListTable(void);
  virtual u32 GetListTableSize(void);
  
  virtual launcher::ListData::RequestID SelectListener( u32 pos, launcher::LauncherProc * launcher );
};


GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)


#endif  // PM_DEBUG

#endif  // NIJI_PROJECT_DEBUG_STARTMENU_D_ANAI_H_INCLUDED
