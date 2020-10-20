#if !defined __FINDER_STUDIO_CAPTURE_PHOTO_LIST_VIEW_H_INCLUDED__
#define __FINDER_STUDIO_CAPTURE_PHOTO_LIST_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCapturePhotoListView.h
 * @date    2016/11/28 17:07:27
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：写真リストView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureViewBase.h"

#include <App/FinderStudioCapture/include/FinderStudioCaptureWork.h>
#include <App/FinderStudioStatic/include/FinderStudioMessageMenuView.h>
#include <App/FinderStudioStatic/include/FinderStudioInformationWindow.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    写真リストView
///
//////////////////////////////////////////////////////////////////////////
class PhotoListView
  : public CaptureViewBase
  , public Static::MessageMenuViewListener
  , public app::tool::InfoWindowBookTypeListener
{
  GFL_FORBID_COPY_AND_ASSIGN( PhotoListView ); //コピーコンストラクタ＋代入禁止

public:
  enum {
    RES_CONTINUE,     //!< 継続
    RES_CONFIRM,      //!< 確認
    RES_DECIDE,       //!< 決定
    RES_CANCEL,       //!< キャンセル
    RES_OVER_NUM,     //!< 保存数オーバー
    RES_NUM,
  };
  enum PhotoState
  {
    PHOTO_STATE_UNSELECT,   //!< 非選択
    PHOTO_STATE_SELECT,     //!< 選択
    PHOTO_STATE_NUM,
  };
  enum MsgWndType
  {
    MSGWND_TYPE_CONFIRM,
    MSGWND_TYPE_OVER_NUM,
    MSGWND_TYPE_NUM,
    MSGWND_TYPE_NONE,
  };

private:
  enum {
    BTN_ID_PHOTO_00,
    BTN_ID_PHOTO_01,
    BTN_ID_PHOTO_02,
    BTN_ID_PHOTO_03,
    BTN_ID_PHOTO_04,
    BTN_ID_PHOTO_05,
    BTN_ID_DECIDE,
    BTN_ID_INFO,
    BTN_ID_NUM,
  };
  enum {
    BTN_MTX_ROW = 2,
    BTN_MTX_COL = 3,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  PhotoListView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, void* cursorBuf );
  virtual ~PhotoListView( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   初期化・終了
  //-------------------------------------------------------
  virtual void Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual bool Terminate( void );

  //-------------------------------------------------------
  // @brief   更新・描画
  //-------------------------------------------------------
  virtual void Update( void );

  //-------------------------------------------------------
  // @brief   開く・閉じる
  //-------------------------------------------------------
  virtual void Show( void );

  //-------------------------------------------------------
  // @brief   サムネイル関連
  //-------------------------------------------------------
  void InitThumbnail( void );
  void TermThumbnail( void );
  bool IsReadyThumbnail( void );

  //-------------------------------------------------------
  // @brief   設定関数
  //-------------------------------------------------------
  void SetSavePhotoCount( u32 num ) { m_SavePhotoCount = num; }
  void SetMsgWndType( MsgWndType type ) { m_MsgWndType = type; }

  //-------------------------------------------------------
  // @brief   取得関数
  //-------------------------------------------------------
  PhotoState GetPhotoState( u32 index );
  u32        GetSavePhotoNum( void ) { return _GetSavePhotoNum(); }

private:
  //-------------------------------------------------------
  // @brief   初期化：非公開
  //-------------------------------------------------------
  virtual void Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual void InitPane( void );
  virtual void InitText( void );
  virtual void InitButton( void );
  virtual void InitAnim( void );

  void _InitPhotoList( void );

  //-------------------------------------------------------
  // @brief   写真選択処理：非公開
  //-------------------------------------------------------
  void _SelectPhoto( u32 photoIndex );
  void _SelectPhoto_CursorMove( u32 photoIndex );
  void _SelectPhoto_Decide( u32 photoIndex );

  void _SetSelectPhoto( u32 photoIndex );
  void _SetSelectPhoto_State( u32 photoIndex, PhotoState state );
  void _SetSelectPhoto_Anim( u32 photoIndex, PhotoState state );
  void _SetPhoto( u32 photoIndex );

  u32 _GetSavePhotoNum( void );

  bool _IsSaveablePhoto( void );

  //-------------------------------------------------------
  // @brief   サムネイル関連：非公開
  //-------------------------------------------------------
  void _ChangeThumbImage( u32 photoIndex );
  void _MakeTexInfo( gfl2::lyt::LytResTextureInfo* dstTexInfo, void* srcTexture );
  void _SetTexture( const gfl2::lyt::LytResTextureInfo* texInfo, gfl2::lyt::LytPicture* picture );

  void _CreatePhotoTexture( void );
  void _DeletePhotoTexture( void );

  //-------------------------------------------------------
  // @brief   カーソル：非公開
  //-------------------------------------------------------
  void _CursorMove( u32 row, u32 col );
  void _SetCursor( u32 cursorPos );

  bool _InputCursorMove( void );
  bool _InputSelectPhoto( gfl2::ui::Button* button );

  //-------------------------------------------------------
  // @brief   インフォウィンドウ：非公開
  //-------------------------------------------------------
  void _ShowInfo( void );
  void _ShowTutorial( void );

  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick );

  virtual void OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event );
  virtual void OnSelectMenu( const u32 index );

  virtual void OnClose( void );

private:
  gfl2::lyt::LytPicture*    m_pPhotoPicture[PHOTO_MAX];
  gfl2::lyt::LytPane*       m_pCheckMarkPane[PHOTO_MAX];
  gfl2::lyt::LytPane*       m_pCursorPane[PHOTO_MAX];

  void* m_pPhotoTexture[PHOTO_MAX];

  u32   m_PhotoIndex;
  u32   m_SavePhotoCount;
  
  u32   m_ThumbSeq;
  u32   m_ThumbIndex;

  u32   m_CursorMtxCol;
  u32   m_CursorMtxRow;

  PhotoState    m_PhotoState[PHOTO_MAX];

  MsgWndType    m_MsgWndType;

  u8    m_Seq;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_PHOTO_LIST_VIEW_H_INCLUDED__
