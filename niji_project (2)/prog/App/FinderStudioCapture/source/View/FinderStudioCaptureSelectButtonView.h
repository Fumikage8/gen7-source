#if !defined __FINDER_STUDIO_CAPTURE_SELECT_BUTTON_VIEW_H_INCLUDED__
#define __FINDER_STUDIO_CAPTURE_SELECT_BUTTON_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureSelectButtonView.h
 * @date    2016/11/24 20:06:24
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：汎用選択ボタンView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureViewBase.h"


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
/// @brief    汎用選択ボタンView
///
//////////////////////////////////////////////////////////////////////////
class SelectButtonView
  : public CaptureViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( SelectButtonView ); //コピーコンストラクタ＋代入禁止

public:
  /// ボタンタイプ
  enum BtnType
  {
    BTN_TYPE_DIFFICULTY,          //!< 難易度選択
    BTN_TYPE_CAPTURE_MODE,        //!< 撮影モード選択
    BTN_TYPE_FINISH_YESNO,        //!< 難易度選択からの終了確認
    BTN_TYPE_FINISH_PREPARE,      //!< 準備画面からの終了確認
    BTN_TYPE_FINISH_PREPARE_LOCK, //!< 準備画面からの終了確認：本格撮影ロック状態
    BTN_TYPE_FINISH_MAIN,         //!< 撮影メインからの終了確認
    BTN_TYPE_NUM,
  };
  /// リザルト
  enum {
    RES_CONTINUE,         //!< 継続
    RES_SELECT_BTN_00,    //!< ボタン１を選択
    RES_SELECT_BTN_01,    //!< ボタン２を選択
    RES_SELECT_BTN_02,    //!< ボタン３を選択
    RES_NUM,
  };

private:
  /// ボタンID
  enum {
    BTN_ID_THREE_BTN_00,  //!< ３つボタン１
    BTN_ID_THREE_BTN_01,  //!< ３つボタン２
    BTN_ID_THREE_BTN_02,  //!< ３つボタン３
    BTN_ID_TWO_BTN_00,    //!< ２つボタン１
    BTN_ID_TWO_BTN_01,    //!< ２つボタン２
    BTN_ID_NUM,

    TWO_BTN_NUM = 2,
    THREE_BTN_NUM = 3,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  SelectButtonView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, void* cursorBuf = NULL );
  virtual ~SelectButtonView( void ) { ; }

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

  //-------------------------------------------------------
  // @brief   ボタン設定
  //-------------------------------------------------------
  void SetSelectButtonType( BtnType type );

private:
  //-------------------------------------------------------
  // @brief   初期化：非公開
  //-------------------------------------------------------
  virtual void Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual void InitPane( void );
  virtual void InitButton( void );

  //-------------------------------------------------------
  // @brief   更新：非公開
  //-------------------------------------------------------
  void _UpdateCommonButton( void );

  //-------------------------------------------------------
  // @brief   ボタン設定：非公開
  //-------------------------------------------------------
  void _SetSelectButtonNum( void );
  void _SetSelectButtonText( void );
  void _SetSelectButtonSE( void );
  void _SetSelectButtonEnable( void );

  void _SetButtonText( u32 btnID, u32 msgID );
  void _SetButtonSE( u32 btnID, u32 seID );

  //-------------------------------------------------------
  // @brief   カーソル：非公開
  //-------------------------------------------------------
  void _MoveCursor( u32 nextPos );

  gfl2::lyt::LytPane* _GetCursorPane( u32 pos );

  //-------------------------------------------------------
  // @brief   チェック関数：非公開
  //-------------------------------------------------------
  bool _IsThreeButton( void );

  bool _IsButtonAction( void );

  //-------------------------------------------------------
  // @brief   ボタンアクション：非公開
  //-------------------------------------------------------
  void ButtonAction_Difficulty( const u32 paneID );
  void ButtonAction_CaptureMode( const u32 paneID );
  void ButtonAction_FinishYesNo( const u32 paneID );
  void ButtonAction_FinishPrepare( const u32 paneID );
  void ButtonAction_FinishPrepareLock( const u32 paneID );

  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  virtual void                                     OnLayoutPaneStartingAnimeEvent( const u32 paneId );
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick );

private:
  PrepareCommonViewListener*    m_pPreCmnViewListener;

  BtnType   m_ButtonType;

  u32       m_ButtonNum;

  gfl2::lyt::LytPane*       m_pTwoBtnPane;
  gfl2::lyt::LytPane*       m_pThreeBtnPane;

  gfl2::lyt::LytTextBox*    m_pButtonText[BTN_ID_NUM];

  gfl2::lyt::LytPane*       m_pTwoBtnCursorPane[TWO_BTN_NUM];
  gfl2::lyt::LytPane*       m_pThreeBtnCursorPane[THREE_BTN_NUM];
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_SELECT_BUTTON_VIEW_H_INCLUDED__
