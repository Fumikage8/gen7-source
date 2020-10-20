//======================================================================
/**
 * @file d_araki.h
 * @date 2015/10/28 16:31:27
 * @author araki_syo
 * @brief 荒木テストランチャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined(NIJI_PROJECT_DEBUG_STARTMENU_D_ARAKI_H_INCLUDED)
#define NIJI_PROJECT_DEBUG_STARTMENU_D_ARAKI_H_INCLUDED
#pragma once

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


/**
 * @class     ArakiTestLauncher
 * @brief     荒木テストランチャー
 */
class ArakiTestLauncher : public debug::launcher::ListData
{
  GFL_FORBID_COPY_AND_ASSIGN(ArakiTestLauncher);

private:
  static const u32 LIST_MAX = 2;
  static const u32 ItemList[LIST_MAX];

public:
  ArakiTestLauncher(void);
  virtual ~ArakiTestLauncher();

  virtual gfl2::fs::ArcFile::ARCDATID GetArcDataID(void);
  virtual const u32 * GetListTable(void);
  virtual u32 GetListTableSize(void);
  
  virtual launcher::ListData::RequestID SelectListener( u32 pos, launcher::LauncherProc * launcher );
};


GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)


#endif  // PM_DEBUG

#endif  // NIJI_PROJECT_DEBUG_STARTMENU_D_ARAKI_H_INCLUDED
