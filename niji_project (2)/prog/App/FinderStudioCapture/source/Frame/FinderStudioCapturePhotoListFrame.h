#if !defined __FINDER_STUDIO_CAPTURE_PHOTO_LIST_FRAME_H_INCLUDED__
#define __FINDER_STUDIO_CAPTURE_PHOTO_LIST_FRAME_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCapturePhotoListFrame.h
 * @date    2016/11/09 20:38:56
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：写真リストフレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureFrameBase.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class UpperView;
class BGView;
class PhotoListView;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
///
/// @brief    写真リストフレーム
///
//////////////////////////////////////////////////////////////////////////
class PhotoListFrame
  : public FrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( PhotoListFrame );

private:
  enum {
    SEQ_PHOTOLIST_INIT,
    SEQ_PHOTOLIST_MAIN,
    SEQ_CURTAIN_CLOSE,
    SEQ_ERROR_MESSAGE,
    SEQ_END,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  PhotoListFrame( void );
  virtual ~PhotoListFrame( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   継承関数群
  //-------------------------------------------------------
  virtual applib::frame::Result InitFunc( void );
  virtual applib::frame::Result EndFunc( void );
  virtual applib::frame::Result UpdateFunc( void );

private:
  //-------------------------------------------------------
  // @brief   更新：非公開
  //-------------------------------------------------------
  Result _UpdatePhotoListInit( void );
  Result _UpdatePhotoListMain( void );
  Result _UpdateCurtainClose( void );
  Result _UpdateErrorMessage( void );

private:
  UpperView*        m_pUpperView;
  BGView*           m_pBGView;
  PhotoListView*    m_pPhotoListView;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_PHOTO_LIST_FRAME_H_INCLUDED__