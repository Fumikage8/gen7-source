//======================================================================
/**
 * @file d_movie.h
 * @date 2016/02/22 20:43:23
 * @author nagihashi_toru
 * @brief ムービー用ランチャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG

#if !defined __D_MOVIE_HEADER_H_INCLUDED__
#define __D_MOVIE_HEADER_H_INCLUDED__
#pragma once
#include <Debug/Launcher/include/LauncherListData.h>

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)

/**
 * @class MovieTestLauncher
 * @brief ムービー用ランチャー
 */
class MovieTestLauncher : public debug::launcher::ListData
{
public:
  MovieTestLauncher();
  virtual ~MovieTestLauncher();

  virtual gfl2::fs::ArcFile::ARCDATID GetArcDataID();
  virtual const u32* GetListTable();
  virtual u32 GetListTableSize();

  virtual launcher::ListData::RequestID SelectListener( u32 pos, launcher::LauncherProc* launcher );
};

GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)
#endif // __D_MOVIE_HEADER_H_INCLUDED__

#endif