#if PM_DEBUG
//======================================================================
/**
 * @file d_nagi.h
 * @date 2016/02/26 18:20:01
 * @author nagihashi_toru
 * @brief なぎはしスタートメニュー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __D_NAGI_HEADER_H_INCLUDED__
#define __D_NAGI_HEADER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <Debug/Launcher/include/LauncherListData.h>

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)

class NagiTestLauncher : public debug::launcher::ListData
{
  GFL_FORBID_COPY_AND_ASSIGN(NagiTestLauncher);

public:
  NagiTestLauncher();
  virtual ~NagiTestLauncher();

  virtual gfl2::fs::ArcFile::ARCDATID GetArcDataID();
  virtual const u32* GetListTable();
  virtual u32 GetListTableSize();

  virtual launcher::ListData::RequestID SelectListener( u32 pos, launcher::LauncherProc* launcher );
};

GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)

#endif // __D_NAGI_HEADER_H_INCLUDED__
#endif //PM_DEBUG