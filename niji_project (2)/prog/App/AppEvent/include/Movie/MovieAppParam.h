//===================================================================
/**
 *	@file	    MovieAppParam.h
 *	@brief    ムービーのパラメータ
 *	@author	  k.ohno
 *	@date		  2015.10.25
 *
 */
//===================================================================
#ifndef __MOVIE_APP_PARAM_H__
#define __MOVIE_APP_PARAM_H__
#pragma once




GFL_NAMESPACE_BEGIN( movie )
class MovieProc;
GFL_NAMESPACE_END( movie )




GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Event )

enum{
  MOVIE_TITLE_JAPAN,
  MOVIE_TITLE_ENGLISH,
  MOVIE_TITLE_FRANCE,
  MOVIE_TITLE_ITALY,
  MOVIE_TITLE_GERMANY,
  MOVIE_TITLE_SPAIN,
  MOVIE_TITLE_KOREA,
  MOVIE_TITLE_CHINA,
  MOVIE_TITLE_TAIWAN,
};

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
/// 外部設定パラメータ（初期化データ・終了時のデータなど）
struct APP_PARAM {
	// [ in ]
  int movieNo;
  // ムービーのファイルのインデックス 上に定義を書きました
  // [ in/out ]


};

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------



GFL_NAMESPACE_END( Event )
GFL_NAMESPACE_END( App )

#endif // __MOVIE_APP_PARAM_H__

