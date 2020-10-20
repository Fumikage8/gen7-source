#ifndef NIJI_PROJECT_APP_STRINPUT_APP_PARAM_H_INCLUDED
#define NIJI_PROJECT_APP_STRINPUT_APP_PARAM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    StrInputAppParam.h
 * @brief   外部設定パラメータ
 * @author  fukushima_yuya
 * @date    2015.04.30
 */
//==============================================================================

// niji
#include <GameSys/include/GameProcManager.h>
#include <Savedata/include/SaveData.h>

// pml
#include <pml/include/pmlib.h>

// gflib
#include <macro/include/gfl2_Macros.h>
#include <str/include/gfl2_Str.h>


GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( StrInput )

//------------------------------------------------------------------------------
/**
 * @brief  外部設定パラメータ（初期化データ・終了時のデータなど）
 */
//------------------------------------------------------------------------------
typedef struct StrInputParam {
  // [ in ]
  GameSys::GameProcManager* mpGameProcManager;      //!< ゲームプロックマネージャ
  Savedata::Misc * pSaveMisc;     //!< セーブデータ(入力モードの取得で使用)
  u32  type;                      //!< 入力タイプ
  u32  param;                     //!< 入力タイプパラメータ（ポケモン番号など）
  pml::pokepara::CoreParam * cp;  //!< CoreParam ( type = TYPE_POKEMON_NAME のときのみ必要 )
  bool isFieldSubProc;            //!< フィールドのサブPROCとして動作させるならtrueを指定

  u8  language;                   //!< 言語ID

  // [ in / out ]
  gfl2::str::StrBuf * strBuf;     //!< 入出力バッファ いらない場合はNULL ※EOM分も確保すること
  gfl2::str::STRCODE * strCode;   //!< 入出力バッファ いらない場合はNULL ※EOM分も確保すること
  gfl2::str::StrBuf * oldNameBuf; //!< 姓名判断氏用(元のニックネームを設定) いらない場合はNULL ※EOM分も確保すること
  
  // [ out ]
  bool changeFlag;                //!< 文字列が変更されたか true = 変更
  bool isInputCancel;             //!< true = 文字を入力しないで「おわり」を押下した
  
  // [in]
  bool skipInitGraphicMemory;     //!< グラフィックシステムを初期化しないようにするフラグ

  // [in]
  app::util::Heap* mpHeap;
  app::util::AppRenderingManager* mpAppRenderingManager;

  StrInputParam( void )
    : mpGameProcManager( NULL )
    , pSaveMisc( NULL )
    , type( 0 )
    , param( 0 )
    , cp( NULL )
    , isFieldSubProc( false )
    , language( System::GetLang() )
    , strBuf( NULL )
    , strCode( NULL )
    , oldNameBuf(NULL)
    , changeFlag( false )
    , isInputCancel( false )
    , skipInitGraphicMemory( true )
    , mpHeap( NULL )
    , mpAppRenderingManager( NULL )
  {
    ;
  }
} APP_PARAM;

GFL_NAMESPACE_END( StrInput )
GFL_NAMESPACE_END( App )

#endif // #ifndef NIJI_PROJECT_APP_STRINPUT_APP_PARAM_H_INCLUDED
