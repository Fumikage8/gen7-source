//======================================================================
/**
 * @file d_movie.cpp
 * @date 2016/02/22 20:42:50
 * @author nagihashi_toru
 * @brief ムービー用ランチャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG
#include <Debug/StartMenu/include/d_movie.h>
#include <Debug/StartMenu/include/StartLauncher.h>
#include <macro/include/gfl2_Macros.h>
#include <App/AppEvent/include/Movie/MovieAppParam.h>
#include <GameSys/include/DllProc.h>
#include <niji_conv_header/message/debug/msg_d_movie.h>
#include <arc_index/debug_message.gaix>
#include <Movie/MovieLib/source/MovieProc.h>


#include <ro/include/gfl2_RoManager.h>

#include <System/include/HeapDefine.h>
#include <GameSys/include/DllProc.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameProcManager.h>

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)


namespace {
static const u32 ItemList[] =
{
  d_movie_0,
  d_movie_1,
  d_movie_2,
  d_movie_3,
  d_movie_4,
  d_movie_5,
  d_movie_6,
  d_movie_7,
  d_movie_8,
};

static const int LIST_MAX = GFL_NELEMS(ItemList);
}

MovieTestLauncher::MovieTestLauncher()
{
}

MovieTestLauncher::~MovieTestLauncher()
{
}

gfl2::fs::ArcFile::ARCDATID MovieTestLauncher::GetArcDataID()
{
  return GARC_debug_message_d_movie_DAT;
}

const u32* MovieTestLauncher::GetListTable()
{
  return ItemList;
}

u32 MovieTestLauncher::GetListTableSize()
{
  return LIST_MAX;
}

launcher::ListData::RequestID MovieTestLauncher::SelectListener( u32 pos, launcher::LauncherProc* launcher)
{
  // キャンセル
  if( pos == debug::launcher::LauncherProc::CURSOR_POS_CANCEL )
  {
    launcher->CreateListData<debug::startmenu::StartLauncher>();
    return debug::launcher::ListData::REQID_RESET;
  }

  if( pos < LIST_MAX )
  {
#if defined(GF_PLATFORM_CTR)
    ::movie::MovieProc* proc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, movie::MovieProc>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "MovieLib.cro" );
    proc->SetNo( pos );
    return debug::launcher::ListData::REQID_CALL;
#endif
  }

  return debug::launcher::ListData::REQID_NONE;
}

GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debugg)
#endif
