#ifndef NIJI_PROJECT_APP_LANG_SELECT_BUTTON_H_INCLUDED
#define NIJI_PROJECT_APP_LANG_SELECT_BUTTON_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file LangSelectButton.h
 * @date 2015/07/23 18:40:20
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "App/LangSelect/source/LangSelectConst.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( LangSelect )

//------------------------------------------------------------------------------
// @brief   言語選択ボタン
//------------------------------------------------------------------------------
class LangSelectButton
{
  GFL_FORBID_COPY_AND_ASSIGN( LangSelectButton );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   * @param   pBase     描画共通処理クラス
   * @param   langData  言語データ
   */
  //------------------------------------------------------------------
  LangSelectButton( DrawerBase* pBase, LangData langData[] );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~LangSelectButton( void ) { ; }

private:
  //------------------------------------------------------------------
  /**
   * @brief   ボタンの位置の設定
   */
  //------------------------------------------------------------------
  void SetButtonPos( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   次の項目へ
   */
  //------------------------------------------------------------------
  void Next( void );

  //------------------------------------------------------------------
  /**
   * @brief   前の項目へ
   */
  //------------------------------------------------------------------
  void Prev( void );

  //------------------------------------------------------------------
  /**
   * @brief   選択しているボタンインデックスを取得
   * @return  インデックス(0 ～ LANG_ID_MAX)
   */
  //------------------------------------------------------------------
  u32 GetSelectBtn( void ) const;

private:
  DrawerBase*             m_pBase;

  LangData                m_LangData[LANG_ID_MAX];

  u32                     m_Index;

  gfl2::lyt::LytParts*    m_pBtnParts[LANG_ID_MAX];
};

GFL_NAMESPACE_END( LangSelect )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_LANG_SELECT_BUTTON_H_INCLUDED