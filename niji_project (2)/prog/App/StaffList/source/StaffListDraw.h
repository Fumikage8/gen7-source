//=============================================================================
/**
 * @file    StaffListDraw.h
 * @brief   momiji追加分のスタッフロール画面描画処理
 * @author  Hiroyuki Nakamura
 * @date    2017.03.02
 */
//=============================================================================
#if !defined( STAFFLIST_DRAW_H_INCLUDED )
#define STAFFLIST_DRAW_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// module
#include "App/StaffList/include/StaffListDef.h"
// momiji
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(StaffList)

//=============================================================================
/**
 * @class StaffListDraw
 * @brief スタッフロール画面描画クラス
 * @date  2017.03.02
 */
//=============================================================================
class StaffListDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
public:
  //! リストデータ
  struct LIST_ITEM_DATA
  {
    u32 index;        //!< データインデックス
    u8 put;           //!< 配置モード
    u8 is_end;        //!< 1 = 表示後に終了
    u8 is_vanish_bg;  //!< 1 = 背景非表示
    u8 is_double;     //!< 1 = 二重表示
    u16 label;        //!< テキストラベル
    u16 frame;        //!< 表示終了までのフレーム
    s16 x;            //!< X座標
    s16 y;            //!< Y座標
  };

  //! 文字列配置
  enum TextPutMode
  {
    TEXT_PUT_CENTER = 0,  //!< 中央
    TEXT_PUT_LEFT,        //!< 左
    TEXT_PUT_RIGHT,       //!< 右
  };


