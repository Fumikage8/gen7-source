#if !defined( __FINDER_STUDIO_CAPTURE_PREPARE_SELECT_VIEW_H_INCLUDED__ )
#define __FINDER_STUDIO_CAPTURE_PREPARE_SELECT_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCapturePrepareSelectView.h
 * @date    2017/01/18 11:46:40
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：準備選択画面View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureViewBase.h"

#include <App/FinderStudioStatic/include/FinderStudioDefine.h>


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
/// @brief    準備選択画面View
///
//////////////////////////////////////////////////////////////////////////
class PrepareSelectView
  : public CaptureViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( PrepareSelectView ); //コピーコンストラクタ＋代入禁止

public:
  enum {
    RES_CONTINUE,
    RES_CAPTURE_MODE,
    RES_POKEMON,
    RES_BACKGROUND,
    RES_DRESSUP,
    RES_COMPOSITION,
    RES_SELECTED_ITEM,    //!< @fix MMCat[108]
    RES_NUM,
  };

private:
  enum {
    BTN_ID_AUTHENTIC_00,
    BTN_ID_AUTHENTIC_01,
    BTN_ID_AUTHENTIC_02,
    BTN_ID_AUTHENTIC_03,
    BTN_ID_AUTHENTIC_04,
    BTN_ID_EASY_00,
    BTN_ID_EASY_01,
    BTN_ID_NUM,
  };

  enum {
    PREPARE_BTN_00,
    PREPARE_BTN_01,
    PREPARE_BTN_02,
    PREPARE_BTN_03,
    PREPARE_BTN_04,
    PREPARE_BTN_NUM,

    PREPARE_EASY_BTN_NUM = 2,
    PREPARE_AUTH_BOTH_BTN_NUM = PREPARE_BTN_NUM,
    PREPARE_AUTH_POKE_BTN_NUM = PREPARE_BTN_NUM - 2,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  PrepareSelectView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, void* cursorBuf );
  virtual ~PrepareSelectView( void ) { ; }

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
  // @brief   設定関数群
  //-------------------------------------------------------
  void SetPrepareCommonViewListener( PrepareCommonViewListener* listener ) { m_pPreCmnViewListener = listener; }

  void SetDifficulty( Static::DifficultyMode mode );

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
  void _UpdateCommonButton( void );

  //-------------------------------------------------------
  // @brief   ボタン設定：非公開
  //-------------------------------------------------------
  void _SetButtonNum( Static::DifficultyMode mode );
  void _SetButtonVisible( Static::DifficultyMode mode );
  void _SetButtonEnable( Static::DifficultyMode mode );
  
  void _SetButtonText( Static::DifficultyMode mode );
  void _SetButtonText_CaptureMode( u32 index );
  void _SetButtonText_Pokemon( u32 index );
  void _SetButtonText_BackGround( u32 index );
  void _SetButtonText_Dressup( u32 index );
  void _SetButtonText_Composition( u32 index );

  //-------------------------------------------------------
  // @brief   カーソル：非公開
  //-------------------------------------------------------
  void _MoveCursor( u32 nextPos );

  gfl2::lyt::LytPane* _GetCursorPane( u32 pos );

  //-------------------------------------------------------
  // @brief   チェック関数：非公開
  //-------------------------------------------------------
  bool _IsCaptureModeBoth( void ) const;

  bool _IsButtonAction( void );

  //-------------------------------------------------------
  // @brief   リザルト：非公開
  //-------------------------------------------------------
  void _SetViewResult( const u32 paneId );

  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  virtual void                                     OnLayoutPaneStartingAnimeEvent( const u32 paneId );
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick );

private:
  PrepareCommonViewListener*    m_pPreCmnViewListener;

  gfl2::lyt::LytPane*       m_pAuthenticPane;
  gfl2::lyt::LytPane*       m_pEasyPane;

  gfl2::lyt::LytTextBox*    m_pTitleText[BTN_ID_NUM];
  gfl2::lyt::LytTextBox*    m_pContentsText[BTN_ID_NUM];

  gfl2::lyt::LytPane*   m_pAuthenticCursorPane[PREPARE_BTN_NUM];
  gfl2::lyt::LytPane*   m_pEasyCursorPane[PREPARE_EASY_BTN_NUM];

  u8    m_ButtonNum;

  Static::DifficultyMode  m_DifficultyMode;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_PREPARE_SELECT_VIEW_H_INCLUDED__
