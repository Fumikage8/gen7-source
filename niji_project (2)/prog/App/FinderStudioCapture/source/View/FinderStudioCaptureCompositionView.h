#if !defined( __FINDER_STUDIO_CAPTURE_COMPOSITION_VIEW_H_INCLUDED__ )
#define __FINDER_STUDIO_CAPTURE_COMPOSITION_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureCompositionView.h
 * @date    2017/01/20 13:23:51
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：構図選択View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureViewBase.h"

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
/// @brief    構図選択View
///
//////////////////////////////////////////////////////////////////////////
class CompositionView
  : public CaptureViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( CompositionView ); //コピーコンストラクタ＋代入禁止

public:
  enum {
    RES_CONTINUE,
    RES_ENABLE,
    RES_DISABLE,
    RES_NUM,
  };

private:
  enum {
    BTN_ID_POSE_01,
    BTN_ID_POSE_02,
    BTN_ID_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  CompositionView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, void* cursorBuf );
  virtual ~CompositionView( void ) { ; }

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

  //-------------------------------------------------------
  // @brief   設定関数群
  //-------------------------------------------------------
  void SetPrepareCommonViewListener( PrepareCommonViewListener* listener ) { m_pPreCmnViewListener = listener; }

private:
  //-------------------------------------------------------
  // @brief   初期化：非公開
  //-------------------------------------------------------
  virtual void Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual void InitButton( void );

  //-------------------------------------------------------
  // @brief   更新：非公開
  //-------------------------------------------------------
  void _UpdateCommonButton( void );

  //-------------------------------------------------------
  // @brief   設定：非公開
  //-------------------------------------------------------
  void _SetComposition( void );
  void _SetComposition_Button( void );
  void _SetComposition_Cursor( void );

  void _SetCompositionEnableResult( void );

  //-------------------------------------------------------
  // @brief   カーソル：非公開
  //-------------------------------------------------------
  void _CursorMove( u32 nextPos );

  void _SetCursorPos( u32 pos );
  void _SetCursorPosWithMessage( u32 pos );

  //-------------------------------------------------------
  // @brief   ボタン：非公開
  //-------------------------------------------------------
  bool _IsButtonAction( void );

  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  virtual void                                     OnLayoutPaneStartingAnimeEvent( const u32 paneId );
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick );

private:
  PrepareCommonViewListener*    m_pPreCmnViewListener;

  gfl2::lyt::LytPane*   m_pCursorPane[BTN_ID_NUM];

  u8    m_Seq;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_COMPOSITION_VIEW_H_INCLUDED__