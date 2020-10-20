//=============================================================================================
/**
 * @file  btl_ui.h
 * @brief ポケモンXY バトルシステム UI管理
 * @author  taya
 *
 * @date  2011.03.02  作成
 */
//=============================================================================================
#ifndef __BTL_UI_H__
#define __BTL_UI_H__
#include <ui/include/gfl2_UI.h>
#include "btl_common.h"

GFL_NAMESPACE_BEGIN(btl)

/**
 *  各種ボタン・キー定義
 */
enum {
  BUTTON_A      = gfl2::ui::BUTTON_A,
  BUTTON_B      = gfl2::ui::BUTTON_B,
  BUTTON_X      = gfl2::ui::BUTTON_X,
  BUTTON_Y      = gfl2::ui::BUTTON_Y,
  BUTTON_L      = gfl2::ui::BUTTON_L,
  BUTTON_R      = gfl2::ui::BUTTON_R,
  BUTTON_START  = gfl2::ui::BUTTON_START,
  BUTTON_SELECT = gfl2::ui::BUTTON_SELECT,
  BUTTON_HOME   = gfl2::ui::BUTTON_HOME,

  DIR_LEFT  = gfl2::ui::DIR_LEFT,
  DIR_RIGHT = gfl2::ui::DIR_RIGHT,
  DIR_UP    = gfl2::ui::DIR_UP,
  DIR_DOWN  = gfl2::ui::DIR_DOWN,
};


namespace ui {

//=============================================================================================
/**
 * システム初期化：namespace 'ui' 内部に各種デバイス管理モジュールを構築する
 *
 * @param   pHeap   モジュール構築用ヒープハンドル
 *
 */
//=============================================================================================
extern void InitSystem( HeapHandle pHeap );

//=============================================================================================
/**
 * システム終了：namespace 'ui' 内部に構築した各種デバイス管理モジュールを破棄する
 */
//=============================================================================================
extern void QuitSystem( void );

//=============================================================================================
/**
 * プレイヤーがメッセージ表示を「次へ送る」ための入力を行ったか判定
 *（ex. A_button or B_button or panel_Touch ）
 *
 * @retval  bool    所定の入力が行われたら true
 */
//=============================================================================================
extern bool CheckNextTrg( void );

//=============================================================================================
/**
 * 十字キー＆ボタンのトリガー入力チェック
 *
 * @param   keyMask   キーマスク（BUTTON_A, BUTTON_L, DIR_UP 等を | で連結する）
 *
 * @retval  bool    入力があったらtrue
 */
//=============================================================================================
extern bool CheckKeyTrg( u32 keyMask );

//=============================================================================================
/**
 * 十字キー＆ボタンの継続入力チェック
 *
 * @param   keyMask   キーマスク（BUTTON_A, BUTTON_L, DIR_UP 等を | で連結する）
 *
 * @retval  bool    入力があったらtrue
 */
//=============================================================================================
extern bool CheckKeyCont( u32 keyMask );

//=============================================================================================
/**
 * 十字キー＆ボタンのリピート入力チェック
 *
 * @param   keyMask
 *
 * @retval  bool
 */
//=============================================================================================
extern bool CheckKeyRepeat( u32 keyMask );


} // end of namespace 'ui'
GFL_NAMESPACE_END(btl)



#endif  // #ifndef __BTL_UI_H__
