//============================================================================================
/**
 * @file		app_tool_BoxMark.cpp
 * @brief		ボックスマーク表示関連
 * @author	Hiroyuki Nakamura
 * @date		12.09.25
 */
//============================================================================================
#include <AppLib/include/Tool/app_tool_BoxMark.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace	app::tool
 * @brief			アプリツール
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace app {
namespace tool {

static const u32 MARK_ON = 0xff;		//!< マークON時のアルファ値
static const u32 MARK_OFF = 0x7f;		//!< マークOFF時のアルファ値


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
void SetBoxMarkPane(
			pml::pokepara::BoxMark markID, const pml::pokepara::BoxMarkContainer& param,
			gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytPaneIndex pane_index )

{
	gfl2::lyt::LytPane * pane = lytwk->GetPane( pane_index );

  if( param.markColor[ markID ] != pml::pokepara::BOX_MARK_COLOR_NONE ){
		pane->SetAlpha( MARK_ON );
	}else{
		pane->SetAlpha( MARK_OFF );
	}
}

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
void SetBoxMarkPane(
			pml::pokepara::BoxMark markID, const pml::pokepara::CoreParam * cp,
			gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytPaneIndex pane_index )
{
  pml::pokepara::BoxMarkContainer boxMark;
  cp->GetAllBoxMark( &boxMark );
  SetBoxMarkPane( markID, boxMark, lytwk, pane_index );
}

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
void SetBoxMarkPane(
      const pml::pokepara::BoxMarkContainer& param, gfl2::lyt::LytWk * lytwk, const gfl2::lyt::LytPaneIndex * pane_tbl )
{
	for( int i=0; i<static_cast<int>(pml::pokepara::BOX_MARK_NUM); i++ ){
		SetBoxMarkPane( static_cast<pml::pokepara::BoxMark>(i), param, lytwk, pane_tbl[i] );
	}
}

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
void SetBoxMarkPane(
			const pml::pokepara::CoreParam * cp,
			gfl2::lyt::LytWk * lytwk, const gfl2::lyt::LytPaneIndex * pane_tbl )
{
  pml::pokepara::BoxMarkContainer boxMark;
  cp->GetAllBoxMark( &boxMark );
  SetBoxMarkPane( boxMark, lytwk, pane_tbl );
}


}	// tool
}	// app
