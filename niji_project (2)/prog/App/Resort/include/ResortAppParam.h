#ifndef __RESORT_APP_PARAM_H__
#define __RESORT_APP_PARAM_H__
//=============================================================================
/**
 * @file	  ResortAppParam.h
 * @brief	  
 * @author  k.ohno
 * @date	  2015.5.24
 */
//=============================================================================
#pragma once

#include <pml/include/pmlib.h>

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( fs )
class AsyncFileManager;
GFL_NAMESPACE_END( fs )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class AppRenderingManager;
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( ui )
class UIView;
GFL_NAMESPACE_END( ui )
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN( applib )
GFL_NAMESPACE_BEGIN( frame )
class Manager;
GFL_NAMESPACE_END( frame )
GFL_NAMESPACE_END( applib )

GFL_NAMESPACE_BEGIN( AppLib )
GFL_NAMESPACE_BEGIN( Tool )
class PokeCapAccesser;
GFL_NAMESPACE_END( Tool )
GFL_NAMESPACE_END( AppLib )



GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( Skybox )
class Skybox;
GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( System )


GFL_NAMESPACE_BEGIN( Savedata )
class ResortSave;
class BoxPokemon;
class MyStatus;
class BOX;
GFL_NAMESPACE_END( Savedata )


GFL_NAMESPACE_BEGIN( Field )
class EventWork;
GFL_NAMESPACE_END( Field )



GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )

enum{
  NEXTSEQ_START,          //  開始
  NEXTSEQ_END,            //  終了
  NEXTSEQ_BOX_GIM,        // 運動リゾートの看板クリック あずけ
  NEXTSEQ_BOX_GIM_RE,     // 運動リゾートの看板クリック ひきとり
  NEXTSEQ_BOX_HOTSPA,     // おんせんのかんばんくりっく
  NEXTSEQ_BOX_HOTSPA_RE,  // おんせんのかんばんくりっく ひきとり
  NEXTSEQ_ZUKAN,          // 図鑑
  NEXTSEQ_TAMAGO,         // タマゴ孵化
  NEXTSEQ_STATUS,         // ポケモンステータス
};


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
  applib::frame::Manager* mpAppFrameManager;
  System::Skybox::Skybox* mpSkybox;
  app::ui::UIView* mpUIView;          // 
  Savedata::ResortSave* pSave;        //
  Savedata::BoxPokemon* pBoxPokemon;
  Savedata::BOX* pBox;  
  Field::EventWork* pEventSave;
  Savedata::MyStatus* pMyStatus;
  int firstOpenPos;                   // 最初に開く場所
  int selectPokemonNo;                // 選んだポケモン番号
  int inputEnable;                   // 入力有効フラグ
  int isStart;   //開始時のみ、true
  int bgmReq;   //開始時のみＢＧＭ再生のためのフラグ
  //[temp]
  AppLib::Tool::PokeCapAccesser* pPokeCapAccesser;
  pml::PokeParty* pPokeParty;           // ぽけもんのデータ 手持ちではない
  int pokemonSelecetMonsNo;   //タッチしたポケモンのモンスター番号
  int groundNo;  //地形データの場所
  int nadenadeNo;
#if PM_DEBUG
  int debugCameraMode;
  int debugFullMode;  //リゾートデータをFULLでうめる
#endif
  int pokemonSelectIndex;   // タッチしたポケモンのインデックス  ポケモンデータが欲しい場合pPokePartyをつかって

  // [ in/out ]
  int seqReq;  //次のシーケンス
  int selectBoxGroup;  //ボックスグループ番号
  int selectGroup;      //選択したグループ番号
};


GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )

#endif //__RESORT_APP_PARAM_H__


