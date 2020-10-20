#if !defined( __FINDER_MENU_WINDOW_H__ )
#define __FINDER_MENU_WINDOW_H__
#pragma once
//======================================================================
/**
 * @file    FinderMenuWindow.h
 * @date    2015/11/06 11:18:34
 * @author  fukushima_yuya
 * @brief   ウィンドウ関連
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Layout/include/gfl2_Layout.h>
#include <str/include/gfl2_Str.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)
class DrawerBase;
class MenuCursor;
GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class MenuWindow;
class MenuWindowListener;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_BEGIN(util)
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
enum WindowKind
{
  WND_KIND_PHOTO_SELECT,          //!< 写真選択確認
  WND_KIND_PHOTO_QUIT,            //!< やめる確認
  WND_KIND_PHOTO_DECIDE,          //!< 写真決定

  WND_KIND_SD_SAVE_CONF,          //!< 保存確認
  WND_KIND_SD_SAVE_SUCCESS,       //!< 保存成功
  WND_KIND_SD_SAVE_FAILED_1,      //!< 保存失敗：容量不足
  WND_KIND_SD_SAVE_FAILED_2,      //!< 保存失敗：空き不足
  WND_KIND_SD_SAVE_FAILED_3,      //!< 保存失敗：それ以外のエラー

  WND_KIND_TUTORIAL_SELECT,       //!< チュートリアル：写真選択
  WND_KIND_TUTORIAL_SCORE,        //!< チュートリアル：得点表示

  WND_KIND_SCORE_BEGIN,           //!< 開始メッセージ
  WND_KIND_SCORE_PHOTO_EVAL_1,    //!< 評価１
  WND_KIND_SCORE_PHOTO_EVAL_2,    //!< 評価２
  WND_KIND_SCORE_PHOTO_EVAL_3,    //!< 評価３
  WND_KIND_SCORE_PHOTO_EVAL_4,    //!< 評価４
  WND_KIND_SCORE_HIGH_SCORE,      //!< 最高記録更新

  WND_KIND_SCORE_VER_UP_1,        //!< バージョンアップ１
  WND_KIND_SCORE_VER_UP_2,        //!< バージョンアップ２
  WND_KIND_SCORE_VER_UP_3,        //!< バージョンアップ３
  WND_KIND_SCORE_VER_UP_4,        //!< バージョンアップ４
  WND_KIND_SCORE_DOWNLOAD,        //!< データ受信
  WND_KIND_SCORE_NEW_VER_1,       //!< 新バージョン説明１
  WND_KIND_SCORE_NEW_VER_2,       //!< 新バージョン説明２
  WND_KIND_SCORE_NEW_VER_3,       //!< 新バージョン説明３
  WND_KIND_SCORE_NEW_VER_4,       //!< 新バージョン説明４

  WND_KIND_SCORE_NEXT_1,          //!< 次の目標１
  WND_KIND_SCORE_NEXT_2,          //!< 次の目標２
  WND_KIND_SCORE_NEXT_3,          //!< 次の目標３
  WND_KIND_SCORE_NEXT_4,          //!< 次の目標４

  WND_KIND_SCORE_SUPPORT_1,       //!< サポート１
  WND_KIND_SCORE_SUPPORT_2,       //!< サポート２
  WND_KIND_SCORE_SUPPORT_3,       //!< サポート３

  WND_KIND_SCORE_FINAL_1,         //!< 最終１
  WND_KIND_SCORE_FINAL_2,         //!< 最終２
  WND_KIND_SCORE_FINAL_3,         //!< 最終３

  WND_KIND_NUM,
};

enum WindowResult
{
  WND_RES_YES,
  WND_RES_NO,
  WND_RES_NUM,
};

//------------------------------------------------------------------------------
// @brief   メニューウィンドウ
//------------------------------------------------------------------------------
class MenuWindow
{
  GFL_FORBID_COPY_AND_ASSIGN( MenuWindow );

public:
  //------------------------------------------------------------------
  /**
   * @brief    コンストラクタ
   */
  //------------------------------------------------------------------
  MenuWindow( DrawerBase* base, app::util::AppRenderingManager* renderMan, app::tool::MenuWindowListener* listener, void* cursorBuf );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~MenuWindow( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   準備完了か
   */
  //------------------------------------------------------------------
  bool IsReady( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   破棄完了か
   */
  //------------------------------------------------------------------
  bool IsDelete( void ) const;

public:
  //------------------------------------------------------------------
  /**
   * @brief   開く
   */
  //------------------------------------------------------------------
  void Open( void );

  //------------------------------------------------------------------
  /**
   * @brief   閉じる
   */
  //------------------------------------------------------------------
  void Close( void );

  //------------------------------------------------------------------
  /**
   * @brief   開いているか
   */
  //------------------------------------------------------------------
  bool IsOpened( void ) const;

private:
  DrawerBase*               m_pBase;

  app::tool::MenuWindow*    m_pMenuWindow;

  bool                      m_isOpen;
};

//------------------------------------------------------------------------------
// @brief   メッセージウィンドウリスナー
//------------------------------------------------------------------------------
class MessageWindowListener
{
public:
  //------------------------------------------------------------------
  /**
   * @brief   閉じたことを通知
   */
  //------------------------------------------------------------------
  virtual void OnClosed( void ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   ボタンの入力設定
   * @param   enable    入力フラグ
   */
  //------------------------------------------------------------------
  virtual void SetButtonEnable( bool enable ) { ; }
};

//------------------------------------------------------------------------------
// @brief   メッセージウィンドウ
//------------------------------------------------------------------------------
class MessageWindow
{
  GFL_FORBID_COPY_AND_ASSIGN( MessageWindow );

public:
  //------------------------------------------------------------------
  /**
   * @brief    コンストラクタ
   */
  //------------------------------------------------------------------
  MessageWindow( DrawerBase* base, MessageWindowListener* listener, const u32 partsIndex, bool maskVisible = true );

  //------------------------------------------------------------------
  /**
   * @brief    デストラクタ
   */
  //------------------------------------------------------------------
  ~MessageWindow( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief    初期化
   */
  //------------------------------------------------------------------
  bool InitFunc( app::util::AppRenderingManager* renderMan, app::tool::MenuWindowListener* listener, void* cursorBuf );

  //------------------------------------------------------------------
  /**
   * @brief    破棄
   */
  //------------------------------------------------------------------
  bool EndFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief    更新
   */
  //------------------------------------------------------------------
  void UpdateFunc( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief    開く
   */
  //------------------------------------------------------------------
  void Open( WindowKind kind );

  //------------------------------------------------------------------
  /**
   * @brief    閉じる
   */
  //------------------------------------------------------------------
  void Close( void );

  //------------------------------------------------------------------
  /**
   * @brief    開いているか
   */
  //------------------------------------------------------------------
  bool IsOpened( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief    ウィンドウの種類を取得
   */
  //------------------------------------------------------------------
  WindowKind GetWindowKind( void ) const;

private:
  DrawerBase*               m_pBase;

  MenuWindow*               m_pMenuWindow;

  MessageWindowListener*    m_pListener;

private:
  u32           m_InitSeq;

  u32           m_EndSeq;

  u32           m_MainSeq;

  u8            m_WndType;

  WindowKind    m_WndKind;

  u32           m_ActiveAnim;

  WindowKind    m_OpenReqKind;

  bool          m_isOpened;

private:
  gfl2::str::StrWin::Result   m_Result;

private:
  gfl2::lyt::LytTextBox*    m_pMsgLine_1;

  gfl2::lyt::LytTextBox*    m_pMsgLine_2;

  gfl2::lyt::LytPane*       m_pMsgCursor;
};

GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)

#endif // __FINDER_MENU_WINDOW_H__