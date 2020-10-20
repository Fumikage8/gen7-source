#if PM_DEBUG

#if !defined __FINDER_STUDIO_CAPTURE_DEBUG_PHOTO_DIFF_FRAME_H_INCLUDED__
#define __FINDER_STUDIO_CAPTURE_DEBUG_PHOTO_DIFF_FRAME_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureDebugPhotoCompareFrame.h
 * @date    2017/04/13 11:32:36
 * @author  fukushima_yuya
 * @brief   デバッグ機能フレーム：写真差分表示
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioCapture/source/Frame/FinderStudioCaptureFrameBase.h>
#include <App/FinderStudioCapture/source/FinderStudioCaptureViewerManager.h>
#include <App/FinderStudioStatic/include/FinderStudioUtil.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class ViewerManager;
class UpperView;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
///
/// @brief    写真リストフレーム
///
//////////////////////////////////////////////////////////////////////////
class DebugPhotoCompareFrame
  : public FrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( DebugPhotoCompareFrame );

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  DebugPhotoCompareFrame( void );
  virtual ~DebugPhotoCompareFrame( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   継承関数群
  //-------------------------------------------------------
  virtual applib::frame::Result InitFunc( void );
  virtual applib::frame::Result EndFunc( void );
  virtual applib::frame::Result UpdateFunc( void );

private:
  UpperView*    m_pUpperView;

  bool    m_bVisibleImage;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif  // __FINDER_STUDIO_CAPTURE_DEBUG_PHOTO_DIFF_FRAME_H_INCLUDED__

#endif  // PM_DEBUG
