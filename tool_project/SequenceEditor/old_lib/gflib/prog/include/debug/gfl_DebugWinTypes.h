//======================================================================
/**
 * @file  gfl_DebugwinTypes.h
 * @brief デバッグウィンドウ タイプ定義
 * @author  ariizumi
 * @date  12/11/08
 */
//======================================================================
#if !defined __GFL_DEBUGWIN_TYPE_H__
#define __GFL_DEBUGWIN_TYPE_H__

#pragma once

//有効無効定義
#if GFL_DEBUG
#define GFL_DEBUGWIN_USE 0
#else
#define GFL_DEBUGWIN_USE 0
#endif

namespace gfl
{

namespace debug
{
  //効果音再生用の定義
  typedef enum
  {
    SE_CURSOR_MOVE,
    SE_CHANGE_VALUE,
    SE_GROUP_IN,
    SE_GROUP_OUT,
    SE_ACCEPT,
    SE_CANCEL,
    SE_ERROR,
    SE_WINDOW_OPEN,
    SE_WINDOW_CLOSE,
  }DEBUGWIN_SOUND_TYPE;


}//namespace debug
}//namespace gfl


#endif  //__GFL_DEBUGWIN_TYPE_H__
