#if !defined( __FINDER_STUDIO_DECORATION_FRAME_SEQUENCE_H_INCLUDED__ )
#define __FINDER_STUDIO_DECORATION_FRAME_SEQUENCE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioDecorationFrameSequence.h
 * @date    2017/01/31 15:49:28
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：フレームシーケンス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDecorationFrameDefine.h"

#include <AppLib/include/Frame/AppFrameManager.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

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

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_DECORATION_FRAME_SEQUENCE_H_INCLUDED__
