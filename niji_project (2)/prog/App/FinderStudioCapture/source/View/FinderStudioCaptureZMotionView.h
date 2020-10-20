#if !defined( __FINDER_STUDIO_CAPTURE_ZMOTION_VIEW_H_INCLUDED__ )
#define __FINDER_STUDIO_CAPTURE_ZMOTION_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureZMotionView.h
 * @date    2016/11/28 14:58:39
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：Zモーション選択View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureViewBase.h"

#include <App/FinderStudioStatic/include/FinderStudioViewerDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioMessageMenuView.h>
#include <App/FinderStudioCapture/source/FinderStudioCaptureZMotionData.h>

#include <AppLib/include/Tool/app_tool_CursorManager.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    Zモーション選択View
///
//////////////////////////////////////////////////////////////////////////
class ZMotionView
  : public CaptureViewBase
  , public Static::MessageMenuViewListener
{
  GFL_FORBID_COPY_AND_ASSIGN( ZMotionView ); //コピーコンストラクタ＋代入禁止

public:
  enum {
    RES_CONTINUE,
    RES_CLOSE,
    RES_CALL_MSGWND,
    RES_MSGWND_CLOSE,
    RES_NUM,
  };
private:
  enum Sequence
  {
    SEQ_IDLE,
    SEQ_SHOW_MOTION_RESET,
    SEQ_SHOW_MOTION_RESET_END,
    SEQ_MAIN,
    SEQ_HIDE_MOTION_RESET,
    SEQ_HIDE_MOTION_RESET_END,
    SEQ_NUM,
  };

  enum {
    BTN_ID_MOTION_00,
    BTN_ID_MOTION_01,
    BTN_ID_MOTION_02,
    BTN_ID_MOTION_03,
    BTN_ID_MOTION_04,
    BTN_ID_MOTION_05,
    BTN_ID_MOTION_06,
    BTN_ID_MOTION_07,
    BTN_ID_MOTION_08,
    BTN_ID_MOTION_09,
    BTN_ID_MOTION_10,
    BTN_ID_MOTION_11,
    BTN_ID_MOTION_12,
    BTN_ID_MOTION_13,
    BTN_ID_MOTION_14,
    BTN_ID_MOTION_15,
    BTN_ID_MOTION_16,
    BTN_ID_MOTION_17,
    BTN_ID_MOTION_18,
    BTN_ID_MOTION_19,
    BTN_ID_BACK,
    BTN_ID_DECIDE,
    BTN_ID_NUM,

    BTN_MTX_COL = 5,
    BTN_MTX_ROW = 4,

    MOTION_NUM = PlayerMotionData::PLAYER_MOTION_NUM,
  };

  enum Dir
  {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  ZMotionView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );
  virtual ~ZMotionView( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   初期化・終了
  //-------------------------------------------------------
  virtual void Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual bool Terminate( void );

  //-------------------------------------------------------
  // @brief   更新
  //-------------------------------------------------------
  virtual void Update( void );

  //-------------------------------------------------------
  // @brief   表示・非表示
  //-------------------------------------------------------
  virtual void Show( void );
  virtual void Hide( void );

  void StartHideView( void );

private:
  //-------------------------------------------------------
  // @brief   初期化：非公開
  //-------------------------------------------------------
  virtual void Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual void InitPane( void );
  virtual void InitText( void );
  virtual void InitAnim( void );
  virtual void InitButton( void );

  //-------------------------------------------------------
  // @brief   更新：非公開
  //-------------------------------------------------------
  void _UpdateSequence( void );
  void _UpdateTrainerMotion( void );

  //-------------------------------------------------------
  // @brief   モーション：非公開
  //-------------------------------------------------------
  void _InitMotionData( void );

  void _PlayMotion( Viewer::ZMotionType type );
  void _ResetMotion( void );
  bool _IsPlayingMotion( void );
  bool _IsUpdateViewer( void );

  Viewer::ZMotionType _GetMotionType_Index( u32 index );

  //-------------------------------------------------------
  // @brief   カーソル：非公開
  //-------------------------------------------------------
  void _InitCursorMoveData( void );

  u32  _GetNextButtonIndex( Dir dir, u32 nowIndex );
  void _SetMoveData( Dir dir, app::tool::CursorManager::MOVE_DATA* dstData, u32 nextIndex );

  void _InitCursor( void );
  void _InitCursorPos( u32* dstCursorPos );

  void _SetCursor( u32 cursorPos );
  void _SetVisibleCursor( u32 cursorPos, bool visible );

  //-------------------------------------------------------
  // @brief   リスナー関数群：非公開
  //-------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick );

  virtual void OnEvent( const Tool::MessageMenuView::IEventHandler::EventCode event );

private:
  PlayerMotionData::MotionButton    m_ButtonList[MOTION_NUM];

  app::tool::CursorManager*               m_pCursorManager;
  app::tool::CursorManager::MOVE_DATA*    m_pCursorMoveData;

  Viewer::ZMotionType   m_MotionType;
  Viewer::ZMotionType   m_ReqMotionType;

  u8    m_MainSeq;
  u8    m_SubSeq;

  u32   m_ButtonNum;

  gfl2::lyt::LytPane*   m_pCursorPane[MOTION_NUM];
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_ZMOTION_VIEW_H_INCLUDED__
