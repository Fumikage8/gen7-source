//============================================================================================
/**
 * @file		app_tool_BoxMark.h
 * @brief		ボックスマーク表示関連
 * @author	Hiroyuki Nakamura
 * @date		12.09.25
 */
//============================================================================================
#if !defined( __APP_TOOL_BOXMARK_H__ )
#define __APP_TOOL_BOXMARK_H__

#include <pml/include/pmlib.h>
#include <Layout/include/gfl2_Layout.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace	app::tool
 * @brief			アプリツール
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace app {
namespace tool {

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボックスマークの個別表示（パラメータ指定）
 *
 * @param		markID			マークID
 * @param		param				ポケモンが持っているパラメータ
 * @param		lytwk				レイアウトワーク
 * @param		pane_index	ペインのインデックス
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void SetBoxMarkPane(
							pml::pokepara::BoxMark markID, const pml::pokepara::BoxMarkContainer& param,
							gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytPaneIndex pane_index );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボックスマークの個別表示（CoreParam指定）
 *
 * @param		markID			マークID
 * @param		cp					CoreParam
 * @param		lytwk				レイアウトワーク
 * @param		pane_index	ペインのインデックス
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void SetBoxMarkPane(
							pml::pokepara::BoxMark markID, const pml::pokepara::CoreParam * cp,
							gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytPaneIndex pane_index );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボックスマークの表示（パラメータ指定）
 *
 * @param		param				ポケモンが持っているパラメータ
 * @param		lytwk				レイアウトワーク
 * @param		pane_tbl		ペインのインデックステーブル
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void SetBoxMarkPane(
							const pml::pokepara::BoxMarkContainer& param,
							gfl2::lyt::LytWk * lytwk, const gfl2::lyt::LytPaneIndex * pane_tbl );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボックスマークの表示（CoreParam指定）
 *
 * @param		cp					CoreParam
 * @param		lytwk				レイアウトワーク
 * @param		pane_tbl		ペインのインデックステーブル
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void SetBoxMarkPane(
							const pml::pokepara::CoreParam * cp,
							gfl2::lyt::LytWk * lytwk, const gfl2::lyt::LytPaneIndex * pane_tbl );

}	// tool
}	// app

#endif // __APP_TOOL_BOXMARK_H__