private:
  //! レイアウトID
  enum LyoutID
  {
    LYTID_UPPER = 0,  //!< 上画面
    LYTID_MAX         //!< 最大数
  };

  static const s32 START_ALPHA_FRAME_BG = 15;   //!< 開始時の背景アルファフレーム
  static const s32 START_ALPHA_FRAME_STR = 10;  //!< 開始時の文字列アルファフレーム
  static const s32 END_ALPHA_FRAME = 10;        //!< 終了時のアルファフレーム

  static const s32 BG_ALPHA = 60;     //!< 背景のアルファ値
  static const s32 STR_ALPHA = 255;   //!< 文字列のアルファ値

  static const f32 START_ALPHA_BG_CALC_VALUE;   //!< 開始時の背景アルファ加算値
  static const f32 START_ALPHA_STR_CALC_VALUE;  //!< 開始時の文字列アルファ加算値

  static const f32 END_ALPHA_BG_CALC_VALUE;     //!< 終了時の背景アルファ加算値
  static const f32 END_ALPHA_STR_CALC_VALUE;    //!< 終了時の文字列アルファ加算値

  static const u32 TEXT_PANE_MAX = 24;            //!< テキストボックスペインの数
  static const u32 TEXT_PANE_DOUBLE_INDEX = 12;   //!< 二重化用テキストボックスペインのインデックス

  //! テキスト表示ワーク
  struct LABEL_WORK
  {
    u32 seq:8;
    u32 count:11;
    u32 frame:11;
    u32 is_end:1;
    u32 is_vanish_bg:1;
  };

  static const u32 LABEL_WORK_NONE = 0xffffffff;  //!< テキスト表示ワークなし

  //! テキスト表示シーケンス
  enum LabelSeq
  {
    LABEL_SEQ_NONE = 0,   //!< 動作なし
    LABEL_SEQ_IN,         //!< イン
    LABEL_SEQ_WAIT,       //!< 待機
    LABEL_SEQ_OUT,        //!< アウト
    LABEL_SEQ_END,        //!< 終了
  };

  static const gfl2::lyt::LytPaneIndex TextPaneTable[TEXT_PANE_MAX];  //!< テキストボックスペインテーブル

  static const u32 LABEL_WAIT_FRAME = 90;  //!< テキスト待機フレーム

  //! 背景表示シーケンス
  enum BgSeq
  {
    BG_SEQ_WAIT = 0,  //!< 待機
    BG_SEQ_IN,        //!< イン
    BG_SEQ_MAIN,      //!< メイン
    BG_SEQ_OUT,       //!< アウト
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
  StaffListDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~StaffListDraw();

  //-----------------------------------------------------------------------------
  /**
   * @func    Init
   * @brief   初期化処理
   * @date    2017.03.02
   *
   * @param   res_buf   レイアウトリソースバッファ
   * @param   msg_buf   メッセージリソースバッファ
   *
   * @retval  true  = 処理完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Init( void * res_buf, void * msg_buf );

  //-----------------------------------------------------------------------------
  /**
   * @func    End
   * @brief   終了処理
   * @date    2017.03.02
   *
   * @retval  true  = 処理完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool End(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutBgPane
   * @brief   背景ペインを配置
   * @date    2017.03.03
   *
   * @param   mode  配置モード
   */
  //-----------------------------------------------------------------------------
  void PutBgPane( BgPutMode mode );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetListData
   * @brief   リストデータをセット
   * @date    2017.03.03
   *
   * @param   no    データ番号
   * @param   data  リストデータ
   * @param   max   リストデータ数
   */
  //-----------------------------------------------------------------------------
  void SetListData( u32 no, const LIST_ITEM_DATA * data, u32 max );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsSetLabel
   * @brief   設定されているラベルデータがあるか
   * @date    2017.03.17
   *
   * @retval  true  = 有
   * @retval  false = 無
   */
  //-----------------------------------------------------------------------------
  bool IsSetLabel(void);


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新
   * @date    2017.03.02
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   * @date    2017.03.02
   *
   * @param   displayNo   描画面
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    Initialize2D
   * @brief   2D関連初期化
   * @date    2017.03.02
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
   * @date    2017.03.02
   */
  //-----------------------------------------------------------------------------
  void Terminate2D(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitPane
   * @brief   ペインの状態初期設定
   * @date    2017.03.03
   */
  //-----------------------------------------------------------------------------
  void InitPane(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetBgPaneAlpha
   * @brief   背景ペインにアルファを設定
   * @date    2017.03.03
   *
   * @param   alpha   アルファ値
   */
  //-----------------------------------------------------------------------------
  void SetBgPaneAlpha( f32 alpha );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetBgPaneAlpha
   * @brief   背景ペインのアルファを取得
   * @date    2017.03.17
   *
   * @return  アルファ値
   */
  //-----------------------------------------------------------------------------
  u32 GetBgPaneAlpha(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetTextPaneAlpha
   * @brief   テキストペインにアルファを設定
   * @date    2017.03.03
   *
   * @param   index   インデックス
   * @param   alpha   アルファ値
   */
  //-----------------------------------------------------------------------------
  void SetTextPaneAlpha( u32 index, f32 alpha );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutTextPane
   * @brief   デキストペインを配置
   * @date    2017.03.03
   *
   * @param   index   インデックス
   * @param   x       X座標
   * @param   y       Y座標
   */
  //-----------------------------------------------------------------------------
  void PutTextPane( u32 index, f32 x, f32 y );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetString
   * @brief   文字列書き込み
   * @date    2017.03.03
   *
   * @param   index   インデックス
   * @param   label   テキストラベル
   * @param   mode    配置モード
   */
  //-----------------------------------------------------------------------------
  void SetString( u32 index, u32 label, TextPutMode mode );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsVanishBg
   * @brief   背景が非表示か
   * @date    2017.03.03
   *
   * @retval  true  = 非表示
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsVanishBg(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitLabelWork
   * @brief   ラベルワークを初期化
   * @date    2017.03.03
   */
  //-----------------------------------------------------------------------------
  void InitLabelWork(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetLabelWork
   * @brief   ラベルワークをセット
   * @date    2017.03.03
   *
   * @param   data  リストデータ
   *
   * @retval  LABEL_WORK_NONE  = 失敗
   * @retval  LABEL_WORK_NONE != セットしたリストデータのインデックス
   */
  //-----------------------------------------------------------------------------
  u32 SetLabelWork( const LIST_ITEM_DATA * data );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetDoubleLabelWork
   * @brief   二重化用ラベルワークをセット
   * @date    2017.04.07
   *
   * @param   index   二重化元のワークインデックス
   *
   * @retval  LABEL_WORK_NONE  = 失敗
   * @retval  LABEL_WORK_NONE != セットしたリストデータのインデックス
   */
  //-----------------------------------------------------------------------------
  u32 SetDoubleLabelWork( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsActiveLabel
   * @brief   動作中のラベルデータがあるか
   * @date    2017.03.03
   *
   * @retval  true  = 有
   * @retval  false = 無
   */
  //-----------------------------------------------------------------------------
  bool IsActiveLabel(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsEndLabel
   * @brief   終了を発行しているラベルがあるか
   * @date    2017.03.03
   *
   * @retval  true  = 有
   * @retval  false = 無
   */
  //-----------------------------------------------------------------------------
  bool IsEndLabel(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsVanishBgLabel
   * @brief   背景を非表示にしているラベルデータがあるか
   * @date    2017.03.17
   *
   * @retval  true  = 有
   * @retval  false = 無
   */
  //-----------------------------------------------------------------------------
  bool IsVanishBgLabel(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateLabel
   * @brief   ラベル更新処理
   * @date    2017.03.03
   *
   * @param   index   ラベルワークインデックス
   */
  //-----------------------------------------------------------------------------
  void UpdateLabel( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateMain
   * @brief   全体の更新処理
   * @date    2017.03.03
   */
  //-----------------------------------------------------------------------------
  void UpdateMain(void);


private:
  app::util::Heap * m_heap;                             //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ

  gfl2::str::MsgData * m_pMsgData;  //!< メッセージデータ
  print::WordSet * m_pWordSet;      //!< 単語セット

  LABEL_WORK m_labelWork[TEXT_PANE_MAX];

  u16 m_bgSeq;
  u16 m_bgCount;

  u32 m_subSeq;         //!< サブシーケンス
  bool m_isDrawEnable;  //!< 描画フラグ



#if PM_DEBUG
public:
  //=============================================================================
  /**
   * @class Listener
   * @brief リスナークラス
   * @date  2017.03.02
   */
  //=============================================================================
  class Listener
  {
  public:
    //! 動作ID
    enum ActionID
    {
      ACTION_ID_END = 0,
      ACTION_ID_REQ_0,
      ACTION_ID_REQ_1,
      ACTION_ID_REQ_2,
    };

  public:
    //-----------------------------------------------------------------------------
    /**
     * @func    OnAction
     * @brief   動作リスナー
     * @date    2017.03.02
     *
     * @param   id    動作ID
     */
    //-----------------------------------------------------------------------------
    virtual void OnAction( ActionID id ) = 0;
  };

  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ（デバッグ用）
   *
   * @param   heap        ヒープ
   * @param   render_man  描画マネージャ
   * @param   listener    リスナー
   */
  //-----------------------------------------------------------------------------
  StaffListDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man, Listener * listener );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    OnKeyAction
   * @brief   キー入力の検知
   * @date    2017.01.25
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

  Listener * m_listener;  //!< リスナー
#endif // PM_DEBUG
};


GFL_NAMESPACE_END(StaffList)
GFL_NAMESPACE_END(App)

#endif  // STAFFLIST_DRAW_H_INCLUDED
