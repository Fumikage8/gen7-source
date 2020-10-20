#if !defined(NIJI_PROJECT_DEBUG_STARTMENU_D_TAKEBE_H_INCLUDED)
#define NIJI_PROJECT_DEBUG_STARTMENU_D_TAKEBE_H_INCLUDED
#pragma once

//==============================================================================
/**
 @file    d_takebe.h
 @author  makoto takebe
 @date    2015.05.22
 @brief   竹部テストランチャー
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
 @class     TakebeTestLauncher
 @brief     竹部テストランチャー
 */
//==============================================================================
class TakebeTestLauncher : public debug::launcher::ListData
{
  GFL_FORBID_COPY_AND_ASSIGN(TakebeTestLauncher);

private:
  static const u32 LIST_MAX = 2;
  static const u32 ItemList[LIST_MAX];

public:
  TakebeTestLauncher(void);
  virtual ~TakebeTestLauncher();

  virtual gfl2::fs::ArcFile::ARCDATID GetArcDataID(void);
  virtual const u32 * GetListTable(void);
  virtual u32 GetListTableSize(void);
  
  virtual launcher::ListData::RequestID SelectListener( u32 pos, launcher::LauncherProc * launcher );
};


GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)


#endif  // PM_DEBUG

#endif  // NIJI_PROJECT_DEBUG_STARTMENU_D_KAWADA_H_INCLUDED
