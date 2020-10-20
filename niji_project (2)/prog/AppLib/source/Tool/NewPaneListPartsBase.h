﻿//=============================================================================
/**
 * @file    NewPaneListPartsBase.h
 * @brief   ペインリスト項目関連処理
 * @author  Hiroyuki Nakamura
 * @date    2015.07.22
 */
//=============================================================================
#if !defined( NEW_PANE_LIST_PARTS_BASE_H_INCUDED )
#define NEW_PANE_LIST_PARTS_BASE_H_INCUDED

#pragma once

#include "AppLib/include/Tool/AppToolNewPaneList.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

//=============================================================================
/**
 * @class NewPaneListPartsBase
 * @brief ペインリスト項目処理クラス
 * @date  2015.07.22
 */
//=============================================================================
class NewPaneListPartsBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NewPaneListPartsBase );

public:
  //! 入力ID
  enum InputID
  {
    INPUT_NONE = 0,   //!< 入力なし
    INPUT_PLUS,       //!< ＋方向
    INPUT_MINUS,      //!< －方向
  };

  static const s32 ITEM_CHANGE_OFF = -1;  //!< @note 未使用


protected:
  static const u32 EFFECT_PARAM = 255;  //!< デフォルトエフェクト値


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   param           セットアップパラメータ
   * @param   listener        リスナー
   * @param   button_manager  ボタンマネージャ
   * @param   insert_pane     並び替え時の基準位置ペイン
   * @param   low_tbl         ペインの並びテーブル
   * @param   scroll_max      スクロール最大値
   * @param   effect          エフェクトモード
   */
  //-----------------------------------------------------------------------------
  NewPaneListPartsBase(
    const NewPaneList::SETUP_PARAM * param,
    NewPaneList::ListListener * listener,
    app::tool::ButtonManager * button_manager,
    gfl2::lyt::LytPane * insert_pane,
    u8 * low_tbl,
    f32 scroll_max,
    NewPaneList::EffectMode effect );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~NewPaneListPartsBase();

  //-----------------------------------------------------------------------------
  /**
   * @func    SetBasePane
   * @brief   リスト開始座標のペインを動作基準ペインに設定
   * @date    2015.07.24
   */
  //-----------------------------------------------------------------------------
  void SetBasePane(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetDeviceKey
   * @brief   十字キーデバイスを設定
   * @date    2015.07.29
   *
   * @param   key   十字キーデバイス
   */
  //-----------------------------------------------------------------------------
  void SetDeviceKey( gfl2::ui::VectorDevice * key );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleCursor
   * @brief   カーソル表示切り替え
   * @date    2015.07.29
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleCursor( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutCursor
   * @brief   カーソル配置
   * @date    2015.07.29
   *
   * @param   pos         配置位置
   * @param   is_visible  true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void PutCursor( u32 pos, bool is_visible );

  //-----------------------------------------------------------------------------
  /**
   * @func    CheckKeyInputSkip
   * @brief   スキップ入力をチェック
   * @date    2015.07.30
   *
   * @param   button  ボタンデバイス
   *
   * @retval  INPUT_NONE  = 入力なし
   * @retval  INPUT_PLUS  = ＋方向
   * @retval  INPUT_MINUS = －方向
   */
  //-----------------------------------------------------------------------------
  InputID CheckKeyInputSkip( gfl2::ui::Button * button );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetItemChange
   * @brief   入れ替え処理
   * @date    2015.08.01
   */
  //-----------------------------------------------------------------------------
  void SetItemChange(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CancelItemChange
   * @brief   入れ替えキャンセル
   * @date    2015.08.04
   */
  //-----------------------------------------------------------------------------
  void CancelItemChange(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsItemChange
   * @brief   入れ替え処理中か
   * @date    2015.08.01
   *
   * @retval  true  = 処理中
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsItemChange(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetItemChangePos
   * @brief   入れ替え位置を取得
   * @date    2015.08.01
   *
   * @return  入れ替え位置
   */
  //-----------------------------------------------------------------------------
  u32 GetItemChangePos(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SwapItemChangePane
   * @brief   入れ替え：ペインを入れ替える（方向指定）
   * @date    2015.08.01
   *
   * @param   mv  入れ替え方向
   */
  //-----------------------------------------------------------------------------
  void SwapItemChangePane( s32 mv );

  //-----------------------------------------------------------------------------
  /**
   * @func    SwapItemChangePane
   * @brief   入れ替え：ペインを入れ替える（範囲指定）
   * @date    2015.08.03
   *
   * @param   p1  開始ペイン位置
   * @param   p2  終了ペイン位置
   */
  //-----------------------------------------------------------------------------
  void SwapItemChangePane( s32 p1, s32 p2 );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateChangeLowTable
   * @brief   入れ替え用のテーブルを生成
   * @date    2015.08.03
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  void CreateChangeLowTable( app::util::Heap * heap );

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteChangeLowTable
   * @brief   入れ替え用のテーブルを削除
   * @date    2015.08.03
   */
  //-----------------------------------------------------------------------------
  void DeleteChangeLowTable(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SwapChangeLowTable
   * @brief   入れ替え用のテーブル入れ替え
   * @date    2015.08.03
   *
   * @param   p1  位置１
   * @param   p2  位置２
   */
  //-----------------------------------------------------------------------------
  void SwapChangeLowTable( s32 p1, s32 p2 );

  //-----------------------------------------------------------------------------
  /**
   * @func    LoopChangeLowTable
   * @brief   入れ替え用のテーブルをループ
   * @date    2015.08.03
   *
   * @param   mv  ループ方向
   */
  //-----------------------------------------------------------------------------
  void LoopChangeLowTable( s32 mv );

  //-----------------------------------------------------------------------------
  /**
   * @func    SkipChangeLowTable
   * @brief   入れ替え用のテーブルをスキップ
   * @date    2015.08.03
   *
   * @param   offs  スキップする値
   * @param   mv    方向
   */
  //-----------------------------------------------------------------------------
  void SkipChangeLowTable( s32 offs, s32 mv );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitListPut
   * @brief   リスト初期配置
   * @date    2015.07.22
   */
  //-----------------------------------------------------------------------------
  virtual void InitListPut(void) = 0;

  //-----------------------------------------------------------------------------
  /**
   * @func    InitListPutBase
   * @brief   基準ペインからリストを配置
   * @date    2015.07.24
   *
   * @param   mv  配置方向
   *
   * @return  スクロール値
   */
  //-----------------------------------------------------------------------------
  virtual f32 InitListPutBase( s32 mv ) = 0;

  //-----------------------------------------------------------------------------
  /**
   * @func    MoveList
   * @brief   リスト移動
   * @date    2015.07.24
   *
   * @param   offs  移動値
   *
   * @return  スクロール値
   */
  //-----------------------------------------------------------------------------
  virtual f32 MoveList( s32 offs ) = 0;

  //-----------------------------------------------------------------------------
  /**
   * @func    GetPosPaneIndex
   * @brief   リスト位置のペインを取得
   * @date    2015.07.29
   *
   * @param   リスト位置
   *
   * @return  ペインのインデックス
   */
  //-----------------------------------------------------------------------------
  virtual u32 GetPosPaneIndex( int pos ) = 0;

  //-----------------------------------------------------------------------------
  /**
   * @func    GetPosPaneLow
   * @brief   ペインの配置位置を取得
   * @date    2015.07.29
   *
   * @param   index   ペインのインデックス
   *
   * @return  配置位置
   */
  //-----------------------------------------------------------------------------
  virtual s32 GetPosPaneLow( u32 index ) = 0;

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeButtonEnable
   * @brief   ボタン入力可否を切り替える
   * @date    2015.07.30
   */
  //-----------------------------------------------------------------------------
  virtual void ChangeButtonEnable(void) = 0;

  //-----------------------------------------------------------------------------
  /**
   * @func    CheckKeyInput
   * @brief   キー入力チェック
   * @date    2015.07.29
   *
   * @retval  INPUT_NONE  = 入力なし
   * @retval  INPUT_PLUS  = ＋方向
   * @retval  INPUT_MINUS = －方向
   */
  //-----------------------------------------------------------------------------
  virtual InputID CheckKeyInput(void) = 0;

  //-----------------------------------------------------------------------------
  /**
   * @func    CheckKeyInput
   * @brief   キー入力チェック
   * @date    2015.07.29
   *
   * @param   key   十字キーデバイス
   *
   * @retval  INPUT_NONE  = 入力なし
   * @retval  INPUT_PLUS  = ＋方向
   * @retval  INPUT_MINUS = －方向
   */
  //-----------------------------------------------------------------------------
  virtual InputID CheckKeyInput( gfl2::ui::VectorDevice * key ) = 0;

  //-----------------------------------------------------------------------------
  /**
   * @func    CheckKeyInputSkip
   * @brief   スキップ入力チェック
   * @date    2015.07.30
   *
   * @param   key   十字キーデバイス
   *
   * @retval  INPUT_NONE  = 入力なし
   * @retval  INPUT_PLUS  = ＋方向
   * @retval  INPUT_MINUS = －方向
   */
  //-----------------------------------------------------------------------------
  virtual InputID CheckKeyInputSkip( gfl2::ui::VectorDevice * key ) = 0;

  //-----------------------------------------------------------------------------
  /**
   * @func    IsKeyHold
   * @brief   指定方向にキーが入力されているか
   * @date    2015.07.29
   *
   * @param   id  入力ID
   *
   * @retval  true  = 入力されている
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  virtual bool IsKeyHold( InputID id ) = 0;

  //-----------------------------------------------------------------------------
  /**
   * @func    StartItemChangeScroll
   * @brief   入れ替え用：スクロール開始
   * @date    2015.08.01
   *
   * @param   mv  スクロール方向
   */
  //-----------------------------------------------------------------------------
  virtual void StartItemChangeScroll( s32 mv ) = 0;

  //-----------------------------------------------------------------------------
  /**
   * @func    MoveChangeList
   * @brief   入れ替え用：リスト動作
   * @date    2015.08.01
   *
   * @param   offs  移動値
   */
  //-----------------------------------------------------------------------------
  virtual void MoveChangeList( s32 offs ) = 0;

  //-----------------------------------------------------------------------------
  /**
   * @func    AdjustChangeList
   * @brief   入れ替え用：リスト配置補正
   * @date    2015.08.01
   */
  //-----------------------------------------------------------------------------
  virtual void AdjustChangeList(void) = 0;

  //-----------------------------------------------------------------------------
  /**
   * @func    EndItemChangeScroll
   * @brief   入れ替え用：スクロール終了
   * @date    2015.08.01
   */
  //-----------------------------------------------------------------------------
  virtual void EndItemChangeScroll(void) = 0;

  //-----------------------------------------------------------------------------
  /**
   * @func    InitListPutItemChangeLoop
   * @brief   入れ替え用：ループ時のリスト配置
   * @date    2015.08.01
   */
  //-----------------------------------------------------------------------------
  virtual void InitListPutItemChangeLoop(void) = 0;

  //-----------------------------------------------------------------------------
  /**
   * @func    InitListPutItemChangeSkip
   * @brief   入れ替え用：スキップ時のリスト配置
   * @date    2015.08.03
   */
  //-----------------------------------------------------------------------------
  virtual void InitListPutItemChangeSkip(void) = 0;


protected:
  //-----------------------------------------------------------------------------
  /**
   * @func    MakePaneLow
   * @brief   ペインの座標順にテーブルを並び替える
   * @date    2015.07.30
   */
  //-----------------------------------------------------------------------------
  void MakePaneLow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetPaneLowIndex
   * @brief   ペインの並び位置を取得
   * @date    2015.08.01
   *
   * @param   index   ペインのインデックス
   *
   * @return  並び位置
   */
  //-----------------------------------------------------------------------------
  u32 GetPaneLowIndex( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangePaneLow
   * @brief   入れ替え用：ペイン並び替え
   * @date    2015.07.30
   */
  //-----------------------------------------------------------------------------
  void ChangePaneLow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InsertPane
   * @brief   ペインの表示優先を変更
   * @date    2015.08.04
   *
   * @param   index   ペインのインデックス
   */
  //-----------------------------------------------------------------------------
  void InsertPane( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetEffect
   * @brief   範囲外のペインにエフェクトをかける
   * @date    2015.07.31
   */
  //-----------------------------------------------------------------------------
  void SetEffect(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetEffectParam
   * @brief   エフェクト値を取得
   * @date    2015.07.31
   *
   * @param   p1      座標１
   * @param   p2      座標２
   * @param   space   配置スペース
   *
   * @return  エフェクト値
   */
  //-----------------------------------------------------------------------------
  u32 GetEffectParam( f32 p1, f32 p2, f32 space );

  //-----------------------------------------------------------------------------
  /**
   * @func    CalcEffectParam
   * @brief   エフェクト値を計算
   * @date    2015.07.31
   *
   * @param   index   ペインのインデックス
   *
   * @return  エフェクト値
   */
  //-----------------------------------------------------------------------------
  virtual u32 CalcEffectParam( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    ScrollChangeLowTable
   * @brief   入れ替え用テーブルをスクロール
   * @date    2015.08.03
   *
   * @param   mv  スクロール方向
   *
   * @return  回り込んだペインのインデックス
   */
  //-----------------------------------------------------------------------------
  s32 ScrollChangeLowTable( s32 mv );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    SetEffectAlpha
   * @brief   ペインに半透明エフェクトをかける
   * @date    2015.07.31
   */
  //-----------------------------------------------------------------------------
  void SetEffectAlpha(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetEffectColor
   * @brief   ペインにカラーエフェクトをかける
   * @date    2015.07.31
   */
  //-----------------------------------------------------------------------------
  void SetEffectColor(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitChangeLowTable
   * @brief   入れ替え用のテーブルを初期化
   * @date    2015.08.03
   */
  //-----------------------------------------------------------------------------
  void InitChangeLowTable(void);


protected:
  const NewPaneList::SETUP_PARAM * m_param;     //!< セットアップパラメータ
  NewPaneList::ListListener * m_listener;       //!< リスナー
  app::tool::ButtonManager * m_buttonManager;   //!< ボタンマネージャ

  gfl2::lyt::LytPane * m_insertPane;    //!< 並び替えの基準ペイン

  u8 * m_paneLowTable;  //!< ペインの並びテーブル

  f32 m_scrollMax;  //!< スクロール最大値

  NewPaneList::EffectMode m_effect;   //!< エフェクトモード

  u32 m_basePaneIndex;      //!< 動作基準ペインのインデックス

  u32 m_itemChangePane;     //!< 入れ替え時のペイン
  u32 m_itemChangePos;      //!< 入れ替え時のペインのリスト位置
  bool m_isItemChange;      //!< true = 入れ替え中
  s32 * m_changeLowTable;   //!< 入れ替え用テーブル
  u32 m_changeLowTableMax;  //!< 入れ替え用テーブル要素数

  gfl2::ui::VectorDevice * m_key;   //!< 十字キーデバイス
};

GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

#endif  // NEW_PANE_LIST_PARTS_BASE_H_INCUDED
