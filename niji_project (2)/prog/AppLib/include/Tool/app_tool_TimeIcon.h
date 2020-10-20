//=============================================================================
/**
 * @file    app_tool_TimeIcon.h
 * @brief   タイマーアイコン
 * @author  Hiroyuki Nakamura
 * @date    12.04.18
 */
//=============================================================================
#if !defined( APP_TOOL_TIMEICON_H_INCLUDED )
#define APP_TOOL_TIMEICON_H_INCLUDED

#pragma once

// gflib2
#include <Layout/include/gfl2_Layout.h>
// niji
#include "GameSys/include/GameFrameMode.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

//=============================================================================
/**
 * @class TimeIcon
 * @brief タイマーアイコン
 * @date  12.04.18
 */
//=============================================================================
class TimeIcon
{
public:
  //-----------------------------------------------------------------------------
  /**
   * @func    StartAnime
   * @brief   アニメ再生
   * @date    2015.06.25
   *
   * @param   lytwk   レイアウトワーク
   * @param   anime   アニメ番号
   * @param   frame   フレームレート　※デフォルト = FRAMEMODE_DEFAULT
   */
  //-----------------------------------------------------------------------------
  static void StartAnime( gfl2::lyt::LytWk * lytwk, u32 anime, GameSys::FrameMode frame=GameSys::FRAMEMODE_DEFAULT );
};


GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

#endif  // APP_TOOL_TIMEICON_H_INCLUDED
