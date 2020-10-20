#if !defined( __FINDER_STUDIO_CAPTURE_FRAME_MANAGER_H_INCLUDED__ )
#define __FINDER_STUDIO_CAPTURE_FRAME_MANAGER_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureFrameManager.h
 * @date    2016/11/09 19:02:05
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：フレームマネージャ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureFrameDefine.h"

#include <AppLib/include/Frame/AppFrameCellSuper.h>
#include <AppLib/include/Frame/AppFrameManager.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )
class ViewList;
GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
struct Work;
class  ViewerManager;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///
/// @brief    フレームリスナー
///
//////////////////////////////////////////////////////////////////////////
class FrameListener
{
public:
  /// Viewerの表示
  virtual void ShowViewer( void ) { ; }
  /// Viewerの非表示
  virtual void HideViewer( void ) { ; }
};



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
  /// メインワーク
  struct MainWork
  {
    app::util::Heap*  heap;
    Work*             work;
    Static::ViewList* viewList;
    ViewerManager*    viewer;

    MainWork( void )
      : heap(NULL), work(NULL), viewList(NULL), viewer(NULL)
    {
      ;
    }
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  FrameManager( gfl2::heap::HeapBase* heap, app::util::Heap* appHeap, Work* work, Static::ViewList* list, ViewerManager* viewer );
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
  void SetFrameListener( FrameListener* listener ) { m_pFrameListener = listener; }
  void SetResult( Frame::Result result ) { m_FrameResult = result; }

private:
  //-------------------------------------------------------
  // @brief   コール関数
  //-------------------------------------------------------
  void CallFrame( Frame::FrameID id );
  template<class CallClass> void _CallFrame( void );

private:
  FrameListener*      m_pFrameListener;

  MainWork            m_MainWork;

  Frame::FrameID      m_FrameID;
  Frame::Result       m_FrameResult;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_FRAME_MANAGER_H_INCLUDED__
