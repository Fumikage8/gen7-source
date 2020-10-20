#if !defined( __FINDER_STUDIO_CAPTURE_BG_LIST_VIEW_H_INCLUDED__ )
#define __FINDER_STUDIO_CAPTURE_BG_LIST_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureBGListView.h
 * @date    2016/11/24 16:07:09
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：背景リストView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureViewBase.h"

#include <App/FinderStudioCapture/source/FinderStudioCapturePaneList.h>
#include <App/FinderStudioStatic/include/FinderStudioBGDataAccessor.h>
#include <App/FinderStudioStatic/include/FinderStudioViewerDefine.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class PrepareCommonViewListener;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
///
/// @brief    背景リストView
///
//////////////////////////////////////////////////////////////////////////
class BGListView
  : public CaptureViewBase
  , public app::tool::PaneList::CallBack
  , public app::tool::PaneListView::Listener
{
  GFL_FORBID_COPY_AND_ASSIGN( BGListView ); //コピーコンストラクタ＋代入禁止

private:
  enum {
    LIST_BTN_NUM = 7,               //!< リストのペイン数
    LIST_MAX = BG_ID_NUM,  //!< リストの最大数
    LIST_NONE = 0xFFFF,             //!< リスト
  };
public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  BGListView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, void* cursorBuf );
  virtual ~BGListView( void ) { ; }

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
  virtual void Draw( gfl2::gfx::CtrDisplayNo no );

  //-------------------------------------------------------
  // @brief   表示・非表示
  //-------------------------------------------------------
  virtual void Show( void );
  virtual void Hide( void );

  //-------------------------------------------------------
  // @brief   設定関数
  //-------------------------------------------------------
  void SetPrepareCommonViewListener( PrepareCommonViewListener* listener ) { m_pPreCmnViewListener = listener; }

private:
  //-------------------------------------------------------
  // @brief   初期化：非公開
  //-------------------------------------------------------
  virtual void Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual void InitAnim( void );

  //-------------------------------------------------------
  // @brief   ペインリスト関連：非公開
  //-------------------------------------------------------
  void _CreatePaneListView( void );
  void _DeletePaneListView( void );

  void _CreatePaneList( void );
  void _DeletePaneList( void );

  void _InitListMax( void );
  void _InitPaneListPaneData( void );
  void _InitPaneListSetupParam( app::tool::PaneList::SETUP_PARAM* param );
  void _InitScrollBarSetupParam( app::util::ScrollBar::SETUP_PARAM* param, u32 max );

  void _InitSelectedIndex( void );

  void _UpdatePaneList_Text( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  void _UpdatePaneList_Texture( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  void _UpdatePaneList_NewIcon( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

  void _ItemSelect_BackGround( u32 pos );
  void _ItemSelect_NewIcon( u32 pos );

  void _ResetListData( void );

  //-------------------------------------------------------
  // @brief   更新：非公開
  //-------------------------------------------------------
  void _UpdateCommonButton( void );

  //-------------------------------------------------------
  // @brief   時間帯関連：非公開
  //-------------------------------------------------------
  void _NextTimeZone( Viewer::TimeZone timeZone );
  void _SetTimeZone( Viewer::TimeZone timeZone );
  bool _CheckTimeZone( Viewer::TimeZone timeZone );

  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick );

  virtual void CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos );
  virtual void CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos ) { ; }
  virtual void CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  virtual void CallBack_ItemChange( void * work, u32 pos1, u32 pos2 ) { ; }
  virtual void PaneListItemSelectListener( u32 pos );

private:
  PrepareCommonViewListener*    m_pPreCmnViewListener;

  PaneList*                         m_pPaneList;
  app::tool::PaneList::PANE_DATA    m_PaneData[LIST_BTN_NUM];
  gfl2::lyt::LytPicture*            m_pPicture[LIST_BTN_NUM];
  gfl2::lyt::LytParts*              m_pNewIcon[LIST_BTN_NUM];

  Viewer::TimeZone    m_TimeZone;

  u32   m_ListMax;
  u32   m_ListItemID[LIST_MAX];
  u32   m_NewIconItemID[LIST_BTN_NUM];

  u32   m_SelectPaneIndex;
  u32   m_DecideItemIndex;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_BG_LIST_VIEW_H_INCLUDED__
