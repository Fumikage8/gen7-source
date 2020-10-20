#if !defined __FINDER_STUDIO_CAPTURE_CAMERA_VIEW_H_INCLUDED__
#define __FINDER_STUDIO_CAPTURE_CAMERA_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureCameraView.h
 * @date    2016/11/28 16:09:39
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：カメラ操作View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureViewBase.h"

#include <App/FinderStudioStatic/include/FinderStudioInformationWindow.h>

#include <math/include/gfl2_math.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class ScrollBar;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class SlideScrollBar;
class ShootCommonView;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
///
/// @brief    カメラ操作View
///
//////////////////////////////////////////////////////////////////////////
class CameraView
  : public CaptureViewBase
  , public app::tool::InfoWindowBookTypeListener
{
  GFL_FORBID_COPY_AND_ASSIGN( CameraView ); //コピーコンストラクタ＋代入禁止

private:
  enum {
    BTN_ID_INFO,
    BTN_ID_ROTATE_X_UP,
    BTN_ID_ROTATE_X_DOWN,
    BTN_ID_ROTATE_Y_LEFT,
    BTN_ID_ROTATE_Y_RIGHT,
    BTN_ID_ZOOM_PLUS,
    BTN_ID_ZOOM_MINUS,
    BTN_ID_HEIGHT_PLUS,
    BTN_ID_HEIGHT_MINUS,
    BTN_ID_FORCUS_TRAINER,
    BTN_ID_FORCUS_POKEMON,
    BTN_ID_NUM,
  };

  enum {
    SCROLLBAR_ID_ZOOM,
    SCROLLBAR_ID_HEIGHT,
    SCROLLBAR_ID_FORCUS,
    SCROLLBAR_ID_NUM,
  };

  enum {
    CAMERA_ROTATE_AXIS_X_UP,
    CAMERA_ROTATE_AXIS_X_DOWN,
    CAMERA_ROTATE_AXIS_Y_LEFT,
    CAMERA_ROTATE_AXIS_Y_RIGHT,
    CAMERA_ROTATE_NUM,

    CAMERA_ROTATE_NULL = CAMERA_ROTATE_NUM,
  };

public:
  enum {
    RES_CONTINUE,
    RES_INFO_SHOW,
    RES_INFO_HIDE,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  CameraView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );
  virtual ~CameraView( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   初期化・終了
  //-------------------------------------------------------
  virtual void Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual bool Terminate( void );

  //-------------------------------------------------------
  // @brief   表示・非表示
  //-------------------------------------------------------
  virtual void Show( void );
  virtual void Hide( void );

  void ShowTutorial( void ) { _ShowTutorial(); }

  //-------------------------------------------------------
  // @brief   設定
  //-------------------------------------------------------
  void SetWork( Work* work ) { m_pWork = work; }
  void SetViewListener( ViewListener* listener ) { m_pListener = listener; }
  void SetShootCommonView( ShootCommonView* view ) { m_pCommonView = view; }

private:
  //-------------------------------------------------------
  // @brief   初期化：非公開
  //-------------------------------------------------------
  virtual void Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual void InitPane( void );
  virtual void InitAnim( void );
  virtual void InitButton( void );

  void _InitScrollBar( void );

  //-------------------------------------------------------
  // @brief   表示設定：非公開
  //-------------------------------------------------------
  void _SetCaptureMode( void );
  void _SetDiffMode( void );

  //-------------------------------------------------------
  // @brief   インフォウィンドウ：非公開
  //-------------------------------------------------------
  void _InitInfo( void );
  void _ShowInfo( void );

  void _InitTutorial_Authentic( void );
  void _InitTutorial_Easy( void );
  void _ShowTutorial( void );
  bool _ShowTutorial_Authentic( void );
  bool _ShowTutorial_Easy( void );

  //-------------------------------------------------------
  // @brief   カメラ設定
  //-------------------------------------------------------
  void _SetCamera_ZoomIn( void );
  void _SetCamera_ZoomOut( void );
  void _SetCamera_HeightUp( void );
  void _SetCamera_HeightDown( void );
  void _SetCamera_ForcusPoke( void );
  void _SetCamera_ForcusPlayer( void );

  //-------------------------------------------------------
  // @brief   スクロールバー
  //-------------------------------------------------------
  void _UpdateScrollBar_Zoom( void );
  void _UpdateScrollBar_Height( void );
  void _UpdateScrollBar_Forcus( void );

  //-------------------------------------------------------
  // @brief   タッチイベント
  //-------------------------------------------------------
  void _TouchEvent( gfl2::ui::TouchPanel* touchPanel, bool isTouch );
  void _TouchEventFull( gfl2::ui::TouchPanel* touchPanel, bool isTouch );
  void _TouchEventPart( gfl2::ui::TouchPanel* touchPanel, bool isTouch );
  void _TouchEventScrollBar( gfl2::ui::TouchPanel* touchPanel, bool isTouch );

  void _TouchEventFullButton( gfl2::ui::TouchPanel* touchPanel, bool isTouch );
  void _TouchEventPartButton( gfl2::ui::TouchPanel* touchPanel, bool isTouch );

  void _CameraRotateButtonTrigger( gfl2::ui::TouchPanel* touchPanel, bool isTouch );
  bool _IsTouchCameraRotateButton( gfl2::ui::TouchPanel* touchPanel, bool isTouch );

  //-------------------------------------------------------
  // @brief   キーアクション
  //-------------------------------------------------------
  void _KeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* stick );
  void _KeyActionFull( gfl2::ui::Button* button, gfl2::ui::VectorDevice* stick );
  void _KeyActionPart( gfl2::ui::Button* button, gfl2::ui::VectorDevice* stick );

  void _KeyActionRotate( gfl2::ui::VectorDevice* stick );
  void _KeyActionZoom( gfl2::ui::Button* button );
  void _KeyActionHeight( gfl2::ui::Button* button );
  void _KeyActionForcus( gfl2::ui::Button* button );

  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  virtual void                                     OnLayoutPaneStartingAnimeEvent( const u32 paneId );
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );
  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick );

  virtual void OnClose( void );

private:
  static const u8 INFO_PAGE_MAX = 1;

private:
  ShootCommonView*  m_pCommonView;

  SlideScrollBar*   m_pScrollBar[SCROLLBAR_ID_NUM];

  gfl2::lyt::LytPane*   m_pCameraAuthenticBasePane;
  gfl2::lyt::LytPane*   m_pCameraEasyBasePane;
  gfl2::lyt::LytPane*   m_pForcusBasePane;

  gfl2::lyt::LytPane*   m_pCameraRotatePane[CAMERA_ROTATE_NUM];
  u32                   m_LastTouchCameraRotateIndex;

  bool    m_bTouchEnable;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_CAMERA_VIEW_H_INCLUDED__
