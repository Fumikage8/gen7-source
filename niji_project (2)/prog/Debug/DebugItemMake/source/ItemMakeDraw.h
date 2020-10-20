#if PM_DEBUG
//=============================================================================
/**
 * @file    ItemMakeDraw.h
 * @brief   デバッグ用アイテム作成描画処理
 * @author  Hiroyuki Nakamura
 * @date    2015.05.26
 */
//=============================================================================
#if !defined( DEBUG_ITEM_MANE_DRAW_H_INCLUDED )
#define DEBUG_ITEM_MANE_DRAW_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"


GFL_NAMESPACE_BEGIN(Debug)

// 前方宣言
class ItemMakeListener;


//=============================================================================
/**
 * @class ItemMakeDraw
 * @brief 
 * @date  2015.05.20
 */
//=============================================================================
class ItemMakeDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
private:
  //! ボタンID
  enum ButtonID
  {
    BUTTON_ID_NAME = 0,   //!< 道具名
    BUTTON_ID_ITEM_ID,    //!< 道具ID
    BUTTON_ID_NUM,        //!< 個数
    BUTTON_ID_ENTER,      //!< 決定
    BUTTON_ID_ADD_ONE,    //!< 全道具１個追加
    BUTTON_ID_ADD_MAX,    //!< 全道具９９９個追加
    BUTTON_ID_EXIT,       //!< 終了

    BUTTON_ID_MAX,  //!< 最大
  };


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap        ヒープ
   * @param   render_man  描画マネージャ
   */
  //-----------------------------------------------------------------------------
  ItemMakeDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~ItemMakeDraw();

  //-----------------------------------------------------------------------------
  /**
   * @func    SetItemMakeListener
   * @brief   リスナーをセット
   * @date    2015.05.26
   *
   * @param   listener リスナー
   */
  //-----------------------------------------------------------------------------
  void SetItemMakeListener( ItemMakeListener * listener );

  //-----------------------------------------------------------------------------
  /**
   * @func    Init
   * @brief   初期化処理
   * @date    2015.05.26
   *
   * @param   res_buf レイアウトリソースバッファ
   */
  //-----------------------------------------------------------------------------
  void Init( void * res_buf );

  //-----------------------------------------------------------------------------
  /**
   * @func    End
   * @brief   終了処理
   * @date    2015.05.26
   *
   * @retval  true  = 処理完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool End(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutString
   * @brief   デフォルト文字列書き込み
   * @date    2015.05.26
   *
   * @param   item  道具ID
   * @param   num   個数
   */
  //-----------------------------------------------------------------------------
  void PutString( u32 item, u32 num );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutItem
   * @brief   道具書き込み
   * @date    2016.04.11
   *
   * @param   id  道具ID
   */
  //-----------------------------------------------------------------------------
  void PutItem( u32 id );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutNumber
   * @brief   個数書き込み
   * @date    2015.05.27
   *
   * @param   num   個数
   */
  //-----------------------------------------------------------------------------
  void PutNumber( u32 num );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetNumInputTitle
   * @brief   数値入力のタイトル取得
   * @date    2015.05.27
   *
   * @return  タイトル文字列
   */
  //-----------------------------------------------------------------------------
  gfl2::str::StrBuf * GetNumInputTitle(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetItemIDInputTitle
   * @brief   道具ID入力のタイトル取得
   * @date    2016.04.11
   *
   * @return  タイトル文字列
   */
  //-----------------------------------------------------------------------------
  gfl2::str::StrBuf * GetItemIDInputTitle(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetTempStrBuf
   * @brief   汎用文字列バッファ取得
   * @date    2015.05.27
   *
   * @return  汎用文字列バッファ
   */
  //-----------------------------------------------------------------------------
  gfl2::str::StrBuf * GetTempStrBuf(void);


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新
   * @date    2015.05.26
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   * @date    2015.05.26
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    Initialize2D
   * @brief   2D関連初期化
   * @date    2015.05.26
   *
   * @param   res_buf   レイアウトリソースバッファ
   */
  //-----------------------------------------------------------------------------
  void Initialize2D( void * res_buf );

  //-----------------------------------------------------------------------------
  /**
   * @func    Terminate2D
   * @brief   2D関連削除
   * @date    2015.05.26
   */
  //-----------------------------------------------------------------------------
  void Terminate2D(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateButton
   * @brief   ボタン作成
   * @date    2015.05.26
   */
  //-----------------------------------------------------------------------------
  void CreateButton(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetButtonInfoEx
   * @brief   ボタン情報セット
   * @date    2015.05.26
   *
   * @param   info      情報格納場所
   * @param   parts_tbl 部品ペインテーブル
   * @param   info_tbl  デフォルト情報テーブル
   * @parma   bound     境界ペインテーブル
   * @parma   max       セット数
   */
  //-----------------------------------------------------------------------------
  void SetButtonInfoEx( app::ui::ButtonInfoEx * info, const gfl2::lyt::LytPaneIndex * parts_tbl, const app::ui::ButtonInfoEx * info_tbl, const gfl2::lyt::LytPaneIndex * bound, u32 max );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateNormalButton
   * @brief   通常ボタンの情報をセット
   * @date    2015.05.26
   *
   * @param   info      情報格納場所
   */
  //-----------------------------------------------------------------------------
  void CreateNormalButton( app::ui::ButtonInfoEx * info );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateExitButton
   * @brief   戻るボタンの情報をセット
   * @date    2015.05.26
   *
   * @param   info      情報格納場所
   */
  //-----------------------------------------------------------------------------
  void CreateExitButton( app::ui::ButtonInfoEx * info );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnLayoutPaneEvent
   * @brief   ペインイベントの検知
   * @date    2015.05.26
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


private:
  app::util::Heap * m_heap;                             //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ

  ItemMakeListener * m_itemMakeListener;  //!< リスナー

  gfl2::str::MsgData * m_pMsgData;  //!< メッセージデータ
  print::WordSet * m_pWordSet;      //!< 単語セット
};

GFL_NAMESPACE_END(Debug)

#endif  // MENU_BASE_H_INCLUDED

#endif  // PM_DEBUG
