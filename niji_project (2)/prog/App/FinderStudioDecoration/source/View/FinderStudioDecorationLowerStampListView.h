#if !defined( __FINDER_STUDIO_DECORATIONE_STAMP_LIST_VIEW_H_INCLUDED__ )
#define __FINDER_STUDIO_DECORATIONE_STAMP_LIST_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioDecorationLowerStampListView.h
 * @date    2017/01/31 19:29:29
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：スタンプリストView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDecorationViewBase.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    スタンプリストView
///
//////////////////////////////////////////////////////////////////////////
class StampListView
  : public DecoViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( StampListView ); //コピーコンストラクタ＋代入禁止

public:
  enum {
    RES_CONTINUE,
    RES_CLOSE,
    RES_NUM,
  };

private:
  enum {
    BTN_ID_BACK,
    BTN_ID_ARROW_LEFT,
    BTN_ID_ARROW_RIGHT,
    BTN_ID_STAMP_00,
    BTN_ID_STAMP_01,
    BTN_ID_STAMP_02,
    BTN_ID_STAMP_03,
    BTN_ID_STAMP_04,
    BTN_ID_STAMP_05,
    BTN_ID_STAMP_06,
    BTN_ID_STAMP_07,
    BTN_ID_STAMP_08,
    BTN_ID_STAMP_09,
    BTN_ID_STAMP_10,
    BTN_ID_STAMP_11,
    BTN_ID_NUM,
  };

  enum {
    STAMP_PANE_MAX = 12,              //!< １ページに配置されるスタンプ数
    STAMP_NULL = Static::DECO_NULL,   //!< スタンプのNULL値
    STAMP_SELECT_FRAME = 3,           //!< スタンプ選択判定フレーム値
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  StampListView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );
  virtual ~StampListView( void ) { ; }

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
  // @brief   表示・非表示
  //-------------------------------------------------------
  virtual void Show( void );
  virtual void Hide( void );

  //-------------------------------------------------------
  // @brief   背景アニメ
  //-------------------------------------------------------
  void SetBackGroundAnimFrame( f32 frame );

  //-------------------------------------------------------
  // @brief   表示設定
  //-------------------------------------------------------
  void SetDecoType( DECO_TYPE type );

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
  // @brief   スタンプ：非公開
  //-------------------------------------------------------
  void _CreateStampList( void );
  void _DeleteStampList( void );

  void _InitStamp( void );
  void _InitStamp_UnlockStamp( void );
  void _InitStamp_PageNum( void );

  void _SetStamp( void );
  void _SetStamp_Texture( void );
  void _SetStamp_Button( void );
  void _SetStamp_ButtonStamp( void );
  void _SetStamp_ButtonArrow( void );
  void _SetStamp_PageNum( void );
  void _SetStamp_NewIcon( void );

  void _ReplaceStampTexture( u32 pane_index, u32 dataID );

  u32  _GetStampIndex( u32 index );
  u8   _GetLangIndex( void );
  u32  _GetExceptionStamp( u32 stampIndex );

  void _SetInvisibleNewIcon( u32 stampIndex );
  bool _IsStampNewVisible( u32 stampIndex );

  //-------------------------------------------------------
  // @brief   ページ：非公開
  //-------------------------------------------------------
  void _PageFeed_Prev( void );
  void _PageFeed_Next( void );
  void _PageFeed( u8 nextPage );

  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneLongPressEvent( const u32 paneId );

private:
  DECO_TYPE     m_DecoType;

  u8    m_PageIndex;
  u8    m_PageMax;

  u8    m_StampCount;
  u32*  m_pStampList;

  gfl2::lyt::LytParts*      m_pStampParts[STAMP_PANE_MAX];
  gfl2::lyt::LytPicture*    m_pStampPicture[STAMP_PANE_MAX];
  gfl2::lyt::LytPane*       m_pStampNewIcon[STAMP_PANE_MAX];

  gfl2::lyt::LytPane*       m_pPageNumPane;

  gfl2::lyt::LytTextBox*    m_pPageNowText;
  gfl2::lyt::LytTextBox*    m_pPageMaxText;

  gfl2::math::VEC3  m_TouchPos;

  u8    m_Seq;
};

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_DECORATIONE_STAMP_LIST_VIEW_H_INCLUDED__
