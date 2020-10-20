//=============================================================================
/**
 * @file    AppToolSlideScroll.h
 * @brief   スライド操作でのスクロール判定処理
 * @author  Hiroyuki Nakamura
 * @date    2015.06.04
 */
//=============================================================================
#if !defined( APP_TOOL_SLIDE_SCROLL_H_INCLUDED )
#define APP_TOOL_SLIDE_SCROLL_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <ui/include/gfl2_UI.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

//=============================================================================
/**
 * @class SlideScroll
 * @brief スライド操作スクロール判定クラス
 * @date  2015.06.04
 */
//=============================================================================
class SlideScroll
{
  GFL_FORBID_COPY_AND_ASSIGN(SlideScroll);  // コピーコンストラクタ＋代入禁止

public:
  //! スライド操作モード
  enum SlideMode
  {
    MODE_H = 0,   //!< 横方向
    MODE_V,       //!< 縦方向
  };

  //! 動作結果
  enum Result
  {
    RESULT_NONE = 0,      //!< なし
    RESULT_DRAG_START,    //!< ドラッグ開始
    RESULT_DRAG,          //!< ドラッグ中
    RESULT_RELEASE,       //!< リリース
    RESULT_SLIDE_PLUS,    //!< ＋方向にスライド
    RESULT_SLIDE_MINUS,   //!< −方向にスライド

    RESULT_SLIDE_LEFT  = RESULT_SLIDE_PLUS,   //!< ＋方向にスライド ( MODE_H用 )
    RESULT_SLIDE_RIGHT = RESULT_SLIDE_MINUS,  //!< −方向にスライド ( MODE_H用 )

    RESULT_SLIDE_UP   = RESULT_SLIDE_PLUS,    //!< ＋方向にスライド ( MODE_V用 )
    RESULT_SLIDE_DOWN = RESULT_SLIDE_MINUS,   //!< −方向にスライド ( MODE_V用 )
  };


private:
  //! シーケンスID
  enum SeqID
  {
    SEQ_WAIT = 0,   //!< 待機
    SEQ_TOUCH,      //!< タッチ
    SEQ_DRAG,       //!< ドラッグ
  };

  static const u32 DRAG_START_ABS = 12;    //!< ドラッグを開始させる座標の差分
  static const u32 SLIDE_START_ABS = 12;   //!< スライドを開始させる座標の差分

  
public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   mode  スライド操作モード
   */
  //-----------------------------------------------------------------------------
  SlideScroll( SlideMode mode );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デズトラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~SlideScroll();

  //-----------------------------------------------------------------------------
  /**
   * @func    SetTouchAreaDisp
   * @brief   タッチ有効範囲を設定（左上(0,0)座標）
   * @date    2015.06.05
   *
   * @param   up      上Y座標
   * @param   down    下Y座標
   * @param   left    左X座標
   * @param   right   右X座標
   */
  //-----------------------------------------------------------------------------
  void SetTouchAreaDisp( u16 up, u16 down, u16 left, u16 right );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetTouchArea
   * @brief   タッチ有効範囲を設定（中心(0,0)座標）
   * @date    2015.06.05
   *
   * @param   up      上Y座標
   * @param   down    下Y座標
   * @param   left    左X座標
   * @param   right   右X座標
   */
  //-----------------------------------------------------------------------------
  void SetTouchArea( f32 up, f32 down, f32 left, f32 right );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateSlide
   * @brief   更新処理
   * @date    2015.06.04
   *
   * @param   tp        タッチパネル
   * @param   is_touch  true = タッチされている
   *
   * @return  動作結果
   */
  //-----------------------------------------------------------------------------
  Result UpdateSlide( gfl2::ui::TouchPanel * tp, bool is_touch );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetResult
   * @brief   動作結果を取得
   * @date    2015.06.04
   *
   * @return  動作結果
   */
  //-----------------------------------------------------------------------------
  Result GetResult(void) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    GetDragOffset
   * @brief   ドラッグ中の座標の差分を取得
   * @date    2015.06.04
   *
   * @return  開始からの座標の差分
   */
  //-----------------------------------------------------------------------------
  int GetDragOffset(void) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    GetSlideLevel
   * @brief   スライド時の速度レベルを取得
   * @date    2015.07.27
   *
   * @return  速度レベル
   *
   * @li  PaneListで使用（計算は旧PaneListと踏襲）
   */
  //-----------------------------------------------------------------------------
  int GetSlideLevel(void) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    SetExTouchFlag
   * @brief   トリガー情報を無視してドラッグを開始するフラグをセット
   * @date    2015.07.28
   */
  //-----------------------------------------------------------------------------
  void SetExTouchFlag(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitWork
   * @brief   Update用のワークを初期化
   * @date    2015.06.17
   *
   * @li  UpdateSlide()の呼び出し元で別処理に遷移した場合に使用してください
   */
  //-----------------------------------------------------------------------------
  void InitUpdateWork(void);


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    IsTouchArea
   * @brief   指定座標がタッチ有効範囲内か
   * @date    2015.06.05
   *
   * @retval  true  = 範囲内
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsTouchArea( u16 x, u16 y ) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    GetOffset
   * @brief   座標の差分を取得
   * @date    2015.06.04
   *
   * @param   x1  X座標１
   * @param   x2  X座標２
   * @param   y1  Y座標１
   * @param   y2  Y座標２
   *
   * @return  モード別の座標の差分
   */
  //-----------------------------------------------------------------------------
  int GetOffset( int x1, int x2, int y1, int y2 ) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    IsStartDrag
   * @brief   ドラッグを開始するか
   * @date    2015.06.04
   *
   * @param   x   タッチX座標
   * @param   y   タッチY座標
   *
   * @retval  true  = 開始
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsStartDrag( u16 x, u16 y ) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    CheckSlideScroll
   * @brief   スライドするかをチェック
   * @date    2015.06.04
   *
   * @retval  RESULT_SLIDE_PLUS  = ＋方向にスライド
   * @retval  RESULT_SLIDE_MINUS = −方向にスライド
   * @retval  RESULT_RELEASE     = スライドしない
   */
  //-----------------------------------------------------------------------------
  Result CheckSlideScroll(void) const;


private:
  Result m_result;    //!< 動作結果
  SlideMode m_mode;   //!< スライド操作モード

  u16 m_initDragX;    //!< ドラッグ開始時のタッチX座標
  u16 m_initDragY;    //!< ドラッグ開始時のタッチY座標
  u16 m_lastDragX;    //!< 最後のタッチX座標
  u16 m_lastDragY;    //!< 最後のタッチY座標
  u16 m_oldDragX;     //!< 前回のタッチX座標
  u16 m_oldDragY;     //!< 前回のタッチY座標

  SeqID m_seq;        //!< シーケンスID

  u16 m_touchUY;      //!< タッチ有効範囲：上Y座標
  u16 m_touchDY;      //!< タッチ有効範囲：下Y座標
  u16 m_touchLX;      //!< タッチ有効範囲：左X座標
  u16 m_touchRX;      //!< タッチ有効範囲：右X座標

  bool m_exTouchFlag; //!< トリガー情報を無視してドラッグを開始するフラグ
};

GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

#endif
