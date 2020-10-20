#if !defined( __FINDER_STUDIO_DECORATION_FRAME_MANAGER_H_INCLUDED__ )
#define __FINDER_STUDIO_DECORATION_FRAME_MANAGER_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioDecorationFrameManager.h
 * @date    2016/11/22 17:01:52
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：フレームマネージャ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDecorationFrameDefine.h"

#include <AppLib/include/Frame/AppFrameCellSuper.h>
#include <AppLib/include/Frame/AppFrameManager.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )
class ViewList;
struct AlbumWork;
GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
struct DecoWork;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///
/// @brief    フレームマネージャ
///
//////////////////////////////////////////////////////////////////////////
class FrameManager
  : public applib::frame::Manager
{
  GFL_FORBID_COPY_AND_ASSIGN( FrameManager );

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  FrameManager( gfl2::heap::HeapBase* heap, Static::ViewList* list, Static::AlbumWork* work, DecoWork* decoWork );
  virtual ~FrameManager( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   フレーム
  //-------------------------------------------------------
  void StartFrame( Frame::FrameID id );
  bool NextFrame( applib::frame::MainStatus status );

  //-------------------------------------------------------
  // @brief   設定関数群
  //-------------------------------------------------------
  void SetResult( Frame::Result result ) { m_FrameResult = result; }

private:
  void CallFrame( Frame::FrameID id );
  template<class CallClass> void _CallFrame( void );

private:
  Static::ViewList*   m_pViewList;
  Static::AlbumWork*  m_pWork;
  DecoWork*           m_pDecoWork;

  Frame::FrameID      m_FrameID;
  Frame::Result       m_FrameResult;
};

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_DECORATION_FRAME_MANAGER_H_INCLUDED__
