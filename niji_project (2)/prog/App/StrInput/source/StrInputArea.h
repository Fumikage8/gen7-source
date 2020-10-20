#ifndef NIJI_PROJECT_APP_STRINPUT_AREA_H_INCLUDED
#define NIJI_PROJECT_APP_STRINPUT_AREA_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    StrInputArea.h
 * @brief   入力エリア関連
 * @author  fukushima_yuya
 * @date    2015.05.22
 * @note    sango_project app strinput_area.hから移植
 */
//==============================================================================

// gflib2
#include <Layout/include/gfl2_Layout.h>
#include <str/include/gfl2_Str.h>
#include <ui/include/gfl2_UI.h>


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace  App::StrInput
 * @brief      文字入力画面
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( StrInput )


//------------------------------------------------------------------------------
/**
 * @brief   入力エリア
 */
//------------------------------------------------------------------------------
class InputArea
{
public:
  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   lytwk   レイアウトワーク
   * @param   lytwk   レイアウトワーク
   * @param   tbl     ペインテーブル
   * @param   buf     入力文字列
   * @param   siz     入力文字数
   */
  //----------------------------------------------------------------------------
  InputArea( gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytMultiResID resid, const gfl2::lyt::LytPaneIndex * tbl, const gfl2::str::STRCODE * buf, u32 siz );

  //----------------------------------------------------------------------------
  /**
   * @brief    デストラクタ
   */
  //----------------------------------------------------------------------------
  ~InputArea( void );

  //----------------------------------------------------------------------------
  /**
   * @brief    入力エリア選択
   *
   * @param    tp    タッチパネル
   *
   * @retval  "true = エリア選択処理中"
   * @retval  "false = それ以外"
   */
  //----------------------------------------------------------------------------
  bool CheckInputArea( gfl2::ui::TouchPanel * tp );

  //----------------------------------------------------------------------------
  /**
   * @brief    入力エリアの範囲を取得
   *
   * @param    start_pos  開始位置格納場所
   * @param    end_pos    終了位置格納場所
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void GetAreaPos( u16 * start_pos, u16 * end_pos );

  //----------------------------------------------------------------------------
  /**
   * @brief    入力エリアサイズを取得
   *
   * @param    none
   *
   * @return  入力エリアサイズ
   */
  //----------------------------------------------------------------------------
  u32  GetAreaSize( void );


private:
  //----------------------------------------------------------------------------
  /**
   * @brief    タッチ座標のペインテーブルのインデックスを取得
   *
   * @param    tp  タッチパネル
   *
   * @return  ペインテーブルのインデックス
   */
  //----------------------------------------------------------------------------
  u32 HitCheck( gfl2::ui::TouchPanel * tp );


private:
  const gfl2::lyt::LytPaneIndex * areaTbl;    //!< 入力エリアのペインテーブル
  const gfl2::str::STRCODE * str;             //!< 入力された文字列
  u32  tblSize;                               //!< ペインテーブルサイズ（入力文字数）

  gfl2::lyt::LytWk * lytWk;         //!< レイアウトワーク
  gfl2::lyt::LytMultiResID resID;   //!< リソースID
  bool flg;                         //!< 範囲選択中フラグ
  u16  start;                       //!< 入力開始位置
  u16  end;                         //!< 入力終了位置

  u16  sx, sy;     //!< 範囲選択開始座標
  u16  ex, ey;     //!< 範囲選択終了座標
};


GFL_NAMESPACE_END( StrInput )
GFL_NAMESPACE_END( App )

#endif // #ifndef NIJI_PROJECT_APP_STRINPUT_AREA_H_INCLUDED
