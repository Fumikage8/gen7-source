#if !defined( __FINDER_STUDIO_CAPTURE_FRAME_SEQUENCE_H_INCLUDED__ )
#define __FINDER_STUDIO_CAPTURE_FRAME_SEQUENCE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureFrameSequence.h
 * @date    2016/11/16 15:04:45
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：フレームシーケンス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureFrameDefine.h"

#include <AppLib/include/Frame/AppFrameManager.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    フレームシーケンス
///
//////////////////////////////////////////////////////////////////////////
class FrameSequence
{
public:
  // 次のフレームIDの取得
  static Frame::FrameID GetNextFrameID( Frame::FrameID nowFrameID, Frame::Result result );
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_FRAME_SEQUENCE_H_INCLUDED__
