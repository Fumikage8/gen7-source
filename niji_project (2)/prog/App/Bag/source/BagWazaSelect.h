//=============================================================================
/**
 * @file    BagWazaSelect.h
 * @brief   バッグ画面技選択
 * @author  Hiroyuki Nakamura
 * @date    2015.04.21
 */
//=============================================================================
#if !defined( BAG_WAZA_SELECT_H_INCLUDED )
#define BAG_WAZA_SELECT_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class AppCommonGrpIconData;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(bag)
// 前方宣言
struct APP_PARAM;


//=============================================================================
/**
 * @class BagWazaSelect
 * @brief バッグ画面技選択ウクラス
 * @date  2015.04.21
 */
//=============================================================================
class BagWazaSelect
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
private:
  //! シーケンスID
  enum SeqID
  {
    SEQ_NONE = 0,   //!< 処理なし
    SEQ_IN_WAIT,    //!< INアニメ待ち
    SEQ_MAIN,       //!< 第一ウィンドウ終了待ち
    SEQ_OUT_WAIT,   //!< OUTアニメ待ち
    SEQ_LAYOUT_OFF, //!< レイアウトを非表示にする
  };

  static const u32 RESULT_NONE = 0xffff;   //!< 未選択

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   param         外部設定パラメータ
   * @param   heap          ヒープ
   * @param   render_man    描画マネージャ
   * @param   is_demo_mode  デモモードフラグ
   */
  //-----------------------------------------------------------------------------
  BagWazaSelect(
    const APP_PARAM * param,
    app::util::Heap * heap,
    app::util::AppRenderingManager * render_man,
    app::tool::AppCommonGrpIconData * common_icon,
    const bool * is_demo_mode );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~BagWazaSelect();

  //-----------------------------------------------------------------------------
  /**
   * @func    Setup
   * @brief   データ設定
   * @date    2015.04.21
   *
   * @param   lyt_buf       レイアウトリソースバッファ
   * @param   msg_data      メッセージデータ
   * @param   wset          単語セット
   * @param   cursor_buf    メニューカーソルのリソースバッファ
   */
  //-----------------------------------------------------------------------------
  void Setup( void * lyt_buf, gfl2::str::MsgData * msg_data, print::WordSet * wset, void ** cursor_buf );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsModuleEnd
   * @brief   終了できるかチェック
   * @date    2015.08.21
   *
   * @retval  true  = 可
   * @retval  false = 不可
   */
  //-----------------------------------------------------------------------------
  bool IsModuleEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Start
   * @brief   処理開始
   * @date    2015.04.21
   *
   * @param   pp  ポケモンパラメータ
   */
  //-----------------------------------------------------------------------------
  void Start( const pml::pokepara::PokemonParam * pp, u32 message );

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

  //-----------------------------------------------------------------------------
  /**
   * @func    GetResult
   * @brief   終了チェック
   * @date    2015.04.21
   *
   * @return  選択結果
   */
  //-----------------------------------------------------------------------------
  u32 GetResult(void);


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
   * @func    CreateLayout
   * @brief   レイアウト生成
   * @date    2015.04.21
   *
   * @param   buf       レイアウトリソースバッファ
   * @param   msg_data  メッセージデータ
   * @param   wset      単語セット
   */
  //-----------------------------------------------------------------------------
  void CreateLayout( void * buf, gfl2::str::MsgData * msg_data, print::WordSet * wset );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetParts
   * @brief   項目の部品ペインを取得
   * @date    2015.04.21
   *
   * @param   id  項目ID
   *
   * @return  部品ペイン
   */
  //-----------------------------------------------------------------------------
  gfl2::lyt::LytParts * GetParts( u32 id );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetPane
   * @brief   項目の部品からペインを取得
   * @date    2015.04.21
   *
   * @param   id      項目ID
   * @param   index   ペインのインデックス
   *
   * @return  ペイン
   */
  //-----------------------------------------------------------------------------
  gfl2::lyt::LytPane * GetPane( u32 id, gfl2::lyt::LytPaneIndex index );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetTextBoxPane
   * @brief   項目の部品からテキストボックスペインを取得
   * @date    2015.04.21
   *
   * @param   id      項目ID
   * @param   index   ペインのインデックス
   *
   * @return  テキストボックスペイン
   */
  //-----------------------------------------------------------------------------
  gfl2::lyt::LytTextBox * GetTextBoxPane( u32 id, gfl2::lyt::LytPaneIndex index );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetDefaultString
   * @brief   デフォルト文字列を描画
   * @date    2015.04.21
   */
  //-----------------------------------------------------------------------------
  void SetDefaultString(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateButton
   * @brief   ボタン生成
   * @date    2015.04.21
   */
  //-----------------------------------------------------------------------------
  void CreateButton(void);

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
   * @func    SetItemColosr
   * @brief   項目の色を変更
   * @date    2015.04.21
   *
   * @param   id    項目ID
   * @param   waza  技番号
   */
  //-----------------------------------------------------------------------------
  void SetItemColosr( u32 id, WazaNo waza );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMenuCursor
   * @brief   メニューカーソル生成
   * @date    2015.04.21
   *
   * @param   buf   メニューカーソルのリソースバッファ
   */
  //-----------------------------------------------------------------------------
  void CreateMenuCursor( void ** buf );

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMenuCursor
   * @brief   メニューカーソル削除
   * @date    2015.04.21
   */
  //-----------------------------------------------------------------------------
  void DeleteMenuCursor(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetCursor
   * @brief   カーソル配置
   * @date    2015.04.21
   *
   * @param   pos   カーソル位置
   */
  //-----------------------------------------------------------------------------
  void PutCursor( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnLayoutPaneTouchTriggerEvent
   * @brief   ペインタッチ時を検知
   * @date    2015.04.21
   *
   * @param   button_id   選択されたボタンID
   *
   * @note  override UIInputListener
   */
  //-----------------------------------------------------------------------------
  virtual void OnLayoutPaneTouchTriggerEvent( const u32 button_id );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnLayoutPaneEvent
   * @brief   ペインイベントの検知
   * @date    2015.04.21
   *
   * @param   button_id   通知されたボタンID
   *
   * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
   * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
   * @retval  ENABLE_ACCESS         = それ以外
   *
   * @note  override UIInputListener
   */
  //-----------------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 button_id );

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
  const APP_PARAM * m_appParam;     //!< 外部設定パラメータ
  app::util::Heap * m_heap;         //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ
  app::tool::AppCommonGrpIconData * m_commonIcon;       //!< タイプアイコン

  app::tool::MenuCursor * m_pMenuCursor;  //!< メニューカーソル

  u16 m_result;     //!< 選択結果
  s16 m_cursorPos;  //!< カーソル位置

  SeqID m_seq;    //!< シーケンス

  const bool * m_isDemoMode;  //!< デモモードフラグ

  bool m_isDrawEnable;  //!< 描画許可フラグ
};


GFL_NAMESPACE_END(app)
GFL_NAMESPACE_END(bag)

#endif  // BAG_WAZA_SELECT_H_INCLUDED
