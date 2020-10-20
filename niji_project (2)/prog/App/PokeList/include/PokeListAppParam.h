#ifndef NIJI_PROJECT_APP_POKELIST_APPPARAM_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_APPPARAM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    PokeListAppParam.h
 * @author  fukushima_yuya
 * @date    2015.03.23
 * @brief   
 */
//==============================================================================


#include <macro/include/gfl2_Macros.h>

//-----------------------------------------------------------------------------
// @brief   前方宣言
//-----------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class StaticWork;
class TimeLimit;
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class AppCommonGrpIconData;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_BEGIN( util )
class AppRenderingManager;
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( Fade )
class FadeManager;
GFL_NAMESPACE_END( Fade )
GFL_NAMESPACE_BEGIN( fs )
class AsyncFileManager;
GFL_NAMESPACE_END( fs )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( GameSys )
class GameManager;
GFL_NAMESPACE_END( GameSys )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//-----------------------------------------------------------------------------
/**
 *					定数宣言
 */
//-----------------------------------------------------------------------------

/// 外部設定パラメータ（初期化データ・終了時のデータなど）
struct APP_PARAM {
	// [ in ]
  gfl2::fs::AsyncFileManager* mpAsyncFileManager;   //ファイルマネージャー
  app::util::AppRenderingManager* mpAppRenderingManager;   //レンダリングマネージャー
  app::util::Heap* mpAppHeap;  //appheap
  GameSys::GameManager* mpGameManager;
  gfl2::Fade::FadeManager* mpFadeManager;
  app::tool::AppCommonGrpIconData* mpCmnIcon;
  // [ out ]
	//u8 outNo;			            ///< 選んだ番号
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )


#endif  // #ifndef NIJI_PROJECT_APP_POKELIST_APPPARAM_H_INCLUDED
