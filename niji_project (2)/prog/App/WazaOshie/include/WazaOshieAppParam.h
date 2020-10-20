#ifndef NIJI_PROJECT_APP_WAZAOSHIE_APPPARAM_H_INCLUDED
#define NIJI_PROJECT_APP_WAZAOSHIE_APPPARAM_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file WazaOshieAppParam.h
 * @date 2015/07/01 13:33:02
 * @author fukushima_yuya
 * @brief 技教え・技忘れ外部設定パラメータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gfl2
#include <macro/include/gfl2_Macros.h>

// pml
#include <pml/include/pmlib.h>


//-----------------------------------------------------------------------------
/**
 *					前方宣言
 */
//-----------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class AppCommonGrpIconData;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class IntermediateData;
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( fs )
class AsyncFileManager;
GFL_NAMESPACE_END( fs )
GFL_NAMESPACE_BEGIN( Fade )
class FadeManager;
GFL_NAMESPACE_END( Fade )
GFL_NAMESPACE_END( gfl2 )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( WazaOshie )

enum {
  MODE_WAZA_OBOE = 0, //!< 技覚え
  MODE_WAZA_WASURE,   //!< 技忘れ
  MODE_WAZA_NUM,

  END_MODE_CANCEL = 0,    //!< キャンセル
  END_MODE_DECIDE,        //!< 技を決定
  END_MODE_NUM,
};

//! @brief    外部設定パラメータ
struct APP_PARAM {
  // [in]
  u8 mode;  //!< 画面モード  0:技教え 1:技忘れ
  u8 select_poke_index;   //!< ポケモンリストの外部設定パラメータの select_poke_index をそのまま入れる
  u8 upper_draw;          //!< 上画面の描画フラグ(0 = 非表示、1 = 表示)

  pml::PokeParty* partyData;   //!< 外からパーティを指定したい場合に使用：基本的には内部でパーティデータを設定する

  // [in/out]
  WazaNo waza_no;   //!< in:新しく覚える技No out:忘れる技No

  // [out]
  u8 end_mode;      //!< 終了モード  0:キャンセル 1:技を決定
  u8 select_index;  //!< 選択した項目のインデックス

  APP_PARAM( void )
    : mode( MODE_WAZA_OBOE )
    , select_poke_index( 0 )
    , upper_draw( false )
    , partyData( 0 )
    , waza_no( WAZANO_NULL )
    , end_mode( END_MODE_NUM )
    , select_index( 0 )
  { ; }
};


GFL_NAMESPACE_END( WazaOshie )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_WAZAOSHIE_APPPARAM_H_INCLUDED