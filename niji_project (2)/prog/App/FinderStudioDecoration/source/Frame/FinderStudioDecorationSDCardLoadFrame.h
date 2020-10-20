#if !defined __FINDER_STUDIO_DECORATION_SDCARD_LOAD_FRAME_H_INCLUDED__
#define __FINDER_STUDIO_DECORATION_SDCARD_LOAD_FRAME_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioDecorationSDCardLoadFrame.h
 * @date    2017/01/31 15:16:38
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：SDカード読込みフレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDecorationFrameBase.h"

#include <App/FinderStudioStatic/include/FinderStudioMessageMenuView.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( ExtSavedata )
class FinderStudioExtSaveData;
GFL_NAMESPACE_END( ExtSavedata )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class UpperView;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///
/// @brief    SDカード読込みフレーム
///
//////////////////////////////////////////////////////////////////////////
class SDCardLoadFrame
  : public FrameBase
  , public Static::MessageMenuViewListener
{
  GFL_FORBID_COPY_AND_ASSIGN( SDCardLoadFrame );

private:
  enum {
    SEQ_FRAME_COUNT,
    SEQ_CHECK_IMAGEDB,
    SEQ_CHECK_EXTSAVEDATA,
    SEQ_END,
    SEQ_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  SDCardLoadFrame( void );
  virtual ~SDCardLoadFrame( void ) { ; }

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
  applib::frame::Result _UpdateFrameCount( void );
  applib::frame::Result _UpdateCheckImageDB( void );
  applib::frame::Result _UpdateCheckExtSaveData( void );

private:
  //-------------------------------------------------------
  // @brief   リスナー関数：非公開
  //-------------------------------------------------------
  virtual void OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event );

private:
  ExtSavedata::FinderStudioExtSaveData*    m_pExtSaveData;

  UpperView*        m_pUpperView;

  u32   m_FrameCount;
};

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_DECORATION_SDCARD_LOAD_FRAME_H_INCLUDED__
