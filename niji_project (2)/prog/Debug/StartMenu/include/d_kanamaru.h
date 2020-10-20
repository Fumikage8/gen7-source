#if !defined(NIJI_PROJECT_DEBUG_STARTMENU_D_KANAMARU_H_INCLUDED)
#define NIJI_PROJECT_DEBUG_STARTMENU_D_KANAMARU_H_INCLUDED
#pragma once

//==============================================================================
/**
@file    d_kanamaru.h
@author  Masanori Kanamaru
@date    2015.06.25
@brief   金丸テストランチャー
*/
//==============================================================================

#if PM_DEBUG
#include <macro/include/gfl2_Macros.h>

#include <Debug/Launcher/include/LauncherListData.h>

namespace debug {
  namespace startmenu {

    //==============================================================================
    /**
     @class     KanamaruTestLauncher
     @brief     金丸テストランチャー
     */
    //==============================================================================
    class KanamaruTestLauncher : public debug::launcher::ListData
    {
      GFL_FORBID_COPY_AND_ASSIGN(KanamaruTestLauncher);
    private:
      static const u32 LIST_MAX = 1;
      static const u32 ItemList[LIST_MAX];

    public:
      KanamaruTestLauncher();
      virtual ~KanamaruTestLauncher();

      virtual gfl2::fs::ArcFile::ARCDATID GetArcDataID();
      virtual const u32* GetListTable();
      virtual u32 GetListTableSize();

      virtual launcher::ListData::RequestID SelectListener( u32 pos, launcher::LauncherProc* launcher );
    };
  } // startmenu
} // debug



#endif // PM_DEBUG

#endif // NIJI_PROJECT_DEBUG_STARTMENU_D_KANAMARU_H_INCLUDED