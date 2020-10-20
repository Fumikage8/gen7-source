//=============================================================================
/**
 * @file    BagSaleWindow.h
 * @brief   バッグ画面：道具売却ウィンドウ
 * @author  Hiroyuki Nakamura
 * @date    2015.07.16
 */
//=============================================================================
#if !defined( BAG_SALE_WINDOW_H_INCLUDED )
#define BAG_SALE_WINDOW_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "Savedata/include/MyItemSave.h"
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(bag)


//=============================================================================
/**
 * @class BagSaleWindow
 * @brief バッグ画面道具売却ウィンドウ
 * @date  2015.07.17
 */
//=============================================================================
class BagSaleWindow
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
public:
  //! 処理状況
  enum Result
  {
    RESULT_UPDATE = 0,  //!< 処理中
    RESULT_DECIDE,      //!< 決定
    RESULT_CANCEL,      //!< キャンセル
  };

private:
  //! ボタンID
  enum ButtonID
  {
    BUTTON_ID_PLUS = 0,   //!< ＋矢印
    BUTTON_ID_MINUS,      //!< －矢印
    BUTTON_ID_DECIDE,     //!< 決定
    BUTTON_ID_CANCEL,     //!< キャンセル
    BUTTON_ID_MAX         //!< 最大数
  };


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap  ヒープ
   * @param   g2d   G2DUtil
   */
  //-----------------------------------------------------------------------------
  BagSaleWindow( app::util::Heap * heap, app::util::G2DUtil * g2d );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~BagSaleWindow();

  //-----------------------------------------------------------------------------
  /**
   * @func    GetResult
   * @brief   処理状況を取得
   * @date    2015.07.16
   *
   * @return  処理状況
   */
  //-----------------------------------------------------------------------------
  Result GetResult(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Start
   * @brief   処理開始
   * @date    2015.07.16
   *
   * @param   item  道具
   * @param   gold  売値
   */
  //-----------------------------------------------------------------------------
  void Start( const Savedata::MyItem::ITEM_ST * item, u32 gold );

  //-----------------------------------------------------------------------------
  /**
   * @func    StartOnce
   * @brief   所持数１の場合の開始処理
   * @date    2016.04.07
   *
   * @param   item  道具
   * @param   gold  売値
   */
  //-----------------------------------------------------------------------------
  void StartOnce( const Savedata::MyItem::ITEM_ST * item, u32 gold );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsStart
   * @brief   開始チェック
   * @date    2015.07.16
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsStart(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    End
   * @brief   処理終了
   * @date    2015.07.16
   */
  //-----------------------------------------------------------------------------
  void End(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsEnd
   * @brief   終了チェック
   * @date    2015.07.16
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetItem
   * @brief   道具取得
   * @date    2015.07.16
   *
   * @return  道具
   */
  //-----------------------------------------------------------------------------
  const Savedata::MyItem::ITEM_ST * GetItem(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetGold
   * @brief   売値取得
   * @date    2015.07.16
   *
   * @return  売値
   */
  //-----------------------------------------------------------------------------
  u32 GetGold(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetNum
   * @brief   個数取得
   * @date    2015.07.16
   *
   * @return  個数
   */
  //-----------------------------------------------------------------------------
  u32 GetNum(void);


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新処理
   * @date    2015.07.16
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画処理
   * @date    2015.07.16
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    Init
   * @brief   初期処理
   * @date    2015.07.16
   */
  //-----------------------------------------------------------------------------
  void Init(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateButton
   * @brief   ボタン生成
   * @date    2015.07.17
   */
  //-----------------------------------------------------------------------------
  void CreateButton(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetButtonInfoEx
   * @brief   ボタン情報の生成
   * @date    2015.07.17
   *
   * @param   info      生成場所
   * @param   parts_tbl 部品ペインテーブル
   * @param   info_tbl  デフォルト情報テーブル
   * @param   bound     境界ペインテーブル
   * @param   max       生成数
   */
  //-----------------------------------------------------------------------------
  void SetButtonInfoEx( app::ui::ButtonInfoEx * info, const gfl2::lyt::LytPaneIndex * parts_tbl, const app::ui::ButtonInfoEx * info_tbl, const gfl2::lyt::LytPaneIndex * bound, u32 max );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateNormalButton
   * @brief   通常のボタン情報生成
   * @date    2015.07.17
   *
   * @param   info  生成場所
   */
  //-----------------------------------------------------------------------------
  void CreateNormalButton( app::ui::ButtonInfoEx * info );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateInfo
   * @brief   情報更新
   * @date    2015.07.16
   */
  //-----------------------------------------------------------------------------
  void UpdateInfo(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetArrowAnime
   * @brief   矢印アニメセット（キー操作用）
   * @date    2015.07.17
   *
   * @param   mv    個数の増減方向
   * @param   flg   true = ON, false = OFF
   */
  //-----------------------------------------------------------------------------
  void SetArrowAnime( int mv, bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetValue
   * @brief   個数を変更する
   * @date    2015.07.17
   *
   * @param   plus    増減値
   * @param   is_trg  true = トリガー入力
   */
  //-----------------------------------------------------------------------------
  void SetValue( int plus, bool is_trg );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsArrowHit
   * @brief   矢印上をタッチしているか
   * @date    2015.07.17
   *
   * @param   id  ボタンID
   *
   * @retval  true  = タッチしている
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsArrowHit( ButtonID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetInputKey
   * @brief   キー入力の状態を設定
   * @date    2016.03.25
   *
   * @param   flg   true = キー入力, false = それ以外
   */
  //-----------------------------------------------------------------------------
  void SetInputKey( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnLayoutPaneEvent
   * @brief   ペインイベントの検知
   * @date    2015.07.16
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
   * @func    OnTouchEvent
   * @brief   タッチパネルイベントの検知
   * @date    2015.07.17
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
   * @date    2015.07.16
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

  gfl2::ui::VectorDevice * m_key;   //!< 十字キー

  const Savedata::MyItem::ITEM_ST * m_item;   //!< 道具
  u32 m_gold;   //!< 売値
  u32 m_num;    //!< 個数

  Result m_result;  //!< 処理状況

  bool m_isPlus;    //!< 個数を＋しているか（キー操作用）
  bool m_isMinus;   //!< 個数を－しているか（キー操作用）

  gfl2::lyt::LytPane * m_arrowBound[2];   //!< 矢印の当たり判定用ペイン

  bool m_isBeforeTouch;   //!< true = タッチ入力されている
  bool m_isBeforeKey;     //!< true = キー入力されている
};



GFL_NAMESPACE_END(app)
GFL_NAMESPACE_END(bag)

#endif  // BAG_SALE_WINDOW_H_INCLUDED
