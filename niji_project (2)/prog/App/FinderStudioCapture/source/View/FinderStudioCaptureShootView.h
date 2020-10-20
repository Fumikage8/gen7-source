#if !defined( __FINDER_STUDIO_CAPTURE_SHOOT_VIEW_H_INCLUDED__ )
#define __FINDER_STUDIO_CAPTURE_SHOOT_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureShootView.h
 * @date    2016/11/28 12:55:51
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：撮影メインView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureViewBase.h"

#include <App/FinderStudioStatic/include/FinderStudioViewerDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioMessageMenuView.h>
#include <App/FinderStudioStatic/include/FinderStudioInformationWindow.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class ShootCommonView;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
///
/// @brief    撮影メインView
///
//////////////////////////////////////////////////////////////////////////
class ShootView
  : public CaptureViewBase
  , public Static::MessageMenuViewListener
  , public app::tool::InfoWindowBookTypeListener
{
  GFL_FORBID_COPY_AND_ASSIGN( ShootView ); //コピーコンストラクタ＋代入禁止

public:
  enum {
    RES_CONTINUE,
    RES_CALL_SELECT_ZMOTION,    //!< Zモーション選択へ
    RES_CAPTURE,                //!< 撮影へ
    RES_CALL_PHOTO_LIST,        //!< 写真リスト
    RES_MOTION_TYPE_CONFIRM,    //!< モーションタイプ変更時のリセット確認
    RES_NUM,
  };

  enum MsgWndType
  {
    MSGWND_TYPE_NONE,
    MSGWND_TYPE_CAPTURE_FINISH,         //!< 撮影完了
    MSGWND_TYPE_MOTION_TYPE_CONFIRM,    //!< モーションタイプ変更時のリセット確認
    MSGWND_TYPE_NUM,
  };

private:
  enum {
    BTN_ID_SHUTTER,
    BTN_ID_RESET,
    BTN_ID_INFO,
    BTN_ID_POKE_MOTION_BTL,
    BTN_ID_POKE_MOTION_RFR,
    BTN_ID_POKE_MOTION_START,
    BTN_ID_PLAYER_MOTION_SELECT,
    BTN_ID_PLAYER_MOTION_CONTROL,
    BTN_ID_NUM,
  };
  enum {
    SEQ_IDLE,
    SEQ_TUTORIAL,
    SEQ_MAIN,
    SEQ_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  ShootView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );
  virtual ~ShootView( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   初期化・終了
  //-------------------------------------------------------
  virtual void Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual bool Terminate( void );

  void InitShowView( void );

  //-------------------------------------------------------
  // @brief   更新・描画
  //-------------------------------------------------------
  virtual void Update( void );
  virtual void Draw( gfl2::gfx::CtrDisplayNo no );

  //-------------------------------------------------------
  // @brief   表示・非表示
  //-------------------------------------------------------
  virtual void Show( void );
  virtual void Hide( void );

  //-------------------------------------------------------
  // @brief   設定関数群
  //-------------------------------------------------------
  void SetShootCommonView( ShootCommonView* view ) { m_pCommonView = view; }

  void SetMsgWndType( MsgWndType type ) { m_MsgWndType = type; }

private:
  //-------------------------------------------------------
  // @brief   初期化：非公開
  //-------------------------------------------------------
  virtual void Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual void InitPane( void );
  virtual void InitText( void );
  virtual void InitButton( void );

  //-------------------------------------------------------
  // @brief   更新：非公開
  //-------------------------------------------------------
  void _UpdateSequence( void );
  void _UpdatePlayerMotion( void );
  void _UpdateMotionType( void );
  void _UpdateCaptureButton( void );

  //-------------------------------------------------------
  // @brief   撮影モード：非公開
  //-------------------------------------------------------
  void _ChangeCaptureMode( void );
  void _SetCaptureModeAnim( Viewer::CaptureMode mode );
  void _SetCaptureModeButton( Viewer::CaptureMode mode );

  //-------------------------------------------------------
  // @brief   モーション：非公開
  //-------------------------------------------------------
  void _ChangePokeMotionType( u32 paneId, bool bReset );
  void _SetPokeMotionType( Viewer::PokeMotionType type );
  void _SetPokeMotionType_Anim( Viewer::PokeMotionType type );
  void _SetPokeMotionType_Button( Viewer::PokeMotionType type );

  bool _IsComparePokeMotionType( u32 btnID );
  bool _IsNeedReset( void );

  void _ReplaceTypeIcon( Viewer::ZMotionType type );
  void _SetPlayerMotionControl( void );

  //-------------------------------------------------------
  // @brief   撮影：非公開
  //-------------------------------------------------------
  void _StartCapture( void );

  //-------------------------------------------------------
  // @brief   インフォウィンドウ：非公開
  //-------------------------------------------------------
  void _InitInfo( void );
  void _InitInfo_Authentic( void );
  void _InitInfo_Easy( void );

  void _SetupInfo( void );

  //-------------------------------------------------------
  // @brief   チュートリアル
  //-------------------------------------------------------
  void _Tutorial( void );

  void _InitTutorial( void );
  void _InitTutorial_Authentic( void );
  void _InitTutorial_Easy( void );

  //-------------------------------------------------------
  // @brief   チェック関数
  //-------------------------------------------------------
  bool _IsDifficultyAuthentic( void ) const;

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
  virtual void                                     OnLayoutPaneStartingAnimeEvent( const u32 painId );
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick );

  virtual void OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event );

  virtual void OnClose( void );

private:
  static const u8 INFO_PAGE_MAX = 5;

private:
  ShootCommonView*        m_pCommonView;

  Viewer::ZMotionType     m_ZMotionType;
  Viewer::PokeMotionType  m_PokeMotionType;

  u32   m_SelectedBtnID;

  gfl2::lyt::LytParts*    m_pTypeIcon;
  gfl2::lyt::LytPane*     m_pAloraIcon;
  gfl2::lyt::LytPane*     m_pPikaIcon;
  gfl2::lyt::LytPicture*  m_pIconPicture;

  u8    m_MainSeq;

  MsgWndType    m_MsgWndType;

  bool  m_bPrevPlaying;

  u8    m_InfoPageMax;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_SHOOT_VIEW_H_INCLUDED__
