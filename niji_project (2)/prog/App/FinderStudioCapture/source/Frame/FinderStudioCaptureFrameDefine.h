#if !defined( __FINDER_STUDIO_CAPTURE_FRAME_DEFINE_H_INCLUDED__ )
#define __FINDER_STUDIO_CAPTURE_FRAME_DEFINE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureFrameDefine.h
 * @date    2016/11/10 21:00:31
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：フレーム用定義
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioCapture/source/Debug/FinderStudioCaptureDebug.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )
GFL_NAMESPACE_BEGIN( Frame )

//////////////////////////////////////////////////////////////////////////
/// ---[ 定数定義 begin ]---
// フレームID
enum FrameID {
  FRAME_ID_SDCARD_LOAD,
  FRAME_ID_SELECT_DIFFICULTY,
  FRAME_ID_SHOOT_PREPARE,
  FRAME_ID_SHOOT_MAIN,
  FRAME_ID_PHOTO_LIST,
  FRAME_ID_SDCARD_SAVE,
#if PM_DEBUG
  FRAME_ID_DEBUG_AUTOCAP,
  FRAME_ID_DEBUG_PHOTOCOMPARE,
  FRAME_ID_DEBUG_AGING,
#endif
  FRAME_ID_NUM,
  FRAME_ID_NULL = FRAME_ID_NUM,
};


// リザルト
enum Result
{
  RES_PREV,   //!< 前のフレームへ
  RES_NEXT,   //!< 次のフレームへ
  RES_FINISH, //!< 強制終了
  RES_ERROR,  //!< フェイタルエラー
  RES_NUM,
};
/// ---[ 定数定義 end ]---
//////////////////////////////////////////////////////////////////////////

GFL_NAMESPACE_END( Frame )
GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_FRAME_DEFINE_H_INCLUDED__