#if PM_DEBUG
//=============================================================================
/**
 * @file    PokeIconViewDraw.h
 * @brief   ポケモンアイコン確認画面描画処理
 * @author  Hiroyuki Nakamura
 * @date    2016.04.15
 */
//=============================================================================
#if !defined( POKEICON_VIEW_DRAW_H_INCLUDED )
#define POKEICON_VIEW_DRAW_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class PokeIcon;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(Debug)

//=============================================================================
/**
 * @class PokeIconViewDraw
 * @brief ポケモンアイコン確認画面描画クラス
 * @date  2016.04.15
 */
//=============================================================================
class PokeIconViewDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
public:
  //=============================================================================
  /**
   * @class Listener
   * @brief リスナークラス
   * @date  2016.04.15
   */
  //=============================================================================
  class Listener
  {
  public:
    enum ActionID
    {
      ACTION_EXIT = 0,            //!< 終了
      ACTION_CHANGE_POKE_PLUS,    //!< ポケモン切り替え+
      ACTION_CHANGE_POKE_MINUS,   //!< ポケモン切り替え-
      ACTION_CHANGE_PAGE_PLUS,    //!< ページ切り替え+
      ACTION_CHANGE_PAGE_MINUS,   //!< ページ切り替え-
      ACTION_CHANGE_SEX,          //!< 性別切り替え
      ACTION_CHANGE_FORM_PLUS,    //!< フォルム切り替え+
      ACTION_CHANGE_FORM_MINUS,   //!< フォルム切り替え-
      ACTION_CHANGE_FLIP,         //!< 反転切り替え
      ACTION_CHANGE_BG_COLOR,     //!< 背景色切り替え
    };

    //-----------------------------------------------------------------------------
    /**
     * @func    OnAction
     * @brief   入力時のリスナー
     * @date    2016.04.15
     *
     * @param   id    動作ID
     */
    //-----------------------------------------------------------------------------
    virtual void OnAction( ActionID id ) = 0;
  };

  static const s32 ICON_UPPER_MAX = 14;                         //!< 上画面のアイコン数
  static const s32 ICON_LOWER_MAX = 12;                         //!< 下画面のアイコン数
  static const s32 ICON_MAX = ICON_UPPER_MAX + ICON_LOWER_MAX;  //!< アイコン総数


private:
  //! レイアウトID
  enum LyoutID
  {
    LYTID_UPPER = 0,
    LYTID_LOWER,
    LYTID_MAX
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
  PokeIconViewDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~PokeIconViewDraw();

  //-----------------------------------------------------------------------------
  /**
   * @func    SetListener
   * @brief   リスナーをセット
   * @date    2016.04.15
   *
   * @param   listener  リスナー
   */
  //-----------------------------------------------------------------------------
  void SetListener( Listener * listener );

  //-----------------------------------------------------------------------------
  /**
   * @func    Init
   * @brief   初期化処理
   * @date    2016.04.15
   *
   * @param   res_buf   レイアウトリソースバッファ
   * @param   msg_buf   メッセージリソースバッファ
   */
  //-----------------------------------------------------------------------------
  void Init( void * res_buf, void * msg_buf );

  //-----------------------------------------------------------------------------
  /**
   * @func    End
   * @brief   終了処理
   * @date    2016.04.15
   *
   * @retval  true  = 処理完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool End(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Start
   * @brief   開始処理
   * @date    2016.04.15
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Start(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangePage
   * @brief   ページ切り替え
   * @date    2016.04.15
   *
   * @param   val   切り替え数
   */
  //-----------------------------------------------------------------------------
  void ChangePage( s32 val );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeSex
   * @brief   性別切り替え
   * @date    2016.04.15
   */
  //-----------------------------------------------------------------------------
  void ChangeSex(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeForm
   * @brief   フォルム切り替え
   * @date    2016.04.15
   *
   * @param   val   切り替え数
   */
  //-----------------------------------------------------------------------------
  void ChangeForm( s32 val );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeFlip
   * @brief   反転切り替え
   * @date    2016.04.15
   */
  //-----------------------------------------------------------------------------
  void ChangeFlip(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeColor
   * @brief   背景色切り替え
   * @date    2016.04.15
   */
  //-----------------------------------------------------------------------------
  void ChangeBgColor(void);


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新
   * @date    2016.04.15
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   * @date    2016.04.15
   *
   * @param   displayNo   描画面
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    Initialize2D
   * @brief   2D関連初期化
   * @date    2016.04.15
   *
   * @param   res_buf   レイアウトリソースバッファ
   * @param   msg_buf   メッセージリソースバッファ
   */
  //-----------------------------------------------------------------------------
  void Initialize2D( void * res_buf, void * msg_buf );

  //-----------------------------------------------------------------------------
  /**
   * @func    Terminate2D
   * @brief   2D関連削除
   * @date    2016.04.15
   */
  //-----------------------------------------------------------------------------
  void Terminate2D(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreatePokeIcon
   * @brief   ポケモンアイコン生成
   * @date    2016.04.15
   */
  //-----------------------------------------------------------------------------
  void CreatePokeIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreatePokeIcon
   * @brief   ポケモンアイコン削除
   * @date    2016.04.15
   */
  //-----------------------------------------------------------------------------
  void DeletePokeIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangePokemon
   * @brief   ポケモン切り替え
   * @date    2016.04.15
   *
   * @param   is_str  true = 文字列描画
   */
  //-----------------------------------------------------------------------------
  void ChangePokemon( bool is_str );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutStrBgColor
   * @brief   背景色表示
   * @date    2016.04.15
   */
  //-----------------------------------------------------------------------------
  void PutStrBgColor(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutStrForm
   * @brief   フォルム表示
   * @date    2016.04.15
   */
  //-----------------------------------------------------------------------------
  void PutStrForm(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutStrSex
   * @brief   性別表示
   * @date    2016.04.15
   */
  //-----------------------------------------------------------------------------
  void PutStrSex(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutStrFlip
   * @brief   反転表示
   * @date    2016.04.15
   */
  //-----------------------------------------------------------------------------
  void PutStrFlip(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    OnKeyAction
   * @brief   キー入力の検知
   * @date    2016.04.15
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
  app::util::Heap * m_heap;                             //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ

  Listener * m_listener;  //!< 入力リスナー

  gfl2::str::MsgData * m_pMsgData;  //!< メッセージデータ
  print::WordSet * m_pWordSet;      //!< 単語セット

  app::tool::PokeIcon * m_pPokeIcon;  //!< ポケモンアイコン

  u32 m_pokeArcID[ICON_MAX];

  u8 m_sex;         //!< 表示中の性別
  u8 m_formNo;      //!< 表示中のフォルム番号
  u8 m_flip;        //!< 反転フラグ
  u8 m_bgColor;     //!< 背景色

  u16 m_topMonsNo;  //!< 表示中の開始ポケモン番号

  u16 m_subSeq;         //!< サブシーケンス
  bool m_isDrawEnable;  //!< 描画フラグ
};


GFL_NAMESPACE_END(Debug)

#endif  // POKEICON_VIEW_DRAW_H_INCLUDED

#endif  // PM_DEBUG
