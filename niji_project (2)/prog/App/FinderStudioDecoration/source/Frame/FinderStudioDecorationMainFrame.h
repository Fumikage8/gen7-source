#if !defined __FINDER_STUDIO_DECORATION_SDCARD_MAIN_FRAME_H_INCLUDED__
#define __FINDER_STUDIO_DECORATION_SDCARD_MAIN_FRAME_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioDecorationMainFrame.h
 * @date    2017/01/31 15:43:12
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：編集(デコ)メインフレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDecorationFrameBase.h"
#include "../View/FinderStudioDecorationViewDefine.h"

#include <App/FinderStudioStatic/include/FinderStudioMessageMenuView.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )
class ViewBase;
class ImageDBUtil;
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
class UpperView;
class LowerMainView;
class FrameListView;
class StampListView;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///
/// @brief    編集(デコ)メインフレーム
///
//////////////////////////////////////////////////////////////////////////
class DecorationMainFrame
  : public FrameBase
  , public Static::MessageMenuViewListener
{
  GFL_FORBID_COPY_AND_ASSIGN( DecorationMainFrame );

private:
  enum {
    INIT_SEQ_VIEW,
    INIT_SEQ_PHOTO,
    INIT_SEQ_SHOW,
    INIT_SEQ_NUM,
  };
  enum {
    SEQ_CAUTION,
    SEQ_MAIN,
    SEQ_LIST_STAMP_S,
    SEQ_LIST_STAMP_M,
    SEQ_LIST_STAMP_L,
    SEQ_LIST_TEXT,
    SEQ_LIST_FRAME,
    SEQ_CAPTURE,
    SEQ_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  DecorationMainFrame( void );
  virtual ~DecorationMainFrame( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   継承関数群
  //-------------------------------------------------------
  virtual applib::frame::Result InitFunc( void );
  virtual applib::frame::Result EndFunc( void );
  virtual applib::frame::Result UpdateFunc( void );

private:
  //-------------------------------------------------------
  // @brief   初期化：非公開
  //-------------------------------------------------------
  bool _InitFunc_View( void );
  bool _InitFunc_Photo( void );
  bool _InitFunc_Show( void );

  //-------------------------------------------------------
  // @brief   更新：非公開
  //-------------------------------------------------------
  Result _UpdateCaution( void );
  Result _UpdateMain( void );
  Result _UpdateFrameList( void );
  Result _UpdateStampList( void );
  Result _UpdateCapture( void );

  //-------------------------------------------------------
  // @brief   LowerMainViewリザルト：非公開
  //-------------------------------------------------------
  Result _SetResult_StampList( u32 result );
  Result _SetResult_MsgWnd( u32 result );

  //-------------------------------------------------------
  // @brief   メッセージウィンドウ：非公開
  //-------------------------------------------------------
  void _ShowMsgWnd( MsgWndID id );
  void _ShowMsgWnd_Caution( MsgWndID id );
  void _ShowMsgWnd_OverStamp( MsgWndID id );
  void _ShowMsgWnd_FinMenu( MsgWndID id );
  void _ShowMsgWnd_Confirm( MsgWndID id );

  void _HideMsgWnd( void );

  //-------------------------------------------------------
  // @brief   設定・取得関数：非公開
  //-------------------------------------------------------
  void _SetMsgWndID( MsgWndID id );

  u32 _GetViewResult( Static::ViewBase* view );

  //-------------------------------------------------------
  // @brief   チェック関数：非公開
  //-------------------------------------------------------
  bool _IsPhotoMax( void ) const;
  bool _IsDecoration( void ) const;

  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  virtual void OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event );

private:
  static const u8   CAPTURE_WAIT_FRAME = 5;

private:
  UpperView*        m_pUpperView;
  LowerMainView*    m_pLowerMainView;
  FrameListView*    m_pFrameListView;
  StampListView*    m_pStampListView;

  Static::ImageDBUtil*    m_pImageDBUtil;

  MsgWndID    m_MsgWndID;

  u8    m_FrameWait;
};

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_DECORATION_SDCARD_MAIN_FRAME_H_INCLUDED__
