#if !defined( __FINDER_STUDIO_DECORATIONE_LOWER_MAIN_VIEW_H_INCLUDED__ )
#define __FINDER_STUDIO_DECORATIONE_LOWER_MAIN_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioDecorationLowerMainView.h
 * @date    2017/01/31 19:16:30
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：下メイン画面View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDecorationViewBase.h"
#include "FinderStudioDecorationViewDefine.h"

#include <App/FinderStudioStatic/include/FinderStudioMessageMenuView.h>
#include <App/FinderStudioStatic/include/FinderStudioInformationWindow.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    下メイン画面View
///
//////////////////////////////////////////////////////////////////////////
class LowerMainView
  : public DecoViewBase
  , public app::tool::InfoWindowBookTypeListener
  , public Static::MessageMenuViewListener
{
  GFL_FORBID_COPY_AND_ASSIGN( LowerMainView ); //コピーコンストラクタ＋代入禁止

public:
  /// リザルトID
  enum {
    RES_CONTINUE,
    // スタンプ・フレームリスト
    RES_LIST_STAMP_S,
    RES_LIST_STAMP_M,
    RES_LIST_STAMP_L,
    RES_LIST_TEXT,
    RES_LIST_FRAME,
    // スタンプ配置オーバー
    RES_STAMP_OVER_OPEN,
    RES_STAMP_OVER_CLOSE,
    // 終了メニュー
    RES_FIN_MENU_OPEN,
    RES_FIN_MENU_CLOSE,
    RES_FIN_MENU_OVERWRITE,
    RES_FIN_MENU_OVERWRITE_YES,
    RES_FIN_MENU_OVERWRITE_NO,
    RES_FIN_MENU_NEW_WRITE,
    RES_FIN_MENU_NEW_WRITE_CAUTION,
    RES_FIN_MENU_NOT_WRITE,
    RES_FIN_MENU_NOT_WRITE_YES,
    RES_FIN_MENU_NOT_WRITE_NO,
    RES_FIN_MENU_QUIT,
    RES_NUM,
  };

private:
  enum {
    BTN_ID_LIST_STAMP_S,
    BTN_ID_LIST_STAMP_M,
    BTN_ID_LIST_STAMP_L,
    BTN_ID_LIST_TEXT,
    BTN_ID_LIST_FRAME,
    BTN_ID_INFO,
    BTN_ID_FINISH,
    BTN_ID_NUM,
  };

  enum TouchType {
    TOUCH_TYPE_NONE,        //!< 操作なし
    TOUCH_TYPE_NEW_ICON,    //!< 新規アイコンの移動
    TOUCH_TYPE_OLD_ICON,    //!< 旧アイコンの再配置
    TOUCH_TYPE_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  LowerMainView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );
  virtual ~LowerMainView( void ) { ; }

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
  // @brief   背景アニメ
  //-------------------------------------------------------
  void SetBackGroundAnimFrame( f32 frame );

  //-------------------------------------------------------
  // @brief   写真
  //-------------------------------------------------------
  void ShowPhotoData( void );

  //-------------------------------------------------------
  // @brief   スタンプ
  //-------------------------------------------------------
  void StartStampMove( void );

  void CreateStamp( DECO_TYPE type, u32 decoIndex, gfl2::math::VEC3 pos );

  //-------------------------------------------------------
  // @brief   フレーム
  //-------------------------------------------------------
  void CreateFrame( u32 frameIndex );

  //-------------------------------------------------------
  // @brief   設定・取得
  //-------------------------------------------------------
  void SetMsgWndID( MsgWndID id ) { m_MsgWndID = id; }

  void* GetPhotoBuffer( void ) { return m_pPhotoBuffer; }

private:
  //-------------------------------------------------------
  // @brief   初期化：非公開
  //-------------------------------------------------------
  virtual void Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual void InitPane( void );
  virtual void InitAnim( void );
  virtual void InitButton( void );

  //-------------------------------------------------------
  // @brief   スタンプ：非公開
  //-------------------------------------------------------
  void _CreateStampTexture( DECO_TYPE type, u32 decoIndex, gfl2::math::VEC3 pos );
  void _DeleteNewStampTexture( void );
  void _DeleteStampTexture( u8 index );
  void _StuffingStampTexture( u8 startIndex );

  void _RegisterStamp( void );
  void _CheckStampOutRange( gfl2::math::VEC3* pos );

  void _StartStampMove( void );
  void _StartOldStampMove( u8 index );
  void _FinishStampMove( void );
  void _FinishOldStampMove( void );

  void _SetStampMove_Button( bool enable );

  void _SetSubTouchPos( gfl2::ui::TouchPanel* tp, u8 index );

  //-------------------------------------------------------
  // @brief   フレーム：非公開
  //-------------------------------------------------------
  void _CreateFrameTexture( u32 frameIndex );
  void _DeleteFrameTexture( void );

  //-------------------------------------------------------
  // @brief   インフォウィンドウ
  //-------------------------------------------------------
  void _InitInfo( void );
  void _ShowInfo( void );

  void _ShowTutorial( void );

  //-------------------------------------------------------
  // @brief   変換
  //-------------------------------------------------------
  gfl2::math::VEC3 _ConvertLayoutPoint( gfl2::ui::TouchPanel* touchPanel );

  //-------------------------------------------------------
  // @brief   ボタン
  //-------------------------------------------------------
  void _LockButton( void );
  void _UnlockButton( void );

  bool _IsButtonAction( void );

  //-------------------------------------------------------
  // @brief   ペインアクション
  //-------------------------------------------------------
  void _PaneActionStampList( const u32 paneId );
  void _PaneActionInfo( const u32 paneId );
  void _PaneActionFinish( const u32 paneId );

  //-------------------------------------------------------
  // @brief   タッチイベント
  //-------------------------------------------------------
  void _TouchEventNone( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  void _TouchEventNewIcon( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  void _TouchEventOldIcon( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );

  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  virtual void                                     OnLayoutPaneStartingAnimeEvent( const u32 paneID );
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );
  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  virtual void OnClose( void );

  virtual void OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event );
  virtual void OnSelectMenu( const u32 index );

#if PM_DEBUG
private:
  void DebugPrint_VEC3( const char* name, gfl2::math::VEC3 pos )
  {
    FUKUSHIMA_PRINT( "%s : x=[%.1f] y=[%.1f z=[%.1f]\n", name, pos.x, pos.y, pos.z );
  }
#endif

private:
  static const u8 STAMP_MAX = Static::DECO_ITEM_MAX;

private:
  gfl2::lyt::LytPicture*    m_pCanvasPane;

  gfl2::lyt::LytPane*       m_pStampParentPane;
  gfl2::lyt::LytPane*       m_pStampMovePane;
  gfl2::lyt::LytPane*       m_pStampBasePane;
  gfl2::lyt::LytPicture*    m_pStampMovePicture;
  gfl2::lyt::LytPicture*    m_pStampPicture[STAMP_MAX];
  gfl2::lyt::LytPane*       m_pTrashPane;

  gfl2::lyt::LytPane*       m_pFrameParentPane;
  gfl2::lyt::LytPicture*    m_pFramePicture;

  gfl2::math::VEC3          m_SubTouchPos;

  u8          m_StampCount;
  u32         m_FrameIndex;

  TouchType   m_TouchType;
  u8          m_TouchIndex;

  DECO_TYPE   m_DecoType;
  u32         m_DecoIndex;

  bool        m_bTrashTouch;

  MsgWndID    m_MsgWndID;

  void*       m_pPhotoBuffer;

private:
  static const u8 INFO_PAGE_MAX = 6;

  gfl2::str::StrBuf*                    m_pInfoStr[INFO_PAGE_MAX];
  const gfl2::lyt::LytResTextureInfo*   m_pInfoTex[INFO_PAGE_MAX];
};

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_DECORATIONE_LOWER_MAIN_VIEW_H_INCLUDED__
