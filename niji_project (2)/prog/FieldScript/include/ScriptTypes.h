//=============================================================================
/**
 * @file  ScriptType.h
 * @brief フィールドスクリプトの種類指定
 * @date    2015.02.28
 * @author  iwasawa GAMEFREAK inc.
 */
//=============================================================================
#ifndef __SCRIPTYPE_H__
#define __SCRIPTYPE_H__
#pragma once

#include "niji_reference_files/script/FieldPawnTypes.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )

//-----------------------------------------------------------------------------
/// スクリプトの大別
//-----------------------------------------------------------------------------
enum SCRIPT_TYPE
{
  SCRIPT_TYPE_NONE = 0,
  SCRIPT_TYPE_ZONE,
  SCRIPT_TYPE_COMMON,
  SCRIPT_TYPE_ZONEINIT,

  SCRIPT_TYPE_NUM
};

//-----------------------------------------------------------------------------
/// スクリプトID型
//-----------------------------------------------------------------------------
typedef s32 SCRIPTID;

enum
{
  SCRID_DUMMY = 2000,       ///< 仮定義
};

//--------------------------------------------------------------
/// コールバックスクリプトのタイプ指定
//--------------------------------------------------------------
enum SCR_CALLBACK_TYPE
{
  SCR_CALLBACK_TYPE_NOTHING,        ///<存在しない

  SCR_CALLBACK_TYPE_TRAINER_AFTER,  ///<トレーナー戦後呼び出し

  SCR_CALLBACK_TYPE_NUM
};

//--------------------------------------------------------------
// ▼追加手順
// 1:.\src\funcset_xxx.inc というファイルを追加する
// 2:下のenumに追加
// 3:prog\src\field\script に ScriptFuncSetxxx.cpp というファイルを追加する。
// 4:prog\src\field\script に ScriptFuncSetxxx.h というファイルを追加する。
// 5:prog\src\field\script\ScriptObject.cpp の RegistFuncに判定を追加する。
//
// ※このファイルのオリジナルは sango_field/convert/script/funcset_id.hです
// 　prog/src/field/scriptフォルダ内のものはmake時にコピーされるものなので
//　 定義を追加したい時はオリジナルのほうに追加してください
// 定義場所は仮。専用ヘッダを作って、コンバート環境とプログラム環境で共有する
//==============================================================
enum
{ 
  FUNCSET_BIT_NONE        = 0,         ///< なし
  FUNCSET_BIT_INIT        = 1 << 0,    ///< 初期化スクリプト用命令セット（通常の.pでも読み込む共通命令セット）
  FUNCSET_BIT_COMMON      = 1 << 1,    ///< 汎用命令セット(初期化スクリプトでは登録しない)
  FUNCSET_BIT_JOIN_FESTA  = 1 << 2,    ///< ジョインフェスタ命令セット(初期化スクリプトでは登録しない)
  FUNCSET_BIT_ITEM        = 1 << 3,    ///< アイテム操作命令セット
  FUNCSET_BIT_POKECEN     = 1 << 4,    ///< ポケセン用命令セット
  FUNCSET_BIT_PROG        = 1 << 5,    ///< プログラマ専用命令セット
  FUNCSET_BIT_PROG_FIELD  = 1 << 6,    ///< プログラマ専用フィールド限定命令セット
  FUNCSET_BIT_SODATEYA    = 1 << 7,    ///< 育て屋命令セット
  FUNCSET_BIT_BATTLE_INST = 1 << 8,    ///< バトル施設命令セット
};

//-----------------------------------------------------------------------------
/**
 * @brief スクリプト用デバッグ出力
 */
//-----------------------------------------------------------------------------
#if defined(DEBUG_ONLY_FOR_hosaka_genya) || defined(DEBUG_ONLY_FOR_tamada) || defined(DEBUG_ONLY_FOR_iwasawa)
#define SCRIPT_PRINT( ... ) GFL_PRINT(__VA_ARGS__)
#else
#define SCRIPT_PRINT( ... ) /* do nothing */
#endif


GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )

#endif  // __SCRIPTYPE_H__



