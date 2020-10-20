//=============================================================================
/**
 * @file    BagLvupWindow.h
 * @brief   バッグ画面レベルアップウィンドウ
 * @author  Hiroyuki Nakamura
 * @date    2015.04.21
 */
//=============================================================================
#if !defined( BAG_LVUP_WINDOW_H_INCLUDED )
#define BAG_LVUP_WINDOW_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(bag)

//=============================================================================
/**
 * @class BagLvupWindow
 * @brief バッグ画面レベルアップウィンドウクラス
 * @date  2015.04.21
 */
//=============================================================================
class BagLvupWindow
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
private:
  static const u32 PARAM_MAX = 6;   //!< 表示パラメータ数

  //! シーケンスID
  enum SeqID
  {
    SEQ_NONE = 0,   //!< 処理なし
    SEQ_IN_WAIT,    //!< INアニメ待ち
    SEQ_WIN1_WAIT,  //!< 第一ウィンドウ終了待ち
    SEQ_ADD_WAIT,   //!< ADDアニメ待ち
    SEQ_WIN2_WAIT,  //!< 第二ウィンドウ終了待ち
    SEQ_OUT_WAIT,   //!< OUTアニメ待ち
  };


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap          ヒープ
   * @param   g2d           G2DUtil
   * @param   is_demo_mode  デモモードフラグ
   */
  //-----------------------------------------------------------------------------
  BagLvupWindow( app::util::Heap * heap, app::util::G2DUtil * g2d, const bool * is_demo_mode );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~BagLvupWindow();

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPokemon
   * @brief   ポケモンをセット
   * @date    2015.04.21
   *
   * @param   pp  ポケモンパラメータ
   */
  //-----------------------------------------------------------------------------
  void SetPokemon( const pml::pokepara::PokemonParam * pp );

  //-----------------------------------------------------------------------------
  /**
   * @func    Start
   * @brief   処理開始
   * @date    2015.04.21
   */
  //-----------------------------------------------------------------------------
  void Start(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsEnd
   * @brief   終了チェック
   * @date    2015.04.21
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsEnd(void);


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    SetInputEnabled
   * @brief   入力判定の切り替え
   * @date    2015.06.25
   *
   * @param   flg   true = 有効, false = 無効
   */
  //-----------------------------------------------------------------------------
  void SetInputEnabled( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    Init
   * @brief   画面初期化
   * @date    2015.04.21
   */
  //-----------------------------------------------------------------------------
  void Init(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新処理
   * @date    2015.04.21
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画処理
   * @date    2015.04.21
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutWindow1st
   * @brief   最初のパラメータを表示
   * @date    2015.04.21
   */
  //-----------------------------------------------------------------------------
  void PutWindow1st(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutWindow2nd
   * @brief   レベルアップ後のパラメータを表示
   * @date    2015.04.21
   */
  //-----------------------------------------------------------------------------
  void PutWindow2nd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    OnTouchEvent
   * @brief   タッチパネルイベントの検知
   * @date    2015.04.21
   *
   * @param   touch_panel タッチパネル
   * @param   is_touch    true = タッチされている
   *
   * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
   * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
   * @retval  ENABLE_ACCESS         = それ以外
   *
   * @note  override UIInputListener
   */
  //-----------------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel * touch_panel, bool is_touch );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnKeyAction
   * @brief   キー入力の検知
   * @date    2015.04.21
   *
   * @param   button  ボタン
   * @param   key     十字キー
   * @param   stick   アナログスティック
   *
   * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
   * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
   * @retval  ENABLE_ACCESS         = それ以外
   *
   * @note  override UIInputListener
   */
  //-----------------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick );


private:
  app::util::Heap * m_heap;         //!< ヒープ
  app::util::G2DUtil * m_g2dUtil;   //!< G2DUtil

  const pml::pokepara::PokemonParam * m_pp; //!< ポケモンパラメータ
  u16	m_rep[PARAM_MAX];                     //!< 表示パラメータ

  bool m_input;   //!< 入力フラグ
  SeqID m_seq;    //!< シーケンス

  const bool * m_isDemoMode;  //!< デモモードフラグ
};


GFL_NAMESPACE_END(app)
GFL_NAMESPACE_END(bag)

#endif  // BAG_LVUP_WINDOW_H_INCLUDED
