#ifndef NIJI_PROJECT_APP_LANG_SELECT_APP_PARAM_H_INCLUDED
#define NIJI_PROJECT_APP_LANG_SELECT_APP_PARAM_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file LangSelectAppParam.h
 * @date 2015/07/23 15:19:02
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(LangSelect)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
//! セーブデータの状態
enum SAVE_STATUS{
	SAVE_STATUS_TRUE = 0,				//!< セーブデータあり
	SAVE_STATUS_FALSE,					//!< セーブデータなし
	SAVE_STATUS_ERROR,					//!< セーブデータエラー
	SAVE_STATUS_REWIND_ERROR,		//!< 巻き戻し検出エラー
	SAVE_STATUS_REWIND_WARNING,	//!< 巻き戻し検出（ただし復帰可能）
	SAVE_STATUS_NEED_FORMAT,		//!< 未初期化状態
};

//! 拡張セーブデータの状態
enum EX_SAVE_STATUS{
	EX_SAVE_STATUS_TRUE = 0,			//!< 拡張セーブデータあり
	EX_SAVE_STATUS_FALSE,					//!< 拡張セーブデータなし
	EX_SAVE_STATUS_BROKEN_DATA,		//!< 拡張セーブデータエラー　壊れている
	EX_SAVE_STATUS_NOT_FOUND,			//!< 拡張セーブデータエラー　見つからない
};

//------------------------------------------------------------------------------
// @brief   外部設定パラメータ
//------------------------------------------------------------------------------
struct APP_PARAM
{
  // [in]
  u8 defaultLang;   //!< レポートがある場合はレポートの言語、ない場合は本体の言語
  SAVE_STATUS saveStatus;   //!< セーブデータの状態
  EX_SAVE_STATUS exSaveStatus;    //!< 拡張セーブデータの状態

  // [out]
  u8   selectLang;    //!< 選択した言語
  bool kanjiFlg;      //!< 漢字フラグ
  bool requestFormatSaveData;   //!< セーブデータ消去がリクエストされたか？

  bool debugLangSelect;         //!< デバッグモードでの起動フラグ

  // [in]
  bool debugFlag;   //!< デバッグ用フラグ

  // コンストラクタ
  APP_PARAM( void )
    : saveStatus( SAVE_STATUS_TRUE )
    , exSaveStatus( EX_SAVE_STATUS_TRUE )
    , kanjiFlg( false )
    , requestFormatSaveData( false )
    , debugLangSelect( false )
    , debugFlag( false )
  { ; }
};

GFL_NAMESPACE_END(LangSelect)
GFL_NAMESPACE_END(App)

#endif // NIJI_PROJECT_APP_LANG_SELECT_APP_PARAM_H_INCLUDED