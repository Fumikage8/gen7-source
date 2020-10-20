//=============================================================================
/**
 * @file    NewPaneListPartsH.h
 * @brief   ペインリスト項目関連処理（横リスト）
 * @author  Hiroyuki Nakamura
 * @date    2015.07.22
 */
//=============================================================================
#if !defined( NEW_PANE_LIST_PARTS_H_H_INCUDED )
#define NEW_PANE_LIST_PARTS_H_H_INCUDED

#pragma once

#include "NewPaneListPartsBase.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

//=============================================================================
/**
 * @class NewPaneListPartsH
 * @brief ペインリスト項目処理クラス（横リスト）
 * @date  2015.07.22
 */
//=============================================================================
class NewPaneListPartsH
  : public NewPaneListPartsBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NewPaneListPartsH );

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
  NewPaneListPartsH(
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
  virtual ~NewPaneListPartsH();

  //-----------------------------------------------------------------------------
  /**
   * @func    InitListPut
   * @brief   リスト初期配置
   * @date    2015.07.22
   */
  //-----------------------------------------------------------------------------
  virtual void InitListPut(void);

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
  virtual f32 InitListPutBase( s32 mv );

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
  virtual f32 MoveList( s32 offs );

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
  virtual u32 GetPosPaneIndex( int pos );

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
  virtual s32 GetPosPaneLow( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeButtonEnable
   * @brief   ボタン入力可否を切り替える
   * @date    2015.07.30
   */
  //-----------------------------------------------------------------------------
  virtual void ChangeButtonEnable(void);

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
  virtual NewPaneListPartsBase::InputID CheckKeyInput(void);

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
  virtual NewPaneListPartsBase::InputID CheckKeyInput( gfl2::ui::VectorDevice * key );

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
  virtual NewPaneListPartsBase::InputID CheckKeyInputSkip( gfl2::ui::VectorDevice * key );

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
  virtual bool IsKeyHold( NewPaneListPartsBase::InputID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    StartItemChangeScroll
   * @brief   入れ替え用：スクロール開始
   * @date    2015.08.01
   *
   * @param   mv  スクロール方向
   */
  //-----------------------------------------------------------------------------
  virtual void StartItemChangeScroll( s32 mv );

  //-----------------------------------------------------------------------------
  /**
   * @func    MoveChangeList
   * @brief   入れ替え用：リスト動作
   * @date    2015.08.01
   *
   * @param   offs  移動値
   */
  //-----------------------------------------------------------------------------
  virtual void MoveChangeList( s32 offs );

  //-----------------------------------------------------------------------------
  /**
   * @func    AdjustChangeList
   * @brief   入れ替え用：リスト配置補正
   * @date    2015.08.01
   */
  //-----------------------------------------------------------------------------
  virtual void AdjustChangeList(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndItemChangeScroll
   * @brief   入れ替え用：スクロール終了
   * @date    2015.08.01
   */
  //-----------------------------------------------------------------------------
  virtual void EndItemChangeScroll(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitListPutItemChangeLoop
   * @brief   入れ替え用：ループ時のリスト配置
   * @date    2015.08.01
   */
  //-----------------------------------------------------------------------------
  virtual void InitListPutItemChangeLoop(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitListPutItemChangeSkip
   * @brief   入れ替え用：スキップ時のリスト配置
   * @date    2015.08.03
   */
  //-----------------------------------------------------------------------------
  virtual void InitListPutItemChangeSkip(void);


private:
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

};

GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

#endif  // NEW_PANE_LIST_PARTS_H_H_INCUDED
